
#include "Exchange_Scene.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "dialog_msg_box.h"
#include "lobby_scene.h"
#include "Exchange_Box.h"

static ExchangeScene * g_pExchangeScene = nullptr;

ExchangeScene * ExchangeScene::getInstance()
{
	return g_pExchangeScene;
}

Scene* ExchangeScene::createScene()
{
    auto scene = Scene::create();
    
	auto layer = ExchangeScene::create();

    scene->addChild(layer);

    return scene;
}

ExchangeScene::ExchangeScene()
{
	g_pExchangeScene = this;

	m_dwCurPage = 1;
	m_dwTotalPage = 1;

	memset(&m_ExchangeInfo, 0, sizeof(m_ExchangeInfo));
}

ExchangeScene::~ExchangeScene()
{
}

bool ExchangeScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(ExchangeScene::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(ExchangeScene::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(ExchangeScene::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(ExchangeScene::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("Exchange.csb");
	addChild(ui);
    
	Button* btBack = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE");
	btBack->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickBack, this));
	Button* btBack0 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_0");
	btBack0->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickBack, this));
	Button* btBack1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_1");
	btBack1->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickBack, this));
	Button* btBack2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_2");
	btBack2->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickBack, this));
	Button* btBack3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_3");
	btBack3->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickBack, this));
	
	m_pbtOn = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_ON");
	m_pbtOn->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickOn, this));

	m_pbtNext = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_NEXT");
	m_pbtNext->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickNext, this));

	m_pbtExchange0 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_BUY0");
	m_pbtExchange0->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickExchange0, this));
	m_pbtExchange0->setVisible(false);

	m_pbtExchange1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_BUY1");
	m_pbtExchange1->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickExchange1, this));
	m_pbtExchange1->setVisible(false);

	m_pbtExchange2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_BUY2");
	m_pbtExchange2->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickExchange2, this));
	m_pbtExchange2->setVisible(false);

	m_pbtExchange3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_BUY3");
	m_pbtExchange3->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickExchange3, this));
	m_pbtExchange3->setVisible(false);

	m_pbtExchange4 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_BUY4");
	m_pbtExchange4->addClickEventListener(CC_CALLBACK_1(ExchangeScene::OnClickExchange4, this));
	m_pbtExchange4->setVisible(false);

	m_pTextLottery = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_Lottery");
	m_pTextMedal = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_Medal");
	
	char szTemp[64] = { 0 };
	for (int i = 0; i < 5; i++)
	{
		sprintf(szTemp, "Text_id%d", i);
		m_pTextId[i] = (Text *)Helper::seekWidgetByName((Widget*)ui, szTemp);
		sprintf(szTemp, "Text_Price%d", i);
		m_pTextPrice[i] = (Text *)Helper::seekWidgetByName((Widget*)ui, szTemp);
	}

	m_pTextPage = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_page");

    return true;
}

bool ExchangeScene::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void ExchangeScene::onTouchMoved(Touch *touch, Event *event)
{
}

void ExchangeScene::onTouchEnded(Touch *touch, Event *event)
{
}

void ExchangeScene::onTouchCancelled(Touch *touch, Event *event)
{
}

void ExchangeScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	GetExchangeInfo();

	updateControl();
}

void ExchangeScene::onExit()
{
	Layer::onExit();
}

bool ExchangeScene::OnSocketExchange(uint16 sub_cmd_id, void * pData, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GP_EXCHANGE_INFO:		// 兑换信息
	{
		//参数校验
		assert(sizeof(CMD_GP_ExchangeInfo) == data_size);
		if (sizeof(CMD_GP_ExchangeInfo) != data_size) return false;

		//提取数据
		CMD_GP_ExchangeInfo * pExchangeInfo = (CMD_GP_ExchangeInfo *)pData;
		memcpy(&m_ExchangeInfo, pExchangeInfo, sizeof(m_ExchangeInfo));

		//设置变量
		for (int i = 0; i < 5; i++)
		{
			if (0 == pExchangeInfo->dwId[i])
			{
				m_pTextId[i]->setString("");
				m_pTextPrice[i]->setString("");

				switch (i)
				{
				case 0:
					m_pbtExchange0->setVisible(false);
					break;
				case 1:
					m_pbtExchange1->setVisible(false);
					break;
				case 2:
					m_pbtExchange2->setVisible(false);
					break;
				case 3:
					m_pbtExchange3->setVisible(false);
					break;
				case 4:
					m_pbtExchange4->setVisible(false);
					break;
				}

				continue;
			}

			std::string strName;
			strName.resize(33 * 2 + 2, '\0');
			memcpy(&strName[0], pExchangeInfo->szName[i], 33 * 2);
			strName = l2u(strName);

			m_pTextId[i]->setString(strName);
			m_pTextPrice[i]->setString(a2u(StringUtils::format("%ld+%ld", pExchangeInfo->dwPrice[i] / 2, pExchangeInfo->dwPrice[i] / 2)));
			switch (i)
			{
			case 0:
				m_pbtExchange0->setVisible(true);
				break;
			case 1:
				m_pbtExchange1->setVisible(true);
				break;
			case 2:
				m_pbtExchange2->setVisible(true);
				break;
			case 3:
				m_pbtExchange3->setVisible(true);
				break;
			case 4:
				m_pbtExchange4->setVisible(true);
				break;
			}
		}

		m_dwTotalPage = pExchangeInfo->dwPage;
		std::string strPage1 = StringUtils::format("%ld", m_dwTotalPage);
		std::string strPage2 = StringUtils::format("%ld", m_dwCurPage);
		std::string strPage = strPage2 + "/" + strPage1;
		m_pTextPage->setString(strPage);

		if (m_dwCurPage <= 1)
		{
			m_dwCurPage = 1;
		}
		else if (m_dwCurPage >= m_dwTotalPage)
		{
			m_dwCurPage = m_dwTotalPage;
		}

		updateControl();

		return true;
	}
	}

	return false;
}

