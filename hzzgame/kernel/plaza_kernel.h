
#ifndef PLAZA_KERNEL_H_
#define PLAZA_KERNEL_H_

#include <string>
#include <vector>
#include <map>

#include "../base/basictypes.h"
#include "../../../../global/Define.h"
#include "../../../../global/Packet.h"
#include "../../../../global/struct_mobile.h"
#include "../../../../command/CMD_LogonServer.h"
#include "../../../../command/CMD_Commom.h"
#include "../base/convert.h"
#include "../network/socket_engine.h"
#include "../network/packet_helper.h"

typedef std::vector<tagGameKind*> GameKindVector;
typedef std::vector<tagGameServer*> GameServerVector;
typedef std::map<uint16, GameServerVector> GameServerMap;

class PlazaKernel : public SocketEngine::Delegate
{
public:
	class BankDelegate
	{
	public:
		BankDelegate() { }
		virtual ~BankDelegate() { }

		virtual bool OnSocketBankService(uint16 sub_cmd_id, void* data, uint16 data_size) = 0;
	};
	class RankingDelegate
	{
	public:
		RankingDelegate(){}
		virtual ~RankingDelegate() {}
		virtual bool OnSocketRankingResult(uint16 sub_cmd_id, void* data, uint16 data_size) = 0;
	};

	static PlazaKernel* getInstance();
	static void destroyInstance();

	virtual ~PlazaKernel();

	// ��������
	bool Connect(const char* url, int port);
	// �ر�����
	bool Disconnect();
	// ��������
	bool Send(uint16 main, uint16 sub, void* data, uint16 size);

	const GameKindVector& game_kind_list() const { return game_kind_list_; }
	const GameServerVector& GetGameServerList(uint16 kindid);
	const tagGameKind* GetGameKind(uint16 kindid);

	void set_bank_delegate(BankDelegate* delegate) { bank_delegate_ = delegate; }
	void set_ranking_delegate(RankingDelegate* delegate) { ranking_delegate_ = delegate; }

private:
	PlazaKernel();

	virtual void OnSocketConnect();
	virtual void OnSocketClose();
	virtual void OnSocketError(int errorCode);
	virtual bool OnSocketData(uint16 main, uint16 sub, void* data, uint16 dataSize);

	// ��½���
private:
	// ��¼�ɹ�
	bool OnSocketSubLogonSuccess(void* data, uint16 data_size);
	// ��¼ʧ��
	bool OnSocketSubLogonFailure(void* data, uint16 data_size);
	// ��¼���
	bool OnSocketSubLogonFinish(void* data, uint16 data_size);

	// �б����
private:
	// �б�����
	bool OnSocketSubListType(void* data, uint16 data_size);
	// �б�����
	bool OnSocketSubListKind(void* data, uint16 data_size);
	// �б���
	bool OnSocketSubListServer(void* data, uint16 data_size);
	// �б����
	bool OnSocketSubListFinish(void* data, uint16 data_size);
	// ��������
	bool OnSocketSubOnlineCount(void* data, uint16 data_size);

	// �û�����
private:
	// ���½��
	bool OnSocketSubUserInfoResult(void* data, uint16 data_size);
	// �û�ʧ��
	bool OnSocketSubUserFailure(void* data, uint16 data_size);

	// ϵͳ��Ϣ���
private:
	// �������Ϣ
	bool OnSocketSubMarqueeMessage(void* data, uint16 data_size);

private:
	SocketEngine* socket_engine_;
	GameKindVector game_kind_list_;	// ��Ϸ�����б�
	GameServerMap game_server_list_;	// ��Ϸ�����б�
	BankDelegate* bank_delegate_;
	RankingDelegate* ranking_delegate_;
};

#endif // PLAZA_KERNEL_H_