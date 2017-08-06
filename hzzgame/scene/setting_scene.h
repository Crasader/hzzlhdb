
#ifndef SETTING_SCENE_H_
#define SETTING_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocos2d::ui;

class SettingScene : public Layer
{
public:
    static Scene* createScene();

	SettingScene();
	virtual ~SettingScene();

    virtual bool init();
    
	CREATE_FUNC(SettingScene);

private:
	void OnClickBack(Ref* sender);
	void OnClickFeedback(Ref* sender);
	void OnClickHelp(Ref* sender);
	void OnClickPasswordModify(Ref* sender);
	void OnClickBindPhone(Ref* sender);
	void OnClickSwitchAccounts(Ref* sender);
	void OnClickPrivacy(Ref* sender);
	void OnClickService(Ref* sender);
	void OnClickGold(Ref* sender);
};

#endif // SETTING_SCENE_H_

