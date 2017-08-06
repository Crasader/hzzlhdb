
#ifndef PROMPT_BOX_H_
#define PROMPT_BOX_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../common/UIDialog.h"

USING_NS_CC;
using namespace cocos2d::ui;

class PromptBox : public Dialog
{
public:
	PromptBox();
	virtual ~PromptBox();
	CREATE_FUNC(PromptBox);

	virtual bool init();

	void SetTipsText(const std::string& text);

private:
	void OnClickConfirm(Ref* sender);
	void OnClickScreenshot(Ref* sender);

private:
	Text* text_tips_;
};

void _PromptBox(const std::string& text);

#endif	// PROMPT_BOX_H_
