
#include "Auction_Scene.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "dialog_msg_box.h"
#include "lobby_scene.h"

static AuctionScene * g_pAuctionScene = nullptr;

AuctionScene * AuctionScene::getInstance()
{
	return g_pAuctionScene;
}

Scene* AuctionScene::createScene()
{
    auto scene = Scene::create();
    
	auto layer = AuctionScene::create();

    scene->addChild(layer);

    return scene;
}

AuctionScene::AuctionScene()
{
	g_pAuctionScene = this;

	m_dwCurPage = 1;
	m_dwTotalPage = 1;
	m_cbType = 1;

	memset(m_dwState, 0, sizeof(m_dwState));
}

AuctionScene::~AuctionScene()
{
}

bool AuctionScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(AuctionScene::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(AuctionScene::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(AuctionScene::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(AuctionScene::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("Auction.csb");
	addChild(ui);
    
	Button* btBack = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE");
	btBack->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickBack, this));

	Button* btBack0 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_0");
	btBack0->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickBack, this));
	Button* btBack1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_1");
	btBack1->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickBack, this));
	Button* btBack2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_2");
	btBack2->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickBack, this));
	Button* btBack3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_3");
	btBack3->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickBack, this));

	Button* btRefresh = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_Refresh");
	btRefresh->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickRefresh, this));
	
	Button * btSearch = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_search");
	btSearch->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickSearch, this));

	Button * btAuction = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_AUCTION");
	btAuction->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickAuction, this));

	Button * btMyAuction = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_myAUCTION");
	btMyAuction->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickMyAuction, this));
	
	m_pbtOn = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_ON");
	m_pbtOn->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickOn, this));

	m_pbtNext = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_NEXT");
	m_pbtNext->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickNext, this));

	m_pbtBuy0 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_BUY0");
	m_pbtBuy0->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickBuy0, this));
	m_pbtBuy0->setVisible(false);
	m_pbtBuy0->setTitleFontSize(24);
	m_pbtBuy0->setTitleColor(Color3B::WHITE);

	m_pbtBuy1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_BUY1");
	m_pbtBuy1->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickBuy1, this));
	m_pbtBuy1->setVisible(false);
	m_pbtBuy1->setTitleFontSize(24);
	m_pbtBuy1->setTitleColor(Color3B::WHITE);

	m_pbtBuy2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_BUY2");
	m_pbtBuy2->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickBuy2, this));
	m_pbtBuy2->setVisible(false);
	m_pbtBuy2->setTitleFontSize(24);
	m_pbtBuy2->setTitleColor(Color3B::WHITE);

	m_pbtBuy3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_BUY3");
	m_pbtBuy3->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickBuy3, this));
	m_pbtBuy3->setVisible(false);
	m_pbtBuy3->setTitleFontSize(24);
	m_pbtBuy3->setTitleColor(Color3B::WHITE);

	m_pbtBuy4 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_BUY4");
	m_pbtBuy4->addClickEventListener(CC_CALLBACK_1(AuctionScene::OnClickBuy4, this));
	m_pbtBuy4->setVisible(false);
	m_pbtBuy4->setTitleFontSize(24);
	m_pbtBuy4->setTitleColor(Color3B::WHITE);

	m_pTextLottery = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_Lottery");
	m_pTextMedal = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_Medal");
	
	char szTemp[64] = { 0 };
	for (int i = 0; i < 5; i++)
	{
		sprintf(szTemp, "Text_id%d", i);
		m_pTextId[i] = (Text *)Helper::seekWidgetByName((Widget*)ui, szTemp);
		sprintf(szTemp, "Text_type%d", i);
		m_pTextType[i] = (Text *)Helper::seekWidgetByName((Widget*)ui, szTemp);
		sprintf(szTemp, "Text_total_num%d", i);
		m_pTextTotalNum[i] = (Text *)Helper::seekWidgetByName((Widget*)ui, szTemp);
		sprintf(szTemp, "Text_Price%d", i);
		m_pTextPrice[i] = (Text *)Helper::seekWidgetByName((Widget*)ui, szTemp);
		sprintf(szTemp, "Text_total_price%d", i);
		m_pTextTotalPrice[i] = (Text *)Helper::seekWidgetByName((Widget*)ui, szTemp);
	}

	m_pTextPage = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_page");

	m_pEbSearch = EditBox::create(Size(140, 36), "ED_SEARCH_2");
	m_pEbSearch->setPosition(Vec2(980, 512));
	m_pEbSearch->setFontColor(Color3B::BLACK);
	m_pEbSearch->setInputMode(EditBox::InputMode::NUMERIC);
	addChild(m_pEbSearch);

    return true;
}

