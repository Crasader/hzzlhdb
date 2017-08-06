
#include "plaza_kernel.h"

#include "../data/global_user_info.h"

#include "cocos2d.h"
#include "../scene/dialog_msg_box.h"
#include "../scene/dialog_waiting.h"
#include "../scene/CheckIn_scene.h"
#include "../scene/Auction_Scene.h"
#include "../scene/lobby_scene.h"
#include "../scene/Exchange_Scene.h"
#include "../scene/prompt_box.h"

USING_NS_CC;

static PlazaKernel *g_PlazaKernel = nullptr;

PlazaKernel* PlazaKernel::getInstance()
{
	if (!g_PlazaKernel)
	{
		g_PlazaKernel = new (std::nothrow) PlazaKernel();
	}

	return g_PlazaKernel;
}

void PlazaKernel::destroyInstance()
{
	if (g_PlazaKernel)
	{
		delete g_PlazaKernel;
		g_PlazaKernel = nullptr;
	}
}

PlazaKernel::PlazaKernel()
	: bank_delegate_(nullptr)
{
	socket_engine_ = new SocketEngine(this);
}

PlazaKernel::~PlazaKernel()
{
	delete socket_engine_;
}

// ��������
bool PlazaKernel::Connect(const char* url, int port)
{
	return socket_engine_->Connect(url, port);
}

// �ر�����
bool PlazaKernel::Disconnect()
{
	return socket_engine_->Disconnect();
}

// ��������
bool PlazaKernel::Send(uint16 main, uint16 sub, void* data, uint16 size)
{
	return socket_engine_->Send(main, sub, data, size);
}

const GameServerVector& PlazaKernel::GetGameServerList(uint16 kindid)
{
	static GameServerVector servervector;
	GameServerMap::iterator it = game_server_list_.find(kindid);
	if (it != game_server_list_.end())
	{
		return it->second;
	}

	return servervector;
}

const tagGameKind* PlazaKernel::GetGameKind(uint16 kindid)
{
	for (GameKindVector::iterator it = game_kind_list_.begin(); it != game_kind_list_.end(); ++it)
	{
		tagGameKind* game_kind = *it;
		if (game_kind->wKindID == kindid)
			return game_kind;
	}
	return nullptr;
}

void PlazaKernel::OnSocketConnect()
{
	CCLOG("PlazaKernel::OnSocketConnect");
}

void PlazaKernel::OnSocketClose()
{
	CCLOG("PlazaKernel::OnSocketClose");
}

void PlazaKernel::OnSocketError(int errorCode)
{
	CCLOG("PlazaKernel::OnSocketError:%d", errorCode);
}

