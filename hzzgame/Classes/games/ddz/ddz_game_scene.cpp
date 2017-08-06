#include "ddz_game_scene.h"

#include "audio/include/SimpleAudioEngine.h"
#include "../../base/convert.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace CocosDenshion;

namespace ddz
{

const uint32 kTimerStart = 1;
const uint32 kTimerStartTime = 15;
const uint32 kTimerCallScore = 2;
const uint32 kTimerCallScoreTime = 15;
const uint32 kTimerOutCard = 3;
const uint32 kTimerOutCardTime = 15;

GameScene::GameScene()
	: timer_(0), timer_id_(kuint32max), auto_out_card_(false), hand_card_count_(0), current_user_(kuint16max), lord_user_(kuint16max), lord_score_(0), turn_outcard_count_(0), search_card_result_index_(0), bottom_cards_ani_finish_(false)
{
	memset(hand_card_data_, 0, sizeof(hand_card_data_));
	memset(bottom_card_data_, 0, sizeof(bottom_card_data_));
	memset(turn_ountcard_data_, 0, sizeof(turn_ountcard_data_));
	memset(&search_card_result_, 0, sizeof(search_card_result_));
}

GameScene::~GameScene()
{
	ani_bomb_->release();
	ani_rocket_->release();
	ani_plane_->release();
	ani_chainpairs_->release();
	ani_straight_->release();
	ani_chuntian_->release();
	ani_alarm_->release();
	for (uint16 i = 0; i < kMaxPlayer; ++i)
	{
		ani_switch_role_[i]->release();
	}

	client_kernel_->SetDelegate(nullptr);
}

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}  

