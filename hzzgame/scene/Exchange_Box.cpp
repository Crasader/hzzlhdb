
#include "Exchange_Box.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "dialog_msg_box.h"

static ExchangeBox * g_pExchangeBox = nullptr;

ExchangeBox * ExchangeBox::getInstance()
{
	return g_pExchangeBox;
}

Scene* ExchangeBox::createScene()
{
    auto scene = Scene::create();
    
	auto layer = ExchangeBox::create();

    scene->addChild(layer);

    return scene;
}

ExchangeBox::ExchangeBox()
{
	g_pExchangeBox = this;

	m_dwID = 0;
	m_dwPrice = 0;
}

ExchangeBox::~ExchangeBox()
{
}

bool ExchangeBox::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(ExchangeBox::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(ExchangeBox::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(ExchangeBox::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(ExchangeBox::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("ExchangeBox.csb");
	addChild(ui);

	Button* btBack = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close");
	btBack->addClickEventListener(CC_CALLBACK_1(ExchangeBox::OnClickBack, this));

	Button* btConfirm = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_confirm");
	btConfirm->addClickEventListener(CC_CALLBACK_1(ExchangeBox::OnClickConfirm, this));

	m_pTextLottery = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_Lottery");
	m_pTextMedal = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_Medal");
	m_pTextPrice = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_price");

	m_pCbMedal = (CheckBox *)Helper::seekWidgetByName((Widget*)ui, "Cb_Medal");
	m_pCbMedal->addEventListener(CC_CALLBACK_2(ExchangeBox::selectedMedalEvent, this));
	m_pCbLottery = (CheckBox *)Helper::seekWidgetByName((Widget*)ui, "Cb_Lottery");
	m_pCbLottery->addEventListener(CC_CALLBACK_2(ExchangeBox::selectedLotteryEvent, this));

	m_pEbNum = EditBox::create(Size(100, 50), "Exchange/ED_NUM.png");
	m_pEbNum->setPosition(Vec2(655, 291));
	m_pEbNum->setFontColor(Color3B::BLACK);
	m_pEbNum->setInputMode(EditBox::InputMode::NUMERIC);
	m_pEbNum->setMaxLength(2);
	addChild(m_pEbNum);
	
    return true;
}

bool ExchangeBox::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void ExchangeBox::onTouchMoved(Touch *touch, Event *event)
{
}

void ExchangeBox::onTouchEnded(Touch *touch, Event *event)
{
}

void ExchangeBox::onTouchCancelled(Touch *touch, Event *event)
{
}

void ExchangeBox::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	//获取对象
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	m_pTextMedal->setString(a2u(StringUtils::format("%ld", pGlobalUserData->user_Medal)));
	m_pTextLottery->setString(a2u(StringUtils::format("%ld", pGlobalUserData->user_Lottery)));
	m_pTextPrice->setString(a2u(StringUtils::format("%ld", m_dwPrice / 2)));
}

void ExchangeBox::onExit()
{
	Layer::onExit();
}

//关闭窗口
void ExchangeBox::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

//提交
void ExchangeBox::OnClickConfirm(Ref* sender)
{
	//获取对象
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	DWORD dwNum = atol(m_pEbNum->getText());

	if (0 == dwNum)
	{
		MsgBox(a2u("数量不能为0！"));
		return;
	}
	DWORD dwType = 0;
	if (m_pCbMedal->isSelected())
	{
		if (pGlobalUserData->user_Medal < dwNum/2)
		{
			MsgBox(a2u("龙角或鹅毛数量不足！"));
			return;
		}

		dwType = 1;
	}
	else
	{
		if (pGlobalUserData->user_Lottery < dwNum/2)
		{
			MsgBox(a2u("龙角或鹅毛数量不足！"));
			return;
		}

		dwType = 2;
	}

	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
	packet.write4Byte(m_dwID);
	packet.write4Byte(dwType);
	packet.write4Byte(dwNum);

	//发送数据
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_EXCHANGE, packet.getBuffer(), packet.getPosition());

	removeFromParentAndCleanup(true);
}

//选择龙角
void ExchangeBox::selectedMedalEvent(Ref* ref, CheckBox::EventType event)
{
	m_pCbMedal->setSelected(true);
	m_pCbLottery->setSelected(false);
}

//选择鹅毛
void ExchangeBox::selectedLotteryEvent(Ref* ref, CheckBox::EventType event)
{
	m_pCbLottery->setSelected(true);
	m_pCbMedal->setSelected(false);
}

//设置兑换信息
void ExchangeBox::SetExchangeInfo(DWORD dwID, DWORD dwPrice)
{
	m_dwID = dwID;
	m_dwPrice = dwPrice;
}