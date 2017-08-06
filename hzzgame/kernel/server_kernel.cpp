
#include "server_kernel.h"

#include "cocos2d.h"
#include "../scene/dialog_msg_box.h"
#include "../scene/lobby_scene.h"
#include "plaza_kernel.h"
#include "game_kernel.h"
#include "../data/global_user_info.h"

static ServerKernel *g_ServerKernel = nullptr;
static int g_ServerKernelRefCount = 0;

ServerKernel* ServerKernel::create()
{
	if (g_ServerKernelRefCount == 0)
	{
		g_ServerKernel = new ServerKernel();
	}

	g_ServerKernelRefCount++;

	return g_ServerKernel;
}

void ServerKernel::destory()
{
	if (g_ServerKernelRefCount > 0)
	{
		g_ServerKernelRefCount--;
		if (g_ServerKernelRefCount <= 0)
		{
			g_ServerKernelRefCount = 0;
			delete g_ServerKernel;
			g_ServerKernel = nullptr;
		}
	}
}

ServerKernel* ServerKernel::get()
{
	return g_ServerKernel;
}

ServerKernel::ServerKernel()
	: game_table_proxy_(new GameTableProxy()), service_status_(SS_UNKNOW), server_type_(0), server_rule_(0), user_right_(0), master_right_(0), myself_user_item_(nullptr),
	table_count_(0), chair_count_(0), request_table_id_(kuint16max), request_chair_id_(kuint16max), find_table_id_(kuint16max), game_ready_(false)
{
	socket_engine_ = new SocketEngine(this);
	client_user_manager_ = new ClientUserManager(this);
	memset(&game_kind_, 0, sizeof(game_kind_));
	memset(&game_server_, 0, sizeof(game_server_));
}

ServerKernel::~ServerKernel()
{
	Disconnect();
	delete client_user_manager_;
	delete socket_engine_;
	delete game_table_proxy_;
}

// ��������
bool ServerKernel::Connect(const tagGameKind* game_kind, const tagGameServer* game_server)
{
	if (service_status_ != SS_UNKNOW && service_status_ != SS_NETWORKCLOSE)
	{
		MsgBox(a2u("���Ѿ�����Ϸ�����ˣ������˳�֮ǰ�ķ���"));
		return false;
	}

	// �ر��ж�
	assert(game_server->wServerID != 0);
	if (game_server->wServerID == 0)
	{
		service_status_ = SS_UNKNOW;
		MsgBox(a2u("�ܱ�Ǹ������Ϸ�����Ѿ��ر��ˣ��������������!"));
		return false;
	}

	CCLOG("%s, %d", game_server->szServerAddr, game_server->wServerPort);
	if (!socket_engine_->Connect(game_server->szServerAddr, game_server->wServerPort))
	{
		MsgBox(a2u("��Ϸ��������ʧ�ܣ�����ϵͳ�������ò����Ƿ���ȷ!"));
		return false;
	}

	memcpy(&game_kind_, game_kind, sizeof(game_kind_));
	memcpy(&game_server_, game_server, sizeof(game_server_));
	service_status_ = SS_ENTERING;
	return true;
}

// �ر�����
bool ServerKernel::Disconnect()
{
	if (service_status_ == SS_UNKNOW || service_status_ == SS_NETWORKCLOSE)
	{
		return false;
	}

	if (service_status_ == SS_SERVICEING)
	{
		if (GameKernel::get())
		{
			GameKernel::get()->Intermit();
		}
	}

	socket_engine_->Disconnect();

	service_status_ = SS_NETWORKCLOSE;
	server_type_ = 0;
	server_rule_ = 0;
	memset(&game_kind_, 0, sizeof(game_kind_));
	memset(&game_server_, 0, sizeof(game_server_));

	user_right_ = 0;
	master_right_ = 0;
	myself_user_item_ = 0;

	table_count_ = 0;
	chair_count_ = 0;

	game_ready_ = false;

	client_user_manager_->ResetUserManager();

	return true;
}

// ��������
bool ServerKernel::Send(uint16 main, uint16 sub, void* data, uint16 size)
{
	return socket_engine_->Send(main, sub, data, size);
}

bool ServerKernel::PerformLookonAction(uint16 table_id, uint16 chair_id)
{
	// Ч������
	assert(table_id != INVALID_TABLE);
	assert(chair_id != INVALID_CHAIR);

	// ״̬����
	if (service_status_ != SS_SERVICEING)
		return false;
	if (request_table_id_ == table_id && request_chair_id_ == chair_id)
		return false;

	// �Լ�״̬
	if (myself_user_item_->GetUserStatus() >= US_PLAYING)
	{
		MsgBox(a2u("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"));

		return false;
	}

	// Ȩ���ж�
	if (!CUserRight::CanLookon(user_right_))
	{
		MsgBox(a2u("��Ǹ������ʱû���Թ���Ϸ��Ȩ�ޣ�"));

		return false;
	}

	// �������
	if (request_table_id_ != INVALID_TABLE && request_chair_id_ != INVALID_CHAIR && request_chair_id_ != MAX_CHAIR)
	{
		IGameUserItem* user_item = game_table_proxy_->GetTableUserItem(request_table_id_, request_chair_id_);
		if (user_item == myself_user_item_)
			game_table_proxy_->SetTableUserItem(request_table_id_, request_chair_id_, NULL);
	}

	request_table_id_ = table_id;
	request_chair_id_ = chair_id;
	find_table_id_ = INVALID_TABLE;

	SendLookonPacket(table_id, chair_id);

	return true;
}