// on "init" you need to initialize your instance
bool GameScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	//if (!glview)
	//	return;
	//glview->setFrameSize(1280, 800);
	glview->setDesignResolutionSize(1280, 800, ResolutionPolicy::EXACT_FIT);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// 背景
    auto bg = Sprite::create("ddz/Resources/Image/back.png");
    bg->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    addChild(bg);
	// 角色
	spr_user_avatar_[0] = Sprite::create();
	spr_user_avatar_[0]->setPosition(Vec2(125, 502));
	spr_user_avatar_[0]->setVisible(false);
	addChild(spr_user_avatar_[0]);
	spr_role_[0] = Sprite::create();
	spr_role_[0]->setPosition(Vec2(125, 502));
	spr_role_[0]->setVisible(false);
	addChild(spr_role_[0]);
	spr_user_avatar_[2] = Sprite::create();
	spr_user_avatar_[2]->setPosition(Vec2(1280 - 125, 504));
	spr_user_avatar_[2]->setFlippedX(true);
	spr_user_avatar_[2]->setVisible(false);
	addChild(spr_user_avatar_[2]);
	spr_role_[2] = Sprite::create();
	spr_role_[2]->setPosition(Vec2(1280 - 125, 504));
	spr_role_[2]->setFlippedX(true);
	spr_role_[2]->setVisible(false);
	addChild(spr_role_[2]);
	auto table = Sprite::create("ddz/Resources/Image/table.png");
	table->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	addChild(table);
	spr_user_avatar_[1] = Sprite::create();
	spr_user_avatar_[1]->setPosition(Vec2(124, 148));
	spr_user_avatar_[1]->setVisible(false);
	addChild(spr_user_avatar_[1]);
	spr_role_[1] = Sprite::create();
	spr_role_[1]->setPosition(Vec2(124, 148));
	spr_role_[1]->setVisible(false);
	addChild(spr_role_[1]);

	text_lord_score_= Text::create("", "arial", 24);
	text_lord_score_->setPosition(Vec2(660, 758));
	text_lord_score_->setAnchorPoint(Vec2(0.0f, 0.5f));
	text_lord_score_->setVisible(false);
	text_lord_score_->setColor(Color3B(225, 225, 136));
	addChild(text_lord_score_);

	spr_result_back_ = Sprite::create("ddz/Resources/Image/result/result_back.png");
	spr_result_back_->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	spr_result_back_->setVisible(false);
	addChild(spr_result_back_);

	// 时钟
	spr_timer_bg_ = Sprite::create("ddz/Resources/Image/number_timer/timer_bottom.png");
	spr_timer_bg_->setVisible(false);
	addChild(spr_timer_bg_);
	text_timer_number_ = TextBMFont::create("", "ddz/Resources/Image/number_timer/timer_font.fnt");
	text_timer_number_->setVisible(false);
	addChild(text_timer_number_);
	timer_pos_[0] = Vec2(264, 574);
	timer_pos_[1] = Vec2(1280 / 2, 380);
	timer_pos_[2] = Vec2(1280 - 264, 574);

	// 扑克
	myself_card_ = Card::create();
	myself_card_->SetCenterPosition(Vec2(1280 / 2, 120));
	addChild(myself_card_);

	// 发牌位置
	send_cards_pos_[0] = Vec2(220, 416);
	send_cards_pos_[1] = Vec2(1280/2, 120);
	send_cards_pos_[2] = Vec2(1280 - 220, 416);

	// 底牌位置
	bottom_cards_pos_[0] = Vec2(548, 770);
	bottom_cards_pos_[1] = Vec2(548 + 32, 770);
	bottom_cards_pos_[2] = Vec2(548 + 32 + 32, 770);

	for (uint16 i = 0; i < kMaxPlayer; ++i)
	{
		back_card_[i] = BackCard::create();
		back_card_[i]->SetChairID(i);
		addChild(back_card_[i]);
		out_card_[i] = OutCard::create();
		out_card_[i]->SetChairID(i);
		addChild(out_card_[i]);
	}

	back_card_[0]->SetCenterPosition(Vec2(220, 416));
	back_card_[2]->SetCenterPosition(Vec2(1280 - 220, 416));
	back_card_[1]->SetCenterPosition(Vec2(500, 618));

	out_card_[0]->SetPosition(Vec2(268, 574));
	out_card_[1]->SetPosition(Vec2(640, 380));
	out_card_[2]->SetPosition(Vec2(1012, 576));
	out_card_[0]->SetDispatchPos(Vec2(268, 470));
	out_card_[1]->SetDispatchPos(Vec2(640, 380));
	out_card_[2]->SetDispatchPos(Vec2(1012, 472));
	myself_card_->SetOutCardPos(Vec2(640, 380));
	myself_card_->on_mouse_right_ = CC_CALLBACK_0(GameScene::OnCardMouseRight, this);
	myself_card_->On_mouse_left_ = CC_CALLBACK_0(GameScene::OnCardMouseLeft, this);

	// 准备图片
	spr_ready_[0] = Sprite::create("ddz/Resources/Image/ready.png");
	spr_ready_[0]->setPosition(Vec2(264, 574));
	spr_ready_[0]->setVisible(false);
	addChild(spr_ready_[0]);
	spr_ready_[1] = Sprite::create("ddz/Resources/Image/ready.png");
	spr_ready_[1]->setPosition(Vec2(1280 / 2, 380));
	spr_ready_[1]->setVisible(false);
	addChild(spr_ready_[1]);
	spr_ready_[2] = Sprite::create("ddz/Resources/Image/ready.png");
	spr_ready_[2]->setPosition(Vec2(1280 - 264, 574));
	spr_ready_[2]->setVisible(false);
	addChild(spr_ready_[2]);

	// 用户名和分数
	text_user_name_[0] = Text::create("", "arial", 18);
	text_user_name_[0]->setPosition(Vec2(100, 364));
	text_user_name_[0]->setVisible(false);
	addChild(text_user_name_[0]);
	text_user_name_[1] = Text::create("", "arial", 18);
	text_user_name_[1]->setPosition(Vec2(260, 18));
	text_user_name_[1]->setVisible(false);
	addChild(text_user_name_[1]);
	text_user_name_[2] = Text::create("", "arial", 18);
	text_user_name_[2]->setPosition(Vec2(1280 - 100, 366));
	text_user_name_[2]->setVisible(false);
	addChild(text_user_name_[2]);

	text_user_score_[0] = Text::create("", "arial", 20);
	text_user_score_[0]->setPosition(Vec2(100, 340));
	text_user_score_[0]->setColor(Color3B(255, 204, 0));
	text_user_score_[0]->setVisible(false);
	addChild(text_user_score_[0]);
	text_user_score_[1] = Text::create("", "arial", 20);
	text_user_score_[1]->setPosition(Vec2(356, 18));
	text_user_score_[1]->setColor(Color3B(255, 204, 0));
	text_user_score_[1]->setVisible(false);
	addChild(text_user_score_[1]);
	text_user_score_[2] = Text::create("", "arial", 20);
	text_user_score_[2]->setPosition(Vec2(1280 - 100, 342));
	text_user_score_[2]->setColor(Color3B(255, 204, 0));
	text_user_score_[2]->setVisible(false);
	addChild(text_user_score_[2]);

	// 气泡提示
	spr_spreak_[0] = Sprite::create();
	spr_spreak_[0]->setPosition(Vec2(234, 638));
	spr_spreak_[0]->setVisible(false);
	addChild(spr_spreak_[0]);
	spr_spreak_[1] = Sprite::create();
	spr_spreak_[1]->setPosition(Vec2(268, 230));
	spr_spreak_[1]->setVisible(false);
	addChild(spr_spreak_[1]);
	spr_spreak_[2] = Sprite::create();
	spr_spreak_[2]->setPosition(Vec2(1044, 638));
	spr_spreak_[2]->setVisible(false);
	addChild(spr_spreak_[2]);

	// 开始按钮
	btn_start_ = Button::create("ddz/Resources/Image/button/btn_start_normal.png", "ddz/Resources/Image/button/btn_start_pressed.png");
	btn_start_->setPosition(Vec2(1280 / 2, 268.0f));
	btn_start_->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickStart, this));
	btn_start_->setVisible(false);
	addChild(btn_start_);

	// 1分
	btn_call1_ = Button::create("ddz/Resources/Image/button/btn_call1_normal.png", "ddz/Resources/Image/button/btn_call1_pressed.png", "ddz/Resources/Image/button/btn_call1_disabled.png");
	btn_call1_->setPosition(Vec2(1280 / 2 - 210, 268.0f));
	btn_call1_->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickCall1, this));
	btn_call1_->setVisible(false);
	addChild(btn_call1_);

	// 2分
	btn_call2_ = Button::create("ddz/Resources/Image/button/btn_call2_normal.png", "ddz/Resources/Image/button/btn_call2_pressed.png", "ddz/Resources/Image/button/btn_call2_disabled.png");
	btn_call2_->setPosition(Vec2(1280 / 2 - 70, 268.0f));
	btn_call2_->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickCall2, this));
	btn_call2_->setVisible(false);
	addChild(btn_call2_);

	// 3分
	btn_call3_ = Button::create("ddz/Resources/Image/button/btn_call3_normal.png", "ddz/Resources/Image/button/btn_call3_pressed.png");
	btn_call3_->setPosition(Vec2(1280 / 2 + 70, 268.0f));
	btn_call3_->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickCall3, this));
	btn_call3_->setVisible(false);
	addChild(btn_call3_);

	// 不叫
	btn_call0_ = Button::create("ddz/Resources/Image/button/btn_call0_normal.png", "ddz/Resources/Image/button/btn_call0_pressed.png");
	btn_call0_->setPosition(Vec2(1280 / 2 + 210, 268.0f));
	btn_call0_->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickCall0, this));
	btn_call0_->setVisible(false);
	addChild(btn_call0_);

	// 出牌
	btn_outcard_ = Button::create("ddz/Resources/Image/button/btn_outcard_normal.png", "ddz/Resources/Image/button/btn_outcard_pressed.png", "ddz/Resources/Image/button/btn_outcard_disabled.png");
	btn_outcard_->setPosition(Vec2(1280 / 2 - 165, 268.0f));
	btn_outcard_->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickOutCard, this));
	btn_outcard_->setVisible(false);
	addChild(btn_outcard_);

	// 不出
	btn_pass_ = Button::create("ddz/Resources/Image/button/btn_pass_normal.png", "ddz/Resources/Image/button/btn_pass_pressed.png", "ddz/Resources/Image/button/btn_pass_disabled.png");
	btn_pass_->setPosition(Vec2(1280 / 2, 268.0f));
	btn_pass_->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickPass, this));
	btn_pass_->setVisible(false);
	addChild(btn_pass_);

	// 提示
	btn_hint_ = Button::create("ddz/Resources/Image/button/btn_hint_normal.png", "ddz/Resources/Image/button/btn_hint_pressed.png");
	btn_hint_->setPosition(Vec2(1280 / 2 + 165, 268.0f));
	btn_hint_->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickHint, this));
	btn_hint_->setVisible(false);
	addChild(btn_hint_);

	// 要不起
	btn_miss_ = Button::create("ddz/Resources/Image/button/btn_miss_normal.png", "ddz/Resources/Image/button/btn_miss_pressed.png");
	btn_miss_->setPosition(Vec2(1280 / 2, 268.0f));
	btn_miss_->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickMiss, this));
	btn_miss_->setVisible(false);
	addChild(btn_miss_);

	//退出按钮 小孟改坐标
	btBack = Button::create("ddz/Resources/Image/button/toolbar_quit.png", "ddz/Resources/Image/button/toolbar_quit.png");
	btBack->setPosition(Vec2(btBack->getContentSize().width / 2 +1200, visibleSize.height - btBack->getContentSize().height / 2-10));
	btBack->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickBack, this));
	btBack->setVisible(true);
	addChild(btBack);

	spr_miss_ = Sprite::create("ddz/Resources/Image/ShowText.png");
	spr_miss_->setPosition(Vec2(1280 / 2, 39));
	spr_miss_->setAnchorPoint(Vec2(0.5f, 0.0f));
	spr_miss_->setVisible(false);
	addChild(spr_miss_);

	btn_auto_outcard_ = Button::create("ddz/Resources/Image/button/btn_auto_outcard_normal.png", "ddz/Resources/Image/button/btn_auto_outcard_pressed.png");
	btn_auto_outcard_->setPosition(Vec2(1280 - 88, 70.0f));
	btn_auto_outcard_->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickAutoOutCard, this));
	btn_auto_outcard_->setVisible(false);
	addChild(btn_auto_outcard_);

	btn_cancel_auto_outcard_ = Button::create("ddz/Resources/Image/button/btn_cancel_auto_outcard_normal.png", "ddz/Resources/Image/button/btn_cancel_auto_outcard_pressed.png");
	btn_cancel_auto_outcard_->setPosition(Vec2(1280 - 88, 70.0f));
	btn_cancel_auto_outcard_->addClickEventListener(CC_CALLBACK_1(GameScene::OnClickCancelAutoOutCard, this));
	btn_cancel_auto_outcard_->setVisible(false);
	addChild(btn_cancel_auto_outcard_);

	// 动画
	auto animation = Animation::create();
	char temp[256] = { 0 };
	for (int i = 0; i < 19; ++i)
	{
		sprintf(temp, "ddz/Resources/Image/anim/bomb/lord_anim_bomb_frame%02d.png", i + 1);
		animation->addSpriteFrameWithFile(temp);
	}
	animation->setDelayPerUnit(1.0f / 19.0f);

	spr_bomb_ = Sprite::create();
	spr_bomb_->setVisible(false);
	addChild(spr_bomb_);

	ani_bomb_ = Animate::create(animation);
	ani_bomb_->retain();

	animation = Animation::create();
	for (int i = 0; i < 14; ++i)
	{
		sprintf(temp, "ddz/Resources/Image/anim/rocket/yan/lord_anim_rocket_yan%02d.png", i + 1);
		animation->addSpriteFrameWithFile(temp);
	}
	animation->setDelayPerUnit(0.5f / 14.0f);

	spr_rocket_ = Sprite::create("ddz/Resources/Image/anim/rocket/lord_anim_rocket.png");
	spr_rocket_->setAnchorPoint(Vec2(0.5f, 0.0f));
	spr_rocket_->setPosition(Vec2(1280 / 2, 0));
	spr_rocket_->setVisible(false);

	spr_rocket_yan_ = Sprite::create();
	spr_rocket_yan_->setVisible(false);
	spr_rocket_yan_->setAnchorPoint(Vec2(0.5f, 0.0f));
	spr_rocket_yan_->setPosition(Vec2(1280 / 2, 0));
	addChild(spr_rocket_yan_);
	addChild(spr_rocket_);

	ani_rocket_ = Animate::create(animation);
	ani_rocket_->retain();

	animation = Animation::create();
	for (int i = 0; i < 3; ++i)
	{
		sprintf(temp, "ddz/Resources/Image/anim/plane/lord_anim_plane%02d.png", i + 1);
		animation->addSpriteFrameWithFile(temp);
	}
	animation->setDelayPerUnit(1.0f / 14.0f);

	spr_plane_ = Sprite::create();
	spr_plane_->setVisible(false);
	addChild(spr_plane_);

	ani_plane_ = Animate::create(animation);
	ani_plane_->retain();

	animation = Animation::create();
	for (int i = 0; i < 14; ++i)
	{
		sprintf(temp, "ddz/Resources/Image/anim/chainpairs/lord_anim_chainpairs%02d.png", i + 1);
		animation->addSpriteFrameWithFile(temp);
	}
	animation->setDelayPerUnit(1.0f / 14.0f);

	spr_chainpairs_ = Sprite::create();
	spr_chainpairs_->setVisible(false);
	addChild(spr_chainpairs_);

	ani_chainpairs_ = Animate::create(animation);
	ani_chainpairs_->retain();

	animation = Animation::create();
	for (int i = 0; i < 14; ++i)
	{
		sprintf(temp, "ddz/Resources/Image/anim/straight/lord_anim_straight%02d.png", i + 1);
		animation->addSpriteFrameWithFile(temp);
	}
	animation->setDelayPerUnit(1.0f / 14.0f);

	spr_straight_ = Sprite::create();
	spr_straight_->setVisible(false);
	addChild(spr_straight_);

	ani_straight_ = Animate::create(animation);
	ani_straight_->retain();

	animation = Animation::create();
	for (int i = 0; i < 17; ++i)
	{
		sprintf(temp, "ddz/Resources/Image/anim/chuntian/chuntian_%02d.png", i + 1);
		animation->addSpriteFrameWithFile(temp);
	}
	animation->setDelayPerUnit(1.0f / 17.0f);

	spr_chuntian_ = Sprite::create();
	spr_chuntian_->setVisible(false);
	spr_chuntian_->setPosition(Vec2(1280 / 2, 800 / 2));
	addChild(spr_chuntian_);

	ani_chuntian_ = Animate::create(animation);
	ani_chuntian_->retain();

	animation = Animation::create();
	for (int i = 0; i < 2; ++i)
	{
		sprintf(temp, "ddz/Resources/Image/anim/alarm/lord_anim_remain_alarm_%02d.png", i + 1);
		animation->addSpriteFrameWithFile(temp);
	}
	animation->setDelayPerUnit(1.0f / 2.0f);
	for (uint8 i = 0; i < kMaxPlayer; ++i)
	{
		spr_alarm_[i] = Sprite::create();
		spr_alarm_[i]->setVisible(false);
		addChild(spr_alarm_[i]);
	}
	spr_alarm_[0]->setPosition(Vec2(195, 405));
	spr_alarm_[2]->setPosition(Vec2(1280 - 195, 405));

	ani_alarm_= Animate::create(animation);
	ani_alarm_->retain();

	animation = Animation::create();
	for (int i = 0; i < 10; ++i)
	{
		sprintf(temp, "ddz/Resources/Image/anim/changetolordorfarmer/lord_anim_changetolordorfarmer%02d.png", i + 1);
		animation->addSpriteFrameWithFile(temp);
	}
	animation->setDelayPerUnit(1.0f / 20.0f);
	for (uint8 i = 0; i < kMaxPlayer; ++i)
	{
		spr_switch_role_[i] = Sprite::create();
		spr_switch_role_[i]->setVisible(false);
		spr_switch_role_[i]->setPosition(spr_role_[i]->getPosition());
		addChild(spr_switch_role_[i]);
		ani_switch_role_[i] = Animate::create(animation);
		ani_switch_role_[i]->retain();
	}

	// 游戏结果
	for (uint8 i = 0; i < kMaxPlayer; ++i)
	{
		text_result_score_win_[i] = TextBMFont::create("", "ddz/Resources/Image/result/lord_result_getscore_num_win.fnt");
		text_result_score_win_[i]->setVisible(false);
		addChild(text_result_score_win_[i]);

		text_result_score_loss_[i] = TextBMFont::create("", "ddz/Resources/Image/result/lord_result_getscore_num_loss.fnt");
		text_result_score_loss_[i]->setVisible(false);
		addChild(text_result_score_loss_[i]);
	}
	text_result_score_win_[0]->setAnchorPoint(Vec2(0.f, 0.f));
	text_result_score_win_[0]->setPosition(Vec2(216, 655));
	text_result_score_win_[1]->setAnchorPoint(Vec2(0.f, 0.f));
	text_result_score_win_[1]->setPosition(Vec2(216, 255));
	text_result_score_win_[2]->setAnchorPoint(Vec2(1.f, 0.f));
	text_result_score_win_[2]->setPosition(Vec2(1064, 655));

	text_result_score_loss_[0]->setAnchorPoint(Vec2(0.f, 0.f));
	text_result_score_loss_[0]->setPosition(Vec2(216, 655));
	text_result_score_loss_[1]->setAnchorPoint(Vec2(0.f, 0.f));
	text_result_score_loss_[1]->setPosition(Vec2(216, 255));
	text_result_score_loss_[2]->setAnchorPoint(Vec2(1.f, 0.f));
	text_result_score_loss_[2]->setPosition(Vec2(1064, 655));

	text_result_lord_score_= Text::create("", "arial", 24);
	text_result_lord_score_->setPosition(Vec2(406, 85));
	text_result_lord_score_->setAnchorPoint(Vec2(0.0f, 0.5f));
	text_result_lord_score_->setVisible(false);
	text_result_lord_score_->setColor(Color3B(153, 153, 153));
	addChild(text_result_lord_score_);

	text_result_bomb_= Text::create("", "arial", 24);
	text_result_bomb_->setPosition(Vec2(510, 85));
	text_result_bomb_->setAnchorPoint(Vec2(0.0f, 0.5f));
	text_result_bomb_->setVisible(false);
	text_result_bomb_->setColor(Color3B(153, 153, 153));
	addChild(text_result_bomb_);

	text_result_chuntian_= Text::create("", "arial", 24);
	text_result_chuntian_->setPosition(Vec2(625, 85));
	text_result_chuntian_->setAnchorPoint(Vec2(0.0f, 0.5f));
	text_result_chuntian_->setVisible(false);
	text_result_chuntian_->setColor(Color3B(153, 153, 153));
	addChild(text_result_chuntian_);

	text_result_times_= Text::create("", "arial", 24);
	text_result_times_->setPosition(Vec2(734, 85));
	text_result_times_->setAnchorPoint(Vec2(0.0f, 0.5f));
	text_result_times_->setVisible(false);
	text_result_times_->setColor(Color3B(153, 153, 153));
	addChild(text_result_times_);

	client_kernel_ = GameKernel::get();
	assert(client_kernel_);
	client_kernel_->SetDelegate(this);
    
    return true;
}

void GameScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
}
void GameScene::OnClickBack(Ref* sender)
{
	client_kernel_->Intermit();
}
void GameScene::onExit()
{
	Layer::onExit();
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	//if (!glview)
	//	return;
	//glview->setFrameSize(1280, 800);
	//glview->setDesignResolutionSize(1280, 720, ResolutionPolicy::EXACT_FIT);
}

void GameScene::ResetGameClient()
{
}

void GameScene::CloseGameClient()
{
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	//if (!glview)
	//	return;
	//glview->setFrameSize(1280, 800);
	glview->setDesignResolutionSize(1280, 720, ResolutionPolicy::EXACT_FIT);
	//Director::getInstance()->end();
	Director::getInstance()->popScene();
}

bool GameScene::OnEventGameMessage(WORD sub_cmdid, void* data, WORD data_size)
{
	switch (sub_cmdid)
	{
	case SUB_S_GAME_START:		// 游戏开始
		{
			return OnSubGameStart(data, data_size);
		}
	case SUB_S_CALL_SCORE:		// 用户叫分
		{
			return OnSubCallScore(data, data_size);
		}
	case SUB_S_LORD_INFO:		// 庄家信息
		{
			return OnSubLordInfo(data, data_size);
		}
	case SUB_S_OUT_CARD:		// 用户出牌
		{
			return OnSubOutCard(data, data_size);
		}
	case SUB_S_PASS_CARD:		// 用户放弃
		{
			return OnSubPassCard(data, data_size);
		}
	case SUB_S_GAME_CONCLUDE:	// 游戏结束
		{
			return OnSubGameConclude(data, data_size);
		}
	}

	return false;
}

bool GameScene::OnEventSceneMessage(uint8 game_status, bool lookon_user, void* data, WORD data_size)
{
	CCLOG("OnEventSceneMessage");
	switch (game_status)
	{
	case GAME_SCENE_FREE:	// 空闲状态
		{
			// 效验数据
			assert(data_size == sizeof(CMD_S_StatusFree));
			if (data_size != sizeof(CMD_S_StatusFree))
				return false;

			CMD_S_StatusFree* status_free = (CMD_S_StatusFree*)data;

			// 玩家设置
			if (!client_kernel_->IsLookonMode())
			{
				if (client_kernel_->GetMeUserItem()->GetUserStatus() != US_READY)
				{
					btn_start_->setVisible(true);
					//ShowGameTimer(1, kTimerStart, kTimerStartTime);
				}
				btn_auto_outcard_->setVisible(true);
			}

			for (uint16 i = 0; i < kMaxPlayer; ++i)
			{
				IGameUserItem* user_item = client_kernel_->GetTableUserItem(i);
				if (user_item)
				{
					uint16 view_id = ViewChairID(i);
					if (user_item->GetUserStatus() == US_READY)
						spr_ready_[view_id]->setVisible(true);

					char temp[64];
					sprintf(temp, "avatar/%s%d.png", user_item->GetGender() == GENDER_MALE ? "boy" : "girl", user_item->GetMemberOrder());
					spr_user_avatar_[view_id]->setTexture(temp);
					spr_user_avatar_[view_id]->setVisible(true);
					text_user_name_[view_id]->setString(user_item->GetNickName());
					text_user_name_[view_id]->setVisible(true);
					sprintf(temp, LLSTRING, user_item->GetUserScore());
					text_user_score_[view_id]->setString(temp);
					text_user_score_[view_id]->setVisible(true);
				}
			}

			return true;
		}
	case GAME_SCENE_CALL:	//叫分状态
		{
			// 效验数据
			assert(data_size == sizeof(CMD_S_StatusCall));
			if (data_size != sizeof(CMD_S_StatusCall))
				return false;

			for (uint16 i = 0; i < kMaxPlayer; ++i)
			{
				IGameUserItem* user_item = client_kernel_->GetTableUserItem(i);
				if (user_item)
				{
					uint16 view_id = ViewChairID(i);

					char temp[64];
					sprintf(temp, "avatar/%s%d.png", user_item->GetGender() == GENDER_MALE ? "boy" : "girl", user_item->GetMemberOrder());
					spr_user_avatar_[view_id]->setTexture(temp);
					spr_user_avatar_[view_id]->setVisible(true);
					text_user_name_[view_id]->setString(user_item->GetNickName());
					text_user_name_[view_id]->setVisible(true);
					sprintf(temp, LLSTRING, user_item->GetUserScore());
					text_user_score_[view_id]->setString(temp);
					text_user_score_[view_id]->setVisible(true);
				}
			}

			CMD_S_StatusCall* status_call = (CMD_S_StatusCall*)data;

			// 设置变量
			current_user_ = status_call->current_user;

			// 扑克数据
			memcpy(hand_card_data_, status_call->hand_card_data, sizeof(hand_card_data_));
			hand_card_count_ = 0;

			// 叫分状态
			Sprite* bottom_card = Sprite::create("ddz/Resources/Image/cards/card_00.png");
			float card_width = bottom_card->getContentSize().width;
			bottom_card->setPosition(Vec2(1280 / 2 - card_width, 618));
			bottom_card->setName("bottom_card_bg_1");
			addChild(bottom_card);
			bottom_card = Sprite::create("ddz/Resources/Image/cards/card_00.png");
			bottom_card->setName("bottom_card_bg_2");
			bottom_card->setPosition(Vec2(1280 / 2, 618));
			addChild(bottom_card);
			bottom_card = Sprite::create("ddz/Resources/Image/cards/card_00.png");
			bottom_card->setName("bottom_card_bg_3");
			bottom_card->setPosition(Vec2(1280 / 2 + card_width, 618));
			addChild(bottom_card);

			for (uint16 i = 0; i < GAME_PLAYER; i++)
			{
				uint16 view_id = ViewChairID(i);
				if (status_call->callscore_info[i] != 0)
				{
					uint8 callscore = status_call->callscore_info[i] == 0xFF ? 0 : status_call->callscore_info[i];
					SetSpeakContent(view_id, SpeakContent(callscore));
				}
				
				if (view_id == 1)
				{
					GameLogic::SortCardList(hand_card_data_, hand_card_count_);
					myself_card_->SetCardData(hand_card_data_, hand_card_count_);
				}
				else
				{
					back_card_[view_id]->SetCardCount(17);
				}
			}

			// 叫分设置
			if (!client_kernel_->IsLookonMode() && current_user_ == client_kernel_->GetMeChairID())
			{
				btn_call0_->setVisible(true);
				btn_call1_->setVisible(true);
				btn_call2_->setVisible(true);
				btn_call3_->setVisible(true);

				btn_call1_->setBright(status_call->lord_callscore < 1 ? true : false);
				btn_call2_->setBright(status_call->lord_callscore < 2 ? true : false);
				btn_call3_->setBright(status_call->lord_callscore < 3 ? true : false);
				btn_call1_->setEnabled(status_call->lord_callscore < 1 ? true : false);
				btn_call2_->setEnabled(status_call->lord_callscore < 2 ? true : false);
				btn_call3_->setEnabled(status_call->lord_callscore < 3 ? true : false);
			}
			ShowGameTimer(ViewChairID(current_user_), kTimerCallScore, kTimerCallScoreTime);

			// 玩家控制
			if (!client_kernel_->IsLookonMode())	
			{
				btn_auto_outcard_->setVisible(true);
			}

			// 旁观设置界面
			if (client_kernel_->IsLookonMode())
			{
			}

			return true;
		}
	case GAME_SCENE_PLAY:	// 游戏状态
		{
			// 效验数据
			assert(data_size == sizeof(CMD_S_StatusPlay));
			if (data_size != sizeof(CMD_S_StatusPlay))
				return false;

			CMD_S_StatusPlay* status_play = (CMD_S_StatusPlay*)data;

			// 设置变量
			lord_user_ = status_play->lord_user;
			text_lord_score_->setString(a2u(StringUtils::format("底分：%d", status_play->lord_callscore)));
			text_lord_score_->setVisible(true);
			current_user_ = status_play->current_user;

			for (uint16 i = 0; i < kMaxPlayer; ++i)
			{
				IGameUserItem* user_item = client_kernel_->GetTableUserItem(i);
				if (user_item)
				{
					uint16 view_id = ViewChairID(i);
					spr_user_avatar_[view_id]->setVisible(false);
					char temp[64];
					text_user_name_[view_id]->setString(user_item->GetNickName());
					text_user_name_[view_id]->setVisible(true);
					sprintf(temp, LLSTRING, user_item->GetUserScore());
					text_user_score_[view_id]->setString(temp);
					text_user_score_[view_id]->setVisible(true);
					spr_role_[view_id]->setTexture(lord_user_ == i ? "ddz/Resources/Image/role_lord.png" : "ddz/Resources/Image/role_farmer.png");
					spr_role_[view_id]->setVisible(true);
				}
			}

			// 出牌变量
			turn_outcard_count_ = status_play->turn_card_count;
			memcpy(turn_ountcard_data_, status_play->turn_card_data, sizeof(turn_ountcard_data_));

			// 扑克数据
			for (uint16 i = 0; i < kMaxPlayer; ++i)
			{
				uint16 view_id = ViewChairID(i);
				if (view_id != 1)
				{
					back_card_[view_id]->SetCardCount(status_play->hand_card_count[i]);
				}
				else
				{
					hand_card_count_ = status_play->hand_card_count[i];
				}
			}
			memcpy(hand_card_data_, status_play->hand_card_data, sizeof(hand_card_data_));
			GameLogic::SortCardList(hand_card_data_, hand_card_count_);
			myself_card_->SetCardData(hand_card_data_, hand_card_count_);
			memcpy(bottom_card_data_, status_play->bottom_card, sizeof(bottom_card_data_));

			char temp[128] = { 0 };
			for (int i = 0; i < 3; ++i)
			{
				char temp2[128] = { 0 };
				if (bottom_card_data_[i] == 0x4E || bottom_card_data_[i] == 0x4F)
				{
					sprintf(temp2, "ddz/Resources/Image/bottomcards/bottomcard_%02d.png", bottom_card_data_[i] == 0x4E ? 53 : 54);
				}
				else
				{
					sprintf(temp2, "ddz/Resources/Image/bottomcards/bottomcard_%02d.png", (bottom_card_data_[i] & MASK_VALUE) + ((bottom_card_data_[i] & MASK_COLOR) >> 4) * 13);
				}
				Sprite* scard = Sprite::create(temp2);
				scard->setPosition(bottom_cards_pos_[i]);
				sprintf(temp2, "bottom_card_small_%d", i + 1);
				scard->setName(temp2);
				addChild(scard);
			}

			// 出牌界面
			if (status_play->turn_winner != kuint16max)
			{
				out_card_[ViewChairID(status_play->turn_winner)]->SetCardData(turn_ountcard_data_, turn_outcard_count_, false);
			}

			// 玩家控制
			if (!client_kernel_->IsLookonMode())	
			{
				btn_auto_outcard_->setVisible(true);
				myself_card_->SetTouchEnable(true);
				if (current_user_ == client_kernel_->GetMeChairID())
				{
					btn_outcard_->setVisible(true);
					btn_pass_->setVisible(true);
					btn_hint_->setVisible(true);

					bool outcard = VerdictOutCard();
					btn_outcard_->setBright(outcard);
					btn_pass_->setBright(turn_outcard_count_ > 0);
					btn_outcard_->setBright(outcard);
					btn_pass_->setEnabled(turn_outcard_count_ > 0);

					if (status_play->turn_winner == current_user_)
					{
						GameLogic::SearchOutCard(hand_card_data_, hand_card_count_, nullptr, 0, &search_card_result_);
					}
					else
					{
						GameLogic::SearchOutCard(hand_card_data_, hand_card_count_, turn_ountcard_data_, turn_outcard_count_, &search_card_result_);
					}
				}
			}

			// 设置时间
			ShowGameTimer(ViewChairID(current_user_), kTimerOutCard, kTimerOutCardTime);

			// 旁观设置界面
			if (client_kernel_->IsLookonMode())
			{
			}

			return true;
		}
	}

	return true;
}

