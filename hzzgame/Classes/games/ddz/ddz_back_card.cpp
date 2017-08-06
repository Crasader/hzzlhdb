
#include "ddz_back_card.h"

USING_NS_CC;
using namespace cocos2d::ui;

namespace ddz
{

	const float kCardSpace = 5.0f;
	const float kSkewDigree = 25.0f;

	BackCard::BackCard()
		: chair_id_(1), card_count_(0), text_card_number_(nullptr)
	{

	}

	BackCard::~BackCard()
	{
		text_card_number_->release();
	}

	bool BackCard::init()
	{
		if (!Layer::init())
		{
			return false;
		}

		text_card_number_ = TextBMFont::create("", "ddz/Resources/Image/card_number/card_number_font.fnt");
		if (chair_id_ != 1)
			addChild(text_card_number_);
		text_card_number_->retain();

		return true;
	}

	void BackCard::SetCardCount(uint8 card_count)
	{
		removeAllChildren();
		card_count_ = card_count;
		if (card_count == 0)
			return;

		Texture2D* tex_card;
		if (chair_id_ == 0)
		{
			tex_card = Director::getInstance()->getTextureCache()->addImage("ddz/Resources/Image/tableCardBackLeft.png");
		}
		else if (chair_id_ == 2)
		{
			tex_card = Director::getInstance()->getTextureCache()->addImage("ddz/Resources/Image/tableCardBackRight.png");
		}
		else
		{
			tex_card = Director::getInstance()->getTextureCache()->addImage("ddz/Resources/Image/cards/card_00.png");
		}

		auto card_item = Sprite::createWithTexture(tex_card);
		for (uint8 i = 0; i < card_count; ++i)
		{
			card_item = Sprite::createWithTexture(tex_card);
			card_item->setTag(i + 1);
			if (chair_id_ == 0)
			{
				card_item->setPosition(Vec2(position_.x + i * 5.0f, position_.y + i * kCardSpace));
				addChild(card_item, card_count - i - 1);
			}
			else if (chair_id_ == 2)
			{
				card_item->setPosition(Vec2(position_.x - i * 5.0f, position_.y + i * kCardSpace));
				addChild(card_item, card_count - i - 1);
			}
			else
			{
				card_item->setPosition(Vec2(position_.x + i * kCardSpace, position_.y));
				addChild(card_item);
			}
		}

		if (chair_id_ != 1)
		{
			char temp[128] = { 0 };
			sprintf(temp, "%d", card_count_);
			text_card_number_->setString(temp);
			addChild(text_card_number_, 100);
		}
	}

	void BackCard::SetCenterPosition(const Vec2& pos)
	{
		position_ = pos;
		text_card_number_->setPosition(pos);
	}

	Vec2 BackCard::GetRailCardPos()
	{
		if (card_count_ == 0)
			return Vec2(0, 0);
		return getChildByTag(card_count_)->getPosition();
	}

}