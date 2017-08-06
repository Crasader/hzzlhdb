
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
	//�رմ���
	void OnClickBack(Ref* sender);
	//�ύ
	void OnClickConfirm(Ref* sender);
	//ѡ������
	void selectedMedalEvent(Ref* ref, CheckBox::EventType event);
	//ѡ���ë
	void selectedLotteryEvent(Ref* ref, CheckBox::EventType event);

	//
protected:
	Text * m_pTextLottery;				//��ë
	Text * m_pTextMedal;				//����
	CheckBox * m_pCbMedal;
	CheckBox * m_pCbLottery;
	EditBox	 * m_pEbNum;				//����
	EditBox	 * m_pEbPrice;				//�۸�
};

#endif // AUTOIN_BOX_H_

