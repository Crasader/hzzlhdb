
#ifndef GAMES_DDZ_OUT_CARD_H_
#define GAMES_DDZ_OUT_CARD_H_

#include "cocos2d.h"

#include "../../base/basictypes.h"
#include "CMD_Ddz.h"

namespace ddz
{

	class OutCard : public cocos2d::Layer
	{
	public:
		OutCard();
		virtual ~OutCard();
		CREATE_FUNC(OutCard);

		virtual bool init();

		// 设置扑克数据
		void SetCardData(uint8* card_data, uint8 card_count, bool anim = true);
		// 设置扑克基准位置
		void SetPosition(const cocos2d::Vec2& pos);
		void SetDispatchPos(const cocos2d::Vec2& pos) { dispatch_pos_ = pos; }
		// 设置坐位
		void SetChairID(uint16 chair_id) { chair_id_ = chair_id; }
		// 获取中心位置
		cocos2d::Vec2 GetCenterPos();

	private:
		uint8 card_data_[kMaxCardCount];	// 扑克数据
		uint8 card_count_;					// 扑克数量
		cocos2d::Vec2 position_;		// 基准位置
		cocos2d::Vec2 dispatch_pos_;	// 发牌动画起始位置
		uint16 chair_id_;
	};

}

#endif	// GAMES_DDZ_OUT_CARD_H_