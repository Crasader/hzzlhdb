
#include "WebViewDlg.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS )

#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "../common/UIHelperEx.h"
#include "../data/global_user_info.h"

using namespace cocos2d::experimental::ui;

static WebViewDlg * g_pWebViewDlg = nullptr;

WebViewDlg * WebViewDlg::getInstance()
{
	return g_pWebViewDlg;
}

Scene* WebViewDlg::createScene()
{
    auto scene = Scene::create();
    
	auto layer = WebViewDlg::create();

    scene->addChild(layer);

    return scene;
}

WebViewDlg::WebViewDlg()
{
	g_pWebViewDlg = this;
}

WebViewDlg::~WebViewDlg()
{
}

bool WebViewDlg::init()
{
    if (!Layer::init())
    {
        return false;
    }
	//Size visibleSize2 = Director::getInstance()->getVisibleSize();
	
	GlobalUserData* user_data = g_global_userinfo.GetGlobalUserData();

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(WebViewDlg::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(WebViewDlg::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(WebViewDlg::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(WebViewDlg::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto ui = CSLoader::createNode("storeweb.csb");
	addChild(ui);
    
	Button* btBack = (Button*)Helper::seekWidgetByName((Widget*)ui, "back");
	btBack->addClickEventListener(CC_CALLBACK_1(WebViewDlg::OnClickBack, this));

	Button* btBack0 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_0");
	btBack0->addClickEventListener(CC_CALLBACK_1(WebViewDlg::OnClickBack, this));
	Button* btBack1 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_1");
	btBack1->addClickEventListener(CC_CALLBACK_1(WebViewDlg::OnClickBack, this));
	Button* btBack2 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_2");
	btBack2->addClickEventListener(CC_CALLBACK_1(WebViewDlg::OnClickBack, this));
	Button* btBack3 = (Button*)Helper::seekWidgetByName((Widget*)ui, "bt_CLOSE_3");
	btBack3->addClickEventListener(CC_CALLBACK_1(WebViewDlg::OnClickBack, this));

	//Size visibleSize = Director::getInstance()->getVisibleSize();
	Size visibleSize = Size(860,470);
	WebView *webView = WebView::create();
	webView->setPosition(Vec2(640, 322));
	webView->setContentSize(visibleSize);
	char temp[256] = { 0 };
	//sprintf(temp, "http://183.60.110.216:8084/UserQuery.aspx?gid=%d", user_data->game_id);
	sprintf(temp, "http://www.baidu.com");
	webView->loadURL(temp);
	webView->setScalesPageToFit(true);
	webView->setOnShouldStartLoading([](WebView *sender, const std::string &url){
		return true;
	});
	webView->setOnDidFinishLoading([](WebView *sender, const std::string &url){});
	webView->setOnDidFailLoading([](WebView *sender, const std::string &url){});
	this->addChild(webView);

    return true;
}

bool WebViewDlg::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void WebViewDlg::onTouchMoved(Touch *touch, Event *event)
{
}

void WebViewDlg::onTouchEnded(Touch *touch, Event *event)
{
}

void WebViewDlg::onTouchCancelled(Touch *touch, Event *event)
{
}

void WebViewDlg::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
}

void WebViewDlg::onExit()
{
	Layer::onExit();
}

//¹Ø±Õ´°¿Ú
void WebViewDlg::OnClickBack(Ref* sender)
{
	removeFromParentAndCleanup(true);
}

#endif