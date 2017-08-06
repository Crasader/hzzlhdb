
#ifndef CHECKIN_SCENE_H_
#define CHECKIN_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../kernel/plaza_kernel.h"

USING_NS_CC;
using namespace cocos2d::ui;

class CheckInScene : public Layer
{
public:
    static Scene * createScene();

	CheckInScene();
	virtual ~CheckInScene();

    virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
    
	CREATE_FUNC(CheckInScene);

	static CheckInScene * getInstance();
	bool OnSocketCheckIn(uint16 sub_cmd_id, void* data, uint16 data_size);

private:
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

private:
	//�رմ���
	void OnClickBack(Ref* sender);
	//ִ��ǩ��
	void OnClickCheckInDone(Ref* sender);
	//���¿ؼ�
	void updateControl();

	//ǩ������
protected:
	bool							m_bTodayChecked;					//ǩ����ʶ
	uint16							m_wDateIndex;						//��������
	int64							m_lRewardGold[LEN_WEEK];			//�������	

public:
	Text *							m_text[LEN_WEEK];
	Text *							m_textDate;
	Sprite *						m_sprImageCard[LEN_WEEK];
	Animate *						m_act[LEN_WEEK];
	Button *						m_btCheckIn;
};

#endif // CHECKIN_SCENE_H_

