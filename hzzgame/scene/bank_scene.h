
#ifndef BANK_SCENE_H_
#define BANK_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../kernel/plaza_kernel.h"

USING_NS_CC;
using namespace cocos2d::ui;

class BankSaveScene : public Layer, public PlazaKernel::BankDelegate
{
public:
    static Scene* createScene();

	BankSaveScene();
	virtual ~BankSaveScene();

    virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
    
	CREATE_FUNC(BankSaveScene);

private:
	virtual bool OnSocketBankService(uint16 sub_cmd_id, void* data, uint16 data_size);

	void OnClickBack(Ref* sender);
	void OnClickSave(Ref* sender);
	void OnClickModifyPassword(Ref* sender);
	void OnClickGetScore(Ref* sender);
	void OnClickTransfer(Ref* sender);
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

private:
	EditBox* edit_save_num_;
	Text* userscore_;
};

//////////////////////////////////////////////////////////////////////////

class BankGetScene : public Layer, public PlazaKernel::BankDelegate
{
public:
	static Scene* createScene();

	BankGetScene();
	virtual ~BankGetScene();

	virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();

	CREATE_FUNC(BankGetScene);

private:
	virtual bool OnSocketBankService(uint16 sub_cmd_id, void* data, uint16 data_size);

	void OnClickBack(Ref* sender);
	void OnClickGet(Ref* sender);
	void OnClickModifyPassword(Ref* sender);
	void OnClickSaveScore(Ref* sender);
	void OnClickTransfer(Ref* sender);
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

private:
	EditBox* edit_get_num_;
	EditBox* edit_password_;
	Text* insurescore_;
};

//////////////////////////////////////////////////////////////////////////

class BankTransferScene : public Layer, public PlazaKernel::BankDelegate
{
public:
	static Scene* createScene();

	BankTransferScene();
	virtual ~BankTransferScene();

	virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();

	CREATE_FUNC(BankTransferScene);

private:
	virtual bool OnSocketBankService(uint16 sub_cmd_id, void* data, uint16 data_size);

	void OnClickBack(Ref* sender);
	void OnClickModifyPassword(Ref* sender);
	void OnClickSaveScore(Ref* sender);
	void OnClickGetScore(Ref* sender);
	void OnClickQuery(Ref* sender);
	void OnClickTransfer(Ref* sender);
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

private:
	EditBox* edit_id_;
	EditBox* edit_get_num_;
	EditBox* edit_password_;
	Text* insurescore_;
	Text* Text_nickname;
};

#endif // BANK_SCENE_H_