//关闭窗口
void ExchangeScene::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

//上一页
void ExchangeScene::OnClickOn(Ref* sender)
{
	if (m_dwCurPage <= 1)
	{
		//m_pbtOn->setEnabled(false);

		return;
	}

	m_dwCurPage -= 1;
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();

	PACKET_HELPER_SIZE(128);
	
	//发送数据
	packet.write4Byte(m_dwCurPage);
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_GET_EXCHANGE_INFO, packet.getBuffer(), packet.getPosition());

	updateControl();
}

//下一页
void ExchangeScene::OnClickNext(Ref* sender)
{
	if (m_dwCurPage >= m_dwTotalPage)
	{
		return;
	}

	m_dwCurPage += 1;
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();

	PACKET_HELPER_SIZE(128);
	
	//发送数据
	packet.write4Byte(m_dwCurPage);
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_GET_EXCHANGE_INFO, packet.getBuffer(), packet.getPosition());

	updateControl();
}

//兑换
void ExchangeScene::OnClickExchange0(Ref* sender)
{
	auto scene = ExchangeBox::createScene();
	ExchangeBox::getInstance()->SetExchangeInfo(m_ExchangeInfo.dwId[0], m_ExchangeInfo.dwPrice[0]);
	Director::getInstance()->getRunningScene()->addChild(scene);
	
}

//兑换
void ExchangeScene::OnClickExchange1(Ref* sender)
{
	auto scene = ExchangeBox::createScene();
	ExchangeBox::getInstance()->SetExchangeInfo(m_ExchangeInfo.dwId[1], m_ExchangeInfo.dwPrice[1]);
	Director::getInstance()->getRunningScene()->addChild(scene);
}

//兑换
void ExchangeScene::OnClickExchange2(Ref* sender)
{
	auto scene = ExchangeBox::createScene();
	ExchangeBox::getInstance()->SetExchangeInfo(m_ExchangeInfo.dwId[2], m_ExchangeInfo.dwPrice[2]);
	Director::getInstance()->getRunningScene()->addChild(scene);
}

//兑换
void ExchangeScene::OnClickExchange3(Ref* sender)
{
	auto scene = ExchangeBox::createScene();
	ExchangeBox::getInstance()->SetExchangeInfo(m_ExchangeInfo.dwId[3], m_ExchangeInfo.dwPrice[3]);
	Director::getInstance()->getRunningScene()->addChild(scene);
}

//兑换
void ExchangeScene::OnClickExchange4(Ref* sender)
{
	auto scene = ExchangeBox::createScene();
	ExchangeBox::getInstance()->SetExchangeInfo(m_ExchangeInfo.dwId[4], m_ExchangeInfo.dwPrice[4]);
	Director::getInstance()->getRunningScene()->addChild(scene);
}

//更新控件
void ExchangeScene::updateControl()
{
	//获取对象
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	m_pTextMedal->setString(StringUtils::format("%ld", pGlobalUserData->user_Medal));
	m_pTextLottery->setString(StringUtils::format("%ld", pGlobalUserData->user_Lottery));
}

//获取拍卖信息
void ExchangeScene::GetExchangeInfo()
{
	PACKET_HELPER_SIZE(128);
	packet.write4Byte(m_dwCurPage);

	//发送数据
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_GET_EXCHANGE_INFO, packet.getBuffer(), packet.getPosition());
}
