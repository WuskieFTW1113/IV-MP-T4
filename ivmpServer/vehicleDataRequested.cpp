#include "vehicles.h"
#include "easylogging++.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "vehicleSyncDeclarations.h"
#include "sendClientRequestedData.h"
#include <sstream>

void sendVehicleData(players::player& p, int vehicleId, networkManager::connection* con)
{
	//LINFO << "Sending vehicle " << vehicleId << " data to " << p.id;

	p.sVehicles[vehicleId].hasSpawned = false;
	p.sVehicles[vehicleId].lastPacket = con->cTime;

	vehicles::netVehicle& v = vehicles::getVehicle(vehicleId);

	if(v.engineHealth < 0 && con->cTime > v.lastReceived + 4000)
	{
		v.engineHealth = -1400;
	}

	RakNet::BitStream bsOut;

	if(v.driverId != 0)
	{
		decideAndSendData(v.driverId, p, con);
	}

	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CLIENT_RECEIVE_VEHICLE_DATA);
	bsOut.Write(vehicleId);
	bsOut.Write(v.model);

	bsOut.Write(v.position.x);
	bsOut.Write(v.position.y);
	bsOut.Write(v.position.z);

	bsOut.Write(v.rotation.x);
	bsOut.Write(v.rotation.y);
	bsOut.Write(v.rotation.z);
	//bsOut.Write(v.rotation.W);

	bsOut.Write(v.colors[0]);
	bsOut.Write(v.colors[1]);
	bsOut.Write(v.colors[2]);
	bsOut.Write(v.colors[3]);

	bsOut.Write(v.engineHealth);
	bsOut.Write(v.engine);

	RakNet::RakString rs;
	std::ostringstream s;
	bool anyTune = false;
	for(unsigned int i = 0; i < 10; i++)
	{
		if(v.tunes[i])
		{
			s << i << ",";
			anyTune = true;
		}
	}
	if(anyTune)
	{
		rs = s.str().c_str();
		bsOut.Write(rs);
		//LINFO << "Sent tunes: " << rs;
	}

	con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);

	bool sendTyreData = false;
	bsOut.Reset();
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(TYRE_POPPED_FULL_DATA);
	bsOut.Write(vehicleId);

	for(unsigned int i = 0; i < 6; ++i)
	{
		if(v.tyrePopped[i])
		{
			sendTyreData = true;
		}

		bsOut.Write(v.tyrePopped[i] ? 1 : 0);
	}	

	if(sendTyreData)
	{
		con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
	}

	if(v.horn && v.driverId != 0)
	{
		bsOut.Reset();
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(HORN_STATE_CHANGED);
		bsOut.Write(vehicleId);
		bsOut.Write(1);
		con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
	}

	if(v.siren)
	{
		bsOut.Reset();
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(SIREN_STATE_CHANGED);
		bsOut.Write(vehicleId);
		bsOut.Write(1);
		con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
		//LINFO << "Sent siren status";
	}

	for(unsigned int i = 0; i < 2; i++)
	{
		if(v.indicators[i])
		{
			bsOut.Reset();
			bsOut.Write((MessageID)IVMP);
			bsOut.Write(INDICATOR_STATE_CHANGED);
			bsOut.Write(vehicleId);
			bsOut.Write(i);
			bsOut.Write(1);
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
			break;
		}
	}

	if(v.dirt > 0.0f)
	{
		bsOut.Reset();
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(VEHICLE_DIRT_LEVEL);
		bsOut.Write(vehicleId);
		bsOut.Write(v.dirt);
		con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
	}

	if(v.livery != 0)
	{
		bsOut.Reset();
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(VEHICLE_LIVERY);
		bsOut.Write(vehicleId);
		bsOut.Write(v.livery);
		con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
	}

	for(std::map<unsigned int, vehicles::vMem>::iterator i = v.membits.begin(); i != v.membits.end(); ++i)
	{
		bsOut.Reset();
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(VEHICLE_MEMBIT);
		bsOut.Write(vehicleId);
		bsOut.Write(i->first);
		bsOut.Write(i->second.mType);
		bsOut.Write(i->second.value);
		con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
	}

	if(v.trunk || v.hood)
	{
		bsOut.Reset();
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(VEHICLE_HOOD_TRUNK);
		bsOut.Write(vehicleId);
		bsOut.Write(v.hood);
		bsOut.Write(v.trunk);
		con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
	}
	//LINFO << "Sent";	
}

void vehicleDataRequested(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);

	int newVehicle = 99999;
	bsIn.Read(newVehicle);

	if(!vehicles::isVehicle(newVehicle))
	{
		//LINFO << "Received packet for invalid vehicle: " << newVehicle;
		return;
	}

	//LINFO << "Vehicle data requested: " << newVehicle;

	vehicles::netVehicle& v = vehicles::getVehicle(newVehicle);

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CLIENT_RECEIVE_VEHICLE_DATA);
	bsOut.Write(newVehicle);
	bsOut.Write(v.model);
	bsOut.Write(v.colors[0]);
	bsOut.Write(v.colors[1]);
	bsOut.Write(v.colors[2]);
	bsOut.Write(v.colors[3]);

	con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
	//LINFO << "Sent";
}