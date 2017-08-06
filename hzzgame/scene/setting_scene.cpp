
#include "setting_scene.h"

#include "extensions/cocos-ext.h"
#include "cocostudio/CocoStudio.h"

#include "../data/global_user_info.h"
#include "../base/convert.h"
#include "../kernel/plaza_kernel.h"
#include "dialog_password_modify.h"
#include "web_scene.h"

Scene* SettingScene::createScene()
{
    auto scene = Scene::create();
    
	auto layer = SettingScene::create();

    scene->addChild(layer);

    return scene;
}

SettingScene::SettingScene()
{
}

SettingScene::~SettingScene()
{
}

bool SettingScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();

	auto ui = CSLoader::createNode("setting.csb");
	addChild(ui);

	Button* btn_back = (Button*)Helper::seekWidgetByName((Widget*)ui, "back");
	btn_back->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickBack, this));

	//ScrollView* scrollview = (ScrollView*)Helper::seekWidgetByName((Widget*)ui, "ListView");
	//scrollview->setInnerContainerSize(Size(1280, 1200.0f));
	ScrollView* scrollview = ScrollView::create();
	scrollview->setContentSize(Size(1280, 600.0f));
	scrollview->setInnerContainerSize(Size(1280, 1200.0f));
	scrollview->setPosition(Vec2(0, 0));
	scrollview->setTouchEnabled(true);
	scrollview->setBounceEnabled(true);
	scrollview->setScrollBarEnabled(false);
	addChild(scrollview);

	Text* text_item1 = Text::create(a2u("反馈与帮助"), "arial", 30);
	text_item1->setAnchorPoint(Vec2(0, 0.5f));
	text_item1->setPosition(Vec2(scrollview->getContentSize().width / 2 - 392, scrollview->getInnerContainerSize().height - 30));
	text_item1->setColor(Color3B(145, 145, 145));
	scrollview->addChild(text_item1);

	Scale9Sprite* item1_bg = Scale9Sprite::create("common/item_bg.png");
	item1_bg->setAnchorPoint(Vec2(0.5f, 1));
	item1_bg->setCapInsets(Rect(10, 10, 33, 32));
	item1_bg->setContentSize(Size(822, 179));
	item1_bg->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item1->getBottomBoundary() - 10));
	scrollview->addChild(item1_bg);

	Button* btn_item1_feedback = Button::create("setting/button_input.png");
	btn_item1_feedback->setScale9Enabled(true);
	btn_item1_feedback->setZoomScale(0);
	btn_item1_feedback->setPressedActionEnabled(false);
	btn_item1_feedback->setContentSize(Size(784, 70));
	btn_item1_feedback->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item1->getBottomBoundary() - 24 - 35));
	btn_item1_feedback->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickFeedback, this));
	scrollview->addChild(btn_item1_feedback);

	Text* text_item1_feedback = Text::create(a2u("反馈"), "arial", 36);
	text_item1_feedback->setAnchorPoint(Vec2(0, 0.5f));
	text_item1_feedback->setPosition(Vec2(scrollview->getContentSize().width / 2 - 372, text_item1->getBottomBoundary() - 24 - 35));
	text_item1_feedback->setColor(Color3B(89, 89, 89));
	scrollview->addChild(text_item1_feedback);

	Button* btn_item1_feedback_ex = Button::create("setting/button_right.png");
	btn_item1_feedback_ex->setPosition(Vec2(scrollview->getContentSize().width / 2 + 362, text_item1->getBottomBoundary() - 24 - 35));
	btn_item1_feedback_ex->setZoomScale(0);
	btn_item1_feedback_ex->setPressedActionEnabled(false);
	btn_item1_feedback_ex->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickFeedback, this));
	scrollview->addChild(btn_item1_feedback_ex);

	Button* btn_item1_help = Button::create("setting/button_input.png");
	btn_item1_help->setScale9Enabled(true);
	btn_item1_help->setZoomScale(0);
	btn_item1_help->setPressedActionEnabled(false);
	btn_item1_help->setContentSize(Size(784, 70));
	btn_item1_help->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item1->getBottomBoundary() - 24 - 35 - 84));
	btn_item1_help->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickHelp, this));
	scrollview->addChild(btn_item1_help);

	Text* text_item1_help = Text::create(a2u("帮助"), "arial", 36);
	text_item1_help->setAnchorPoint(Vec2(0, 0.5f));
	text_item1_help->setPosition(Vec2(scrollview->getContentSize().width / 2 - 372, text_item1->getBottomBoundary() - 24 - 35 - 84));
	text_item1_help->setColor(Color3B(89, 89, 89));
	scrollview->addChild(text_item1_help);

	Button* btn_item1_help_ex = Button::create("setting/button_right.png");
	btn_item1_help_ex->setPosition(Vec2(scrollview->getContentSize().width / 2 + 362, text_item1->getBottomBoundary() - 24 - 35 - 84));
	btn_item1_help_ex->setZoomScale(0);
	btn_item1_help_ex->setPressedActionEnabled(false);
	btn_item1_help_ex->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickHelp, this));
	scrollview->addChild(btn_item1_help_ex);

	Text* text_item2 = Text::create(a2u("个人账户"), "arial", 30);
	text_item2->setAnchorPoint(Vec2(0, 0.5f));
	text_item2->setPosition(Vec2(scrollview->getContentSize().width / 2 - 392, btn_item1_help->getBottomBoundary() - 40));
	text_item2->setColor(Color3B(145, 145, 145));
	scrollview->addChild(text_item2);

	Scale9Sprite* item2_bg = Scale9Sprite::create("common/item_bg.png");
	item2_bg->setAnchorPoint(Vec2(0.5f, 1));
	item2_bg->setCapInsets(Rect(10, 10, 33, 32));
	item2_bg->setContentSize(Size(822, 261));
	item2_bg->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item2->getBottomBoundary() - 10));
	scrollview->addChild(item2_bg);

	Button* btn_item2_accounts = Button::create("setting/button_input.png");
	btn_item2_accounts->setScale9Enabled(true);
	btn_item2_accounts->setZoomScale(0);
	btn_item2_accounts->setPressedActionEnabled(false);
	btn_item2_accounts->setContentSize(Size(784, 70));
	btn_item2_accounts->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item2->getBottomBoundary() - 24 - 35));
	scrollview->addChild(btn_item2_accounts);

	Text* text_item2_accounts = Text::create(StringUtils::format("%s:%s", a2u("账号").c_str(), user_data->accounts.c_str()), "arial", 36);
	text_item2_accounts->setAnchorPoint(Vec2(0, 0.5f));
	text_item2_accounts->setPosition(Vec2(scrollview->getContentSize().width / 2 - 372, text_item2->getBottomBoundary() - 24 - 35));
	text_item2_accounts->setColor(Color3B(89, 89, 89));
	scrollview->addChild(text_item2_accounts);

	auto btn_item2_accounts_switch = Button::create("setting/btn_switch_accounts.png");
	btn_item2_accounts_switch->setPosition(Vec2(scrollview->getContentSize().width / 2 + 300, text_item2->getBottomBoundary() - 24 - 35));
	btn_item2_accounts_switch->setZoomScale(0);
	btn_item2_accounts_switch->setPressedActionEnabled(false);
	btn_item2_accounts_switch->setTitleText(a2u("切换账号"));
	btn_item2_accounts_switch->setTitleFontName("arial");
	btn_item2_accounts_switch->setTitleFontSize(30);
	btn_item2_accounts_switch->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickSwitchAccounts, this));
	scrollview->addChild(btn_item2_accounts_switch);

	Button* btn_item2_password = Button::create("setting/button_input.png");
	btn_item2_password->setScale9Enabled(true);
	btn_item2_password->setZoomScale(0);
	btn_item2_password->setPressedActionEnabled(false);
	btn_item2_password->setContentSize(Size(784, 70));
	btn_item2_password->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item2->getBottomBoundary() - 24 - 35 - 84));
	btn_item2_password->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickPasswordModify, this));
	scrollview->addChild(btn_item2_password);

	Text* text_item2_password = Text::create(a2u("密码修改"), "arial", 36);
	text_item2_password->setAnchorPoint(Vec2(0, 0.5f));
	text_item2_password->setPosition(Vec2(scrollview->getContentSize().width / 2 - 372, text_item2->getBottomBoundary() - 24 - 35 - 84));
	text_item2_password->setColor(Color3B(89, 89, 89));
	scrollview->addChild(text_item2_password);

	Button* btn_item2_password_ex = Button::create("setting/button_right.png");
	btn_item2_password_ex->setPosition(Vec2(scrollview->getContentSize().width / 2 + 362, text_item2->getBottomBoundary() - 24 - 35 - 84));
	btn_item2_password_ex->setZoomScale(0);
	btn_item2_password_ex->setPressedActionEnabled(false);
	btn_item2_password_ex->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickPasswordModify, this));
	scrollview->addChild(btn_item2_password_ex);

	Button* btn_item2_bindphone = Button::create("setting/button_input.png");
	btn_item2_bindphone->setScale9Enabled(true);
	btn_item2_bindphone->setZoomScale(0);
	btn_item2_bindphone->setPressedActionEnabled(false);
	btn_item2_bindphone->setContentSize(Size(784, 70));
	btn_item2_bindphone->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item2->getBottomBoundary() - 24 - 35 - 84 - 84));
	btn_item2_bindphone->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickBindPhone, this));
	scrollview->addChild(btn_item2_bindphone);

	Text* text_item2_bindphone = Text::create(a2u("绑定手机"), "arial", 36);
	text_item2_bindphone->setAnchorPoint(Vec2(0, 0.5f));
	text_item2_bindphone->setPosition(Vec2(scrollview->getContentSize().width / 2 - 372, text_item2->getBottomBoundary() - 24 - 35 - 84 - 84));
	text_item2_bindphone->setColor(Color3B(89, 89, 89));
	scrollview->addChild(text_item2_bindphone);

	Button* btn_item2_bindphone_ex = Button::create("setting/button_right.png");
	btn_item2_bindphone_ex->setPosition(Vec2(scrollview->getContentSize().width / 2 + 362, text_item2->getBottomBoundary() - 24 - 35 - 84 - 84));
	btn_item2_bindphone_ex->setZoomScale(0);
	btn_item2_bindphone_ex->setPressedActionEnabled(false);
	btn_item2_bindphone_ex->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickBindPhone, this));
	scrollview->addChild(btn_item2_bindphone_ex);

	Text* text_item3 = Text::create(a2u("系统音效"), "arial", 30);
	text_item3->setAnchorPoint(Vec2(0, 0.5f));
	text_item3->setPosition(Vec2(scrollview->getContentSize().width / 2 - 392, btn_item2_bindphone->getBottomBoundary() - 40));
	text_item3->setColor(Color3B(145, 145, 145));
	scrollview->addChild(text_item3);

	Scale9Sprite* item3_bg = Scale9Sprite::create("common/item_bg.png");
	item3_bg->setAnchorPoint(Vec2(0.5f, 1));
	item3_bg->setCapInsets(Rect(10, 10, 33, 32));
	item3_bg->setContentSize(Size(822, 179));
	item3_bg->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item3->getBottomBoundary() - 10));
	scrollview->addChild(item3_bg);

	Button* btn_item3_volume = Button::create("setting/button_input.png");
	btn_item3_volume->setScale9Enabled(true);
	btn_item3_volume->setZoomScale(0);
	btn_item3_volume->setPressedActionEnabled(false);
	btn_item3_volume->setContentSize(Size(784, 70));
	btn_item3_volume->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item3->getBottomBoundary() - 24 - 35));
	scrollview->addChild(btn_item3_volume);

	Text* text_item3_volume = Text::create(a2u("音量:"), "arial", 36);
	text_item3_volume->setAnchorPoint(Vec2(0, 0.5f));
	text_item3_volume->setPosition(Vec2(scrollview->getContentSize().width / 2 - 372, text_item3->getBottomBoundary() - 24 - 35));
	text_item3_volume->setColor(Color3B(89, 89, 89));
	scrollview->addChild(text_item3_volume);

	Slider* slider_item3_volume = Slider::create();
	slider_item3_volume->loadBarTexture("setting/bar.png");
	slider_item3_volume->loadSlidBallTextures("setting/thumb.png", "", "");
	slider_item3_volume->loadProgressBarTexture("setting/progress_bar.png");
	slider_item3_volume->setZoomScale(0);
	slider_item3_volume->setMaxPercent(100);
	slider_item3_volume->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item3->getBottomBoundary() - 24 - 35));
	//slider_item3_volume->addEventListener(CC_CALLBACK_2(UISliderTest::sliderEvent, this));
	scrollview->addChild(slider_item3_volume);

	Button* btn_item3_sound = Button::create("setting/button_input.png");
	btn_item3_sound->setScale9Enabled(true);
	btn_item3_sound->setZoomScale(0);
	btn_item3_sound->setPressedActionEnabled(false);
	btn_item3_sound->setContentSize(Size(784, 70));
	btn_item3_sound->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item3->getBottomBoundary() - 24 - 35 - 84));
	scrollview->addChild(btn_item3_sound);

	Text* text_item3_sound = Text::create(a2u("音效"), "arial", 36);
	text_item3_sound->setAnchorPoint(Vec2(0, 0.5f));
	text_item3_sound->setPosition(Vec2(scrollview->getContentSize().width / 2 - 372, text_item3->getBottomBoundary() - 24 - 35 - 84));
	text_item3_sound->setColor(Color3B(89, 89, 89));
	scrollview->addChild(text_item3_sound);

	extension::ControlSwitch* sound_switch = extension::ControlSwitch::create(Sprite::create("setting/switch_mask.png"), Sprite::create("setting/switch_on.png"), Sprite::create("setting/switch_off.png"), Sprite::create("setting/switch_thumb.png"), Label::create(), Label::create());
	sound_switch->setPosition(Vec2(scrollview->getContentSize().width / 2 + 300, text_item3->getBottomBoundary() - 24 - 35 - 84));
	scrollview->addChild(sound_switch);

	Text* text_item4 = Text::create(a2u("其他"), "arial", 30);
	text_item4->setAnchorPoint(Vec2(0, 0.5f));
	text_item4->setPosition(Vec2(scrollview->getContentSize().width / 2 - 392, btn_item3_sound->getBottomBoundary() - 40));
	text_item4->setColor(Color3B(145, 145, 145));
	scrollview->addChild(text_item4);

	Scale9Sprite* item4_bg = Scale9Sprite::create("common/item_bg.png");
	item4_bg->setAnchorPoint(Vec2(0.5f, 1));
	item4_bg->setCapInsets(Rect(10, 10, 33, 32));
	item4_bg->setContentSize(Size(822, 350));
	item4_bg->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item4->getBottomBoundary() - 10));
	scrollview->addChild(item4_bg);

	Button* btn_item4_privacy = Button::create("setting/button_input.png");
	btn_item4_privacy->setScale9Enabled(true);
	btn_item4_privacy->setZoomScale(0);
	btn_item4_privacy->setPressedActionEnabled(false);
	btn_item4_privacy->setContentSize(Size(784, 70));
	btn_item4_privacy->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item4->getBottomBoundary() - 24 - 35));
	btn_item4_privacy->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickPrivacy, this));
	scrollview->addChild(btn_item4_privacy);

	Text* text_item4_privacy = Text::create(a2u("隐私策略"), "arial", 36);
	text_item4_privacy->setAnchorPoint(Vec2(0, 0.5f));
	text_item4_privacy->setPosition(Vec2(scrollview->getContentSize().width / 2 - 372, text_item4->getBottomBoundary() - 24 - 35));
	text_item4_privacy->setColor(Color3B(89, 89, 89));
	scrollview->addChild(text_item4_privacy);

	Button* btn_item4_privacy_ex = Button::create("setting/button_right.png");
	btn_item4_privacy_ex->setPosition(Vec2(scrollview->getContentSize().width / 2 + 362, text_item4->getBottomBoundary() - 24 - 35));
	btn_item4_privacy_ex->setZoomScale(0);
	btn_item4_privacy_ex->setPressedActionEnabled(false);
	scrollview->addChild(btn_item4_privacy_ex);

	Button* btn_item4_service = Button::create("setting/button_input.png");
	btn_item4_service->setScale9Enabled(true);
	btn_item4_service->setZoomScale(0);
	btn_item4_service->setPressedActionEnabled(false);
	btn_item4_service->setContentSize(Size(784, 70));
	btn_item4_service->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item4->getBottomBoundary() - 24 - 35 - 84));
	btn_item4_service->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickService, this));
	scrollview->addChild(btn_item4_service);

	Text* text_item4_service = Text::create(a2u("服务协议"), "arial", 36);
	text_item4_service->setAnchorPoint(Vec2(0, 0.5f));
	text_item4_service->setPosition(Vec2(scrollview->getContentSize().width / 2 - 372, text_item4->getBottomBoundary() - 24 - 35 - 84));
	text_item4_service->setColor(Color3B(89, 89, 89));
	scrollview->addChild(text_item4_service);

	Button* btn_item4_service_ex = Button::create("setting/button_right.png");
	btn_item4_service_ex->setPosition(Vec2(scrollview->getContentSize().width / 2 + 362, text_item4->getBottomBoundary() - 24 - 35 - 84));
	btn_item4_service_ex->setZoomScale(0);
	btn_item4_service_ex->setPressedActionEnabled(false);
	scrollview->addChild(btn_item4_service_ex);

	Button* btn_item4_gold = Button::create("setting/button_input.png");
	btn_item4_gold->setScale9Enabled(true);
	btn_item4_gold->setZoomScale(0);
	btn_item4_gold->setPressedActionEnabled(false);
	btn_item4_gold->setContentSize(Size(784, 70));
	btn_item4_gold->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item4->getBottomBoundary() - 24 - 35 - 84 - 84));
	btn_item4_gold->addClickEventListener(CC_CALLBACK_1(SettingScene::OnClickGold, this));
	scrollview->addChild(btn_item4_gold);

	Text* text_item4_gold = Text::create(a2u("金币声明"), "arial", 36);
	text_item4_gold->setAnchorPoint(Vec2(0, 0.5f));
	text_item4_gold->setPosition(Vec2(scrollview->getContentSize().width / 2 - 372, text_item4->getBottomBoundary() - 24 - 35 - 84 - 84));
	text_item4_gold->setColor(Color3B(89, 89, 89));
	scrollview->addChild(text_item4_gold);

	Button* btn_item4_gold_ex = Button::create("setting/button_right.png");
	btn_item4_gold_ex->setPosition(Vec2(scrollview->getContentSize().width / 2 + 362, text_item4->getBottomBoundary() - 24 - 35 - 84 - 84));
	btn_item4_gold_ex->setZoomScale(0);
	btn_item4_gold_ex->setPressedActionEnabled(false);
	scrollview->addChild(btn_item4_gold_ex);

	Button* btn_item4_version = Button::create("setting/button_input.png");
	btn_item4_version->setScale9Enabled(true);
	btn_item4_version->setZoomScale(0);
	btn_item4_version->setPressedActionEnabled(false);
	btn_item4_version->setContentSize(Size(784, 70));
	btn_item4_version->setPosition(Vec2(scrollview->getContentSize().width / 2, text_item4->getBottomBoundary() - 24 - 35 - 84 - 84 - 84));
	scrollview->addChild(btn_item4_version);

	Text* text_item4_version = Text::create(a2u("当前版本:0.0.1"), "arial", 36);
	text_item4_version->setAnchorPoint(Vec2(0, 0.5f));
	text_item4_version->setPosition(Vec2(scrollview->getContentSize().width / 2 - 372, text_item4->getBottomBoundary() - 24 - 35 - 84 - 84 - 84));
	text_item4_version->setColor(Color3B(89, 89, 89));
	scrollview->addChild(text_item4_version);
    
    return true;
}