bool ServerKernel::PerformStandUpAction(uint16 table_id, uint16 chair_id)
{
	// Ч������
	assert(table_id != INVALID_TABLE);
	assert(chair_id != INVALID_CHAIR);

	// ״̬����
	if (service_status_ != SS_SERVICEING)
		return false;
	if (request_table_id_ == table_id && request_chair_id_ == chair_id)
		return false;

	// �Լ�״̬
	if (myself_user_item_->GetUserStatus() >= US_PLAYING)
	{
		MsgBox(a2u("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"));

		return false;
	}

	request_table_id_ = INVALID_TABLE;
	request_chair_id_ = INVALID_CHAIR;
	find_table_id_ = INVALID_TABLE;

	SendStandUpPacket(table_id, chair_id, 0);

	return true;
}

bool ServerKernel::PerformSitDownAction(uint16 table_id, uint16 chair_id, bool efficacy_pass)
{
	// ״̬����
	if (service_status_ != SS_SERVICEING)
		return false;
	if (request_table_id_ != INVALID_TABLE && request_table_id_ == table_id)
		return false;
	if (request_chair_id_ != INVALID_CHAIR && request_chair_id_ == chair_id)
		return false;

	// �Լ�״̬
	if (myself_user_item_->GetUserStatus() >= US_PLAYING)
	{
		MsgBox(a2u("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"));

		return false;
	}

	// Ȩ���ж�
	if (!CUserRight::CanPlay(user_right_))
	{
		MsgBox(a2u("��Ǹ������ʱû�м�����Ϸ��Ȩ�ޣ�"));

		return false;
	}

	// ����Ч��
	if ((table_id != INVALID_TABLE) && (chair_id != INVALID_CHAIR))
	{
		std::string desc;
		if (chair_id != MAX_CHAIR && !EfficacyTableRule(table_id, chair_id, false, desc))
		{
			if (!desc.size())
				MsgBox(desc);

			return false;
		}
	}

	// �����ж�
	char password[LEN_PASSWORD] = { 0 };
	//if (myself_user_item_->GetMasterOrder() == 0 && efficacy_pass && table_id != INVALID_TABLE && chair_id != INVALID_CHAIR && chair_id != MAX_CHAIR)
	//{
	//	//��������
	//	bool bLocker=m_TableViewFrame.GetLockerFlag(wTableID);

	//	//�����ж�
	//	if(bLocker)
	//	{
	//		//��������
	//		CDlgTablePassword DlgTablePassword;
	//		DlgTablePassword.SetPromptString(TEXT("���������ý������룬���������롣"));
	//		if (DlgTablePassword.DoModal()!=IDOK) return false;

	//		//��������
	//		lstrcpyn(szPassword,DlgTablePassword.m_szPassword,CountArray(szPassword));
	//	}
	//}

	// �������
	if (request_table_id_ != INVALID_TABLE && request_chair_id_ != INVALID_CHAIR && request_chair_id_ != MAX_CHAIR)
	{
		IGameUserItem* user_item = game_table_proxy_->GetTableUserItem(request_table_id_, request_chair_id_);
		if (user_item == myself_user_item_)
			game_table_proxy_->SetTableUserItem(request_table_id_, request_chair_id_, NULL);
	}

	// ���ý���
	if (chair_id != MAX_CHAIR && table_id != INVALID_TABLE && chair_id != INVALID_CHAIR)
	{
		game_table_proxy_->SetTableUserItem(table_id, chair_id, myself_user_item_);
	}

	request_table_id_ = table_id;
	request_chair_id_ = chair_id;
	find_table_id_ = INVALID_TABLE;

	SendSitDownPacket(table_id, chair_id, password);

	return true;
}

// ����
bool ServerKernel::KickUser(DWORD dwGameID)
{
	//��������
	if (myself_user_item_->GetMasterOrder() != 0)
	{
		PACKET_HELPER_SIZE(128);
		packet.write4Byte(dwGameID);

		Send(MDM_GR_MANAGE, SUB_GR_KILL_USER, packet.getBuffer(), packet.getPosition());
	}

	return true;
}

// ����
bool ServerKernel::KickAllUser()
{
	//��������
	if (myself_user_item_->GetMasterOrder() != 0)
	{
		PACKET_HELPER_SIZE(128);
		packet.writeString(a2u("kick_all_��"), 64);

		Send(MDM_GR_MANAGE, SUB_GR_KICK_ALL_USER, packet.getBuffer(), packet.getPosition());
	}

	return true;
}

// ��ֹ����
bool ServerKernel::ProhibitedEntry()
{
	//��������
	if (myself_user_item_->GetMasterOrder() != 0)
	{
		PACKET_HELPER_SIZE(128);
		packet.write2Byte(0);
		packet.write2Byte(0);
		packet.write8Byte(0);
		packet.write8Byte(0);
		packet.write8Byte(0);
		packet.writeByte(0);
		packet.writeByte(0);
		packet.write4Byte(SR_FORFEND_ROOM_ENTER);
		packet.writeString(a2u(""), LEN_SERVER);

		Send(MDM_GR_MANAGE, SUB_GR_OPTION_SERVER, packet.getBuffer(), packet.getPosition());
	}
	return true;
}

// ����λ��
bool ServerKernel::PerformChangeChair()
{
	return Send(MDM_GR_USER, SUB_GR_USER_CHAIR_REQ, nullptr, 0);
}

