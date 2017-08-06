
#ifndef CMD_DDZ_H_
#define CMD_DDZ_H_

#pragma pack(1)

//------------------------------------------------------------------------------
// 服务定义

#define KIND_ID               1
#define GAME_NAME             TEXT("斗地主")
#define GAME_PLAYER           3

// 最大牌数
const int kMaxCardCount = 20;
// 全牌数目
const int kFullCardCount = 54;
// 最大玩家
const uint16 kMaxPlayer = GAME_PLAYER;

// 花色掩码
#define	MASK_COLOR	0xF0
// 数值掩码
#define	MASK_VALUE	0x0F

//------------------------------------------------------------------------------
// 游戏状态

#define GAME_SCENE_FREE				GAME_STATUS_FREE					// 等待开始
#define GAME_SCENE_CALL				GAME_STATUS_PLAY					// 叫分状态
#define GAME_SCENE_PLAY				(GAME_STATUS_PLAY + 1)				// 游戏进行

// 空闲状态
struct CMD_S_StatusFree
{
	int	cell_score;		// 底分
};

// 叫分状态
struct CMD_S_StatusCall
{
	int cell_score;		// 底分
	uint16 current_user;	// 当前玩家
	uint8 lord_callscore;	// 地主叫分
	uint8 callscore_info[kMaxPlayer];	// 叫分信息
	uint8 hand_card_data[17];	// 手上扑克
};

// 游戏状态
struct CMD_S_StatusPlay
{
	// 游戏变量
	int cell_score;		// 底分
	uint8 bomb_count;	// 炸弹次数
	uint16 lord_user;	// 地主用户
	uint16 current_user;	// 当前用户
	uint8 lord_callscore;	// 地主叫分

	// 出牌信息
	uint16 turn_winner;	// 胜利玩家
	uint8 turn_card_count;	// 出牌数量
	uint8 turn_card_data[kMaxCardCount];	// 出牌数据

	// 扑克信息
	uint8 bottom_card[3];	// 游戏底牌
	uint8 hand_card_data[kMaxCardCount];	// 手上扑克
	uint8 hand_card_count[kMaxPlayer];		// 扑克数目
};

//------------------------------------------------------------------------------
// 服务端命令

#define SUB_S_GAME_START					100					// 游戏开始
#define SUB_S_CALL_SCORE					101					// 用户叫分
#define SUB_S_LORD_INFO						102					// 地主信息
#define SUB_S_OUT_CARD						103					// 用户出牌
#define SUB_S_PASS_CARD						104					// 用户放弃
#define SUB_S_GAME_CONCLUDE					105					// 游戏结束

// 游戏开始
struct CMD_S_GameStart
{
	uint16 start_user;		// 开始用户
	uint16 current_user;	// 当前用户
	uint8 card_data[17];	// 扑克数据
};

// 机器人扑克
struct CMD_S_AndroidCard
{
	uint8 hand_card_data[GAME_PLAYER][17]; // 手上扑克
	uint16 current_user;	// 当前玩家
	uint8 bottom_card[3];	// 地主扑克
};

// 用户叫分
struct CMD_S_CallScore
{
	uint16 current_user;	// 当前用户
	uint16 callscore_user;	// 叫分用户
	uint8 current_score;	// 当前叫分
	uint8 last_score;		// 上次叫分
};

// 地主信息
struct CMD_S_LordInfo
{
	uint16 lord_user;		// 地主用户
	uint16 current_user;	// 当前用户
	uint8 lord_score;	// 地主叫分
	uint8 bottom_card_data[3];	// 底牌数据
};

// 用户出牌
struct CMD_S_OutCard
{
	uint8 card_count;	// 出牌数量
	uint16 current_user;	// 当前用户
	uint16 outcard_user;	// 出牌用户
	uint8 card_data[kMaxCardCount];	// 出牌数据
};

// 放弃出牌
struct CMD_S_PassCard
{
	bool turn_over;	// 一轮结束
	uint16 current_user;	// 当前用户
	uint16 pass_user;		// 放弃用户
};

// 游戏结束
struct CMD_S_GameConclude
{
	int64 game_score[kMaxPlayer];	// 游戏得分

	bool chuntian;	// 春天
	bool fan_chuntian;	// 反春天

	uint8 bomb_count;	// 炸弹数目

	uint8 card_count[kMaxPlayer];	// 扑克数目
	uint8 card_data[kFullCardCount];	// 扑克数据
};

//------------------------------------------------------------------------------
// 客户端命令

#define SUB_C_CALL_SCORE					1					// 用户叫分
#define SUB_C_OUT_CARD						2					// 用户出牌
#define SUB_C_PASS_CARD						3					// 用户放弃

// 用户叫分
struct CMD_C_CallScore
{
	uint8 call_score;
};

// 用户出牌
struct CMD_C_OutCard
{
	uint8 card_count;	// 出牌数目
	uint8 card_data[kMaxCardCount];		// 扑克数据
};

#pragma pack()

#endif // CMD_DDZ_H_
