#include "streamFunctions.h"
#include "checkPointsController.h"
#include "../SharedDefines/packetsIds.h"
#include "../SharedDefines/easylogging++.h"

RakNet::BitStream cpOut;
apiMath::Vector3 cpBufPos;

void streamCheckPoint(players::player& iteratingPlayer, networkManager::connection* con)
{
	std::map<int, apiCheckPoint::checkPoint*>::iterator it = checkPointsController::getBegin(), e = checkPointsController::getEnd();
	for(it; it != e; ++it)
	{
		it->second->copyPos(cpBufPos);
		cpOut.Reset();
		cpOut.Write((MessageID)IVMP);
		cpOut.Write(CHECK_POINT_CREATE);
		cpOut.Write(it->first);
		cpOut.Write(it->second->getWorld());
		cpOut.Write(it->second->getStream());
		cpOut.Write(it->second->getType());
		cpOut.Write(cpBufPos.x);
		cpOut.Write(cpBufPos.y);
		cpOut.Write(cpBufPos.z);
		cpOut.Write(it->second->getRadius());
		cpOut.Write(it->second->getColor());
		cpOut.Write(it->second->getBlip());
		con->peer->Send(&cpOut, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, iteratingPlayer.ip, false);
	}
}
