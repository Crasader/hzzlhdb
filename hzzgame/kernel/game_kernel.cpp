
#include "game_kernel.h"

#include "server_kernel.h"

#include "../games/game_installer.h"

static GameKernel *g_GameKernel = nullptr;
static int g_GameKernelRefCount = 0;

GameKernel* GameKernel::create()
{
	if (g_GameKernelRefCount == 0)
	{
		g_GameKernel = new GameKernel();
	}

	g_GameKernelRefCount++;

	return g_GameKernel;
}

void GameKernel::destory()
{
	if (g_GameKernelRefCount > 0)
	{
		g_GameKernelRefCount--;
		if (g_GameKernelRefCount <= 0)
		{
			g_GameKernelRefCount = 0;
			delete g_GameKernel;
			g_GameKernel = nullptr;
		}
	}
}

GameKernel* GameKernel::get()
{
	return g_GameKernel;
}

GameKernel::GameKernel()
	: in_service_(false), allow_lookon_(false), game_status_(GAME_STATUS_FREE),
	myself_user_item_(NULL), delegate_(nullptr)
{
	game_user_manager_ = new GameUserManager(this);

	memset(&user_attribute_, 0, sizeof(user_attribute_));
	memset(&game_attribute_, 0, sizeof(game_attribute_));
	memset(&server_attribute_, 0, sizeof(server_attribute_));
}

GameKernel::~GameKernel()
{
	delete game_user_manager_;
}

bool GameKernel::Init(uint16 kind_id, uint16 player_count, const char* game_name)
{
	SetGameAttribute(kind_id, player_count, 0, game_name);

	GameInstaller::InstallGame(kind_id);

	return true;
}

//中断
bool GameKernel::Intermit()
{
	if (delegate_)
		delegate_->CloseGameClient();
	GameKernel::destory();
	if (ServerKernel::get())
		ServerKernel::get()->OnGFGameClose();
	return true;
}

void GameKernel::SetDelegate(Delegate* delegate)
{
	delegate_ = delegate;

	if (delegate_ && ServerKernel::get())
		ServerKernel::get()->OnGFGameReady();
}

bool GameKernel::SetGameAttribute(uint16 kind_id, uint16 player_count, uint32 client_version, const char* game_name)
{
	game_attribute_.kind_id = kind_id;
	game_attribute_.chair_count = player_count;
	game_attribute_.client_version = client_version;
	strncpy(game_attribute_.game_name, game_name, arraysize(game_attribute_.game_name));

	return true;
}

bool GameKernel::IsSingleMode()
{
	return false;
}

bool GameKernel::IsLookonMode() 
{
	if (myself_user_item_ == NULL)
		return true;
	return myself_user_item_->GetUserStatus() == US_LOOKON;
}

bool GameKernel::IsAllowLookon() const
{
	return allow_lookon_;
}

bool GameKernel::IsServiceStatus() const
{
	return in_service_;
}

uint16 GameKernel::GetMeChairID() const
{
	if (myself_user_item_ == NULL)
		return INVALID_CHAIR;
	return myself_user_item_->GetChairID();
}

IGameUserItem* GameKernel::GetMeUserItem() const
{
	return myself_user_item_;
}

IGameUserItem* GameKernel::GetTableUserItem(uint16 chair_id)
{
	return game_user_manager_->GetTableUserItem(chair_id);
}

const GameUserItemVector& GameKernel::GetLookonUserVector()
{
	return game_user_manager_->GetLookonUserVector();
}

IGameUserItem* GameKernel::SearchUserItemByUserID(uint32 user_id)
{
	return game_user_manager_->SearchUserItemByUserID(user_id);
}

IGameUserItem* GameKernel::SearchUserItemByGameID(uint32 game_id)
{
	return game_user_manager_->SearchUserItemByGameID(game_id);
}

IGameUserItem* GameKernel::SearchUserItemByNickName(const char* nick_name)
{
	return game_user_manager_->SearchUserItemByNickName(nick_name);
}

