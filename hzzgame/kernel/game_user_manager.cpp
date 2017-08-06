
#include "game_user_manager.h"

#include <iterator>

GameUserItem::GameUserItem()
{
	memset(&user_info_, 0, sizeof(user_info_));
	user_info_.wTableID = INVALID_TABLE;
	user_info_.wChairID = INVALID_CHAIR;
}

GameUserItem::~GameUserItem()
{
}

// 用户胜率
float GameUserItem::GetUserWinRate()
{
	uint32 count = GetUserPlayCount();
	if (count != 0)
		return (float)(GetUserWinCount() * 100.0f / (float)count);
	return 0.0f;
}

// 用户输率
float GameUserItem::GetUserLostRate()
{
	uint32 count = GetUserPlayCount();
	if (count != 0)
		return (float)(GetUserLostCount() * 100.0f / (float)count);
	return 0.0f;
}

// 用户和率
float GameUserItem::GetUserDrawRate()
{
	uint32 count = GetUserPlayCount();
	if (count != 0)
		return (float)(GetUserDrawCount() * 100.0f / (float)count);
	return 0.0f;
}

// 用户逃率
float GameUserItem::GetUserFleeRate()
{
	uint32 count = GetUserPlayCount();
	if (count != 0)
		return (float)(GetUserFleeCount() * 100.0f / (float)count);
	return 0.0f;
}

//------------------------------------------------------------------------------

ClientUserManager::ClientUserManager(Delegate* delegate)
	: delegate_(delegate)
{
}

ClientUserManager::~ClientUserManager()
{
	GameUserItemVector::iterator it;
	for (it = active_user_items_.begin(); it != active_user_items_.end(); ++it)
	{
		delete *it;
	}
	active_user_items_.clear();
	for (it = stoarge_user_items_.begin(); it != stoarge_user_items_.end(); ++it)
	{
		delete *it;
	}
	stoarge_user_items_.clear();
}

bool ClientUserManager::ResetUserManager()
{
	std::copy(active_user_items_.begin(), active_user_items_.end(), std::back_inserter(stoarge_user_items_));
	active_user_items_.clear();

	return true;
}

// 删除用户
bool ClientUserManager::DeleteUserItem(IGameUserItem* game_user_item)
{
	GameUserItem* user_item;
	for (GameUserItemVector::iterator it = active_user_items_.begin(); it != active_user_items_.end(); ++it)
	{
		user_item = *it;
		if (game_user_item == user_item)
		{
			active_user_items_.erase(it);
			stoarge_user_items_.push_back(user_item);
			delegate_->OnUserItemDelete(user_item);
			memset(&user_item->user_info_, 0, sizeof(tagUserInfo));
			return true;
		}
	}

	assert(0);
	return false;
}

// 增加用户
IGameUserItem* ClientUserManager::ActiveUserItem(tagUserInfo& user_info)
{
	GameUserItem* user_item = NULL;
	if (!stoarge_user_items_.empty())
	{
		user_item = stoarge_user_items_.back();
		stoarge_user_items_.pop_back();
	}
	else
	{
		user_item = new GameUserItem();
	}
	active_user_items_.push_back(user_item);
	memcpy(&user_item->user_info_, &user_info, sizeof(user_info));
	delegate_->OnUserItemAcitve(user_item);
	return user_item;
}

// 更新积分
bool ClientUserManager::UpdateUserItemScore(IGameUserItem* game_user_item, const tagUserScore* user_score)
{
	tagUserInfo* user_info = game_user_item->GetUserInfo();

	// 以往数据
	tagUserScore last_user_score;
	last_user_score.lScore = user_info->lScore;
	last_user_score.dwUserMedal = user_info->dwUserMedal;
	last_user_score.dwUserLottery = user_info->dwUserLottery;
	last_user_score.lInsure = user_info->lInsure;
	last_user_score.dwWinCount = user_info->dwWinCount;
	last_user_score.dwLostCount = user_info->dwLostCount;
	last_user_score.dwDrawCount = user_info->dwDrawCount;
	last_user_score.dwFleeCount = user_info->dwFleeCount;
	last_user_score.dwExperience = user_info->dwExperience;

	// 设置数据
	user_info->lScore = user_score->lScore;
	user_info->dwUserMedal = user_score->dwUserMedal;
	user_info->dwUserLottery = user_score->dwUserLottery;
	user_info->lInsure = user_score->lInsure;
	user_info->dwWinCount = user_score->dwWinCount;
	user_info->dwLostCount = user_score->dwLostCount;
	user_info->dwDrawCount = user_score->dwDrawCount;
	user_info->dwFleeCount = user_score->dwFleeCount;
	user_info->dwExperience = user_score->dwExperience;

	// 通知更新
	delegate_->OnUserItemUpdate(game_user_item, last_user_score);

	return true;
}

