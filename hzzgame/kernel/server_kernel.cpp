
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

// 连接网络
bool ServerKernel::Connect(const tagGameKind* game_kind, const tagGameServer* game_server)
{
	if (service_status_ != SS_UNKNOW && service_status_ != SS_NETWORKCLOSE)
	{
		MsgBox(a2u("您已经在游戏房间了，请先退出之前的房间"));
		return false;
	}

	// 关闭判断
	assert(game_server->wServerID != 0);
	if (game_server->wServerID == 0)
	{
		service_status_ = SS_UNKNOW;
		MsgBox(a2u("很抱歉，此游戏房间已经关闭了，不允许继续进入!"));
		return false;
	}

	CCLOG("%s, %d", game_server->szServerAddr, game_server->wServerPort);
	if (!socket_engine_->Connect(game_server->szServerAddr, game_server->wServerPort))
	{
		MsgBox(a2u("游戏房间连接失败，请检查系统网络配置参数是否正确!"));
		return false;
	}

	memcpy(&game_kind_, game_kind, sizeof(game_kind_));
	memcpy(&game_server_, game_server, sizeof(game_server_));
	service_status_ = SS_ENTERING;
	return true;
}

// 关闭连接
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

// 发送数据
bool ServerKernel::Send(uint16 main, uint16 sub, void* data, uint16 size)
{
	return socket_engine_->Send(main, sub, data, size);
}

