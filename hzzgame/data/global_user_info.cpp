
#include "global_user_info.h"

GlobalUserInfo g_global_userinfo;

GlobalUserInfo::GlobalUserInfo()
{
	ResetUserInfoData();
}

GlobalUserInfo::~GlobalUserInfo()
{
}

void GlobalUserInfo::ResetUserInfoData()
{
	global_user_data_.user_id = 0;
	global_user_data_.game_id = 0;
	global_user_data_.face_id = 0;
	global_user_data_.gender = 0;
	global_user_data_.experience = 0;
	global_user_data_.moor_machine = 0;
	global_user_data_.accounts.clear();
	global_user_data_.nickname.clear();
	global_user_data_.password.clear();
	global_user_data_.ingot = 0;
	global_user_data_.ticket = 0;
	global_user_data_.user_score = 0;
	global_user_data_.user_Medal = 0;
	global_user_data_.user_Lottery = 0;
	global_user_data_.user_insure = 0;
	global_user_data_.member_order = 0;
	memset(&global_user_data_.member_over_date, 0, sizeof(global_user_data_.member_over_date));
	global_user_data_.signature.empty();
}
