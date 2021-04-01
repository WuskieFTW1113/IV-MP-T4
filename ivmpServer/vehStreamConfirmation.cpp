#include "vehStreamConfirmation.h"
#include "players.h"
#include "easylogging++.h"
#include <time.h>
#include "vehicles.h"
#include "entitySpawned.h"

void clientSpawnedVehicle(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	int v = -1;
	bsIn.Read(v);

	if(v == -1 || !players::isPlayer(con->packet->guid.g) || !vehicles::isVehicle(v))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);
	p.sVehicles[v].hasSpawned = true;
	entitySpawned::spawned(p.id, 1, 1, v);
	//LINFO << "Player " << p.id << " spawned veh " << v;
}

void clientDeletedVehicle(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	int v = -1;
	bsIn.Read(v);

	if(v == -1 || !players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);
	if(p.sVehicles.find(v) != p.sVehicles.end())
	{
		p.sVehicles.erase(v);
		entitySpawned::spawned(p.id, 0, 1, v);
		//LINFO << "Player " << p.id << " deleted veh " << v;
	}
	else
	{
		//LINFO << "Player " << p.id << " delete an invalid veh " << v;
	}
}