#include "NetworkManager.h"

namespace gameWorld
{
	void receivedTime(RakNet::BitStream& bsIn);
	void changedWorld(RakNet::BitStream& bsIn);

	void setLocalSpawnCoords(float x, float y, float z);
	void getLocalSpawnCoords(float &x, float &y, float &z);

	void setPlayerFrozen(bool b);
	bool isPlayerFrozen();

	void pulseCurrentWorld(long cTime);

	void resetWeather();

	unsigned int getCurrentWorld();
}