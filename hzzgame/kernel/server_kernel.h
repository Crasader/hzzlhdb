
#ifndef SERVER_KERNEL_H_
#define SERVER_KERNEL_H_

#include <string>

#include "../base/basictypes.h"
#include "../../../../global/Define.h"
#include "../../../../global/Packet.h"
#include "../../../../global/struct_mobile.h"
#include "../../../../global/ServerRule.h"
#include "../../../../global/RightDefine.h"
#include "../../../../command/CMD_LogonServer.h"
#include "../../../../command/CMD_Commom.h"
#include "../base/convert.h"
#include "../network/packet_helper.h"
#include "game_user_manager.h"
#include "../network/socket_engine.h"
#include "game_table_proxy.h"

class ServerKernel : public SocketEngine::Delegate, public ClientUserManager::Delegate
{
public:
	static ServerKernel* create();
	static void destory();
	static ServerKernel* get();

	virtual ~ServerKernel();

	// ��������
	bool Connect(const tagGameKind* game_kind, const tagGameServer* game_server);
	// �ر�����
	bool Disconnect();
	// ��������
	bool Send(uint16 main, uint16 sub, void* data, uint16 size);

public:
	uint16 GetTableCount() const { return table_count_; }
	uint16 GetChairCount() const { return chair_count_; }
	GameTableProxy* GetGameTableProxy() { return game_table_proxy_; }
	// ִ���Թ�
	bool PerformLookonAction(uint16 table_id, uint16 chair_id);
	// ִ������
	bool PerformStandUpAction(uint16 table_id, uint16 chair_id);
	// ִ������
	bool PerformSitDownAction(uint16 table_id, uint16 chair_id, bool efficacy_pass);
	// ����
	bool KickUser(DWORD dwGameID);
	// ����
	bool KickAllUser();
	// ��ֹ����
	bool ProhibitedEntry();
	// ����λ��
	bool PerformChangeChair();
	bool IsRequestAction() { return (request_table_id_ != INVALID_TABLE || request_chair_id_ != INVALID_CHAIR); }
	IGameUserItem* GetMeUserItem() const { return myself_user_item_; }
	const tagGameKind* GetGameKind() const { return &game_kind_; }
	const tagGameServer* GetGameServer() const { return &game_server_; }
	const uint16 getServerType() const { return server_type_; }

	// �����Ϣ
public:
	// ��Ϸ��׼����
	void OnGFGameReady();
	// ��Ϸ�ر�
	void OnGFGameClose();
	// ��������
	bool GFSendData(uint16 main, uint16 sub, void* data, uint16 size);

private:
	ServerKernel();

	virtual void OnSocketConnect();
	virtual void OnSocketClose();
	virtual void OnSocketError(int errorCode);
	virtual bool OnSocketData(uint16 main, uint16 sub, void* data, uint16 dataSize);

	// �û��ӿ�
private:
	// �û�����
	virtual void OnUserItemAcitve(IGameUserItem* game_user_item);
	// �û�ɾ��
	virtual void OnUserItemDelete(IGameUserItem* game_user_item);
	// �û�����
	virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserScore& LastScore);
	// �û�����
	virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserStatus& LastStatus);

	// �����¼�
private:
	// ��¼����
	bool OnSocketMainLogon(uint16 sub_cmd_id, void* data, uint16 data_size);
	// ���ô���
	bool OnSocketMainConfig(uint16 sub_cmd_id, void* data, uint16 data_size);
	// �û�����
	bool OnSocketMainUser(uint16 sub_cmd_id, void* data, uint16 data_size);
	// ״̬����
	bool OnSocketMainStatus(uint16 sub_cmd_id, void* data, uint16 data_size);
	// ϵͳ��Ϣ
	bool OnSocketMainSystem(uint16 sub_cmd_id, void* data, uint16 data_size);
	// ��Ϸ����
	bool OnSocketMainGameFrame(uint16 main_cmd_id, uint16 sub_cmd_id, void* data, uint16 data_size);

	// ��½���
private:
	// ��¼�ɹ�
	bool OnSocketSubLogonSuccess(void* data, uint16 data_size);
	// ��¼ʧ��
	bool OnSocketSubLogonFailure(void* data, uint16 data_size);
	// ��¼���
	bool OnSocketSubLogonFinish(void* data, uint16 data_size);
	// ��Ϸ����
	bool OnSocketSubUpdateNotify(void* data, uint16 data_size);
	// ʧ��
	bool OnSocketSubFailure(void* data, uint16 data_size);

	// �û����
private:
	// �û�����
	bool OnSocketSubUserEnter(void* data, uint16 data_size);
	// �û�����
	bool OnSocketSubUserScore(void* data, uint16 data_size);
	// �û�״̬
	bool OnSocketSubUserStatus(void* data, uint16 data_size);
	// �û�����
	bool OnSocketSubUserChat(void* data, uint16 data_size);
	// ����ʧ��
	bool OnSocketSubRequestFailure(void* data, uint16 data_size);
	// ���·���
	bool OnSocketSubUpdateScore(void* data, uint16 data_size);

private:
	// ���͵�¼
	bool SendLoginPacket();
	// ���͹���
	bool SendUserRulePacket();
	// �����Թ�
	bool SendLookonPacket(uint16 table_id, uint16 chair_id);
	// ��������
	bool SendSitDownPacket(uint16 table_id, uint16 chair_id, const char* password = NULL);
	// ��������
	bool SendStandUpPacket(uint16 table_id, uint16 chair_id, uint8 force_leave);

private:
	// ����Ч��
	bool EfficacyTableRule(uint16 table_id, uint16 chair_id, bool req_lookon, std::string& desc);

private:
	GameTableProxy* game_table_proxy_;
	SocketEngine* socket_engine_;
	// ����״̬
	enum ServiceStatus
	{
		SS_UNKNOW = 0,			// δ֪״̬
		SS_ENTERING,			// ����״̬
		SS_VALIDATE,			// ��֤״̬
		SS_RECVINFO,			// ��ȡ״̬
		SS_SERVICEING,			// ����״̬
		SS_NETWORKCLOSE			// �ж�״̬
	};
	ServiceStatus service_status_;
	uint16 server_type_;	// ��������
	uint32 server_rule_;	// �������
	tagGameKind game_kind_;
	tagGameServer game_server_;

	uint32 user_right_;	// �û�Ȩ��
	uint32 master_right_;	// ����Ȩ��
	IGameUserItem* myself_user_item_;	// �Լ�

	uint16 table_count_;
	uint16 chair_count_;

	uint16 request_table_id_;
	uint16 request_chair_id_;
	uint16 find_table_id_;

	bool game_ready_;

	ClientUserManager* client_user_manager_;
};

#endif // SERVER_KERNEL_H_