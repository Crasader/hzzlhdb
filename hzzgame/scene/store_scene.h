
#ifndef STORE_SCENE_H_
#define STORE_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "RechargeSelect1.h"
#include "RechargeSelect2.h"
#include "RechargeSelect3.h"
#include "RechargeSelect4.h"
#include "RechargeSelect5.h"

USING_NS_CC;
using namespace cocos2d::ui;

class StoreScene : public Layer
{
public:
    static Scene* createScene();

	StoreScene();
	virtual ~StoreScene();

    virtual bool init();
    
	CREATE_FUNC(StoreScene);

private:
	void OnClickBack(Ref* sender);
	void OnClickCommodity(Ref* sender);
	void OnClickRecord(Ref* sender);
	void OnClickBuy1(Ref* sender);
	void OnClickBuy2(Ref* sender);
	void OnClickBuy3(Ref* sender);
	void OnClickBuy4(Ref* sender);
	void OnClickBuy5(Ref* sender);
	void OnClickBuy6(Ref* sender);
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

private:
	ListView* commodity_list_;
	ListView* record_list_;
	Button* btn_commodity_;
	Button* btn_record_;
};

#endif // STORE_SCENE_H_

