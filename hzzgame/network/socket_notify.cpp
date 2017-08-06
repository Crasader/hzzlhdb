
#include "socket_notify.h"
#include "Socket.h"

static SocketNotify *g_SocketNotify = nullptr;

SocketNotify* SocketNotify::getInstance()
{
	if (!g_SocketNotify)
	{
		g_SocketNotify = new (std::nothrow) SocketNotify();
	}

	return g_SocketNotify;
}

void SocketNotify::destroyInstance()
{
	CSocket::purge();
	CC_SAFE_RELEASE_NULL(g_SocketNotify);
}

SocketNotify::SocketNotify()
{
}

SocketNotify::~SocketNotify()
{
}

void SocketNotify::OnSocketUpdate(float dt)
{
	CSocket::post();
}
