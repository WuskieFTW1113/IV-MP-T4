#include "vehicles.h"
#include "../SharedDefines/easylogging++.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
//#include "../Server/eventManager.h"
#include "vehicleSyncDeclarations.h"
#include "API/apiVehicleEvents.h"

apiVehicleEvents::vHealth healthChange = NULL;
apiVehicleEvents::vTyre tyrePop = NULL;

void clientVehicleTyrePopped(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);

	int v = 0;
	bsIn.Read(v);

	if(!vehicles::isVehicle(v))
	{
		//LINFO << "Received tyre pop for invalid vehicle: " << v;
		return;
	}

	int tyre = 0;
	bsIn.Read(tyre);

	if(tyre < 0 || tyre > 5)
	{
		//LINFO << "Received tyre pop for invalid tyre: " << tyre;
		return;
	}

	vehicles::netVehicle& veh = vehicles::getVehicle(v);
	if(veh.tyrePopped[tyre])
	{
		return; //why waste band
	}

	veh.tyrePopped[tyre] = true;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SINGLE_TYRE_POPPED);
	bsOut.Write(v);
	bsOut.Write(tyre);
	bsOut.Write(1);

	for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		if(p.id != i->second.id && veh.vWorld == i->second.camWorld && i->second.sVehicles.find(v) != i->second.sVehicles.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
		}
	}

	if(tyrePop != NULL)
	{
		tyrePop(v, tyre);
	}
	//LINFO << "Car " << v << " tyre " << tyre << " was popped";
}

void clientVehicleHealth(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);

	int v = 0;
	bsIn.Read(v);

	if(!vehicles::isVehicle(v))
	{
		//LINFO << "Received car hp for invalid vehicle: " << v;
		return;
	}

	int hp = 0;
	bsIn.Read(hp);

	if(hp < -4000 || hp > 1000)
	{
		//LINFO << "Received car hp for invalid hp: " << hp;
		return;
	}

	vehicles::netVehicle& veh = vehicles::getVehicle(v);
	if(veh.engineHealth == hp)
	{
		return; //why waste band
	}
	veh.engineHealth = hp;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CAR_HP_UPDATE);
	bsOut.Write(v);
	bsOut.Write(hp);

	for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		if(p.id != i->second.id && veh.vWorld == i->second.camWorld && i->second.sVehicles.find(v) != i->second.sVehicles.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
		}
	}

	if(healthChange != NULL)
	{
		healthChange(v, hp);
	}
	//LINFO << "Car " << v << " hp: " << hp;
}

void clientVehicleSiren(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);

	int v = 0;
	bsIn.Read(v);

	if(!vehicles::isVehicle(v))
	{
		//LINFO << "Received siren for invalid vehicle: " << v;
		return;
	}

	int status = 0;
	bsIn.Read(status);

	vehicles::netVehicle& veh = vehicles::getVehicle(v);
	veh.siren = status == 1;

	//LINFO << "Vehicle: " << v << " siren status: " << veh.siren;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SIREN_STATE_CHANGED);
	bsOut.Write(v);
	bsOut.Write(veh.siren ? 1 : 0);

	for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		if(p.id != i->second.id && veh.vWorld == i->second.camWorld && i->second.sVehicles.find(v) != i->second.sVehicles.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
		}
	}
}

void clientVehicleHorn(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);

	int v = 0;
	bsIn.Read(v);

	if(!vehicles::isVehicle(v))
	{
		//LINFO << "Received horn for invalid vehicle: " << v;
		return;
	}

	int status = 0;
	bsIn.Read(status);

	vehicles::netVehicle& veh = vehicles::getVehicle(v);
	veh.horn = status == 1;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(HORN_STATE_CHANGED);
	bsOut.Write(v);
	bsOut.Write(veh.horn? 1 : 0);

	for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		if(p.id != i->second.id && veh.vWorld == i->second.camWorld && i->second.sVehicles.find(v) != i->second.sVehicles.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
		}
	}
}

void clientVehicleDirt(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);

	int v = 0;
	bsIn.Read(v);

	if(!vehicles::isVehicle(v))
	{
		//LINFO << "Received dirt for invalid vehicle: " << v;
		return;
	}

	float status = 0;
	bsIn.Read(status);

	vehicles::netVehicle& veh = vehicles::getVehicle(v);
	veh.dirt = status;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(VEHICLE_DIRT_LEVEL);
	bsOut.Write(v);
	bsOut.Write(veh.dirt);

	for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		if(p.id != i->second.id && veh.vWorld == i->second.camWorld && i->second.sVehicles.find(v) != i->second.sVehicles.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
		}
	}
}

void apiVehicleEvents::registerHealthChange(vHealth f)
{
	healthChange = f;
}

void apiVehicleEvents::registerTyrePop(vTyre f)
{
	tyrePop = f;
}
