
#ifndef GAMES_DDZ_GAME_LOGIC_H_
#define GAMES_DDZ_GAME_LOGIC_H_

#include "../../base/basictypes.h"
#include "CMD_Ddz.h"

namespace ddz
{

#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_THREE					3									//三条类型
#define CT_SINGLE_LINE				4									//单连类型
#define CT_DOUBLE_LINE				5									//对连类型
#define CT_THREE_LINE				6									//三连类型
#define CT_THREE_TAKE_ONE			7									//三带一单
#define CT_THREE_TAKE_TWO			8									//三带一对
#define CT_FOUR_TAKE_ONE			9									//四带两单
#define CT_FOUR_TAKE_TWO			10									//四带两对
#define CT_BOMB_CARD				11									//炸弹类型
#define CT_MISSILE_CARD				12									//火箭类型

	// 分析结构
	struct AnalyseResult
	{
		uint8 block_count[4];
		uint8 block_card_data[4][kMaxCardCount];
	};

	// 分布信息
	struct Distributing
	{
		uint8 card_count;
		uint8 distributing[15][6];
	};

	// 搜索结果
	struct SearchCardResult
	{
		uint8 search_count;
		uint8 card_count[kMaxCardCount];				//扑克数目
		uint8 result_data[kMaxCardCount][kMaxCardCount];	//结果扑克
	};

	class GameLogic
	{
	public:
		// 获取类型
		static uint8 GetCardType(const uint8 card_data[], uint8 card_count);
		// 获取数值
		static uint8 GetCardValue(uint8 card_data) { return card_data & MASK_VALUE; }
		// 获取花色
		static uint8 GetCardColor(uint8 card_data) { return card_data & MASK_COLOR; }

		// 混乱扑克
		static void RandCardList(uint8 card_buffer[], uint8 buffer_count);
		// 排列扑克
		static void SortCardList(uint8 card_data[], uint8 card_count);
		//排列扑克
		static void SortOutCardList(uint8 card_data[], uint8 card_count);
		// 删除扑克
		static bool RemoveCardList(const uint8 remove_card[], uint8 remove_count, uint8 card_data[], uint8 card_count);

		// 逻辑数值
		static uint8 GetCardLogicValue(uint8 card_data);
		// 对比扑克
		static bool CompareCard(const uint8 first_card[], const uint8 next_card[], uint8 first_count, uint8 next_count);

		// 出牌搜索
		static uint8 SearchOutCard(const uint8 hand_card_data[], uint8 hand_card_count, const uint8 turn_card_data[], uint8 turn_card_count, SearchCardResult* result);
		// 同牌搜索
		static uint8 SearchSameCard(const uint8 hand_card_data[], uint8 hand_card_count, uint8 refer_card, uint8 same_count, SearchCardResult* result);
		// 连牌搜索
		static uint8 SearchLineCardType(const uint8 hand_card_data[], uint8 hand_card_count, uint8 refer_card, uint8 block_count, uint8 line_card_count, SearchCardResult* result);
		// 带牌类型搜索(三带一，四带一等)
		static uint8 SearchTakeCardType(const uint8 hand_card_data[], uint8 hand_card_count, uint8 refer_card, uint8 same_count, uint8 take_card_count, SearchCardResult* result);
		// 搜索飞机
		static uint8 SearchThreeTwoLine(const uint8 hand_card_data[], uint8 hand_card_count, SearchCardResult* result);

		// 分析扑克
		static void AnalysebCardData(const uint8 card_data[], uint8 card_count, AnalyseResult& analyse_result);
		// 分析分布
		static void AnalysebDistributing(const uint8 card_data[], uint8 card_count, Distributing& distributing);

	private:
		GameLogic();
		GameLogic(const GameLogic&);
		void operator=(const GameLogic&);
	};

}

#endif
