
#ifndef CMD_DDZ_H_
#define CMD_DDZ_H_

#pragma pack(1)

//------------------------------------------------------------------------------
// ������

#define KIND_ID               1
#define GAME_NAME             TEXT("������")
#define GAME_PLAYER           3

// �������
const int kMaxCardCount = 20;
// ȫ����Ŀ
const int kFullCardCount = 54;
// ������
const uint16 kMaxPlayer = GAME_PLAYER;

// ��ɫ����
#define	MASK_COLOR	0xF0
// ��ֵ����
#define	MASK_VALUE	0x0F

//------------------------------------------------------------------------------
// ��Ϸ״̬

#define GAME_SCENE_FREE				GAME_STATUS_FREE					// �ȴ���ʼ
#define GAME_SCENE_CALL				GAME_STATUS_PLAY					// �з�״̬
#define GAME_SCENE_PLAY				(GAME_STATUS_PLAY + 1)				// ��Ϸ����

// ����״̬
struct CMD_S_StatusFree
{
	int	cell_score;		// �׷�
};

// �з�״̬
struct CMD_S_StatusCall
{
	int cell_score;		// �׷�
	uint16 current_user;	// ��ǰ���
	uint8 lord_callscore;	// �����з�
	uint8 callscore_info[kMaxPlayer];	// �з���Ϣ
	uint8 hand_card_data[17];	// �����˿�
};

// ��Ϸ״̬
struct CMD_S_StatusPlay
{
	// ��Ϸ����
	int cell_score;		// �׷�
	uint8 bomb_count;	// ը������
	uint16 lord_user;	// �����û�
	uint16 current_user;	// ��ǰ�û�
	uint8 lord_callscore;	// �����з�

	// ������Ϣ
	uint16 turn_winner;	// ʤ�����
	uint8 turn_card_count;	// ��������
	uint8 turn_card_data[kMaxCardCount];	// ��������

	// �˿���Ϣ
	uint8 bottom_card[3];	// ��Ϸ����
	uint8 hand_card_data[kMaxCardCount];	// �����˿�
	uint8 hand_card_count[kMaxPlayer];		// �˿���Ŀ
};

//------------------------------------------------------------------------------
// ���������

#define SUB_S_GAME_START					100					// ��Ϸ��ʼ
#define SUB_S_CALL_SCORE					101					// �û��з�
#define SUB_S_LORD_INFO						102					// ������Ϣ
#define SUB_S_OUT_CARD						103					// �û�����
#define SUB_S_PASS_CARD						104					// �û�����
#define SUB_S_GAME_CONCLUDE					105					// ��Ϸ����

// ��Ϸ��ʼ
struct CMD_S_GameStart
{
	uint16 start_user;		// ��ʼ�û�
	uint16 current_user;	// ��ǰ�û�
	uint8 card_data[17];	// �˿�����
};

// �������˿�
struct CMD_S_AndroidCard
{
	uint8 hand_card_data[GAME_PLAYER][17]; // �����˿�
	uint16 current_user;	// ��ǰ���
	uint8 bottom_card[3];	// �����˿�
};

// �û��з�
struct CMD_S_CallScore
{
	uint16 current_user;	// ��ǰ�û�
	uint16 callscore_user;	// �з��û�
	uint8 current_score;	// ��ǰ�з�
	uint8 last_score;		// �ϴνз�
};

// ������Ϣ
struct CMD_S_LordInfo
{
	uint16 lord_user;		// �����û�
	uint16 current_user;	// ��ǰ�û�
	uint8 lord_score;	// �����з�
	uint8 bottom_card_data[3];	// ��������
};

// �û�����
struct CMD_S_OutCard
{
	uint8 card_count;	// ��������
	uint16 current_user;	// ��ǰ�û�
	uint16 outcard_user;	// �����û�
	uint8 card_data[kMaxCardCount];	// ��������
};

// ��������
struct CMD_S_PassCard
{
	bool turn_over;	// һ�ֽ���
	uint16 current_user;	// ��ǰ�û�
	uint16 pass_user;		// �����û�
};

// ��Ϸ����
struct CMD_S_GameConclude
{
	int64 game_score[kMaxPlayer];	// ��Ϸ�÷�

	bool chuntian;	// ����
	bool fan_chuntian;	// ������

	uint8 bomb_count;	// ը����Ŀ

	uint8 card_count[kMaxPlayer];	// �˿���Ŀ
	uint8 card_data[kFullCardCount];	// �˿�����
};

//------------------------------------------------------------------------------
// �ͻ�������

#define SUB_C_CALL_SCORE					1					// �û��з�
#define SUB_C_OUT_CARD						2					// �û�����
#define SUB_C_PASS_CARD						3					// �û�����

// �û��з�
struct CMD_C_CallScore
{
	uint8 call_score;
};

// �û�����
struct CMD_C_OutCard
{
	uint8 card_count;	// ������Ŀ
	uint8 card_data[kMaxCardCount];		// �˿�����
};

#pragma pack()

#endif // CMD_DDZ_H_
