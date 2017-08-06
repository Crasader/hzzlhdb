
#ifndef REGISTER_SCENE_H_
#define REGISTER_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

class RegisterScene : public Layer
{
public:
    static Scene* createScene();

	RegisterScene();
	virtual ~RegisterScene();

    virtual bool init();
    
	CREATE_FUNC(RegisterScene);

private:
	void OnClickBack(Ref* sender);
	void OnClickQuick(Ref* sender);
	void OnClickNormal(Ref* sender);
	void OnClickNormalOk(Ref* sender);

	/* 储存内容到文件 */
	bool saveFile(string pFileName);

private:
	Node* quick_layer_;
	Node* normal_layer_;
	EditBox* normal_accounts_;
	EditBox* normal_acplayername_;
	EditBox* normal_telphone_;
	EditBox* normal_password_;
	EditBox* normal_password2_;
};

#endif // REGISTER_SCENE_H_

