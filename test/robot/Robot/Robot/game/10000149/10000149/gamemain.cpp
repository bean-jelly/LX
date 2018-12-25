#include "gamemain.h"
#include "gamelogic.h"

extern "C"
{
	HANDLE BASICGAME_API WINAPI CreateGameInterface(UINT uVer)
	{
		return new  gamelogic();
	}
}
