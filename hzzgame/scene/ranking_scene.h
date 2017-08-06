
#ifndef RANKING_SCENE_H_
#define RANKING_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../kernel/plaza_kernel.h"

USING_NS_CC;
using namespace cocos2d::ui;

class RankingScene : public Layer, public PlazaKernel::RankingDelegate
{
public:
    static Scene* createScene();

	RankingScene();
	virtual ~RankingScene();

    virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
    
	CREATE_FUNC(RankingScene);

private:
	virtual bool OnSocketRankingResult(uint16 sub_cmd_id, void* data, uint16 data_size);

private:
	void OnClickBack(Ref* sender);
	void AddRankingItem(int order, const std::string nickname, SCORE score, WORD RankID);
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);
	Text* rank_name[10];
	Text* rank_score[10];
	Text* rank_id[10];

//private:
//	ListView* listview_;
};

#endif // RANKING_SCENE_H_

