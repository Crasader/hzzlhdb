#include <iostream>   
#include <map>
#include <thread>
#include <mutex>
#include "cocos2d.h"
#include "Socket.h"

#if (CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
	#include <WinSock2.h>  
	#pragma comment (lib,"WS2_32.Lib")  
	#define LHS_EAGAIN          WSAEWOULDBLOCK    
	#define LHS_EWOULDBLOCK     WSAEWOULDBLOCK  
#else
	#include <signal.h>  
	#include <sys/socket.h>  
	#include <netinet/in.h>  
	#include<netinet/tcp.h>  
	#include <netdb.h>  
	#include <errno.h>  
	#include <fcntl.h>  
	#include <unistd.h>  
	#include <sys/stat.h>  
	#include <sys/types.h>  
	#include <arpa/inet.h> 
	typedef int					SOCKET;  
	#define INVALID_SOCKET		(SOCKET)(~0)  
	#define SOCKET_ERROR		(-1)  
	#define LHS_EAGAIN          EAGAIN              //此操作会令线程阻塞,但socket为不可阻塞的  
	#define LHS_EWOULDBLOCK     EWOULDBLOCK         //资源暂时不可用  
#endif

USING_NS_CC;
using namespace std;   

#define SOCKET_PRINTF log

//////////////////////////////////////////////////////////////////////////
static bool					__isSetSig		= true; 
static int					__SocketIndex	= 1;
static map<int, CSocket*>	__SocketMap;

//////////////////////////////////////////////////////////////////////////
void socket_close(int s)
{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)  
	shutdown(s, SD_BOTH);
	closesocket(s);
#else  
	shutdown(s, 2);
	close(s);  
#endif 
}

int socket_send(int s, const char* data, int size)
{
	unsigned int flags = 0;
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)  
	flags=MSG_NOSIGNAL;  
#endif  
	return send(s, data, size, flags);
}

int socket_recv(int s, char* data, int size)
{
	unsigned int flags=0;  
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)  
	flags=MSG_NOSIGNAL;  
#endif  
	return recv(s, data, size, flags);
}

int socket_error()
{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
	return WSAGetLastError();
#else
	return errno;
#endif
}

void socket_sleep(unsigned int delay)
{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
	Sleep(delay);
#else
	usleep(delay*1000); 
#endif
}
//////////////////////////////////////////////////////////////////////////
class CInitSock
{
public:
	CInitSock()
	{
		#if (CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
			//WSADATA对象，用于存储Socket动态库的信息
			WSADATA wsaData;
			//调用Windows Sockets DLL
			if(WSAStartup(0x0110,&wsaData))
			{
				//初始化DLL错误，显示错误提示，程序退出
				SOCKET_PRINTF("Initialize socket failed.\n");
			}
		#endif

		#if (CC_TARGET_PLATFORM==CC_PLATFORM_IOS)  
			if (__isSetSig)  
			{  
				__isSetSig=false;  
				struct sigaction Sa;  
				Sa.sa_handler = SIG_IGN;  
				sigaction(SIGPIPE,&Sa,0);
			}  
		#endif 
	}

	~CInitSock()
	{
		#if (CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
			WSACleanup();
		#endif
	}
};

CInitSock __InitSock;

//////////////////////////////////////////////////////////////////////////
// 多线程共享消息队列

//////////////////////////////////////////////////////////////////////////
// 队列锁
class MTSocketQueueLocker  
{  
	std::mutex* _mutex;  

public:  
	MTSocketQueueLocker(std::mutex* aMutex) : _mutex(aMutex)  
	{  
		_mutex->lock();
	}  
	~MTSocketQueueLocker()
	{  
		_mutex->unlock();
	}  
}; // MTSocketQueueLocker

#define MTSocketQueueLock(mutex) MTSocketQueueLocker __locker__(mutex) 

//////////////////////////////////////////////////////////////////////////
// 循环消息处理
class MTSocketQueue
{
public:
	static MTSocketQueue* shared();
	static void purge();

public:
	void push(unsigned int onwer, int cmd, char* data, int dataSize);
	void post();

private:
	MTSocketQueue();
	~MTSocketQueue();

private:
	struct MTData
	{
		unsigned int	onwer;		//数据拥有者
		int				cmd;		//命令:0关闭连接 1:错误码(dataSize) 2:消息
		char*			data;
		int				dataSize;
	}; // MTData

	std::list<MTData>	mDataQueues;
	std::mutex			mDataMutex;

	static MTSocketQueue*	msInstance;
}; // MTSocketQueue

