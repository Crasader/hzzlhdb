
#ifndef GAME_USER_MANAGER_H_
#define GAME_USER_MANAGER_H_
#pragma once

#include <vector>

#include "../base/basictypes.h"
#include "../../../../global/define.h"
#include "../../../../global/struct_mobile.h"

class IGameUserItem
{
public:
	// 用户信息
	virtual tagUserInfo* GetUserInfo() = 0;
	// 用户头像
	virtual uint16 GetFaceID() = 0;
	// 用户性别
	virtual uint8 GetGender() = 0;
	// 用户标识
	virtual uint32 GetUserID() = 0;
	// 游戏标识
	virtual uint32 GetGameID() = 0;
	// 用户昵称
	virtual const char* GetNickName() = 0;
	// 个性签名
	virtual const char* GetSignature() = 0;
	// 经验数值
	virtual uint32 GetUserExperience() = 0;
	// 会员等级
	virtual uint8 GetMemberOrder() = 0;
	// 管理等级
	virtual uint8 GetMasterOrder() = 0;
	// 用户桌子
	virtual uint16 GetTableID() = 0;
	// 用户椅子
	virtual uint16 GetChairID() = 0;
	// 用户状态
	virtual uint8 GetUserStatus() = 0;
	// 积分数值
	virtual SCORE GetUserScore() = 0;
	// 银行数值
	virtual SCORE GetUserInsure() = 0;
	// 胜利盘数
	virtual uint32 GetUserWinCount() = 0;
	// 失败盘数
	virtual uint32 GetUserLostCount() = 0;
	// 和局盘数
	virtual uint32 GetUserDrawCount() = 0;
	// 逃跑盘数
	virtual uint32 GetUserFleeCount() = 0;
	// 游戏局数
	virtual uint32 GetUserPlayCount() = 0;
	// 用户胜率
	virtual float GetUserWinRate() = 0;
	// 用户输率
	virtual float GetUserLostRate() = 0;
	// 用户和率
	virtual float GetUserDrawRate() = 0;
	// 用户逃率
	virtual float GetUserFleeRate() = 0;
};

//------------------------------------------------------------------------------

class GameUserItem : public IGameUserItem
{
	friend class ClientUserManager;
	friend class GameUserManager;
public:
	GameUserItem();
	virtual ~GameUserItem();

	// 用户信息
	virtual tagUserInfo* GetUserInfo() { return &user_info_; }
	// 头像索引
	virtual uint16 GetFaceID() { return user_info_.wFaceID; }
	// 用户性别
	virtual uint8 GetGender() { return user_info_.cbGender; }
	// 用户标识
	virtual uint32 GetUserID() { return user_info_.dwUserID; }
	// 游戏标识
	virtual uint32 GetGameID() { return user_info_.dwGameID; }
	// 用户昵称
	virtual const char* GetNickName() { return user_info_.szNickName; }
	// 个性签名
	virtual const char* GetSignature() { return user_info_.szSignature; }
	// 经验数值
	virtual uint32 GetUserExperience() { return user_info_.dwExperience; }
	// 会员等级
	virtual uint8 GetMemberOrder() { return user_info_.cbMemberOrder; }
	// 管理等级
	virtual uint8 GetMasterOrder() { return user_info_.cbMasterOrder; }
	// 用户桌子
	virtual uint16 GetTableID() { return user_info_.wTableID; }
	// 用户椅子
	virtual uint16 GetChairID() { return user_info_.wChairID; }
	// 用户状态
	virtual uint8 GetUserStatus() { return user_info_.cbUserStatus; }
	// 积分数值
	virtual SCORE GetUserScore() { return user_info_.lScore; }
	// 用户元宝
	virtual SCORE GetUserMedal() { return user_info_.dwUserMedal; }
	// 用户奖券
	virtual SCORE GetUserLottery() { return user_info_.dwUserLottery; }
	// 银行数值
	virtual SCORE GetUserInsure() { return user_info_.lInsure; }
	// 胜利盘数
	virtual uint32 GetUserWinCount() { return user_info_.dwWinCount; }
	// 失败盘数
	virtual uint32 GetUserLostCount() { return user_info_.dwLostCount; }
	// 和局盘数
	virtual uint32 GetUserDrawCount() { return user_info_.dwDrawCount; }
	// 逃跑盘数
	virtual uint32 GetUserFleeCount() { return user_info_.dwFleeCount; }
	// 游戏局数
	virtual uint32 GetUserPlayCount() { return user_info_.dwWinCount + user_info_.dwLostCount + user_info_.dwDrawCount + user_info_.dwFleeCount; }
	// 用户胜率
	virtual float GetUserWinRate();
	// 用户输率
	virtual float GetUserLostRate();
	// 用户和率
	virtual float GetUserDrawRate();
	// 用户逃率
	virtual float GetUserFleeRate();

private:
	tagUserInfo user_info_; // 用户信息
};

