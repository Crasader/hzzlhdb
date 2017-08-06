
#ifndef ROOM_SCENE_H_
#define ROOM_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../base/basictypes.h"
#include "../kernel/game_user_manager.h"
#include "../kernel/server_kernel.h"
#include "../kernel/game_table_proxy.h"

USING_NS_CC;
using namespace cocos2d::ui;

class RoomScene : public Layer, public GameTableProxy::Delegate
{
public:
    static Scene* createScene();

	RoomScene();
	virtual ~RoomScene();

    virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
    
	CREATE_FUNC(RoomScene);

private:
	// �����û�
	virtual bool SetTableUserItem(uint16 table_id, uint16 chair_id, IGameUserItem* user_item);
	// ����״̬ 
	virtual void SetTableStatus(uint16 table_id, bool playing, bool locker);

private:
	void OnClickChair(Ref* sender);
	void OnClickQuickSitdown(Ref* sender);
	//�رշ���
	void OnClickBack(Ref* sender);
	//����
	void OnClickKick(Ref* sender);
	//����
	void OnClickKickAllUser(Ref* sender);
	//��ֹ����
	void OnClickProhibitedEntry(Ref* sender);
private:
	ListView* tablelist_;
	EditBox * id_input;

	Text * tePrize[6];
	Text * teRoomName;
	Text * teTime;
	Text * teCost;
};

#endif // ROOM_SCENE_H_

