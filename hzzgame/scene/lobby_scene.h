
#ifndef LOBBY_SCENE_H_
#define LOBBY_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../base/basictypes.h"
#include "../scene/CheckIn_scene.h"

USING_NS_CC;
using namespace cocos2d::ui;

class LobbyScene : public Layer
{
public:
    static Scene* createScene();

	LobbyScene();
	virtual ~LobbyScene();

    virtual bool init();
	virtual void onEnterTransitionDidFinish();
    
	CREATE_FUNC(LobbyScene);

private:
	void ShowGameServerList(uint16 kind_id);
	void BackToGameKindList();
	void SetTitle(uint16 kind_id);

private:
	void OnClickHorn(Ref* sender);
	void OnClickHornView(Ref* sender);
	void OnClickFace(Ref* sender);
	void OnClickBack(Ref* sender);
	void OnClickSetting(Ref* sender);
	void OnClickNews(Ref* sender);
	void OnClickMore(Ref* sender);
	void OnClickActivity(Ref* sender);
	void OnClickStore(Ref* sender);
	void OnClickStoreWeb(Ref* sender);
	void OnClickBank(Ref* sender);
	void OnClickExchange(Ref* sender);
	void OnClickCheckIn(Ref* sender);
	void OnClickRanking(Ref* sender);
	void OnClickWXShare(Ref* sender);
	//拍卖
	void OnClickAuction(Ref* sender);
	void OnClickQuickStart(Ref* sender);
	void selectedItemEvent(Ref *pSender, ListView::EventType type);
	void OnClickGameServer(Ref* sender);

public:
	static LobbyScene * getInstance();

	//更新用户分数
	void UpdateUserScore();

private:
	Node* top_bar_layer_;
	Node* bottom_bar_layer_;
	Node* kindlist_layer_;
	Node* serverlist_layer_;
	ListView * m_pServerList;
	CheckInScene * m_checkInScene;

	Sprite* photo_;
	ImageView* photo_box_;
	ImageView* back_;
	Text * userscore_;
	Text * usergameid_;
	Text * usermedal_;
	Text * userLottery_;

	tagGameServer m_GameServer[50];
};

#endif // LOBBY_SCENE_H_