bool AuctionScene::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void AuctionScene::onTouchMoved(Touch *touch, Event *event)
{
}

void AuctionScene::onTouchEnded(Touch *touch, Event *event)
{
}

void AuctionScene::onTouchCancelled(Touch *touch, Event *event)
{
}

void AuctionScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	GetAuctionInfo();

	updateControl();
}

void AuctionScene::onExit()
{
	Layer::onExit();
}

bool AuctionScene::OnSocketAuction(uint16 sub_cmd_id, void * pData, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GP_AUCTION_INFO:		// ������Ϣ
	{
		//����У��
		assert(sizeof(CMD_GP_AuctionInfo) == data_size);
		if (sizeof(CMD_GP_AuctionInfo) != data_size) return false;

		//��ȡ����
		CMD_GP_AuctionInfo * pAuctionInfo = (CMD_GP_AuctionInfo *)pData;

		//���ñ���
		for (int i = 0; i < 5; i++)
		{
			m_dwState[i] = 0;
			if (0 == pAuctionInfo->dwId[i])
			{
				m_pTextId[i]->setString("");
				m_pTextType[i]->setString("");
				m_pTextTotalNum[i]->setString("");
				m_pTextPrice[i]->setString("");
				m_pTextTotalPrice[i]->setString("");

				switch (i)
				{
				case 0:
					m_pbtBuy0->setVisible(false);
					break;
				case 1:
					m_pbtBuy1->setVisible(false);
					break;
				case 2:
					m_pbtBuy2->setVisible(false);
					break;
				case 3:
					m_pbtBuy3->setVisible(false);
					break;
				case 4:
					m_pbtBuy4->setVisible(false);
					break;
				}

				continue;
			}

			m_pTextId[i]->setString(StringUtils::format("%ld", pAuctionInfo->dwId[i]));
			switch (pAuctionInfo->dwType[i])
			{
			case 1:
				m_pTextType[i]->setString(a2u("����"));
				break;
			case 2:
				m_pTextType[i]->setString(a2u("��ë"));
				break;
			}
			//m_pTextType[i]->setString(StringUtils::format("%ld", pAuctionInfo->dwType[i]));
			m_pTextTotalNum[i]->setString(StringUtils::format("%ld", pAuctionInfo->dwTotalNum[i]));
			if (pAuctionInfo->dwPrice[i] > 10000)
			{
				m_pTextPrice[i]->setString(a2u(StringUtils::format("%.1fW", pAuctionInfo->dwPrice[i] / 10000.0)));
			}
			else
			{
				m_pTextPrice[i]->setString(StringUtils::format("%ld", pAuctionInfo->dwPrice[i]));
			}

			if (pAuctionInfo->dwTotalPrice[i] > 10000)
			{
				m_pTextTotalPrice[i]->setString(a2u(StringUtils::format("%.1fW", pAuctionInfo->dwTotalPrice[i] / 10000.0)));
			}
			else
			{
				m_pTextTotalPrice[i]->setString(StringUtils::format("%ld", pAuctionInfo->dwTotalPrice[i]));
			}

			//m_pTextPrice[i]->setString(StringUtils::format("%ld", pAuctionInfo->dwPrice[i]));
			//m_pTextTotalPrice[i]->setString(StringUtils::format("%ld", pAuctionInfo->dwTotalPrice[i]));

			switch (i)
			{
			case 0:
				m_pbtBuy0->setTitleText(a2u("ȷ�Ϲ���"));
				m_pbtBuy0->setVisible(true);
				break;
			case 1:
				m_pbtBuy1->setTitleText(a2u("ȷ�Ϲ���"));
				m_pbtBuy1->setVisible(true);
				break;
			case 2:
				m_pbtBuy2->setTitleText(a2u("ȷ�Ϲ���"));
				m_pbtBuy2->setVisible(true);
				break;
			case 3:
				m_pbtBuy3->setTitleText(a2u("ȷ�Ϲ���"));
				m_pbtBuy3->setVisible(true);
				break;
			case 4:
				m_pbtBuy4->setTitleText(a2u("ȷ�Ϲ���"));
				m_pbtBuy4->setVisible(true);
				break;
			}
		}

		m_dwTotalPage = pAuctionInfo->dwPage;
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
	case SUB_GP_SEARCH_RESULT:		//���ҽ��
	{
		//����У��
		assert(sizeof(CMD_GP_SearchResult) == data_size);
		if (sizeof(CMD_GP_SearchResult) != data_size) return false;

		//��ȡ����
		CMD_GP_SearchResult * pSearchResult = (CMD_GP_SearchResult *)pData;

		//���ñ���
		for (int i = 0; i < 5; i++)
		{
			m_dwState[i] = 0;
			m_pTextId[i]->setString("");
			m_pTextType[i]->setString("");
			m_pTextTotalNum[i]->setString("");
			m_pTextPrice[i]->setString("");
			m_pTextTotalPrice[i]->setString("");
		}

		m_pbtBuy0->setVisible(false);
		m_pbtBuy1->setVisible(false);
		m_pbtBuy2->setVisible(false);
		m_pbtBuy3->setVisible(false);
		m_pbtBuy4->setVisible(false);

		m_pTextId[0]->setString(StringUtils::format("%ld", pSearchResult->dwId));
		switch (pSearchResult->dwType)
		{
		case 1:
			m_pTextType[0]->setString(a2u("����"));
			break;
		case 2:
			m_pTextType[0]->setString(a2u("��ë"));
			break;
		}

		m_pTextTotalNum[0]->setString(StringUtils::format("%ld", pSearchResult->dwNum));
		m_pTextPrice[0]->setString(StringUtils::format("%ld", pSearchResult->dwPrice / pSearchResult->dwNum));
		m_pTextTotalPrice[0]->setString(StringUtils::format("%ld", pSearchResult->dwPrice));


		if (pSearchResult->dwPrice / pSearchResult->dwNum> 10000)
		{
			m_pTextPrice[0]->setString(a2u(StringUtils::format("%.1fW", pSearchResult->dwPrice / pSearchResult->dwNum / 10000.0)));
		}
		else
		{
			m_pTextPrice[0]->setString(StringUtils::format("%ld", pSearchResult->dwPrice / pSearchResult->dwNum));
		}

		if (pSearchResult->dwPrice > 10000)
		{
			m_pTextTotalPrice[0]->setString(a2u(StringUtils::format("%.1fW", pSearchResult->dwPrice / 10000.0)));
		}
		else
		{
			m_pTextTotalPrice[0]->setString(StringUtils::format("%ld", pSearchResult->dwPrice));
		}


		m_pbtBuy0->setTitleText(a2u("ȷ�Ϲ���"));
		m_pbtBuy0->setVisible(true);

		updateControl();

		return true;
	}
	case SUB_GP_MY_AUCTION_INFO:
	{
		//����У��
		assert(sizeof(CMD_GP_MyAuctionInfo) == data_size);
		if (sizeof(CMD_GP_MyAuctionInfo) != data_size) return false;

		//��ȡ����
		CMD_GP_MyAuctionInfo * pMyAuctionInfo = (CMD_GP_MyAuctionInfo *)pData;

		//���ñ���
		for (int i = 0; i < 5; i++)
		{
			m_dwState[i] = pMyAuctionInfo->dwState[i];

			//CHAR buf[1024];
			//sprintf(buf, "pMyAuctionInfo=%d", pMyAuctionInfo->dwState[i]);
			//OutputDebugStringA(buf);

			if (0 == pMyAuctionInfo->dwId[i])
			{
				m_pTextId[i]->setString("");
				m_pTextType[i]->setString("");
				m_pTextTotalNum[i]->setString("");
				m_pTextPrice[i]->setString("");
				m_pTextTotalPrice[i]->setString("");

				switch (i)
				{
				case 0:
					m_pbtBuy0->setVisible(false);
					break;
				case 1:
					m_pbtBuy1->setVisible(false);
					break;
				case 2:
					m_pbtBuy2->setVisible(false);
					break;
				case 3:
					m_pbtBuy3->setVisible(false);
					break;
				case 4:
					m_pbtBuy4->setVisible(false);
					break;
				}

				continue;
			}

			m_pTextId[i]->setString(StringUtils::format("%ld", pMyAuctionInfo->dwId[i]));
			switch (pMyAuctionInfo->dwType[i])
			{
			case 1:
				m_pTextType[i]->setString(a2u("����"));
				break;
			case 2:
				m_pTextType[i]->setString(a2u("��ë"));
				break;
			}

			m_pTextTotalNum[i]->setString(StringUtils::format("%ld", pMyAuctionInfo->dwTotalNum[i]));
			//m_pTextPrice[i]->setString(StringUtils::format("%ld", pMyAuctionInfo->dwPrice[i]));
			//m_pTextTotalPrice[i]->setString(StringUtils::format("%ld", pMyAuctionInfo->dwTotalPrice[i]));

			if (pMyAuctionInfo->dwPrice[i] > 10000)
			{
				m_pTextPrice[i]->setString(a2u(StringUtils::format("%.1fW", pMyAuctionInfo->dwPrice[i] / 10000.0)));
			}
			else
			{
				m_pTextPrice[i]->setString(StringUtils::format("%ld", pMyAuctionInfo->dwPrice[i]));
			}

			if (pMyAuctionInfo->dwTotalPrice[i] > 10000)
			{
				m_pTextTotalPrice[i]->setString(a2u(StringUtils::format("%.1fW", pMyAuctionInfo->dwTotalPrice[i] / 10000.0)));
			}
			else
			{
				m_pTextTotalPrice[i]->setString(StringUtils::format("%ld", pMyAuctionInfo->dwTotalPrice[i]));
			}

			switch (i)
			{
			case 0:
				switch (m_dwState[i])
				{
				case 0:
					m_pbtBuy0->setTitleText(a2u("ȷ�Ϲ���"));
					break;
				case 1:
					m_pbtBuy0->setTitleText(a2u("���׳ɹ�"));
					break;
				case 2:
					m_pbtBuy0->setTitleText(a2u("��ʱ����"));
					break;
				}
				m_pbtBuy0->setVisible(true);
				break;
			case 1:
				switch (m_dwState[i])
				{
				case 0:
					m_pbtBuy1->setTitleText(a2u("ȷ�Ϲ���"));
					break;
				case 1:
					m_pbtBuy1->setTitleText(a2u("���׳ɹ�"));
					break;
				case 2:
					m_pbtBuy1->setTitleText(a2u("��ʱ����"));
					break;
				}
				m_pbtBuy1->setVisible(true);
				break;
			case 2:
				switch (m_dwState[i])
				{
				case 0:
					m_pbtBuy2->setTitleText(a2u("ȷ�Ϲ���"));
					break;
				case 1:
					m_pbtBuy2->setTitleText(a2u("���׳ɹ�"));
					break;
				case 2:
					m_pbtBuy2->setTitleText(a2u("��ʱ����"));
					break;
				}
				m_pbtBuy2->setVisible(true);
				break;
			case 3:
				switch (m_dwState[i])
				{
				case 0:
					m_pbtBuy3->setTitleText(a2u("ȷ�Ϲ���"));
					break;
				case 1:
					m_pbtBuy3->setTitleText(a2u("���׳ɹ�"));
					break;
				case 2:
					m_pbtBuy3->setTitleText(a2u("��ʱ����"));
					break;
				}
				m_pbtBuy3->setVisible(true);
				break;
			case 4:
				switch (m_dwState[i])
				{
				case 0:
					m_pbtBuy4->setTitleText(a2u("ȷ�Ϲ���"));
					break;
				case 1:
					m_pbtBuy4->setTitleText(a2u("���׳ɹ�"));
					break;
				case 2:
					m_pbtBuy4->setTitleText(a2u("��ʱ����"));
					break;
				}
				m_pbtBuy4->setVisible(true);
				break;
			}
		}

		m_dwTotalPage = pMyAuctionInfo->dwPage;
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

//�رմ���
void AuctionScene::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

//ˢ��
void AuctionScene::OnClickRefresh(Ref* sender)
{
	GetAuctionInfo();
}

//����
void AuctionScene::OnClickSearch(Ref* sender)
{
	//��ȡ����
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();

	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
	packet.write4Byte(atol(m_pEbSearch->getText()));

	//��������
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_SEARCH, packet.getBuffer(), packet.getPosition());
}

//����
void AuctionScene::OnClickAuction(Ref* sender)
{
	auto scene = AuctionBox::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

//�ҵ�����
void AuctionScene::OnClickMyAuction(Ref* sender)
{
	//��ȡ����
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	m_dwCurPage = 1;
	m_dwTotalPage = 1;
	m_cbType = 2;

	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
	packet.write4Byte(m_dwCurPage);

	//��������
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_GET_MY_AUCTION_INFO, packet.getBuffer(), packet.getPosition());
}

//��һҳ
void AuctionScene::OnClickOn(Ref* sender)
{
	if (m_dwCurPage <= 1)
	{
		//m_pbtOn->setEnabled(false);

		return;
	}

	m_dwCurPage -= 1;
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();

	PACKET_HELPER_SIZE(128);
	
	switch (m_cbType)
	{
	case 1:
		//��������
		packet.write4Byte(m_dwCurPage);
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_GET_AUCTION_INFO, packet.getBuffer(), packet.getPosition());
		break;
	case 2:
		//��������
		packet.write4Byte(pGlobalUserData->user_id);
		packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
		packet.write4Byte(m_dwCurPage);
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_GET_MY_AUCTION_INFO, packet.getBuffer(), packet.getPosition());
		break;
	}

	updateControl();
}

