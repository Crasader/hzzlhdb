
#ifndef EXCHANGE_SCENE_H_
#define EXCHANGE_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../kernel/plaza_kernel.h"

USING_NS_CC;
using namespace cocos2d::ui;

class ExchangeScene : public Layer
{
public:
    static Scene * createScene();

	ExchangeScene();
	virtual ~ExchangeScene();

    virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
    
	CREATE_FUNC(ExchangeScene);

	static ExchangeScene * getInstance();
	bool OnSocketExchange(uint16 sub_cmd_id, void* data, uint16 data_size);

private:
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

public:
	//�رմ���
	void OnClickBack(Ref* sender);
	//��һҳ
	void OnClickOn(Ref* sender);
	//��һҳ
	void OnClickNext(Ref* sender);
	//�һ�
	void OnClickExchange0(Ref* sender);
	//�һ�
	void OnClickExchange1(Ref* sender);
	//�һ�
	void OnClickExchange2(Ref* sender);
	//�һ�
	void OnClickExchange3(Ref* sender);
	//�һ�
	void OnClickExchange4(Ref* sender);

public:
	//���¿ؼ�
	void updateControl();
	//��ȡ�һ���Ϣ
	void GetExchangeInfo();

	//
protected:
	Text * m_pTextLottery;				//��ë
	Text * m_pTextMedal;				//����
	Text * m_pTextId[5];				//��ƷID
	Text * m_pTextPrice[5];				//����
	Text * m_pTextPage;					//��ҳ
	Button * m_pbtOn;
	Button * m_pbtNext;
	Button * m_pbtExchange0;
	Button * m_pbtExchange1;
	Button * m_pbtExchange2;
	Button * m_pbtExchange3;
	Button * m_pbtExchange4;

	DWORD m_dwCurPage;					//��ǰҳ
	DWORD m_dwTotalPage;				//��ҳ

	CMD_GP_ExchangeInfo m_ExchangeInfo;
};

#endif // EXCHANGE_SCENE_H_

