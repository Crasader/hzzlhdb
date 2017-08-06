
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

// ��������
bool SocketEngine::Connect(const char* url, uint16 port)
{
	if (socket_.connect(url, port) != 0)
		return false;

	// ���ճ���
	receive_buffer_length_ = 0;
	return IsAlive();
}

// �ر�����
bool SocketEngine::Disconnect()
{
	socket_.disconnect();
	return true;
}

// ��������
bool SocketEngine::Send(uint16 main, uint16 sub, void* data, uint16 dataSize)
{
	if (!IsAlive())
		return false;

	// �������
	const uint8* datasend = Pack(main, sub, (uint8*)data, dataSize);
	socket_.send((const char*)datasend, sizeof(TCP_Head) + dataSize);
	return true;
}

// ״̬�ж�
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

	// ���Խ��
	int unpack_index = 0;
	bool is_read = false;
	int dest_len = sizeof(TCP_Info);

	// ���ճ��� - �Ѿ���������ݳ��� 
	while ((receive_buffer_length_ - unpack_index) >= dest_len)
	{
		if (!is_read)
		{
			is_read = true;
			dest_len = QPCipher::GetPackSize((unsigned char*)receive_buffer, unpack_index);

			if (dest_len > receive_buffer_length_)
			{
				// �������ݰ�����
				// Disconnect();
				break;
			}
		}
		else
		{
			// ������ݲ�֪ͨ����
			if (!Unpack(receive_buffer, unpack_index, dest_len))
			{
				Disconnect();
				return;
			}
			// ��������α��ƶ�
			unpack_index += dest_len;
			// ���ý��ճ���Ϊ����Ϣͷ����
			dest_len = sizeof(TCP_Info);
			// ���ý���Ϊδ��ȡ����Ϣͷ״̬
			is_read = false;
		}
	}

	// ʣ�����ݳ���
	receive_buffer_length_ = receive_buffer_length_ - unpack_index;
	// �ƶ�ʣ������
	if (receive_buffer_length_ > 0)
	{
		memmove(receive_buffer, &receive_buffer[unpack_index], receive_buffer_length_);
	}
}

// ���
const uint8* SocketEngine::Pack(uint16 main, uint16 sub, uint8* data, uint16 size)
{
	pack_buffer_[0] = 0;
	int packsize = sizeof(TCP_Head) + size;
	QPCipher::SetPackInfo(pack_buffer_, packsize, main, sub);

	// ��ֵ
	if (size > 0)
	{
		memcpy(&pack_buffer_[sizeof(TCP_Head)], data, size);
	}
	// ��������
	QPCipher::EncryptBuffer(pack_buffer_, packsize);
	return pack_buffer_;
}

// ���
bool SocketEngine::Unpack(uint8* data, int start, int length)
{
	// ����
	if ((data[start] & QPCipher::GetCipherMode()) > 0)
	{
		QPCipher::DecryptBuffer(data, start, length);
	}

	// ��������
	int main = QPCipher::GetMainCommand(data, start);
	// ��������
	int sub = QPCipher::GetSubConmmand(data, start);

	// ��������
	if (length > sizeof(TCP_Head))
	{
		memcpy(unpack_buffer_, &data[start + sizeof(TCP_Head)], length - sizeof(TCP_Head));
	}

	length -= sizeof(TCP_Head);

	if (main == MDM_KN_COMMAND && sub == SUB_KN_DETECT_SOCKET)
	{
		// ������
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
