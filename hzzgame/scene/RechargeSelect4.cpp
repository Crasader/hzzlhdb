
#include "RechargeSelect4.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "../platform/coplatform.h"

static RechargeSelect4 * g_pRechargeSelect4 = nullptr;

RechargeSelect4 * RechargeSelect4::getInstance()
{
	return g_pRechargeSelect4;
}

Scene* RechargeSelect4::createScene()
{
    auto scene = Scene::create();
    
	auto layer = RechargeSelect4::create();

    scene->addChild(layer);

    return scene;
}

RechargeSelect4::RechargeSelect4()
{
	g_pRechargeSelect4 = this;
}

RechargeSelect4::~RechargeSelect4()
{
}

bool RechargeSelect4::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(RechargeSelect4::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(RechargeSelect4::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(RechargeSelect4::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(RechargeSelect4::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("RechargeSelect.csb");
	addChild(ui);
    
	Button* btBack = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close");
	btBack->addClickEventListener(CC_CALLBACK_1(RechargeSelect4::OnClickBack, this));

	auto Image1 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "zhifubao");
	Image1->addClickEventListener(CC_CALLBACK_1(RechargeSelect4::OnClickRecharge1, this));
	
	auto Image2 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "weixin");
	Image2->addClickEventListener(CC_CALLBACK_1(RechargeSelect4::OnClickRecharge2, this));

    return true;
}

bool RechargeSelect4::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void RechargeSelect4::onTouchMoved(Touch *touch, Event *event)
{
}

void RechargeSelect4::onTouchEnded(Touch *touch, Event *event)
{
}

void RechargeSelect4::onTouchCancelled(Touch *touch, Event *event)
{
}

void RechargeSelect4::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
}

void RechargeSelect4::onExit()
{
	Layer::onExit();
}

//关闭窗口
void RechargeSelect4::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

//支付宝
void RechargeSelect4::OnClickRecharge1(Ref* sender)
{
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	std::string strRecharge;
	strRecharge = "{\"act\":300 ,\"propName\":\"宝箱\",\"propInfo\":\"";
	strRecharge += pGlobalUserData->accounts.c_str();
	strRecharge += "\",\"propPice\":\"100.00\"}";
	platformAction(a2u(strRecharge));
}

//微信
void RechargeSelect4::OnClickRecharge2(Ref* sender)
{
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	std::string strRecharge;
	strRecharge = "{\"act\":400 ,\"propName\":\"微信充值\",\"propInfo\":\"";
	strRecharge += StringUtils::format("%ld", pGlobalUserData->user_id);
	strRecharge += "\",\"propPice\":\"10000\"}";
	platformAction(a2u(strRecharge));
}