
#ifndef DIALOG_BANK_PASSWORD_MODIFY_H_
#define DIALOG_BANK_PASSWORD_MODIFY_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../common/UIDialog.h"

USING_NS_CC;
using namespace cocos2d::ui;

class DialogBankPasswordMofify : public Dialog
{
public:
	DialogBankPasswordMofify();
	virtual ~DialogBankPasswordMofify();

    virtual bool init();
    
	CREATE_FUNC(DialogBankPasswordMofify);

private:
	void OnClickClose(Ref* sender);
	void OnClickOk(Ref* sender);

private:
	EditBox* password1_;
	EditBox* password2_;
	EditBox* password3_;
};

#endif // DIALOG_BANK_PASSWORD_MODIFY_H_

