
#include "dialog_box.h"
#include "../../base/convert.h"

ConfirmDlg::ConfirmDlg()
{
}

ConfirmDlg::~ConfirmDlg()
{

}

bool ConfirmDlg::init()
{
	if (!Dialog::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("msg_box/bg.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	addChild(bg);

	text_tips_ = Text::create();
	text_tips_->setPosition(Vec2(visibleSize.width / 2, 430.0f));
	text_tips_->setTextColor(Color4B(0, 0, 0, 255));
	text_tips_->setFontSize(32);
	addChild(text_tips_);

	// 确定按钮
	auto btOk = Button::create("msg_box/confirm.png", "msg_box/confirm.png");
	btOk->setPosition(Vec2(visibleSize.width / 2 - 130, 287.0f));
	btOk->setTitleText(a2u("确定"));
	btOk->setTitleFontSize(32);
	btOk->addClickEventListener(CC_CALLBACK_1(ConfirmDlg::OnClickConfirm, this));
	addChild(btOk);
	
	// 确定按钮
	auto btCancel = Button::create("msg_box/confirm.png", "msg_box/confirm.png");
	btCancel->setPosition(Vec2(visibleSize.width / 2 + 130, 287.0f));
	btCancel->setTitleText(a2u("取消"));
	btCancel->setTitleFontSize(32);
	btCancel->addClickEventListener(CC_CALLBACK_1(ConfirmDlg::OnClickCancel, this));
	addChild(btCancel);

	return true;
}

void ConfirmDlg::SetTipsText(const std::string& text)
{
	text_tips_->setString(text);
}

//确定
void ConfirmDlg::OnClickConfirm(Ref* sender)
{
	removeFromParentAndCleanup(true);
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CLOSE_GAME");
}

//取消
void ConfirmDlg::OnClickCancel(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

void ConfirmBox(const std::string& text)
{
	auto box = ConfirmDlg::create();
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
