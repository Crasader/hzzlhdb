
#ifndef WX_SHARE_H_
#define WX_SHARE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../kernel/plaza_kernel.h"

USING_NS_CC;
using namespace cocos2d::ui;

class WXShare : public Layer
{
public:
	static Scene * createScene();

	WXShare();
	virtual ~WXShare();

	virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();

	CREATE_FUNC(WXShare);

	static WXShare * getInstance();
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
	//分享1
	void OnClickWXShare1(Ref* sender);
	//分享2
	void OnClickWXShare2(Ref* sender);

	ImageView* Image1;
	ImageView* Image2;
};

#endif // RECHARGE_SELECT_H_

