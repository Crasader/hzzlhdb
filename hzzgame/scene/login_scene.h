
#ifndef LOGIN_SCENE_H_
#define LOGIN_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "D:\code\waibao\hzzgameLHDB\hzzgame\cocos2d\cocos\network\HttpClient.h"
USING_NS_CC;
using namespace cocos2d::network;
using namespace cocos2d::ui;
using namespace std;

class LoginScene : public Layer
{
public:
	static Scene* createScene();

	LoginScene();
	virtual ~LoginScene();

	virtual bool init();

	static LoginScene* getInstance();

	CREATE_FUNC(LoginScene);

public:
	void OnClickStartGame(Ref* sender);
	void OnClickRegister(Ref* sender);
	void OnClickAccounts(Ref* sender);
	void OnClickBack(Ref* sender);
	void OnClickLoginOk(Ref* sender);

	void WaitWXCode(float dt);
	//得到微信token;
	void gettoken(string APP_Code);

	void getAccessToken();
	void jsjbacktoken(HttpClient *sender, HttpResponse *response);

	void handshakeResponse(HttpClient *sender, HttpResponse *response);

	void GetUserDataResponse(HttpClient *sender, HttpResponse *response);
	/* 读取本地文件 */
	bool getFileByName(string pFileName);

	/* 储存内容到文件 */
	bool saveFile(string pFileName);

private:
	Node* login1_;
	Node* login2_;
	Button* btn_back_;
	Button* login_startgame;
	EditBox* login2_accounts_;
	EditBox* login2_password_;
};

#endif // LOGIN_SCENE_H_

