
#include "room_scene.h"

#include "cocostudio/CocoStudio.h"

#include "../base/convert.h"
#include "../base/base_util.h"
#include "dialog_msg_box.h"
#include "../common/UIHelperEx.h"
#include "../../../../command/CMD_GameServer.h"

Scene* RoomScene::createScene()
{
    auto scene = Scene::create();
    
	auto layer = RoomScene::create();

    scene->addChild(layer);

    return scene;
}

RoomScene::RoomScene()
{
}

RoomScene::~RoomScene()
{
	if (ServerKernel::get())
	{
		ServerKernel::get()->GetGameTableProxy()->SetDelegate(nullptr);
	}
}

bool RoomScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto ui = CSLoader::createNode("room.csb");
	addChild(ui);

	tablelist_ = (ListView*)Helper::seekWidgetByName((Widget*)ui, "tablelist");
	tablelist_->setScrollBarEnabled(false);
	auto servername = (Text*)Helper::seekWidgetByName((Widget*)ui, "servername");
	auto quicksit = (Button*)Helper::seekWidgetByName((Widget*)ui, "quickstart");
	auto back = (Button*)Helper::seekWidgetByName((Widget*)ui, "back");
	auto Button_1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "Button_1");
	//auto bg = (Sprite*)Helper::seekWidgetByName((Widget*)ui, "bg");
	//bg->setVisible(false);
	if (ServerKernel::get()->getServerType() == GAME_GENRE_MATCH)
	{
		quicksit->setVisible(true);
		back->setVisible(true);
		//bg->setVisible(true);
		Button_1->setVisible(true);
	}
	quicksit->addClickEventListener(CC_CALLBACK_1(RoomScene::OnClickQuickSitdown, this));

	Button* login_startgame = (Button*)Helper::seekWidgetByName((Widget*)ui, "back");
	login_startgame->addClickEventListener(CC_CALLBACK_1(RoomScene::OnClickBack, this));

	ImageView* id_input_ = (ImageView*)HelperEx::seekNodeByName(ui, "id_input");
	id_input = EditBox::create(id_input_->getContentSize(), "login/login_input.png");
	id_input->setPosition(id_input_->getPosition());
	id_input->setPlaceHolder(a2u("id").c_str());
	id_input->setPlaceholderFontColor(Color3B(169, 169, 169));
	id_input->setPlaceholderFontSize(36);
	id_input->setFontSize(36);
	id_input->setFontColor(Color3B::BLACK);
	id_input->setInputMode(EditBox::InputMode::NUMERIC);
	id_input->setMaxLength(12);
	//addChild(id_input);

	Button* kick_id = (Button*)Helper::seekWidgetByName((Widget*)ui, "kick_id");
	kick_id->addClickEventListener(CC_CALLBACK_1(RoomScene::OnClickKick, this));

	Button* kick_all_user = (Button*)Helper::seekWidgetByName((Widget*)ui, "kick_all_user");
	kick_all_user->addClickEventListener(CC_CALLBACK_1(RoomScene::OnClickKickAllUser, this));

	Button* prohibited_entry = (Button*)Helper::seekWidgetByName((Widget*)ui, "prohibited_entry");
	prohibited_entry->addClickEventListener(CC_CALLBACK_1(RoomScene::OnClickProhibitedEntry, this));


	//奖品坐标
	//奖品坐标
	tePrize[0] = Text::create("0", "arial", 30);
	tePrize[0]->setPosition(Vec2(visibleSize.width / 2 + 30, visibleSize.height / 2 +28));
	tePrize[0]->setTextColor(Color4B(255, 222, 0, 255));
	tePrize[0]->setVisible(false);
	addChild(tePrize[0]);

	tePrize[1] = Text::create("0", "arial", 30);
	tePrize[1]->setPosition(Vec2(visibleSize.width / 2 + 30, visibleSize.height / 2 - 19));
	tePrize[1]->setTextColor(Color4B(255, 222, 0, 255));
	tePrize[1]->setVisible(false);
	addChild(tePrize[1]);

	tePrize[2] = Text::create("0", "arial", 30);
	tePrize[2]->setPosition(Vec2(visibleSize.width / 2 + 30, visibleSize.height / 2 - 67));
	tePrize[2]->setTextColor(Color4B(255, 222, 0, 255));
	tePrize[2]->setVisible(false);
	addChild(tePrize[2]);

	tePrize[3] = Text::create("0", "arial", 30);
	tePrize[3]->setPosition(Vec2(visibleSize.width / 2 + 30, visibleSize.height / 2 - 113));
	tePrize[3]->setTextColor(Color4B(255, 222, 0, 255));
	tePrize[3]->setVisible(false);
	addChild(tePrize[3]);

	tePrize[4] = Text::create("0", "arial", 30);
	tePrize[4]->setPosition(Vec2(visibleSize.width / 2 + 30, visibleSize.height / 2 - 160));
	tePrize[4]->setTextColor(Color4B(255, 222, 0, 255));
	tePrize[4]->setVisible(false);
	addChild(tePrize[4]);

	tePrize[5] = Text::create("0", "arial", 30);
	tePrize[5]->setPosition(Vec2(visibleSize.width / 2 + 30, visibleSize.height / 2 - 209));
	tePrize[5]->setTextColor(Color4B(255, 222, 0, 255));
	tePrize[5]->setVisible(false);
	addChild(tePrize[5]);

	teRoomName = Text::create("0", "arial", 30);
	teRoomName->setPosition(Vec2(visibleSize.width / 2 + 40, visibleSize.height / 2 + 236));
	teRoomName->setTextColor(Color4B(255, 222, 0, 255));
	teRoomName->setVisible(false);
	addChild(teRoomName);
	
	teTime = Text::create("0", "arial", 30);
	teTime->setPosition(Vec2(visibleSize.width / 2 + 39, visibleSize.height / 2 + 195));
	teTime->setTextColor(Color4B(255, 222, 0, 255));
	teTime->setVisible(false);
	addChild(teTime);

	teCost = Text::create("0", "arial", 30);
	teCost->setPosition(Vec2(visibleSize.width / 2 + 40, visibleSize.height / 2 + 151));
	teCost->setTextColor(Color4B(255, 222, 0, 255));
	teCost->setVisible(false);
	addChild(teCost);

	if (ServerKernel::get())
	{
		servername->setString(ServerKernel::get()->GetGameServer()->szServerName);

		auto node = CSLoader::createNode(StringUtils::format("table%d.csb", ServerKernel::get()->GetChairCount()));
		auto layout = (Layout*)Helper::seekWidgetByName((Widget*)node, "panel");

		uint16 table_count = ServerKernel::get()->GetTableCount();
		uint16 chair_count = ServerKernel::get()->GetChairCount();

		for (uint16 i = 0; i < table_count; ++i)
		{
			Layout* table = (Layout*)layout->clone();
			auto index = (Text*)Helper::seekWidgetByName(table, "index");
			index->setString(StringUtils::toString(i + 1));
			for (uint16 j = 0; j < chair_count; ++j)
			{
				auto chair = (ImageView*)Helper::seekWidgetByName(table, StringUtils::format("chair_%d", j));
				chair->setTag(makelong(i, j));
				chair->addClickEventListener(CC_CALLBACK_1(RoomScene::OnClickChair, this));
			}

			tablelist_->addChild(table);
		}

		for (uint16 i = 0; i < table_count; ++i)
		{
			SetTableStatus(i, ServerKernel::get()->GetGameTableProxy()->IsPlaying(i), ServerKernel::get()->GetGameTableProxy()->IsLocking(i));
			for (uint16 j = 0; j < chair_count; ++j)
			{
				SetTableUserItem(i, j, ServerKernel::get()->GetGameTableProxy()->GetTableUserItem(i, j));
			}
		}

		ServerKernel::get()->GetGameTableProxy()->SetDelegate(this);
	}

    return true;
}

void RoomScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	//if (ServerKernel::get()->GetGameKind()->wKindID != 2301)
	//if (ServerKernel::get()->getServerType() != GAME_GENRE_MATCH)
	//{
		OnClickQuickSitdown(nullptr);
	//}
	//else
	//{
	//	for (int i = 0; i < 6; i++)
	//	{
	//		tePrize[i]->setString(ServerKernel::get()->GetGameServer()->szPrize[i]);
	//		tePrize[i]->setVisible(true);
	//	}

	//	teRoomName->setString(ServerKernel::get()->GetGameServer()->szServerName);
	//	teRoomName->setVisible(true);

	//	teTime->setString(ServerKernel::get()->GetGameServer()->szStartTime);
	//	teTime->setVisible(true);

	//	teCost->setString(StringUtils::format("%ld", ServerKernel::get()->GetGameServer()->lMatchCost));
	//	teCost->setVisible(true);
	//}
}

void RoomScene::onExit()
{
	Layer::onExit();
	
	//if (ServerKernel::get())
	//{
	//	ServerKernel::get()->GetGameTableProxy()->SetDelegate(nullptr);
	//}
}

// 设置用户
bool RoomScene::SetTableUserItem(uint16 table_id, uint16 chair_id, IGameUserItem* user_item)
{
	if (ServerKernel::get() == nullptr)
		return false;
	if (ServerKernel::get()->GetChairCount() >= MAX_CHAIR)
		return false;

	auto table = dynamic_cast<Layout*>(tablelist_->getItem(table_id));
	assert(table);
	if (table == nullptr)
		return false;

	auto chair = (ImageView*)Helper::seekWidgetByName(table, StringUtils::format("chair_%d", chair_id));
	assert(chair);
	if (chair == nullptr)
		return false;

	chair->setUserData(user_item);
	//if (user_item != nullptr)
	//{
	//	// 设置头像
	//	chair->setColor(Color3B(255, 0, 0));
	//	// 名字
	//	auto name = (Text*)Helper::seekWidgetByName(table, StringUtils::format("name_%d", chair_id));
	//	assert(name);
	//	name->setString(user_item->GetNickName());
	//	// gameid
	//	auto gameid = (Text*)Helper::seekWidgetByName(table, StringUtils::format("gameid_%d", chair_id));
	//	assert(gameid);
	//	gameid->setString(StringUtils::format("%u", user_item->GetUserID()));

	//	// 准备
	//	auto ready = (ImageView*)Helper::seekWidgetByName(table, StringUtils::format("ready_%d", chair_id));
	//	assert(ready);
	//	if (user_item->GetUserStatus() == US_READY)
	//	{
	//		ready->setVisible(true);
	//	}
	//	else
	//	{
	//		ready->setVisible(false);
	//	}
	//}
	//else
	//{ 
	//	// 设置头像
	//	chair->setColor(Color3B::WHITE);
	//	// 名字
	//	auto name = (Text*)Helper::seekWidgetByName(table, StringUtils::format("name_%d", chair_id));
	//	assert(name);
	//	name->setString("");
	//	// gameid
	//	auto gameid = (Text*)Helper::seekWidgetByName(table, StringUtils::format("gameid_%d", chair_id));
	//	assert(gameid);
	//	gameid->setString("");

	//	// 准备
	//	auto ready = (ImageView*)Helper::seekWidgetByName(table, StringUtils::format("ready_%d", chair_id));
	//	assert(ready);
	//	ready->setVisible(false);
	//}

	return true;
}