// ��Ϸ��׼����
void ServerKernel::OnGFGameReady()
{
	uint16 table_id = myself_user_item_->GetTableID();
	uint16 chair_id = myself_user_item_->GetChairID();

	// ������Ϣ
	UserAttribute user_attrib;
	user_attrib.user_id = myself_user_item_->GetUserID();
	user_attrib.table_id = table_id;
	user_attrib.chair_id = chair_id;
	user_attrib.user_right = user_right_;
	user_attrib.master_right = master_right_;

	ServerAttribute server_attrib;
	server_attrib.kind_id = game_kind_.wKindID;
	server_attrib.server_id = game_server_.wServerID;
	server_attrib.server_type = server_type_;
	server_attrib.server_rule = server_rule_;
	strncpy(server_attrib.server_name, game_server_.szServerName, arraysize(server_attrib.server_name));
	GameKernel::get()->OnGFConfigServer(user_attrib, server_attrib);

	// �����û�
	if (table_id != INVALID_TABLE && chair_id != INVALID_CHAIR)
	{
		const GameUserItemVector& user_vector = client_user_manager_->GetUserVector();
		for (GameUserItemVector::const_iterator it = user_vector.begin(); it != user_vector.end(); ++it)
		{
			IGameUserItem* user_item = *it;
			if (user_item->GetTableID() != table_id)
				continue;

			GameKernel::get()->OnGFUserEnter(user_item);
		}
	}

	// �������
	GameKernel::get()->OnGFConfigFinish();

	game_ready_ = true;
}

// ��Ϸ�ر�
void ServerKernel::OnGFGameClose()
{
	// Ч��״̬
	assert(myself_user_item_ != nullptr);
	if (myself_user_item_ == nullptr)
		return;

	game_ready_ = false;

	uint16 table_id = myself_user_item_->GetTableID();
	uint16 chair_id = myself_user_item_->GetChairID();

	// �˳���Ϸ
	if ((table_id != INVALID_TABLE) && (service_status_ == SS_SERVICEING))
	{
		SendStandUpPacket(table_id, chair_id, 1);
	}
	if (ServerKernel::get())
	{
		ServerKernel::destory();
	}
}

// ��������
bool ServerKernel::GFSendData(uint16 main, uint16 sub, void* data, uint16 size)
{
	return socket_engine_->Send(main, sub, data, size);
}

void ServerKernel::OnSocketConnect()
{
	CCLOG("OnSocketConnect");
	SendLoginPacket();
}

void ServerKernel::OnSocketClose()
{
	CCLOG("OnSocketClose");
	if (service_status_ != SS_NETWORKCLOSE)
	{
		Disconnect();
		// TODO:���ص������б����
	}
}

void ServerKernel::OnSocketError(int errorCode)
{
	switch (errorCode)
	{
	case 10054:
	{
		MsgBox(a2u("�����쳣, ���Ͽ�����!!!"));
	}
	break;
	}
}

bool ServerKernel::OnSocketData(uint16 main, uint16 sub, void* data, uint16 data_size)
{
	switch (main)
	{
	case MDM_GR_LOGON:		// ��¼��Ϣ
	{
		return OnSocketMainLogon(sub, data, data_size);
	}
	case MDM_GR_CONFIG:		// ������Ϣ
	{
		return OnSocketMainConfig(sub, data, data_size);
	}
	case MDM_GR_USER:		// �û���Ϣ
	{
		return OnSocketMainUser(sub, data, data_size);
	}
	case MDM_GR_STATUS:		// ״̬��Ϣ
	{
		return OnSocketMainStatus(sub, data, data_size);
	}
	//case MDM_GR_MANAGE:		//������Ϣ
	//	{
	//		br=OnSocketMainManager(sub,pData,wDataSize);

	//		return true;
	//	}
	case MDM_CM_SYSTEM:		// ϵͳ��Ϣ
	{
		return OnSocketMainSystem(sub, data, data_size);
	}
	case MDM_GF_GAME:		// ��Ϸ��Ϣ
	case MDM_GF_FRAME:		// �����Ϣ
	{
		return OnSocketMainGameFrame(main, sub, data, data_size);
	}
	}

	return true;
}

void ServerKernel::OnUserItemAcitve(IGameUserItem* game_user_item)
{
	// �ж��Լ�
	if (myself_user_item_ == NULL)
	{
		myself_user_item_ = game_user_item;
	}

	// ��������
	uint8 user_status = game_user_item->GetUserStatus();
	if ((user_status >= US_SIT) && (user_status != US_LOOKON))
	{
		game_table_proxy_->SetTableUserItem(game_user_item->GetTableID(), game_user_item->GetChairID(), game_user_item);
	}
}

void ServerKernel::OnUserItemDelete(IGameUserItem* game_user_item)
{
	uint16 last_table_id = game_user_item->GetTableID();
	uint16 last_chair_id = game_user_item->GetChairID();
	uint32 leave_user_id = game_user_item->GetUserID();
	tagUserInfo* me_user_info = myself_user_item_->GetUserInfo();

	// �뿪����
	if ((last_table_id != INVALID_TABLE) && (last_chair_id != INVALID_CHAIR))
	{
		// ��ȡ�û�
		IGameUserItem* table_user_item = game_table_proxy_->GetTableUserItem(last_table_id, last_chair_id);
		if (table_user_item == game_user_item) game_table_proxy_->SetTableUserItem(last_table_id, last_chair_id, NULL);

		// �뿪֪ͨ
		if ((game_user_item == myself_user_item_) || (last_table_id == me_user_info->wTableID))
		{
			tagUserStatus user_status;
			user_status.cbUserStatus = US_NULL;
			user_status.wTableID = INVALID_TABLE;
			user_status.wChairID = INVALID_CHAIR;
			if (GameKernel::get())
				GameKernel::get()->OnGFUserStatus(game_user_item->GetUserID(), user_status);
		}
	}
}