bool GameKernel::SendSocketData(uint16 main_cmdid, uint16 sub_cmdid)
{
	return SendSocketData(main_cmdid, sub_cmdid, 0, 0);
}

bool GameKernel::SendSocketData(uint16 main_cmdid, uint16 sub_cmdid, void* data, uint16 data_size)
{
	if (!ServerKernel::get())
		return false;

	if (data_size > SOCKET_TCP_PACKET)
		return false;

	return ServerKernel::get()->Send(main_cmdid, sub_cmdid, data, data_size);
}

bool GameKernel::SendGameOption()
{
	in_service_ = true;

	PACKET_HELPER_SIZE(512);
	packet.writeByte(allow_lookon_ ? 1 : 0);
	packet.write4Byte(0);
	packet.write4Byte(game_attribute_.client_version);
	return SendSocketData(MDM_GF_FRAME, SUB_GF_GAME_OPTION, packet.getBuffer(), packet.getPosition());
}

bool GameKernel::SendUserReady()
{
	if (myself_user_item_ == NULL)
		return false;
	return SendSocketData(MDM_GF_FRAME, SUB_GF_USER_READY, NULL, 0);
}

bool GameKernel::SendUserLookon(uint32 user_id, bool allow_lookon)
{
	if (!IsLookonMode()) {
		CMD_GF_LookonConfig lookon_config;
		lookon_config.dwUserID = user_id;
		lookon_config.cbAllowLookon = allow_lookon ? 1 : 0;
		return SendSocketData(MDM_GF_FRAME, SUB_GF_LOOKON_CONFIG, &lookon_config, sizeof(lookon_config));
	}
	return true;
}

bool GameKernel::SendUserChatMessage(uint32 target_user_id, const char* message, COLORREF text_color)
{
	if (myself_user_item_ == NULL)
		return false;

	/*CMD_GF_C_UserChat user_chat;
	lstrcpyn(user_chat.szChatString, message, _countof(user_chat.szChatString));
	user_chat.dwChatColor = text_color;
	user_chat.dwTargetUserID = target_user_id;
	user_chat.wChatLength = CountStringBuffer(user_chat.szChatString);
	uint16 head_size = sizeof(user_chat) - sizeof(user_chat.szChatString);
	return SendSocketData(MDM_GF_FRAME, SUB_GF_USER_CHAT, &user_chat, head_size + user_chat.wChatLength * sizeof(TCHAR));*/

	return true;
}

void GameKernel::OnUserItemAcitve(IGameUserItem* game_user_item)
{
	if (game_user_item == NULL)
		return;
	if (myself_user_item_ == NULL && user_attribute_.user_id == game_user_item->GetUserID())
		myself_user_item_ = game_user_item;
	if (delegate_ != nullptr)
		delegate_->OnEventUserEnter(game_user_item, game_user_item->GetUserStatus() == US_LOOKON);
}

void GameKernel::OnUserItemDelete(IGameUserItem* game_user_item)
{
	if (game_user_item == NULL || delegate_ == NULL)
		return;
	delegate_->OnEventUserLeave(game_user_item, game_user_item->GetUserStatus() == US_LOOKON);
}

void GameKernel::OnUserItemUpdate(IGameUserItem* game_user_item, tagUserScore& last_score)
{
	if (game_user_item == NULL || delegate_ == NULL)
		return;
	delegate_->OnEventUserScore(game_user_item, game_user_item->GetUserStatus() == US_LOOKON);
}

void GameKernel::OnUserItemUpdate(IGameUserItem* game_user_item, tagUserStatus& last_status)
{
	if (game_user_item == NULL || delegate_ == NULL)
		return;
	delegate_->OnEventUserStatus(game_user_item, game_user_item->GetUserStatus() == US_LOOKON);
}

// 房间配置
void GameKernel::OnGFConfigServer(const UserAttribute& user_attrib, const ServerAttribute& server_attrib)
{
	memcpy(&user_attribute_, &user_attrib, sizeof(user_attribute_));
	memcpy(&server_attribute_, &server_attrib, sizeof(server_attribute_));
}