//////////////////////////////////////////////////////////////////////////
MTSocketQueue* MTSocketQueue::msInstance = 0;

MTSocketQueue* MTSocketQueue::shared()
{
	if (msInstance == 0)
		msInstance = new MTSocketQueue();

	return msInstance;
}

void MTSocketQueue::purge()
{
	if (msInstance)
		delete msInstance;
	msInstance = 0;
}

//////////////////////////////////////////////////////////////////////////

MTSocketQueue::MTSocketQueue()
{
}

MTSocketQueue::~MTSocketQueue()
{
	{
		MTSocketQueueLock(&mDataMutex);

		while (!mDataQueues.empty())
		{
			MTData& mtData = mDataQueues.front();
			if (mtData.data)
				delete []mtData.data;
			mDataQueues.pop_front();
		}
	}
}

void MTSocketQueue::push(unsigned int owner, int cmd, char* data, int dataSize)
{
	if (__SocketMap[owner] == 0)
		return;

	MTData mtData;
	memset(&mtData, 0, sizeof(mtData));
	mtData.onwer	= owner;
	mtData.cmd		= cmd;
	mtData.dataSize = dataSize;

	if (data != 0)
	{
		mtData.data = new char[dataSize];
		if (mtData.data == 0)
			return;
		memcpy(mtData.data, data, dataSize);
	}
	
	MTSocketQueueLock(&mDataMutex);
	mDataQueues.push_back(mtData);
}

void MTSocketQueue::post()
{
	MTSocketQueueLock(&mDataMutex);
	while (!mDataQueues.empty())
	{
		const MTData& mtData = mDataQueues.front();
		CSocket* owner = __SocketMap[mtData.onwer];
		if (owner)
		{
			switch (mtData.cmd)
			{
			case 0:
				owner->disconnect();
				break;
			case 1:
				owner->onSocketError(mtData.dataSize);
				break;
			case 2:
				owner->onSocketData(mtData.data, mtData.dataSize);
				break;
			}
			
		}

		if (mtData.data)
			delete []mtData.data;
		mDataQueues.pop_front();
	}
}

//////////////////////////////////////////////////////////////////////////

CSocket::CSocket()
	: mSocket(INVALID_SOCKET)
	, mSocketSink(0)
{
	mSocketIndex=-1;
}

CSocket::~CSocket()
{
	if (mSocketIndex >= 0)
		__SocketMap[mSocketIndex]=0;
	mSocketSink = 0;
	disconnect();
	
}

void CSocket::setSocketSink(ISocketSink* pISocketSink)
{
	mSocketSink = pISocketSink;
}

bool CSocket::isAlive() const
{
	return mSocket != INVALID_SOCKET;
}

int CSocket::send(const char* data, int size)
{
	if (!isAlive())
		return -1;

	return socket_send(mSocket, data, size);
}

int CSocket::connect(const char* url, int port)
{
	disconnect();

	hostent* hp = gethostbyname(url);

	if (!hp)
	{
		SOCKET_PRINTF("socket gethostbyname error!\n");
		onSocketError(socket_error());
		return -1;
	}

	//创建SOCKET
	//孟

#if (defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= 90000) || (defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && __MAC_OS_X_VERSION_MIN_REQUIRED >= 101100)
	SOCKET s = socket(AF_INET6,SOCK_STREAM,0);
#else
	SOCKET s = socket(AF_INET,SOCK_STREAM,0);
#endif


	//SOCKET s = socket(AF_INET,SOCK_STREAM,0);

	if (s == INVALID_SOCKET)
	{
		SOCKET_PRINTF("create socket failed\n");
		onSocketError(socket_error());
		return -1;
	}

	////设置非阻塞方式连接
	//unsigned long ul = 1;
	//int ret = ioctlsocket(s, FIONBIO, (unsigned long*)&ul);
	//if(ret==SOCKET_ERROR)
	//	return -1;

	sockaddr_in local;
	memset(&local, 0, sizeof(local));  
	//连接的服务器IP地址
	memcpy((char*)&local.sin_addr, hp->h_addr, hp->h_length);  
	//连接的协议类型
	local.sin_family=hp->h_addrtype;
	//连接的服务器端口
	local.sin_port=htons(port);
	SOCKET_PRINTF("socket connect...!!! ip=%s port=%d\n", url, port);

	//连接服务器
	if(::connect(s,(const sockaddr*)&local,sizeof(local))<0)
	{
		SOCKET_PRINTF("connect failed.\n");
		int errCode =socket_error();  
		socket_close(s);
		onSocketError(errCode);
		return -1;
	}

	////select 模型，即设置超时
	//struct timeval timeout ;
	//fd_set r;

	//FD_ZERO(&r);
	//FD_SET(s, &r);
	//timeout.tv_sec = 3; //连接超时15秒
	//timeout.tv_usec =0; 
	//ret = select(0, 0, &r, 0, &timeout);
	//if ( ret <= 0 )
	//{
	//	socket_close(s);
	//	return -1;
	//}
	////一般非锁定模式套接比较难控制，可以根据实际情况考虑 再设回阻塞模式
	//unsigned long ul1= 0 ;
	//ret = ioctlsocket(s, FIONBIO, (unsigned long*)&ul1);
	//if(ret==SOCKET_ERROR)
	//{
	//	socket_close(s);
	//	return -1;
	//}

	//SOCKET_PRINTF("connect ok.\n");
		
	if (mSocketIndex == -1)
	{
		mSocketIndex = __SocketIndex++;
		__SocketMap[mSocketIndex]=this;
	}

	mSocket	= (int)s;

	// 创建线程
	thread t(recv_thread, &mSocketIndex);
	t.detach();
	//SOCKET_PRINTF("connect 1.\n");
	
	if (false)
	{
		//SOCKET_PRINTF("connect 2.\n");
		socket_close(s);
		mSocket=INVALID_SOCKET;
		__SocketMap[mSocketIndex]=0;
		mSocketIndex=-1;
		onSocketError(10000 + 1);
		return -1;
	}

	//SOCKET_PRINTF("connect 3.\n");
	onSocketLink();
	//SOCKET_PRINTF("connect 4.\n");
	return 0;
}

