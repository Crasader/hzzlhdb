
#include "store_scene.h"

#include "cocostudio/CocoStudio.h"

#include "../base/convert.h"
#include "dialog_msg_box.h"
#include "../platform/coplatform.h"
#include "../data/global_user_info.h"

Scene* StoreScene::createScene()
{
	auto scene = Scene::create();

	auto layer = StoreScene::create();

	scene->addChild(layer);

	return scene;
}

StoreScene::StoreScene()
{
}

StoreScene::~StoreScene()
{
}

bool StoreScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(StoreScene::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(StoreScene::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(StoreScene::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(StoreScene::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("store.csb");
	addChild(ui);

	Button* btn_back = (Button*)Helper::seekWidgetByName((Widget*)ui, "back");
	btn_back->addClickEventListener(CC_CALLBACK_1(StoreScene::OnClickBack, this));

	Button* btn_back1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_0");
	btn_back1->addClickEventListener(CC_CALLBACK_1(StoreScene::OnClickBack, this));
	Button* btn_back2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_1");
	btn_back2->addClickEventListener(CC_CALLBACK_1(StoreScene::OnClickBack, this));
	Button* btn_back3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_2");
	btn_back3->addClickEventListener(CC_CALLBACK_1(StoreScene::OnClickBack, this));
	Button* btn_back4 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_3");
	btn_back4->addClickEventListener(CC_CALLBACK_1(StoreScene::OnClickBack, this));

	Button* Button_1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "Button_1");
	Button_1->addClickEventListener(CC_CALLBACK_1(StoreScene::OnClickBuy1, this));
	Button* Button_2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "Button_2");
	Button_2->addClickEventListener(CC_CALLBACK_1(StoreScene::OnClickBuy2, this));
	Button* Button_3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "Button_3");
	Button_3->addClickEventListener(CC_CALLBACK_1(StoreScene::OnClickBuy3, this));
	Button* Button_4 = (Button*)Helper::seekWidgetByName((Widget*)ui, "Button_4");
	Button_4->addClickEventListener(CC_CALLBACK_1(StoreScene::OnClickBuy4, this));

	return true;
}

void StoreScene::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
	//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOBBY_SCENE");
}

void StoreScene::OnClickCommodity(Ref* sender)
{
	commodity_list_->setVisible(true);
	record_list_->setVisible(false);
	btn_commodity_->setEnabled(false);
	btn_record_->setEnabled(true);
}

void StoreScene::OnClickRecord(Ref* sender)
{
	commodity_list_->setVisible(false);
	record_list_->setVisible(true);
	btn_commodity_->setEnabled(true);
	btn_record_->setEnabled(false);
}

void StoreScene::OnClickBuy1(Ref* sender)
{
	auto scene = RechargeSelect1::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void StoreScene::OnClickBuy2(Ref* sender)
{
	auto scene = RechargeSelect2::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void StoreScene::OnClickBuy3(Ref* sender)
{
	auto scene = RechargeSelect3::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void StoreScene::OnClickBuy4(Ref* sender)
{
	auto scene = RechargeSelect4::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void StoreScene::OnClickBuy5(Ref* sender)
{
	auto scene = RechargeSelect5::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void StoreScene::OnClickBuy6(Ref* sender)
{
	auto scene = RechargeSelect1::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

bool StoreScene::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}
void StoreScene::onTouchMoved(Touch *touch, Event *event)
{
}

void StoreScene::onTouchEnded(Touch *touch, Event *event)
{
}

void StoreScene::onTouchCancelled(Touch *touch, Event *event)
{
}