void GameScene::OnEventUserEnter(IGameUserItem* game_user_item, bool lookon_user)
{
	if (!lookon_user)
	{
		uint16 view_id = ViewChairID(game_user_item->GetChairID());
		char temp[64];
		sprintf(temp, "avatar/%s%d.png", game_user_item->GetGender() == GENDER_MALE ? "boy" : "girl", game_user_item->GetMemberOrder());
		spr_user_avatar_[view_id]->setTexture(temp);
		spr_user_avatar_[view_id]->setVisible(true);
		spr_role_[view_id]->setVisible(false);
		text_user_name_[view_id]->setString(game_user_item->GetNickName());
		text_user_name_[view_id]->setVisible(true);
		sprintf(temp, LLSTRING, game_user_item->GetUserScore());
		text_user_score_[view_id]->setString(temp);
		text_user_score_[view_id]->setVisible(true);
	}
}

void GameScene::OnEventUserLeave(IGameUserItem* game_user_item, bool lookon_user)
{
	if (!lookon_user)
	{
		uint16 view_id = ViewChairID(game_user_item->GetChairID());
		if (game_user_item->GetUserStatus() == US_READY)
			spr_ready_[view_id]->setVisible(false);
		spr_user_avatar_[view_id]->setVisible(false);
		text_user_name_[view_id]->setVisible(false);
		text_user_score_[view_id]->setVisible(false);
	}
}

void GameScene::OnEventUserScore(IGameUserItem* game_user_item, bool lookon_user)
{
}

void GameScene::OnEventUserStatus(IGameUserItem* game_user_item, bool lookon_user)
{
	if (!lookon_user)
	{
		uint16 view_id = ViewChairID(game_user_item->GetChairID());
		if (game_user_item->GetUserStatus() == US_READY)
			spr_ready_[view_id]->setVisible(true);
	}
}

void GameScene::OnSystemMessage(const char* sys_msg, WORD msg_len, WORD type)
{
}

bool GameScene::SendSocketData(WORD sub_cmdid)
{
	if (client_kernel_->IsLookonMode())
		return false;
	return client_kernel_->SendSocketData(MDM_GF_GAME, sub_cmdid);
}

bool GameScene::SendSocketData(WORD sub_cmdid, void* data, WORD data_size)
{
	if (client_kernel_->IsLookonMode())
		return false;
	return client_kernel_->SendSocketData(MDM_GF_GAME, sub_cmdid, data, data_size);
}

// 游戏开始
bool GameScene::OnSubGameStart(void* data, WORD data_size)
{
	// 效验参数
	assert(data_size == sizeof(CMD_S_GameStart));
	if (data_size != sizeof(CMD_S_GameStart))
		return false;

	CMD_S_GameStart* game_start = (CMD_S_GameStart*)data;

	// 设置状态
	client_kernel_->SetGameStatus(GAME_SCENE_CALL);

	for (uint8 i = 0; i < kMaxPlayer; ++i)
	{
		spr_ready_[i]->setVisible(false);
		spr_user_avatar_[i]->setVisible(true);
		spr_role_[i]->setVisible(false);
	}
	current_user_ = game_start->current_user;

	turn_outcard_count_ = 0;
	memset(turn_ountcard_data_, 0, sizeof(turn_ountcard_data_));

	// 设置扑克
	uint16 me_chair_id = client_kernel_->GetMeChairID();
	memcpy(hand_card_data_, game_start->card_data, sizeof(uint8) * arraysize(game_start->card_data));
	hand_card_count_ = 17;

	// 发牌动画
	SendCards(ViewChairID(game_start->start_user));

	return true;
}

// 用户叫分
bool GameScene::OnSubCallScore(void* data, WORD data_size)
{
	// 效验参数
	assert(data_size == sizeof(CMD_S_CallScore));
	if (data_size != sizeof(CMD_S_CallScore))
		return false;

	HideGameTimer();

	CMD_S_CallScore* call_score = (CMD_S_CallScore*)data;
	current_user_ = call_score->current_user;
	if (call_score->last_score == 0xFF)
		call_score->last_score = 0;

	SetSpeakContent(ViewChairID(call_score->callscore_user), SpeakContent(call_score->last_score));
	if (call_score->callscore_user != client_kernel_->GetMeChairID())
	{
		IGameUserItem* user_item = client_kernel_->GetTableUserItem(call_score->callscore_user);
		if (user_item)
		{
			char temp[256] = { 0 };
			sprintf(temp, "ddz/Resources/Sound/%s/CallScore_%d.mp3", user_item->GetGender() == GENDER_MALE ? "Male" : "Female", call_score->last_score);
			SimpleAudioEngine::getInstance()->playEffect(temp);
		}
	}

	if (!client_kernel_->IsLookonMode() && call_score->current_user == client_kernel_->GetMeChairID())
	{
		btn_call0_->setVisible(true);
		btn_call1_->setVisible(true);
		btn_call2_->setVisible(true);
		btn_call3_->setVisible(true);

		btn_call1_->setBright(call_score->current_score < 1 ? true : false);
		btn_call2_->setBright(call_score->current_score < 2 ? true : false);
		btn_call3_->setBright(call_score->current_score < 3 ? true : false);
		btn_call1_->setEnabled(call_score->current_score < 1 ? true : false);
		btn_call2_->setEnabled(call_score->current_score < 2 ? true : false);
		btn_call3_->setEnabled(call_score->current_score < 3 ? true : false);
	}

	if (call_score->current_user != kuint16max)
	{
		ShowGameTimer(ViewChairID(call_score->current_user), kTimerCallScore, kTimerCallScoreTime);
	}

	return true;
}

// 地主信息
bool GameScene::OnSubLordInfo(void* data, WORD data_size)
{
	// 效验参数
	assert(data_size == sizeof(CMD_S_LordInfo));
	if (data_size != sizeof(CMD_S_LordInfo))
		return false;

	HideGameTimer();
	SetSpeakContent(kuint16max, SPEAK_COUNT);

	CMD_S_LordInfo* lord_info = (CMD_S_LordInfo*)data;

	// 设置状态
	client_kernel_->SetGameStatus(GAME_SCENE_PLAY);

	lord_user_ = lord_info->lord_user;
	current_user_ = lord_info->current_user;
	PlaySwitchRoleAnimation(ViewChairID(lord_user_));
	lord_score_ = lord_info->lord_score;
	text_lord_score_->setString(a2u(StringUtils::format("底分：%d", lord_info->lord_score)));
	text_lord_score_->setVisible(true);

	memcpy(bottom_card_data_, lord_info->bottom_card_data, sizeof(bottom_card_data_));
	ShowBottomCards();

	return true;
}