void ServerKernel::OnUserItemUpdate(IGameUserItem* game_user_item, tagUserScore& LastScore)
{
	tagUserInfo* user_info = game_user_item->GetUserInfo();
	tagUserInfo* me_user_info = myself_user_item_->GetUserInfo();

	if ((me_user_info->wTableID != INVALID_TABLE) && (user_info->wTableID == me_user_info->wTableID))
	{
		tagUserScore UserScore;
		memset(&UserScore, 0, sizeof(UserScore));

		UserScore.lScore = user_info->lScore;
		UserScore.dwUserMedal = user_info->dwUserMedal;
		UserScore.dwUserLottery = user_info->dwUserLottery;
		UserScore.lInsure = user_info->lInsure;
		UserScore.dwWinCount = user_info->dwWinCount;
		UserScore.dwLostCount = user_info->dwLostCount;
		UserScore.dwDrawCount = user_info->dwDrawCount;
		UserScore.dwFleeCount = user_info->dwFleeCount;
		UserScore.dwExperience = user_info->dwExperience;

		if (GameKernel::get())
			GameKernel::get()->OnGFUserScore(game_user_item->GetUserID(), UserScore);
	}
	
	if ((user_info->dwUserID == me_user_info->dwUserID) && (!GameKernel::get()))
	{
		GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
		user_data->user_score = user_info->lScore;
		user_data->user_Medal = user_info->dwUserMedal;
		user_data->user_Lottery = user_info->dwUserLottery;
		LobbyScene::getInstance()->UpdateUserScore();
	}
}

void ServerKernel::OnUserItemUpdate(IGameUserItem* game_user_item, tagUserStatus& LastStatus)
{
	tagUserInfo* user_info = game_user_item->GetUserInfo();
	tagUserInfo* me_user_info = myself_user_item_->GetUserInfo();
	uint16 now_table_id = game_user_item->GetTableID();
	uint16 last_table_id = LastStatus.wTableID;
	uint16 now_chair_id = game_user_item->GetChairID();
	uint16 last_chair_id = LastStatus.wChairID;
	uint8 now_status = game_user_item->GetUserStatus();
	uint8 last_status = LastStatus.cbUserStatus;

	// �����뿪
	if (last_table_id != INVALID_TABLE && (last_table_id != now_table_id || last_chair_id != now_chair_id))
	{
		IGameUserItem* table_user_item = game_table_proxy_->GetTableUserItem(last_table_id, last_chair_id);
		if (table_user_item == game_user_item)
		{
			game_table_proxy_->SetTableUserItem(last_table_id, last_chair_id, NULL);
		}
	}

	// ���Ӽ���
	if (now_table_id != INVALID_TABLE && now_status != US_LOOKON && (now_table_id != last_table_id || now_chair_id != last_chair_id))
	{
		game_table_proxy_->SetTableUserItem(now_table_id, now_chair_id, game_user_item);
	}

	// ����״̬
	if (chair_count_ < MAX_CHAIR && now_table_id != INVALID_TABLE && now_table_id == last_table_id && now_chair_id == last_chair_id)
	{
		game_table_proxy_->SetTableUserItem(now_table_id, now_chair_id, game_user_item);
	}

	// �뿪֪ͨ
	if (last_table_id != INVALID_TABLE && (now_table_id != last_table_id || now_chair_id != last_chair_id))
	{
		// ��Ϸ֪ͨ
		if (game_user_item == myself_user_item_ || last_table_id == me_user_info->wTableID)
		{
			tagUserStatus UserStatus;
			UserStatus.wTableID = now_table_id;
			UserStatus.wChairID = now_chair_id;
			UserStatus.cbUserStatus = now_status;
			if (GameKernel::get())
				GameKernel::get()->OnGFUserStatus(game_user_item->GetUserID(), UserStatus);
		}

		if (game_user_item == myself_user_item_)
		{
			request_table_id_ = INVALID_TABLE;
			request_chair_id_ = INVALID_CHAIR;
			//uint16 kindid = game_kind_.wKindID;
			//Disconnect();
			//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOBBY_SCENE");
		}

		return;
	}

	// ���봦��
	if (now_table_id != INVALID_TABLE && (now_table_id != last_table_id || now_chair_id != last_chair_id))
	{
		// ��Ϸ֪ͨ
		if (game_user_item != myself_user_item_ && me_user_info->wTableID == now_table_id)
		{
			assert(now_table_id != INVALID_CHAIR);
			if (GameKernel::get())
				GameKernel::get()->OnGFUserEnter(game_user_item);
		}

		// �Լ��ж�
		if (myself_user_item_ == game_user_item)
		{
			if (GameKernel::get() == nullptr)
			{
				GameKernel* kernel = GameKernel::create();
				if (!kernel->Init(game_kind_.wKindID, chair_count_, game_kind_.szKindName))
				{
					GameKernel::destory();
					OnGFGameClose();
					return;
				}
			}
		}

		return;
	}

	// ״̬�ı�
	if (now_table_id != INVALID_TABLE && now_table_id == last_table_id && me_user_info->wTableID == now_table_id)
	{
		// ��Ϸ֪ͨ
		tagUserStatus UserStatus;
		UserStatus.wTableID = now_table_id;
		UserStatus.wChairID = now_chair_id;
		UserStatus.cbUserStatus = now_status;
		if (GameKernel::get())
			GameKernel::get()->OnGFUserStatus(game_user_item->GetUserID(), UserStatus);

		return;
	}
}

