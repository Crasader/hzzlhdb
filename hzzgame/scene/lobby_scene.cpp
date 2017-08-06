
#include "lobby_scene.h"

#include "cocostudio/CocoStudio.h"

#include "../common/UIHelperEx.h"
#include "../data/global_user_info.h"
#include "../kernel/plaza_kernel.h"
#include "../kernel/server_kernel.h"

static LobbyScene * g_pLobbyScene = nullptr;

LobbyScene * LobbyScene::getInstance()
{
	return g_pLobbyScene;
}

Scene* LobbyScene::createScene()
{
    auto scene = Scene::create();
    
	auto layer = LobbyScene::create();

    scene->addChild(layer);
	getInstance()->UpdateUserScore();

    return scene;
}

LobbyScene::LobbyScene()
{
	g_pLobbyScene = this;
}

LobbyScene::~LobbyScene()
{
}

bool LobbyScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();

	auto lobby = CSLoader::createNode("lobby.csb");
	addChild(lobby);

	top_bar_layer_ = CSLoader::createNode("lobby_top.csb");
	addChild(top_bar_layer_);

	photo_ = (Sprite*)HelperEx::seekNodeByName(top_bar_layer_, "photo");
	auto nickname = (Text*)HelperEx::seekNodeByName(top_bar_layer_, "nickname");
	nickname->setString(user_data->nickname);
	userscore_ = (Text*)Helper::seekWidgetByName((Widget*)top_bar_layer_, "userscore");
	//userscore_->setString(StringUtils::toString(user_data->user_score));

	userscore_->setString(StringUtils::format("%.2fW", user_data->user_score / 10000.0));

	
	usergameid_ = (Text*)Helper::seekWidgetByName((Widget*)top_bar_layer_, "gameid");
	usergameid_->setString(StringUtils::format("%d",user_data->game_id));


	usermedal_ = (Text*)Helper::seekWidgetByName((Widget*)top_bar_layer_, "usermedal");
	usermedal_->setString(StringUtils::toString(user_data->user_Medal));

	userLottery_ = (Text*)Helper::seekWidgetByName((Widget*)top_bar_layer_, "userlottery");
	userLottery_->setString(StringUtils::toString(user_data->user_Lottery));

	bottom_bar_layer_ = CSLoader::createNode("lobby_bottom.csb");
	addChild(bottom_bar_layer_);

	kindlist_layer_ = CSLoader::createNode("lobby_kindlist.csb");
	addChild(kindlist_layer_);

	serverlist_layer_ = CSLoader::createNode("lobby_serverlist.csb");
	addChild(serverlist_layer_);
	serverlist_layer_->setVisible(false);

	m_pServerList = (ListView*)Helper::seekWidgetByName((Widget*)serverlist_layer_, "serverlist");
	m_pServerList->setScrollBarEnabled(false);

	auto horn = (Button*)HelperEx::seekNodeByName(lobby, "horn");
	horn->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickHorn, this));

	auto horn_bg = (ImageView*)HelperEx::seekNodeByName(lobby, "horn_bg");
	horn_bg->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickHornView, this));

	photo_box_ = (ImageView*)HelperEx::seekNodeByName(top_bar_layer_, "photo_box");
	photo_box_->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickFace, this));

	back_ = (ImageView*)HelperEx::seekNodeByName(top_bar_layer_, "back");
	back_->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickBack, this));

	auto news = (ImageView*)HelperEx::seekNodeByName(top_bar_layer_, "news");
	news->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickNews, this));

	auto setting = (ImageView*)HelperEx::seekNodeByName(top_bar_layer_, "setting");
	setting->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickSetting, this));

	auto more = (ImageView*)HelperEx::seekNodeByName(top_bar_layer_, "more");
	more->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickMore, this));

	//auto activity = (ImageView*)HelperEx::seekNodeByName(bottom_bar_layer_, "activity");
	//activity->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickActivity, this));

	//auto exchange = (ImageView*)HelperEx::seekNodeByName(bottom_bar_layer_, "exchange");
	//exchange->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickExchange, this));


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	auto store = (ImageView*)HelperEx::seekNodeByName(bottom_bar_layer_, "store");
	store->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickStore, this));