// 用户出牌
bool GameScene::OnSubOutCard(void* data, WORD data_size)
{
	CMD_S_OutCard* outcard = (CMD_S_OutCard*)data;
	uint16 head_size = sizeof(CMD_S_OutCard) - sizeof(outcard->card_data);

	// 效验数据
	assert((data_size >= head_size) && (data_size == (head_size + outcard->card_count * sizeof(uint8))));
	if ((data_size < head_size) || (data_size != (head_size+ outcard->card_count * sizeof(uint8))))
		return false;

	HideGameTimer();
	bottom_cards_ani_finish_ = true;

	out_card_[ViewChairID(outcard->outcard_user + 1) % kMaxPlayer]->SetCardData(nullptr, 0, false);
	SetSpeakContent(ViewChairID((outcard->outcard_user + 1) % kMaxPlayer), SPEAK_COUNT);

	uint16 me_chair_id = client_kernel_->GetMeChairID();

	uint8 turn_card_type = GameLogic::GetCardType(outcard->card_data, outcard->card_count);

	// 出牌动作
	if (outcard->outcard_user != me_chair_id || client_kernel_->IsLookonMode())
	{
		// 显示出牌
		out_card_[ViewChairID(outcard->outcard_user)]->SetCardData(outcard->card_data, outcard->card_count);

		// 删除扑克
		uint16 view_outcard_user = ViewChairID(outcard->outcard_user);
		back_card_[view_outcard_user]->SetCardCount(back_card_[view_outcard_user]->GetCardCount() - outcard->card_count);

		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/DisCard.wav");

		// 播放动画
		if (turn_card_type == CT_BOMB_CARD)
		{
			PlayBombAnimation(out_card_[ViewChairID(outcard->outcard_user)]->GetCenterPos());
			SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Bomb.wav");
		}
		else if (turn_card_type == CT_MISSILE_CARD)
		{
			PlayRocketAnimation();
			SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Rocket.wav");
		}
		else if (turn_card_type == CT_DOUBLE_LINE)
		{
			PlayChainPairsAnimation(out_card_[ViewChairID(outcard->outcard_user)]->GetCenterPos());
		}
		else if (turn_card_type == CT_SINGLE_LINE)
		{
			PlayStraightAnimation(out_card_[ViewChairID(outcard->outcard_user)]->GetCenterPos());
		}
		else if (turn_card_type == CT_THREE_LINE || (turn_card_type == CT_THREE_TAKE_ONE && outcard->card_count >= 8) || (turn_card_type == CT_THREE_TAKE_TWO && outcard->card_count >= 10))
		{
			PlayPlaneAnimation();
			SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/plane.ogg");
		}
		PlayGameSound(outcard->outcard_user, turn_card_type, outcard->card_data, outcard->card_count);
	}

	// 出牌变量
	current_user_ = outcard->current_user;
	turn_outcard_count_ = outcard->card_count;
	memcpy(turn_ountcard_data_, outcard->card_data, outcard->card_count * sizeof(uint8));

	// 报牌警告
	if (back_card_[ViewChairID(outcard->outcard_user)]->GetCardCount() <= 2 && back_card_[ViewChairID(outcard->outcard_user)]->GetCardCount() > 0)
	{
		PlayAlarmAnimation(ViewChairID(outcard->outcard_user));
	}

	if (current_user_ != kuint16max)
	{
		if (!client_kernel_->IsLookonMode() && current_user_ == client_kernel_->GetMeChairID())
		{
			GameLogic::SearchOutCard(hand_card_data_, hand_card_count_, turn_ountcard_data_, turn_outcard_count_, &search_card_result_);
			if (search_card_result_.search_count == 0)
			{
				btn_miss_->setVisible(true);
				spr_miss_->setVisible(true);
			}
			else
			{
				btn_outcard_->setVisible(true);
				btn_pass_->setVisible(true);
				btn_hint_->setVisible(true);
				bool outcard = VerdictOutCard();
				btn_pass_->setBright(turn_outcard_count_ > 0 ? true : false);
				btn_outcard_->setBright(outcard);
				btn_pass_->setEnabled(turn_outcard_count_ > 0 ? true : false);
				btn_outcard_->setEnabled(outcard);
			}
		}
		ShowGameTimer(ViewChairID(current_user_), kTimerOutCard, kTimerOutCardTime);
	}

	return true;
}

// 用户放弃
bool GameScene::OnSubPassCard(void* data, WORD data_size)
{
	// 效验数据
	assert(data_size == sizeof(CMD_S_PassCard));
	if (data_size != sizeof(CMD_S_PassCard))
		return false;

	CMD_S_PassCard* pass_card = (CMD_S_PassCard*)data;

	current_user_ = pass_card->current_user;

	if (client_kernel_->IsLookonMode() ||  pass_card->pass_user != client_kernel_->GetMeChairID())
	{
		HideGameTimer();
		SetSpeakContent(ViewChairID(pass_card->pass_user), SPEAK_PASS);
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Pass.wav");
	}

	if (current_user_ != kuint16max)
	{
		out_card_[ViewChairID(current_user_)]->SetCardData(nullptr, 0, false);
	}

	// 一轮判断
	if (pass_card->turn_over)
	{
		turn_outcard_count_=0;
		memset(turn_ountcard_data_, 0, sizeof(turn_ountcard_data_));
		SetSpeakContent(kuint16max, SPEAK_COUNT);
		for (uint8 i = 0; i < kMaxPlayer; ++i)
		{
			out_card_[ViewChairID(i)]->SetCardData(nullptr, 0, false);
		}
	}

	if (!client_kernel_->IsLookonMode() && current_user_ == client_kernel_->GetMeChairID())
	{
		GameLogic::SearchOutCard(hand_card_data_, hand_card_count_, turn_ountcard_data_, turn_outcard_count_, &search_card_result_);
		if (search_card_result_.search_count == 0)
		{
			btn_miss_->setVisible(true);
			spr_miss_->setVisible(true);
		}
		else
		{
			btn_outcard_->setVisible(true);
			btn_pass_->setVisible(true);
			btn_hint_->setVisible(true);

			btn_pass_->setBright(turn_outcard_count_ > 0 ? true : false);
			btn_pass_->setEnabled(turn_outcard_count_ > 0 ? true : false);
			bool outcard = VerdictOutCard();
			btn_outcard_->setBright(outcard);
			btn_outcard_->setEnabled(outcard);
		}
	}

	if (current_user_ != kuint16max)
	{
		ShowGameTimer(ViewChairID(current_user_), kTimerOutCard, kTimerOutCardTime);
	}

	return true;
}

// 游戏结束
bool GameScene::OnSubGameConclude(void* data, WORD data_size)
{
	// 效验数据
	assert(data_size == sizeof(CMD_S_GameConclude));
	if (data_size != sizeof(CMD_S_GameConclude))
		return false;

	CMD_S_GameConclude* game_conclude = (CMD_S_GameConclude*)data;
	memcpy(&game_conclude_, game_conclude, sizeof(CMD_S_GameConclude));

	// 设置状态
	client_kernel_->SetGameStatus(GAME_SCENE_FREE);

	// 删除时间
	HideGameTimer();
	current_user_ = kuint16max;

	btn_outcard_->setVisible(false);
	btn_pass_->setVisible(false);
	btn_hint_->setVisible(false);
	btn_call0_->setVisible(false);
	btn_call1_->setVisible(false);
	btn_call2_->setVisible(false);
	btn_call3_->setVisible(false);

	// 显示成绩
	spr_result_back_->setVisible(true);
	char str[128] = { 0 };
	TextBMFont* text_result_score = nullptr;
	for (uint16 i = 0; i < kMaxPlayer; ++i)
	{
		uint16 view_id = ViewChairID(i);
		if (game_conclude->game_score[i] >= 0)
		{
			text_result_score = text_result_score_win_[view_id];
		}
		else
		{
			text_result_score = text_result_score_loss_[view_id];
		}
		text_result_score->setVisible(true);
		if (game_conclude->game_score[i] < 100 && game_conclude->game_score[i] > -100)
		{
			char temp[128] = { 0 };
			sprintf(temp, LLSTRING, game_conclude->game_score[i] >= 0 ? "+" : "", game_conclude->game_score[i]);
			text_result_score->setString(temp);
		}
		else
		{
			sprintf(str, "result_score%d", i);
			text_result_score->setString("0");
			schedule([=](float dt)
			{
				int score = atoi(text_result_score->getString().c_str());
				score += game_conclude_.game_score[i] / 10;
				if (game_conclude_.game_score[i] >= 0 && score > game_conclude_.game_score[i])
					score = game_conclude_.game_score[i];
				else if (game_conclude_.game_score[i] < 0 && score < game_conclude_.game_score[i])
					score = game_conclude_.game_score[i];
				char temp[128] = { 0 };
				if (game_conclude_.game_score[i] >= 0)
				{
					sprintf(temp, "+%d", score);
				}
				else
				{
					sprintf(temp, "%d", score);
				}
				text_result_score->setString(temp);
			}, 0.1f, 9, 0.0f, str);
		}
	}

	// 显示扑克
	uint8 card_index = 0;
	for (uint16 i = 0; i < kMaxPlayer; ++i)
	{
		uint16 view_id = ViewChairID(i);
		if (view_id == 1)
		{
			myself_card_->SetCardData(nullptr, 0);
		}
		else
		{
			back_card_[view_id]->SetCardCount(0);
		}
		if (game_conclude->game_score[i] < 10 && game_conclude->game_score[i] > -10)
		{
			out_card_[view_id]->SetCardData(&game_conclude->card_data[card_index], game_conclude->card_count[i], false);
		}
		else
		{
			sprintf(str, "result_card%d", i);
			scheduleOnce([=](float)
			{
				out_card_[view_id]->SetCardData(&game_conclude_.card_data[card_index], game_conclude_.card_count[i], false);
			}, 1.0f, str);
		}
		card_index += game_conclude->card_count[i];
	}

	// 倍数相关
	text_result_lord_score_->setString(a2u(StringUtils::format("底分：%d", lord_score_)));
	text_result_lord_score_->setVisible(true);
	if (game_conclude->bomb_count > 0)
	{
		text_result_bomb_->setString(a2u(StringUtils::format("炸弹：%d", game_conclude->bomb_count)));
	}
	else
	{
		text_result_bomb_->setString(a2u("炸弹：无"));
	}
	text_result_bomb_->setVisible(true);
	text_result_chuntian_->setString(a2u(StringUtils::format("春天：%s", (game_conclude->chuntian || game_conclude->fan_chuntian) ? "有" : "无")));
	text_result_chuntian_->setVisible(true);
	text_result_times_->setString(a2u(StringUtils::format("总倍数：%d", (game_conclude->bomb_count + 1) * lord_score_ * ((game_conclude->chuntian || game_conclude->fan_chuntian) ? 2 : 1))));
	text_result_times_->setVisible(true);

	SetSpeakContent(kuint16max, SPEAK_COUNT);

	// 播放声音
	if (game_conclude->game_score[client_kernel_->GetMeChairID()] >= 0)
	{
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Victory.wav");
	}
	else
	{
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Loss.wav");
	}

	if (!client_kernel_->IsLookonMode())
	{
		// 取消托管
		if (auto_out_card_)
			OnClickCancelAutoOutCard(nullptr);

		btn_start_->setVisible(true);
		ShowGameTimer(1, kTimerStart, kTimerStartTime);
	}

	return true;
}