// ��¼����
bool ServerKernel::OnSocketMainLogon(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GR_LOGON_SUCCESS:	// ��¼�ɹ�
	{
		return OnSocketSubLogonSuccess(data, data_size);
	}
	case SUB_GR_LOGON_FAILURE:	// ��¼ʧ��
	{
		return OnSocketSubLogonFailure(data, data_size);
	}
	case SUB_GR_LOGON_FINISH:	// ��¼���
	{
		return OnSocketSubLogonFinish(data, data_size);
	}
	case SUB_GR_UPDATE_NOTIFY:	// ������ʾ
	{
		return OnSocketSubUpdateNotify(data, data_size);
	}
	case SUB_GR_FAILURE:
	{
		return OnSocketSubFailure(data, data_size);
	}
	}

	return true;
}

// ���ô���
bool ServerKernel::OnSocketMainConfig(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GR_CONFIG_COLUMN:		// �б�����
	{
		CCLOG("SUB_GR_CONFIG_COLUMN �б�����");
		return true;
	}
	case SUB_GR_CONFIG_SERVER:		// ��������
	{
		//Ч������
		assert(data_size == sizeof(CMD_GR_ConfigServer));
		if (data_size < sizeof(CMD_GR_ConfigServer))
			return false;

		// ��Ϣ����
		PACKET_HELPER_DATA(data);

		table_count_ = packet.read2Byte();
		chair_count_ = packet.read2Byte();

		// ��������
		server_type_ = packet.read2Byte();
		server_rule_ = packet.read4Byte();
		if (!game_table_proxy_->InitGameTable(table_count_, chair_count_, server_rule_, server_type_))
		{
			return false;
		}

		return true;
	}
	case SUB_GR_CONFIG_FINISH:		// �������
	{
		return true;
	}
	case SUB_GR_CONFIG_USER_RIGHT:	// �������Ȩ��
	{
		//Ч������
		assert(data_size >= sizeof(CMD_GR_ConfigUserRight));
		if (data_size < sizeof(CMD_GR_ConfigUserRight))
			return false;

		PACKET_HELPER_DATA(data);

		// ���Ȩ��
		user_right_ = packet.read4Byte();

		// TODO:֪ͨ�ͻ���
		if (game_ready_)
		{
			/*IPC_GF_UserRight ur = { 0 };
			ur.dwUserRight = user_right_;

			ipc_manager_->SendProcessData(IPC_CMD_GF_CONFIG, IPC_SUB_GF_USER_RIGHT, &ur, sizeof(ur));*/
		}

		return true;
	}
	}

	//�������
	assert(0);

	return true;
}

// �û�����
bool ServerKernel::OnSocketMainUser(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GR_USER_ENTER:			// �û�����
	{
		return OnSocketSubUserEnter(data, data_size);
	}
	case SUB_GR_USER_SCORE:			// �û�����
	{
		return OnSocketSubUserScore(data, data_size);
	}
	case SUB_GR_USER_STATUS:		// �û�״̬
	{
		return OnSocketSubUserStatus(data, data_size);
	}
	case SUB_GR_REQUEST_FAILURE:	// ����ʧ��
	{
		return OnSocketSubRequestFailure(data, data_size);
	}
	case SUB_GR_USER_CHAT:			// �û�����
	{
		return OnSocketSubUserChat(data, data_size);
	}
	case SUB_GR_UPDATE_SCORE:		//���·���
	{
		return OnSocketSubUpdateScore(data, data_size);
	}
	}

	return true;
}

// ״̬����
bool ServerKernel::OnSocketMainStatus(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GR_TABLE_INFO:		// ������Ϣ
	{
		PACKET_HELPER_DATA(data);
		uint16 table_count = packet.read2Byte();

		for (uint16 i = 0; i < table_count; i++)
		{
			uint8 lock = packet.readByte();
			uint8 playing = packet.readByte();
			game_table_proxy_->SetTableStatus(i, (playing != 0), (lock != 0));
		}

		return true;
	}
	case SUB_GR_TABLE_STATUS:	// ����״̬
	{
		// Ч�����
		assert(data_size == sizeof(CMD_GR_TableStatus));
		if (data_size != sizeof(CMD_GR_TableStatus))
			return false;

		// ��������
		PACKET_HELPER_DATA(data);
		uint16 table_id = packet.read2Byte();
		uint8 lock = packet.readByte();
		uint8 playing = packet.readByte();
		game_table_proxy_->SetTableStatus(table_id, (playing != 0), (lock != 0));

		return true;
	}
	}

	return true;
}

// ϵͳ��Ϣ
bool ServerKernel::OnSocketMainSystem(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	PACKET_HELPER_DATA(data);
	uint16 type = packet.read2Byte();
	uint16 len = packet.read2Byte();
	std::string str = packet.readString(len);

	// �رմ���
	if ((type & (SMT_CLOSE_ROOM | SMT_CLOSE_LINK)) != 0)
	{
		// �رմ���
		//socket_engine_->Disconnect();
		if (GameKernel::get())
			GameKernel::get()->Intermit();

		// ״̬����
		service_status_ = SS_NETWORKCLOSE;
		MsgBox(str);
	}

	// ��ʾ��Ϣ
	if (type & SMT_CHAT)
	{
		//InsertSystemMessage(system_message->szString);
	}

	// �ر���Ϸ
	if (type & SMT_CLOSE_GAME)
	{
		//socket_engine_->Disconnect();
		if (GameKernel::get())
			GameKernel::get()->Intermit();

		// ״̬����
		service_status_ = SS_NETWORKCLOSE;
		MsgBox(str);
	}

	// ������Ϣ
	if (type & SMT_EJECT)
	{
		//ShowInformation(system_message->szString);
	}

	// �رշ���
	if (type & SMT_CLOSE_ROOM)
	{
		//socket_engine_->Disconnect();
		if (GameKernel::get())
			GameKernel::get()->Intermit();

		// ״̬����
		service_status_ = SS_NETWORKCLOSE;
	}

	// ƽ̨ά��
	if (type & SMT_MAINTENANCE)
	{
		//socket_engine_->Disconnect();
		if (GameKernel::get())
			GameKernel::get()->Intermit();
		MsgBox(str);
	}

	return true;
}