bool ServerKernel::PerformLookonAction(uint16 table_id, uint16 chair_id)
{
	// 效验数据
	assert(table_id != INVALID_TABLE);
	assert(chair_id != INVALID_CHAIR);

	// 状态过滤
	if (service_status_ != SS_SERVICEING)
		return false;
	if (request_table_id_ == table_id && request_chair_id_ == chair_id)
		return false;

	// 自己状态
	if (myself_user_item_->GetUserStatus() >= US_PLAYING)
	{
		MsgBox(a2u("您正在游戏中，暂时不能离开，请先结束当前游戏！"));

		return false;
	}

	// 权限判断
	if (!CUserRight::CanLookon(user_right_))
	{
		MsgBox(a2u("抱歉，您暂时没有旁观游戏的权限！"));

		return false;
	}

	// 清理界面
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
	// 效验数据
	assert(table_id != INVALID_TABLE);
	assert(chair_id != INVALID_CHAIR);

	// 状态过滤
	if (service_status_ != SS_SERVICEING)
		return false;
	if (request_table_id_ == table_id && request_chair_id_ == chair_id)
		return false;

	// 自己状态
	if (myself_user_item_->GetUserStatus() >= US_PLAYING)
	{
		MsgBox(a2u("您正在游戏中，暂时不能离开，请先结束当前游戏！"));

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
	// 状态过滤
	if (service_status_ != SS_SERVICEING)
		return false;
	if (request_table_id_ != INVALID_TABLE && request_table_id_ == table_id)
		return false;
	if (request_chair_id_ != INVALID_CHAIR && request_chair_id_ == chair_id)
		return false;

	// 自己状态
	if (myself_user_item_->GetUserStatus() >= US_PLAYING)
	{
		MsgBox(a2u("您正在游戏中，暂时不能离开，请先结束当前游戏！"));

		return false;
	}

	// 权限判断
	if (!CUserRight::CanPlay(user_right_))
	{
		MsgBox(a2u("抱歉，您暂时没有加入游戏的权限！"));

		return false;
	}

	// 桌子效验
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

	// 密码判断
	char password[LEN_PASSWORD] = { 0 };
	//if (myself_user_item_->GetMasterOrder() == 0 && efficacy_pass && table_id != INVALID_TABLE && chair_id != INVALID_CHAIR && chair_id != MAX_CHAIR)
	//{
	//	//变量定义
	//	bool bLocker=m_TableViewFrame.GetLockerFlag(wTableID);

	//	//密码判断
	//	if(bLocker)
	//	{
	//		//设置密码
	//		CDlgTablePassword DlgTablePassword;
	//		DlgTablePassword.SetPromptString(TEXT("该桌已设置进入密码，请输入密码。"));
	//		if (DlgTablePassword.DoModal()!=IDOK) return false;

	//		//设置密码
	//		lstrcpyn(szPassword,DlgTablePassword.m_szPassword,CountArray(szPassword));
	//	}
	//}

	// 清理界面
	if (request_table_id_ != INVALID_TABLE && request_chair_id_ != INVALID_CHAIR && request_chair_id_ != MAX_CHAIR)
	{
		IGameUserItem* user_item = game_table_proxy_->GetTableUserItem(request_table_id_, request_chair_id_);
		if (user_item == myself_user_item_)
			game_table_proxy_->SetTableUserItem(request_table_id_, request_chair_id_, NULL);
	}

	// 设置界面
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

// 踢人
bool ServerKernel::KickUser(DWORD dwGameID)
{
	//发送数据
	if (myself_user_item_->GetMasterOrder() != 0)
	{
		PACKET_HELPER_SIZE(128);
		packet.write4Byte(dwGameID);

		Send(MDM_GR_MANAGE, SUB_GR_KILL_USER, packet.getBuffer(), packet.getPosition());
	}

	return true;
}

// 踢人
bool ServerKernel::KickAllUser()
{
	//发送数据
	if (myself_user_item_->GetMasterOrder() != 0)
	{
		PACKET_HELPER_SIZE(128);
		packet.writeString(a2u("kick_all_！"), 64);

		Send(MDM_GR_MANAGE, SUB_GR_KICK_ALL_USER, packet.getBuffer(), packet.getPosition());
	}

	return true;
}

// 禁止进入
bool ServerKernel::ProhibitedEntry()
{
	//发送数据
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

// 更换位置
bool ServerKernel::PerformChangeChair()
{
	return Send(MDM_GR_USER, SUB_GR_USER_CHAIR_REQ, nullptr, 0);
}

// 游戏已准备好
void ServerKernel::OnGFGameReady()
{
	uint16 table_id = myself_user_item_->GetTableID();
	uint16 chair_id = myself_user_item_->GetChairID();

	// 房间信息
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

	// 发送用户
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

	// 配置完成
	GameKernel::get()->OnGFConfigFinish();

	game_ready_ = true;
}

// 游戏关闭
void ServerKernel::OnGFGameClose()
{
	// 效验状态
	assert(myself_user_item_ != nullptr);
	if (myself_user_item_ == nullptr)
		return;

	game_ready_ = false;

	uint16 table_id = myself_user_item_->GetTableID();
	uint16 chair_id = myself_user_item_->GetChairID();

	// 退出游戏
	if ((table_id != INVALID_TABLE) && (service_status_ == SS_SERVICEING))
	{
		SendStandUpPacket(table_id, chair_id, 1);
	}
	if (ServerKernel::get())
	{
		ServerKernel::destory();
	}
}

// 发送数据
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
		// TODO:返回到房间列表界面
	}
}

void ServerKernel::OnSocketError(int errorCode)
{
	switch (errorCode)
	{
	case 10054:
	{
		MsgBox(a2u("操作异常, 被断开连接!!!"));
	}
	break;
	}
}

bool ServerKernel::OnSocketData(uint16 main, uint16 sub, void* data, uint16 data_size)
{
	switch (main)
	{
	case MDM_GR_LOGON:		// 登录消息
	{
		return OnSocketMainLogon(sub, data, data_size);
	}
	case MDM_GR_CONFIG:		// 配置信息
	{
		return OnSocketMainConfig(sub, data, data_size);
	}
	case MDM_GR_USER:		// 用户信息
	{
		return OnSocketMainUser(sub, data, data_size);
	}
	case MDM_GR_STATUS:		// 状态信息
	{
		return OnSocketMainStatus(sub, data, data_size);
	}
	//case MDM_GR_MANAGE:		//管理消息
	//	{
	//		br=OnSocketMainManager(sub,pData,wDataSize);

	//		return true;
	//	}
	case MDM_CM_SYSTEM:		// 系统消息
	{
		return OnSocketMainSystem(sub, data, data_size);
	}
	case MDM_GF_GAME:		// 游戏消息
	case MDM_GF_FRAME:		// 框架消息
	{
		return OnSocketMainGameFrame(main, sub, data, data_size);
	}
	}

	return true;
}

void ServerKernel::OnUserItemAcitve(IGameUserItem* game_user_item)
{
	// 判断自己
	if (myself_user_item_ == NULL)
	{
		myself_user_item_ = game_user_item;
	}

	// 设置桌子
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

	// 离开处理
	if ((last_table_id != INVALID_TABLE) && (last_chair_id != INVALID_CHAIR))
	{
		// 获取用户
		IGameUserItem* table_user_item = game_table_proxy_->GetTableUserItem(last_table_id, last_chair_id);
		if (table_user_item == game_user_item) game_table_proxy_->SetTableUserItem(last_table_id, last_chair_id, NULL);

		// 离开通知
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

	// 桌子离开
	if (last_table_id != INVALID_TABLE && (last_table_id != now_table_id || last_chair_id != now_chair_id))
	{
		IGameUserItem* table_user_item = game_table_proxy_->GetTableUserItem(last_table_id, last_chair_id);
		if (table_user_item == game_user_item)
		{
			game_table_proxy_->SetTableUserItem(last_table_id, last_chair_id, NULL);
		}
	}

	// 桌子加入
	if (now_table_id != INVALID_TABLE && now_status != US_LOOKON && (now_table_id != last_table_id || now_chair_id != last_chair_id))
	{
		game_table_proxy_->SetTableUserItem(now_table_id, now_chair_id, game_user_item);
	}

	// 桌子状态
	if (chair_count_ < MAX_CHAIR && now_table_id != INVALID_TABLE && now_table_id == last_table_id && now_chair_id == last_chair_id)
	{
		game_table_proxy_->SetTableUserItem(now_table_id, now_chair_id, game_user_item);
	}

	// 离开通知
	if (last_table_id != INVALID_TABLE && (now_table_id != last_table_id || now_chair_id != last_chair_id))
	{
		// 游戏通知
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

	// 加入处理
	if (now_table_id != INVALID_TABLE && (now_table_id != last_table_id || now_chair_id != last_chair_id))
	{
		// 游戏通知
		if (game_user_item != myself_user_item_ && me_user_info->wTableID == now_table_id)
		{
			assert(now_table_id != INVALID_CHAIR);
			if (GameKernel::get())
				GameKernel::get()->OnGFUserEnter(game_user_item);
		}

		// 自己判断
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

	// 状态改变
	if (now_table_id != INVALID_TABLE && now_table_id == last_table_id && me_user_info->wTableID == now_table_id)
	{
		// 游戏通知
		tagUserStatus UserStatus;
		UserStatus.wTableID = now_table_id;
		UserStatus.wChairID = now_chair_id;
		UserStatus.cbUserStatus = now_status;
		if (GameKernel::get())
			GameKernel::get()->OnGFUserStatus(game_user_item->GetUserID(), UserStatus);

		return;
	}
}

// 登录处理
bool ServerKernel::OnSocketMainLogon(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GR_LOGON_SUCCESS:	// 登录成功
	{
		return OnSocketSubLogonSuccess(data, data_size);
	}
	case SUB_GR_LOGON_FAILURE:	// 登录失败
	{
		return OnSocketSubLogonFailure(data, data_size);
	}
	case SUB_GR_LOGON_FINISH:	// 登录完成
	{
		return OnSocketSubLogonFinish(data, data_size);
	}
	case SUB_GR_UPDATE_NOTIFY:	// 更新提示
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

// 配置处理
bool ServerKernel::OnSocketMainConfig(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GR_CONFIG_COLUMN:		// 列表配置
	{
		CCLOG("SUB_GR_CONFIG_COLUMN 列表配置");
		return true;
	}
	case SUB_GR_CONFIG_SERVER:		// 房间配置
	{
		//效验数据
		assert(data_size == sizeof(CMD_GR_ConfigServer));
		if (data_size < sizeof(CMD_GR_ConfigServer))
			return false;

		// 消息处理
		PACKET_HELPER_DATA(data);

		table_count_ = packet.read2Byte();
		chair_count_ = packet.read2Byte();

		// 房间属性
		server_type_ = packet.read2Byte();
		server_rule_ = packet.read4Byte();
		if (!game_table_proxy_->InitGameTable(table_count_, chair_count_, server_rule_, server_type_))
		{
			return false;
		}

		return true;
	}
	case SUB_GR_CONFIG_FINISH:		// 配置完成
	{
		return true;
	}
	case SUB_GR_CONFIG_USER_RIGHT:	// 配置玩家权限
	{
		//效验数据
		assert(data_size >= sizeof(CMD_GR_ConfigUserRight));
		if (data_size < sizeof(CMD_GR_ConfigUserRight))
			return false;

		PACKET_HELPER_DATA(data);

		// 玩家权限
		user_right_ = packet.read4Byte();

		// TODO:通知客户端
		if (game_ready_)
		{
			/*IPC_GF_UserRight ur = { 0 };
			ur.dwUserRight = user_right_;

			ipc_manager_->SendProcessData(IPC_CMD_GF_CONFIG, IPC_SUB_GF_USER_RIGHT, &ur, sizeof(ur));*/
		}

		return true;
	}
	}

	//错误断言
	assert(0);

	return true;
}

// 用户处理
bool ServerKernel::OnSocketMainUser(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GR_USER_ENTER:			// 用户进入
	{
		return OnSocketSubUserEnter(data, data_size);
	}
	case SUB_GR_USER_SCORE:			// 用户积分
	{
		return OnSocketSubUserScore(data, data_size);
	}
	case SUB_GR_USER_STATUS:		// 用户状态
	{
		return OnSocketSubUserStatus(data, data_size);
	}
	case SUB_GR_REQUEST_FAILURE:	// 请求失败
	{
		return OnSocketSubRequestFailure(data, data_size);
	}
	case SUB_GR_USER_CHAT:			// 用户聊天
	{
		return OnSocketSubUserChat(data, data_size);
	}
	case SUB_GR_UPDATE_SCORE:		//更新分数
	{
		return OnSocketSubUpdateScore(data, data_size);
	}
	}

	return true;
}

// 状态处理
bool ServerKernel::OnSocketMainStatus(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GR_TABLE_INFO:		// 桌子信息
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
	case SUB_GR_TABLE_STATUS:	// 桌子状态
	{
		// 效验参数
		assert(data_size == sizeof(CMD_GR_TableStatus));
		if (data_size != sizeof(CMD_GR_TableStatus))
			return false;

		// 设置桌子
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

// 系统消息
bool ServerKernel::OnSocketMainSystem(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	PACKET_HELPER_DATA(data);
	uint16 type = packet.read2Byte();
	uint16 len = packet.read2Byte();
	std::string str = packet.readString(len);

	// 关闭处理
	if ((type & (SMT_CLOSE_ROOM | SMT_CLOSE_LINK)) != 0)
	{
		// 关闭处理
		//socket_engine_->Disconnect();
		if (GameKernel::get())
			GameKernel::get()->Intermit();

		// 状态设置
		service_status_ = SS_NETWORKCLOSE;
		MsgBox(str);
	}

	// 显示消息
	if (type & SMT_CHAT)
	{
		//InsertSystemMessage(system_message->szString);
	}

	// 关闭游戏
	if (type & SMT_CLOSE_GAME)
	{
		//socket_engine_->Disconnect();
		if (GameKernel::get())
			GameKernel::get()->Intermit();

		// 状态设置
		service_status_ = SS_NETWORKCLOSE;
		MsgBox(str);
	}

	// 弹出消息
	if (type & SMT_EJECT)
	{
		//ShowInformation(system_message->szString);
	}

	// 关闭房间
	if (type & SMT_CLOSE_ROOM)
	{
		//socket_engine_->Disconnect();
		if (GameKernel::get())
			GameKernel::get()->Intermit();

		// 状态设置
		service_status_ = SS_NETWORKCLOSE;
	}

	// 平台维护
	if (type & SMT_MAINTENANCE)
	{
		//socket_engine_->Disconnect();
		if (GameKernel::get())
			GameKernel::get()->Intermit();
		MsgBox(str);
	}

	return true;
}

// 游戏处理
bool ServerKernel::OnSocketMainGameFrame(uint16 main_cmd_id, uint16 sub_cmd_id, void* data, uint16 data_size)
{
	// 效验数据
	assert(data_size <= SOCKET_TCP_PACKET);
	if (data_size > SOCKET_TCP_PACKET)
		return false;
	if (!GameKernel::get() || !game_ready_)
		return true;

	return GameKernel::get()->OnGFEventSocket(main_cmd_id, sub_cmd_id, data, data_size);

	return true;
}

// 登录成功
bool ServerKernel::OnSocketSubLogonSuccess(void* data, uint16 data_size)
{
	// 效验参数
	assert(data_size == sizeof(CMD_GR_LogonSuccess));
	if (data_size != sizeof(CMD_GR_LogonSuccess))
		return false;

	// 消息处理
	PACKET_HELPER_DATA(data);

	//玩家属性
	user_right_ = packet.read4Byte();
	master_right_ = packet.read4Byte();

	service_status_ = SS_RECVINFO;

	// TODO:可以做一个连接房间的信息提示"正在读取房间信息..."

	return true;
}

// 登录失败
bool ServerKernel::OnSocketSubLogonFailure(void* data, uint16 data_size)
{
	// 消息处理
	PACKET_HELPER_DATA(data);

	// TODO:关闭提示

	// 关闭处理
	socket_engine_->Disconnect();

	// 设置状态
	service_status_ = SS_NETWORKCLOSE;

	// 显示消息
	int error_code = packet.read4Byte();
	std::string str = packet.readString((data_size - 4) / 2);
	MsgBox(str);

	// TODO: 如果有切换界面的话应当在这返回到房间列表界面

	return true;
}

// 登录完成
bool ServerKernel::OnSocketSubLogonFinish(void* data, uint16 data_size)
{
	// TODO:关闭提示

	service_status_ = SS_SERVICEING;

	////规则判断
	//if (CServerRule::IsForfendGameRule(m_dwServerRule)==false)
	//{
	//	//发送规则
	//	SendUserRulePacket();

	//	//设置按钮
	//	m_btTableButton4.EnableWindow(TRUE);
	//}

	////锁桌判断
	//if (CServerRule::IsForfendLockTable(m_dwServerRule)==false)
	//{
	//	//设置按钮
	//	m_btTableButton3.EnableWindow(TRUE);
	//}

	////查找按钮
	//if (CServerRule::IsAllowAvertCheatMode(m_dwServerRule)==false)
	//{
	//	m_btTableButton2.EnableWindow(TRUE);
	//}

	////重入判断
	//if (myself_user_item_ && myself_user_item_->GetTableID() != INVALID_TABLE)
	//{
	//	PerformSitDownAction(myself_user_item_->GetTableID(), myself_user_item_->GetChairID(), false);
	//}

	// 进入房间
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_ROOM_SCENE");

	return true;
}

// 更新提示
bool ServerKernel::OnSocketSubUpdateNotify(void* data, uint16 data_size)
{
	// 效验参数
	assert(data_size == sizeof(CMD_GR_UpdateNotify));
	if (data_size != sizeof(CMD_GR_UpdateNotify))
		return false;

	// TODO;如果有必要可以提示游戏更新

	return true;
}

bool ServerKernel::OnSocketSubUserEnter(void* data, uint16 data_size)
{
	// 效验参数
	assert(data_size >= sizeof(tagUserInfoHead));
	if (data_size < sizeof(tagUserInfoHead))
		return false;

	tagUserInfo user_info;
	memset(&user_info, 0, sizeof(user_info));

	PACKET_HELPER_DATA(data);

	// 用户属性
	user_info.dwGameID = packet.read4Byte();
	user_info.dwUserID = packet.read4Byte();
	user_info.wFaceID = packet.read2Byte();
	user_info.cbGender = packet.readByte();
	user_info.cbMemberOrder = packet.readByte();
	user_info.cbMasterOrder = packet.readByte();

	// 用户状态
	user_info.wTableID = packet.read2Byte();
	user_info.wChairID = packet.read2Byte();
	user_info.cbUserStatus = packet.readByte();

	// 用户积分
	user_info.lScore = packet.read8Byte();
	user_info.dwUserMedal = packet.read4Byte();
	user_info.dwUserLottery = packet.read4Byte();
	user_info.lInsure = packet.read8Byte();

	user_info.dwWinCount = packet.read4Byte();
	user_info.dwLostCount = packet.read4Byte();
	user_info.dwDrawCount = packet.read4Byte();
	user_info.dwFleeCount = packet.read4Byte();
	user_info.dwExperience = packet.read4Byte();

	// 扩展信息
	while (data_size > packet.getPosition())
	{
		uint16 describe_size = packet.read2Byte();
		uint16 describe = packet.read2Byte();
		if (describe == 0) break;

		switch (describe)
		{
		case DTP_GR_NICK_NAME:		// 用户昵称
		{
			std::string str = packet.readString(describe_size / 2);
			strncpy(user_info.szNickName, str.c_str(), arraysize(user_info.szNickName));
			break;
		}
		case DTP_GR_SIGNATURE:		// 个性签名
		{
			std::string str = packet.readString(describe_size / 2);
			strncpy(user_info.szSignature, str.c_str(), arraysize(user_info.szSignature));
			break;
		}
		}

	}

	bool master_user_order = user_info.cbMasterOrder > 0 || (myself_user_item_ && myself_user_item_->GetMasterOrder() > 0);

	// 激活用户
	IGameUserItem* game_user_item = client_user_manager_->SearchUserByUserID(user_info.dwUserID);
	if (game_user_item == NULL)
		game_user_item = client_user_manager_->ActiveUserItem(user_info);

	return true;
}

bool ServerKernel::OnSocketSubUserScore(void* data, uint16 data_size)
{
	// 效验参数
	assert(data_size == sizeof(CMD_GR_UserScore));
	if (data_size < sizeof(CMD_GR_UserScore))
		return false;

	// 寻找用户
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

	// 用户判断
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
	// 效验参数
	assert(data_size == sizeof(CMD_GR_UserStatus));
	if (data_size < sizeof(CMD_GR_UserStatus))
		return false;

	// 寻找用户
	PACKET_HELPER_DATA(data);
	uint32 dwUserID = packet.read4Byte();
	tagUserStatus UserStatus;
	UserStatus.wTableID = packet.read2Byte();
	UserStatus.wChairID = packet.read2Byte();
	UserStatus.cbUserStatus = packet.readByte();

	IGameUserItem* user_item = client_user_manager_->SearchUserByUserID(dwUserID);

	// 用户判断
	assert(user_item != NULL);
	if (user_item == NULL)
		return true;

	// 设置状态
	if (UserStatus.cbUserStatus == US_NULL)
	{
		// 删除用户
		client_user_manager_->DeleteUserItem(user_item);
	}
	else
	{
		// 更新用户
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

// 更新分数
bool ServerKernel::OnSocketSubUpdateScore(void* data, uint16 data_size)
{
	// 效验参数
	assert(data_size == sizeof(CMD_GR_UpdateScore));
	if (data_size < sizeof(CMD_GR_UpdateScore))
		return false;

	// 寻找用户
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

// 桌子效验
bool ServerKernel::EfficacyTableRule(uint16 table_id, uint16 chair_id, bool req_lookon, std::string& desc)
{
	// 效验参数
	assert(table_id < table_count_);
	assert(chair_id < chair_count_);

	// 状态过滤
	if (service_status_ != SS_SERVICEING)
		return false;
	if (table_id >= table_count_)
		return false;
	if (chair_id >= chair_count_)
		return false;

	bool game_start = game_table_proxy_->IsPlaying(table_id);
	bool dynamic_join = CServerRule::IsAllowDynamicJoin(server_rule_);

	// 游戏状态
	if (game_start && !dynamic_join && !req_lookon)
	{
		desc = a2u("此游戏桌已经开始游戏了，暂时不能加入！");
		return false;
	}

	////其他判断
	//if ((bReqLookon==false)&&(m_pIMySelfUserItem->GetMasterOrder()==0))
	//{
	//	//规则判断
	//	for (WORD i=0;i<m_TableViewFrame.GetChairCount();i++)
	//	{
	//		//获取用户
	//		IClientUserItem * pITableUserItem=pITableView->GetClientUserItem(i);
	//		if ((pITableUserItem==NULL)||(pITableUserItem==m_pIMySelfUserItem)) continue;

	//		//厌恶玩家
	//		if (pParameterGlobal->m_bLimitDetest==true)
	//		{
	//			if (pITableUserItem->GetUserCompanion()==CP_DETEST)
	//			{
	//				//设置提示
	//				LPCTSTR pszNickName=pITableUserItem->GetNickName();
	//				strDescribe.Format(TEXT("您设置了不与不受欢迎的玩家游戏，此桌有您不欢迎的玩家 [ %s ] ！"),pszNickName);

	//				return false;
	//			}
	//		}

	//		//胜率效验
	//		if (m_pParameterGame->m_bLimitWinRate==true)
	//		{
	//			if (((WORD)(pITableUserItem->GetUserWinRate()*1000L))<m_pParameterGame->m_wMinWinRate)
	//			{
	//				strDescribe.Format(TEXT("[ %s ] 的胜率太低了，与您的设置不符！"),pITableUserItem->GetNickName());
	//				return false;
	//			}
	//		}

	//		//逃率效验
	//		if (m_pParameterGame->m_bLimitFleeRate)
	//		{
	//			if (((WORD)(pITableUserItem->GetUserFleeRate()*1000L))<m_pParameterGame->m_wMaxFleeRate)
	//			{
	//				strDescribe.Format(TEXT("[ %s ] 的逃率太高了，与您的设置不符！"),pITableUserItem->GetNickName());
	//				return false;
	//			}
	//		}

	//		//积分效验
	//		if (m_pParameterGame->m_bLimitGameScore)
	//		{
	//			//最高积分
	//			if (pITableUserItem->GetUserScore()>m_pParameterGame->m_lMaxGameScore)
	//			{
	//				strDescribe.Format(TEXT("[ %s ] 的积分太高了，与您的设置不符！"),pITableUserItem->GetNickName());
	//				return false;
	//			}

	//			//最低积分
	//			if (pITableUserItem->GetUserScore()<m_pParameterGame->m_lMinGameScore)
	//			{
	//				strDescribe.Format(TEXT("[ %s ] 的积分太低了，与您的设置不符！"),pITableUserItem->GetNickName());
	//				return false;
	//			}
	//		}
	//	}
	//}

	return true;
}

// 失败
bool ServerKernel::OnSocketSubFailure(void* data, uint16 data_size)
{
	// 消息处理
	PACKET_HELPER_DATA(data);

	// 显示消息
	int error_code = packet.read4Byte();
	std::string str = packet.readString((data_size - 4) / 2);
	MsgBox(str);

	// TODO: 如果有切换界面的话应当在这返回到房间列表界面

	return true;
}