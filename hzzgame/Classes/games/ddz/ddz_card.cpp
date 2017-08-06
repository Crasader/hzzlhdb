
#include "ddz_card.h"
#include "ddz_out_card.h"

USING_NS_CC;
using namespace cocos2d::ui;

namespace ddz
{

	const float kCardSpace = 40;
	const float kCardShootHeight = 20;

	const float kCardWidth = 121.0f;
	const float kCardHeight = 162.0f;

	Card::Card()
		: card_count_(0), last_select_index_(-1), moving_index_(-1), double_click_(false), touch_enable_(false), on_mouse_right_(nullptr), On_mouse_left_(nullptr)
	{
		memset(card_data_, 0, sizeof(card_data_));
	}

	Card::~Card()
	{

	}

	bool Card::init()
	{
		if (!Layer::init())
		{
			return false;
		}

		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = CC_CALLBACK_2(Card::onTouchBegan, this);
		listener->onTouchMoved = CC_CALLBACK_2(Card::onTouchMoved, this);
		listener->onTouchEnded = CC_CALLBACK_2(Card::onTouchEnded, this);
		listener->onTouchCancelled = CC_CALLBACK_2(Card::onTouchCancelled, this);
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

		auto mouse_listener = EventListenerMouse::create();
		mouse_listener->onMouseUp = CC_CALLBACK_1(Card::onMouseUp, this);
		mouse_listener->onMouseDown = CC_CALLBACK_1(Card::onMouseDown, this);

		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouse_listener, this);

