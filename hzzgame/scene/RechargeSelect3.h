
#ifndef RECHARGE3_SELECT_H_
#define RECHARGE3_SELECT_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../kernel/plaza_kernel.h"

USING_NS_CC;
using namespace cocos2d::ui;

class RechargeSelect3 : public Layer
{
public:
    static Scene * createScene();

	RechargeSelect3();
	virtual ~RechargeSelect3();

    virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
    
	CREATE_FUNC(RechargeSelect3);

	static RechargeSelect3 * getInstance();

private:
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

private:
	//关闭窗口
	void OnClickBack(Ref* sender);
	//充值1
	void OnClickRecharge1(Ref* sender);
	//充值2
	void OnClickRecharge2(Ref* sender);
};

#endif // RECHARGE_SELECT_H_

