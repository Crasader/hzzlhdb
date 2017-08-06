
#include "ranking_scene.h"

#include "cocostudio/CocoStudio.h"

#include "../base/convert.h"

Scene* RankingScene::createScene()
{
	auto scene = Scene::create();

	auto layer = RankingScene::create();

	scene->addChild(layer);

	return scene;
}

RankingScene::RankingScene()
{
}

RankingScene::~RankingScene()
{
}

bool RankingScene::init()
{
	if (!Layer::init())
	{
		return false;
	}


	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(RankingScene::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(RankingScene::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(RankingScene::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(RankingScene::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("ranking.csb");
	addChild(ui);

	Button* btn_back = (Button*)Helper::seekWidgetByName((Widget*)ui, "back");
	btn_back->addClickEventListener(CC_CALLBACK_1(RankingScene::OnClickBack, this));
	Button* btn_back0 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_0");
	btn_back0->addClickEventListener(CC_CALLBACK_1(RankingScene::OnClickBack, this));
	Button* btn_back1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_1");
	btn_back1->addClickEventListener(CC_CALLBACK_1(RankingScene::OnClickBack, this));
	Button* btn_back2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_2");
	btn_back2->addClickEventListener(CC_CALLBACK_1(RankingScene::OnClickBack, this));
	Button* btn_back3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_3");
	btn_back3->addClickEventListener(CC_CALLBACK_1(RankingScene::OnClickBack, this));


	//listview_ = (ListView*)Helper::seekWidgetByName((Widget*)ui, "ListView");
	//listview_->setScrollBarEnabled(false);
	rank_name[0] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_name0");
	rank_name[1] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_name1");
	rank_name[2] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_name2");
	rank_name[3] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_name3");
	rank_name[4] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_name4");
	rank_name[5] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_name5");
	rank_name[6] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_name6");
	rank_name[7] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_name7");
	rank_name[8] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_name8");
	rank_name[9] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_name9");


	rank_score[0] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_score0");
	rank_score[1] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_score1");
	rank_score[2] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_score2");
	rank_score[3] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_score3");
	rank_score[4] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_score4");
	rank_score[5] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_score5");
	rank_score[6] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_score6");
	rank_score[7] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_score7");
	rank_score[8] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_score8");
	rank_score[9] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_score9");


	rank_id[0] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_id0");
	rank_id[1] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_id1");
	rank_id[2] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_id2");
	rank_id[3] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_id3");
	rank_id[4] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_id4");
	rank_id[5] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_id5");
	rank_id[6] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_id6");
	rank_id[7] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_id7");
	rank_id[8] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_id8");
	rank_id[9] = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_id9");


	return true;
}

void RankingScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	PlazaKernel::getInstance()->set_ranking_delegate(this);

	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_GET_RANKING, nullptr, 0);
}

void RankingScene::onExit()
{
	Layer::onExit();

	PlazaKernel::getInstance()->set_ranking_delegate(nullptr);
}

bool RankingScene::OnSocketRankingResult(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GP_RANKING_RESULT:
	{
		// 效验参数
		assert(data_size == sizeof(CMD_GP_RankingResult));
		if (data_size != sizeof(CMD_GP_RankingResult))
			return false;

		CMD_GP_RankingResult ranking_result;
		memset(&ranking_result, 0, sizeof(ranking_result));

		SCORE score[arraysize(ranking_result.lScore)];
		memset(score, 0, sizeof(score));
		std::string nickname[arraysize(ranking_result.lScore)];

		DWORD Rand_ID[arraysize(ranking_result.Rank_ID)];

		PACKET_HELPER_DATA(data);
		for (WORD i = 0; i < arraysize(ranking_result.lScore); i++)
		{
			score[i] = packet.read8Byte();
			//CCLOG(StringUtils::toString(score[i]).c_str());
		}
		for (WORD i = 0; i < arraysize(ranking_result.lScore); i++)
		{
			nickname[i] = packet.readString(LEN_NICKNAME);
			//CCLOG(nickname[i].c_str());
		}
		for (WORD i = 0; i < arraysize(ranking_result.lScore); i++)
		{
			Rand_ID[i] = packet.read4Byte();
			//CCLOG(nickname[i].c_str());
		}

		//for (int i = 0; i < arraysize(score); ++i)
		//{
		//	if (score[i] == 0)
		//		break;
		//	AddRankingItem(i + 1, nickname[i], score[i], Rand_ID[i]);
		//}
		for (int i = 0; i < 10; ++i)
		{
			rank_name[i]->setString(StringUtils::toString(nickname[i]));
			rank_score[i]->setString(StringUtils::toString(score[i]));
			rank_id[i]->setString(StringUtils::toString(Rand_ID[i]));
		}

		return true;
	}
	}

	return false;
}

void RankingScene::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
	//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOBBY_SCENE");
}

void RankingScene::AddRankingItem(int order, const std::string nickname, SCORE score, WORD RankID)
{
	//Layout* layout = Layout::create();
	//layout->setContentSize(Size(1280, 131));

	//Scale9Sprite* item_bg = Scale9Sprite::create("ranking/sc_gz.png");
	//item_bg->setCapInsets(Rect(27, 0, 113, 90));
	//item_bg->setContentSize(Size(1280, 131));
	//item_bg->setPosition(Vec2(640, 131 / 2));
	//layout->addChild(item_bg);

	//if (order <= 3)
	//{
	//	ImageView* icon = ImageView::create(StringUtils::format("ranking/rank-no%02d.png", order));
	//	icon->setPosition(Vec2(190, 131 / 2));
	//	layout->addChild(icon);
	//}
	//else
	//{
	//	ImageView* icon = ImageView::create("ranking/rank-rank-bg.png");
	//	icon->setPosition(Vec2(190, 131 / 2));
	//	layout->addChild(icon);

	//	Text* rank_no = Text::create(StringUtils::toString(order), "arial", 30);
	//	rank_no->setPosition(Vec2(190, 131 / 2));
	//	layout->addChild(rank_no);
	//}

	//Text* textnickname = Text::create(nickname, "arial", 30);
	////textnickname->setColor(Color3B(253, 221, 0));
	//textnickname->setPosition(Vec2(640, 131 /2));
	//layout->addChild(textnickname);

	//Text* text_score = Text::create(StringUtils::toString(score), "arial", 30);
	//text_score->setColor(Color3B(255, 255, 255));
	//text_score->setPosition(Vec2(1090, 131 / 2));
	//layout->addChild(text_score);

	//listview_->pushBackCustomItem(layout);
}
bool RankingScene::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}
void RankingScene::onTouchMoved(Touch *touch, Event *event)
{
}

void RankingScene::onTouchEnded(Touch *touch, Event *event)
{
}

void RankingScene::onTouchCancelled(Touch *touch, Event *event)
{
}