uint16 GameScene::ViewChairID(uint16 chair_id)
{
	if (chair_id == INVALID_CHAIR)
		return INVALID_CHAIR;

	// 转换椅子
	uint16 chair_count = client_kernel_->GetGameAttribute().chair_count;
	uint16 view_id = (chair_id + chair_count * 3 / 2 - client_kernel_->GetUserAttribute().chair_id) % chair_count;

	return view_id;
}

// 显示计时器
void GameScene::ShowGameTimer(uint16 chair_id, uint32 timer_id, int sec)
{
	assert(chair_id <  kMaxPlayer);
	assert(sec < 100 && sec > 0);
	if (sec >= 100 || sec <= 0)
		return;

	timer_id_ = timer_id;
	timer_ = sec;

	spr_timer_bg_->setPosition(timer_pos_[chair_id]);
	text_timer_number_->setPosition(timer_pos_[chair_id]);
	spr_timer_bg_->setVisible(true);
	text_timer_number_->setVisible(true);
	char temp[128] = { 0 };
	sprintf(temp, "%d", timer_);
	text_timer_number_->setString(temp);
	schedule(schedule_selector(GameScene::OnGameTimer), 1.0f, timer_ - 1, 1.0f);
}

// 隐藏计时器
void GameScene::HideGameTimer()
{
	timer_ = 0;
	timer_id_ = kuint32max;
	spr_timer_bg_->setVisible(false);
	text_timer_number_->setVisible(false);
	unschedule(schedule_selector(GameScene::OnGameTimer));
}

// 计时器回调
void GameScene::OnGameTimer(float dt)
{
	--timer_;
	if (auto_out_card_ && current_user_ == client_kernel_->GetMeChairID())
	{
		if (timer_id_ == kTimerOutCard && timer_ < kTimerOutCardTime - 1)
		{
			AutoOutCard();
			current_user_ = kuint16max;
			HideGameTimer();
			return;
		}
		if (timer_id_ == kTimerCallScore && timer_ < kTimerCallScoreTime - 1)
		{
			OnClickCall0(nullptr);
			current_user_ = kuint16max;
			HideGameTimer();
			return;
		}
	}

	if (current_user_ == client_kernel_->GetMeChairID() && timer_ <= 5)
	{
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/LaterTime.wav");
	}

	if (timer_ <= 0)
	{
		if (timer_id_ == kTimerStart)
		{
			Director::getInstance()->end();
		}

		if (current_user_ == client_kernel_->GetMeChairID())
		{
			if (timer_id_ == kTimerCallScore)
			{
				OnClickCall0(nullptr);
			}
			else if (timer_id_ == kTimerOutCard)
			{
				if (turn_outcard_count_ == 0)
				{
					AutoOutCard();
				}
				else
				{
					if (btn_pass_->isVisible() && btn_pass_->isEnabled())
					{
						OnClickPass(nullptr);
					}
					else if (btn_miss_->isVisible())
					{
						OnClickMiss(nullptr);
					}
				}
			}
		}
		HideGameTimer();
		return;
	}
	char temp[128] = { 0 };
	sprintf(temp, "%d", timer_);
	text_timer_number_->setString(temp);
}

// 发牌
void GameScene::SendCards(uint16 first_user)
{
	SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/ShuffleCard.wav");
	back_card_[1]->SetCardCount(54);
	uint16 send_chair_id = first_user;
	Sprite* send_card_item = Sprite::create("ddz/Resources/Image/cards/card_00.png");
	send_card_item->setPosition(back_card_[1]->GetRailCardPos());
	send_card_item->setTag(send_chair_id);
	send_card_item->runAction(Sequence::create(MoveTo::create(0.16f, send_cards_pos_[send_chair_id]), CallFuncN::create(CC_CALLBACK_1(GameScene::OnSendCardsCallback, this)), nullptr));
	addChild(send_card_item);
	back_card_[1]->SetCardCount(back_card_[1]->GetCardCount() - 1);

	send_chair_id = (send_chair_id + 1) % kMaxPlayer;
	Sprite* send_card_item1 = Sprite::create("ddz/Resources/Image/cards/card_00.png");
	send_card_item1->setPosition(back_card_[1]->GetRailCardPos());
	send_card_item1->setTag(send_chair_id);
	send_card_item1->runAction(Sequence::create(DelayTime::create(0.05f), MoveTo::create(0.16f, send_cards_pos_[send_chair_id]), CallFuncN::create(CC_CALLBACK_1(GameScene::OnSendCardsCallback, this)), nullptr));
	addChild(send_card_item1);
	back_card_[1]->SetCardCount(back_card_[1]->GetCardCount() - 1);

	send_chair_id = (send_chair_id + 1) % kMaxPlayer;
	Sprite* send_card_item2 = Sprite::create("ddz/Resources/Image/cards/card_00.png");
	send_card_item2->setPosition(back_card_[1]->GetRailCardPos());
	send_card_item2->setTag(send_chair_id);
	send_card_item2->runAction(Sequence::create(DelayTime::create(0.1f), MoveTo::create(0.16f, send_cards_pos_[send_chair_id]), CallFuncN::create(CC_CALLBACK_1(GameScene::OnSendCardsCallback, this)), nullptr));
	addChild(send_card_item2);
	back_card_[1]->SetCardCount(back_card_[1]->GetCardCount() - 1);
}

// 发牌回调
void GameScene::OnSendCardsCallback(cocos2d::Node* node)
{
	uint16 chair_id = node->getTag();

	bool dispatch = true;
	if (chair_id != 1)
	{
		back_card_[chair_id]->SetCardCount(back_card_[chair_id]->GetCardCount() + 1);
		if (back_card_[chair_id]->GetCardCount() >= 17)
		{
			removeChild(node);
			dispatch = false;
		}
	}
	else
	{
		myself_card_->SetCardData(hand_card_data_, myself_card_->GetCardCount() + 1);
		if (myself_card_->GetCardCount() >= 17)
		{
			removeChild(node);
			dispatch = false;
		}
	}

	if (dispatch)
	{
		node->setPosition(back_card_[1]->GetRailCardPos());
		node->runAction(Sequence::create(MoveTo::create(0.16f, send_cards_pos_[chair_id]), CallFuncN::create(CC_CALLBACK_1(GameScene::OnSendCardsCallback, this)), nullptr));
		back_card_[1]->SetCardCount(back_card_[1]->GetCardCount() - 1);
		//SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/InitCard.wav");
	}

	// 发牌结束
	if (back_card_[0]->GetCardCount() == 17 && back_card_[2]->GetCardCount() == 17 && myself_card_->GetCardCount() == 17)
	{
		back_card_[1]->SetCardCount(0);
		Sprite* bottom_card = Sprite::create("ddz/Resources/Image/cards/card_00.png");
		float card_width = bottom_card->getContentSize().width;
		bottom_card->setPosition(Vec2(1280 / 2 - card_width, 618));
		bottom_card->setName("bottom_card_bg_1");
		addChild(bottom_card);
		bottom_card = Sprite::create("ddz/Resources/Image/cards/card_00.png");
		bottom_card->setName("bottom_card_bg_2");
		bottom_card->setPosition(Vec2(1280 / 2, 618));
		addChild(bottom_card);
		bottom_card = Sprite::create("ddz/Resources/Image/cards/card_00.png");
		bottom_card->setName("bottom_card_bg_3");
		bottom_card->setPosition(Vec2(1280 / 2 + card_width, 618));
		addChild(bottom_card);

		if (!client_kernel_->IsLookonMode() && current_user_ == client_kernel_->GetMeChairID())
		{
			btn_call0_->setVisible(true);
			btn_call1_->setVisible(true);
			btn_call2_->setVisible(true);
			btn_call3_->setVisible(true);
			btn_call0_->setBright(true);
			btn_call1_->setBright(true);
			btn_call2_->setBright(true);
			btn_call3_->setBright(true);
			btn_call0_->setEnabled(true);
			btn_call1_->setEnabled(true);
			btn_call2_->setEnabled(true);
			btn_call3_->setEnabled(true);
		}

		GameLogic::SortCardList(hand_card_data_, hand_card_count_);
		myself_card_->SetCardData(hand_card_data_, hand_card_count_);

		ShowGameTimer(ViewChairID(current_user_), kTimerCallScore, kTimerCallScoreTime);
	}
}

// 开始按钮
void GameScene::OnClickStart(Ref* sender)
{
	//CloseGameClient();
	//return;
	hand_card_count_ = 0;
	current_user_ = kuint16max;
	lord_user_ = kuint16max;
	lord_score_ = 0;
	turn_outcard_count_ = 0;
	search_card_result_index_ = 0;
	memset(hand_card_data_, 0, sizeof(hand_card_data_));
	memset(bottom_card_data_, 0, sizeof(bottom_card_data_));
	memset(turn_ountcard_data_, 0, sizeof(turn_ountcard_data_));
	memset(&search_card_result_, 0, sizeof(search_card_result_));

	HideGameTimer();
	stopAllActions();
	unscheduleAllCallbacks();

	spr_result_back_->setVisible(false);
	text_lord_score_->setVisible(false);
	text_result_lord_score_->setVisible(false);
	text_result_bomb_->setVisible(false);
	text_result_chuntian_->setVisible(false);
	text_result_times_->setVisible(false);
	char temp[128] = { 0 };
	for (uint16 i = 0; i < kMaxPlayer; ++i)
	{
		spr_alarm_[i]->setVisible(false);
		text_result_score_win_[i]->setVisible(false);
		text_result_score_loss_[i]->setVisible(false);
		out_card_[i]->SetCardData(nullptr, 0, false);
		sprintf(temp, "bottom_card_small_%d", i + 1);
		Node* node = getChildByName(temp);
		if (node)
			removeChild(node);
	}

	btn_start_->setVisible(false);
	client_kernel_->SendUserReady();
	spr_ready_[1]->setVisible(true);
}