void SettingScene::OnClickBack(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOBBY_SCENE");
}

void SettingScene::OnClickFeedback(Ref* sender)
{
	//auto scene = FeedbackScene::createScene();
	//Director::getInstance()->pushScene(TransitionMoveInR::create(0.3f, scene));
}

void SettingScene::OnClickHelp(Ref* sender)
{
	//auto scene = HelpScene::createScene();
	//Director::getInstance()->pushScene(TransitionMoveInR::create(0.3f, scene));
}

void SettingScene::OnClickPasswordModify(Ref* sender)
{
	DialogPasswordMofify* layer = DialogPasswordMofify::create();
 	addChild(layer);
 
 	layer->setVisible(false);
 	layer->setScale(0, 0);
 	Action* act = Sequence::create(
 		Show::create(),
 		ScaleTo::create(0.06f, 1.05f),
 		ScaleTo::create(0.08f, 0.95f),
 		ScaleTo::create(0.08f, 1.0f),
 		nullptr);
 	layer->runAction(act);
}

void SettingScene::OnClickBindPhone(Ref* sender)
{
// 	BindPhoneLayer* layer = BindPhoneLayer::create();
// 	addChild(layer);
// 
// 	layer->setVisible(false);
// 	layer->setScale(0, 0);
// 	Action* act = Sequence::create(
// 		Show::create(),
// 		ScaleTo::create(0.06f, 1.05f),
// 		ScaleTo::create(0.08f, 0.95f),
// 		ScaleTo::create(0.08f, 1.0f),
// 		nullptr);
// 	layer->runAction(act);
}

void SettingScene::OnClickSwitchAccounts(Ref* sender)
{
	PlazaKernel::getInstance()->Disconnect();
	PlazaKernel::getInstance()->destroyInstance();
	g_global_userinfo.ResetUserInfoData();
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOGIN_SCENE");
}

void SettingScene::OnClickPrivacy(Ref* sender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	CreateWebScene("http://www.baidu.com");
#endif
}

void SettingScene::OnClickService(Ref* sender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	CreateWebScene("http://www.baidu.com");
#endif
}

void SettingScene::OnClickGold(Ref* sender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	CreateWebScene("http://www.baidu.com");
#endif
}

