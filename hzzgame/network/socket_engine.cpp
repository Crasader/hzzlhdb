
#include "socket_engine.h"

#include "cocos2d.h"
#include "QPCipher.h"

SocketEngine::SocketEngine(Delegate* delegate)
	: delegate_(delegate), receive_buffer_length_(0)
{
	socket_.setSocketSink(this);
}

SocketEngine::~SocketEngine()
{
	socket_.setSocketSink(nullptr);
	Disconnect();
}

// 连接网络
bool SocketEngine::Connect(const char* url, uint16 port)
{
	if (socket_.connect(url, port) != 0)
		return false;

	// 接收长度
	receive_buffer_length_ = 0;
	return IsAlive();
}

// 关闭连接
bool SocketEngine::Disconnect()
{
	socket_.disconnect();
	return true;
}

// 发送数据
bool SocketEngine::Send(uint16 main, uint16 sub, void* data, uint16 dataSize)
{
	if (!IsAlive())
		return false;

	// 打包数据
	const uint8* datasend = Pack(main, sub, (uint8*)data, dataSize);
	socket_.send((const char*)datasend, sizeof(TCP_Head) + dataSize);
	return true;
}

// 状态判断
bool SocketEngine::IsAlive()
{
	return socket_.isAlive();
}

void SocketEngine::onSocketLink()
{
	delegate_->OnSocketConnect();
}

void SocketEngine::onSocketShut()
{
	delegate_->OnSocketClose();
}

void SocketEngine::onSocketError(int errorCode)
{
	delegate_->OnSocketError(errorCode);
}

void SocketEngine::onSocketData(void* data, int dataSize)
{
	memcpy(&receive_buffer[receive_buffer_length_], data, dataSize);
	receive_buffer_length_ += dataSize;

	// 尝试解包
	int unpack_index = 0;
	bool is_read = false;
	int dest_len = sizeof(TCP_Info);

	// 接收长度 - 已经解包的数据长度 
	while ((receive_buffer_length_ - unpack_index) >= dest_len)
	{
		if (!is_read)
		{
			is_read = true;
			dest_len = QPCipher::GetPackSize((unsigned char*)receive_buffer, unpack_index);

			if (dest_len > receive_buffer_length_)
			{
				// 错误数据包长度
				// Disconnect();
				break;
			}
		}
		else
		{
			// 解包数据并通知调用
			if (!Unpack(receive_buffer, unpack_index, dest_len))
			{
				Disconnect();
				return;
			}
			// 解包数据游标移动
			unpack_index += dest_len;
			// 重置接收长度为包信息头长度
			dest_len = sizeof(TCP_Info);
			// 重置接收为未读取包信息头状态
			is_read = false;
		}
	}

	// 剩余数据长度
	receive_buffer_length_ = receive_buffer_length_ - unpack_index;
	// 移动剩余数据
	if (receive_buffer_length_ > 0)
	{
		memmove(receive_buffer, &receive_buffer[unpack_index], receive_buffer_length_);
	}
}

// 封包
const uint8* SocketEngine::Pack(uint16 main, uint16 sub, uint8* data, uint16 size)
{
	pack_buffer_[0] = 0;
	int packsize = sizeof(TCP_Head) + size;
	QPCipher::SetPackInfo(pack_buffer_, packsize, main, sub);

	// 赋值
	if (size > 0)
	{
		memcpy(&pack_buffer_[sizeof(TCP_Head)], data, size);
	}
	// 加密数据
	QPCipher::EncryptBuffer(pack_buffer_, packsize);
	return pack_buffer_;
}

// 解包
bool SocketEngine::Unpack(uint8* data, int start, int length)
{
	// 解密
	if ((data[start] & QPCipher::GetCipherMode()) > 0)
	{
		QPCipher::DecryptBuffer(data, start, length);
	}

	// 主命令码
	int main = QPCipher::GetMainCommand(data, start);
	// 次命令码
	int sub = QPCipher::GetSubConmmand(data, start);

	// 附加数据
	if (length > sizeof(TCP_Head))
	{
		memcpy(unpack_buffer_, &data[start + sizeof(TCP_Head)], length - sizeof(TCP_Head));
	}

	length -= sizeof(TCP_Head);

	if (main == MDM_KN_COMMAND && sub == SUB_KN_DETECT_SOCKET)
	{
		// 心跳包
	}
	else
	{
		//return delegate_->OnSocketData((uint16)main, (uint16)sub, unpack_buffer_, (uint16)length);
		bool result = delegate_->OnSocketData((uint16)main, (uint16)sub, unpack_buffer_, (uint16)length);
		if (!result)
		{
			CCLOG("main:%d, sub:%d", main, sub);
		}
	}

	return true;
}