// 不叫
void GameScene::OnClickCall0(cocos2d::Ref* sender)
{
	if (client_kernel_->GetMeUserItem()->GetGender() == GENDER_MALE)
	{
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Male/CallScore_0.mp3");
	}
	else
	{
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Female/CallScore_0.mp3");
	}
	HideGameTimer();
	btn_call0_->setVisible(false);
	btn_call1_->setVisible(false);
	btn_call2_->setVisible(false);
	btn_call3_->setVisible(false);

	CMD_C_CallScore call_score;
	call_score.call_score = 0xFF;
	SendSocketData(SUB_C_CALL_SCORE, &call_score, sizeof(call_score));
}

// 1分
void GameScene::OnClickCall1(cocos2d::Ref* sender)
{
	if (client_kernel_->GetMeUserItem()->GetGender() == GENDER_MALE)
	{
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Male/CallScore_1.mp3");
	}
	else
	{
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Female/CallScore_1.mp3");
	}
	HideGameTimer();
	btn_call0_->setVisible(false);
	btn_call1_->setVisible(false);
	btn_call2_->setVisible(false);
	btn_call3_->setVisible(false);

	CMD_C_CallScore call_score;
	call_score.call_score = 1;
	SendSocketData(SUB_C_CALL_SCORE, &call_score, sizeof(call_score));
}

// 2分
void GameScene::OnClickCall2(cocos2d::Ref* sender)
{
	if (client_kernel_->GetMeUserItem()->GetGender() == GENDER_MALE)
	{
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Male/CallScore_2.mp3");
	}
	else
	{
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Female/CallScore_2.mp3");
	}
	HideGameTimer();
	btn_call0_->setVisible(false);
	btn_call1_->setVisible(false);
	btn_call2_->setVisible(false);
	btn_call3_->setVisible(false);

	CMD_C_CallScore call_score;
	call_score.call_score = 2;
	SendSocketData(SUB_C_CALL_SCORE, &call_score, sizeof(call_score));
}

// 3分
void GameScene::OnClickCall3(cocos2d::Ref* sender)
{
	if (client_kernel_->GetMeUserItem()->GetGender() == GENDER_MALE)
	{
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Male/CallScore_3.mp3");
	}
	else
	{
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Female/CallScore_3.mp3");
	}
	HideGameTimer();
	btn_call0_->setVisible(false);
	btn_call1_->setVisible(false);
	btn_call2_->setVisible(false);
	btn_call3_->setVisible(false);

	CMD_C_CallScore call_score;
	call_score.call_score = 3;
	SendSocketData(SUB_C_CALL_SCORE, &call_score, sizeof(call_score));
}

// 出牌
void GameScene::OnClickOutCard(cocos2d::Ref* sender)
{
	if (!btn_outcard_->isBright() || !btn_outcard_->isVisible())
		return;

	// 删除时间
	HideGameTimer();
	SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/DisCard.wav");

	// 设置变量
	current_user_ = kuint16max;
	search_card_result_index_ = 0;
	search_card_result_.search_count = 0;

	btn_outcard_->setVisible(false);
	btn_pass_->setVisible(false);
	btn_hint_->setVisible(false);

	// 获取扑克
	uint8 card_data[kMaxCardCount];
	uint8 card_count = myself_card_->GetShootCardData(card_data, arraysize(card_data));

	// 排列扑克
	GameLogic::SortCardList(card_data, card_count);

	GameLogic::RemoveCardList(card_data, card_count, hand_card_data_, hand_card_count_);
	hand_card_count_ -= card_count;
	myself_card_->SetOutCardData(out_card_[1]);
	myself_card_->SetCardData(hand_card_data_, hand_card_count_);

	uint8 turn_card_type = GameLogic::GetCardType(card_data, card_count);
	// 播放动画
	if (turn_card_type == CT_BOMB_CARD)
	{
		PlayBombAnimation(out_card_[1]->GetCenterPos());
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Bomb.wav");
	}
	else if (turn_card_type == CT_MISSILE_CARD)
	{
		PlayRocketAnimation();
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Rocket.wav");
	}
	else if (turn_card_type == CT_DOUBLE_LINE)
	{
		PlayChainPairsAnimation(out_card_[1]->GetCenterPos());
	}
	else if (turn_card_type == CT_SINGLE_LINE)
	{
		PlayStraightAnimation(out_card_[1]->GetCenterPos());
	}
	else if (turn_card_type == CT_THREE_LINE || (turn_card_type == CT_THREE_TAKE_ONE && card_count >= 8) || (turn_card_type == CT_THREE_TAKE_TWO && card_count >= 10))
	{
		PlayPlaneAnimation();
		SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/plane.ogg");
	}

	PlayGameSound(client_kernel_->GetMeChairID(), turn_card_type, card_data, card_count);

	CMD_C_OutCard outcard;
	outcard.card_count = card_count;
	memcpy(outcard.card_data, card_data, card_count * sizeof(uint8));
	SendSocketData(SUB_C_OUT_CARD, &outcard, sizeof(outcard) - sizeof(outcard.card_data) + outcard.card_count * sizeof(uint8));
}

// 不出
void GameScene::OnClickPass(cocos2d::Ref* sender)
{
	// 删除时间
	HideGameTimer();
	SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Pass.wav");

	// 设置变量
	current_user_ = kuint16max;
	search_card_result_index_ = 0;
	search_card_result_.search_count = 0;

	btn_outcard_->setVisible(false);
	btn_pass_->setVisible(false);
	btn_hint_->setVisible(false);

	myself_card_->SetShootCardData(nullptr, 0);

	SetSpeakContent(1, SPEAK_PASS);
	SendSocketData(SUB_C_PASS_CARD);
}

// 提示
void GameScene::OnClickHint(cocos2d::Ref* sender)
{
	if (search_card_result_.search_count > 0)
	{
		myself_card_->SetShootCardData(search_card_result_.result_data[search_card_result_index_], search_card_result_.card_count[search_card_result_index_]);
		search_card_result_index_ = (search_card_result_index_ + 1) % search_card_result_.search_count;

		bool outcard = VerdictOutCard();
		assert(outcard);
		btn_outcard_->setBright(outcard);
		btn_outcard_->setEnabled(outcard);

		return;
	}

	// 放弃出牌
	OnClickPass(nullptr);
}

// 要不起
void GameScene::OnClickMiss(cocos2d::Ref* sender)
{
	spr_miss_->setVisible(false);
	btn_miss_->setVisible(false);
	// 删除时间
	HideGameTimer();
	SimpleAudioEngine::getInstance()->playEffect("ddz/Resources/Sound/Pass.wav");

	// 设置变量
	current_user_ = kuint16max;
	search_card_result_index_ = 0;
	search_card_result_.search_count = 0;

	SetSpeakContent(1, SPEAK_PASS);
	SendSocketData(SUB_C_PASS_CARD);
}

// 自动出牌
void GameScene::OnClickAutoOutCard(cocos2d::Ref* sender)
{
	auto_out_card_ = true;
	btn_auto_outcard_->setVisible(false);
	btn_cancel_auto_outcard_->setVisible(true);
}

// 取消自动出牌
void GameScene::OnClickCancelAutoOutCard(cocos2d::Ref* sender)
{
	auto_out_card_ = false;
	btn_auto_outcard_->setVisible(true);
	btn_cancel_auto_outcard_->setVisible(false);
}

// 鼠标右键
void GameScene::OnCardMouseRight()
{
	OnClickOutCard(nullptr);
}

// 鼠标左键
void GameScene::OnCardMouseLeft()
{
	bool outcard = VerdictOutCard();
	btn_outcard_->setBright(outcard);
	btn_outcard_->setEnabled(outcard);
}

// 说话内容
void GameScene::SetSpeakContent(uint16 chair_id, SpeakContent sc)
{
	if (sc == SPEAK_COUNT && chair_id == kuint16max)
	{
		for (uint16 i = 0; i < kMaxPlayer; ++i)
		{
			spr_spreak_[i]->setVisible(false);
		}
	}
	else
	{
		assert(chair_id < kMaxPlayer);
		if (sc == SPEAK_COUNT)
		{
			spr_spreak_[chair_id]->setVisible(false);
		}
		else
		{
			char temp[256] = { 0 };
			if (sc == SPEAK_PASS)
			{
				sprintf(temp, "ddz/Resources/Image/speak_pass_%d.png", chair_id == 2 ? 1 : 0);
			}
			else
			{
				sprintf(temp, "ddz/Resources/Image/callsore/lord_speak_call_%d_%d.png", sc, chair_id == 2 ? 1 : 0);
			}
			spr_spreak_[chair_id]->setTexture(temp);
			spr_spreak_[chair_id]->setVisible(true);
		}
	}
}

