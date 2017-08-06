
#include "game_installer.h"

#include "cocos2d.h"

//#include "ddz/lgdb2_loading_scene.h"
//#include "lkpy/lkpy_loading_scene.h"
#include "ddz/ddz_game_scene.h"
//#include "nnTB/nnTB_game_scene.h"
//#include "fishmatch/fishmatch_loading_scene.h"

USING_NS_CC;

void GameInstaller::InstallGame(uint16 kind_id)
{
	switch (kind_id)
	{
	case 1:
	{
		auto scene = ddz::GameScene::createScene();
		Director::getInstance()->pushScene(TransitionFade::create(0.0f, scene));
		break;
	}
	//case 219:
	//{
	//	auto scene = LHDB::GameScene::createScene();
	//	Director::getInstance()->pushScene(TransitionFade::create(0.0f, scene));
	//	break;
	//}
 	//case 2255:
 	//{
 	//	auto scene = nnTB::GameScene::createScene();
 	//	Director::getInstance()->pushScene(TransitionFade::create(0.0f, scene));
 	//	break;
 	//}
	//case 2100:
	//{
	//	auto scene = lkpy::LoadingScene::createScene();
	//	Director::getInstance()->pushScene(TransitionFade::create(0.0f, scene));
	//	break;
	//}
	//case 2200:
	//{
	//	auto scene = lgdb2::LoadingScene::createScene();
	//	Director::getInstance()->pushScene(TransitionFade::create(0.0f, scene));
	//	break;
	//}
	//case 2301:
	//{
	//	auto scene = fishmatch::LoadingScene::createScene();
	//	Director::getInstance()->pushScene(TransitionFade::create(0.0f, scene));
	//	break;
	//}
	default:
		break;
	}
}