		return true;
	}

	// 设置扑克数据
	void Card::SetCardData(uint8* card_data, uint8 card_count)
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
			pos.x = center_position_.x - ((card_count_ - 1) * kCardSpace + kCardWidth) / 2.0f + kCardWidth / 2;
			pos.y = center_position_.y;

			char temp[128] = { 0 };
			for (uint8 i = 0; i < card_count; ++i)
			{
				if (card_data_[i] == 0x4E || card_data_[i] == 0x4F)
				{
					sprintf(temp, "ddz/Resources/Image/cards/card_%02d.png", card_data_[i] == 0x4E ? 53 : 54);
				}
				else
				{
					sprintf(temp, "ddz/Resources/Image/cards/card_%02d.png", (card_data_[i] & MASK_VALUE) + ((card_data_[i] & MASK_COLOR) >> 4) * 13);
				}
				auto card_item = Sprite::create(temp);
				card_item->setPosition(Vec2(pos.x + i * kCardSpace, pos.y));
				card_item->setTag(0);
				addChild(card_item);
			}
		}
	}

	// 设置扑克基准位置
	void Card::SetCenterPosition(const Vec2& pos)
	{
		center_position_ = pos;
	}

	// 设置出牌数据
	void Card::SetOutCardData(OutCard* outcard)
	{
		int shoot_count = GetShootCardCount();
		Vec2 pos;
		pos.x = outcard_position_.x - ((shoot_count - 1) * 25.0f + kCardWidth * 0.66f) / 2.0f + kCardWidth * 0.66f / 2;
		pos.y = outcard_position_.y;
		int count = 0;
		for (int i = 0; i < getChildrenCount(); ++i)
		{
			Sprite* spr = dynamic_cast<Sprite*>(getChildren().at(i));
			if (spr->getTag() != 0)
			{
				Sprite* spr_new = Sprite::createWithTexture(spr->getTexture());
				spr_new->setScale(0.66f);
				spr_new->setPosition(spr->getPosition());
				spr_new->runAction(Sequence::create(DelayTime::create(count * 0.05f), MoveTo::create(0.1f, Vec2(pos.x + count * 25.0f, pos.y)), nullptr));
				outcard->addChild(spr_new);
				count++;
			}
		}
	}

	// 弹出扑克数量
	int Card::GetShootCardCount()
	{
		int count = 0;
		for (int i = 0; i < getChildrenCount(); ++i)
		{
			Node* node = getChildren().at(i);
			if (node->getTag() != 0)
				count++;
		}
		return count;
	}

	uint8 Card::GetShootCardData(uint8 card_data_buffer[], uint8 card_buffer_count)
	{
		uint8 count = 0;
		for (int i = 0; i < getChildrenCount(); ++i)
		{
			Node* node = getChildren().at(i);
			if (node->getTag() != 0)
			{
				card_data_buffer[count++] = card_data_[i];
			}
		}
		return count;
	}

	// 设置弹起扑克
	void Card::SetShootCardData(uint8 card_data[], uint8 card_count)
	{
		for (int i = 0; i < getChildrenCount(); ++i)
		{
			Node* node = getChildren().at(i);
			if (node->getTag() != 0)
			{
				node->setTag(0);
				node->setPosition(Vec2(getChildren().at(i)->getPositionX(), getChildren().at(i)->getPositionY() - kCardShootHeight));
			}
		}

		for (uint8 i = 0; i < card_count; ++i)
		{
			for (int j = 0; j < getChildrenCount(); ++j)
			{
				Node* node = getChildren().at(j);
				if (node->getTag() == 0 && card_data[i] == card_data_[j])
				{
					node->setTag(1);
					node->setPosition(Vec2(getChildren().at(j)->getPositionX(), getChildren().at(j)->getPositionY() + kCardShootHeight));
				}
			}
		}
	}

	bool Card::onTouchBegan(Touch *touch, Event *event)
	{
		if (!touch_enable_)
			return false;
		auto target = event->getCurrentTarget();
		Point locationInNode = target->convertToNodeSpace(touch->getLocation());

		for (uint8 i = 0; i < card_count_; ++i)
		{
			uint8 index = card_count_ - i - 1;
			Rect rc_item = getChildren().at(index)->getBoundingBox();
			if (rc_item.containsPoint(locationInNode))
			{
				last_select_index_ = index;
				break;
			}
		}

		if (last_select_index_ == -1)
		{
			if (double_click_)
			{
				double_click_ = false;
				for (uint8 i = 0; i < card_count_; ++i)
				{
					if (getChildren().at(i)->getTag() != 0)
					{
						getChildren().at(i)->setTag(0);
						getChildren().at(i)->setPosition(Vec2(getChildren().at(i)->getPositionX(), getChildren().at(i)->getPositionY() - kCardShootHeight));
					}
				}
				if (On_mouse_left_ != nullptr)
					On_mouse_left_();
			}
			else
			{
				double_click_ = true;
				scheduleOnce([&](float dt){ double_click_ = false; }, 0.25f, "card_double_click");
			}
		}

		return true;
	}

	void Card::onTouchMoved(Touch *touch, Event *event)
	{
		if (!touch_enable_)
			return;

		moving_index_ = -1;
		if (last_select_index_ != -1)
		{
			auto target = event->getCurrentTarget();
			Point locationInNode = target->convertToNodeSpace(touch->getLocation());

			Rect rc_layer = getBoundingBox();

			if (locationInNode.x < getChildren().at(0)->getPosition().x)
			{
				moving_index_ = 0;
			}
			else if (locationInNode.x > getChildren().at(card_count_ - 1)->getPosition().x)
			{
				moving_index_ = card_count_ - 1;
			}

			for (uint8 i = 0; i < card_count_; ++i)
			{
				uint8 index = card_count_ - i - 1;
				Rect rc_item = getChildren().at(index)->getBoundingBox();
				rc_item.setRect(rc_item.origin.x, 0, rc_item.size.width, rc_layer.size.height);
				if (rc_item.containsPoint(locationInNode))
				{
					moving_index_ = index;
					break;
				}
			}

			int head = moving_index_ > last_select_index_ ? last_select_index_ : moving_index_;
			int tail = moving_index_ > last_select_index_ ? moving_index_ : last_select_index_;
			for (int i = 0; i < card_count_; ++i)
			{
				if (i >= head && i <= tail)
				{
					Sprite* spr = dynamic_cast<Sprite*>(getChildren().at(i));
					BlendFunc bf = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
					spr->setBlendFunc(bf);
					spr->setColor(Color3B(128, 128, 128));
				}
				else
				{
					Sprite* spr = dynamic_cast<Sprite*>(getChildren().at(i));
					spr->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
					spr->setColor(Color3B(255, 255, 255));
				}
			}
		}
	}

	void Card::onTouchEnded(Touch *touch, Event *event)
	{
		if (!touch_enable_)
			return;

		if (last_select_index_ == -1)
			return;

		if (last_select_index_ == moving_index_ || moving_index_ == -1)
		{
			int tag = getChildren().at(last_select_index_)->getTag();
			if (tag == 0)
			{
				getChildren().at(last_select_index_)->setPosition(Vec2(getChildren().at(last_select_index_)->getPositionX(), getChildren().at(last_select_index_)->getPositionY() + kCardShootHeight));
				getChildren().at(last_select_index_)->setTag(1);
			}
			else
			{
				getChildren().at(last_select_index_)->setPosition(Vec2(getChildren().at(last_select_index_)->getPositionX(), getChildren().at(last_select_index_)->getPositionY() - kCardShootHeight));
				getChildren().at(last_select_index_)->setTag(0);
			}
			Sprite* spr = dynamic_cast<Sprite*>(getChildren().at(last_select_index_));
			spr->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
			spr->setColor(Color3B(255, 255, 255));
			last_select_index_ = -1;
			moving_index_ = -1;

			if (On_mouse_left_ != nullptr)
				On_mouse_left_();

			return;
		}

		if (moving_index_ != -1)
		{
			int head = moving_index_ > last_select_index_ ? last_select_index_ : moving_index_;
			int tail = moving_index_ > last_select_index_ ? moving_index_ : last_select_index_;
			for (int i = head; i <= tail; ++i)
			{
				int tag = getChildren().at(i)->getTag();
				if (tag == 0)
				{
					getChildren().at(i)->setPosition(Vec2(getChildren().at(i)->getPositionX(), getChildren().at(i)->getPositionY() + kCardShootHeight));
					getChildren().at(i)->setTag(1);
				}
				else
				{
					getChildren().at(i)->setPosition(Vec2(getChildren().at(i)->getPositionX(), getChildren().at(i)->getPositionY() - kCardShootHeight));
					getChildren().at(i)->setTag(0);
				}
				Sprite* spr = dynamic_cast<Sprite*>(getChildren().at(i));
				spr->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
				spr->setColor(Color3B(255, 255, 255));
			}
		}

		if (On_mouse_left_ != nullptr)
			On_mouse_left_();

		last_select_index_ = -1;
		moving_index_ = -1;
	}

	void Card::onTouchCancelled(Touch *touch, Event *event)
	{

	}

	void Card::onMouseDown(Event *event)
	{
		EventMouse* e = (EventMouse*)event;
		if (e->getMouseButton() == 1)	// 右键
		{
			if (on_mouse_right_ != nullptr)
				on_mouse_right_();
		}
	}

	void Card::onMouseUp(Event *event)
	{
		EventMouse* e = (EventMouse*)event;
	}

}