// ��Ϸ����
bool ServerKernel::OnSocketMainGameFrame(uint16 main_cmd_id, uint16 sub_cmd_id, void* data, uint16 data_size)
{
	// Ч������
	assert(data_size <= SOCKET_TCP_PACKET);
	if (data_size > SOCKET_TCP_PACKET)
		return false;
	if (!GameKernel::get() || !game_ready_)
		return true;

	return GameKernel::get()->OnGFEventSocket(main_cmd_id, sub_cmd_id, data, data_size);

	return true;
}

// ��¼�ɹ�
bool ServerKernel::OnSocketSubLogonSuccess(void* data, uint16 data_size)
{
	// Ч�����
	assert(data_size == sizeof(CMD_GR_LogonSuccess));
	if (data_size != sizeof(CMD_GR_LogonSuccess))
		return false;

	// ��Ϣ����
	PACKET_HELPER_DATA(data);

	//�������
	user_right_ = packet.read4Byte();
	master_right_ = packet.read4Byte();

	service_status_ = SS_RECVINFO;

	// TODO:������һ�����ӷ������Ϣ��ʾ"���ڶ�ȡ������Ϣ..."

	return true;
}

// ��¼ʧ��
bool ServerKernel::OnSocketSubLogonFailure(void* data, uint16 data_size)
{
	// ��Ϣ����
	PACKET_HELPER_DATA(data);

	// TODO:�ر���ʾ

	// �رմ���
	socket_engine_->Disconnect();

	// ����״̬
	service_status_ = SS_NETWORKCLOSE;

	// ��ʾ��Ϣ
	int error_code = packet.read4Byte();
	std::string str = packet.readString((data_size - 4) / 2);
	MsgBox(str);

	// TODO: ������л�����Ļ�Ӧ�����ⷵ�ص������б����

	return true;
}

// ��¼���
bool ServerKernel::OnSocketSubLogonFinish(void* data, uint16 data_size)
{
	// TODO:�ر���ʾ

	service_status_ = SS_SERVICEING;

	////�����ж�
	//if (CServerRule::IsForfendGameRule(m_dwServerRule)==false)
	//{
	//	//���͹���
	//	SendUserRulePacket();

	//	//���ð�ť
	//	m_btTableButton4.EnableWindow(TRUE);
	//}

	////�����ж�
	//if (CServerRule::IsForfendLockTable(m_dwServerRule)==false)
	//{
	//	//���ð�ť
	//	m_btTableButton3.EnableWindow(TRUE);
	//}

	////���Ұ�ť
	//if (CServerRule::IsAllowAvertCheatMode(m_dwServerRule)==false)
	//{
	//	m_btTableButton2.EnableWindow(TRUE);
	//}

	////�����ж�
	//if (myself_user_item_ && myself_user_item_->GetTableID() != INVALID_TABLE)
	//{
	//	PerformSitDownAction(myself_user_item_->GetTableID(), myself_user_item_->GetChairID(), false);
	//}

	// ���뷿��
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_ROOM_SCENE");

	return true;
}

// ������ʾ
bool ServerKernel::OnSocketSubUpdateNotify(void* data, uint16 data_size)
{
	// Ч�����
	assert(data_size == sizeof(CMD_GR_UpdateNotify));
	if (data_size != sizeof(CMD_GR_UpdateNotify))
		return false;

	// TODO;����б�Ҫ������ʾ��Ϸ����

	return true;
}

bool ServerKernel::OnSocketSubUserEnter(void* data, uint16 data_size)
{
	// Ч�����
	assert(data_size >= sizeof(tagUserInfoHead));
	if (data_size < sizeof(tagUserInfoHead))
		return false;

	tagUserInfo user_info;
	memset(&user_info, 0, sizeof(user_info));

	PACKET_HELPER_DATA(data);

	// �û�����
	user_info.dwGameID = packet.read4Byte();
	user_info.dwUserID = packet.read4Byte();
	user_info.wFaceID = packet.read2Byte();
	user_info.cbGender = packet.readByte();
	user_info.cbMemberOrder = packet.readByte();
	user_info.cbMasterOrder = packet.readByte();

	// �û�״̬
	user_info.wTableID = packet.read2Byte();
	user_info.wChairID = packet.read2Byte();
	user_info.cbUserStatus = packet.readByte();

	// �û�����
	user_info.lScore = packet.read8Byte();
	user_info.dwUserMedal = packet.read4Byte();
	user_info.dwUserLottery = packet.read4Byte();
	user_info.lInsure = packet.read8Byte();

	user_info.dwWinCount = packet.read4Byte();
	user_info.dwLostCount = packet.read4Byte();
	user_info.dwDrawCount = packet.read4Byte();
	user_info.dwFleeCount = packet.read4Byte();
	user_info.dwExperience = packet.read4Byte();

	// ��չ��Ϣ
	while (data_size > packet.getPosition())
	{
		uint16 describe_size = packet.read2Byte();
		uint16 describe = packet.read2Byte();
		if (describe == 0) break;

		switch (describe)
		{
		case DTP_GR_NICK_NAME:		// �û��ǳ�
		{
			std::string str = packet.readString(describe_size / 2);
			strncpy(user_info.szNickName, str.c_str(), arraysize(user_info.szNickName));
			break;
		}
		case DTP_GR_SIGNATURE:		// ����ǩ��
		{
			std::string str = packet.readString(describe_size / 2);
			strncpy(user_info.szSignature, str.c_str(), arraysize(user_info.szSignature));
			break;
		}
		}

	}

	bool master_user_order = user_info.cbMasterOrder > 0 || (myself_user_item_ && myself_user_item_->GetMasterOrder() > 0);

	// �����û�
	IGameUserItem* game_user_item = client_user_manager_->SearchUserByUserID(user_info.dwUserID);
	if (game_user_item == NULL)
		game_user_item = client_user_manager_->ActiveUserItem(user_info);

	return true;
}

