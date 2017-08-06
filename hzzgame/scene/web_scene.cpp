
#include "web_scene.h"

#include "../base/convert.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

Scene* WebScene::createScene(const std::string& url)
{
	auto scene = Scene::create();

	auto layer = WebScene::createWithUrl(url);

	scene->addChild(layer);

	return scene;
}

WebScene::WebScene()
{
}

WebScene::~WebScene()
{
}

WebScene* WebScene::createWithUrl(const std::string& url)
{
	WebScene* web = new (std::nothrow) WebScene();
	if (web && web->initWithUrl(url))
	{
		web->autorelease();
		return web;
	}
	CC_SAFE_DELETE(web);
	return nullptr;
}

bool WebScene::initWithUrl(const std::string& url)
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto web = cocos2d::experimental::ui::WebView::create();
	web->setPosition(visibleSize / 2);
	web->setContentSize(visibleSize);
	web->loadURL(url);
	web->setScalesPageToFit(true);
	addChild(web);

	auto btn_back = Button::create("common/btn_back_1.png", "common/btn_back_2.png");
	btn_back->setPosition(Vec2(75, 666));
	btn_back->setTitleText(a2u("·µ»Ø"));
	btn_back->setTitleFontName("arial");
	btn_back->setTitleFontSize(32);
	btn_back->addClickEventListener(CC_CALLBACK_1(WebScene::OnClickBack, this));
	addChild(btn_back);

	return true;
}

void WebScene::OnClickBack(Ref* sender)
{
	Director::getInstance()->popScene();
}

void CreateWebScene(const std::string& url)
{
	auto scene = WebScene::createScene(url);
	Director::getInstance()->pushScene(TransitionFade::create(0.3f, scene));
}

#endif
