
#include "ddz_out_card.h"
#include <limits>

USING_NS_CC;

namespace ddz
{

	const float kCardSpace = 25;
	const float kCardWidth = 121.0f;
	const float kCardHeight = 162.0f;

	OutCard::OutCard()
		: card_count_(0), chair_id_(0)
	{
		memset(card_data_, 0, sizeof(card_data_));
	}

	OutCard::~OutCard()
	{

	}

	bool OutCard::init()
	{
		if (!Layer::init())
		{
			return false;
		}

		return true;
	}

	// 设置扑克数据
	void OutCard::SetCardData(uint8* card_data, uint8 card_count, bool anim)
	{
		removeAllChildren();
		if (card_count == 0)
		{
			memset(card_data_, 0, sizeof(card_data_));
			card_count_ = 0;
		}
		else
		{
			memcpy(card_data_, card_data, card_count * sizeof(uint8));
			card_count_ = card_count;

			Vec2 pos;
			int count = std::min((int)card_count, 9);
			if (chair_id_ == 0)
			{
				pos.x = position_.x;
			}
			else if (chair_id_ == 2)
			{
				pos.x = position_.x - (count - 1) * kCardSpace;
			}
			else
			{
				pos.x = position_.x - ((count - 1) * kCardSpace + kCardWidth) / 2.0f + kCardWidth / 2;
			}
			pos.y = position_.y;

			char temp[128] = { 0 };
			uint8 i = 0;
			for (uint8 index = 0; index < card_count; ++index, i++)
			{
				if (index == 9)
				{
					i = 0;
					pos.y -= kCardWidth / 2.0f;
				}

				if (card_data_[index] == 0x4E || card_data_[index] == 0x4F)
				{
					sprintf(temp, "ddz/Resources/Image/cards/card_%02d.png", card_data_[index] == 0x4E ? 53 : 54);
				}
				else
				{
					sprintf(temp, "ddz/Resources/Image/cards/card_%02d.png", (card_data_[index] & MASK_VALUE) + ((card_data_[index] & MASK_COLOR) >> 4) * 13);
				}
				auto card_item = Sprite::create(temp);
				if (anim)
				{
					card_item->setPosition(dispatch_pos_);
				}
				else
				{
					card_item->setPosition(Vec2(pos.x + i * kCardSpace, pos.y));
				}
				card_item->setScale(0.66f);
				addChild(card_item);
				if (anim)
					card_item->runAction(Sequence::create(DelayTime::create(i * 0.05f), MoveTo::create(0.1f, Vec2(pos.x + i * kCardSpace, pos.y)), nullptr));
			}
		}
	}

	// 设置扑克基准位置
	void OutCard::SetPosition(const Vec2& pos)
	{
		position_ = pos;
	}

	// 获取中心位置
	Vec2 OutCard::GetCenterPos()
	{
		Vec2 pos;
		if (chair_id_ == 0)
		{
			pos.x = position_.x + ((card_count_ - 1) * kCardSpace) / 2.0f;;
		}
		else if (chair_id_ == 2)
		{
			pos.x = position_.x - ((card_count_ - 1) * kCardSpace) / 2.0f;
		}
		else
		{
			pos.x = position_.x;
		}
		pos.y = position_.y;
		return pos;
	}

}
