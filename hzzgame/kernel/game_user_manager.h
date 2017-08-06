
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
	// �û���Ϣ
	virtual tagUserInfo* GetUserInfo() = 0;
	// �û�ͷ��
	virtual uint16 GetFaceID() = 0;
	// �û��Ա�
	virtual uint8 GetGender() = 0;
	// �û���ʶ
	virtual uint32 GetUserID() = 0;
	// ��Ϸ��ʶ
	virtual uint32 GetGameID() = 0;
	// �û��ǳ�
	virtual const char* GetNickName() = 0;
	// ����ǩ��
	virtual const char* GetSignature() = 0;
	// ������ֵ
	virtual uint32 GetUserExperience() = 0;
	// ��Ա�ȼ�
	virtual uint8 GetMemberOrder() = 0;
	// ����ȼ�
	virtual uint8 GetMasterOrder() = 0;
	// �û�����
	virtual uint16 GetTableID() = 0;
	// �û�����
	virtual uint16 GetChairID() = 0;
	// �û�״̬
	virtual uint8 GetUserStatus() = 0;
	// ������ֵ
	virtual SCORE GetUserScore() = 0;
	// ������ֵ
	virtual SCORE GetUserInsure() = 0;
	// ʤ������
	virtual uint32 GetUserWinCount() = 0;
	// ʧ������
	virtual uint32 GetUserLostCount() = 0;
	// �;�����
	virtual uint32 GetUserDrawCount() = 0;
	// ��������
	virtual uint32 GetUserFleeCount() = 0;
	// ��Ϸ����
	virtual uint32 GetUserPlayCount() = 0;
	// �û�ʤ��
	virtual float GetUserWinRate() = 0;
	// �û�����
	virtual float GetUserLostRate() = 0;
	// �û�����
	virtual float GetUserDrawRate() = 0;
	// �û�����
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

	// �û���Ϣ
	virtual tagUserInfo* GetUserInfo() { return &user_info_; }
	// ͷ������
	virtual uint16 GetFaceID() { return user_info_.wFaceID; }
	// �û��Ա�
	virtual uint8 GetGender() { return user_info_.cbGender; }
	// �û���ʶ
	virtual uint32 GetUserID() { return user_info_.dwUserID; }
	// ��Ϸ��ʶ
	virtual uint32 GetGameID() { return user_info_.dwGameID; }
	// �û��ǳ�
	virtual const char* GetNickName() { return user_info_.szNickName; }
	// ����ǩ��
	virtual const char* GetSignature() { return user_info_.szSignature; }
	// ������ֵ
	virtual uint32 GetUserExperience() { return user_info_.dwExperience; }
	// ��Ա�ȼ�
	virtual uint8 GetMemberOrder() { return user_info_.cbMemberOrder; }
	// ����ȼ�
	virtual uint8 GetMasterOrder() { return user_info_.cbMasterOrder; }
	// �û�����
	virtual uint16 GetTableID() { return user_info_.wTableID; }
	// �û�����
	virtual uint16 GetChairID() { return user_info_.wChairID; }
	// �û�״̬
	virtual uint8 GetUserStatus() { return user_info_.cbUserStatus; }
	// ������ֵ
	virtual SCORE GetUserScore() { return user_info_.lScore; }
	// �û�Ԫ��
	virtual SCORE GetUserMedal() { return user_info_.dwUserMedal; }
	// �û���ȯ
	virtual SCORE GetUserLottery() { return user_info_.dwUserLottery; }
	// ������ֵ
	virtual SCORE GetUserInsure() { return user_info_.lInsure; }
	// ʤ������
	virtual uint32 GetUserWinCount() { return user_info_.dwWinCount; }
	// ʧ������
	virtual uint32 GetUserLostCount() { return user_info_.dwLostCount; }
	// �;�����
	virtual uint32 GetUserDrawCount() { return user_info_.dwDrawCount; }
	// ��������
	virtual uint32 GetUserFleeCount() { return user_info_.dwFleeCount; }
	// ��Ϸ����
	virtual uint32 GetUserPlayCount() { return user_info_.dwWinCount + user_info_.dwLostCount + user_info_.dwDrawCount + user_info_.dwFleeCount; }
	// �û�ʤ��
	virtual float GetUserWinRate();
	// �û�����
	virtual float GetUserLostRate();
	// �û�����
	virtual float GetUserDrawRate();
	// �û�����
	virtual float GetUserFleeRate();

private:
	tagUserInfo user_info_; // �û���Ϣ
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
		// �û�����
		virtual void OnUserItemAcitve(IGameUserItem* game_user_item) = 0;
		// �û�ɾ��
		virtual void OnUserItemDelete(IGameUserItem* game_user_item) = 0;
		// �û�����
		virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserScore& LastScore) = 0;
		// �û�����
		virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserStatus& LastStatus) = 0;
	};

	ClientUserManager(Delegate* delegate);
	~ClientUserManager();

	bool ResetUserManager();
	// ɾ���û�
	bool DeleteUserItem(IGameUserItem* game_user_item);
	// �����û�
	IGameUserItem* ActiveUserItem(tagUserInfo& user_info);
	// ���»���
	bool UpdateUserItemScore(IGameUserItem* game_user_item, const tagUserScore* user_score);
	// ����״̬
	bool UpdateUserItemStatus(IGameUserItem* game_user_item, const tagUserStatus* user_status);
	// ö���û�
	const GameUserItemVector& GetUserVector() const { return active_user_items_;  }
	// �����û�
	IGameUserItem* SearchUserByUserID(uint32 user_id);
	IGameUserItem* SearchUserByGameID(uint32 game_id);
	IGameUserItem* SearchUserByNickName(const char* nick_name);
	// �������
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