// 配置完成
void GameKernel::OnGFConfigFinish()
{
	SendGameOption();
}

bool GameKernel::OnGFEventSocket(uint16 main, uint16 sub, void* data, uint16 dataSize)
{
	// 游戏消息
	if (main == MDM_GF_GAME)
	{
		assert(delegate_);
		if (delegate_ == nullptr)
			return false;
		return delegate_->OnEventGameMessage(sub, data, dataSize);
	}

	// 内核处理
	if (main == MDM_GF_FRAME)
	{
		switch (sub)
		{
		case SUB_GF_USER_CHAT:			// 用户聊天
		{
			return true;
		}
		case SUB_GF_GAME_STATUS:		// 游戏状态
		{
			assert(dataSize == sizeof(CMD_GF_GameStatus));
			if (dataSize != sizeof(CMD_GF_GameStatus))
				return false;

			PACKET_HELPER_DATA(data);

			// 设置变量
			game_status_ = packet.readByte();
			allow_lookon_ = packet.readByte() != 0;
			return true;
		}
		case SUB_GF_GAME_SCENE:			// 游戏场景
		{
			if (delegate_ == nullptr)
				return true;
			// 场景处理
			bool lookon_user = (myself_user_item_->GetUserStatus() == US_LOOKON);
			return delegate_->OnEventSceneMessage(game_status_, lookon_user, data, dataSize);
		}
		case SUB_GF_LOOKON_STATUS:		// 旁观状态
		{
			return true;
		}
		case SUB_GF_SYSTEM_MESSAGE:		// 系统消息
		{
			PACKET_HELPER_DATA(data);
			uint16 type = packet.read2Byte();
			uint16 len = packet.read2Byte();
			std::string str = packet.readString(len);
			if (delegate_)
				delegate_->OnSystemMessage(str.c_str(), len, type);
			return true;
		}
		case SUB_GF_USER_READY:			// 用户准备
		{
			if (myself_user_item_ == NULL || myself_user_item_->GetUserStatus() >= US_READY)
				return true;

			SendUserReady();

			return true;
		}
		}

		return true;
	}

	return false;
}

// 用户进入
void GameKernel::OnGFUserEnter(IGameUserItem* user_item)
{
	// 用户判断
	assert(user_item != nullptr);
	if (user_item == nullptr)
		return;

	game_user_manager_->ActiveUserItem(*user_item->GetUserInfo());
}

// 用户积分
void GameKernel::OnGFUserScore(uint32 user_id, const tagUserScore& user_score)
{
	if (delegate_ == nullptr)
		return;

	// 寻找用户
	IGameUserItem* user_item = game_user_manager_->SearchUserItemByUserID(user_id);

	// 用户判断
	assert(user_item != nullptr);
	if (user_item == nullptr)
		return;

	game_user_manager_->UpdateUserItemScore(user_item, &user_score);
}

// 用户状态
void GameKernel::OnGFUserStatus(uint32 user_id, const tagUserStatus& user_status)
{
	if (delegate_ == nullptr)
		return;

	// 寻找用户
	IGameUserItem* user_item = game_user_manager_->SearchUserItemByUserID(user_id);

	// 用户判断
	assert(user_item != nullptr);
	if (user_item == nullptr)
		return;

	if (user_status.cbUserStatus == US_NULL || user_status.cbUserStatus == US_FREE)
	{
		if (user_item == myself_user_item_)
		{
			in_service_ = false;
			allow_lookon_ = false;
			myself_user_item_ = NULL;
			game_status_ = GAME_STATUS_FREE;
			memset(&user_attribute_, 0, sizeof(user_attribute_));
			if (delegate_)
				delegate_->ResetGameClient();
			game_user_manager_->ResetUserManager();
		}
		else
		{
			game_user_manager_->DeleteUserItem(user_item);
		}
		return;
	}

	game_user_manager_->UpdateUserItemStatus(user_item, &user_status);
}