// 更新状态
bool ClientUserManager::UpdateUserItemStatus(IGameUserItem* game_user_item, const tagUserStatus* user_status)
{
	// 获取用户
	tagUserInfo* user_info = game_user_item->GetUserInfo();

	// 以往数据
	tagUserStatus last_user_status;
	last_user_status.wTableID = user_info->wTableID;
	last_user_status.wChairID = user_info->wChairID;
	last_user_status.cbUserStatus = user_info->cbUserStatus;

	// 设置数据
	user_info->wTableID = user_status->wTableID;
	user_info->wChairID = user_status->wChairID;
	user_info->cbUserStatus = user_status->cbUserStatus;

	// 通知更新
	delegate_->OnUserItemUpdate(game_user_item, last_user_status);

	return true;
}

// 查找用户
IGameUserItem* ClientUserManager::SearchUserByUserID(uint32 user_id)
{
	for (GameUserItemVector::iterator it = active_user_items_.begin(); it != active_user_items_.end(); ++it)
	{
		if ((*it)->user_info_.dwUserID == user_id)
			return *it;
	}
	return NULL;
}

IGameUserItem* ClientUserManager::SearchUserByGameID(uint32 game_id)
{
	for (GameUserItemVector::iterator it = active_user_items_.begin(); it != active_user_items_.end(); ++it)
	{
		if ((*it)->user_info_.dwGameID == game_id)
			return *it;
	}
	return NULL;
}

IGameUserItem* ClientUserManager::SearchUserByNickName(const char* nick_name)
{
	for (GameUserItemVector::iterator it = active_user_items_.begin(); it != active_user_items_.end(); ++it)
	{
		if (strcmp((*it)->user_info_.szNickName, nick_name) == 0)
			return *it;
	}
	return NULL;
}

//------------------------------------------------------------------------------

GameUserManager::GameUserManager(Delegate* delegate)
	: delegate_(delegate)
{
	memset(game_user_item_, 0, sizeof(game_user_item_));
}

GameUserManager::~GameUserManager()
{
	GameUserItemVector::iterator it;
	for (it = lookon_user_vector_.begin(); it != lookon_user_vector_.end(); ++it)
	{
		delete (*it);
	}
	lookon_user_vector_.clear();
	for (uint16 i = 0; i < MAX_CHAIR; ++i)
	{
		if (game_user_item_[i] != NULL)
			delete game_user_item_[i];
	}
	for (it = storage_user_vector_.begin(); it != storage_user_vector_.end(); ++it)
	{
		delete (*it);
	}
	storage_user_vector_.clear();
}

bool GameUserManager::ResetUserManager()
{
	std::copy(lookon_user_vector_.begin(), lookon_user_vector_.end(), std::back_inserter(storage_user_vector_));
	lookon_user_vector_.clear();

	for (uint16 i = 0; i < MAX_CHAIR; ++i)
	{
		if (game_user_item_[i] != NULL)
			storage_user_vector_.push_back(game_user_item_[i]);
	}
	memset(game_user_item_, 0, sizeof(game_user_item_));
	return true;
}

bool GameUserManager::DeleteUserItem(IGameUserItem* game_user_item)
{
	if (game_user_item == NULL || game_user_item->GetChairID() >= MAX_CHAIR)
		return false;

	if (game_user_item->GetUserStatus() != US_LOOKON)
	{
		uint16 chair_id = game_user_item->GetChairID();
		GameUserItem* game_user_item_remove = static_cast<GameUserItem*>(game_user_item);
		if (game_user_item_[chair_id] != game_user_item)
			return false;

		game_user_item_[chair_id] = NULL;
		storage_user_vector_.push_back(game_user_item_remove);

		if (delegate_ != NULL)
			delegate_->OnUserItemDelete(game_user_item);

		memset(&game_user_item_remove->user_info_, 0, sizeof(game_user_item_remove->user_info_));

		return true;
	}

	GameUserItem* game_user_item_active = NULL;
	GameUserItemVector::iterator it;
	for (it = lookon_user_vector_.begin(); it != lookon_user_vector_.end(); ++it)
	{
		game_user_item_active = *it;
		if (game_user_item_active == game_user_item)
		{
			lookon_user_vector_.erase(it);
			storage_user_vector_.push_back(game_user_item_active);

			if (delegate_ != NULL)
				delegate_->OnUserItemDelete(game_user_item);

			memset(&game_user_item_active->user_info_, 0, sizeof(game_user_item_active->user_info_));
			return true;
		}
	}

	return false;
}

