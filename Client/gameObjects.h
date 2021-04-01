#include "NetworkManager.h"

namespace gameObjects
{
	void spawnObject(RakNet::BitStream& bsIn);
	void deleteObject(RakNet::BitStream& bsIn);
	void moveObject(RakNet::BitStream& bsIn);
	void updateStreamDistance(RakNet::BitStream& bsIn);

	void setObjectAudio(RakNet::BitStream& bsIn);

	void wipeObjects();
	void pulseObjects(long cTime); //Good idea to check if they are still spawned
}