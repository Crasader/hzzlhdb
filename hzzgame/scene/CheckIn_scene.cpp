
#include "CheckIn_scene.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "dialog_msg_box.h"
#include "lobby_scene.h"

#define MAX_ANIMAL_FRAME			6									//动画帧数

//std::string WideByte2UTF8(const std::wstring& text)
//{
//	int asciisize = ::WideCharToMultiByte(CP_UTF8, 0, text.c_str(), text.size(), NULL, 0, NULL, NULL);
//	if (asciisize == ERROR_NO_UNICODE_TRANSLATION ||
//		asciisize == 0)
//	{
//		return std::string();
//	}
//
//	char* resultstring = new char[asciisize];
//	int convresult = ::WideCharToMultiByte(CP_UTF8, 0, text.c_str(), text.size(), resultstring, asciisize, NULL, NULL);
//
//	if (convresult != asciisize)
//	{
//		return std::string();
//	}
//
//	std::string buffer(resultstring, convresult);
//	delete[] resultstring;
//	return buffer;
//}

static CheckInScene * g_pCheckInScene = nullptr;

CheckInScene * CheckInScene::getInstance()
{
	return g_pCheckInScene;
}

Scene* CheckInScene::createScene()
{
	auto scene = Scene::create();

	auto layer = CheckInScene::create();

	scene->addChild(layer);

	return scene;
}

CheckInScene::CheckInScene()
{
	g_pCheckInScene = this;

	m_bTodayChecked = false;
	m_wDateIndex = 0;
	memset((m_lRewardGold), 0, sizeof(m_lRewardGold));
}

CheckInScene::~CheckInScene()
{
	for (int i = 0; i < LEN_WEEK; i++)
	{
		m_act[i]->release();
	}
}

bool CheckInScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(CheckInScene::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(CheckInScene::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(CheckInScene::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(CheckInScene::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("CheckIn.csb");
	addChild(ui);

	Button* btn_back = (Button*)Helper::seekWidgetByName((Widget*)ui, "close");
	btn_back->addClickEventListener(CC_CALLBACK_1(CheckInScene::OnClickBack, this));
	Button* btn_back0 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_0");
	btn_back0->addClickEventListener(CC_CALLBACK_1(CheckInScene::OnClickBack, this));
	Button* btn_back1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_1");
	btn_back1->addClickEventListener(CC_CALLBACK_1(CheckInScene::OnClickBack, this));
	Button* btn_back2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_2");
	btn_back2->addClickEventListener(CC_CALLBACK_1(CheckInScene::OnClickBack, this));
	Button* btn_back3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_3");
	btn_back3->addClickEventListener(CC_CALLBACK_1(CheckInScene::OnClickBack, this));

	m_btCheckIn = (Button*)Helper::seekWidgetByName((Widget*)ui, "CheckIn");
	m_btCheckIn->addClickEventListener(CC_CALLBACK_1(CheckInScene::OnClickCheckInDone, this));

	m_text[0] = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_1");
	m_text[1] = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_2");
	m_text[2] = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_3");
	m_text[3] = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_4");
	m_text[4] = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_5");
	m_text[5] = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_6");
	m_text[6] = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_7");

	m_textDate = (Text *)Helper::seekWidgetByName((Widget*)ui, "Text_Date");

	char szTemp[64] = { 0 };
	for (int i = 0; i < LEN_WEEK; i++)
	{
		sprintf(szTemp, "CheckIn/IMAGE_CHECKIN_CARD%d.png", i + 1);
		m_sprImageCard[i] = Sprite::create(szTemp);
		auto animation = Animation::create();
		sprintf(szTemp, "CheckIn/IMAGE_CHECKIN_ANIMAION%d_0.png", i + 1);
		animation->addSpriteFrameWithFile(szTemp);
		sprintf(szTemp, "CheckIn/IMAGE_CHECKIN_ANIMAION%d_1.png", i + 1);
		animation->addSpriteFrameWithFile(szTemp);
		sprintf(szTemp, "CheckIn/IMAGE_CHECKIN_ANIMAION8.png");
		animation->addSpriteFrameWithFile(szTemp);
		sprintf(szTemp, "CheckIn/IMAGE_CHECKIN_ANIMAION9.png");
		animation->addSpriteFrameWithFile(szTemp);
		sprintf(szTemp, "CheckIn/IMAGE_CHECKIN_FINISH%d.png", i + 1);
		animation->addSpriteFrameWithFile(szTemp);
		animation->setDelayPerUnit(0.1f);
		m_act[i] = Animate::create(animation);
		m_act[i]->retain();
		//m_sprImageCard[i]->runAction(act);
		addChild(m_sprImageCard[i]);
	}
	m_sprImageCard[0]->setPosition(Vec2(436.33f, 394.20f));
	m_sprImageCard[1]->setPosition(Vec2(632.67f, 394.20f));
	m_sprImageCard[2]->setPosition(Vec2(826.01f, 394.20f));
	m_sprImageCard[3]->setPosition(Vec2(351.34f, 240.89f));
	m_sprImageCard[4]->setPosition(Vec2(535.52f, 240.89f));
	m_sprImageCard[5]->setPosition(Vec2(727.90f, 240.89f));
	m_sprImageCard[6]->setPosition(Vec2(905.67f, 240.89f));

	return true;
}

bool CheckInScene::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void CheckInScene::onTouchMoved(Touch *touch, Event *event)
{
}

void CheckInScene::onTouchEnded(Touch *touch, Event *event)
{
}

void CheckInScene::onTouchCancelled(Touch *touch, Event *event)
{
}

void CheckInScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);

	//发送数据
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_CHECKIN_QUERY, packet.getBuffer(), packet.getPosition());
}

