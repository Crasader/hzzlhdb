
#include "login_scene.h"
#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../kernel/plaza_kernel.h"
#include "dialog_waiting.h"
#include "../base/convert.h"
#include "../base/base_util.h"
#include "dialog_msg_box.h"
#include "../data/global_user_info.h"
#include "../platform/coplatform.h"
#include "base/JsonAide.h"

static LoginScene * g_pLoginScene = nullptr;

LoginScene * LoginScene::getInstance()
{
	return g_pLoginScene;
}

Scene* LoginScene::createScene()
{
	auto scene = Scene::create();

	auto layer = LoginScene::create();

	scene->addChild(layer);

	return scene;
}

LoginScene::LoginScene()
{
	g_pLoginScene = this;
}

LoginScene::~LoginScene()
{
}

bool LoginScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto login = CSLoader::createNode("login.csb");
	addChild(login);

	login1_ = (Node*)HelperEx::seekNodeByName(login, "login1");
	login2_ = (Node*)HelperEx::seekNodeByName(login, "login2");

	btn_back_ = (Button*)HelperEx::seekNodeByName(login, "login_back");
	btn_back_->addClickEventListener(CC_CALLBACK_1(LoginScene::OnClickBack, this));



	login_startgame = (Button*)HelperEx::seekNodeByName(login1_, "login_start_game");
	login_startgame->addClickEventListener(CC_CALLBACK_1(LoginScene::OnClickStartGame, this));

	//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	//	login_startgame->setVisible(false);
	//	login_startgame->setEnabled(false);
	//#else
	//#endif

	Button* login_register = (Button*)HelperEx::seekNodeByName(login1_, "login_register");
	login_register->addClickEventListener(CC_CALLBACK_1(LoginScene::OnClickRegister, this));

	Button* login_accounts = (Button*)HelperEx::seekNodeByName(login1_, "login_accounts");
	login_accounts->addClickEventListener(CC_CALLBACK_1(LoginScene::OnClickAccounts, this));

	ImageView* login_input_1 = (ImageView*)HelperEx::seekNodeByName(login2_, "login_input_1");
	ImageView* login_input_2 = (ImageView*)HelperEx::seekNodeByName(login2_, "login_input_2");
	login2_accounts_ = EditBox::create(login_input_1->getContentSize(), "login/login_input.png");
	login2_accounts_->setPosition(login_input_1->getPosition());
	login2_accounts_->setPlaceHolder(a2u("帐号").c_str());
	login2_accounts_->setPlaceholderFontColor(Color3B(169, 169, 169));
	login2_accounts_->setPlaceholderFontSize(36);
	login2_accounts_->setFontSize(36);
	login2_accounts_->setFontColor(Color3B::BLACK);
	login2_accounts_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	login2_accounts_->setMaxLength(12);
	login2_->addChild(login2_accounts_);

	login2_password_ = EditBox::create(login_input_2->getContentSize(), "login/login_input.png");
	login2_password_->setPosition(login_input_2->getPosition());
	login2_password_->setPlaceHolder(a2u("密码").c_str());
	login2_password_->setPlaceholderFontColor(Color3B(169, 169, 169));
	login2_password_->setPlaceholderFontSize(36);
	login2_password_->setFontSize(36);
	login2_password_->setFontColor(Color3B::BLACK);
	login2_password_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	login2_password_->setMaxLength(12);
	login2_password_->setInputFlag(EditBox::InputFlag::PASSWORD);
	login2_->addChild(login2_password_);

	Button* login_ok = (Button*)HelperEx::seekNodeByName(login2_, "login_ok");
	login_ok->addClickEventListener(CC_CALLBACK_1(LoginScene::OnClickLoginOk, this));



	return true;
}

void LoginScene::OnClickStartGame(Ref* sender)
{
	//platformAction("{\"act\":400 ,\"propName\":\"1分钱商品\",\"propInfo\":\"10\",\"propPice\":\"100\"}");
	//platformAction("{\"act\":500 }");
	login_startgame->setEnabled(false);
	this->runAction(Sequence::create(DelayTime::create(5), CallFunc::create([=]{
		login_startgame->setEnabled(true);
	}), nullptr));

	PlazaKernel::getInstance()->Disconnect();
	PlazaKernel::getInstance()->destroyInstance();
	g_global_userinfo.ResetUserInfoData();
	DlgWaiting(a2u("正在登录..."));

	//if (!PlazaKernel::getInstance()->Connect("dl.hzzgame.com", 8300))
	//if (!PlazaKernel::getInstance()->Connect("192.168.1.132", 8300))
	//if (!PlazaKernel::getInstance()->Connect("dl2.hzzgame.com", 8300))
	if (!PlazaKernel::getInstance()->Connect("127.0.0.1", 8300))
	{
		RemoveDlgWaiting();
		MsgBox(a2u("服务器连接失败"));
		return;
	}
	else
	{
		//if (!getFileByName("Last.txt"))
		//{
		//	RemoveDlgWaiting();
		//	MsgBox(a2u("您的帐号不存在或者密码输入有误，请查证后再次尝试登录！"));
		//}


		PACKET_HELPER_SIZE(SOCKET_TCP_PACKET);
		packet.writeString("EBCD", LEN_MACHINE_ID);
		//packet.writeString(BaseUtil::MD5Encrypt(login2_password_->getText()), LEN_PASSWORD);
		packet.writeString("oO7ejw2UUdb78YqutaiMGDFziM3A", LEN_ACCOUNTS);
		packet.writeString("红蜘蛛测试", LEN_NICKNAME);
		packet.writeString("http://wx.qlogo.cn/mmopen/8tScjJibaI7yRxvrp8jwIRxLG5YncSaRaqpddNBLJxW1VrfDFjMQPrsn9bAA4OywWVKAibHAl9cpE2j60Xvz6O0CyrKdXug3h2/0", 200);


		PlazaKernel::getInstance()->Send(MDM_GP_LOGON, SUB_GP_LOGON_WX_ACCOUNTS, packet.getBuffer(), packet.getPosition());

		g_global_userinfo.GetGlobalUserData()->password = "111111";
	}
}

