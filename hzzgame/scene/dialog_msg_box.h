
#ifndef DIALOG_MSG_BOX_H_
#define DIALOG_MSG_BOX_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../common/UIDialog.h"

USING_NS_CC;
using namespace cocos2d::ui;

class DialogMsgBox : public Dialog
{
public:
	DialogMsgBox();
	virtual ~DialogMsgBox();
	CREATE_FUNC(DialogMsgBox);

	virtual bool init();

	void SetTipsText(const std::string& text);

private:
	void OnClickConfirm(Ref* sender);

private:
	Text* text_tips_;
};

void MsgBox(const std::string& text);

#endif	// DIALOG_MSG_BOX_H_