bool PlazaKernel::OnSocketData(uint16 main, uint16 sub, void* data, uint16 data_size)
{
	if (main == MDM_GP_LOGON)
	{
		switch (sub)
		{
		case SUB_GP_LOGON_SUCCESS:
			return OnSocketSubLogonSuccess(data, data_size);
		case SUB_GP_LOGON_FAILURE:
			return OnSocketSubLogonFailure(data, data_size);
		case SUB_GP_LOGON_FINISH:
			return OnSocketSubLogonFinish(data, data_size);
		}
	}
	else if (main == MDM_GP_SERVER_LIST)
	{
		switch (sub)
		{
		case SUB_GP_LIST_TYPE:
			return OnSocketSubListType(data, data_size);
		case SUB_GP_LIST_KIND:
			return OnSocketSubListKind(data, data_size);
		case SUB_GP_LIST_SERVER:
			return OnSocketSubListServer(data, data_size);
		case SUB_GP_LIST_FINISH:
			return OnSocketSubListFinish(data, data_size);
		case SUB_GP_ONLINE_COUNT:
			return OnSocketSubOnlineCount(data, data_size);
		}
	}
	else if (main == MDM_GP_BANK_SERVICE)
	{
		if (bank_delegate_)
			bank_delegate_->OnSocketBankService(sub, data, data_size);
		LobbyScene::getInstance()->UpdateUserScore();
		return true;
	}
	else if (main == MDM_GP_USER_SERVICE)
	{
		switch (sub)
		{
		case SUB_GP_USERINFO_RESULT:

			return OnSocketSubUserInfoResult(data, data_size);
		case SUB_GP_USER_FAILURE:
			return OnSocketSubUserFailure(data, data_size);
		case SUB_GP_CHECKIN_INFO:
		case SUB_GP_CHECKIN_RESULT:
			return CheckInScene::getInstance()->OnSocketCheckIn(sub, data, data_size);
		case SUB_GP_AUCTION_INFO:
		case SUB_GP_SEARCH_RESULT:
		case SUB_GP_MY_AUCTION_INFO:
			return AuctionScene::getInstance()->OnSocketAuction(sub, data, data_size);
		case SUB_GP_UPDATE_SCORE:
		{
			// Ч�����
			assert(data_size == sizeof(CMD_GP_UpdateScore));
			if (data_size < sizeof(CMD_GP_UpdateScore))
				return false;

			//��ȡ����
			CMD_GP_UpdateScore * pUpdateScore = (CMD_GP_UpdateScore *)data;

			// �����û�����
			GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
			user_data->user_score = pUpdateScore->lUserScore;
			user_data->user_Medal = pUpdateScore->dwUserMedal;
			user_data->user_Lottery = pUpdateScore->dwUserLottery;

			LobbyScene::getInstance()->UpdateUserScore();
			AuctionScene::getInstance()->updateControl();

			switch (pUpdateScore->nReturnType)
			{
			case 1:
				MsgBox(a2u("��ϲ�������������ɹ���"));
				AuctionScene::getInstance()->GetAuctionInfo();
				break;
			case 2:
				MsgBox(a2u("��ϲ��������ɹ���"));
				AuctionScene::getInstance()->GetAuctionInfo();
				break;
			case 3:
				MsgBox(a2u("��ϲ����������ɣ�"));
				AuctionScene::getInstance()->GetMyAuctionInfo();
				break;
			case 4:
				MsgBox(a2u("��ϲ����������ɣ�"));
				AuctionScene::getInstance()->GetMyAuctionInfo();
				break;
			default:
				break;
			}
			return true;
		}
		case SUB_GP_RANKING_RESULT:
			if (ranking_delegate_)
			{
				return ranking_delegate_->OnSocketRankingResult(sub, data, data_size);
			}
		case SUB_GP_EXCHANGE_INFO:
			return ExchangeScene::getInstance()->OnSocketExchange(sub, data, data_size);
		case SUB_GP_EXCHANGE_RESULT:
		{
			// Ч�����
			assert(data_size == sizeof(CMD_GP_ExchangeResult));
			if (data_size < sizeof(CMD_GP_ExchangeResult))
				return false;

			// ��Ϣ����
			PACKET_HELPER_DATA(data);

			// �����û�����
			GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
			packet.read4Byte();
			user_data->user_Medal = packet.read4Byte();
			user_data->user_Lottery = packet.read4Byte();
			LobbyScene::getInstance()->UpdateUserScore();
			ExchangeScene::getInstance()->updateControl();

			std::string strOrderID = packet.readString(LEN_ACCOUNTS);;
			_PromptBox(strOrderID);

			return true;
		}
		case SUB_GP_FENXIANG_RESULT:
		{
			// Ч�����
			assert(data_size == sizeof(CMD_GP_FenxiangResult));
			if (data_size < sizeof(CMD_GP_FenxiangResult))
				return false;

			//��ȡ����
			CMD_GP_FenxiangResult * pFenxiangResult = (CMD_GP_FenxiangResult *)data;

			// �����û�����
			GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();

			user_data->user_Medal = pFenxiangResult->dwUserMedal;
			user_data->user_Lottery = pFenxiangResult->dwUserLottery;
			//user_data->dwCard = pFenxiangResult->dwCard;

			LobbyScene::getInstance()->UpdateUserScore();

			return true;
		}
		}
	}
	else if (main == MDM_CM_SYSTEM)
	{
		switch (sub)
		{
		case SUB_CM_MARQUEE_MESSAGE:
			return OnSocketSubMarqueeMessage(data, data_size);
		}
	}

	return false;
}

