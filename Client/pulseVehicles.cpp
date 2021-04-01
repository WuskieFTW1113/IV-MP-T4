#include "NetworkManager.h"
#include "CustomFiberThread.h"
#include "vehicles.h"
#include "easylogging++.h"
#include "vehicleSync.h"
#include "../SharedDefines/packetsIds.h"
#include "dummyVehicles.h"
#include "removeVehicle.h"

void pulseVehicles(float x, float y, float z)
{
	try
	{
		std::map<size_t, vehicles::netVehicle>::iterator i = vehicles::getVehiclesBegin();
		int loopCount = 0;
		long elap = clock();
		subTask = 799;
		dummyVehicles::pulse();
		subTask = 700;

		while(i != vehicles::getVehiclesEnd())
		{
			loopCount++;
			if(loopCount > 120)
			{
				LINFO << "!Error: Pulse vehicles was stuck in a loop";
				break;
			}

			subTask = 701;
			vehicles::netVehicle& v = vehicles::getVehicle(i->first);
			if(v.threadLock)
			{
				++i;
				continue;
			}

			v.threadLock = true;

			if(v.spawned && !Scripting::DoesVehicleExist(v.vehicle))
			{
				LINFO << "!Error: Vehicle " << i->first << " was spawned yet vehicle doesnt exist";
				v.spawned = false;
				v.threadLock = false;
				++i;
				continue;
			}
			subTask = 702;

			if(v.spawned && v.toBeDeleted != 0)
			{
				//LINFO << "Deleting vehicle " << i->first;
				Scripting::DeleteCar(&v.vehicle);
				emptyVehicleOccupants(i->first, false);

				if(v.toBeDeleted == 1)
				{
					RakNet::BitStream bsOut;
					bsOut.Write((MessageID)IVMP);
					bsOut.Write(CLIENT_DELETED_VEHICLE);
					bsOut.Write(i->first);
					networkManager::connection* con = networkManager::getConnection();
					con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);

					vehicles::removeVehicleByIterator(i++);
				}
				else
				{
					v.spawned = false;
					v.canBeSpawned = true;
					v.toBeDeleted = 0;
				}
				//LINFO << "Vehicle deleted";
			}

			else if(!v.spawned && v.canBeSpawned)
			{
				//LINFO << "Spawning vehicle: " << i->first;
				if(SpawnCar(v))
				{
					RakNet::BitStream bsOut;
					bsOut.Write((MessageID)IVMP);
					bsOut.Write(CLIENT_SPAWNED_VEHICLE);
					bsOut.Write(i->first);
					networkManager::connection* con = networkManager::getConnection();
					con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);
					//LINFO << "Vehicle spawned";
				}
			}
			v.threadLock = false;
			v.spawnTime += 30;
			subTask = 705;
			++i;
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (pulseVehicles): " << e.what();
	}
}