#ifndef GAMES_DDZ_GAME_SCENE_H_
#define GAMES_DDZ_GAME_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../../base/basictypes.h"
#include "CMD_Ddz.h"

#include "../../kernel/game_kernel.h"

#include "ddz_card.h"
#include "ddz_back_card.h"
#include "ddz_out_card.h"

#include "ddz_game_logic.h"

USING_NS_CC;
using namespace cocos2d::ui;

namespace ddz
{

	class GameScene : public cocos2d::Layer, public GameKernel::Delegate
	{
		enum SpeakContent
		{
			SPEAK_CALL0 = 0,	// ����
			SPEAK_CALL1,
			SPEAK_CALL2,
			SPEAK_CALL3,
			SPEAK_PASS,			// ����

			SPEAK_COUNT			// ������ʾ
		};

	public:
		GameScene();
		virtual ~GameScene();
		static cocos2d::Scene* createScene();

		virtual bool init();
		virtual void onEnterTransitionDidFinish();
		virtual void onExit();

		//�˳���Ϸ
		void OnClickBack(Ref* sender);

		// implement the "static create()" method manually
		CREATE_FUNC(GameScene);

	private:
		virtual void ResetGameClient();
		virtual void CloseGameClient();
		virtual bool OnEventGameMessage(WORD sub_cmdid, void* data, WORD data_size);
		virtual bool OnEventSceneMessage(uint8 game_status, bool lookon_user, void* data, WORD data_size);
		virtual void OnEventUserEnter(IGameUserItem* game_user_item, bool lookon_user);
		virtual void OnEventUserLeave(IGameUserItem* game_user_item, bool lookon_user);
		virtual void OnEventUserScore(IGameUserItem* game_user_item, bool lookon_user);
		virtual void OnEventUserStatus(IGameUserItem* game_user_item, bool lookon_user);
		virtual void OnSystemMessage(const char* sys_msg, WORD msg_len, WORD type);

		bool SendSocketData(WORD sub_cmdid);
		bool SendSocketData(WORD sub_cmdid, void* data, WORD data_size);

		// ��Ϸ��ʼ
		bool OnSubGameStart(void* data, WORD data_size);
		// �û��з�
		bool OnSubCallScore(void* data, WORD data_size);
		// ������Ϣ
		bool OnSubLordInfo(void* data, WORD data_size);
		// �û�����
		bool OnSubOutCard(void* data, WORD data_size);
		// �û�����
		bool OnSubPassCard(void* data, WORD data_size);
		// ��Ϸ����
		bool OnSubGameConclude(void* data, WORD data_size);

		uint16 ViewChairID(uint16 chair_id);

	private:
		// ��ʾ��ʱ��
		void ShowGameTimer(uint16 chair_id, uint32 timer_id, int sec);
		// ���ؼ�ʱ��
		void HideGameTimer();
		// ��ʱ���ص�
		void OnGameTimer(float dt);
		// ����
		void SendCards(uint16 first_user);
		// ���ƻص�
		void OnSendCardsCallback(cocos2d::Node* node);
		// ��ʼ��ť
		void OnClickStart(cocos2d::Ref* sender);
		// ����
		void OnClickCall0(cocos2d::Ref* sender);
		// 1��
		void OnClickCall1(cocos2d::Ref* sender);
		// 2��
		void OnClickCall2(cocos2d::Ref* sender);
		// 3��
		void OnClickCall3(cocos2d::Ref* sender);
		// ����
		void OnClickOutCard(cocos2d::Ref* sender);
		// ����
		void OnClickPass(cocos2d::Ref* sender);
		// ��ʾ
		void OnClickHint(cocos2d::Ref* sender);
		// Ҫ����
		void OnClickMiss(cocos2d::Ref* sender);
		// �Զ�����
		void OnClickAutoOutCard(cocos2d::Ref* sender);
		// ȡ���Զ�����
		void OnClickCancelAutoOutCard(cocos2d::Ref* sender);
		// ����Ҽ�
		void OnCardMouseRight();
		// ������
		void OnCardMouseLeft();
		// ˵������
		void SetSpeakContent(uint16 chair_id, SpeakContent sc);
		// ��ʾ����
		void ShowBottomCards();
		// �����ж�
		bool VerdictOutCard();
		// ը������
		void PlayBombAnimation(cocos2d::Vec2 pos);
		// �������
		void PlayRocketAnimation();
		// �ɻ�����
		void PlayPlaneAnimation();
		// ����
		void PlayChainPairsAnimation(cocos2d::Vec2 pos);
		// ˳��
		void PlayStraightAnimation(cocos2d::Vec2 pos);
		// ����
		void PlayChunTianAnimation();
		// ����
		void PlayAlarmAnimation(uint16 chair_id);
		// ��ɫ�л�
		void PlaySwitchRoleAnimation(uint16 lord_user);
		// �Զ�����
		void AutoOutCard();
		// ��������
		void PlayGameSound(uint16 chair_id, uint8 card_type, uint8 card_data[], uint8 card_count);

