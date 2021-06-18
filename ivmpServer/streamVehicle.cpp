#include "streamFunctions.h"
#include "vehicles.h"
#include "../SharedDefines/packetsIds.h"
#include "vehicleSyncDeclarations.h"
#include "npcChar.h"
#include "../SharedDefines/easylogging++.h"

RakNet::BitStream vOut;

bool vehicleInRange(players::player& one, vehicles::netVehicle& two, float dis)
{
	return apiMath::distance3d(one.camPos, two.position) < dis;
}

void streamVehicle(players::player& iteratingPlayer, networkManager::connection* con)
{
	std::map<int, vehicles::netVehicle>::iterator it = vehicles::getVehiclesBegin(), vEnd = vehicles::getVehiclesEnd();
	for(it; it != vEnd; ++it)
	{
		if(it->second.driverId != 0 || it->second.driverId == iteratingPlayer.id || 
			it->second.vWorld != iteratingPlayer.camWorld || it->second.streaming < 0.0f) {
				continue;
		}

		streamEntity* sVehicle = iteratingPlayer.sVehicles.find(it->first) != iteratingPlayer.sVehicles.end() ?
			&iteratingPlayer.sVehicles.at(it->first) : NULL;

		if(vehicleInRange(iteratingPlayer, it->second, it->second.streaming))
		{
			if(!sVehicle || (!sVehicle->hasSpawned && con->cTime > sVehicle->lastPacket + 5000))
			{
				sendVehicleData(iteratingPlayer, it->first, con); //Sends driver information if needed
				continue;
			}		
		}
		else if(sVehicle)
		{
			removeEntity(iteratingPlayer.ip, CLIENT_MUST_DELETE_VEHICLE_AND_DRIVER, it->first);
		}

	}
}
