
#include "RechargeSelect3.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "../platform/coplatform.h"

static RechargeSelect3 * g_pRechargeSelect3 = nullptr;

RechargeSelect3 * RechargeSelect3::getInstance()
{
	return g_pRechargeSelect3;
}

Scene* RechargeSelect3::createScene()
{
    auto scene = Scene::create();
    
	auto layer = RechargeSelect3::create();

    scene->addChild(layer);

    return scene;
}

RechargeSelect3::RechargeSelect3()
{
	g_pRechargeSelect3 = this;
}

RechargeSelect3::~RechargeSelect3()
{
}

bool RechargeSelect3::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(RechargeSelect3::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(RechargeSelect3::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(RechargeSelect3::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(RechargeSelect3::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("RechargeSelect.csb");
	addChild(ui);
    
	Button* btBack = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close");
	btBack->addClickEventListener(CC_CALLBACK_1(RechargeSelect3::OnClickBack, this));

	auto Image1 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "zhifubao");
	Image1->addClickEventListener(CC_CALLBACK_1(RechargeSelect3::OnClickRecharge1, this));
	
	auto Image2 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "weixin");
	Image2->addClickEventListener(CC_CALLBACK_1(RechargeSelect3::OnClickRecharge2, this));

    return true;
}

bool RechargeSelect3::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void RechargeSelect3::onTouchMoved(Touch *touch, Event *event)
{
}

void RechargeSelect3::onTouchEnded(Touch *touch, Event *event)
{
}

void RechargeSelect3::onTouchCancelled(Touch *touch, Event *event)
{
}

void RechargeSelect3::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
}

void RechargeSelect3::onExit()
{
	Layer::onExit();
}

//关闭窗口
void RechargeSelect3::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

//支付宝
void RechargeSelect3::OnClickRecharge1(Ref* sender)
{
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	std::string strRecharge;
	strRecharge = "{\"act\":300 ,\"propName\":\"宝箱\",\"propInfo\":\"";
	strRecharge += pGlobalUserData->accounts.c_str();
	strRecharge += "\",\"propPice\":\"50.00\"}";
	platformAction(a2u(strRecharge));
}

//微信
void RechargeSelect3::OnClickRecharge2(Ref* sender)
{
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	std::string strRecharge;
	strRecharge = "{\"act\":400 ,\"propName\":\"微信充值\",\"propInfo\":\"";
	strRecharge += StringUtils::format("%ld", pGlobalUserData->user_id);
	strRecharge += "\",\"propPice\":\"5000\"}";
	platformAction(a2u(strRecharge));
}