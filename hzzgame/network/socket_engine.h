

#ifndef SOCKET_ENGINE_H_
#define SOCKET_ENGINE_H_

#include "../base/basictypes.h"

#include "../../../../global/Define.h"
#include "../../../../global/Packet.h"

#include "Socket.h"

class SocketEngine : public ISocketSink
{
public:
	class Delegate
	{
	public:
		Delegate() { }
		virtual ~Delegate() { }
		virtual void OnSocketConnect() = 0;
		virtual void OnSocketClose() = 0;
		virtual void OnSocketError(int errorCode) = 0;
		virtual bool OnSocketData(uint16 main, uint16 sub, void* data, uint16 dataSize) = 0;
	};

	explicit SocketEngine(Delegate* delegate);
	virtual ~SocketEngine();

	// ��������
	bool Connect(const char* url, uint16 port);
	// �ر�����
	bool Disconnect();
	// ��������
	bool Send(uint16 main, uint16 sub, void* data, uint16 dataSize);
	// ״̬�ж�
	bool IsAlive();

private:
	virtual void onSocketLink();
	virtual void onSocketShut();
	virtual void onSocketError(int errorCode);
	virtual void onSocketData(void* data, int dataSize);

private:
	// ���
	const uint8* Pack(uint16 main, uint16 sub, uint8* data, uint16 size);
	// ���
	bool Unpack(uint8* data, int start, int length);

private:
	Delegate* delegate_;
	CSocket socket_;
	uint8 pack_buffer_[SOCKET_TCP_BUFFER];	// ��ʱ����
	uint8 unpack_buffer_[SOCKET_TCP_BUFFER];
	uint8 receive_buffer[SOCKET_TCP_BUFFER];	// ���ջ���
	int receive_buffer_length_;				// ���ճ���
};

#endif	// SOCKET_ENGINE_H_
