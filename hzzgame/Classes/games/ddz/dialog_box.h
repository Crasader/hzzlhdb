
#ifndef DIALOG_BOX_H_
#define DIALOG_BOX_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../../common/UIDialog.h"
#include "ddz_game_scene.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace ddz;

class ConfirmDlg : public Dialog
{
public:
	ConfirmDlg();
	virtual ~ConfirmDlg();
	CREATE_FUNC(ConfirmDlg);

	virtual bool init();

	void SetTipsText(const std::string& text);

private:
	//确定
	void OnClickConfirm(Ref* sender);
	//取消
	void OnClickCancel(Ref* sender);

private:
	Text* text_tips_;
};

void ConfirmBox(const std::string& text);

#endif	// DIALOG_BOX_H_
