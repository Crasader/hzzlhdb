
#include "dialog_msg_box.h"

#include "cocostudio/CocoStudio.h"

DialogMsgBox::DialogMsgBox()
{
}

DialogMsgBox::~DialogMsgBox()
{

}

bool DialogMsgBox::init()
{
	if (!Dialog::init())
	{
		return false;
	}

	auto ui = CSLoader::createNode("msgbox.csb");
	addChild(ui);

	auto title = (Text*)Helper::seekWidgetByName((Widget*)ui, "title");
	text_tips_ = (Text*)Helper::seekWidgetByName((Widget*)ui, "content");
	auto confirm = (Button*)Helper::seekWidgetByName((Widget*)ui, "confirm");
	confirm->addClickEventListener(CC_CALLBACK_1(DialogMsgBox::OnClickConfirm, this));

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	return true;
}

void DialogMsgBox::SetTipsText(const std::string& text)
{
	text_tips_->setString(text);
}

void DialogMsgBox::OnClickConfirm(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

void MsgBox(const std::string& text)
{
	auto box = DialogMsgBox::create();
	Director::getInstance()->getRunningScene()->addChild(box);
	box->setVisible(false);
	box->setScale(0, 0);
	Action* act = Sequence::create(
		Show::create(),
		ScaleTo::create(0.06f, 1.05f),
		ScaleTo::create(0.08f, 0.95f),
		ScaleTo::create(0.08f, 1.0f),
		nullptr);
	box->runAction(act);
	box->SetTipsText(text);
}
