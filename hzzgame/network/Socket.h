
#ifndef _Socket_H_
#define _Socket_H_
#include <list>

struct ISocketSink
{
	virtual ~ISocketSink(){}
	virtual void onSocketLink() = 0;
	virtual void onSocketShut() = 0;
	virtual void onSocketError(int errorCode) = 0;
	virtual void onSocketData(void* data, int dataSize) = 0;
	
};

class CSocket
{
public:
	CSocket();
	~CSocket();
	void setSocketSink(ISocketSink* pISocketSink);
	bool isAlive() const;
	int  connect(const char* url, int port);
	void disconnect();
	int send(const char* data, int size);
	//////////////////////////////////////////////////////////////////////////
	// 辅助函数
private:
	void onSocketLink();
	void onSocketShut();
	void onSocketError(int errorCode);
	void onSocketData(void* data, int dataSize);

	//////////////////////////////////////////////////////////////////////////
	// 处理线程
private:
	// 接收数据线程
	static void recv_thread(void* p);
public:
	// 分发数据队列
	static void post();
	static void purge();

private:
	int				mSocket;
	ISocketSink*	mSocketSink;
	int				mSocketIndex;

	friend class MTSocketQueue;
};

#endif // _Socket_H_