bool ServerKernel::OnSocketSubUserScore(void* data, uint16 data_size)
{
	// Ч�����
	assert(data_size == sizeof(CMD_GR_UserScore));
	if (data_size < sizeof(CMD_GR_UserScore))
		return false;

	// Ѱ���û�
	PACKET_HELPER_DATA(data);
	tagUserScore UserScore;
	uint32 dwUserID = packet.read4Byte();
	UserScore.lScore = packet.read8Byte();
	UserScore.dwUserMedal = packet.read4Byte();
	UserScore.dwUserLottery = packet.read4Byte();
	UserScore.lInsure = packet.read8Byte();
	UserScore.dwWinCount = packet.read4Byte();
	UserScore.dwLostCount = packet.read4Byte();
	UserScore.dwDrawCount = packet.read4Byte();
	UserScore.dwFleeCount = packet.read4Byte();
	UserScore.dwExperience = packet.read4Byte();

	IGameUserItem* user_item = client_user_manager_->SearchUserByUserID(dwUserID);

	// �û��ж�
	assert(user_item != NULL);
	if (user_item == NULL)
		return true;

	client_user_manager_->UpdateUserItemScore(user_item, &UserScore);
	GlobalUserData * pUserData = g_global_userinfo.GetGlobalUserData();

	if (pUserData->user_id == dwUserID)
	{
		pUserData->user_score = UserScore.lScore;
		pUserData->user_Medal = UserScore.dwUserMedal;
		pUserData->user_Lottery = UserScore.dwUserLottery;
	}

	return true;
}

bool ServerKernel::OnSocketSubUserStatus(void* data, uint16 data_size)
{
	// Ч�����
	assert(data_size == sizeof(CMD_GR_UserStatus));
	if (data_size < sizeof(CMD_GR_UserStatus))
		return false;

	// Ѱ���û�
	PACKET_HELPER_DATA(data);
	uint32 dwUserID = packet.read4Byte();
	tagUserStatus UserStatus;
	UserStatus.wTableID = packet.read2Byte();
	UserStatus.wChairID = packet.read2Byte();
	UserStatus.cbUserStatus = packet.readByte();

	IGameUserItem* user_item = client_user_manager_->SearchUserByUserID(dwUserID);

	// �û��ж�
	assert(user_item != NULL);
	if (user_item == NULL)
		return true;

	// ����״̬
	if (UserStatus.cbUserStatus == US_NULL)
	{
		// ɾ���û�
		client_user_manager_->DeleteUserItem(user_item);
	}
	else
	{
		// �����û�
		client_user_manager_->UpdateUserItemStatus(user_item, &UserStatus);
	}

	return true;
}

bool ServerKernel::OnSocketSubUserChat(void* data, uint16 data_size)
{
	return true;
}

bool ServerKernel::OnSocketSubRequestFailure(void* data, uint16 data_size)
{
	uint16 me_chairid = myself_user_item_->GetTableID();
	uint16 me_tableid = myself_user_item_->GetChairID();

	if (request_table_id_ != INVALID_TABLE && request_chair_id_ != INVALID_CHAIR && request_chair_id_ != MAX_CHAIR)
	{
		IGameUserItem* table_user_item = game_table_proxy_->GetTableUserItem(request_table_id_, request_chair_id_);
		if (table_user_item == myself_user_item_)
			game_table_proxy_->SetTableUserItem(request_table_id_, request_chair_id_, NULL);
	}

	request_table_id_ = INVALID_TABLE;
	request_chair_id_ = INVALID_CHAIR;

	PACKET_HELPER_DATA(data);
	packet.read4Byte();
	std::string str = packet.readString((data_size - 4) / 2);

	MsgBox(str);

	return true;
}

// ���·���
bool ServerKernel::OnSocketSubUpdateScore(void* data, uint16 data_size)
{
	// Ч�����
	assert(data_size == sizeof(CMD_GR_UpdateScore));
	if (data_size < sizeof(CMD_GR_UpdateScore))
		return false;

	// Ѱ���û�
	PACKET_HELPER_DATA(data);
	tagUserScore UserScore;
	uint32 dwUserID = packet.read4Byte();
	UserScore.lScore = packet.read8Byte();

	//client_user_manager_->UpdateUserItemScore(user_item, &UserScore);
	GlobalUserData * pUserData = g_global_userinfo.GetGlobalUserData();

	if (pUserData->user_id == dwUserID)
	{
		pUserData->user_score = UserScore.lScore;
	}

	return true;
}