#else
	auto store = (ImageView*)HelperEx::seekNodeByName(bottom_bar_layer_, "store");
	store->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickStoreWeb, this));
#endif

	auto bank = (ImageView*)HelperEx::seekNodeByName(bottom_bar_layer_, "bank");
	bank->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickBank, this));

	auto Exchange = (ImageView*)HelperEx::seekNodeByName(bottom_bar_layer_, "Exchange");
	Exchange->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickExchange, this));

	auto CheckIn = (ImageView*)HelperEx::seekNodeByName(bottom_bar_layer_, "CheckIn");
	CheckIn->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickCheckIn, this));
	
	auto Ranking = (ImageView*)HelperEx::seekNodeByName(bottom_bar_layer_, "Ranking");
	Ranking->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickRanking, this));

	auto WXShare = (ImageView*)HelperEx::seekNodeByName(bottom_bar_layer_, "WXShare");
	WXShare->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickWXShare, this));
	
	auto Auction = (ImageView*)HelperEx::seekNodeByName(bottom_bar_layer_, "Auction");
	Auction->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickAuction, this));

	//auto quickstart = (ImageView*)HelperEx::seekNodeByName(bottom_bar_layer_, "quickstart");
	//quickstart->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickQuickStart, this));

	auto kindlistview = (ListView*)Helper::seekWidgetByName((Widget*)kindlist_layer_, "kindlist");
	kindlistview->setScrollBarEnabled(false);
	kindlistview->setItemsMargin(20);
	kindlistview->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(LobbyScene::selectedItemEvent, this));
	const GameKindVector& kindlist = PlazaKernel::getInstance()->game_kind_list();
	for (GameKindVector::const_iterator it = kindlist.begin(); it != kindlist.end(); ++it)
	{
		tagGameKind* game_kind = *it;
		//ImageView* button = ImageView::create(StringUtils::format("lobby/game_kind/game_kind_%d.png", game_kind->wKindID));
		Button * button = Button::create(StringUtils::format("lobby/game_kind/game_kind_%d.png", game_kind->wKindID), StringUtils::format("lobby/game_kind/11game_kind_%d.png", game_kind->wKindID));
		button->setTouchEnabled(true);

		

		//btSound1 = Button::create("lkpy/Resources/setting_button0.png", "lkpy/Resources/setting_button1.png");

		Layout* item = Layout::create();
		item->setContentSize(Size(button->getContentSize().width, button->getContentSize().height));
		button->setPosition(Vec2(item->getContentSize().width / 2.0f, item->getContentSize().height / 2.0f));
		item->addChild(button);

		kindlistview->pushBackCustomItem(item);
	}
    
    return true;
}

void LobbyScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	//获取对象
	GlobalUserData * pGlobalUserData = g_global_userinfo.GetGlobalUserData();

	PACKET_HELPER_SIZE(128);
	packet.write4Byte(pGlobalUserData->user_id);

	//发送数据
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_UPDATE_USERINFO, packet.getBuffer(), packet.getPosition());
}

void LobbyScene::OnClickHorn(Ref* sender)
{

}

void LobbyScene::OnClickHornView(Ref* sender)
{

}

void LobbyScene::OnClickFace(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_USERINFO_SCENE");
}

void LobbyScene::OnClickBack(Ref* sender)
{
	BackToGameKindList();
}

void LobbyScene::OnClickSetting(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_SETTING_SCENE");
}

void LobbyScene::OnClickNews(Ref* sender)
{

}

void LobbyScene::OnClickMore(Ref* sender)
{

}

