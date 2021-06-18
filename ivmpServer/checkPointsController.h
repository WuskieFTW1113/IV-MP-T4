#include "API/apiCheckPoints.h"
#include "networkManager.h"
#include <map>

namespace checkPointsController
{
	std::map<int, apiCheckPoint::checkPoint*>::iterator getBegin();
	std::map<int, apiCheckPoint::checkPoint*>::iterator getEnd();

	void checkPointSpawned(networkManager::connection* con, RakNet::BitStream &bsIn);
	void checkPointDeleted(networkManager::connection* con, RakNet::BitStream &bsIn);
	void checkPointEntered(networkManager::connection* con, RakNet::BitStream &bsIn);
	void checkPointExit(networkManager::connection* con, RakNet::BitStream &bsIn);
}