typedef std::vector<GameUserItem*> GameUserItemVector;

//------------------------------------------------------------------------------

class ClientUserManager
{
public:
	class Delegate
	{
	public:
		Delegate() { }
		virtual ~Delegate() { }
		// 用户激活
		virtual void OnUserItemAcitve(IGameUserItem* game_user_item) = 0;
		// 用户删除
		virtual void OnUserItemDelete(IGameUserItem* game_user_item) = 0;
		// 用户更新
		virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserScore& LastScore) = 0;
		// 用户更新
		virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserStatus& LastStatus) = 0;
	};

	ClientUserManager(Delegate* delegate);
	~ClientUserManager();

	bool ResetUserManager();
	// 删除用户
	bool DeleteUserItem(IGameUserItem* game_user_item);
	// 增加用户
	IGameUserItem* ActiveUserItem(tagUserInfo& user_info);
	// 更新积分
	bool UpdateUserItemScore(IGameUserItem* game_user_item, const tagUserScore* user_score);
	// 更新状态
	bool UpdateUserItemStatus(IGameUserItem* game_user_item, const tagUserStatus* user_status);
	// 枚举用户
	const GameUserItemVector& GetUserVector() const { return active_user_items_;  }
	// 查找用户
	IGameUserItem* SearchUserByUserID(uint32 user_id);
	IGameUserItem* SearchUserByGameID(uint32 game_id);
	IGameUserItem* SearchUserByNickName(const char* nick_name);
	// 获得人数
	uint32 GetActiveUserCount() { return active_user_items_.size(); }

private:
	GameUserItemVector active_user_items_;
	GameUserItemVector stoarge_user_items_;
	Delegate* delegate_;
};

//------------------------------------------------------------------------------

class GameUserManager
{
public:
	class Delegate
	{
	public:
		Delegate() { }
		virtual ~Delegate() { }
		virtual void OnUserItemAcitve(IGameUserItem* game_user_item) = 0;
		virtual void OnUserItemDelete(IGameUserItem* game_user_item) = 0;
		virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserScore& last_score) = 0;
		virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserStatus& last_status) = 0;
	};

	GameUserManager(Delegate* delegate);
	~GameUserManager();

	bool ResetUserManager();
	bool DeleteUserItem(IGameUserItem* game_user_item);
	IGameUserItem* ActiveUserItem(tagUserInfo& user_info);

	bool UpdateUserItemScore(IGameUserItem* game_user_item, const tagUserScore* user_score);
	bool UpdateUserItemStatus(IGameUserItem* game_user_item, const tagUserStatus* user_status);

	IGameUserItem* GetTableUserItem(uint16 chair_id);
	const GameUserItemVector& GetLookonUserVector() const;

	IGameUserItem* SearchUserItemByUserID(uint32 user_id);
	IGameUserItem* SearchUserItemByGameID(uint32 game_id);
	IGameUserItem* SearchUserItemByNickName(const char* nick_name);

private:
	Delegate* delegate_;
	GameUserItem* game_user_item_[MAX_CHAIR];
	GameUserItemVector lookon_user_vector_;
	GameUserItemVector storage_user_vector_;
};

#endif  // GAME_USER_MANAGER_H_