// ��¼�ɹ�
bool PlazaKernel::OnSocketSubLogonSuccess(void* data, uint16 data_size)
{
	// Ч������
	assert(data_size >= sizeof(CMD_GP_LogonSuccess));
	if (data_size < sizeof(CMD_GP_LogonSuccess))
		return false;

	// ��Ϣ����
	PACKET_HELPER_DATA(data);

	// �����û�����
	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	user_data->face_id = packet.read2Byte();
	user_data->gender = packet.readByte();
	user_data->moor_machine = packet.readByte();
	user_data->user_id = packet.read4Byte();
	user_data->game_id = packet.read4Byte();
	user_data->experience = packet.read4Byte();
	user_data->ingot = packet.read4Byte();
	user_data->ticket = packet.read4Byte();
	user_data->user_score = packet.read8Byte();
	user_data->user_Medal = packet.read4Byte();
	user_data->user_Lottery = packet.read4Byte();
	user_data->user_insure = packet.read8Byte();	
	user_data->accounts = packet.readString(LEN_ACCOUNTS);
	user_data->nickname = packet.readString(LEN_NICKNAME);

	uint32 unread_msg_count = packet.read4Byte(); // δ����Ϣ����

	uint8 cbFirstLoginOfDay = packet.readByte(); //һ���1�ε�½
	//�ϴε�½����
	packet.read2Byte();
	packet.read2Byte();
	packet.read2Byte();
	packet.read2Byte();
	packet.read2Byte();
	packet.read2Byte();
	packet.read2Byte();
	packet.read2Byte();

	BYTE cbPerfectData = packet.readByte(); // ��������

	while (data_size > packet.getPosition())
	{
		// ��ȡ����
		uint16 describe_size = packet.read2Byte();
		uint16 describe = packet.read2Byte();
		if (describe == 0) break;

		// ��ȡ����
		switch (describe)
		{
		case DTP_GP_MEMBER_INFO:	// ��Ա��Ϣ
		{
			user_data->member_order = packet.readByte();
			user_data->member_over_date.wYear = packet.read2Byte();
			user_data->member_over_date.wMonth = packet.read2Byte();
			user_data->member_over_date.wDayOfWeek = packet.read2Byte();
			user_data->member_over_date.wDay = packet.read2Byte();
			user_data->member_over_date.wHour = packet.read2Byte();
			user_data->member_over_date.wMinute = packet.read2Byte();
			user_data->member_over_date.wSecond = packet.read2Byte();
			user_data->member_over_date.wMilliseconds = packet.read2Byte();

			break;
		}
		case DTP_GP_SIGNATURE:	// ����ǩ��
		{
			user_data->signature = packet.readString(describe_size / 2);
			break;
		}
		}
	}

	// TODO:��������

	// TODO:�����½��Ϣ���ļ�

	return true;
}

// ��¼ʧ��
bool PlazaKernel::OnSocketSubLogonFailure(void* data, uint16 data_size)
{
	// Ч������

	PACKET_HELPER_DATA(data);
	packet.read4Byte();
	MsgBox(packet.readString((data_size - 4) / 2));

	socket_engine_->Disconnect();
	RemoveDlgWaiting();

	return true;
}

// ��¼���
bool PlazaKernel::OnSocketSubLogonFinish(void* data, uint16 data_size)
{
	// Ч�����
	assert(data_size == sizeof(CMD_GP_LogonFinish));
	if (data_size != sizeof(CMD_GP_LogonFinish))
		return false;
	
	PACKET_HELPER_DATA(data);
	uint32 online_count = packet.read4Byte();

	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOBBY_SCENE");

	return true;
}

// �б�����
bool PlazaKernel::OnSocketSubListType(void* data, uint16 data_size)
{
	// Ч�����
	uint16 item_size = 2 + 2 + LEN_TYPE * 2;
	assert(data_size % item_size == 0);
	if (data_size % item_size != 0)
		return false;

	uint16 item_count = data_size / item_size;

	return true;
}

// �б�����
bool PlazaKernel::OnSocketSubListKind(void* data, uint16 data_size)
{
	// Ч�����
	uint16 item_size = 2 + 2 + 2 + 4 + 4 + LEN_KIND * 2 + LEN_PROCESS * 2;
	assert(data_size % item_size == 0);
	if (data_size % item_size != 0)
		return false;

	WORD item_count = data_size / item_size;
	PACKET_HELPER_DATA(data);
	std::string str;
	for (uint16 i = 0; i < item_count; i++)
	{
		tagGameKind* game_kind = new tagGameKind();
		game_kind->wTypeID = packet.read2Byte();
		game_kind->wSortID = packet.read2Byte();
		game_kind->wKindID = packet.read2Byte();
		game_kind->dwOnLineCount = packet.read4Byte();
		game_kind->dwFullCount = packet.read4Byte();
		str = packet.readString(LEN_KIND);
		strncpy(game_kind->szKindName, str.c_str(), arraysize(game_kind->szKindName));
		str = packet.readString(LEN_PROCESS);
		strncpy(game_kind->szProcessName, str.c_str(), arraysize(game_kind->szProcessName));
		game_kind_list_.push_back(game_kind);
	}

	return true;
}

