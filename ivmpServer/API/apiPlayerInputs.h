#include "apiPlayer.h"
#include <exporting.h>

namespace apiPlayerInputs
{
	DLL typedef void(*keyInput)(apiPlayer::player& player, unsigned int virtualKey, bool keyUp);
	//Will be called once the player presses the virtualKey, it must be registered with apiPlayer::registerKeyHook(...)
	DLL void registerKeyInputs(keyInput f);
}