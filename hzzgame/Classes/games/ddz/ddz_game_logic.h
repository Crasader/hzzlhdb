
#ifndef GAMES_DDZ_GAME_LOGIC_H_
#define GAMES_DDZ_GAME_LOGIC_H_

#include "../../base/basictypes.h"
#include "CMD_Ddz.h"

namespace ddz
{

#define CT_ERROR					0									//��������
#define CT_SINGLE					1									//��������
#define CT_DOUBLE					2									//��������
#define CT_THREE					3									//��������
#define CT_SINGLE_LINE				4									//��������
#define CT_DOUBLE_LINE				5									//��������
#define CT_THREE_LINE				6									//��������
#define CT_THREE_TAKE_ONE			7									//����һ��
#define CT_THREE_TAKE_TWO			8									//����һ��
#define CT_FOUR_TAKE_ONE			9									//�Ĵ�����
#define CT_FOUR_TAKE_TWO			10									//�Ĵ�����
#define CT_BOMB_CARD				11									//ը������
#define CT_MISSILE_CARD				12									//�������

	// �����ṹ
	struct AnalyseResult
	{
		uint8 block_count[4];
		uint8 block_card_data[4][kMaxCardCount];
	};

	// �ֲ���Ϣ
	struct Distributing
	{
		uint8 card_count;
		uint8 distributing[15][6];
	};

	// �������
	struct SearchCardResult
	{
		uint8 search_count;
		uint8 card_count[kMaxCardCount];				//�˿���Ŀ
		uint8 result_data[kMaxCardCount][kMaxCardCount];	//����˿�
	};

	class GameLogic
	{
	public:
		// ��ȡ����
		static uint8 GetCardType(const uint8 card_data[], uint8 card_count);
		// ��ȡ��ֵ
		static uint8 GetCardValue(uint8 card_data) { return card_data & MASK_VALUE; }
		// ��ȡ��ɫ
		static uint8 GetCardColor(uint8 card_data) { return card_data & MASK_COLOR; }

		// �����˿�
		static void RandCardList(uint8 card_buffer[], uint8 buffer_count);
		// �����˿�
		static void SortCardList(uint8 card_data[], uint8 card_count);
		//�����˿�
		static void SortOutCardList(uint8 card_data[], uint8 card_count);
		// ɾ���˿�
		static bool RemoveCardList(const uint8 remove_card[], uint8 remove_count, uint8 card_data[], uint8 card_count);

		// �߼���ֵ
		static uint8 GetCardLogicValue(uint8 card_data);
		// �Ա��˿�
		static bool CompareCard(const uint8 first_card[], const uint8 next_card[], uint8 first_count, uint8 next_count);

		// ��������
		static uint8 SearchOutCard(const uint8 hand_card_data[], uint8 hand_card_count, const uint8 turn_card_data[], uint8 turn_card_count, SearchCardResult* result);
		// ͬ������
		static uint8 SearchSameCard(const uint8 hand_card_data[], uint8 hand_card_count, uint8 refer_card, uint8 same_count, SearchCardResult* result);
		// ��������
		static uint8 SearchLineCardType(const uint8 hand_card_data[], uint8 hand_card_count, uint8 refer_card, uint8 block_count, uint8 line_card_count, SearchCardResult* result);
		// ������������(����һ���Ĵ�һ��)
		static uint8 SearchTakeCardType(const uint8 hand_card_data[], uint8 hand_card_count, uint8 refer_card, uint8 same_count, uint8 take_card_count, SearchCardResult* result);
		// �����ɻ�
		static uint8 SearchThreeTwoLine(const uint8 hand_card_data[], uint8 hand_card_count, SearchCardResult* result);

		// �����˿�
		static void AnalysebCardData(const uint8 card_data[], uint8 card_count, AnalyseResult& analyse_result);
		// �����ֲ�
		static void AnalysebDistributing(const uint8 card_data[], uint8 card_count, Distributing& distributing);

	private:
		GameLogic();
		GameLogic(const GameLogic&);
		void operator=(const GameLogic&);
	};

}

#endif
