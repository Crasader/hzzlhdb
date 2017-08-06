
#include "bank_scene.h"

#include "cocostudio/CocoStudio.h"

#include "../base/convert.h"
#include "../base/base_util.h"
#include "../data/global_user_info.h"
#include "dialog_msg_box.h"
#include "dialog_bank_password_modify.h"

Scene* BankSaveScene::createScene()
{
    auto scene = Scene::create();
    
	auto layer = BankSaveScene::create();

    scene->addChild(layer);

    return scene;
}

BankSaveScene::BankSaveScene()
{
}

BankSaveScene::~BankSaveScene()
{
}

bool BankSaveScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(BankSaveScene::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(BankSaveScene::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(BankSaveScene::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(BankSaveScene::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


	PlazaKernel::getInstance()->set_bank_delegate(this);

	auto ui = CSLoader::createNode("bank_save.csb");
	addChild(ui);

	auto edit_save_num = (ImageView*)Helper::seekWidgetByName((Widget*)ui, "edit_save_num");
	edit_save_num_ = EditBox::create(edit_save_num->getContentSize(), "bank/bank_input.png");
	edit_save_num_->setPosition(edit_save_num->getPosition());
	edit_save_num_->setPlaceHolder(a2u("存入金币数").c_str());
	edit_save_num_->setPlaceholderFontColor(Color3B(169, 169, 169));
	edit_save_num_->setFontSize(30);
	edit_save_num_->setFontColor(Color3B::BLACK);
	edit_save_num_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	edit_save_num_->setInputMode(EditBox::InputMode::NUMERIC);
	edit_save_num_->setMaxLength(9);
	addChild(edit_save_num_);

	Button* btn_back = (Button*)Helper::seekWidgetByName((Widget*)ui, "back");
	btn_back->addClickEventListener(CC_CALLBACK_1(BankSaveScene::OnClickBack, this));
	Button* btn_back0 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_0");
	btn_back0->addClickEventListener(CC_CALLBACK_1(BankSaveScene::OnClickBack, this));
	Button* btn_back1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_1");
	btn_back1->addClickEventListener(CC_CALLBACK_1(BankSaveScene::OnClickBack, this));
	Button* btn_back2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_2");
	btn_back2->addClickEventListener(CC_CALLBACK_1(BankSaveScene::OnClickBack, this));
	Button* btn_back3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_3");
	btn_back3->addClickEventListener(CC_CALLBACK_1(BankSaveScene::OnClickBack, this));

	Button* save = (Button*)Helper::seekWidgetByName((Widget*)ui, "save");
	save->addClickEventListener(CC_CALLBACK_1(BankSaveScene::OnClickSave, this));

	Button* modify_password = (Button*)Helper::seekWidgetByName((Widget*)ui, "modify_password");
	modify_password->addClickEventListener(CC_CALLBACK_1(BankSaveScene::OnClickModifyPassword, this));

	Button* get_Score = (Button*)Helper::seekWidgetByName((Widget*)ui, "getScore");
	get_Score->addClickEventListener(CC_CALLBACK_1(BankSaveScene::OnClickGetScore, this));

	Button* Transfer = (Button*)Helper::seekWidgetByName((Widget*)ui, "Transfer");
	Transfer->addClickEventListener(CC_CALLBACK_1(BankSaveScene::OnClickTransfer, this));
	
	userscore_ = (Text*)Helper::seekWidgetByName((Widget*)ui, "userscore");

	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	userscore_->setString(StringUtils::toString(user_data->user_score));
    
    return true;
}

void BankSaveScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	PACKET_HELPER_SIZE(128);
	packet.write4Byte(user_data->user_id);
	PlazaKernel::getInstance()->set_bank_delegate(this);
	PlazaKernel::getInstance()->Send(MDM_GP_BANK_SERVICE, SUB_GP_QUERY_BANK_INFO, packet.getBuffer(), packet.getPosition());

}

void BankSaveScene::onExit()
{
	Layer::onExit();
}

bool BankSaveScene::OnSocketBankService(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GP_BANK_INFO_RESULT:		// 银行资料
	{
		// 效验参数
		assert(data_size == sizeof(CMD_GP_BankInfoResult));
		if (data_size < sizeof(CMD_GP_BankInfoResult))
			return false;

		CMD_GP_BankInfoResult* bank_info_result = (CMD_GP_BankInfoResult*)data;

		GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
		user_data->user_score = bank_info_result->lUserScore;
		user_data->user_insure = bank_info_result->lUserInsure;
		userscore_->setString(StringUtils::toString(user_data->user_score));

		return true;
	}
	case SUB_GP_BANK_SUCCESS:	// 银行成功
	{
		// 效验参数
		CMD_GP_BankSuccess* bank_success = (CMD_GP_BankSuccess*)data;
		assert(data_size >= sizeof(CMD_GP_BankSuccess) - sizeof(bank_success->szDescribeString));
		if (data_size < sizeof(CMD_GP_BankSuccess) - sizeof(bank_success->szDescribeString))
			return false;

		PACKET_HELPER_DATA(data);
		auto userid = packet.read4Byte();

		GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
		user_data->user_score = packet.read8Byte();
		user_data->user_insure = packet.read8Byte();
		userscore_->setString(StringUtils::toString(user_data->user_score));

		// 显示消息
		std::string describe = packet.readString(128);
		if (describe.size() > 0)
		{
			MsgBox(describe);
		}

		return true;
	}
	case SUB_GP_BANK_FAILURE:	// 银行失败
	{
		// 效验参数
		CMD_GP_BankFailure* bank_failure = (CMD_GP_BankFailure*)data;
		assert(data_size >= (sizeof(CMD_GP_BankFailure) - sizeof(bank_failure->szDescribeString)));
		if (data_size < sizeof(CMD_GP_BankFailure) - sizeof(bank_failure->szDescribeString))
			return false;

		PACKET_HELPER_DATA(data);
		auto result = packet.read4Byte();

		// 显示消息
		std::string describe = packet.readString(128);
		if (describe.size() > 0)
		{
			MsgBox(describe);
		}

		return true;
	}

		return true;
	}

	return false;
}

void BankSaveScene::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

void BankSaveScene::OnClickSave(Ref* sender)
{
	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	PACKET_HELPER_SIZE(512);
	packet.write4Byte(user_data->user_id);
	packet.write8Byte(atoll(edit_save_num_->getText()));
	packet.writeString("LEN_MACHINE_ID", LEN_MACHINE_ID);
	PlazaKernel::getInstance()->Send(MDM_GP_BANK_SERVICE, SUB_GP_BANK_SAVE_SCORE, packet.getBuffer(), packet.getPosition());
}

void BankSaveScene::OnClickModifyPassword(Ref* sender)
{
	DialogBankPasswordMofify* layer = DialogBankPasswordMofify::create();
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

void BankSaveScene::OnClickGetScore(Ref* sender)
{
	removeFromParentAndCleanup(true);

	auto scene = BankGetScene::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void BankSaveScene::OnClickTransfer(Ref* sender)
{
	removeFromParentAndCleanup(true);

	auto scene = BankTransferScene::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

bool BankSaveScene::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}
void BankSaveScene::onTouchMoved(Touch *touch, Event *event)
{
}

void BankSaveScene::onTouchEnded(Touch *touch, Event *event)
{
}

void BankSaveScene::onTouchCancelled(Touch *touch, Event *event)
{
}

//////////////////////////////////////////////////////////////////////////

Scene* BankGetScene::createScene()
{
	auto scene = Scene::create();

	auto layer = BankGetScene::create();

	scene->addChild(layer);

	return scene;
}

BankGetScene::BankGetScene()
{
}

BankGetScene::~BankGetScene()
{
}

bool BankGetScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(BankGetScene::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(BankGetScene::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(BankGetScene::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(BankGetScene::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


	PlazaKernel::getInstance()->set_bank_delegate(this);

	auto ui = CSLoader::createNode("bank_get.csb");
	addChild(ui);

	auto edit_get_num = (ImageView*)Helper::seekWidgetByName((Widget*)ui, "edit_get_num");
	edit_get_num_ = EditBox::create(edit_get_num->getContentSize(), "bank/bank_input.png");
	edit_get_num_->setPosition(edit_get_num->getPosition());
	edit_get_num_->setPlaceHolder(a2u("取出金币数").c_str());
	edit_get_num_->setPlaceholderFontColor(Color3B(169, 169, 169));
	edit_get_num_->setFontSize(30);
	edit_get_num_->setFontColor(Color3B::BLACK);
	edit_get_num_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	edit_get_num_->setInputMode(EditBox::InputMode::NUMERIC);
	edit_get_num_->setMaxLength(9);
	addChild(edit_get_num_);

	auto edit_password = (ImageView*)Helper::seekWidgetByName((Widget*)ui, "edit_password");
	edit_password_ = EditBox::create(edit_password->getContentSize(), "bank/bank_input.png");
	edit_password_->setPosition(edit_password->getPosition());
	edit_password_->setPlaceHolder(a2u("输入密码").c_str());
	edit_password_->setPlaceholderFontColor(Color3B(169, 169, 169));
	edit_password_->setFontSize(30);
	edit_password_->setFontColor(Color3B::BLACK);
	edit_password_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	edit_password_->setMaxLength(12);
	edit_password_->setInputFlag(EditBox::InputFlag::PASSWORD);
	addChild(edit_password_);

	Button* btn_back = (Button*)Helper::seekWidgetByName((Widget*)ui, "back");
	btn_back->addClickEventListener(CC_CALLBACK_1(BankGetScene::OnClickBack, this));

	Button* btn_back0 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_0");
	btn_back0->addClickEventListener(CC_CALLBACK_1(BankGetScene::OnClickBack, this));
	Button* btn_back1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_1");
	btn_back1->addClickEventListener(CC_CALLBACK_1(BankGetScene::OnClickBack, this));
	Button* btn_back2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_2");
	btn_back2->addClickEventListener(CC_CALLBACK_1(BankGetScene::OnClickBack, this));
	Button* btn_back3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_3");
	btn_back3->addClickEventListener(CC_CALLBACK_1(BankGetScene::OnClickBack, this));

	Button* get = (Button*)Helper::seekWidgetByName((Widget*)ui, "get");
	get->addClickEventListener(CC_CALLBACK_1(BankGetScene::OnClickGet, this));

	Button* modify_password = (Button*)Helper::seekWidgetByName((Widget*)ui, "modify_password");
	modify_password->addClickEventListener(CC_CALLBACK_1(BankGetScene::OnClickModifyPassword, this));

	Button* save_Score = (Button*)Helper::seekWidgetByName((Widget*)ui, "saveScore");
	save_Score->addClickEventListener(CC_CALLBACK_1(BankGetScene::OnClickSaveScore, this));

	Button* Transfer = (Button*)Helper::seekWidgetByName((Widget*)ui, "Transfer");
	Transfer->addClickEventListener(CC_CALLBACK_1(BankGetScene::OnClickTransfer, this));

	insurescore_ = (Text*)Helper::seekWidgetByName((Widget*)ui, "insurescore");

	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	insurescore_->setString(StringUtils::toString(user_data->user_insure));

	return true;
}

void BankGetScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	PACKET_HELPER_SIZE(128);
	packet.write4Byte(user_data->user_id);
	PlazaKernel::getInstance()->set_bank_delegate(this);
	PlazaKernel::getInstance()->Send(MDM_GP_BANK_SERVICE, SUB_GP_QUERY_BANK_INFO, packet.getBuffer(), packet.getPosition());

}

void BankGetScene::onExit()
{
	Layer::onExit();
}

bool BankGetScene::OnSocketBankService(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GP_BANK_INFO_RESULT:		// 银行资料
	{
		// 效验参数
		assert(data_size == sizeof(CMD_GP_BankInfoResult));
		if (data_size < sizeof(CMD_GP_BankInfoResult))
			return false;

		CMD_GP_BankInfoResult* bank_info_result = (CMD_GP_BankInfoResult*)data;

		GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
		user_data->user_score = bank_info_result->lUserScore;
		user_data->user_insure = bank_info_result->lUserInsure;
		insurescore_->setString(StringUtils::toString(user_data->user_insure));

		return true;
	}
	case SUB_GP_BANK_SUCCESS:	// 银行成功
	{
		// 效验参数
		CMD_GP_BankSuccess* bank_success = (CMD_GP_BankSuccess*)data;
		assert(data_size >= sizeof(CMD_GP_BankSuccess) - sizeof(bank_success->szDescribeString));
		if (data_size < sizeof(CMD_GP_BankSuccess) - sizeof(bank_success->szDescribeString))
			return false;

		PACKET_HELPER_DATA(data);
		auto userid = packet.read4Byte();

		GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
		user_data->user_score = packet.read8Byte();
		user_data->user_insure = packet.read8Byte();
		insurescore_->setString(StringUtils::toString(user_data->user_insure));

		// 显示消息
		std::string describe = packet.readString(128);
		if (describe.size() > 0)
		{
			MsgBox(describe);
		}

		return true;
	}
	case SUB_GP_BANK_FAILURE:	// 银行失败
	{
		// 效验参数
		CMD_GP_BankFailure* bank_failure = (CMD_GP_BankFailure*)data;
		assert(data_size >= (sizeof(CMD_GP_BankFailure) - sizeof(bank_failure->szDescribeString)));
		if (data_size < sizeof(CMD_GP_BankFailure) - sizeof(bank_failure->szDescribeString))
			return false;

		PACKET_HELPER_DATA(data);
		auto result = packet.read4Byte();

		// 显示消息
		std::string describe = packet.readString(128);
		if (describe.size() > 0)
		{
			MsgBox(describe);
		}

		return true;
	}

	return true;
	}

	return false;
}

void BankGetScene::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
	//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOBBY_SCENE");
}

void BankGetScene::OnClickGet(Ref* sender)
{
	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	PACKET_HELPER_SIZE(512);
	packet.write4Byte(user_data->user_id);
	packet.write8Byte(atoll(edit_get_num_->getText()));
	packet.writeString(BaseUtil::MD5Encrypt(edit_password_->getText()), LEN_MD5);
	packet.writeString("LEN_MACHINE_ID", LEN_MACHINE_ID);
	PlazaKernel::getInstance()->Send(MDM_GP_BANK_SERVICE, SUB_GP_BANK_GET_SCORE, packet.getBuffer(), packet.getPosition());
}

void BankGetScene::OnClickModifyPassword(Ref* sender)
{
	DialogBankPasswordMofify* layer = DialogBankPasswordMofify::create();
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

void BankGetScene::OnClickSaveScore(Ref* sender)
{
	removeFromParentAndCleanup(true);

	auto scene = BankSaveScene::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void BankGetScene::OnClickTransfer(Ref* sender)
{
	removeFromParentAndCleanup(true);

	auto scene = BankTransferScene::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

bool BankGetScene::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}
void BankGetScene::onTouchMoved(Touch *touch, Event *event)
{
}

void BankGetScene::onTouchEnded(Touch *touch, Event *event)
{
}

void BankGetScene::onTouchCancelled(Touch *touch, Event *event)
{
}

//////////////////////////////////////////////////////////////////////////

Scene* BankTransferScene::createScene()
{
	auto scene = Scene::create();

	auto layer = BankTransferScene::create();

	scene->addChild(layer);

	return scene;
}

BankTransferScene::BankTransferScene()
{
}

BankTransferScene::~BankTransferScene()
{
}

bool BankTransferScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(BankTransferScene::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(BankTransferScene::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(BankTransferScene::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(BankTransferScene::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


	PlazaKernel::getInstance()->set_bank_delegate(this);

	auto ui = CSLoader::createNode("bank_Transfer.csb");
	addChild(ui);

	auto edit_id = (ImageView*)Helper::seekWidgetByName((Widget*)ui, "edit_id");
	edit_id_ = EditBox::create(edit_id->getContentSize(), "bank/bank_input.png");
	edit_id_->setPosition(edit_id->getPosition());
	edit_id_->setPlaceHolder(a2u("赠送ID").c_str());
	edit_id_->setPlaceholderFontColor(Color3B(169, 169, 169));
	edit_id_->setFontSize(30);
	edit_id_->setFontColor(Color3B::BLACK);
	edit_id_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	edit_id_->setInputMode(EditBox::InputMode::NUMERIC);
	edit_id_->setMaxLength(9);
	addChild(edit_id_);

	auto edit_get_num = (ImageView*)Helper::seekWidgetByName((Widget*)ui, "edit_get_num");
	edit_get_num_ = EditBox::create(edit_get_num->getContentSize(), "bank/bank_input.png");
	edit_get_num_->setPosition(edit_get_num->getPosition());
	edit_get_num_->setPlaceHolder(a2u("赠送金币数").c_str());
	edit_get_num_->setPlaceholderFontColor(Color3B(169, 169, 169));
	edit_get_num_->setFontSize(30);
	edit_get_num_->setFontColor(Color3B::BLACK);
	edit_get_num_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	edit_get_num_->setInputMode(EditBox::InputMode::NUMERIC);
	edit_get_num_->setMaxLength(9);
	addChild(edit_get_num_);

	auto edit_password = (ImageView*)Helper::seekWidgetByName((Widget*)ui, "edit_password");
	edit_password_ = EditBox::create(edit_password->getContentSize(), "bank/bank_input.png");
	edit_password_->setPosition(edit_password->getPosition());
	edit_password_->setPlaceHolder(a2u("输入密码").c_str());
	edit_password_->setPlaceholderFontColor(Color3B(169, 169, 169));
	edit_password_->setFontSize(30);
	edit_password_->setFontColor(Color3B::BLACK);
	edit_password_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	edit_password_->setMaxLength(12);
	edit_password_->setInputFlag(EditBox::InputFlag::PASSWORD);
	addChild(edit_password_);

	Button* btn_back = (Button*)Helper::seekWidgetByName((Widget*)ui, "back");
	btn_back->addClickEventListener(CC_CALLBACK_1(BankTransferScene::OnClickBack, this));

	Button* btn_back0 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_0");
	btn_back0->addClickEventListener(CC_CALLBACK_1(BankTransferScene::OnClickBack, this));
	Button* btn_back1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_1");
	btn_back1->addClickEventListener(CC_CALLBACK_1(BankTransferScene::OnClickBack, this));
	Button* btn_back2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_2");
	btn_back2->addClickEventListener(CC_CALLBACK_1(BankTransferScene::OnClickBack, this));
	Button* btn_back3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "close_3");
	btn_back3->addClickEventListener(CC_CALLBACK_1(BankTransferScene::OnClickBack, this));

	Button* modify_password = (Button*)Helper::seekWidgetByName((Widget*)ui, "modify_password");
	modify_password->addClickEventListener(CC_CALLBACK_1(BankTransferScene::OnClickModifyPassword, this));

	Button* save_Score = (Button*)Helper::seekWidgetByName((Widget*)ui, "saveScore");
	save_Score->addClickEventListener(CC_CALLBACK_1(BankTransferScene::OnClickSaveScore, this));

	Button* get_Score = (Button*)Helper::seekWidgetByName((Widget*)ui, "getScore");
	get_Score->addClickEventListener(CC_CALLBACK_1(BankTransferScene::OnClickGetScore, this));

	Button* query = (Button*)Helper::seekWidgetByName((Widget*)ui, "query");
	query->addClickEventListener(CC_CALLBACK_1(BankTransferScene::OnClickQuery, this));

	Button* transfer = (Button*)Helper::seekWidgetByName((Widget*)ui, "transfer");
	transfer->addClickEventListener(CC_CALLBACK_1(BankTransferScene::OnClickTransfer, this));

	insurescore_ = (Text*)Helper::seekWidgetByName((Widget*)ui, "insurescore");
	Text_nickname = (Text*)Helper::seekWidgetByName((Widget*)ui, "Text_nickname");

	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	insurescore_->setString(StringUtils::toString(user_data->user_insure));

	return true;
}

void BankTransferScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	PACKET_HELPER_SIZE(128);
	packet.write4Byte(user_data->user_id);
	PlazaKernel::getInstance()->set_bank_delegate(this);
	PlazaKernel::getInstance()->Send(MDM_GP_BANK_SERVICE, SUB_GP_QUERY_BANK_INFO, packet.getBuffer(), packet.getPosition());

}

void BankTransferScene::onExit()
{
	Layer::onExit();
}

bool BankTransferScene::OnSocketBankService(uint16 sub_cmd_id, void* data, uint16 data_size)
{
	switch (sub_cmd_id)
	{
	case SUB_GP_BANK_INFO_RESULT:		// 银行资料
	{
		// 效验参数
		assert(data_size == sizeof(CMD_GP_BankInfoResult));
		if (data_size < sizeof(CMD_GP_BankInfoResult))
			return false;

		CMD_GP_BankInfoResult* bank_info_result = (CMD_GP_BankInfoResult*)data;

		GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
		user_data->user_score = bank_info_result->lUserScore;
		user_data->user_insure = bank_info_result->lUserInsure;
		insurescore_->setString(StringUtils::toString(user_data->user_insure));

		return true;
	}
	case SUB_GP_BANK_SUCCESS:	// 银行成功
	{
		// 效验参数
		CMD_GP_BankSuccess* bank_success = (CMD_GP_BankSuccess*)data;
		assert(data_size >= sizeof(CMD_GP_BankSuccess) - sizeof(bank_success->szDescribeString));
		if (data_size < sizeof(CMD_GP_BankSuccess) - sizeof(bank_success->szDescribeString))
			return false;

		PACKET_HELPER_DATA(data);
		auto userid = packet.read4Byte();

		GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
		user_data->user_score = packet.read8Byte();
		user_data->user_insure = packet.read8Byte();
		insurescore_->setString(StringUtils::toString(user_data->user_insure));

		// 显示消息
		std::string describe = packet.readString(128);
		if (describe.size() > 0)
		{
			MsgBox(describe);
		}

		return true;
	}
	case SUB_GP_BANK_FAILURE:	// 银行失败
	{
		// 效验参数
		CMD_GP_BankFailure* bank_failure = (CMD_GP_BankFailure*)data;
		assert(data_size >= (sizeof(CMD_GP_BankFailure) - sizeof(bank_failure->szDescribeString)));
		if (data_size < sizeof(CMD_GP_BankFailure) - sizeof(bank_failure->szDescribeString))
			return false;

		PACKET_HELPER_DATA(data);
		auto result = packet.read4Byte();

		// 显示消息
		std::string describe = packet.readString(128);
		if (describe.size() > 0)
		{
			MsgBox(describe);
		}

		return true;
	}
	case SUB_GP_TRANSFER_USERINFO_RESULT:	// 转帐用户信息结果
	{
		// 效验参数
		assert(data_size == sizeof(CMD_GP_TransferUserInfoResult));
		if (data_size < sizeof(CMD_GP_TransferUserInfoResult))
			return false;
		PACKET_HELPER_DATA(data);
		auto dwTargetGameID = packet.read4Byte();
		Text_nickname->setString(packet.readString(LEN_NICKNAME));
	}

	return true;
	}

	return false;
}

void BankTransferScene::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
	//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOBBY_SCENE");
}

void BankTransferScene::OnClickModifyPassword(Ref* sender)
{
	DialogBankPasswordMofify* layer = DialogBankPasswordMofify::create();
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

void BankTransferScene::OnClickSaveScore(Ref* sender)
{
	removeFromParentAndCleanup(true);

	auto scene = BankSaveScene::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void BankTransferScene::OnClickGetScore(Ref* sender)
{
	removeFromParentAndCleanup(true);

	auto scene = BankGetScene::createScene();
	Director::getInstance()->getRunningScene()->addChild(scene);
}

void BankTransferScene::OnClickQuery(Ref* sender)
{
	PACKET_HELPER_SIZE(512);
	packet.writeByte(0);
	packet.writeString(edit_id_->getText(), LEN_NICKNAME);
	PlazaKernel::getInstance()->Send(MDM_GP_BANK_SERVICE, SUB_GP_QUERY_TRANSFER_USERINFO, packet.getBuffer(), packet.getPosition());
}

void BankTransferScene::OnClickTransfer(Ref* sender)
{
	std::string transferID = edit_id_->getText();
	if (0 == transferID.length())
	{
		MsgBox(a2u("输入要赠送的玩家ID！"));
		return;
	}

	LONGLONG lTransferScore = atoll(edit_get_num_->getText());
	if (0 == lTransferScore)
	{
		MsgBox(a2u("赠送的金币数量不能为空，请重新输入欢乐豆数量！"));
		return;
	}

	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();

	if (user_data->user_insure < lTransferScore)
	{
		MsgBox(a2u("您银行金币的数目余额不足，请重新输入金币数量"));
		return;
	}

	PACKET_HELPER_SIZE(512);
	packet.write4Byte(user_data->user_id);
	packet.writeByte(0);
	packet.write8Byte(lTransferScore);
	packet.writeString(BaseUtil::MD5Encrypt(edit_password_->getText()), LEN_MD5);
	packet.writeString(transferID, LEN_NICKNAME);
	PlazaKernel::getInstance()->Send(MDM_GP_BANK_SERVICE, SUB_GP_BANK_TRANSFER_SCORE, packet.getBuffer(), packet.getPosition());
}

bool BankTransferScene::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}
void BankTransferScene::onTouchMoved(Touch *touch, Event *event)
{
}

void BankTransferScene::onTouchEnded(Touch *touch, Event *event)
{
}

void BankTransferScene::onTouchCancelled(Touch *touch, Event *event)
{
}