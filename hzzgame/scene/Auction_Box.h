
#ifndef AUTOIN_BOX_H_
#define AUTOIN_BOX_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../kernel/plaza_kernel.h"

USING_NS_CC;
using namespace cocos2d::ui;

class AuctionBox : public Layer
{
public:
    static Scene * createScene();

	AuctionBox();
	virtual ~AuctionBox();

    virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
    
	CREATE_FUNC(AuctionBox);

	static AuctionBox * getInstance();

private:
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

private:
	//关闭窗口
	void OnClickBack(Ref* sender);
	//提交
	void OnClickConfirm(Ref* sender);
	//选择龙角
	void selectedMedalEvent(Ref* ref, CheckBox::EventType event);
	//选择鹅毛
	void selectedLotteryEvent(Ref* ref, CheckBox::EventType event);

	//
protected:
	Text * m_pTextLottery;				//鹅毛
	Text * m_pTextMedal;				//龙角
	CheckBox * m_pCbMedal;
	CheckBox * m_pCbLottery;
	EditBox	 * m_pEbNum;				//数量
	EditBox	 * m_pEbPrice;				//价格
};

#endif // AUTOIN_BOX_H_