//��һҳ
void AuctionScene::OnClickNext(Ref* sender)
{
	if (m_dwCurPage >= m_dwTotalPage)
	{
		//m_pbtNext->setEnabled(false);

		return;
	}

	m_dwCurPage += 1;
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();

	PACKET_HELPER_SIZE(128);
	
	switch (m_cbType)
	{
	case 1:
		//��������
		packet.write4Byte(m_dwCurPage);
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_GET_AUCTION_INFO, packet.getBuffer(), packet.getPosition());
		break;
	case 2:
		//��������
		packet.write4Byte(pGlobalUserData->user_id);
		packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
		packet.write4Byte(m_dwCurPage);
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_GET_MY_AUCTION_INFO, packet.getBuffer(), packet.getPosition());
		break;
	}

	updateControl();
}

//����
void AuctionScene::OnClickBuy0(Ref* sender)
{
	//��ȡ����
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	DWORD dwPrice = atol(m_pTextTotalPrice[0]->getString().c_str());


	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
	packet.write4Byte(atol(m_pTextId[0]->getString().c_str()));

	//��������
	switch (m_dwState[0])
	{
	case 0:
		if (dwPrice > pGlobalUserData->user_score)
		{
			MsgBox(a2u("��Ľ�Ҳ�����"));
			return;
		}
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_BUY, packet.getBuffer(), packet.getPosition());
		break;
	case 1:
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_CONFIRM, packet.getBuffer(), packet.getPosition());
		break;
	case 2:
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_RETURN, packet.getBuffer(), packet.getPosition());
		break;
	default:
		break;
	}
}

