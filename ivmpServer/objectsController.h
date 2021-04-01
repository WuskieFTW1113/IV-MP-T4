#include "apiObjects.h"
#include "networkManager.h"
#include <string>

namespace objectsController
{
	void spawnedObject(networkManager::connection* con, RakNet::BitStream &bsIn);
	void deletedObject(networkManager::connection* con, RakNet::BitStream &bsIn);

	void streamObjects(networkManager::connection* con, RakNet::SystemAddress& ip);
}