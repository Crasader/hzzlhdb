
#include "RechargeSelect2.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "../platform/coplatform.h"

static RechargeSelect2 * g_pRechargeSelect2 = nullptr;

RechargeSelect2 * RechargeSelect2::getInstance()
{
	return g_pRechargeSelect2;
}

Scene* RechargeSelect2::createScene()
{
    auto scene = Scene::create();
    
	auto layer = RechargeSelect2::create();

    scene->addChild(layer);

    return scene;
}

RechargeSelect2::RechargeSelect2()
{
	g_pRechargeSelect2 = this;
}

RechargeSelect2::~RechargeSelect2()
{
}

bool RechargeSelect2::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(RechargeSelect2::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(RechargeSelect2::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(RechargeSelect2::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(RechargeSelect2::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("RechargeSelect.csb");
	addChild(ui);
    
	Button* btBack = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close");
	btBack->addClickEventListener(CC_CALLBACK_1(RechargeSelect2::OnClickBack, this));

	auto Image1 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "zhifubao");
	Image1->addClickEventListener(CC_CALLBACK_1(RechargeSelect2::OnClickRecharge1, this));
	
	auto Image2 = (ImageView*)HelperEx::seekNodeByName((Widget*)ui, "weixin");
	Image2->addClickEventListener(CC_CALLBACK_1(RechargeSelect2::OnClickRecharge2, this));

    return true;
}

bool RechargeSelect2::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void RechargeSelect2::onTouchMoved(Touch *touch, Event *event)
{
}

void RechargeSelect2::onTouchEnded(Touch *touch, Event *event)
{
}

void RechargeSelect2::onTouchCancelled(Touch *touch, Event *event)
{
}

void RechargeSelect2::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
}

void RechargeSelect2::onExit()
{
	Layer::onExit();
}

//关闭窗口
void RechargeSelect2::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

//支付宝
void RechargeSelect2::OnClickRecharge1(Ref* sender)
{
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	std::string strRecharge;
	strRecharge = "{\"act\":300 ,\"propName\":\"宝箱\",\"propInfo\":\"";
	strRecharge += pGlobalUserData->accounts.c_str();
	strRecharge += "\",\"propPice\":\"30.00\"}";
	platformAction(a2u(strRecharge));
}

//微信
void RechargeSelect2::OnClickRecharge2(Ref* sender)
{
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	std::string strRecharge;
	strRecharge = "{\"act\":400 ,\"propName\":\"微信充值\",\"propInfo\":\"";
	strRecharge += StringUtils::format("%ld", pGlobalUserData->user_id);
	strRecharge += "\",\"propPice\":\"3000\"}";
	platformAction(a2u(strRecharge));
}