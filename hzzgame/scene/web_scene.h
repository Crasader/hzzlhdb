
#ifndef WEB_SCENE_H_
#define WEB_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocos2d::ui;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

class WebScene : public Layer
{
public:
	static Scene* createScene(const std::string& url);

	WebScene();
	virtual ~WebScene();
    
	static WebScene* createWithUrl(const std::string& url);

private:
	bool initWithUrl(const std::string& url);
	void OnClickBack(Ref* sender);
};

void CreateWebScene(const std::string& url);

#endif

#endif // WEB_SCENE_H_

