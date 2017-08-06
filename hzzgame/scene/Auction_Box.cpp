
#include "Auction_Box.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "dialog_msg_box.h"

static AuctionBox * g_pAuctionBox = nullptr;

AuctionBox * AuctionBox::getInstance()
{
	return g_pAuctionBox;
}

Scene* AuctionBox::createScene()
{
    auto scene = Scene::create();
    
	auto layer = AuctionBox::create();

    scene->addChild(layer);

    return scene;
}

AuctionBox::AuctionBox()
{
	g_pAuctionBox = this;
}

AuctionBox::~AuctionBox()
{
}

bool AuctionBox::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(AuctionBox::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(AuctionBox::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(AuctionBox::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(AuctionBox::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("auctionBox.csb");
	addChild(ui);

	Button* btBack = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_close");
	btBack->addClickEventListener(CC_CALLBACK_1(AuctionBox::OnClickBack, this));



	Button* btConfirm = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_confirm");
	btConfirm->addClickEventListener(CC_CALLBACK_1(AuctionBox::OnClickConfirm, this));

	m_pTextLottery = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_Lottery");
	m_pTextMedal = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_Medal");

	m_pCbMedal = (CheckBox *)Helper::seekWidgetByName((Widget*)ui, "Cb_Medal");
	m_pCbMedal->addEventListener(CC_CALLBACK_2(AuctionBox::selectedMedalEvent, this));
	m_pCbLottery = (CheckBox *)Helper::seekWidgetByName((Widget*)ui, "Cb_Lottery");
	m_pCbLottery->addEventListener(CC_CALLBACK_2(AuctionBox::selectedLotteryEvent, this));

	m_pEbNum = EditBox::create(Size(240, 36), "common/dialog_input.png");
	m_pEbNum->setPosition(Vec2(719, 333));
	//m_pEbNum->setFontSize(20);
	m_pEbNum->setFontColor(Color3B::BLACK);
	m_pEbNum->setInputMode(EditBox::InputMode::NUMERIC);
	m_pEbNum->setMaxLength(4);
	addChild(m_pEbNum);

	m_pEbPrice = EditBox::create(Size(240, 36), "common/dialog_input.png");
	m_pEbPrice->setPosition(Vec2(719, 290));
	//m_pEbPrice->setFontSize(20);
	m_pEbPrice->setFontColor(Color3B::BLACK);
	m_pEbPrice->setInputMode(EditBox::InputMode::NUMERIC);
	m_pEbPrice->setMaxLength(9);
	addChild(m_pEbPrice);
	
    return true;
}

bool AuctionBox::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void AuctionBox::onTouchMoved(Touch *touch, Event *event)
{
}

void AuctionBox::onTouchEnded(Touch *touch, Event *event)
{
}

void AuctionBox::onTouchCancelled(Touch *touch, Event *event)
{
}

void AuctionBox::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	//获取对象
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	m_pTextMedal->setString(a2u(StringUtils::format("%ld", pGlobalUserData->user_Medal)));
	m_pTextLottery->setString(a2u(StringUtils::format("%ld", pGlobalUserData->user_Lottery)));
}

void AuctionBox::onExit()
{
	Layer::onExit();
}

//关闭窗口
void AuctionBox::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

//提交
void AuctionBox::OnClickConfirm(Ref* sender)
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
		if (pGlobalUserData->user_Medal < dwNum)
		{
			MsgBox(a2u("龙角数量不对！"));
			return;
		}

		dwType = 1;
	}
	else
	{
		if (pGlobalUserData->user_Lottery < dwNum)
		{
			MsgBox(a2u("鹅毛数量不对！"));
			return;
		}

		dwType = 2;
	}

	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
	packet.write4Byte(dwType);
	packet.write4Byte(dwNum);
	packet.write4Byte(atol(m_pEbPrice->getText()));

	//发送数据
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_SELL, packet.getBuffer(), packet.getPosition());

	removeFromParentAndCleanup(true);
}

//选择龙角
void AuctionBox::selectedMedalEvent(Ref* ref, CheckBox::EventType event)
{
	m_pCbMedal->setSelected(true);
	m_pCbLottery->setSelected(false);
}

//选择鹅毛
void AuctionBox::selectedLotteryEvent(Ref* ref, CheckBox::EventType event)
{
	m_pCbLottery->setSelected(true);
	m_pCbMedal->setSelected(false);
}