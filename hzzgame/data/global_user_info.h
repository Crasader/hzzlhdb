
#ifndef GLOBAL_USER_INFO_H_
#define GLOBAL_USER_INFO_H_

#include <string>

#include "../base/basictypes.h"
#include "../../../../global/Define.h"

// ȫ���û�����
struct GlobalUserData
{
	DWORD user_id;	// �û�ID
	DWORD game_id;	// ��ϷID
	WORD face_id;	// �û�ͷ��
	BYTE gender;	// �û��Ա�
	DWORD experience;	// ����ֵ
	BYTE moor_machine;	// ��������
	std::string accounts;	// ��¼�ʺ�
	std::string nickname;	// �û��ǳ�
	std::string password;	// ��¼����

	DWORD ingot;	// �û�Ԫ��
	DWORD ticket;	// �û���ȯ
	int64 user_score;	// �û����
	DWORD user_Medal;	// �û�Ԫ��
	DWORD user_Lottery;	// �û�Ԫ��
	int64 user_insure;	// �û�����

	BYTE member_order;	// ��Ա�ȼ�
	SYSTEMTIME member_over_date;	// ����ʱ��
	std::string signature;	// ����ǩ��
};

// ȫ���û���Ϣ
class GlobalUserInfo
{
public:
	GlobalUserInfo();
	~GlobalUserInfo();

	// ��������
	void ResetUserInfoData();
	// �û�����
	GlobalUserData* GetGlobalUserData() { return &global_user_data_; }

private:
	GlobalUserInfo(const GlobalUserInfo&);
	void operator=(const GlobalUserInfo&);

	GlobalUserData global_user_data_;	// �û�����
};

extern GlobalUserInfo g_global_userinfo;

#endif	// GLOBAL_USER_INFO_H_