void CheckInScene::onExit()
{
	Layer::onExit();
}

bool CheckInScene::OnSocketCheckIn(uint16 sub_cmd_id, void * pData, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GP_CHECKIN_INFO:		// 签到信息
	{
		//参数校验
		assert(sizeof(CMD_GP_CheckInInfo) == data_size);
		if (sizeof(CMD_GP_CheckInInfo) != data_size) return false;

		//提取数据
		CMD_GP_CheckInInfo * pCheckInfo = (CMD_GP_CheckInInfo *)pData;

		//设置变量
		m_wDateIndex = pCheckInfo->wSeriesDate;
		m_bTodayChecked = pCheckInfo->bTodayChecked;
		memcpy((m_lRewardGold), pCheckInfo->lRewardGold, sizeof(m_lRewardGold));

		updateControl();

		return true;
	}
	case SUB_GP_CHECKIN_RESULT:		// 签到结果
	{
		//参数校验
		assert(data_size <= sizeof(CMD_GP_CheckInResult));
		if (data_size > sizeof(CMD_GP_CheckInResult)) return false;

		//提取数据
		CMD_GP_CheckInResult * pCheckInResult = (CMD_GP_CheckInResult *)pData;

		//播放动画
		if (pCheckInResult->bSuccessed == true)
		{
			//获取对象
			GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();
			//设置变量			
			m_bTodayChecked = true;
			pGlobalUserData->user_score = pCheckInResult->lScore;

			//发送事件
			// 更新用户分数
			//CPlatformEvent * pPlatformEvent = CPlatformEvent::GetInstance();
			//if (pPlatformEvent != NULL) pPlatformEvent->SendPlatformEvent(EVENT_USER_INFO_UPDATE, 0L);

			//执行动画
			m_sprImageCard[m_wDateIndex]->runAction(Sequence::create(m_act[m_wDateIndex],
				CallFunc::create(CC_CALLBACK_0(CheckInScene::updateControl, this)), nullptr));
			m_wDateIndex = MIN(m_wDateIndex + 1, LEN_WEEK);

		}
		else
		{
			//std::string str = l2u(pCheckInResult->szNotifyContent);
			std::string ret;
			ret.resize(128 * 2 + 2, '\0');
			memcpy(&ret[0], pCheckInResult->szNotifyContent, 128 * 2);
			ret = l2u(ret);
			MsgBox(ret.c_str());
		}

		//updateControl();

		return true;
	}

	assert(false);
	return false;
	}

	return false;
}

//关闭窗口
void CheckInScene::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

//执行签到
void CheckInScene::OnClickCheckInDone(Ref* sender)
{
	GlobalUserData* pGlobalUserData = g_global_userinfo.GetGlobalUserData();
	PACKET_HELPER_SIZE(512);
	packet.write4Byte(pGlobalUserData->user_id);
	packet.writeString(pGlobalUserData->password, LEN_PASSWORD);
	packet.writeString("LEN_MACHINE_ID", LEN_MACHINE_ID);

	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_CHECKIN_DONE, packet.getBuffer(), packet.getPosition());
}

//更新控件
void CheckInScene::updateControl()
{
	char szTemp[64] = { 0 };
	for (int i = 0; i < LEN_WEEK; i++)
	{
		m_text[i]->setString(StringUtils::toString(m_lRewardGold[i]));
	}

	m_textDate->setString(StringUtils::toString(m_wDateIndex));

	for (INT i = 0; i <= m_wDateIndex; i++)
	{
		if (i != m_wDateIndex)
		{
			sprintf(szTemp, "CheckIn/IMAGE_CHECKIN_FINISH%d.png", i + 1);
			m_sprImageCard[i]->initWithFile(szTemp);
		}
		else
		{
			if (!m_bTodayChecked)
			{
				sprintf(szTemp, "CheckIn/IMAGE_CHECKIN_ANIMAION%d_0.png", i + 1);
				m_sprImageCard[i]->initWithFile(szTemp);
			}
		}
	}

	if (m_bTodayChecked)
	{
		m_btCheckIn->setEnabled(false);
	}
	else
	{
		m_btCheckIn->setEnabled(true);
	}

	LobbyScene::getInstance()->UpdateUserScore();
}