
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
	//关闭窗口
	void OnClickBack(Ref* sender);
	//上一页
	void OnClickOn(Ref* sender);
	//下一页
	void OnClickNext(Ref* sender);
	//兑换
	void OnClickExchange0(Ref* sender);
	//兑换
	void OnClickExchange1(Ref* sender);
	//兑换
	void OnClickExchange2(Ref* sender);
	//兑换
	void OnClickExchange3(Ref* sender);
	//兑换
	void OnClickExchange4(Ref* sender);

public:
	//更新控件
	void updateControl();
	//获取兑换信息
	void GetExchangeInfo();

	//
protected:
	Text * m_pTextLottery;				//鹅毛
	Text * m_pTextMedal;				//龙角
	Text * m_pTextId[5];				//商品ID
	Text * m_pTextPrice[5];				//单价
	Text * m_pTextPage;					//分页
	Button * m_pbtOn;
	Button * m_pbtNext;
	Button * m_pbtExchange0;
	Button * m_pbtExchange1;
	Button * m_pbtExchange2;
	Button * m_pbtExchange3;
	Button * m_pbtExchange4;

	DWORD m_dwCurPage;					//当前页
	DWORD m_dwTotalPage;				//总页

	CMD_GP_ExchangeInfo m_ExchangeInfo;
};

#endif // EXCHANGE_SCENE_H_

