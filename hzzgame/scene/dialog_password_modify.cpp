
#include "dialog_password_modify.h"

#include "cocostudio/CocoStudio.h"

#include "../base/convert.h"
#include "../kernel/plaza_kernel.h"
#include "../data/global_user_info.h"
#include "../base/base_util.h"
#include "dialog_msg_box.h"

DialogPasswordMofify::DialogPasswordMofify()
{
}

DialogPasswordMofify::~DialogPasswordMofify()
{
}

bool DialogPasswordMofify::init()
{
	if (!Dialog::init())
	{
		return false;
	}

	auto ui = CSLoader::createNode("password_modify.csb");
	addChild(ui);

	auto password1 = (ImageView*)Helper::seekWidgetByName((Widget*)ui, "password1");
	password1_ = EditBox::create(password1->getContentSize(), "common/dialog_input.png");
	password1_->setPosition(password1->getPosition());
	password1_->setPlaceHolder(a2u("ÇëÊäÈë¾ÉµÇÂ¼ÃÜÂë").c_str());
	password1_->setPlaceholderFontColor(Color3B(169, 169, 169));
	password1_->setPlaceholderFontSize(36);
	password1_->setFontSize(36);
	password1_->setFontColor(Color3B::BLACK);
	password1_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	password1_->setMaxLength(12);
	password1_->setInputFlag(EditBox::InputFlag::PASSWORD);
	addChild(password1_);

	auto password2 = (ImageView*)Helper::seekWidgetByName((Widget*)ui, "password2");
	password2_ = EditBox::create(password2->getContentSize(), "common/dialog_input.png");
	password2_->setPosition(password2->getPosition());
	password2_->setPlaceHolder(a2u("ÇëÊäÈëÐÂµÇÂ¼ÃÜÂë").c_str());
	password2_->setPlaceholderFontColor(Color3B(169, 169, 169));
	password2_->setPlaceholderFontSize(36);
	password2_->setFontSize(36);
	password2_->setFontColor(Color3B::BLACK);
	password2_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	password2_->setMaxLength(12);
	password2_->setInputFlag(EditBox::InputFlag::PASSWORD);
	addChild(password2_);

	auto password3 = (ImageView*)Helper::seekWidgetByName((Widget*)ui, "password3");
	password3_ = EditBox::create(password3->getContentSize(), "common/dialog_input.png");
	password3_->setPosition(password3->getPosition());
	password3_->setPlaceHolder(a2u("ÇëÔÙ´ÎÊäÈëÐÂµÇÂ¼ÃÜÂë").c_str());
	password3_->setPlaceholderFontColor(Color3B(169, 169, 169));
	password3_->setPlaceholderFontSize(36);
	password3_->setFontSize(36);
	password3_->setFontColor(Color3B::BLACK);
	password3_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	password3_->setMaxLength(12);
	password3_->setInputFlag(EditBox::InputFlag::PASSWORD);
	addChild(password3_);

	auto close = (Button*)Helper::seekWidgetByName((Widget*)ui, "close");
	close->addClickEventListener(CC_CALLBACK_1(DialogPasswordMofify::OnClickClose, this));

	auto ok = (Button*)Helper::seekWidgetByName((Widget*)ui, "confirm");
	ok->addClickEventListener(CC_CALLBACK_1(DialogPasswordMofify::OnClickOk, this));

	return true;
}

void DialogPasswordMofify::OnClickClose(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

void DialogPasswordMofify::OnClickOk(Ref* sender)
{
	if (0 != strcmp(password2_->getText(), password3_->getText()))
	{
		MsgBox(a2u("ÐÂÃÜÂëÉèÖÃ²»Í¬£¡"));
		return;
	}

	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();
	PACKET_HELPER_SIZE(512);
	packet.write4Byte(user_data->user_id);
	packet.writeString(BaseUtil::MD5Encrypt(password1_->getText()), LEN_MD5);
	packet.writeString(BaseUtil::MD5Encrypt(password3_->getText()), LEN_MD5);
	PlazaKernel::getInstance()->Send(MDM_GP_USER_SERVICE, SUB_GP_MODIFY_PASS, packet.getBuffer(), packet.getPosition());

	removeFromParentAndCleanup(true);
}