bool ServerKernel::SendLoginPacket()
{
	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	PACKET_HELPER_SIZE(512);
	packet.write4Byte(1);
	packet.write4Byte(user_data->user_id);
	packet.writeString(user_data->password, LEN_PASSWORD);
	packet.writeString("test", LEN_MACHINE_ID);

	Send(MDM_GR_LOGON, SUB_GR_LOGON_USERID, packet.getBuffer(), packet.getPosition());

	return true;
}

bool ServerKernel::SendUserRulePacket()
{
	return true;
}

bool ServerKernel::SendLookonPacket(uint16 table_id, uint16 chair_id)
{
	PACKET_HELPER_SIZE(4);
	packet.write2Byte(table_id);
	packet.write2Byte(chair_id);

	Send(MDM_GR_USER, SUB_GR_USER_LOOKON, packet.getBuffer(), packet.getPosition());

	return true;
}

bool ServerKernel::SendSitDownPacket(uint16 table_id, uint16 chair_id, const char* password)
{
	PACKET_HELPER_SIZE(128);
	packet.write2Byte(table_id);
	packet.write2Byte(chair_id);
	packet.writeString(password, LEN_PASSWORD);

	Send(MDM_GR_USER, SUB_GR_USER_SITDOWN, packet.getBuffer(), packet.getPosition());

	return true;
}

bool ServerKernel::SendStandUpPacket(uint16 table_id, uint16 chair_id, uint8 force_leave)
{
	PACKET_HELPER_SIZE(10);
	packet.write2Byte(table_id);
	packet.write2Byte(chair_id);
	packet.writeByte(force_leave);

	Send(MDM_GR_USER, SUB_GR_USER_STANDUP, packet.getBuffer(), packet.getPosition());

	return true;
}

// ����Ч��
bool ServerKernel::EfficacyTableRule(uint16 table_id, uint16 chair_id, bool req_lookon, std::string& desc)
{
	// Ч�����
	assert(table_id < table_count_);
	assert(chair_id < chair_count_);

	// ״̬����
	if (service_status_ != SS_SERVICEING)
		return false;
	if (table_id >= table_count_)
		return false;
	if (chair_id >= chair_count_)
		return false;

	bool game_start = game_table_proxy_->IsPlaying(table_id);
	bool dynamic_join = CServerRule::IsAllowDynamicJoin(server_rule_);

	// ��Ϸ״̬
	if (game_start && !dynamic_join && !req_lookon)
	{
		desc = a2u("����Ϸ���Ѿ���ʼ��Ϸ�ˣ���ʱ���ܼ��룡");
		return false;
	}

	////�����ж�
	//if ((bReqLookon==false)&&(m_pIMySelfUserItem->GetMasterOrder()==0))
	//{
	//	//�����ж�
	//	for (WORD i=0;i<m_TableViewFrame.GetChairCount();i++)
	//	{
	//		//��ȡ�û�
	//		IClientUserItem * pITableUserItem=pITableView->GetClientUserItem(i);
	//		if ((pITableUserItem==NULL)||(pITableUserItem==m_pIMySelfUserItem)) continue;

	//		//������
	//		if (pParameterGlobal->m_bLimitDetest==true)
	//		{
	//			if (pITableUserItem->GetUserCompanion()==CP_DETEST)
	//			{
	//				//������ʾ
	//				LPCTSTR pszNickName=pITableUserItem->GetNickName();
	//				strDescribe.Format(TEXT("�������˲��벻�ܻ�ӭ�������Ϸ��������������ӭ����� [ %s ] ��"),pszNickName);

	//				return false;
	//			}
	//		}

	//		//ʤ��Ч��
	//		if (m_pParameterGame->m_bLimitWinRate==true)
	//		{
	//			if (((WORD)(pITableUserItem->GetUserWinRate()*1000L))<m_pParameterGame->m_wMinWinRate)
	//			{
	//				strDescribe.Format(TEXT("[ %s ] ��ʤ��̫���ˣ����������ò�����"),pITableUserItem->GetNickName());
	//				return false;
	//			}
	//		}

	//		//����Ч��
	//		if (m_pParameterGame->m_bLimitFleeRate)
	//		{
	//			if (((WORD)(pITableUserItem->GetUserFleeRate()*1000L))<m_pParameterGame->m_wMaxFleeRate)
	//			{
	//				strDescribe.Format(TEXT("[ %s ] ������̫���ˣ����������ò�����"),pITableUserItem->GetNickName());
	//				return false;
	//			}
	//		}

	//		//����Ч��
	//		if (m_pParameterGame->m_bLimitGameScore)
	//		{
	//			//��߻���
	//			if (pITableUserItem->GetUserScore()>m_pParameterGame->m_lMaxGameScore)
	//			{
	//				strDescribe.Format(TEXT("[ %s ] �Ļ���̫���ˣ����������ò�����"),pITableUserItem->GetNickName());
	//				return false;
	//			}

	//			//��ͻ���
	//			if (pITableUserItem->GetUserScore()<m_pParameterGame->m_lMinGameScore)
	//			{
	//				strDescribe.Format(TEXT("[ %s ] �Ļ���̫���ˣ����������ò�����"),pITableUserItem->GetNickName());
	//				return false;
	//			}
	//		}
	//	}
	//}

	return true;
}

// ʧ��
bool ServerKernel::OnSocketSubFailure(void* data, uint16 data_size)
{
	// ��Ϣ����
	PACKET_HELPER_DATA(data);

	// ��ʾ��Ϣ
	int error_code = packet.read4Byte();
	std::string str = packet.readString((data_size - 4) / 2);
	MsgBox(str);

	// TODO: ������л�����Ļ�Ӧ�����ⷵ�ص������б����

	return true;
}