//����
void AuctionScene::OnClickBuy1(Ref* sender)
{
	//��ȡ����
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	DWORD dwPrice = atol(m_pTextTotalPrice[1]->getString().c_str());


	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
	packet.write4Byte(atol(m_pTextId[1]->getString().c_str()));

	//��������
	switch (m_dwState[1])
	{
	case 0:
		if (dwPrice > pGlobalUserData->user_score)
		{
			MsgBox(a2u("��Ľ�Ҳ�����"));
			return;
		}

		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_BUY, packet.getBuffer(), packet.getPosition());
		break;
	case 1:
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_CONFIRM, packet.getBuffer(), packet.getPosition());
		break;
	case 2:
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_RETURN, packet.getBuffer(), packet.getPosition());
		break;
	default:
		break;
	}
}

//����
void AuctionScene::OnClickBuy2(Ref* sender)
{
	//��ȡ����
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	DWORD dwPrice = atol(m_pTextTotalPrice[2]->getString().c_str());



	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
	packet.write4Byte(atol(m_pTextId[2]->getString().c_str()));

	//��������
	switch (m_dwState[2])
	{
	case 0:
		if (dwPrice > pGlobalUserData->user_score)
		{
			MsgBox(a2u("��Ľ�Ҳ�����"));
			return;
		}
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_BUY, packet.getBuffer(), packet.getPosition());
		break;
	case 1:
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_CONFIRM, packet.getBuffer(), packet.getPosition());
		break;
	case 2:
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_RETURN, packet.getBuffer(), packet.getPosition());
		break;
	default:
		break;
	}
}

