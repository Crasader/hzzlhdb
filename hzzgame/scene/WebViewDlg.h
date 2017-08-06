
#ifndef WEB_VIEW_DLG_H_
#define WEB_VIEW_DLG_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS )

USING_NS_CC;
using namespace cocos2d::ui;

class WebViewDlg : public Layer
{
public:
    static Scene * createScene();

	WebViewDlg();
	virtual ~WebViewDlg();

    virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
    
	CREATE_FUNC(WebViewDlg);

	static WebViewDlg * getInstance();
	//微信分享监听
	void WaitWXfxcg(float dt);

private:
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

private:
	//关闭窗口
	void OnClickBack(Ref* sender);
};

#endif
#endif // RECHARGE_SELECT_H_

