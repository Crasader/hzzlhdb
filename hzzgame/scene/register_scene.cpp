
#include "register_scene.h"

#include "cocostudio/CocoStudio.h"

#include "../common/UIHelperEx.h"
#include "../base/base_util.h"
#include "../base/convert.h"
#include "../kernel/plaza_kernel.h"
#include "dialog_waiting.h"
#include "dialog_msg_box.h"
#include "../data/global_user_info.h"

Scene* RegisterScene::createScene()
{
    auto scene = Scene::create();
    
	auto layer = RegisterScene::create();

    scene->addChild(layer);

    return scene;
}

RegisterScene::RegisterScene()
{
}

RegisterScene::~RegisterScene()
{
}

bool RegisterScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto reg = CSLoader::createNode("register.csb");
	addChild(reg);

	quick_layer_ = (Node*)HelperEx::seekNodeByName(reg, "register1");
	normal_layer_ = (Node*)HelperEx::seekNodeByName(reg, "register2");

	auto back = (Button*)HelperEx::seekNodeByName(reg, "back");
	back->addClickEventListener(CC_CALLBACK_1(RegisterScene::OnClickBack, this));

	auto quick = (Button*)HelperEx::seekNodeByName(reg, "quick");
	quick->addClickEventListener(CC_CALLBACK_1(RegisterScene::OnClickQuick, this));

	auto normal = (Button*)HelperEx::seekNodeByName(reg, "normal");
	normal->addClickEventListener(CC_CALLBACK_1(RegisterScene::OnClickNormal, this));

	auto normal_ok = (Button*)HelperEx::seekNodeByName(normal_layer_, "confirm");
	normal_ok->addClickEventListener(CC_CALLBACK_1(RegisterScene::OnClickNormalOk, this));

	auto telphone = (ImageView*)HelperEx::seekNodeByName(normal_layer_, "telphone");
	normal_telphone_ = EditBox::create(telphone->getContentSize(), "register/register_input.png");
	normal_telphone_->setPosition(telphone->getPosition());
	normal_telphone_->setPlaceHolder(a2u("请输入手机").c_str());
	normal_telphone_->setPlaceholderFontColor(Color3B(169, 169, 169));
	normal_telphone_->setPlaceholderFontSize(32);
	normal_telphone_->setFontSize(32);
	normal_telphone_->setFontColor(Color3B::BLACK);
	normal_telphone_->setInputMode(EditBox::InputMode::NUMERIC);
	normal_telphone_->setMaxLength(11);
	normal_layer_->addChild(normal_telphone_);
	

	auto naccounts = (ImageView*)HelperEx::seekNodeByName(normal_layer_, "accounts");
	normal_accounts_ = EditBox::create(naccounts->getContentSize(), "register/register_input.png");
	normal_accounts_->setPosition(naccounts->getPosition());
	normal_accounts_->setPlaceHolder(a2u("请输入账号").c_str());
	normal_accounts_->setPlaceholderFontColor(Color3B(169, 169, 169));
	normal_accounts_->setPlaceholderFontSize(32);
	normal_accounts_->setFontSize(32);
	normal_accounts_->setFontColor(Color3B::BLACK);
	normal_accounts_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	normal_accounts_->setMaxLength(12);
	normal_layer_->addChild(normal_accounts_);

	auto naplayername = (ImageView*)HelperEx::seekNodeByName(normal_layer_, "playername");
	normal_acplayername_ = EditBox::create(naplayername->getContentSize(), "register/register_input.png");
	normal_acplayername_->setPosition(naplayername->getPosition());
	normal_acplayername_->setPlaceHolder(a2u("请输入昵称").c_str());
	normal_acplayername_->setPlaceholderFontColor(Color3B(169, 169, 169));
	normal_acplayername_->setPlaceholderFontSize(32);
	normal_acplayername_->setFontSize(32);
	normal_acplayername_->setFontColor(Color3B::BLACK);
	normal_acplayername_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	normal_acplayername_->setMaxLength(10);
	normal_layer_->addChild(normal_acplayername_);


	auto npassword = (ImageView*)HelperEx::seekNodeByName(normal_layer_, "password");
	normal_password_ = EditBox::create(npassword->getContentSize(), "register/register_input.png");
	normal_password_->setPosition(npassword->getPosition());
	normal_password_->setPlaceHolder(a2u("请输入密码").c_str());
	normal_password_->setPlaceholderFontColor(Color3B(169, 169, 169));
	normal_password_->setPlaceholderFontSize(32);
	normal_password_->setFontSize(32);
	normal_password_->setFontColor(Color3B::BLACK);
	normal_password_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	normal_password_->setMaxLength(12);
	normal_password_->setInputFlag(EditBox::InputFlag::PASSWORD);
	normal_layer_->addChild(normal_password_);

	auto npassword2 = (ImageView*)HelperEx::seekNodeByName(normal_layer_, "password2");
	normal_password2_ = EditBox::create(npassword2->getContentSize(), "register/register_input.png");
	normal_password2_->setPosition(npassword2->getPosition());
	normal_password2_->setPlaceHolder(a2u("请确认密码").c_str());
	normal_password2_->setPlaceholderFontColor(Color3B(169, 169, 169));
	normal_password2_->setPlaceholderFontSize(32);
	normal_password2_->setFontSize(32);
	normal_password2_->setFontColor(Color3B::BLACK);
	normal_password2_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	normal_password2_->setMaxLength(12);
	normal_password2_->setInputFlag(EditBox::InputFlag::PASSWORD);
	normal_layer_->addChild(normal_password2_);

	//auto ncode = (ImageView*)HelperEx::seekNodeByName(normal_layer_, "code");
	//EditBox* edit_code = EditBox::create(ncode->getContentSize(), "register/register_input.png");
	//edit_code->setPosition(ncode->getPosition());
	//edit_code->setAnchorPoint(Vec2(0, 0.5f));
	//edit_code->setPlaceHolder(a2u("请输入验证码").c_str());
	//edit_code->setPlaceholderFontColor(Color3B(169, 169, 169));
	//edit_code->setPlaceholderFontSize(32);
	//edit_code->setFontSize(32);
	//edit_code->setFontColor(Color3B::BLACK);
	//edit_code->setInputMode(EditBox::InputMode::SINGLE_LINE);
	//normal_layer_->addChild(edit_code);
    
    return true;
}