// 桌子状态 
void RoomScene::SetTableStatus(uint16 table_id, bool playing, bool locker)
{
	auto table = dynamic_cast<Layout*>(tablelist_->getItem(table_id));
	assert(table);
	if (table == nullptr)
		return;

	auto normal = (ImageView*)Helper::seekWidgetByName(table, "normal");
	auto play = (ImageView*)Helper::seekWidgetByName(table, "playing");
	normal->setVisible(!playing);
	play->setVisible(playing);
}

void RoomScene::OnClickChair(Ref* sender)
{
	Node* node = dynamic_cast<Node*>(sender);
	int tag = node->getTag();
	uint16 table_id = loword(tag);
	uint16 chair_id = hiword(tag);

	if (!ServerKernel::get())
		return;

	if (table_id == INVALID_TABLE || chair_id == INVALID_CHAIR)
		return;

	// 获取用户
	IGameUserItem* table_user_item = NULL;
	if (chair_id != MAX_CHAIR)
		table_user_item = ServerKernel::get()->GetGameTableProxy()->GetTableUserItem(table_id, chair_id);

	// 消息处理
	if (table_user_item != NULL)
	{
		if (table_user_item == ServerKernel::get()->GetMeUserItem())
		{
			// 状态过滤
			if (ServerKernel::get()->GetMeUserItem()->GetUserStatus() >= US_PLAYING)
				return;
			if (ServerKernel::get()->IsRequestAction())
				return;

			// 起立动作
			ServerKernel::get()->PerformStandUpAction(table_id, chair_id);
		}
		else
		{
			// 状态过滤
			if (ServerKernel::get()->GetMeUserItem()->GetTableID() == table_id && ServerKernel::get()->GetMeUserItem()->GetChairID() == chair_id)
				return;
			if (ServerKernel::get()->IsRequestAction())
				return;

			// 旁观动作
			ServerKernel::get()->PerformLookonAction(table_id, chair_id);
		}
	}
	else
	{
		// 坐下动作
		ServerKernel::get()->PerformSitDownAction(table_id, chair_id, false);
	}
}

void RoomScene::OnClickQuickSitdown(Ref* sender)
{
	if (ServerKernel::get())
		ServerKernel::get()->PerformChangeChair();
}

//关闭房间
void RoomScene::OnClickBack(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOBBY_SCENE");
	ServerKernel::get()->Disconnect();
}

//踢人
void RoomScene::OnClickKick(Ref* sender)
{

	__String strGameID = id_input->getText();

	// 踢人
	ServerKernel::get()->KickUser(strGameID.uintValue());
}

//踢人
void RoomScene::OnClickKickAllUser(Ref* sender)
{
	// 踢人
	ServerKernel::get()->KickAllUser();
}

//禁止进入
void RoomScene::OnClickProhibitedEntry(Ref* sender)
{
	ServerKernel::get()->ProhibitedEntry();
}
