
#include "dialog_waiting.h"

#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"

DialogWaiting::DialogWaiting()
{
}

DialogWaiting::~DialogWaiting()
{

}

bool DialogWaiting::init()
{
	if (!Dialog::init())
	{
		return false;
	}

	auto ui = CSLoader::createNode("waiting.csb");
	addChild(ui);

	// ¶¯»­
	auto animation = Animation::create();
	char temp[256] = { 0 };
	for (int i = 0; i < 3; ++i)
	{
		sprintf(temp, "common/waiting/%02d.png", i + 1);
		animation->addSpriteFrameWithFile(temp);
	}
	animation->setDelayPerUnit(2.0f / 4.0f);
	auto ani = (Sprite*)HelperEx::seekNodeByName(ui, "animation");
	ani->runAction(RepeatForever::create(Animate::create(animation)));

	text_tips_ = (Text*)Helper::seekWidgetByName((Widget*)ui, "content");
	auto btn_cancel = (Button*)Helper::seekWidgetByName((Widget*)ui, "cancel");
	btn_cancel->addClickEventListener(CC_CALLBACK_1(DialogWaiting::OnClickCancel, this));

	return true;
}

void DialogWaiting::SetTipsText(const std::string& text)
{
	text_tips_->setString(text);
}

void DialogWaiting::OnClickCancel(Ref* sender)
{

}

void DlgWaiting(const std::string& text)
{
	auto box = DialogWaiting::create();
	box->setName("DialogWaiting");
	Director::getInstance()->getRunningScene()->addChild(box);
	box->SetTipsText(text);
}

void RemoveDlgWaiting()
{
	Director::getInstance()->getRunningScene()->removeChildByName("DialogWaiting");
}
