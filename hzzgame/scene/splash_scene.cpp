
#include "splash_scene.h"

#include "../AppMacros.h"

Scene* SplashScene::createScene()
{
    auto scene = Scene::create();
    
	auto layer = SplashScene::create();

    scene->addChild(layer);

    return scene;
}

SplashScene::SplashScene()
{
	std::string storagePath = FileUtils::getInstance()->getWritablePath();
	assets_manager_ = new AssetsManagerEx("project.manifest", storagePath);
}

SplashScene::~SplashScene()
{
	CC_SAFE_DELETE(assets_manager_);
}

bool SplashScene::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("splash/splash_bg.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(bg);

	//scheduleOnce([=](float)
	//{
	//	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOGIN_SCENE");
	//}, 0.1f, "splash_to_login");
	//return true;

	if (!assets_manager_->getLocalManifest()->isLoaded())
	{
		CCLOG("Fail to update assets, step skipped.");
		//OnUpdateEnd();
		scheduleOnce([=](float)
		{
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOGIN_SCENE");
		}, 0.1f, "splash_to_login");
	}
	else
	{
		auto listener = cocos2d::extension::EventListenerAssetsManagerEx::create(assets_manager_, [this](EventAssetsManagerEx* event){
			static int failCount = 0;
			switch (event->getEventCode())
			{
			case EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST:
			{
				CCLOG("No local manifest file found, skip assets update.");
				this->OnUpdateEnd();
			}
			break;
			case EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION:
			{
				std::string assetId = event->getAssetId();
				float percent = event->getPercent();
				std::string str;
				if (assetId == AssetsManagerEx::VERSION_ID)
				{
					str = StringUtils::format("Version file: %.2f", percent) + "%";
				}
				else if (assetId == AssetsManagerEx::MANIFEST_ID)
				{
					str = StringUtils::format("Manifest file: %.2f", percent) + "%";
				}
				else
				{
					str = StringUtils::format("%.2f", percent) + "%";
					CCLOG("%.2f Percent", percent);
				}
				CCLOG(str.c_str());
			}
			break;
			case EventAssetsManagerEx::EventCode::ERROR_DOWNLOAD_MANIFEST:
			case EventAssetsManagerEx::EventCode::ERROR_PARSE_MANIFEST:
			{
				CCLOG("Fail to download manifest file, update skipped.");
				this->OnUpdateEnd();
			}
			break;
			case EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE:
			case EventAssetsManagerEx::EventCode::UPDATE_FINISHED:
			{
				CCLOG("Update finished. %s", event->getMessage().c_str());
				this->OnUpdateEnd();
			}
			break;
			case EventAssetsManagerEx::EventCode::UPDATE_FAILED:
			{
				CCLOG("Update failed. %s", event->getMessage().c_str());

				failCount++;
				if (failCount < 5)
				{
					assets_manager_->downloadFailedAssets();
				}
				else
				{
					CCLOG("Reach maximum fail count, exit update process");
					failCount = 0;
					this->OnUpdateEnd();
				}
			}
			break;
			case EventAssetsManagerEx::EventCode::ERROR_UPDATING:
			{
				CCLOG("Asset %s : %s", event->getAssetId().c_str(), event->getMessage().c_str());
			}
			break;
			case EventAssetsManagerEx::EventCode::ERROR_DECOMPRESS:
			{
				CCLOG("%s", event->getMessage().c_str());
			}
			break;
			default:
				break;
			}
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);

		assets_manager_->update();
	}
    
    return true;
}

void SplashScene::OnUpdateEnd()
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EVENT_CREATE_LOGIN_SCENE");
}

