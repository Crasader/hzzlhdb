
#include "UIDialog.h"


Dialog::Dialog()
{
}

Dialog::~Dialog()
{

}

bool Dialog::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(Dialog::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Dialog::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(Dialog::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(Dialog::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

bool Dialog::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void Dialog::onTouchMoved(Touch *touch, Event *event)
{
}

void Dialog::onTouchEnded(Touch *touch, Event *event)
{
}

void Dialog::onTouchCancelled(Touch *touch, Event *event)
{
}
