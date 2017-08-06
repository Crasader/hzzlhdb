
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
	//�رմ���
	void OnClickBack(Ref* sender);
	//ˢ��
	void OnClickRefresh(Ref* sender);
	//����
	void OnClickSearch(Ref* sender);
	//����
	void OnClickAuction(Ref* sender);
	//�ҵ�����
	void OnClickMyAuction(Ref* sender);
	//��һҳ
	void OnClickOn(Ref* sender);
	//��һҳ
	void OnClickNext(Ref* sender);
	//����
	void OnClickBuy0(Ref* sender);
	//����
	void OnClickBuy1(Ref* sender);
	//����
	void OnClickBuy2(Ref* sender);
	//����
	void OnClickBuy3(Ref* sender);
	//����
	void OnClickBuy4(Ref* sender);

public:
	//���¿ؼ�
	void updateControl();
	//��ȡ������Ϣ
	void GetAuctionInfo();
	//��ȡ�Լ�������Ϣ
	void GetMyAuctionInfo();

	//
protected:
	Text * m_pTextLottery;				//��ë
	Text * m_pTextMedal;				//����
	Text * m_pTextId[5];				//��ƷID
	Text * m_pTextType[5];				//��Ʒ����
	Text * m_pTextTotalNum[5];			//������
	Text * m_pTextPrice[5];				//����
	Text * m_pTextTotalPrice[5];		//�ܼ�
	Text * m_pTextPage;					//��ҳ
	EditBox	* m_pEbSearch;				//����
	Button * m_pbtOn;
	Button * m_pbtNext;
	Button * m_pbtBuy0;
	Button * m_pbtBuy1;
	Button * m_pbtBuy2;
	Button * m_pbtBuy3;
	Button * m_pbtBuy4;

	AuctionBox * m_pAuctionBox;

	DWORD m_dwCurPage;					//��ǰҳ
	DWORD m_dwTotalPage;				//��ҳ
	BYTE m_cbType;
	DWORD m_dwState[5];					//״̬0����������1�ȴ�ȷ�ϣ�2���ڵ�
};

#endif // AUCTION_SCENE_H_

