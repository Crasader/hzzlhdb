
#include "RechargeSelect1.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "../platform/coplatform.h"

static RechargeSelect1 * g_pRechargeSelect1 = nullptr;

RechargeSelect1 * RechargeSelect1::getInstance()
{
	return g_pRechargeSelect1;
}

Scene* RechargeSelect1::createScene()
{
    auto scene = Scene::create();
    
	auto layer = RechargeSelect1::create();

    scene->addChild(layer);

    return scene;
}

RechargeSelect1::RechargeSelect1()
{
	g_pRechargeSelect1 = this;
}

RechargeSelect1::~RechargeSelect1()
{
}

bool RechargeSelect1::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(RechargeSelect1::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(RechargeSelect1::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(RechargeSelect1::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(RechargeSelect1::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("RechargeSelect.csb");
	addChild(ui);
    
	Button* btBack = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close");
	btBack->addClickEventListener(CC_CALLBACK_1(RechargeSelect1::OnClickBack, this));

	auto Image1 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "zhifubao");
	Image1->addClickEventListener(CC_CALLBACK_1(RechargeSelect1::OnClickRecharge1, this));
	
	auto Image2 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "weixin");
	Image2->addClickEventListener(CC_CALLBACK_1(RechargeSelect1::OnClickRecharge2, this));

	auto Image3 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "Image_3");
	Image3->addClickEventListener(CC_CALLBACK_1(RechargeSelect1::OnClickRecharge3, this));

    return true;
}

bool RechargeSelect1::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void RechargeSelect1::onTouchMoved(Touch *touch, Event *event)
{
}

void RechargeSelect1::onTouchEnded(Touch *touch, Event *event)
{
}

void RechargeSelect1::onTouchCancelled(Touch *touch, Event *event)
{
}

void RechargeSelect1::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
}

void RechargeSelect1::onExit()
{
	Layer::onExit();
}

//关闭窗口
void RechargeSelect1::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

//支付宝
void RechargeSelect1::OnClickRecharge1(Ref* sender)
{
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	std::string strRecharge;
	strRecharge = "{\"act\":300 ,\"propName\":\"宝箱\",\"propInfo\":\"";
	strRecharge += pGlobalUserData->accounts.c_str();
	strRecharge += "\",\"propPice\":\"10.00\"}";
	platformAction(a2u(strRecharge));
}

//微信
void RechargeSelect1::OnClickRecharge2(Ref* sender)
{
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	std::string strRecharge;
	strRecharge = "{\"act\":400 ,\"propName\":\"微信充值\",\"propInfo\":\"";
	strRecharge += StringUtils::format("%ld", pGlobalUserData->user_id);
	strRecharge += "\",\"propPice\":\"1000\"}";
	platformAction(a2u(strRecharge));
}

//内购
void RechargeSelect1::OnClickRecharge3(Ref* sender)
{
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	std::string strRecharge;
	strRecharge = "{\"act\":100 ,\"propName\":\"微信充值\",\"propInfo\":\"";
	strRecharge += StringUtils::format("%ld", pGlobalUserData->user_id);
	strRecharge += "\",\"propPice\":\"0\"}";
	platformAction(a2u(strRecharge));
}