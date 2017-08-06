
#ifndef AUCTION_SCENE_H_
#define AUCTION_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../kernel/plaza_kernel.h"
#include "Auction_Box.h"

USING_NS_CC;
using namespace cocos2d::ui;

class AuctionScene : public Layer
{
public:
    static Scene * createScene();

	AuctionScene();
	virtual ~AuctionScene();

    virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
    
	CREATE_FUNC(AuctionScene);

	static AuctionScene * getInstance();
	bool OnSocketAuction(uint16 sub_cmd_id, void* data, uint16 data_size);

private:
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

public:
	//关闭窗口
	void OnClickBack(Ref* sender);
	//刷新
	void OnClickRefresh(Ref* sender);
	//查找
	void OnClickSearch(Ref* sender);
	//拍卖
	void OnClickAuction(Ref* sender);
	//我的拍卖
	void OnClickMyAuction(Ref* sender);
	//上一页
	void OnClickOn(Ref* sender);
	//下一页
	void OnClickNext(Ref* sender);
	//购买
	void OnClickBuy0(Ref* sender);
	//购买
	void OnClickBuy1(Ref* sender);
	//购买
	void OnClickBuy2(Ref* sender);
	//购买
	void OnClickBuy3(Ref* sender);
	//购买
	void OnClickBuy4(Ref* sender);

public:
	//更新控件
	void updateControl();
	//获取拍卖信息
	void GetAuctionInfo();
	//获取自己拍卖信息
	void GetMyAuctionInfo();

	//
protected:
	Text * m_pTextLottery;				//鹅毛
	Text * m_pTextMedal;				//龙角
	Text * m_pTextId[5];				//商品ID
	Text * m_pTextType[5];				//商品类型
	Text * m_pTextTotalNum[5];			//总数量
	Text * m_pTextPrice[5];				//单价
	Text * m_pTextTotalPrice[5];		//总价
	Text * m_pTextPage;					//分页
	EditBox	* m_pEbSearch;				//查找
	Button * m_pbtOn;
	Button * m_pbtNext;
	Button * m_pbtBuy0;
	Button * m_pbtBuy1;
	Button * m_pbtBuy2;
	Button * m_pbtBuy3;
	Button * m_pbtBuy4;

	AuctionBox * m_pAuctionBox;

	DWORD m_dwCurPage;					//当前页
	DWORD m_dwTotalPage;				//总页
	BYTE m_cbType;
	DWORD m_dwState[5];					//状态0正在拍卖，1等待确认，2过期的
};

#endif // AUCTION_SCENE_H_

