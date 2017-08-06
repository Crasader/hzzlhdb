
#ifndef GLOBAL_USER_INFO_H_
#define GLOBAL_USER_INFO_H_

#include <string>

#include "../base/basictypes.h"
#include "../../../../global/Define.h"

// 全局用户数据
struct GlobalUserData
{
	DWORD user_id;	// 用户ID
	DWORD game_id;	// 游戏ID
	WORD face_id;	// 用户头像
	BYTE gender;	// 用户性别
	DWORD experience;	// 经验值
	BYTE moor_machine;	// 锁定机器
	std::string accounts;	// 登录帐号
	std::string nickname;	// 用户昵称
	std::string password;	// 登录密码

	DWORD ingot;	// 用户元宝
	DWORD ticket;	// 用户奖券
	int64 user_score;	// 用户金币
	DWORD user_Medal;	// 用户元宝
	DWORD user_Lottery;	// 用户元宝
	int64 user_insure;	// 用户银行

	BYTE member_order;	// 会员等级
	SYSTEMTIME member_over_date;	// 到期时间
	std::string signature;	// 个性签名
};

// 全局用户信息
class GlobalUserInfo
{
public:
	GlobalUserInfo();
	~GlobalUserInfo();

	// 重置资料
	void ResetUserInfoData();
	// 用户资料
	GlobalUserData* GetGlobalUserData() { return &global_user_data_; }

private:
	GlobalUserInfo(const GlobalUserInfo&);
	void operator=(const GlobalUserInfo&);

	GlobalUserData global_user_data_;	// 用户数据
};

extern GlobalUserInfo g_global_userinfo;

#endif	// GLOBAL_USER_INFO_H_
