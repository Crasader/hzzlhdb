
#ifndef SERVER_KERNEL_H_
#define SERVER_KERNEL_H_

#include <string>

#include "../base/basictypes.h"
#include "../../../../global/Define.h"
#include "../../../../global/Packet.h"
#include "../../../../global/struct_mobile.h"
#include "../../../../global/ServerRule.h"
#include "../../../../global/RightDefine.h"
#include "../../../../command/CMD_LogonServer.h"
#include "../../../../command/CMD_Commom.h"
#include "../base/convert.h"
#include "../network/packet_helper.h"
#include "game_user_manager.h"
#include "../network/socket_engine.h"
#include "game_table_proxy.h"

class ServerKernel : public SocketEngine::Delegate, public ClientUserManager::Delegate
{
public:
	static ServerKernel* create();
	static void destory();
	static ServerKernel* get();

	virtual ~ServerKernel();

	// 连接网络
	bool Connect(const tagGameKind* game_kind, const tagGameServer* game_server);
	// 关闭连接
	bool Disconnect();
	// 发送数据
	bool Send(uint16 main, uint16 sub, void* data, uint16 size);

public:
	uint16 GetTableCount() const { return table_count_; }
	uint16 GetChairCount() const { return chair_count_; }
	GameTableProxy* GetGameTableProxy() { return game_table_proxy_; }
	// 执行旁观
	bool PerformLookonAction(uint16 table_id, uint16 chair_id);
	// 执行起立
	bool PerformStandUpAction(uint16 table_id, uint16 chair_id);
	// 执行坐下
	bool PerformSitDownAction(uint16 table_id, uint16 chair_id, bool efficacy_pass);
	// 踢人
	bool KickUser(DWORD dwGameID);
	// 踢人
	bool KickAllUser();
	// 禁止进入
	bool ProhibitedEntry();
	// 更换位置
	bool PerformChangeChair();
	bool IsRequestAction() { return (request_table_id_ != INVALID_TABLE || request_chair_id_ != INVALID_CHAIR); }
	IGameUserItem* GetMeUserItem() const { return myself_user_item_; }
	const tagGameKind* GetGameKind() const { return &game_kind_; }
	const tagGameServer* GetGameServer() const { return &game_server_; }
	const uint16 getServerType() const { return server_type_; }

	// 框架消息
public:
	// 游戏已准备好
	void OnGFGameReady();
	// 游戏关闭
	void OnGFGameClose();
	// 发送数据
	bool GFSendData(uint16 main, uint16 sub, void* data, uint16 size);

private:
	ServerKernel();

	virtual void OnSocketConnect();
	virtual void OnSocketClose();
	virtual void OnSocketError(int errorCode);
	virtual bool OnSocketData(uint16 main, uint16 sub, void* data, uint16 dataSize);

	// 用户接口
private:
	// 用户激活
	virtual void OnUserItemAcitve(IGameUserItem* game_user_item);
	// 用户删除
	virtual void OnUserItemDelete(IGameUserItem* game_user_item);
	// 用户更新
	virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserScore& LastScore);
	// 用户更新
	virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserStatus& LastStatus);

	// 网络事件
private:
	// 登录处理
	bool OnSocketMainLogon(uint16 sub_cmd_id, void* data, uint16 data_size);
	// 配置处理
	bool OnSocketMainConfig(uint16 sub_cmd_id, void* data, uint16 data_size);
	// 用户处理
	bool OnSocketMainUser(uint16 sub_cmd_id, void* data, uint16 data_size);
	// 状态处理
	bool OnSocketMainStatus(uint16 sub_cmd_id, void* data, uint16 data_size);
	// 系统消息
	bool OnSocketMainSystem(uint16 sub_cmd_id, void* data, uint16 data_size);
	// 游戏处理
	bool OnSocketMainGameFrame(uint16 main_cmd_id, uint16 sub_cmd_id, void* data, uint16 data_size);

	// 登陆相关
private:
	// 登录成功
	bool OnSocketSubLogonSuccess(void* data, uint16 data_size);
	// 登录失败
	bool OnSocketSubLogonFailure(void* data, uint16 data_size);
	// 登录完成
	bool OnSocketSubLogonFinish(void* data, uint16 data_size);
	// 游戏更新
	bool OnSocketSubUpdateNotify(void* data, uint16 data_size);
	// 失败
	bool OnSocketSubFailure(void* data, uint16 data_size);

	// 用户相关
private:
	// 用户进入
	bool OnSocketSubUserEnter(void* data, uint16 data_size);
	// 用户积分
	bool OnSocketSubUserScore(void* data, uint16 data_size);
	// 用户状态
	bool OnSocketSubUserStatus(void* data, uint16 data_size);
	// 用户聊天
	bool OnSocketSubUserChat(void* data, uint16 data_size);
	// 请求失败
	bool OnSocketSubRequestFailure(void* data, uint16 data_size);
	// 更新分数
	bool OnSocketSubUpdateScore(void* data, uint16 data_size);

private:
	// 发送登录
	bool SendLoginPacket();
	// 发送规则
	bool SendUserRulePacket();
	// 发送旁观
	bool SendLookonPacket(uint16 table_id, uint16 chair_id);
	// 发送坐下
	bool SendSitDownPacket(uint16 table_id, uint16 chair_id, const char* password = NULL);
	// 发送起立
	bool SendStandUpPacket(uint16 table_id, uint16 chair_id, uint8 force_leave);

private:
	// 桌子效验
	bool EfficacyTableRule(uint16 table_id, uint16 chair_id, bool req_lookon, std::string& desc);

private:
	GameTableProxy* game_table_proxy_;
	SocketEngine* socket_engine_;
	// 服务状态
	enum ServiceStatus
	{
		SS_UNKNOW = 0,			// 未知状态
		SS_ENTERING,			// 进入状态
		SS_VALIDATE,			// 验证状态
		SS_RECVINFO,			// 读取状态
		SS_SERVICEING,			// 服务状态
		SS_NETWORKCLOSE			// 中断状态
	};
	ServiceStatus service_status_;
	uint16 server_type_;	// 房间类型
	uint32 server_rule_;	// 房间规则
	tagGameKind game_kind_;
	tagGameServer game_server_;

	uint32 user_right_;	// 用户权限
	uint32 master_right_;	// 管理权限
	IGameUserItem* myself_user_item_;	// 自己

	uint16 table_count_;
	uint16 chair_count_;

	uint16 request_table_id_;
	uint16 request_chair_id_;
	uint16 find_table_id_;

	bool game_ready_;

	ClientUserManager* client_user_manager_;
};

#endif // SERVER_KERNEL_H_