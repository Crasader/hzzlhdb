
#ifndef UI_DIALOG_H_
#define UI_DIALOG_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class Dialog : public Layer
{
public:
	Dialog();
	virtual ~Dialog();
	CREATE_FUNC(Dialog);

	virtual bool init();

private:
	virtual bool onTouchBegan(Touch *touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event *event);
	virtual void onTouchCancelled(Touch *touch, Event *event);

private:
};

#endif	// UI_DIALOG_H_