void RegisterScene::OnClickBack(Ref* sender)
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOGIN_SCENE");
}

void RegisterScene::OnClickQuick(Ref* sender)
{
	quick_layer_->setVisible(true);
	normal_layer_->setVisible(false);
}

void RegisterScene::OnClickNormal(Ref* sender)
{
	quick_layer_->setVisible(false);
	normal_layer_->setVisible(true);
}

void RegisterScene::OnClickNormalOk(Ref* sender)
{
	//return;
	std::string telphone = normal_telphone_->getText();
	std::string accounts = normal_accounts_->getText();
	std::string password = normal_password_->getText();

	std::string acplayername = normal_acplayername_->getText();
	std::string password2 = normal_password2_->getText();

	if (telphone.empty())
	{
		MsgBox(a2u("请输入电话"));
		return;
	}
	
	if (accounts.empty())
	{
		MsgBox(a2u("请输入账号"));
		return;
	}

	if (acplayername.empty())
	{
		MsgBox(a2u("请输入昵称"));
		return;
	}

	if (password.empty())
	{
		MsgBox(a2u("请输入密码"));
		return;
	}
	if (password != password2)
	{
		MsgBox(a2u("密码不一致"));
		return;
	}
	
	DlgWaiting(a2u("正在注册中..."));
	//if (!PlazaKernel::getInstance()->Connect("dl.hzzgame.com", 8300))
	if (!PlazaKernel::getInstance()->Connect("dl2.hzzgame.com", 8300))
	{
		RemoveDlgWaiting();
		MsgBox(a2u("服务器连接失败"));
		return;
	}
	else
	{
		PACKET_HELPER_SIZE(SOCKET_TCP_PACKET);
		packet.writeString("EBCD", LEN_MACHINE_ID);
		packet.writeByte(0);
		packet.writeString(normal_accounts_->getText(), LEN_ACCOUNTS);
		packet.writeString(normal_acplayername_->getText(), LEN_NICKNAME);
		packet.writeString(BaseUtil::MD5Encrypt(normal_password_->getText()), LEN_PASSWORD);
		packet.writeString(normal_telphone_->getText(), LEN_MOBILE_PHONE);
		g_global_userinfo.GetGlobalUserData()->password = BaseUtil::MD5Encrypt(normal_password_->getText());
		PlazaKernel::getInstance()->Send(MDM_GP_LOGON, SUB_GP_REGISTER_ACCOUNTS, packet.getBuffer(), packet.getPosition());

		//saveFile("Last.txt");
	}
}

/* 储存内容到文件 */
bool RegisterScene::saveFile(string pFileName)
{
	//第一获取储存的文件路径  
	string path = CCFileUtils::getInstance()->getWritablePath() + pFileName;
	CCLOG("wanna save file path = %s", path.c_str());

	//创建一个文件指针  
	//路径、模式  
	FILE* file = fopen(path.c_str(), "w");
	if (file) {
		//fputs(pContent, file);
		fputs(normal_accounts_->getText(), file);
		fputs("\n", file);
		fputs(BaseUtil::MD5Encrypt(normal_password_->getText()).c_str(), file);
		fclose(file);

		return true;
	}
	else
		CCLOG("save file error.");

	return false;
}
