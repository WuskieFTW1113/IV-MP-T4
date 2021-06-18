#include "vehicles.h"
#include "../SharedDefines/easylogging++.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
//#include "../Server/eventManager.h"
#include "vehicleSyncDeclarations.h"
#include "API/apiVehicleEvents.h"
#include "API/apiPlayerEvents.h"
#include "receiveFootSync.h"

apiPlayerEvents::vLeft enterDone = NULL;

struct IGotJacked
{
	int vehicle;
	long time;

	IGotJacked(int vehicle, long time)
	{
		this->vehicle = vehicle;
		this->time = time;
	}
	~IGotJacked(){}
};

std::map<int, IGotJacked> carJacked;

void removeBeingJacked(int playerid)
{
	if(carJacked.find(playerid) != carJacked.end())
	{
		carJacked.erase(playerid);
	}
}

void clientVehicleSync(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);
	p.passangerOf = 0;
	players::updateTimeOut(p, con->cTime + 2000);

	int newVehicle = 0;
	bsIn.Read(newVehicle);

	if(carJacked.find(p.id) != carJacked.end())
	{
		if(carJacked.at(p.id).vehicle != newVehicle)
		{
			carJacked.erase(p.id);
		}
		else
		{
			if(apiPlayer::isOn(p.id))
			{
				apiPlayer::get(p.id).removeFromVehicle();
				apiPlayer::get(p.id).drawInfoText("~r~You were carjacked", 3000);
			}
			return;
		}
	}

	if(!vehicles::isVehicle(newVehicle))
	{
		//LINFO << "Received packet for invalid vehicle: " << newVehicle;
		return;
	}

	vehicles::netVehicle& v = vehicles::getVehicle(newVehicle);
	if(hasCarJack(p.id))
	{
		//LINFO << "Carjacking status found: " << p.id;
		int vehicle = 0;
		long time = 0;
		getJackData(p.id, vehicle, time);
		removeJack(p.id);

		//LINFO << "CDriver: " << v.driverId << ", " << (int)(time + 12000 < con->cTime);
		if(v.driverId != p.id && time + 12000 > con->cTime) //CarJack time is still valid
		{
			uint64_t driverId = players::isPlayerIdOnline(v.driverId);
			//LINFO << "DriverId: " << driverId;
			if(driverId != 0)
			{
				/*if(carJacked.find(v.driverId) != carJacked.end())
				{
					carJacked.erase(v.driverId);
				}
				carJacked.insert(std::make_pair(v.driverId, IGotJacked(newVehicle, con->cTime)));
				LINFO << "Carjacked status added: " << v.driverId << ", " << newVehicle;*/

				players::getPlayer(driverId).currentVehicle = 0;
				callVehicleLeft(v.driverId, newVehicle, 0);

				if(apiPlayer::isOn(v.driverId))
				{
					apiPlayer::get(v.driverId).removeFromVehicle();
					apiPlayer::get(v.driverId).drawInfoText("~r~CarJacked", 2000);
				}
			}

			if(apiPlayer::isOn(p.id))
			{
				apiPlayer::get(p.id).drawInfoText("~r~You finished carjacking", 5000);
			}
		}
	}

	if(p.currentVehicle != newVehicle)
	{
		if(enterDone != NULL)
		{
			enterDone(p.id, newVehicle, 0);
		}

		//LINFO << "Lib: Player " << p.nick << " veh entry done";

		if(p.currentVehicle != 0 && vehicles::isVehicle(p.currentVehicle))
		{
			vehicles::netVehicle& ov = vehicles::getVehicle(p.currentVehicle);
			ov.driverId = 0;
			if(ov.engine == 1) ov.engine = 2;
		}
		p.currentVehicle = newVehicle;
	}
	v.driverId = p.id;
	v.lastReceived = con->cTime;

	if(v.vWorld != p.vWorld)
	{
		return;
	}
	//if(v.engine == 1) v.engine = 2;

	bsIn.Read(v.position.x);
	bsIn.Read(v.position.y);
	bsIn.Read(v.position.z);

	bsIn.Read(v.rotation.x);
	bsIn.Read(v.rotation.y);
	bsIn.Read(v.rotation.z);
	//bsIn.Read(v.rotation.W);

	bsIn.Read(v.velocity.x);
	bsIn.Read(v.velocity.y);
	bsIn.Read(v.velocity.z);

	bsIn.Read(v.turnSpeed.x);
	bsIn.Read(v.turnSpeed.y);
	bsIn.Read(v.turnSpeed.z);

	bsIn.Read(v.steer);
	//bsIn.Read(v.gasPedal);
	bsIn.Read(v.breakOrGas);
	//bsIn.Read(v.gear);
	bsIn.Read(v.rpm);

	p.position = v.position;
	if(!p.camActive)
	{
		p.camPos = p.position;
	}

	p.bs->Reset();
	p.bs->Write((MessageID)IVMP);
	p.bs->Write(CLIENT_RECEIVE_VEHICLE_SYNC);
	p.bs->Write(p.id);
	p.bs->Write(newVehicle);

	p.bs->Write(v.position.x);
	p.bs->Write(v.position.y);
	p.bs->Write(v.position.z);

	p.bs->Write(v.rotation.x);
	p.bs->Write(v.rotation.y);
	p.bs->Write(v.rotation.z);
	//p.bs->Write(v.rotation.W);

	p.bs->Write(v.steer);
	p.bs->Write(v.breakOrGas);
	p.bs->Write(v.rpm);

	p.bs->Write(v.velocity.x);
	p.bs->Write(v.velocity.y);
	p.bs->Write(v.velocity.z);

	p.bs->Write(v.turnSpeed.x);
	p.bs->Write(v.turnSpeed.y);
	p.bs->Write(v.turnSpeed.z);
}

