
#ifndef PLAZA_KERNEL_H_
#define PLAZA_KERNEL_H_

#include <string>
#include <vector>
#include <map>

#include "../base/basictypes.h"
#include "../../../../global/Define.h"
#include "../../../../global/Packet.h"
#include "../../../../global/struct_mobile.h"
#include "../../../../command/CMD_LogonServer.h"
#include "../../../../command/CMD_Commom.h"
#include "../base/convert.h"
#include "../network/socket_engine.h"
#include "../network/packet_helper.h"

typedef std::vector<tagGameKind*> GameKindVector;
typedef std::vector<tagGameServer*> GameServerVector;
typedef std::map<uint16, GameServerVector> GameServerMap;

class PlazaKernel : public SocketEngine::Delegate
{
public:
	class BankDelegate
	{
	public:
		BankDelegate() { }
		virtual ~BankDelegate() { }

		virtual bool OnSocketBankService(uint16 sub_cmd_id, void* data, uint16 data_size) = 0;
	};
	class RankingDelegate
	{
	public:
		RankingDelegate(){}
		virtual ~RankingDelegate() {}
		virtual bool OnSocketRankingResult(uint16 sub_cmd_id, void* data, uint16 data_size) = 0;
	};

	static PlazaKernel* getInstance();
	static void destroyInstance();

	virtual ~PlazaKernel();

	// 连接网络
	bool Connect(const char* url, int port);
	// 关闭连接
	bool Disconnect();
	// 发送数据
	bool Send(uint16 main, uint16 sub, void* data, uint16 size);

	const GameKindVector& game_kind_list() const { return game_kind_list_; }
	const GameServerVector& GetGameServerList(uint16 kindid);
	const tagGameKind* GetGameKind(uint16 kindid);

	void set_bank_delegate(BankDelegate* delegate) { bank_delegate_ = delegate; }
	void set_ranking_delegate(RankingDelegate* delegate) { ranking_delegate_ = delegate; }

private:
	PlazaKernel();

	virtual void OnSocketConnect();
	virtual void OnSocketClose();
	virtual void OnSocketError(int errorCode);
	virtual bool OnSocketData(uint16 main, uint16 sub, void* data, uint16 dataSize);

	// 登陆相关
private:
	// 登录成功
	bool OnSocketSubLogonSuccess(void* data, uint16 data_size);
	// 登录失败
	bool OnSocketSubLogonFailure(void* data, uint16 data_size);
	// 登录完成
	bool OnSocketSubLogonFinish(void* data, uint16 data_size);

	// 列表相关
private:
	// 列表类型
	bool OnSocketSubListType(void* data, uint16 data_size);
	// 列表种类
	bool OnSocketSubListKind(void* data, uint16 data_size);
	// 列表房间
	bool OnSocketSubListServer(void* data, uint16 data_size);
	// 列表完成
	bool OnSocketSubListFinish(void* data, uint16 data_size);
	// 在线人数
	bool OnSocketSubOnlineCount(void* data, uint16 data_size);

	// 用户服务
private:
	// 更新结果
	bool OnSocketSubUserInfoResult(void* data, uint16 data_size);
	// 用户失败
	bool OnSocketSubUserFailure(void* data, uint16 data_size);

	// 系统消息相关
private:
	// 跑马灯消息
	bool OnSocketSubMarqueeMessage(void* data, uint16 data_size);

private:
	SocketEngine* socket_engine_;
	GameKindVector game_kind_list_;	// 游戏种类列表
	GameServerMap game_server_list_;	// 游戏房间列表
	BankDelegate* bank_delegate_;
	RankingDelegate* ranking_delegate_;
};

#endif // PLAZA_KERNEL_H_