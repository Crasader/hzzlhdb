
#include "WXShare.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "../platform/coplatform.h"
#include "dialog_msg_box.h"
#include "lobby_scene.h"

static WXShare * g_pWXShare = nullptr;

WXShare * WXShare::getInstance()
{
	return g_pWXShare;
}

Scene* WXShare::createScene()
{
	auto scene = Scene::create();

	auto layer = WXShare::create();

	scene->addChild(layer);

	return scene;
}

WXShare::WXShare()
{
	g_pWXShare = this;
}

WXShare::~WXShare()
{
}

bool WXShare::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(WXShare::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(WXShare::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(WXShare::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(WXShare::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("WXShare.csb");
	addChild(ui);

	Button* btBack = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close");
	btBack->addClickEventListener(CC_CALLBACK_1(WXShare::OnClickBack, this));

	Button* btBack0 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close_0");
	btBack0->addClickEventListener(CC_CALLBACK_1(WXShare::OnClickBack, this));
	Button* btBack1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close_1");
	btBack1->addClickEventListener(CC_CALLBACK_1(WXShare::OnClickBack, this));
	Button* btBack2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close_2");
	btBack2->addClickEventListener(CC_CALLBACK_1(WXShare::OnClickBack, this));
	Button* btBack3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close_3");
	btBack3->addClickEventListener(CC_CALLBACK_1(WXShare::OnClickBack, this));

	Image1 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "haoyou");
	Image1->addClickEventListener(CC_CALLBACK_1(WXShare::OnClickWXShare1, this));
	Image1->setEnabled(true);

	Image2 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "pengyouquan");
	Image2->addClickEventListener(CC_CALLBACK_1(WXShare::OnClickWXShare2, this));
	Image2->setEnabled(true);

	return true;
}

bool WXShare::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void WXShare::onTouchMoved(Touch *touch, Event *event)
{
}

void WXShare::onTouchEnded(Touch *touch, Event *event)
{
}

void WXShare::onTouchCancelled(Touch *touch, Event *event)
{
}

void WXShare::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	//GlobalUserData* pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	//PACKET_HELPER_SIZE(128);
	//packet.write4Byte(pGlobalUserData->user_id);
	//PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_FENXIANG_SUCCESS, packet.getBuffer(), packet.getPosition());
}

void WXShare::onExit()
{
	Layer::onExit();
}

//关闭窗口
void WXShare::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

//分享个人
void WXShare::OnClickWXShare1(Ref* sender)
{
	/*
	std::string neirong = "testtest";
	std::string strRecharge;
	strRecharge = "{\"act\":5011 ,\"neirong\":\"";
	strRecharge += neirong;
	strRecharge += "\"}";
	platformAction(a2u(strRecharge));

	*/
	//Image1->setEnabled(false);
	platformAction("{\"act\":501 }");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	this->schedule(schedule_selector(WXShare::WaitWXfxcg), 1.0f);
#endif
}

//分享朋友圈
void WXShare::OnClickWXShare2(Ref* sender)
{
	//GlobalUserData* pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	//PACKET_HELPER_SIZE(512);
	//packet.write4Byte(pGlobalUserData->user_id);
	//PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_FENXIANG_SUCCESS, packet.getBuffer(), packet.getPosition());
	//return;
	/*
	std::string neirong = "testtest";
	std::string strRecharge;
	strRecharge = "{\"act\":5001 ,\"neirong\":\"";
	strRecharge += neirong;
	strRecharge += "\"}";
	platformAction(a2u(strRecharge));
	*/
	//Image2->setEnabled(false);
	platformAction("{\"act\":500 }");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	this->schedule(schedule_selector(WXShare::WaitWXfxcg), 1.0f);
#endif
}

void WXShare::WaitWXfxcg(float dt)
{

	std::string code = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	code = platformAction("{\"act\":701 }");
	if ("0" == code)
	{
		return;
	}
	else
	{

		//MsgBox(a2u("分享成功"));

		LobbyScene::getInstance()->UpdateUserScore();
		GlobalUserData* pGlobalUserData = g_global_userinfo.GetGlobalUserData();
		PACKET_HELPER_SIZE(512);
		packet.write4Byte(pGlobalUserData->user_id);
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_FENXIANG_SUCCESS, packet.getBuffer(), packet.getPosition());

		this->unschedule(schedule_selector(WXShare::WaitWXfxcg));
	}
#endif
}