// �б���
bool PlazaKernel::OnSocketSubListServer(void* data, uint16 data_size)
{
	// Ч�����
	uint16 item_size = 2 + 2 + 2 + 2 + 4 + 4 + 32 * 2 + LEN_SERVER * 2 + 8 + 8 + 1 + 8 + 10 * 2 + 192 * 2;
	assert(data_size % item_size == 0);
	if (data_size % item_size != 0)
		return false;

	WORD item_count = data_size / item_size;
	PACKET_HELPER_DATA(data);
	std::string str;
	for (uint16 i = 0; i < item_count; i++)
	{
		tagGameServer* game_server = new tagGameServer();
		game_server->wKindID = packet.read2Byte();
		game_server->wSortID = packet.read2Byte();
		game_server->wServerID = packet.read2Byte();
		game_server->wServerPort = packet.read2Byte();
		game_server->dwOnLineCount = packet.read4Byte();
		game_server->dwFullCount = packet.read4Byte();

		str = packet.readString(32);
		strncpy(game_server->szServerAddr, str.c_str(), arraysize(game_server->szServerAddr));

		str = packet.readString(LEN_SERVER);
		strncpy(game_server->szServerName, str.c_str(), arraysize(game_server->szServerName));

		game_server->lCellScore = packet.read8Byte();
		game_server->lMinEnterScore = packet.read8Byte();
		game_server->cbServerLevel = packet.readByte();

		game_server->lMatchCost = packet.read8Byte();

		str = packet.readString(10);
		strncpy(game_server->szStartTime, str.c_str(), arraysize(game_server->szStartTime));
		
		str = packet.readString(32);
		strncpy(game_server->szPrize[0], str.c_str(), arraysize(game_server->szPrize[0]));
		str = packet.readString(32);
		strncpy(game_server->szPrize[1], str.c_str(), arraysize(game_server->szPrize[1]));
		str = packet.readString(32);
		strncpy(game_server->szPrize[2], str.c_str(), arraysize(game_server->szPrize[2]));
		str = packet.readString(32);
		strncpy(game_server->szPrize[3], str.c_str(), arraysize(game_server->szPrize[3]));
		str = packet.readString(32);
		strncpy(game_server->szPrize[4], str.c_str(), arraysize(game_server->szPrize[4]));
		str = packet.readString(32);
		strncpy(game_server->szPrize[5], str.c_str(), arraysize(game_server->szPrize[5]));

		//for (int i = 0; i < 6; i++)
		//{
		//	MsgBox(game_server->szPrize[i]);
		//}

		GameServerMap::iterator it = game_server_list_.find(game_server->wKindID);
		if (it != game_server_list_.end())
		{
			it->second.push_back(game_server);
		}
		else
		{
			GameServerVector game_server_vector;
			game_server_vector.push_back(game_server);
			game_server_list_.insert(std::make_pair(game_server->wKindID, game_server_vector));
		}
	}

	return true;
}

// �б����
bool PlazaKernel::OnSocketSubListFinish(void* data, uint16 data_size)
{
	return true;
}

bool PlazaKernel::OnSocketSubOnlineCount(void* data, uint16 data_size)
{
	// Ч�����
	assert(data_size == sizeof(CMD_GP_OnlineCount));
	if (data_size != sizeof(CMD_GP_OnlineCount))
		return false;

	// TODO:������������

	return true;
}

// ���½��
bool PlazaKernel::OnSocketSubUserInfoResult(void* data, uint16 data_size)
{
	// Ч�����
	assert(data_size == sizeof(CMD_GP_UserInfoResult));
	if (data_size < sizeof(CMD_GP_UserInfoResult))
		return false;

	CMD_GP_UserInfoResult * pUserInfoResult = (CMD_GP_UserInfoResult *)data;
	// TODO:�����û���Ϣ
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	pGlobalUserData->user_score = pUserInfoResult->lUserScore;
	pGlobalUserData->user_Medal = pUserInfoResult->dwUserMedal;
	pGlobalUserData->user_Lottery = pUserInfoResult->dwUserLottery;
	LobbyScene::getInstance()->UpdateUserScore();

	return true;
}

// �û�ʧ��
bool PlazaKernel::OnSocketSubUserFailure(void* data, uint16 data_size)
{
	// Ч�����
	CMD_GP_LogonFailure* pLogonFailure = (CMD_GP_LogonFailure*)data;
	assert(data_size >= (sizeof(CMD_GP_LogonFailure) - sizeof(pLogonFailure->szDescribeString)));
	if (data_size < sizeof(CMD_GP_LogonFailure) - sizeof(pLogonFailure->szDescribeString))
		return false;

	PACKET_HELPER_DATA(data);
	auto result = packet.read4Byte();

	// ��ʾ��Ϣ
	std::string describe = packet.readString(128);
	if (describe.size() > 0)
	{
		MsgBox(describe);
	}

	return true;
}

// �������Ϣ
bool PlazaKernel::OnSocketSubMarqueeMessage(void* data, uint16 data_size)
{
	return true;
}