IGameUserItem* GameUserManager::ActiveUserItem(tagUserInfo& user_info)
{
	if (user_info.wChairID >= MAX_CHAIR)
		return NULL;

	GameUserItem* game_user_item = NULL;
	if (storage_user_vector_.size() > 0)
	{
		game_user_item = storage_user_vector_.back();
		storage_user_vector_.pop_back();
	}
	else
	{
		game_user_item = new GameUserItem();
		if (game_user_item == NULL)
			return NULL;
	}

	if (user_info.cbUserStatus == US_LOOKON)
	{
		lookon_user_vector_.push_back(game_user_item);
	}
	else
	{
		game_user_item_[user_info.wChairID] = game_user_item;
	}

	memcpy(&game_user_item->user_info_, &user_info, sizeof(game_user_item->user_info_));

	if (delegate_ != NULL)
		delegate_->OnUserItemAcitve(static_cast<IGameUserItem*>(game_user_item));

	return game_user_item;
}

bool GameUserManager::UpdateUserItemScore(IGameUserItem* game_user_item, const tagUserScore* user_score)
{
	tagUserInfo* user_info = game_user_item->GetUserInfo();

	tagUserScore last_score;
	last_score.lScore = user_info->lScore;
	last_score.dwUserMedal = user_info->dwUserMedal;
	last_score.dwUserLottery = user_info->dwUserLottery;
	last_score.lInsure = user_info->lInsure;
	last_score.dwWinCount = user_info->dwWinCount;
	last_score.dwLostCount = user_info->dwLostCount;
	last_score.dwDrawCount = user_info->dwDrawCount;
	last_score.dwFleeCount = user_info->dwFleeCount;
	last_score.dwExperience = user_info->dwExperience;

	user_info->lScore = user_score->lScore;
	user_info->dwUserMedal = user_score->dwUserMedal;
	user_info->dwUserLottery = user_score->dwUserLottery;
	user_info->lInsure = user_score->lInsure;
	user_info->dwWinCount = user_score->dwWinCount;
	user_info->dwLostCount = user_score->dwLostCount;
	user_info->dwDrawCount = user_score->dwDrawCount;
	user_info->dwFleeCount = user_score->dwFleeCount;
	user_info->dwExperience = user_score->dwExperience;

	if (delegate_ != NULL)
		delegate_->OnUserItemUpdate(game_user_item, last_score);

	return true;
}

bool GameUserManager::UpdateUserItemStatus(IGameUserItem* game_user_item, const tagUserStatus* user_status)
{
	tagUserInfo* user_info = game_user_item->GetUserInfo();

	tagUserStatus last_status;
	last_status.wTableID = user_info->wTableID;
	last_status.wChairID = user_info->wChairID;
	last_status.cbUserStatus = user_info->cbUserStatus;

	user_info->wTableID = user_status->wTableID;
	user_info->wChairID = user_status->wChairID;
	user_info->cbUserStatus = user_status->cbUserStatus;

	if (delegate_ != NULL)
		delegate_->OnUserItemUpdate(game_user_item, last_status);

	return true;
}

IGameUserItem* GameUserManager::GetTableUserItem(uint16 chair_id)
{
	if (chair_id >= arraysize(game_user_item_))
		return NULL;

	return game_user_item_[chair_id];
}

const GameUserItemVector& GameUserManager::GetLookonUserVector() const
{
	return lookon_user_vector_;
}

IGameUserItem* GameUserManager::SearchUserItemByUserID(uint32 user_id)
{
	GameUserItem* game_user_item;
	for (uint16 i = 0; i < MAX_CHAIR; ++i)
	{
		game_user_item = game_user_item_[i];
		if (game_user_item != NULL && game_user_item->GetUserID() == user_id)
			return game_user_item;
	}

	GameUserItemVector::iterator it;
	for (it = lookon_user_vector_.begin(); it != lookon_user_vector_.end(); ++it)
	{
		game_user_item = *it;
		if (game_user_item->GetUserID() == user_id)
			return game_user_item;
	}

	return NULL;
}

IGameUserItem* GameUserManager::SearchUserItemByGameID(uint32 game_id)
{
	GameUserItem* game_user_item;
	for (uint16 i = 0; i < MAX_CHAIR; ++i)
	{
		game_user_item = game_user_item_[i];
		if (game_user_item != NULL && game_user_item->GetGameID() == game_id)
			return game_user_item;
	}

	GameUserItemVector::iterator it;
	for (it = lookon_user_vector_.begin(); it != lookon_user_vector_.end(); ++it)
	{
		game_user_item = *it;
		if (game_user_item->GetGameID() == game_id)
			return game_user_item;
	}

	return NULL;
}

IGameUserItem* GameUserManager::SearchUserItemByNickName(const char* nick_name)
{
	GameUserItem* game_user_item;
	for (uint16 i = 0; i < MAX_CHAIR; ++i)
	{
		game_user_item = game_user_item_[i];
		if (game_user_item != NULL && !strcmp(nick_name, game_user_item->GetNickName()))
			return game_user_item;
	}

	GameUserItemVector::iterator it;
	for (it = lookon_user_vector_.begin(); it != lookon_user_vector_.end(); ++it)
	{
		game_user_item = *it;
		if (!strcmp(nick_name, game_user_item->GetNickName()))
			return game_user_item;
	}

	return NULL;
}
