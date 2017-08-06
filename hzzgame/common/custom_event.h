
#ifndef CUSTOM_EVENT_H_
#define CUSTOM_EVENT_H_

#include "cocos2d.h"

USING_NS_CC;

class CustomEvent : public Ref
{
public:
	CustomEvent();
	virtual ~CustomEvent();

	static CustomEvent* getInstance();
	static void destroyInstance();

private:
	void CreateLoginScene(EventCustom* event);
	void CreateRegisterScene(EventCustom* event);
	void CreateLobbyScene(EventCustom* event);
	void CreateUserInfoScene(EventCustom* event);
	void CreateSettingScene(EventCustom* event);
	void CreateStoreScene(EventCustom* event);
	void CreateStoreSceneWeb(EventCustom* event);
	void CreateBankScene(EventCustom* event);
	void CreateRoomScene(EventCustom* event);
	void CreateCheckInScene(EventCustom* event);
	void CreateRankingScene(EventCustom* event);
	void CreateWXShare(EventCustom* event);
	//创建拍卖窗口
	void CreateAuctionScene(EventCustom* event);
	//创建兑换窗口
	void CreateExchangeScene(EventCustom* event);

private:
};

#endif	// CUSTOM_EVENT_H_
