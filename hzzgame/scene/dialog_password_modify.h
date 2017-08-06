
#ifndef DIALOG_PASSWORD_MODIFY_H_
#define DIALOG_PASSWORD_MODIFY_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../common/UIDialog.h"

USING_NS_CC;
using namespace cocos2d::ui;

class DialogPasswordMofify : public Dialog
{
public:
	DialogPasswordMofify();
	virtual ~DialogPasswordMofify();

    virtual bool init();
    
	CREATE_FUNC(DialogPasswordMofify);

private:
	void OnClickClose(Ref* sender);
	void OnClickOk(Ref* sender);

private:
	EditBox* password1_;
	EditBox* password2_;
	EditBox* password3_;
};

#endif // DIALOG_PASSWORD_MODIFY_H_