void CSocket::disconnect()
{
	if (!isAlive())
		return;

	if (mSocketIndex > 0)
	{
		__SocketMap[mSocketIndex] = 0;
		mSocketIndex = -1;
	}

    socket_close(mSocket);
	mSocket = INVALID_SOCKET;	
	onSocketShut();
}

//////////////////////////////////////////////////////////////////////////
// 辅助函数
void CSocket::onSocketLink()
{
	if (mSocketSink)
		mSocketSink->onSocketLink();
}

void CSocket::onSocketShut()
{
	if (mSocketSink)
		mSocketSink->onSocketShut();
}

void CSocket::onSocketError(int errorCode)
{
	if (mSocketSink)
		mSocketSink->onSocketError(errorCode);
}

void CSocket::onSocketData(void* data, int dataSize)
{
	if (mSocketSink)
		mSocketSink->onSocketData(data, dataSize);
}

//////////////////////////////////////////////////////////////////////////
// 客户端处理线程
void CSocket::recv_thread(void* p)
{
	const int MAX_BUF = 16384;


	int socketIndex = *(int*)p;
	char dataBuffer[MAX_BUF]={0};

	while (__SocketMap[socketIndex] && __SocketMap[socketIndex]->isAlive())
	{
		int dataSize = socket_recv(__SocketMap[socketIndex]->mSocket, dataBuffer, MAX_BUF);

		if(dataSize==SOCKET_ERROR)  
		{  
			int errCode =socket_error();  

			if(errCode==LHS_EWOULDBLOCK)  
			{  
				socket_sleep(20);
				continue; 
			} 
			else if(errCode==LHS_EAGAIN)
			{
				continue;
			}
			

			SOCKET_PRINTF("recv error:%d\n", errCode);

			if (errCode == 10004) // 中断连接
				break;

			//if (__SocketMap[socketIndex])
			//	__SocketMap[socketIndex]->onSocketError(errCode);
			if (__SocketMap[socketIndex] && MTSocketQueue::shared())
				MTSocketQueue::shared()->push(socketIndex, 1, 0, errCode);
			break;
		}

		if (dataSize == 0)
		{
			break;
		}

		//if (__SocketMap[socketIndex])
		//	__SocketMap[socketIndex]->onSocketData(dataBuffer, dataSize);
		if (__SocketMap[socketIndex] && MTSocketQueue::shared())
			MTSocketQueue::shared()->push(socketIndex, 2, dataBuffer, dataSize);
	}

	//if (__SocketMap[socketIndex])
	//	__SocketMap[socketIndex]->disconnect();
	if (__SocketMap[socketIndex] && MTSocketQueue::shared())
		MTSocketQueue::shared()->push(socketIndex, 0, 0, 0);
}

// 分发数据队列
void CSocket::post()
{
	if (MTSocketQueue::shared())
		MTSocketQueue::shared()->post();
}

void CSocket::purge()
{
	MTSocketQueue::purge();
}