void LoginScene::OnClickRegister(Ref* sender)
{
	//platformAction("{\"act\":400 ,\"propName\":\"1分钱商品\",\"propInfo\":\"测试商品\",\"propPice\":\"0.01\"}");
	//return;
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_REGISTER_SCENE");
}

void LoginScene::OnClickAccounts(Ref* sender)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto act1 = MoveBy::create(0.2f, Vec2(0, visibleSize.height));
	login1_->runAction(Sequence::create(act1, Hide::create(), nullptr));

	auto act2 = MoveBy::create(0.2f, Vec2(0, visibleSize.height));
	login2_->setPosition(Vec2(0, -visibleSize.height));
	login2_->runAction(Sequence::create(Show::create(), act2, CallFunc::create([=](){ btn_back_->setVisible(true); }), nullptr));
}

void LoginScene::OnClickBack(Ref* sender)
{
	//btn_back_->setVisible(false);
	btn_back_->setVisible(true);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto act1 = MoveBy::create(0.2f, Vec2(0, -visibleSize.height));
	login1_->setPosition(Vec2(0, visibleSize.height));
	login1_->runAction(Sequence::create(Show::create(), act1, nullptr));

	auto act2 = MoveBy::create(0.2f, Vec2(0, -visibleSize.height));
	login2_->runAction(Sequence::create(act2, Hide::create(), nullptr));
}

void LoginScene::OnClickLoginOk(Ref* sender)
{
	DlgWaiting(a2u("正在登录..."));

	//if (!PlazaKernel::getInstance()->Connect("dl.hzzgame.com", 8300))
	//if (!PlazaKernel::getInstance()->Connect("192.168.1.132", 8300))
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
		packet.writeString(BaseUtil::MD5Encrypt(login2_password_->getText()), LEN_PASSWORD);
		packet.writeString(login2_accounts_->getText(), LEN_ACCOUNTS);
		g_global_userinfo.GetGlobalUserData()->password = BaseUtil::MD5Encrypt(login2_password_->getText());
		PlazaKernel::getInstance()->Send(MDM_GP_LOGON, SUB_GP_LOGON_ACCOUNTS, packet.getBuffer(), packet.getPosition());
		//saveFile("Last.txt");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#else
		saveFile("Last.txt");
#endif
	}
}




bool LoginScene::getFileByName(string pFileName)
{
	//第一先获取文件的路径  
	string path = CCFileUtils::getInstance()->getWritablePath() + pFileName;
	CCLOG("path = %s", path.c_str());

	//创建一个文件指针  
	FILE* file = fopen(path.c_str(), "r");

	if (file) {
		char* buf;  //要获取的字符串  
		int len;    //获取的长度  
		/*获取长度*/
		fseek(file, 0, SEEK_END);   //移到尾部  
		len = ftell(file);          //提取长度  
		rewind(file);               //回归原位  
		CCLOG("count the file content len = %d", len);
		//分配buf空间  
		buf = (char*)malloc(sizeof(char) * len + 1);
		if (!buf) {
			CCLOG("malloc space is not enough.");
			return false;
		}

		//读取文件  
		//读取进的buf，单位大小，长度，文件指针  
		int rLen = fread(buf, sizeof(char), len, file);
		buf[rLen] = '\0';
		CCLOG("has read Length = %d", rLen);
		CCLOG("has read content = %s", buf);

		//string result = buf;
		fclose(file);

		string str = buf;
		int n = 0;
		n = str.find('\n', n);
		string strAccounts = str.substr(0, n);
		string strPass = str.substr(n + 1);

		PACKET_HELPER_SIZE(SOCKET_TCP_PACKET);
		packet.writeString("EBCD", LEN_MACHINE_ID);
		packet.writeString(strPass, LEN_PASSWORD);
		packet.writeString(strAccounts, LEN_ACCOUNTS);
		g_global_userinfo.GetGlobalUserData()->password = strPass;
		PlazaKernel::getInstance()->Send(MDM_GP_LOGON, SUB_GP_LOGON_ACCOUNTS, packet.getBuffer(), packet.getPosition());

		free(buf);

		return true;
	}
	else
		CCLOG("open file error.");

	return false;
}

bool LoginScene::saveFile(string pFileName)
{
	//第一获取储存的文件路径  
	string path = CCFileUtils::getInstance()->getWritablePath() + pFileName;
	CCLOG("wanna save file path = %s", path.c_str());

	//创建一个文件指针  
	//路径、模式  
	FILE* file = fopen(path.c_str(), "w");
	if (file) {
		//fputs(pContent, file);
		fputs(login2_accounts_->getText(), file);
		fputs("\n", file);
		fputs(BaseUtil::MD5Encrypt(login2_password_->getText()).c_str(), file);
		fclose(file);

		return true;
	}
	else
		CCLOG("save file error.");

	return false;
}