bool normalPassengerSync(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return false;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);
	players::updateTimeOut(p, con->cTime + 4000);

	int newVehicle = 0;
	bsIn.Read(newVehicle);

	if(!vehicles::isVehicle(newVehicle))
	{
		//LINFO << "Received passanger packet for invalid vehicle: " << newVehicle;
		return false;
	}

	int seatId = -1;
	bsIn.Read(seatId);

	if((newVehicle != p.passangerOf || p.passangerSeat != seatId) && enterDone != NULL)
	{
		enterDone(p.id, newVehicle, seatId);
	}

	if(p.currentVehicle != 0 && vehicles::isVehicle(p.currentVehicle))
	{
		vehicles::netVehicle& v = vehicles::getVehicle(p.currentVehicle);
		v.driverId = 0;
		//if(v.engine == 2) v.engine = 1;
		p.currentVehicle = 0;
	}
	p.passangerOf = newVehicle;
	p.passangerSeat = seatId;
	
	p.position = vehicles::getVehicle(newVehicle).position;
	if(!p.camActive)
	{
		p.camPos = p.position;
	}
	return true;
}

void clientVehiclePassangerSync(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	normalPassengerSync(con, bsIn);
}

void clientVehiclePassangerSyncWithPos(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!normalPassengerSync(con, bsIn))
	{
		return;
	}

	int vehicle = players::getPlayer(con->packet->guid.g).passangerOf;
	vehicles::netVehicle& v = vehicles::getVehicle(vehicle);

	if(v.driverId == 0 && con->cTime > v.lastReceived + 200)
	{
		bsIn.Read(v.position.x);
		bsIn.Read(v.position.y);
		bsIn.Read(v.position.z);

		bsIn.Read(v.rotation.x);
		bsIn.Read(v.rotation.y);
		bsIn.Read(v.rotation.z);
		//bsIn.Read(v.rotation.W);
	}
}

void apiPlayerEvents::registerEnteredVehicle(vLeft f)
{
	enterDone = f;
}