// 显示底牌
void GameScene::ShowBottomCards()
{
	bottom_cards_ani_finish_ = false;
	char temp[128] = { 0 };
	for (int i = 0; i < 3; ++i)
	{
		sprintf(temp, "bottom_card_bg_%d", i + 1);
		Node* bottom = getChildByName(temp);
		assert(bottom);
		bottom->runAction(Sequence::create(DelayTime::create(0.25f * i), OrbitCamera::create(0.25f, 1, 0, 0, -90, 0, 0), CallFunc::create([=]{removeChild(bottom);}), CallFunc::create([=]
		{
			char temp1[128] = { 0 };
			if (bottom_card_data_[i] == 0x4E || bottom_card_data_[i] == 0x4F)
			{
				sprintf(temp1, "ddz/Resources/Image/cards/card_%02d.png", bottom_card_data_[i] == 0x4E ? 53 : 54);
			}
			else
			{
				sprintf(temp1, "ddz/Resources/Image/cards/card_%02d.png", (bottom_card_data_[i] & MASK_VALUE) + ((bottom_card_data_[i] & MASK_COLOR) >> 4) * 13);
			}
			Sprite* card = Sprite::create(temp1);
			card->setPosition(bottom->getPosition());
			sprintf(temp1, "bottom_card_%d", i + 1);
			card->setName(temp1);
			addChild(card);
			ActionInterval* act = Sequence::create(DelayTime::create(1 - 0.25f * i), Spawn::create(MoveTo::create(0.5f, bottom_cards_pos_[i]), ScaleTo::create(0.5f, 32.f / 121.f, 44.f / 162.f), nullptr), CallFunc::create([=]
			{
				char temp2[128] = { 0 };
				if (bottom_card_data_[i] == 0x4E || bottom_card_data_[i] == 0x4F)
				{
					sprintf(temp2, "ddz/Resources/Image/bottomcards/bottomcard_%02d.png", bottom_card_data_[i] == 0x4E ? 53 : 54);
				}
				else
				{
					sprintf(temp2, "ddz/Resources/Image/bottomcards/bottomcard_%02d.png", (bottom_card_data_[i] & MASK_VALUE) + ((bottom_card_data_[i] & MASK_COLOR) >> 4) * 13);
				}
				Sprite* scard = Sprite::create(temp2);
				scard->setPosition(bottom_cards_pos_[i]);
				sprintf(temp2, "bottom_card_small_%d", i + 1);
				scard->setName(temp2);
				addChild(scard);
				removeChild(card);

				// 翻牌结束
				if (i == 2)
				{
					if (lord_user_ == client_kernel_->GetMeChairID())
					{
						memcpy(&hand_card_data_[17], bottom_card_data_, sizeof(bottom_card_data_));
						hand_card_count_ += 3;
						GameLogic::SortCardList(hand_card_data_, hand_card_count_);
						myself_card_->SetCardData(hand_card_data_, hand_card_count_);
						if (!auto_out_card_)
							myself_card_->SetShootCardData(bottom_card_data_, 3);
					}
					else
					{
						back_card_[ViewChairID(lord_user_)]->SetCardCount(kMaxCardCount);
					}

					if (!client_kernel_->IsLookonMode())
					{
						if (current_user_ == client_kernel_->GetMeChairID() && !bottom_cards_ani_finish_)
						{
							btn_outcard_->setVisible(true);
							btn_pass_->setVisible(true);
							btn_hint_->setVisible(true);

							btn_outcard_->setBright(false);
							btn_pass_->setBright(false);
							btn_outcard_->setEnabled(false);
							btn_pass_->setEnabled(false);

							GameLogic::SearchOutCard(hand_card_data_, hand_card_count_, turn_ountcard_data_, turn_outcard_count_, &search_card_result_);
						}

						myself_card_->SetTouchEnable(true);
					}

					if (!bottom_cards_ani_finish_)
						ShowGameTimer(ViewChairID(current_user_), kTimerOutCard, kTimerOutCardTime);

					bottom_cards_ani_finish_ = true;
				}
			}), nullptr);
			card->runAction(Sequence::create(FlipX::create(true), OrbitCamera::create(0.25f, 1, 0, -90, -90, 0, 0), act, nullptr));
		}), nullptr));
	}
}

// 出牌判断
bool GameScene::VerdictOutCard()
{
	// 状态判断
	if (current_user_ != client_kernel_->GetMeChairID())
		return false;
	if (client_kernel_->GetGameStatus() != GAME_SCENE_PLAY)
		return false;

	// 获取扑克
	uint8 card_data[kMaxCardCount];
	uint8 shoot_count = myself_card_->GetShootCardData(card_data, arraysize(card_data));

	// 出牌判断
	if (shoot_count > 0)
	{
		// 类型判断
		GameLogic::SortCardList(card_data, shoot_count);
		if (GameLogic::GetCardType(card_data, shoot_count) == CT_ERROR)
			return false;

		// 跟牌判断
		if (turn_outcard_count_ == 0)
			return true;
		return GameLogic::CompareCard(turn_ountcard_data_, card_data, turn_outcard_count_, shoot_count);
	}

	return false;
}

// 炸弹动画
void GameScene::PlayBombAnimation(cocos2d::Vec2 pos)
{
	spr_bomb_->setPosition(pos);
	spr_bomb_->runAction(Sequence::create(Show::create(), ani_bomb_, Hide::create(), nullptr));
}

// 火箭动画
void GameScene::PlayRocketAnimation()
{
	spr_rocket_yan_->runAction(Sequence::create(Show::create(), ani_rocket_, Hide::create(), nullptr));
	spr_rocket_->setPosition(1280 / 2, 0);
	spr_rocket_->runAction(Sequence::create(Show::create(), DelayTime::create(0.5f), MoveBy::create(1.0f, Vec2(0, 1000)), Hide::create(), nullptr));
}

// 飞机动画
void GameScene::PlayPlaneAnimation()
{
	spr_plane_->setPosition(Vec2(1280 + 190, 800 / 2));
	spr_plane_->runAction(Sequence::create(Show::create(), Spawn::create(ani_plane_, MoveBy::create(1.5f, Vec2(-1800, 0)), nullptr), Hide::create(), nullptr));
}

// 连对
void GameScene::PlayChainPairsAnimation(cocos2d::Vec2 pos)
{
	spr_chainpairs_->setPosition(pos);
	spr_chainpairs_->runAction(Sequence::create(Show::create(), ani_chainpairs_, Hide::create(), nullptr));
}

// 顺子
void GameScene::PlayStraightAnimation(cocos2d::Vec2 pos)
{
	spr_straight_->setPosition(pos);
	spr_straight_->runAction(Sequence::create(Show::create(), ani_straight_, Hide::create(), nullptr));
}

// 春天
void GameScene::PlayChunTianAnimation()
{
	spr_chuntian_->runAction(Sequence::create(Show::create(), ani_chuntian_, Hide::create(), nullptr));
}

// 警报
void GameScene::PlayAlarmAnimation(uint16 chair_id)
{
	if (chair_id == 1)
		return;

	spr_alarm_[chair_id]->setVisible(true);
	spr_alarm_[chair_id]->stopAllActions();
	spr_alarm_[chair_id]->runAction(RepeatForever::create(ani_alarm_->clone()));
}

// 角色切换
void GameScene::PlaySwitchRoleAnimation(uint16 lord_user)
{
	for (uint16 i = 0; i < kMaxPlayer; ++i)
	{
		spr_user_avatar_[i]->setVisible(false);
		spr_switch_role_[i]->runAction(Sequence::create(Show::create(), ani_switch_role_[i], Hide::create(), CallFunc::create([=]
		{
			spr_role_[i]->setTexture(i == lord_user ? "ddz/Resources/Image/role_lord.png" : "ddz/Resources/Image/role_farmer.png");
			spr_role_[i]->setVisible(true);
		}), nullptr));
	}
}

// 自动出牌
void GameScene::AutoOutCard()
{
	if (client_kernel_->GetGameStatus() != GAME_SCENE_PLAY)
		return;
	if (client_kernel_->IsLookonMode() || current_user_ != client_kernel_->GetMeChairID())
		return;

	if (turn_outcard_count_ == 0 || auto_out_card_)
	{
		if (search_card_result_.search_count > 0)
		{
			myself_card_->SetShootCardData(search_card_result_.result_data[0], search_card_result_.card_count[0]);
			bool outcard = VerdictOutCard();
			btn_outcard_->setBright(outcard);
			btn_outcard_->setEnabled(outcard);
			OnClickOutCard(nullptr);
			return;
		}
	}

	// 放弃出牌
	if (turn_outcard_count_ > 0)
	{
		if (search_card_result_.search_count == 0)
		{
			OnClickMiss(nullptr);
		}
		else
		{
			OnClickPass(nullptr);
		}
	}
}

// 播放声音
void GameScene::PlayGameSound(uint16 chair_id, uint8 card_type, uint8 card_data[], uint8 card_count)
{
	char temp[256] = { 0 };
	char gender[64] = { 0 };
	IGameUserItem* user_item = client_kernel_->GetTableUserItem(chair_id);
	if (user_item == nullptr)
		return;

	strcpy(gender, user_item->GetGender() == GENDER_MALE ? "male" : "Female");
	switch (card_type)
	{
	case CT_MISSILE_CARD:
		sprintf(temp, "ddz/Resources/Sound/%s/wangzha.wav", gender);
		SimpleAudioEngine::getInstance()->playEffect(temp);
		break;
	case CT_BOMB_CARD:
		sprintf(temp, "ddz/Resources/Sound/%s/zhadan.wav", gender);
		SimpleAudioEngine::getInstance()->playEffect(temp);
		break;
	case CT_FOUR_TAKE_TWO:
		sprintf(temp, "ddz/Resources/Sound/%s/sidailiangdui.wav", gender);
		SimpleAudioEngine::getInstance()->playEffect(temp);
		break;
	case CT_FOUR_TAKE_ONE:
		sprintf(temp, "ddz/Resources/Sound/%s/sidaier.wav", gender);
		SimpleAudioEngine::getInstance()->playEffect(temp);
		break;
	case CT_THREE_TAKE_TWO:
		if (card_count == 5)
		{
			sprintf(temp, "ddz/Resources/Sound/%s/sandaiyidui.wav", gender);
			SimpleAudioEngine::getInstance()->playEffect(temp);
		}
		else if (card_count >= 10)
		{
			sprintf(temp, "ddz/Resources/Sound/%s/feiji.wav", gender);
			SimpleAudioEngine::getInstance()->playEffect(temp);
		}
		break;
	case CT_THREE_TAKE_ONE:
		if (card_count == 4)
		{
			sprintf(temp, "ddz/Resources/Sound/%s/sandaiyi.wav", gender);
			SimpleAudioEngine::getInstance()->playEffect(temp);
		}
		else if (card_count >= 8)
		{
			sprintf(temp, "ddz/Resources/Sound/%s/feiji.wav", gender);
			SimpleAudioEngine::getInstance()->playEffect(temp);
		}
		break;
	case CT_THREE_LINE:
		sprintf(temp, "ddz/Resources/Sound/%s/feiji.wav", gender);
		SimpleAudioEngine::getInstance()->playEffect(temp);
		break;
	case CT_DOUBLE_LINE:
		sprintf(temp, "ddz/Resources/Sound/%s/liandui.wav", gender);
		SimpleAudioEngine::getInstance()->playEffect(temp);
		break;
	case CT_SINGLE_LINE:
		sprintf(temp, "ddz/Resources/Sound/%s/shunzi.wav", gender);
		SimpleAudioEngine::getInstance()->playEffect(temp);
		break;
	case CT_THREE:
		sprintf(temp, "ddz/Resources/Sound/%s/sange.wav", gender);
		SimpleAudioEngine::getInstance()->playEffect(temp);
		break;
	case CT_DOUBLE:
		sprintf(temp, "ddz/Resources/Sound/%s/dui%d.wav", gender, GameLogic::GetCardValue(card_data[0]));
		SimpleAudioEngine::getInstance()->playEffect(temp);
		break;
	case CT_SINGLE:
		sprintf(temp, "ddz/Resources/Sound/%s/%d.wav", gender, GameLogic::GetCardValue(card_data[0]));
		SimpleAudioEngine::getInstance()->playEffect(temp);
		break;
	default:
		break;
	}
}

}