void LobbyScene::OnClickActivity(Ref* sender)
{

}

void LobbyScene::OnClickStore(Ref* sender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	ShellExecute(NULL, TEXT("OPEN"), TEXT("http://pay.hzzgame.com/pay/PayWeiXin.aspx"), NULL, NULL, SW_NORMAL);
#else
	//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_STORE_SCENE_WEB");
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_STORE_SCENE");
#endif
}
//充值Web
void LobbyScene::OnClickStoreWeb(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_STORE_SCENE_WEB");
}

void LobbyScene::OnClickBank(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_BANK_SCENE");
}

void LobbyScene::OnClickExchange(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_EXCHANGE_SCENE");
}

void LobbyScene::OnClickCheckIn(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_CHECKIN_SCENE");
}

void LobbyScene::OnClickRanking(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_RANKING_SCENE");
}
void LobbyScene::OnClickWXShare(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_WXSHARE_SCENE");
}
//拍卖
void LobbyScene::OnClickAuction(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_AUCTION_SCENE");
}

void LobbyScene::OnClickQuickStart(Ref* sender)
{

}

void LobbyScene::ShowGameServerList(uint16 kind_id)
{
	kindlist_layer_->setVisible(false);

	m_pServerList->removeAllChildrenWithCleanup(true);

	const GameServerVector& serverlist = PlazaKernel::getInstance()->GetGameServerList(kind_id);

	memset(m_GameServer, 0, sizeof(m_GameServer));
	int nServerCount = 0;
	for (GameServerVector::const_iterator it = serverlist.begin(); it != serverlist.end(); ++it)
	{
		m_GameServer[nServerCount++] = **it;
	}

	for (int i = 0; i < nServerCount - 1; i++)
	{ 
		for (int j = 0; j < nServerCount - i - 1; j++)
		{
			if (m_GameServer[j].wSortID > m_GameServer[j + 1].wSortID)
			{
				tagGameServer Temp;
				memcpy(&Temp, &m_GameServer[j], sizeof(tagGameServer));
				memcpy(&m_GameServer[j], &m_GameServer[j + 1], sizeof(tagGameServer));
				memcpy(&m_GameServer[j + 1], &Temp, sizeof(tagGameServer));
			}
		}
	}

	for (int i = 0; i < nServerCount; i++)
	{
		tagGameServer* game_server = &m_GameServer[i];
		//ImageView * pImageServer = ImageView::create(StringUtils::format("lobby/game_server/game_server_%d.png", game_server->cbServerLevel));
		Button * pImageServer = Button::create(StringUtils::format("lobby/game_server/game%d_server_%d.png", game_server->wKindID, game_server->cbServerLevel), StringUtils::format("lobby/game_server/11game%d_server_%d.png", game_server->wKindID, game_server->cbServerLevel));
		pImageServer->setTouchEnabled(true);
		//pImageServer->setTitleText(game_server->szStartTime);
		pImageServer->setUserData(game_server);
		pImageServer->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickGameServer, this));
		m_pServerList->addChild(pImageServer);


		Text *	StartTime = Text::create(a2u(game_server->szStartTime), "arial", 26);
		StartTime->setPosition(Vec2(217, 49));
		StartTime->setTextColor(Color4B(28, 224, 239, 255));
		StartTime->setVisible(true);
		pImageServer->addChild(StartTime);
	}

	//for (GameServerVector::const_iterator it = serverlist.begin(); it != serverlist.end(); ++it)
	//{ 
	//	tagGameServer* game_server = *it;
	//	ImageView * pImageServer = ImageView::create(StringUtils::format("lobby/game_server/game_server_%d.png", game_server->cbServerLevel));
	//	pImageServer->setTouchEnabled(true);
	//	pImageServer->setUserData(game_server);
	//	pImageServer->addClickEventListener(CC_CALLBACK_1(LobbyScene::OnClickGameServer, this));
	//	//m_pServerList->addChild(pImageServer, game_server->wSortID, game_server->wSortID);
	//	m_pServerList->addChild(pImageServer/*, game_server->wSortID*/);
	//}

	serverlist_layer_->setVisible(true);

	auto act1 = MoveBy::create(0.2f, Vec2(0, 105));
	top_bar_layer_->runAction(Sequence::create(act1, CallFunc::create([=](){ SetTitle(kind_id); }), act1->reverse(), nullptr));

	auto act2 = MoveBy::create(0.2f, Vec2(0, -143));
	bottom_bar_layer_->runAction(Sequence::create(act2, act2->reverse(), nullptr));
}

