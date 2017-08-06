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
			SPEAK_CALL0 = 0,	// 不叫
			SPEAK_CALL1,
			SPEAK_CALL2,
			SPEAK_CALL3,
			SPEAK_PASS,			// 不出

			SPEAK_COUNT			// 不再显示
		};

	public:
		GameScene();
		virtual ~GameScene();
		static cocos2d::Scene* createScene();

		virtual bool init();
		virtual void onEnterTransitionDidFinish();
		virtual void onExit();

		//退出游戏
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

		// 游戏开始
		bool OnSubGameStart(void* data, WORD data_size);
		// 用户叫分
		bool OnSubCallScore(void* data, WORD data_size);
		// 地主信息
		bool OnSubLordInfo(void* data, WORD data_size);
		// 用户出牌
		bool OnSubOutCard(void* data, WORD data_size);
		// 用户放弃
		bool OnSubPassCard(void* data, WORD data_size);
		// 游戏结束
		bool OnSubGameConclude(void* data, WORD data_size);

		uint16 ViewChairID(uint16 chair_id);

	private:
		// 显示计时器
		void ShowGameTimer(uint16 chair_id, uint32 timer_id, int sec);
		// 隐藏计时器
		void HideGameTimer();
		// 计时器回调
		void OnGameTimer(float dt);
		// 发牌
		void SendCards(uint16 first_user);
		// 发牌回调
		void OnSendCardsCallback(cocos2d::Node* node);
		// 开始按钮
		void OnClickStart(cocos2d::Ref* sender);
		// 不叫
		void OnClickCall0(cocos2d::Ref* sender);
		// 1分
		void OnClickCall1(cocos2d::Ref* sender);
		// 2分
		void OnClickCall2(cocos2d::Ref* sender);
		// 3分
		void OnClickCall3(cocos2d::Ref* sender);
		// 出牌
		void OnClickOutCard(cocos2d::Ref* sender);
		// 不出
		void OnClickPass(cocos2d::Ref* sender);
		// 提示
		void OnClickHint(cocos2d::Ref* sender);
		// 要不起
		void OnClickMiss(cocos2d::Ref* sender);
		// 自动出牌
		void OnClickAutoOutCard(cocos2d::Ref* sender);
		// 取消自动出牌
		void OnClickCancelAutoOutCard(cocos2d::Ref* sender);
		// 鼠标右键
		void OnCardMouseRight();
		// 鼠标左键
		void OnCardMouseLeft();
		// 说话内容
		void SetSpeakContent(uint16 chair_id, SpeakContent sc);
		// 显示底牌
		void ShowBottomCards();
		// 出牌判断
		bool VerdictOutCard();
		// 炸弹动画
		void PlayBombAnimation(cocos2d::Vec2 pos);
		// 火箭动画
		void PlayRocketAnimation();
		// 飞机动画
		void PlayPlaneAnimation();
		// 连对
		void PlayChainPairsAnimation(cocos2d::Vec2 pos);
		// 顺子
		void PlayStraightAnimation(cocos2d::Vec2 pos);
		// 春天
		void PlayChunTianAnimation();
		// 警报
		void PlayAlarmAnimation(uint16 chair_id);
		// 角色切换
		void PlaySwitchRoleAnimation(uint16 lord_user);
		// 自动出牌
		void AutoOutCard();
		// 播放声音
		void PlayGameSound(uint16 chair_id, uint8 card_type, uint8 card_data[], uint8 card_count);

	private:
		cocos2d::Sprite* spr_role_[kMaxPlayer];	// 角色
		cocos2d::Sprite* spr_user_avatar_[kMaxPlayer];	// 用户头像
		cocos2d::ui::Text* text_user_name_[kMaxPlayer];	// 用户名
		cocos2d::ui::Text* text_user_score_[kMaxPlayer];	// 用户分数
		cocos2d::Sprite* spr_timer_bg_;
		cocos2d::ui::TextBMFont* text_timer_number_;	// 时器数字
		cocos2d::Vec2 timer_pos_[kMaxPlayer];	// 计时器位置
		int timer_;
		uint32 timer_id_;
		cocos2d::Sprite* spr_ready_[kMaxPlayer];		// 准备
		cocos2d::Sprite* spr_spreak_[kMaxPlayer];		// 气泡提示
		cocos2d::ui::TextBMFont* text_result_score_win_[kMaxPlayer];	// 结算数字
		cocos2d::ui::TextBMFont* text_result_score_loss_[kMaxPlayer];	// 结算数字
		cocos2d::Sprite* spr_result_back_;
		cocos2d::ui::Text* text_lord_score_;
		cocos2d::ui::Text* text_result_lord_score_;
		cocos2d::ui::Text* text_result_bomb_;
		cocos2d::ui::Text* text_result_chuntian_;
		cocos2d::ui::Text* text_result_times_;

		Card* myself_card_;
		BackCard* back_card_[kMaxPlayer];
		OutCard* out_card_[kMaxPlayer];

		cocos2d::ui::Button* btn_start_;		// 开始按钮
		cocos2d::ui::Button* btn_call0_;		// 不叫
		cocos2d::ui::Button* btn_call1_;		// 1分
		cocos2d::ui::Button* btn_call2_;		// 2分
		cocos2d::ui::Button* btn_call3_;		// 3分
		cocos2d::ui::Button* btn_outcard_;		// 出牌
		cocos2d::ui::Button* btn_pass_;			// 不出
		cocos2d::ui::Button* btn_hint_;			// 提示
		cocos2d::ui::Button* btn_miss_;			// 要不起
		cocos2d::ui::Button* btBack;
		cocos2d::Sprite* spr_miss_;
		cocos2d::ui::Button* btn_auto_outcard_;			// 自动出牌
		cocos2d::ui::Button* btn_cancel_auto_outcard_;			// 取消自动出牌

		cocos2d::Vec2 send_cards_pos_[kMaxPlayer];	// 发牌位置
		cocos2d::Vec2 bottom_cards_pos_[3];			// 底牌位置
		bool bottom_cards_ani_finish_;

		// 动画
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
