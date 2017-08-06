
#ifndef GAME_KERNEL_H_
#define GAME_KERNEL_H_

#include "../base/basictypes.h"
#include "../../../../global/Define.h"
#include "../../../../global/Packet.h"
#include "../../../../global/struct_mobile.h"
#include "../../../../command/CMD_GameServer.h"
#include "../../../../command/CMD_Commom.h"
#include "game_user_manager.h"

class GameKernel : public GameUserManager::Delegate
{
public:
	class Delegate
	{
	public:
		Delegate() { }
		virtual ~Delegate() { }

		virtual void ResetGameClient() = 0;
		virtual void CloseGameClient() = 0;
		virtual bool OnEventGameMessage(uint16 sub_cmdid, void* data, uint16 data_size) = 0;
		virtual bool OnEventSceneMessage(uint8 game_status, bool lookon_user, void* data, uint16 data_size) = 0;
		virtual void OnEventUserEnter(IGameUserItem* game_user_item, bool lookon_user) = 0;
		virtual void OnEventUserLeave(IGameUserItem* game_user_item, bool lookon_user) = 0;
		virtual void OnEventUserScore(IGameUserItem* game_user_item, bool lookon_user) = 0;
		virtual void OnEventUserStatus(IGameUserItem* game_user_item, bool lookon_user) = 0;
		virtual void OnSystemMessage(const char* sys_msg, uint16 msg_len, uint16 type) = 0;
	};

	static GameKernel* create();
	static void destory();
	static GameKernel* get();

	~GameKernel();

	bool Init(uint16 kind_id, uint16 player_count, const char* game_name);

	//中断
	bool Intermit();
	void SetDelegate(Delegate* delegate);
	bool SetGameAttribute(uint16 kind_id, uint16 player_count, uint32 client_version, const char* game_name);

	const UserAttribute& GetUserAttribute() const { return user_attribute_; }
	const GameAttribute& GetGameAttribute() const { return game_attribute_; }
	const ServerAttribute& GetServerAttribute() const { return server_attribute_; }

	bool IsSingleMode();
	bool IsLookonMode();
	bool IsAllowLookon() const;
	bool IsServiceStatus() const;

	uint8 GetGameStatus() const { return game_status_; }
	void SetGameStatus(uint8 game_status) { game_status_ = game_status; }

	uint16 GetMeChairID() const;
	IGameUserItem* GetMeUserItem() const;
	IGameUserItem* GetTableUserItem(uint16 chair_id);
	const GameUserItemVector& GetLookonUserVector();
	IGameUserItem* SearchUserItemByUserID(uint32 user_id);
	IGameUserItem* SearchUserItemByGameID(uint32 game_id);
	IGameUserItem* SearchUserItemByNickName(const char* nick_name);

	bool SendSocketData(uint16 main_cmdid, uint16 sub_cmdid);
	bool SendSocketData(uint16 main_cmdid, uint16 sub_cmdid, void* data, uint16 data_size);

	bool SendProcessData(uint16 main_cmdid, uint16 sub_cmdid);
	bool SendProcessData(uint16 main_cmdid, uint16 sub_cmdid, void* data, uint16 data_size);
	bool SendProcessCommand(uint16 main_cmdid, uint16 sub_cmdid, void* data, uint16 data_size);

	bool SendGameOption();
	bool SendUserReady();
	bool SendUserLookon(uint32 user_id, bool allow_lookon);
	bool SendUserChatMessage(uint32 target_user_id, const char* message, COLORREF text_color);

	virtual void OnUserItemAcitve(IGameUserItem* game_user_item);
	virtual void OnUserItemDelete(IGameUserItem* game_user_item);
	virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserScore& last_score);
	virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserStatus& last_status);

	// 房间配置
	void OnGFConfigServer(const UserAttribute& user_attrib, const ServerAttribute& server_attrib);
	// 配置完成
	void OnGFConfigFinish();
	// Socket接口
	bool OnGFEventSocket(uint16 main, uint16 sub, void* data, uint16 dataSize);
	// 用户进入
	void OnGFUserEnter(IGameUserItem* user_item);
	// 用户积分
	void OnGFUserScore(uint32 user_id, const tagUserScore& user_score);
	// 用户状态
	void OnGFUserStatus(uint32 user_id, const tagUserStatus& user_status);

private:
	GameKernel();

private:
	bool in_service_;

	bool allow_lookon_;
	uint8 game_status_;

	IGameUserItem* myself_user_item_;
	GameUserManager* game_user_manager_;

	UserAttribute user_attribute_;
	GameAttribute game_attribute_;
	ServerAttribute server_attribute_;

	Delegate* delegate_;
};

#endif  // GAME_KERNEL_H_
