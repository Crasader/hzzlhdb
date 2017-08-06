
#include "AppDelegate.h"
#include "AppMacros.h"
#include "scene/splash_scene.h"
#include "Common/custom_event.h"
#include "network/Socket_notify.h"
#include "kernel/plaza_kernel.h"
#include "kernel/server_kernel.h"
#include "audio/include/AudioEngine.h"
#include "audio/include/SimpleAudioEngine.h"
#include "scene/login_scene.h"


USING_NS_CC;
using namespace CocosDenshion;
//int AppDelegate::PayType = 0;
AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate()
{
	CustomEvent::destroyInstance();
	SocketNotify::destroyInstance();
	PlazaKernel::destroyInstance();
	ServerKernel::destory();
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
	//set OpenGL context attributions,now can only set six attributions:
	//red,green,blue,alpha,depth,stencil
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

	GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
	FileUtils::getInstance()->addSearchPath("ResourcesPacker");
	return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = GLViewImpl::createWithRect("hzzgame", Rect(0, 0, 1750, 900));
		//glview = GLViewImpl::createWithFullScreen("Test");
		//glview = GLViewImpl::createWithRect("hzzgame", Rect(0, 0, kResolutionSize.width, kResolutionSize.height));
		
		//glview = GLViewImpl::create("hzzgame");
		//director->setOpenGLView(glview);
		//glview->setDesignResolutionSize(1920, 1080, ResolutionPolicy::SHOW_ALL);
#else
		glview = GLViewImpl::create("hzzgame");
#endif
		director->setOpenGLView(glview);
	}

	// turn on display FPS
	director->setDisplayStats(true);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / 60);

	// Set the design resolution
	glview->setDesignResolutionSize(kResolutionSize.width, kResolutionSize.height, ResolutionPolicy::EXACT_FIT);

	register_all_packages();

	// create a scene. it's an autorelease object
	auto scene = SplashScene::createScene();

	// run
	director->runWithScene(scene);

	CustomEvent::getInstance();

	// ÍøÂç
	director->getScheduler()->schedule(schedule_selector(SocketNotify::OnSocketUpdate), SocketNotify::getInstance(), (float)(1.0 / 60.0), false);

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	//Director::getInstance()->stopAnimation();

	// if you use SimpleAudioEngine, it must be pause
	SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

	//	Director::getInstance()->end();
	//
	//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	//	exit(0);
	//#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	//Director::getInstance()->startAnimation();

	// if you use SimpleAudioEngine, it must resume here
	SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	// LoginScene::getInstance()->OnClickStartGame(nullptr);
}
