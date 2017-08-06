
#ifndef GAMES_DDZ_CARD_H_
#define GAMES_DDZ_CARD_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../../base/basictypes.h"
#include "CMD_Ddz.h"

namespace ddz
{

	class OutCard;

	class Card : public cocos2d::Layer
	{
	public:
		Card();
		virtual ~Card();
		CREATE_FUNC(Card);

		virtual bool init();

		// 设置扑克数据
		void SetCardData(uint8* card_data, uint8 card_count);
		uint8 GetCardCount() { return card_count_; }
		// 设置扑克基准位置
		void SetCenterPosition(const cocos2d::Vec2& pos);
		// 设置响应
		void SetTouchEnable(bool enable) { touch_enable_ = enable; }
		// 设置出牌数据
		void SetOutCardData(OutCard* outcard);
		void SetOutCardPos(const cocos2d::Vec2& pos) { outcard_position_ = pos; }
		// 弹出扑克数量
		int GetShootCardCount();
		// 获取弹起扑克
		uint8 GetShootCardData(uint8 card_data_buffer[], uint8 card_buffer_count);
		// 设置弹起扑克
		void SetShootCardData(uint8 card_data[], uint8 card_count);

	private:
		virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
		virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
		virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
		virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

		void onMouseDown(cocos2d::Event* event);
		void onMouseUp(cocos2d::Event* event);

	public:
		typedef std::function<void()> MouseClickCallback;
		MouseClickCallback on_mouse_right_;
		MouseClickCallback On_mouse_left_;

	private:
		uint8 card_data_[kMaxCardCount];	// 扑克数据
		uint8 card_count_;					// 扑克数量
		cocos2d::Vec2 center_position_;		// 基准位置
		int last_select_index_;				// 上次选中索引
		int moving_index_;					// 移动索引
		bool double_click_;					// 双击标志
		bool touch_enable_;					// 是否可以操作
		cocos2d::Vec2 outcard_position_;		// 出牌位置
	};

}

#endif	// GAMES_DDZ_CARD_H_
