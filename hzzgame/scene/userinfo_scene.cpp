
#include "userinfo_scene.h"

#include "cocostudio/CocoStudio.h"

#include "../data/global_user_info.h"
#include "../base/convert.h"
#include "../../../../global/Define.h"

Scene* UserInfoScene::createScene()
{
	auto scene = Scene::create();

	auto layer = UserInfoScene::create();

	scene->addChild(layer);

	return scene;
}

UserInfoScene::UserInfoScene()
{
}

UserInfoScene::~UserInfoScene()
{
}

bool UserInfoScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();

	auto ui = CSLoader::createNode("userinfo.csb");
	addChild(ui);

	Button* btn_back = (Button*)Helper::seekWidgetByName((Widget*)ui, "back");
	btn_back->addClickEventListener(CC_CALLBACK_1(UserInfoScene::OnClickBack, this));

	Button* btn_qihuan = (Button*)Helper::seekWidgetByName((Widget*)ui, "Button_qiehuanzhanghao");
	btn_qihuan->addClickEventListener(CC_CALLBACK_1(UserInfoScene::OnClickSwitchAccounts, this));

	
	Button* btn_xiugai = (Button*)Helper::seekWidgetByName((Widget*)ui, "Button_xiugaimima");
	btn_xiugai->addClickEventListener(CC_CALLBACK_1(UserInfoScene::OnClickPasswordModify, this));
	DWORD AA = user_data->accounts.size();
	if (user_data->accounts.size() > 100)
		btn_xiugai->setVisible(false);

	//auto inickname = (ImageView*)Helper::seekWidgetByName((Widget*)ui, "edit_nickname");
	//nickname_ = EditBox::create(inickname->getContentSize(), "common/dialog_input.png");
	//nickname_->setPosition(inickname->getPosition());
	//nickname_->setFontSize(32);
	//nickname_->setFontColor(Color3B(98, 98, 98));
	//nickname_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	//nickname_->setText(user_data->nickname.c_str());
	//addChild(nickname_);

	auto gameid = (Text*)Helper::seekWidgetByName((Widget*)ui, "gameid");
	gameid->setString(StringUtils::format("%s:%s", a2u("用户ID").c_str(), StringUtils::toString(user_data->game_id).c_str()));


	auto username = (Text*)Helper::seekWidgetByName((Widget*)ui, "text_user_name");
	username->setString(StringUtils::format("%s:%s", a2u("昵称").c_str(), StringUtils::toString(user_data->nickname).c_str()));

	auto userzhanghao = (Text*)Helper::seekWidgetByName((Widget*)ui, "text_user_zhanghao");
	userzhanghao->setString(StringUtils::format("%s:%s", a2u("账号").c_str(), StringUtils::toString(user_data->accounts).c_str()));
	

	//gender1_ = (CheckBox*)Helper::seekWidgetByName((Widget*)ui, "gender1");
	//gender1_->addClickEventListener(CC_CALLBACK_1(UserInfoScene::OnClickGender1, this));
	//gender2_ = (CheckBox*)Helper::seekWidgetByName((Widget*)ui, "gender2");
	//gender2_->addClickEventListener(CC_CALLBACK_1(UserInfoScene::OnClickGender2, this));
	//gender1_->setSelected(false);
	//gender2_->setSelected(false);
	//if (user_data->gender == GENDER_FEMALE)
	//{
	//	gender2_->setSelected(true);
	//}
	//else
	//{
	//	gender1_->setSelected(true);
	//}

	//auto userscore = (Text*)Helper::seekWidgetByName((Widget*)ui, "userscore");
	//userscore->setString(StringUtils::format("%s:%s", a2u("金币").c_str(), StringUtils::toString(user_data->user_score).c_str()));

	//auto insurescore = (Text*)Helper::seekWidgetByName((Widget*)ui, "insurescore");
	//insurescore->setString(StringUtils::format("%s:%s", a2u("金币").c_str(), StringUtils::toString(user_data->user_insure).c_str()));

	return true;
}

void UserInfoScene::OnClickBack(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOBBY_SCENE");
}

void UserInfoScene::OnClickSave(Ref* sender)
{
}

void UserInfoScene::OnClickGender1(Ref* sender)
{
	gender1_->setSelected(true);
	gender2_->setSelected(false);
}

void UserInfoScene::OnClickGender2(Ref* sender)
{
	gender1_->setSelected(false);
	gender2_->setSelected(true);
}
void UserInfoScene::OnClickSwitchAccounts(Ref* sender)
{
	UserDefault::getInstance()->setStringForKey("token", "");
	PlazaKernel::getInstance()->Disconnect();
	PlazaKernel::getInstance()->destroyInstance();
	g_global_userinfo.ResetUserInfoData();
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOGIN_SCENE");
}
void UserInfoScene::OnClickPasswordModify(Ref* sender)
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