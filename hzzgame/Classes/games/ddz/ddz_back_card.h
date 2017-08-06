
#ifndef GAMES_DDZ_BACK_CARD_H_
#define GAMES_DDZ_BACK_CARD_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../../base/basictypes.h"

namespace ddz
{

	class BackCard : public cocos2d::Layer
	{
	public:
		BackCard();
		virtual ~BackCard();
		CREATE_FUNC(BackCard);

		virtual bool init();

		void SetCardCount(uint8 card_count);
		void SetCenterPosition(const cocos2d::Vec2& pos);
		void SetChairID(uint16 id) { chair_id_ = id; }
		cocos2d::Vec2 GetRailCardPos();
		uint8 GetCardCount() const { return card_count_; }

	private:
		cocos2d::Vec2 position_;
		uint8 chair_id_;
		uint8 card_count_;
		cocos2d::ui::TextBMFont* text_card_number_;
	};

}

#endif	// GAMES_DDZ_BACK_CARD_H_