	private:
		cocos2d::Sprite* spr_role_[kMaxPlayer];	// ��ɫ
		cocos2d::Sprite* spr_user_avatar_[kMaxPlayer];	// �û�ͷ��
		cocos2d::ui::Text* text_user_name_[kMaxPlayer];	// �û���
		cocos2d::ui::Text* text_user_score_[kMaxPlayer];	// �û�����
		cocos2d::Sprite* spr_timer_bg_;
		cocos2d::ui::TextBMFont* text_timer_number_;	// ʱ������
		cocos2d::Vec2 timer_pos_[kMaxPlayer];	// ��ʱ��λ��
		int timer_;
		uint32 timer_id_;
		cocos2d::Sprite* spr_ready_[kMaxPlayer];		// ׼��
		cocos2d::Sprite* spr_spreak_[kMaxPlayer];		// ������ʾ
		cocos2d::ui::TextBMFont* text_result_score_win_[kMaxPlayer];	// ��������
		cocos2d::ui::TextBMFont* text_result_score_loss_[kMaxPlayer];	// ��������
		cocos2d::Sprite* spr_result_back_;
		cocos2d::ui::Text* text_lord_score_;
		cocos2d::ui::Text* text_result_lord_score_;
		cocos2d::ui::Text* text_result_bomb_;
		cocos2d::ui::Text* text_result_chuntian_;
		cocos2d::ui::Text* text_result_times_;

		Card* myself_card_;
		BackCard* back_card_[kMaxPlayer];
		OutCard* out_card_[kMaxPlayer];

		cocos2d::ui::Button* btn_start_;		// ��ʼ��ť
		cocos2d::ui::Button* btn_call0_;		// ����
		cocos2d::ui::Button* btn_call1_;		// 1��
		cocos2d::ui::Button* btn_call2_;		// 2��
		cocos2d::ui::Button* btn_call3_;		// 3��
		cocos2d::ui::Button* btn_outcard_;		// ����
		cocos2d::ui::Button* btn_pass_;			// ����
		cocos2d::ui::Button* btn_hint_;			// ��ʾ
		cocos2d::ui::Button* btn_miss_;			// Ҫ����
		cocos2d::ui::Button* btBack;
		cocos2d::Sprite* spr_miss_;
		cocos2d::ui::Button* btn_auto_outcard_;			// �Զ�����
		cocos2d::ui::Button* btn_cancel_auto_outcard_;			// ȡ���Զ�����

		cocos2d::Vec2 send_cards_pos_[kMaxPlayer];	// ����λ��
		cocos2d::Vec2 bottom_cards_pos_[3];			// ����λ��
		bool bottom_cards_ani_finish_;

		// ����
		cocos2d::Sprite* spr_bomb_;
		cocos2d::Animate* ani_bomb_;
		cocos2d::Sprite* spr_rocket_;
		cocos2d::Sprite* spr_rocket_yan_;
		cocos2d::Animate* ani_rocket_;
		cocos2d::Sprite* spr_plane_;
		cocos2d::Animate* ani_plane_;
		cocos2d::Sprite* spr_chainpairs_;
		cocos2d::Animate* ani_chainpairs_;
		cocos2d::Sprite* spr_straight_;
		cocos2d::Animate* ani_straight_;
		cocos2d::Sprite* spr_chuntian_;
		cocos2d::Animate* ani_chuntian_;
		cocos2d::Sprite* spr_alarm_[kMaxPlayer];
		cocos2d::Animate* ani_alarm_;
		cocos2d::Sprite* spr_switch_role_[kMaxPlayer];
		cocos2d::Animate* ani_switch_role_[kMaxPlayer];

		GameKernel* client_kernel_;

		bool auto_out_card_;

		uint8 hand_card_data_[kMaxCardCount];
		uint8 hand_card_count_;
		uint8 bottom_card_data_[3];

		uint16 current_user_;
		uint16 lord_user_;
		uint8 lord_score_;

		uint8 turn_outcard_count_;
		uint8 turn_ountcard_data_[kMaxCardCount];

		SearchCardResult search_card_result_;
		uint8 search_card_result_index_;

		CMD_S_GameConclude game_conclude_;
	};

}

#endif // GAMES_DDZ_GAME_SCENE_H_
