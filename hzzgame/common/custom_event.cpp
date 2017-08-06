
#include "custom_event.h"

#include "../scene/login_scene.h"
#include "../scene/register_scene.h"
#include "../scene/lobby_scene.h"
#include "../scene/userinfo_scene.h"
#include "../scene/setting_scene.h"
#include "../scene/store_scene.h"
#include "../scene/bank_scene.h"
#include "../scene/room_scene.h"
#include "../scene/CheckIn_scene.h"
#include "../scene/Auction_Scene.h"
#include "../scene/ranking_Scene.h"
#include "../scene/WXShare.h"
#include "../scene/Exchange_Scene.h"
#include "../scene/WebViewDlg.h"

static CustomEvent *g_CustomEvent = nullptr;

CustomEvent* CustomEvent::getInstance()
{
	if (!g_CustomEvent)
	{
		g_CustomEvent = new (std::nothrow) CustomEvent();
	}

	return g_CustomEvent;
}

void CustomEvent::destroyInstance()
{
	CC_SAFE_RELEASE_NULL(g_CustomEvent);
}

CustomEvent::CustomEvent()
{
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_LOGIN_SCENE", CC_CALLBACK_1(CustomEvent::CreateLoginScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_REGISTER_SCENE", CC_CALLBACK_1(CustomEvent::CreateRegisterScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_LOBBY_SCENE", CC_CALLBACK_1(CustomEvent::CreateLobbyScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_USERINFO_SCENE", CC_CALLBACK_1(CustomEvent::CreateUserInfoScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_SETTING_SCENE", CC_CALLBACK_1(CustomEvent::CreateSettingScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_STORE_SCENE", CC_CALLBACK_1(CustomEvent::CreateStoreScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_BANK_SCENE", CC_CALLBACK_1(CustomEvent::CreateBankScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_ROOM_SCENE", CC_CALLBACK_1(CustomEvent::CreateRoomScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_CHECKIN_SCENE", CC_CALLBACK_1(CustomEvent::CreateCheckInScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_AUCTION_SCENE", CC_CALLBACK_1(CustomEvent::CreateAuctionScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_RANKING_SCENE", CC_CALLBACK_1(CustomEvent::CreateRankingScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_WXSHARE_SCENE", CC_CALLBACK_1(CustomEvent::CreateWXShare, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_EXCHANGE_SCENE", CC_CALLBACK_1(CustomEvent::CreateExchangeScene, this));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("EVENT_CREATE_STORE_SCENE_WEB", CC_CALLBACK_1(CustomEvent::CreateStoreSceneWeb, this));
}

CustomEvent::~CustomEvent()
{
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_LOGIN_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_REGISTER_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_LOBBY_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_USERINFO_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_SETTING_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_STORE_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_BANK_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_ROOM_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_CHECKIN_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_AUCTION_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_RANKING_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_WXSHARE_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_EXCHANGE_SCENE");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("EVENT_CREATE_STORE_SCENE_WEB");
}

void CustomEvent::CreateLoginScene(EventCustom* event)
{
	auto scene = LoginScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void CustomEvent::CreateRegisterScene(EventCustom* event)
{
	auto scene = RegisterScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void CustomEvent::CreateLobbyScene(EventCustom* event)
{
	auto scene = LobbyScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void CustomEvent::CreateUserInfoScene(EventCustom* event)
{
	auto scene = UserInfoScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void CustomEvent::CreateSettingScene(EventCustom* event)
{
	auto scene = SettingScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void CustomEvent::CreateStoreScene(EventCustom* event)
{
	auto scene = StoreScene::createScene();
	//Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void CustomEvent::CreateStoreSceneWeb(EventCustom* event)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS )
	auto scene = WebViewDlg::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
#endif
}

void CustomEvent::CreateBankScene(EventCustom* event)
{
	auto scene = BankSaveScene::createScene();
	//Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void CustomEvent::CreateRoomScene(EventCustom* event)
{
	auto scene = RoomScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void CustomEvent::CreateCheckInScene(EventCustom* event)
{
	auto scene = CheckInScene::createScene();
	//Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
	Director::getInstance()->getRunningScene()->addChild(scene);
}
void CustomEvent::CreateRankingScene(EventCustom* event)
{
	auto scene = RankingScene::createScene();
	//Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void CustomEvent::CreateWXShare(EventCustom* event)
{
	auto scene = WXShare::createScene();
	//Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
	Director::getInstance()->getRunningScene()->addChild(scene);
}

//创建拍卖窗口
void CustomEvent::CreateAuctionScene(EventCustom* event)
{
	auto scene = AuctionScene::createScene();
	//Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
	Director::getInstance()->getRunningScene()->addChild(scene);
}

//创建兑换窗口
void CustomEvent::CreateExchangeScene(EventCustom* event)
{
	auto scene = ExchangeScene::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}