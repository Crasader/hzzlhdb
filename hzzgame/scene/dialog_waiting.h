
#ifndef DIALOG_WAITING_H_
#define DIALOG_WAITING_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../common/UIDialog.h"

USING_NS_CC;
using namespace cocos2d::ui;

class DialogWaiting : public Dialog
{
public:
	DialogWaiting();
	virtual ~DialogWaiting();
	CREATE_FUNC(DialogWaiting);

	virtual bool init();

	void SetTipsText(const std::string& text);

private:
	void OnClickCancel(Ref* sender);

private:
	Text* text_tips_;
};

void DlgWaiting(const std::string& text);
void RemoveDlgWaiting();

#endif	// DIALOG_WAITING_H_
