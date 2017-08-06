
#ifndef GAMES_GAME_INSTALLER_H_
#define GAMES_GAME_INSTALLER_H_

#include <string>

#include "../base/basictypes.h"

class GameInstaller
{
public:
	static void InstallGame(uint16 kind_id);

private:
	DISALLOW_IMPLICIT_CONSTRUCTORS(GameInstaller);
};

#endif // GAMES_GAME_INSTALLER_H_