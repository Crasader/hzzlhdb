
#ifndef USERINFO_SCENE_H_
#define USERINFO_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../kernel/plaza_kernel.h"
#include "dialog_password_modify.h"
USING_NS_CC;
using namespace cocos2d::ui;

class UserInfoScene : public Layer
{
public:
    static Scene* createScene();

	UserInfoScene();
	virtual ~UserInfoScene();

    virtual bool init();
    
	CREATE_FUNC(UserInfoScene);

private:
	void OnClickBack(Ref* sender);
	void OnClickSave(Ref* sender);
	void OnClickGender1(Ref* sender);
	void OnClickGender2(Ref* sender);
	void OnClickSwitchAccounts(Ref* sender);
	void OnClickPasswordModify(Ref* sender);

private:
	EditBox* nickname_;
	CheckBox* gender1_;
	CheckBox* gender2_;
};

#endif // USERINFO_SCENE_H_

