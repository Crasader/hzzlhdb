
#ifndef SPLASH_SCENE_H_
#define SPLASH_SCENE_H_

#include "cocos2d.h"
#include "extensions/assets-manager/CCEventAssetsManagerEx.h"
#include "extensions/assets-manager/CCEventListenerAssetsManagerEx.h"
#include "extensions/assets-manager/AssetsManagerEx.h"

USING_NS_CC;
USING_NS_CC_EXT;

class SplashScene : public Layer
{
public:
    static Scene* createScene();

	SplashScene();
	virtual ~SplashScene();

    virtual bool init();
    
	CREATE_FUNC(SplashScene);

private:
	void plistImageAsyncCallback(Texture2D* tex);
	void OnUpdateEnd();

private:
	ValueVector resources_;
	ValueVector::size_type resource_index_;
	AssetsManagerEx * assets_manager_;
};

#endif // SPLASH_SCENE_H_

