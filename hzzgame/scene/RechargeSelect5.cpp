
#include "RechargeSelect5.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "../platform/coplatform.h"

static RechargeSelect5 * g_pRechargeSelect5 = nullptr;

RechargeSelect5 * RechargeSelect5::getInstance()
{
	return g_pRechargeSelect5;
}

Scene* RechargeSelect5::createScene()
{
    auto scene = Scene::create();
    
	auto layer = RechargeSelect5::create();

    scene->addChild(layer);

    return scene;
}

RechargeSelect5::RechargeSelect5()
{
	g_pRechargeSelect5 = this;
}

RechargeSelect5::~RechargeSelect5()
{
}

bool RechargeSelect5::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(RechargeSelect5::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(RechargeSelect5::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(RechargeSelect5::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(RechargeSelect5::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("RechargeSelect.csb");
	addChild(ui);
    
	Button* btBack = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close");
	btBack->addClickEventListener(CC_CALLBACK_1(RechargeSelect5::OnClickBack, this));

	auto Image1 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "Image_1");
	Image1->addClickEventListener(CC_CALLBACK_1(RechargeSelect5::OnClickRecharge1, this));
	
	auto Image2 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "Image_2");
	Image2->addClickEventListener(CC_CALLBACK_1(RechargeSelect5::OnClickRecharge2, this));

    return true;
}

bool RechargeSelect5::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void RechargeSelect5::onTouchMoved(Touch *touch, Event *event)
{
}

void RechargeSelect5::onTouchEnded(Touch *touch, Event *event)
{
}

void RechargeSelect5::onTouchCancelled(Touch *touch, Event *event)
{
}

void RechargeSelect5::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
}

void RechargeSelect5::onExit()
{
	Layer::onExit();
}

//关闭窗口
void RechargeSelect5::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

//支付宝
void RechargeSelect5::OnClickRecharge1(Ref* sender)
{
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	std::string strRecharge;
	strRecharge = "{\"act\":300 ,\"propName\":\"宝箱\",\"propInfo\":\"";
	strRecharge += pGlobalUserData->accounts.c_str();
	strRecharge += "\",\"propPice\":\"0.05\"}";
	platformAction(a2u(strRecharge));
}

//微信
void RechargeSelect5::OnClickRecharge2(Ref* sender)
{
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	std::string strRecharge;
	strRecharge = "{\"act\":400 ,\"propName\":\"微信充值\",\"propInfo\":\"";
	strRecharge += StringUtils::format("%ld", pGlobalUserData->user_id);
	strRecharge += "\",\"propPice\":\"5\"}";
	platformAction(a2u(strRecharge));
}