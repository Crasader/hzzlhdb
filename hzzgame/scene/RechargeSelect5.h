
#ifndef RECHARGE5_SELECT_H_
#define RECHARGE5_SELECT_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../kernel/plaza_kernel.h"

USING_NS_CC;
using namespace cocos2d::ui;

class RechargeSelect5 : public Layer
{
public:
    static Scene * createScene();

	RechargeSelect5();
	virtual ~RechargeSelect5();

    virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
    
	CREATE_FUNC(RechargeSelect5);

	static RechargeSelect5 * getInstance();

private:
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

private:
	//�رմ���
	void OnClickBack(Ref* sender);
	//��ֵ1
	void OnClickRecharge1(Ref* sender);
	//��ֵ2
	void OnClickRecharge2(Ref* sender);
};

#endif // RECHARGE_SELECT_H_