//����
void AuctionScene::OnClickBuy3(Ref* sender)
{
	//��ȡ����
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	DWORD dwPrice = atol(m_pTextTotalPrice[3]->getString().c_str());



	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
	packet.write4Byte(atol(m_pTextId[3]->getString().c_str()));

	//��������
	switch (m_dwState[3])
	{
	case 0:
		if (dwPrice > pGlobalUserData->user_score)
		{
			MsgBox(a2u("��Ľ�Ҳ�����"));
			return;
		}
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_BUY, packet.getBuffer(), packet.getPosition());
		break;
	case 1:
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_CONFIRM, packet.getBuffer(), packet.getPosition());
		break;
	case 2:
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_RETURN, packet.getBuffer(), packet.getPosition());
		break;
	default:
		break;
	}
}

//����
void AuctionScene::OnClickBuy4(Ref* sender)
{
	//��ȡ����
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	DWORD dwPrice = atol(m_pTextTotalPrice[4]->getString().c_str());


	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
	packet.write4Byte(atol(m_pTextId[4]->getString().c_str()));

	//��������
	switch (m_dwState[4])
	{
	case 0:
		if (dwPrice > pGlobalUserData->user_score)
		{
			MsgBox(a2u("��Ľ�Ҳ�����"));
			return;
		}

		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_BUY, packet.getBuffer(), packet.getPosition());
		break;
	case 1:
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_CONFIRM, packet.getBuffer(), packet.getPosition());
		break;
	case 2:
		PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_RETURN, packet.getBuffer(), packet.getPosition());
		break;
	default:
		break;
	}
}

//���¿ؼ�
void AuctionScene::updateControl()
{
	//��ȡ����
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	m_pTextMedal->setString(StringUtils::format("%ld", pGlobalUserData->user_Medal));
	m_pTextLottery->setString(StringUtils::format("%ld", pGlobalUserData->user_Lottery));
}

//��ȡ������Ϣ
void AuctionScene::GetAuctionInfo()
{
	m_cbType = 1;

	PACKET_HELPER_SIZE(128);
	packet.write4Byte(m_dwCurPage);

	//��������
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_GET_AUCTION_INFO, packet.getBuffer(), packet.getPosition());
}

//��ȡ�Լ�������Ϣ
void AuctionScene::GetMyAuctionInfo()
{
	m_cbType = 2;

	//��ȡ����
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();

	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
	packet.write4Byte(m_dwCurPage);

	//��������
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_GET_MY_AUCTION_INFO, packet.getBuffer(), packet.getPosition());
}