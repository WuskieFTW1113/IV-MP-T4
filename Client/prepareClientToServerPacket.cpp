#include "prepareClientToServerPacket.h"
#include "players.h"
#include "vehicles.h"
#include "parseOnFootPlayerPacket.h"
#include "simpleMath.h"
#include "easylogging++.h"
#include "../SharedDefines/packetsIds.h"

float lastX = 100000.0;
float lastY = 0.0;
float lastZ = 0.0;

clock_t lastVehicleSync = 0;

int lastVehicleHp = 0;
clock_t lastVehicleHpSync = 0;

bool wasDucking = false;

MessageID lastMessage = 0;

void parseOnFootPlayerPacket(networkManager::connection* con)
{
	try
	{	
		players::netPlayer& p = players::getLocalPlayer();

		if(p.weaponChanged)
		{
			RakNet::BitStream bsOut;
			bsOut.Write((MessageID)IVMP);
			bsOut.Write(WEAPON_CHANGE);
			bsOut.Write(p.currentWeapon);
			bsOut.Write(p.currentAmmo);
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);

			p.weaponChanged = false;
		}

		if((p.isDucking && !wasDucking) || (!p.isDucking && wasDucking))
		{
			RakNet::BitStream bsOut;
			bsOut.Write((MessageID)IVMP);
			bsOut.Write(CHAR_DUCK_STATE);
			bsOut.Write(p.isDucking ? 1 : 0);
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);

			wasDucking = p.isDucking;
		}

		/*if(p.passangerOf != 0 && p.passangerSeat != -1 && (con->cTime > con->lastSentPck + 300 || lastMessage != SERVER_RECEIVE_PASSANGER_SYNC))
		{
			RakNet::BitStream bsOut;
			bsOut.Write((MessageID)IVMP);
			bsOut.Write(SERVER_RECEIVE_PASSANGER_SYNC);
			bsOut.Write(p.passangerOf);
			bsOut.Write(p.passangerSeat);
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);

			lastMessage = SERVER_RECEIVE_PASSANGER_SYNC;
			con->lastSentPck = con->cTime;
		}*/
		if(p.passangerOf == 0 && (con->cTime >= con->lastSentPck + 100 || lastMessage != FOOT_SYNC_TO_SERVER))
		{
			RakNet::BitStream bsOut;
			bsOut.Write((MessageID)IVMP);
			bsOut.Write(FOOT_SYNC_TO_SERVER);
			bsOut.Write(p.pos.x);
			bsOut.Write(p.pos.y);
			bsOut.Write(p.pos.z);
			bsOut.Write(p.heading);

			if(p.currentAmmo != p.startAmmo && p.currentAnim == 1)
			{
				p.currentAnim = 2;
			}
			bsOut.Write(p.currentAnim);

			if(p.currentAnim >= 1 && p.currentAnim <= 4)
			{
				bsOut.Write(p.aim.x);
				bsOut.Write(p.aim.y);
				bsOut.Write(p.aim.z);
				bsOut.Write(p.currentAmmo);
			}
			p.startAmmo = p.currentAmmo;

			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);
			lastMessage = FOOT_SYNC_TO_SERVER;
			con->lastSentPck = con->cTime;
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error: (parseOnFootPlayerPacket): " << e.what();
	}
}

void sendInVehiclePacket(networkManager::connection* con)
{
	try
	{
		if(clock() >= con->lastSentPck + 100 && !isExitingVehicle())
		{	
			players::netPlayer& p = players::getLocalPlayer();
			vehicles::netVehicle& v = vehicles::getVehicle(p.inVehicle);

			if(lastVehicleSync == 0)
			{
				lastVehicleSync = con->cTime - 50;
			}

			RakNet::BitStream bsOut;
			bsOut.Write((MessageID)IVMP);
			bsOut.Write(SERVER_RECEIVE_VEHICLE_SYNC);
			bsOut.Write(p.inVehicle);
			bsOut.Write(v.position.x);
			bsOut.Write(v.position.y);
			bsOut.Write(v.position.z);

			bsOut.Write(v.rotation.x);
			bsOut.Write(v.rotation.y);
			bsOut.Write(v.rotation.z);
			//bsOut.Write(v.rotation.W);

			bsOut.Write(v.speed.x);
			bsOut.Write(v.speed.y);
			bsOut.Write(v.speed.z);

			bsOut.Write(v.turnSpeed.x);
			bsOut.Write(v.turnSpeed.y);
			bsOut.Write(v.turnSpeed.z);

			bsOut.Write(v.steer);
			//bsOut.Write(v.gPedal);
			bsOut.Write(v.breakOrGas);
			//bsOut.Write(v.gear);
			bsOut.Write(v.rpm);

			//bsOut.Write((int)(con->cTime - lastVehicleSync));
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);

			con->lastSentPck = con->cTime;
			lastVehicleSync = con->cTime;

			lastMessage = SERVER_RECEIVE_VEHICLE_SYNC;

			if(con->cTime > lastVehicleHpSync + 200 && lastVehicleHp != v.engineHealth)
			{
				lastVehicleHpSync = con->cTime;
				lastVehicleHp = v.engineHealth;

				bsOut.Reset();
				bsOut.Write((MessageID)IVMP);
				bsOut.Write(CAR_HP_UPDATE);
				bsOut.Write(p.inVehicle);
				bsOut.Write(v.engineHealth);
				con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);
			}

			for(unsigned int i = 0; i < 6; ++i)
			{
				if(v.tyrePopped[i][1])
				{
					v.tyrePopped[i][1] = false;
					bsOut.Reset();
					bsOut.Write((MessageID)IVMP);
					bsOut.Write(SINGLE_TYRE_POPPED);
					bsOut.Write(p.inVehicle);
					bsOut.Write(i);
					con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);
				}
			}
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error: (sendInVehiclePacket): " << e.what();
	}
}