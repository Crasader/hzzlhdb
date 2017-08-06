
#include "ddz_game_logic.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>

namespace ddz
{

// 扑克数据
const uint8	kFullCardData[kFullCardCount] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	// 方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	// 梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	// 红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	// 黑桃 A - K
	0x4E,0x4F,
};

// 获取类型
uint8 GameLogic::GetCardType(const uint8 card_data[], uint8 card_count)
{
	// 简单牌型
	switch (card_count)
	{
	case 0:	// 空牌
		{
			return CT_ERROR;
		}
	case 1: // 单牌
		{
			return CT_SINGLE;
		}
	case 2:	// 对牌或者火箭
		{
			if (card_data[0] == 0x4F && card_data[1] == 0x4E)
				return CT_MISSILE_CARD;
			if (GetCardLogicValue(card_data[0]) == GetCardLogicValue(card_data[1]))
				return CT_DOUBLE;

			return CT_ERROR;
		}
	}

	// 分析扑克
	AnalyseResult analyse_result;
	AnalysebCardData(card_data, card_count, analyse_result);

	// 四牌判断
	if (analyse_result.block_count[3] > 0)
	{
		// 牌型判断
		if (analyse_result.block_count[3] == 1 && card_count == 4)
			return CT_BOMB_CARD;
		if (analyse_result.block_count[3] == 1 && card_count == 6)
			return CT_FOUR_TAKE_ONE;
		if (analyse_result.block_count[3] == 1 && card_count == 8 && analyse_result.block_count[1] == 2)
			return CT_FOUR_TAKE_TWO;

		return CT_ERROR;
	}

	// 三牌判断
	if (analyse_result.block_count[2] > 0)
	{
		// 连牌判断
		if (analyse_result.block_count[2] > 1)
		{
			uint8 first_card_data = analyse_result.block_card_data[2][0];
			uint8 first_logic_value = GetCardLogicValue(first_card_data);

			// 错误过虑
			if (first_logic_value >= 15)
				return CT_ERROR;

			// 连牌判断
			for (uint8 i = 1; i < analyse_result.block_count[2]; i++)
			{
				uint8 data = analyse_result.block_card_data[2][i * 3];
				if (first_logic_value != (GetCardLogicValue(data) + i))
					return CT_ERROR;
			}
		}
		else if (card_count == 3)
		{
			return CT_THREE;
		}

		// 牌形判断
		if (analyse_result.block_count[2] * 3 == card_count)
			return CT_THREE_LINE;
		if (analyse_result.block_count[2] * 4 == card_count)
			return CT_THREE_TAKE_ONE;
		if (analyse_result.block_count[2] * 5 == card_count && analyse_result.block_count[1] == analyse_result.block_count[2])
			return CT_THREE_TAKE_TWO;

		return CT_ERROR;
	}

	// 两张类型
	if (analyse_result.block_count[1] >= 3)
	{
		uint8 data = analyse_result.block_card_data[1][0];
		uint8 first_logic_value = GetCardLogicValue(data);

		// 错误过虑
		if (first_logic_value >= 15)
			return CT_ERROR;

		// 连牌判断
		for (uint8 i = 1; i < analyse_result.block_count[1]; i++)
		{
			uint8 data = analyse_result.block_card_data[1][i * 2];
			if (first_logic_value != (GetCardLogicValue(data) + i))
				return CT_ERROR;
		}

		// 二连判断
		if ((analyse_result.block_count[1] * 2) == card_count)
			return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	// 单张判断
	if (analyse_result.block_count[0] >= 5 && analyse_result.block_count[0] == card_count)
	{
		uint8 data = analyse_result.block_card_data[0][0];
		uint8 first_logic_value = GetCardLogicValue(data);

		// 错误过虑
		if (first_logic_value >= 15)
			return CT_ERROR;

		// 连牌判断
		for (uint8 i = 1; i < analyse_result.block_count[0]; i++)
		{
			uint8 data = analyse_result.block_card_data[0][i];
			if (first_logic_value != (GetCardLogicValue(data) + i))
				return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

// 混乱扑克
void GameLogic::RandCardList(uint8 card_buffer[], uint8 buffer_count)
{
	// 混乱准备
	uint8 card_data[arraysize(kFullCardData)];
	memcpy(card_data, kFullCardData, sizeof(kFullCardData));

	// 混乱扑克
	uint8 rand_count = 0, pos = 0;
	do
	{
		pos = rand() % (buffer_count - rand_count);
		card_buffer[rand_count++] = card_data[pos];
		card_data[pos] = card_data[buffer_count - rand_count];
	} while (rand_count < buffer_count);
}

// 排列扑克
void GameLogic::SortCardList(uint8 card_data[], uint8 card_count)
{
	// 数目过虑
	if (card_count == 0)
		return;

	// 转换数值
	uint8 sort_value[kMaxCardCount];
	for (uint8 i = 0; i < card_count; i++)
	{
		sort_value[i] = GetCardLogicValue(card_data[i]);
	}

	// 排序操作
	bool sort = true;
	uint8 swap_data = 0, last = card_count - 1;
	do
	{
		sort=true;
		for (uint8 i = 0; i < last; i++)
		{
			if (sort_value[i] < sort_value[i+1] || (sort_value[i] == sort_value[i+1]) && (card_data[i] < card_data[i+1]))
			{
				sort = false;

				// 扑克数据
				swap_data = card_data[i];
				card_data[i] = card_data[i+1];
				card_data[i+1] = swap_data;

				// 排序权位
				swap_data = sort_value[i];
				sort_value[i] = sort_value[i+1];
				sort_value[i+1] = swap_data;
			}
		}
		last--;
	} while (!sort);
}

void GameLogic::SortOutCardList(uint8 card_data[], uint8 card_count)
{
	// 获取牌型
	uint8 card_type = GetCardType(card_data, card_count);

	if (card_type == CT_THREE_TAKE_ONE || card_type == CT_THREE_TAKE_TWO)
	{
		// 分析牌
		AnalyseResult analyse_result = { 0 };
		AnalysebCardData(card_data, card_count, analyse_result);

		card_count = analyse_result.block_count[2] * 3;
		memcpy(card_data, analyse_result.block_card_data[2], sizeof(uint8) * card_count);
		for (int i = arraysize(analyse_result.block_count) - 1; i >= 0; i--)
		{
			if (i == 2)
				continue;

			if (analyse_result.block_count[i] > 0)
			{
				memcpy(&card_data[card_count], analyse_result.block_card_data[i], sizeof(uint8) * (i + 1) * analyse_result.block_count[i]);
				card_count += (i + 1) * analyse_result.block_count[i];
			}
		}
	}
	else if (card_type == CT_FOUR_TAKE_ONE || card_type == CT_FOUR_TAKE_TWO)
	{
		// 分析牌
		AnalyseResult analyse_result = { 0 };
		AnalysebCardData(card_data, card_count, analyse_result);

		card_count = analyse_result.block_count[3] * 4;
		memcpy(card_data, analyse_result.block_card_data[3], sizeof(uint8) * card_count);
		for (int i = arraysize(analyse_result.block_count) - 1; i >= 0; i--)
		{
			if (i == 3)
				continue;

			if (analyse_result.block_count[i] > 0)
			{
				memcpy(&card_data[card_count], analyse_result.block_card_data[i], sizeof(uint8) * (i + 1) * analyse_result.block_count[i]);
				card_count += (i + 1) * analyse_result.block_count[i];
			}
		}
	}
}

// 删除扑克
bool GameLogic::RemoveCardList(const uint8 remove_card[], uint8 remove_count, uint8 card_data[], uint8 card_count)
{
	// 检验数据
	assert(remove_count <= card_count);

	uint8 delete_count = 0, temp_card_data[kMaxCardCount];
	if (card_count > kMaxCardCount)
		return false;
	memcpy(temp_card_data, card_data, card_count * sizeof(card_data[0]));

	// 置零扑克
	for (uint8 i = 0; i < remove_count; i++)
	{
		for (uint8 j = 0; j < card_count; j++)
		{
			if (remove_card[i] == temp_card_data[j])
			{
				delete_count++;
				temp_card_data[j]=0;
				break;
			}
		}
	}
	if (delete_count != remove_count)
		return false;

	// 清理扑克
	uint8 pos = 0;
	for (uint8 i = 0; i < card_count; i++)
	{
		if (temp_card_data[i] != 0) card_data[pos++] = temp_card_data[i];
	}

	return true;
}

// 逻辑数值
uint8 GameLogic::GetCardLogicValue(uint8 card_data)
{
	uint8 color = GetCardColor(card_data);
	uint8 value = GetCardValue(card_data);

	// 转换数值
	if (color == 0x40)
		return value + 2;
	return (value <= 2) ? (value + 13): value;
}

// 对比扑克
bool GameLogic::CompareCard(const uint8 first_card[], const uint8 next_card[], uint8 first_count, uint8 next_count)
{
	// 获取类型
	uint8 next_type = GetCardType(next_card, next_count);
	uint8 first_type = GetCardType(first_card, first_count);

	// 类型判断
	if (next_type == CT_ERROR)
		return false;
	if (next_type == CT_MISSILE_CARD)
		return true;

	// 炸弹判断
	if (first_type != CT_BOMB_CARD && next_type == CT_BOMB_CARD)
		return true;
	if (first_type==CT_BOMB_CARD && next_type!=CT_BOMB_CARD)
		return false;

	// 规则判断
	if (first_type != next_type || first_count != next_count)
		return false;

	// 开始对比
	switch (next_type)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
		{
			// 获取数值
			uint8 next_logic_value = GetCardLogicValue(next_card[0]);
			uint8 first_logic_value = GetCardLogicValue(first_card[0]);

			// 对比扑克
			return next_logic_value > first_logic_value;
		}
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
		{
			// 分析扑克
			AnalyseResult next_result;
			AnalyseResult first_result;
			AnalysebCardData(next_card, next_count, next_result);
			AnalysebCardData(first_card, first_count, first_result);

			// 获取数值
			uint8 next_logic_value = GetCardLogicValue(next_result.block_card_data[2][0]);
			uint8 first_logic_value = GetCardLogicValue(first_result.block_card_data[2][0]);

			// 对比扑克
			return next_logic_value > first_logic_value;
		}
	case CT_FOUR_TAKE_ONE:
	case CT_FOUR_TAKE_TWO:
		{
			// 分析扑克
			AnalyseResult next_result;
			AnalyseResult first_result;
			AnalysebCardData(next_card, next_count, next_result);
			AnalysebCardData(first_card, first_count, first_result);

			// 获取数值
			uint8 next_logic_value = GetCardLogicValue(next_result.block_card_data[3][0]);
			uint8 first_logic_value = GetCardLogicValue(first_result.block_card_data[3][0]);

			// 对比扑克
			return next_logic_value > first_logic_value;
		}
	}

	return false;
}

// 出牌搜索
uint8 GameLogic::SearchOutCard(const uint8 hand_card_data[], uint8 hand_card_count, const uint8 turn_card_data[], uint8 turn_card_count, SearchCardResult* result)
{
	assert(result);
	memset(result, 0, sizeof(SearchCardResult));

	uint8 result_count = 0;
	SearchCardResult search_result = { 0 };

	uint8 card_data[kMaxCardCount];
	uint8 card_cunt = hand_card_count;
	memcpy(card_data, hand_card_data, sizeof(uint8) * hand_card_count);

	SortCardList(card_data, card_cunt);

	// 获取类型
	uint8 turn_outcard_type = GetCardType(turn_card_data, turn_card_count);

	// 出牌分析
	switch (turn_outcard_type)
	{
	case CT_ERROR:					// 错误类型
		{
			// 是否一手出完
			if (GetCardType(card_data, card_cunt) != CT_ERROR)
			{
				result->card_count[result_count] = card_cunt;
				memcpy(result->result_data[result_count], card_data, sizeof(uint8) * card_cunt);
				result_count++;
			}

			// 如果最小牌不是单牌，则提取
			uint8 same_count = 0;
			if (card_cunt > 1 && GetCardValue(card_data[card_cunt - 1]) == GetCardValue(card_data[card_cunt - 2]))
			{
				same_count = 1;
				result->result_data[result_count][0] = card_data[card_cunt - 1];
				uint8 card_value = GetCardValue(card_data[card_cunt - 1]);
				for (int i = card_cunt - 2; i >= 0; i--)
				{
					if (GetCardValue(card_data[i]) == card_value)
					{
						result->result_data[result_count][same_count++] = card_data[i];
					}
					else break;
				}

				result->card_count[result_count] = same_count;
				result_count++;
			}

			// 单牌
			uint8 temp_count = 0;
			if (same_count != 1)
			{
				temp_count = SearchSameCard(card_data, card_cunt, 0, 1, &search_result);
				if (temp_count > 0)
				{
					result->card_count[result_count] = search_result.card_count[0];
					memcpy(result->result_data[result_count], search_result.result_data[0], sizeof(uint8) * search_result.card_count[0]);
					result_count++;
				}
			}

			// 对牌
			if (same_count != 2)
			{
				temp_count = SearchSameCard(card_data, card_cunt, 0, 2, &search_result);
				if (temp_count > 0)
				{
					result->card_count[result_count] = search_result.card_count[0];
					memcpy(result->result_data[result_count], search_result.result_data[0], sizeof(uint8)*  search_result.card_count[0]);
					result_count++;
				}
			}

			// 三条
			if (same_count != 3)
			{
				temp_count = SearchSameCard(card_data, card_cunt, 0, 3, &search_result);
				if (temp_count > 0)
				{
					result->card_count[result_count] = search_result.card_count[0];
					memcpy(result->result_data[result_count], search_result.result_data[0], sizeof(uint8)*  search_result.card_count[0]);
					result_count++;
				}
			}

			// 三带一单
			temp_count = SearchTakeCardType(card_data, card_cunt, 0, 3, 1, &search_result);
			if (temp_count > 0)
			{
				result->card_count[result_count] = search_result.card_count[0];
				memcpy(result->result_data[result_count], search_result.result_data[0], sizeof(uint8)*  search_result.card_count[0]);
				result_count++;
			}

			// 三带一对
			temp_count = SearchTakeCardType(card_data, card_cunt, 0, 3, 2, &search_result);
			if (temp_count > 0)
			{
				result->card_count[result_count] = search_result.card_count[0];
				memcpy(result->result_data[result_count], search_result.result_data[0], sizeof(uint8)*  search_result.card_count[0]);
				result_count++;
			}

			// 单连
			temp_count = SearchLineCardType(card_data, card_cunt, 0, 1, 0, &search_result);
			if (temp_count > 0)
			{
				result->card_count[result_count] = search_result.card_count[0];
				memcpy(result->result_data[result_count], search_result.result_data[0], sizeof(uint8)*  search_result.card_count[0]);
				result_count++;
			}

			// 连对
			temp_count = SearchLineCardType(card_data, card_cunt, 0, 2, 0, &search_result);
			if (temp_count > 0)
			{
				result->card_count[result_count] = search_result.card_count[0];
				memcpy(result->result_data[result_count], search_result.result_data[0], sizeof(uint8)*  search_result.card_count[0]);
				result_count++;
			}

			// 三连
			temp_count = SearchLineCardType(card_data, card_cunt, 0, 3, 0, &search_result);
			if (temp_count > 0)
			{
				result->card_count[result_count] = search_result.card_count[0];
				memcpy(result->result_data[result_count], search_result.result_data[0], sizeof(uint8)*  search_result.card_count[0]);
				result_count++;
			}

			//// 飞机
			//temp_count = SearchThreeTwoLine(card_data, card_cunt, &search_result);
			//if (temp_count > 0)
			//{
			//	result->card_count[result_count] = search_result.card_count[0];
			//	memcpy(result->result_data[result_count], search_result.result_data[0], sizeof(uint8)*  search_result.card_count[0]);
			//	result_count++;
			//}

			// 炸弹
			if (same_count != 4)
			{
				temp_count = SearchSameCard(card_data, card_cunt, 0, 4, &search_result);
				if (temp_count > 0)
				{
					result->card_count[result_count] = search_result.card_count[0];
					memcpy(result->result_data[result_count], search_result.result_data[0], sizeof(uint8)*  search_result.card_count[0]);
					result_count++;
				}
			}

			// 搜索火箭
			if ((card_cunt >= 2) && (card_data[0] == 0x4F) && (card_data[1] == 0x4E))
			{
				result->card_count[result_count] = 2;
				result->result_data[result_count][0] = card_data[0];
				result->result_data[result_count][1] = card_data[1];

				result_count++;
			}

			result->search_count = result_count;
			return result_count;
		}
	case CT_SINGLE:					// 单牌类型
	case CT_DOUBLE:					// 对牌类型
	case CT_THREE:					// 三条类型
		{
			uint8 refer_card = turn_card_data[0];
			uint8 same_count = 1;
			if (turn_outcard_type == CT_DOUBLE)
			{
				same_count = 2;
			}
			else if (turn_outcard_type == CT_THREE)
			{
				same_count = 3;
			}

			// 搜索相同牌
			result_count = SearchSameCard(card_data, card_cunt, refer_card, same_count, result);

			break;
		}
	case CT_SINGLE_LINE:		// 单连类型
	case CT_DOUBLE_LINE:		// 对连类型
	case CT_THREE_LINE:			// 三连类型
		{
			uint8 block_count = 1;
			if (turn_outcard_type == CT_DOUBLE_LINE)
			{
				block_count = 2;
			}
			else if (turn_outcard_type == CT_THREE_LINE)
			{
				block_count = 3;
			}

			uint8 line_count = turn_card_count / block_count;

			// 搜索连牌
			result_count = SearchLineCardType(card_data, card_cunt, turn_card_data[0], block_count, line_count, result);

			break;
		}
	case CT_THREE_TAKE_ONE:		// 三带一单
	case CT_THREE_TAKE_TWO:		// 三带一对
		{
			// 效验牌数
			if (card_cunt < turn_card_count)
				break;

			// 如果是三带一或三带二
			if (turn_card_count == 4 || turn_card_count == 5)
			{
				uint8 take_card_count = turn_outcard_type == CT_THREE_TAKE_ONE ? 1 : 2;

				// 搜索三带牌型
				result_count = SearchTakeCardType(card_data, card_cunt, turn_card_data[2], 3, take_card_count, result);
			}
			else
			{
				uint8 block_count = 3;
				uint8 line_count = turn_card_count / (turn_outcard_type == CT_THREE_TAKE_ONE ? 4 : 5);
				uint8 take_card_count = turn_outcard_type == CT_THREE_TAKE_ONE ? 1 : 2;

				// 搜索连牌
				uint8 temp_turn_card[kMaxCardCount] = { 0 };
				memcpy(temp_turn_card, turn_card_data, sizeof(uint8) * turn_card_count);
				SortOutCardList(temp_turn_card, turn_card_count);
				result_count = SearchLineCardType(card_data, card_cunt, temp_turn_card[0], block_count, line_count, result);

				// 提取带牌
				bool all_distill = true;
				for (uint8 i = 0; i < result_count; i++)
				{
					uint8 result_index = result_count - i - 1;

					uint8 temp_card_data[kMaxCardCount];
					uint8 temp_card_count = card_cunt;

					// 删除连牌
					memcpy(temp_card_data, card_data, sizeof(uint8) * card_cunt);
					RemoveCardList(result->result_data[result_index], result->card_count[result_index], temp_card_data, temp_card_count);
					temp_card_count -= result->card_count[result_index];

					// 分析牌
					AnalyseResult  temp_result = { 0 };
					AnalysebCardData(temp_card_data, temp_card_count, temp_result);

					// 提取牌
					uint8 distill_card[kMaxCardCount] = {};
					uint8 distill_count = 0;
					for (uint8 j = take_card_count - 1; j < arraysize(temp_result.block_count); j++)
					{
						if (temp_result.block_count[j] > 0)
						{
							if (j + 1 == take_card_count && temp_result.block_count[j] >= line_count)
							{
								uint8 temp_block_count = temp_result.block_count[j];
								memcpy(distill_card, &temp_result.block_card_data[j][(temp_block_count - line_count) * (j + 1)], sizeof(uint8) * (j + 1) * line_count);
								distill_count = (j + 1) * line_count;
								break;
							}
							else
							{
								for (uint8 k = 0; k < temp_result.block_count[j]; k++)
								{
									uint8 temp_block_count = temp_result.block_count[j];
									memcpy(&distill_card[distill_count], &temp_result.block_card_data[j][(temp_block_count - k - 1) * (j + 1)], sizeof(uint8) * take_card_count);
									distill_count += take_card_count;

									// 提取完成
									if (distill_count == take_card_count * line_count)
										break;
								}
							}
						}

						// 提取完成
						if (distill_count == take_card_count * line_count)
							break;
					}

					// 提取完成
					if (distill_count == take_card_count * line_count)
					{
						// 复制带牌
						uint8 count = result->card_count[result_index];
						memcpy(&result->result_data[result_index][count], distill_card, sizeof(uint8) * distill_count);
						result->card_count[result_index] += distill_count;
					}
					// 否则删除连牌
					else
					{
						all_distill = false;
						result->card_count[result_index] = 0;
					}
				}

				// 整理组合
				if (!all_distill)
				{
					result->search_count = result_count;
					result_count = 0;
					for (uint8 i = 0; i < result->search_count; i++)
					{
						if (result->card_count[i] != 0)
						{
							search_result.card_count[result_count] = result->card_count[i];
							memcpy(search_result.result_data[result_count], result->result_data[i], sizeof(uint8) * result->card_count[i]);
							result_count++;
						}
					}
					search_result.search_count = result_count;
					memcpy(result, &search_result, sizeof(SearchCardResult));
				}
			}

			break;
		}
	case CT_FOUR_TAKE_ONE:		// 四带两单
	case CT_FOUR_TAKE_TWO:		// 四带两双
		{
			uint8 take_count = turn_outcard_type == CT_FOUR_TAKE_ONE ? 1 : 2;

			uint8 temp_turn_card[kMaxCardCount] = { 0 };
			memcpy(temp_turn_card, turn_card_data, sizeof(uint8) * turn_card_count);
			SortOutCardList(temp_turn_card, turn_card_count);

			// 搜索带牌
			result_count = SearchTakeCardType(card_data, card_cunt, temp_turn_card[0],4, take_count, result);

			break;
		}
	}

	// 搜索炸弹
	if ((card_cunt >= 4) && (turn_outcard_type != CT_MISSILE_CARD))
	{
		uint8 refer_card = 0;
		if (turn_outcard_type == CT_BOMB_CARD) refer_card = turn_card_data[0];

		// 搜索炸弹
		uint8 temp_result_count = SearchSameCard(card_data, card_cunt, refer_card, 4, &search_result);
		for (uint8 i = 0; i < temp_result_count; i++)
		{
			result->card_count[result_count] = search_result.card_count[i];
			memcpy(result->result_data[result_count], search_result.result_data[i], sizeof(uint8) * search_result.card_count[i]);
			result_count++;
		}
	}

	// 搜索火箭
	if (turn_outcard_type != CT_MISSILE_CARD && (card_cunt >= 2) && (card_data[0] == 0x4F) && (card_data[1] == 0x4E))
	{
		result->card_count[result_count] = 2;
		result->result_data[result_count][0] = card_data[0];
		result->result_data[result_count][1] = card_data[1];

		result_count++;
	}

	result->search_count = result_count;
	return result_count;
}

// 分析扑克
void GameLogic::AnalysebCardData(const uint8 card_data[], uint8 card_count, AnalyseResult& analyse_result)
{
	memset(&analyse_result, 0, sizeof(analyse_result));

	// 扑克分析
	for (uint8 i = 0; i < card_count; i++)
	{
		uint8 same_count = 1, value_temp = 0;
		uint8 logic_value = GetCardLogicValue(card_data[i]);

		// 搜索同牌
		for (uint8 j = i + 1; j < card_count; j++)
		{
			// 获取扑克
			if (GetCardLogicValue(card_data[j]) != logic_value)
				break;

			same_count++;
		}

		// 设置结果
		uint8 index = analyse_result.block_count[same_count - 1]++;
		for (uint8 j = 0; j < same_count; j++)
		{
			analyse_result.block_card_data[same_count - 1][index * same_count + j] = card_data[i + j];
		}

		// 设置索引
		i += same_count - 1;
	}
}

// 同牌搜索
uint8 GameLogic::SearchSameCard(const uint8 hand_card_data[], uint8 hand_card_count, uint8 refer_card, uint8 same_count, SearchCardResult* result)
{
	memset(result, 0, sizeof(SearchCardResult));
	uint8 result_count = 0;

	uint8 card_data[kMaxCardCount];
	uint8 card_count = hand_card_count;
	memcpy(card_data, hand_card_data, sizeof(uint8) * hand_card_count);

	SortCardList(card_data, card_count);

	// 分析扑克
	AnalyseResult analyse_result = { 0 };
	AnalysebCardData(card_data, card_count, analyse_result);

	uint8 refer_logic_value = refer_card == 0 ? 0 : GetCardLogicValue(refer_card);
	uint8 block_index = same_count - 1;
	do
	{
		for (uint8 i = 0; i < analyse_result.block_count[block_index]; i++)
		{
			uint8 index = (analyse_result.block_count[block_index] - i - 1) * (block_index + 1);
			if (GetCardLogicValue(analyse_result.block_card_data[block_index][index]) > refer_logic_value)
			{
				memcpy(result->result_data[result_count], &analyse_result.block_card_data[block_index][index], same_count * sizeof(uint8) );
				result->card_count[result_count] = same_count;
				result_count++;
			}
		}

		block_index++;
	} while (block_index < arraysize(analyse_result.block_count) );

	result->search_count = result_count;
	return result_count;
}

uint8 MakeCardData(uint8 value_index, uint8 color_index)
{
	return (color_index << 4) | (value_index + 1);
}

// 连牌搜索
uint8 GameLogic::SearchLineCardType(const uint8 hand_card_data[], uint8 hand_card_count, uint8 refer_card, uint8 block_count, uint8 line_card_count, SearchCardResult* result)
{
	memset(result, 0, sizeof(SearchCardResult));
	uint8 result_count = 0;

	uint8 less_line_count = 0;
	if (line_card_count == 0)
	{
		if (block_count == 1)
		{
			less_line_count = 5;	// 顺子
		}
		else if (block_count == 2)
		{
			less_line_count = 3;	// 连对
		}
		else
		{
			less_line_count = 2;	// 三连
		}
	}
	else
	{
		less_line_count = line_card_count;
	}

	uint8 refer_index = 2;
	if (refer_card != 0)
	{
		assert(GetCardLogicValue(refer_card) - less_line_count >= 2);
		refer_index = GetCardLogicValue(refer_card) - less_line_count + 1;
	}

	// 超过A
	if (refer_index + less_line_count > 14)
		return result_count;

	// 长度判断
	if (hand_card_count < less_line_count * block_count)
		return result_count;

	uint8 card_data[kMaxCardCount];
	uint8 card_count = hand_card_count;
	memcpy(card_data, hand_card_data, sizeof(uint8) * hand_card_count);

	SortCardList(card_data, card_count);

	// 分析扑克
	Distributing distributing = { 0 };
	AnalysebDistributing(card_data, card_count, distributing);

	// 搜索顺子
	uint8 temp_link_count = 0;
	uint8 value_index;
	for (value_index = refer_index; value_index < 13; value_index++)
	{
		// 继续判断
		if (distributing.distributing[value_index][5] < block_count)
		{
			if (temp_link_count < less_line_count )
			{
				temp_link_count = 0;
				continue;
			}
			else value_index--;
		}
		else 
		{
			temp_link_count++;
			// 寻找最长连
			if (line_card_count == 0)
				continue;
		}

		if (temp_link_count >= less_line_count)
		{
			uint8 count = 0;
			for (uint8 index = value_index + 1 - temp_link_count; index <= value_index; index++)
			{
				uint8 temp_count = 0;
				for (uint8 color_index = 0; color_index < 4; color_index++)
				{
					for (uint8 color_count = 0; color_count < distributing.distributing[index][3 - color_index]; color_count++)
					{
						result->result_data[result_count][count++] = MakeCardData(index, 3 - color_index);

						if (++temp_count == block_count)
							break;
					}
					if (temp_count == block_count)
						break;
				}
			}

			result->card_count[result_count] = count;
			result_count++;

			if (line_card_count != 0)
			{
				temp_link_count--;
			}
			else 
			{
				temp_link_count = 0;
			}
		}
	}

	// 特殊顺子
	if (temp_link_count >= less_line_count - 1 && value_index == 13)
	{
		if (distributing.distributing[0][5] >= block_count || temp_link_count >= less_line_count)
		{
			uint8 count = 0;
			uint8 temp_count = 0;
			for (uint8 index = value_index - temp_link_count; index < 13; index++ )
			{
				temp_count = 0;
				for (uint8 color_index = 0; color_index < 4; color_index++)
				{
					for (uint8 color_count = 0; color_count < distributing.distributing[index][3 - color_index]; color_count++)
					{
						result->result_data[result_count][count++] = MakeCardData(index, 3 - color_index);

						if (++temp_count == block_count)
							break;
					}
					if (temp_count == block_count)
						break;
				}
			}
			// 复制A
			if (distributing.distributing[0][5] >= block_count)
			{
				temp_count = 0;
				for (uint8 color_index = 0; color_index < 4; color_index++)
				{
					for (uint8 color_count = 0; color_count < distributing.distributing[0][3 - color_index]; color_count++)
					{
						result->result_data[result_count][count++] = MakeCardData(0, 3 - color_index);

						if (++temp_count == block_count)
							break;
					}
					if (temp_count == block_count)
						break;
				}
			}

			result->card_count[result_count] = count;
			result_count++;
		}
	}

	result->search_count = result_count;
	return result_count;
}

// 带牌类型搜索(三带一，四带一等)
uint8 GameLogic::SearchTakeCardType(const uint8 hand_card_data[], uint8 hand_card_count, uint8 refer_card, uint8 same_count, uint8 take_card_count, SearchCardResult* result)
{
	memset(result, 0, sizeof(SearchCardResult));
	uint8 result_count = 0;

	// 效验
	assert(same_count == 3 || same_count == 4);
	assert(take_card_count == 1 || take_card_count == 2);
	if (same_count != 3 && same_count != 4)
		return result_count;
	if (take_card_count != 1 && take_card_count != 2)
		return result_count;

	// 长度判断
	if ((same_count == 4 && hand_card_count < (same_count + take_card_count * 2)) || hand_card_count < same_count + take_card_count)
		return result_count;

	uint8 card_data[kMaxCardCount];
	uint8 card_count = hand_card_count;
	memcpy(card_data, hand_card_data, sizeof(uint8) * hand_card_count);

	SortCardList(card_data, card_count);

	// 搜索同张
	SearchCardResult same_card_result = { 0 };
	uint8 same_card_result_count = SearchSameCard(card_data, card_count, refer_card, same_count, &same_card_result);

	if (same_card_result_count > 0)
	{
		// 分析扑克
		AnalyseResult analyse_result;
		AnalysebCardData(card_data, card_count, analyse_result);

		// 需要牌数
		uint8 need_count = same_count + take_card_count;
		if (same_count == 4) need_count += take_card_count;

		// 提取带牌
		for (uint8 i = 0; i < same_card_result_count; i++)
		{
			bool merge = false;

			for (uint8 j = take_card_count - 1; j < arraysize(analyse_result.block_count); j++)
			{
				for (uint8 k = 0; k < analyse_result.block_count[j]; k++)
				{
					// 从小到大
					uint8 index = (analyse_result.block_count[j] - k - 1) * (j + 1);

					// 过滤相同牌
					if (GetCardValue(same_card_result.result_data[i][0]) == GetCardValue(analyse_result.block_card_data[j][index]) )
						continue;

					// 复制带牌
					uint8 count = same_card_result.card_count[i];
					memcpy(&same_card_result.result_data[i][count], &analyse_result.block_card_data[j][index], sizeof(uint8) * take_card_count);
					same_card_result.card_count[i] += take_card_count;

					if (same_card_result.card_count[i] < need_count)
						continue;

					// 复制结果
					memcpy(result->result_data[result_count], same_card_result.result_data[i], sizeof(uint8) * same_card_result.card_count[i]);
					result->card_count[result_count] = same_card_result.card_count[i];
					result_count++;

					merge = true;

					// 下一组合
					break;
				}

				if (merge)
					break;
			}
		}
	}

	result->search_count = result_count;
	return result_count;
}

// 搜索飞机
uint8 GameLogic::SearchThreeTwoLine(const uint8 hand_card_data[], uint8 hand_card_count, SearchCardResult* result)
{
	memset(result, 0, sizeof(SearchCardResult));

	SearchCardResult temp_search_result = { 0 };
	SearchCardResult temp_single_wing = { 0 };
	SearchCardResult temp_double_wing = { 0 };
	uint8 temp_result_count = 0;

	// 搜索连牌
	temp_result_count = SearchLineCardType(hand_card_data, hand_card_count, 0, 3, 0, &temp_search_result);

	if (temp_result_count > 0)
	{
		// 提取带牌
		for (uint8 i = 0; i < temp_result_count; i++)
		{
			uint8 temp_card_data[kMaxCardCount];
			uint8 temp_card_count = hand_card_count;

			// 不够牌
			if (hand_card_count - temp_search_result.card_count[i] < temp_search_result.card_count[i] / 3)
			{
				uint8 need_del_count = 3;
				while (hand_card_count + need_del_count - temp_search_result.card_count[i] < (temp_search_result.card_count[i] - need_del_count) / 3)
					need_del_count += 3;
				// 不够连牌
				if ((temp_search_result.card_count[i] - need_del_count) / 3 < 2)
				{
					// 废除连牌
					continue;
				}

				// 拆分连牌
				RemoveCardList(temp_search_result.result_data[i], need_del_count, temp_search_result.result_data[i], temp_search_result.card_count[i]);
				temp_search_result.card_count[i] -= need_del_count;
			}

			// 删除连牌
			memcpy(temp_card_data, hand_card_data, sizeof(uint8) * hand_card_count);
			RemoveCardList(temp_search_result.result_data[i], temp_search_result.card_count[i], temp_card_data, temp_card_count);
			temp_card_count -= temp_search_result.card_count[i];

			// 组合飞机
			uint8 need_count = temp_search_result.card_count[i] / 3;
			assert(need_count <= temp_card_count);

			uint8 result_count = temp_single_wing.search_count++;
			memcpy(temp_single_wing.result_data[result_count], temp_search_result.result_data[i], sizeof(uint8) * temp_search_result.card_count[i]);
			memcpy(&temp_single_wing.result_data[result_count][temp_search_result.card_count[i]], &temp_card_data[temp_card_count - need_count], sizeof(uint8) * need_count);
			temp_single_wing.card_count[i] = temp_search_result.card_count[i] + need_count;

			// 不够带翅膀
			if (temp_card_count < temp_search_result.card_count[i] / 3 * 2)
			{
				uint8 need_del_count = 3;
				while (temp_card_count + need_del_count - temp_search_result.card_count[i] < (temp_search_result.card_count[i] - need_del_count) / 3 * 2 )
					need_del_count += 3;
				// 不够连牌
				if ((temp_search_result.card_count[i] - need_del_count) / 3 < 2)
				{
					// 废除连牌
					continue;
				}

				// 拆分连牌
				RemoveCardList(temp_search_result.result_data[i], need_del_count, temp_search_result.result_data[i], temp_search_result.card_count[i]);
				temp_search_result.card_count[i] -= need_del_count;

				// 重新删除连牌
				memcpy(temp_card_data, hand_card_data, sizeof(uint8) * hand_card_count);
				RemoveCardList(temp_search_result.result_data[i],temp_search_result.card_count[i], temp_card_data, temp_card_count);
				temp_card_count = hand_card_count - temp_search_result.card_count[i];
			}

			// 分析牌
			AnalyseResult temp_result = { 0 };
			AnalysebCardData(temp_card_data, temp_card_count,temp_result);

			// 提取翅膀
			uint8 distill_card[kMaxCardCount] = { 0 };
			uint8 distill_count = 0;
			uint8 line_count = temp_search_result.card_count[i] / 3;
			for (uint8 j = 1; j < arraysize(temp_result.block_count); j++)
			{
				if (temp_result.block_count[j] > 0)
				{
					if (j + 1 == 2 && temp_result.block_count[j] >= line_count)
					{
						uint8 temp_block_count = temp_result.block_count[j];
						memcpy(distill_card, &temp_result.block_card_data[j][(temp_block_count - line_count) * (j + 1)], sizeof(uint8) * (j + 1) * line_count);
						distill_count = (j + 1) * line_count;
						break;
					}
					else
					{
						for (uint8 k = 0; k < temp_result.block_count[j]; k++)
						{
							uint8 temp_block_count = temp_result.block_count[j];
							memcpy(&distill_card[distill_count], &temp_result.block_card_data[j][(temp_block_count - k - 1) * (j + 1)], sizeof(uint8) * 2);
							distill_count += 2;

							// 提取完成
							if (distill_count == 2 * line_count)
								break;
						}
					}
				}

				// 提取完成
				if (distill_count == 2 * line_count)
					break;
			}

			// 提取完成
			if (distill_count == 2 * line_count)
			{
				// 复制翅膀
				result_count = temp_double_wing.search_count++;
				memcpy(temp_double_wing.result_data[result_count], temp_search_result.result_data[i], sizeof(uint8) * temp_search_result.card_count[i]);
				memcpy(&temp_double_wing.result_data[result_count][temp_search_result.card_count[i]], distill_card, sizeof(uint8) * distill_count);
				temp_double_wing.card_count[i] = temp_search_result.card_count[i] + distill_count;
			}
		}

		// 复制结果
		for (uint8 i = 0; i < temp_double_wing.search_count; i++)
		{
			uint8 result_count = result->search_count++;
			memcpy(result->result_data[result_count], temp_double_wing.result_data[i], sizeof(uint8) * temp_double_wing.card_count[i]);
			result->card_count[result_count] = temp_double_wing.card_count[i];
		}
		for (uint8 i = 0; i < temp_single_wing.search_count; i++)
		{
			uint8 result_count = result->search_count++;

			memcpy(result->result_data[result_count], temp_single_wing.result_data[i], sizeof(uint8) * temp_single_wing.card_count[i]);
			result->card_count[result_count] = temp_single_wing.card_count[i];
		}
	}

	return result->search_count;
}

void GameLogic::AnalysebDistributing(const uint8 card_data[], uint8 card_count, Distributing& distributing)
{
	memset(&distributing, 0, sizeof(distributing));

	for (uint8 i = 0; i < card_count; i++)
	{
		if (card_data[i] == 0) continue;

		uint8 color = GetCardColor(card_data[i]);
		uint8 value = GetCardValue(card_data[i]);

		distributing.card_count++;
		distributing.distributing[value - 1][5]++;
		distributing.distributing[value - 1][color >> 4]++;
	}
}

}
