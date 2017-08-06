
#ifndef SOCKET_NOTIFY_H_
#define SOCKET_NOTIFY_H_

#include "cocos2d.h"

class SocketNotify : public cocos2d::Ref
{
public:
	static SocketNotify* getInstance();
	static void destroyInstance();

	virtual ~SocketNotify();

	void OnSocketUpdate(float dt);

private:
	SocketNotify();
};

#endif // SOCKET_NOTIFY_H_