void LobbyScene::BackToGameKindList()
{
	kindlist_layer_->setVisible(true);
	serverlist_layer_->setVisible(false);

	auto act1 = MoveBy::create(0.2f, Vec2(0, 105));
	top_bar_layer_->runAction(Sequence::create(act1, CallFunc::create([=](){ SetTitle(0); }), act1->reverse(), nullptr));

	auto act2 = MoveBy::create(0.2f, Vec2(0, -143));
	bottom_bar_layer_->runAction(Sequence::create(act2, act2->reverse(), nullptr));
}

void LobbyScene::SetTitle(uint16 kind_id)
{
	//if (kind_id == 0)
	//{
		//spr_title_->setTexture("main_scene/title.png");
		photo_box_->setVisible(true);
		photo_->setVisible(true);
		//back_->setVisible(false);
		back_->setVisible(true);
	//}
	//else
	//{
	//	//spr_title_->setTexture("main_scene/title1.png");
	//	photo_box_->setVisible(false);
	//	photo_->setVisible(false);
	//	back_->setVisible(true);
	//}
}

void LobbyScene::selectedItemEvent(Ref *pSender, ListView::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_START:
	{
		ListView* listView = static_cast<ListView*>(pSender);
		CC_UNUSED_PARAM(listView);
		break;
	}
	case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END:
	{
		ListView* listView = static_cast<ListView*>(pSender);
		size_t index = listView->getCurSelectedIndex();
		const GameKindVector& kindlist = PlazaKernel::getInstance()->game_kind_list();
		assert(index < kindlist.size());
		if (index >= kindlist.size())
			return;

		tagGameKind* game_kind = kindlist[index];
		uint16 kindid = game_kind->wKindID;
		ShowGameServerList(kindid);

		break;
	}
	default:
		break;
	}
}

void LobbyScene::OnClickGameServer(Ref* sender)
{
	//Node* node = dynamic_cast<Node*>(sender);
	ImageView * pImageServer = (ImageView*)(sender);
	tagGameServer* game_server = (tagGameServer*)(pImageServer->getUserData());

	// 关闭之前的房间
	if (ServerKernel::get())
	{
		ServerKernel::get()->Disconnect();
		ServerKernel::destory();
	}

	const tagGameKind* game_kind = PlazaKernel::getInstance()->GetGameKind(game_server->wKindID);
	assert(game_kind);
	if (game_kind == nullptr)
		return;

	ServerKernel* kernel = ServerKernel::create();
	kernel->Connect(game_kind, game_server);
}

//更新用户分数
void LobbyScene::UpdateUserScore()
{
	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	//超过1万金币的显示  123.12万
	if (user_data->user_score > 100000000)
	{
		userscore_->setString(a2u(StringUtils::format("%.1fW", user_data->user_score / 10000.0)));
	}
	else if (user_data->user_score > 10000)
	{
		userscore_->setString(a2u(StringUtils::format("%.2fW", user_data->user_score / 10000.0)));
	}
	else
	{
		userscore_->setString(StringUtils::toString(user_data->user_score));
	}

	usermedal_->setString(StringUtils::toString(user_data->user_Medal));
	userLottery_->setString(StringUtils::toString(user_data->user_Lottery));
}