
#include "prompt_box.h"

#include "cocostudio/CocoStudio.h"

PromptBox::PromptBox()
{
}

PromptBox::~PromptBox()
{

}

bool PromptBox::init()
{
	if (!Dialog::init())
	{
		return false;
	}

	auto ui = CSLoader::createNode("PromptBox.csb");
	addChild(ui);

	auto title = (Text*)Helper::seekWidgetByName((Widget*)ui, "title");
	text_tips_ = (Text*)Helper::seekWidgetByName((Widget*)ui, "content");
	auto confirm = (Button*)Helper::seekWidgetByName((Widget*)ui, "confirm");
	confirm->addClickEventListener(CC_CALLBACK_1(PromptBox::OnClickConfirm, this));
	auto Screenshot = (Button*)Helper::seekWidgetByName((Widget*)ui, "confirm_0");
	Screenshot->addClickEventListener(CC_CALLBACK_1(PromptBox::OnClickScreenshot, this));

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	return true;
}

void PromptBox::SetTipsText(const std::string& text)
{
	text_tips_->setString(text);
}

void PromptBox::OnClickConfirm(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

void PromptBox::OnClickScreenshot(Ref* sender)
{
	std::string text = text_tips_->getString();
	std::string fileName = text.c_str();
	fileName.append(".png");
	std::string fullpath = CCFileUtils::getInstance()->getWritablePath() + fileName;
	MessageBox(fullpath.c_str(), "");
	utils::captureScreen(nullptr, fullpath);
}

void _PromptBox(const std::string& text)
{
	auto box = PromptBox::create();
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
