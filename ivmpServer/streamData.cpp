#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "../SharedDefines/easylogging++.h"
#include "npcManager.h"
#include "npcChar.h"
#include "streamFunctions.h"
//#include "../Server/eventManager.h"
#include "sendClientRequestedData.h"
#include "worldsController.h"
#include "vehicles.h"
#include "objectsController.h"
#include "vehicleSyncDeclarations.h"
//declared at networkManager

long lastDriverCheck = 0;

void emptyDriverStatus(int id, vehicles::netVehicle& v, const char* reason)
{
	v.driverId = 0;
	mlog("!Veh %i EDS: %s", id, reason);
}

void checkFalseDrivers()
{
	for(std::map<int, vehicles::netVehicle>::iterator it = vehicles::getVehiclesBegin(); it != vehicles::getVehiclesEnd(); ++it)
	{
		if(it->second.driverId != 0 && it->second.driverId < 1000)
		{
			if(!apiPlayer::isOn(it->second.driverId)) emptyDriverStatus(it->first, it->second, "offline");
			else if(apiPlayer::get(it->second.driverId).getDriving() != it->first) emptyDriverStatus(it->first, it->second, "another");
		}
	}
}

void networkManager::streamData(networkManager::connection* con)
{
	RakNet::BitStream bsOut;

	npcManager::interpolateNpcs(con->cTime);

	worldsController::updateWorlds(con->cTime);

	if(con->cTime > lastDriverCheck)
	{
		lastDriverCheck = con->cTime + 2000;
		checkFalseDrivers();
	}

	std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(), playerEnd = players::getPlayersEnd();
	for(i; i != playerEnd; ++i)
	{
		if(!i->second.spawnReady)
		{
			continue;
		}

		if(i->second.passangerOf != 0 && vehicles::isVehicle(i->second.passangerOf))
		{
			i->second.position = vehicles::getVehicle(i->second.passangerOf).position;
			//LINFO << "Passenger pos updated";
		}
		if(i->second.camAttached != 0)
		{
			if(players::isPlayer(i->second.camAttached))
			{
				i->second.camPos = players::getPlayer(i->second.camAttached).position;
			}
			else
			{
				i->second.camAttached = 0;
			}
		}

		if(con->cTime > i->second.timeOutTime)
		{
			if(!i->second.timeOutWarned)
			{
				//LINFO << "Player " << i->second.id << " went afk";
				i->second.timeOutWarned = true;
				i->second.wentAfk();
			}
			continue;
		}

		streamPlayer(i->first, i->second, con);

		if(con->cTime > i->second.lastStaticStream + 2000)
		{
			//Steam vehicle might need to be moved to every pulse
			streamVehicle(i->second, con);
			i->second.lastStaticStream = con->cTime;
		}

		if(con->cTime > i->second.lastSelfPing)
		{
			i->second.lastSelfPing = con->cTime + 5000;

			bsOut.Reset();
			bsOut.Write((MessageID)IVMP);
			bsOut.Write(PLAYER_LIST_PING);
			bsOut.Write(i->second.id);
			bsOut.Write(con->peer->GetLastPing(i->second.ip));
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
		}

		for(std::map<int, npcChars::npcChar*>::iterator it = npcChars::getNpcBegin(); it != npcChars::getNpcEnd(); ++it)
		{
			if(it->second->vWorld != i->second.camWorld)
			{
				continue;
			}

			bool hasStreamData = i->second.sNpcs.find(it->first) != i->second.sNpcs.end();
			//LINFO << it->second->currentPos.x;
			//LINFO << apiMath::distance3d(i->second.camPos, it->second->currentPos);
			if(apiMath::distance3d(i->second.camPos, it->second->currentPos) < it->second->streamDis)
			{
				if(!hasStreamData || 
					(!i->second.sNpcs[it->first].hasSpawned && con->cTime > i->second.sNpcs[it->first].lastPacket + 10000)) {

						sendNpcData(it->first, i->second, con);

						int carId = it->second->vehicle;
						if(carId != -1 && i->second.sVehicles.find(carId) == i->second.sVehicles.end())
						{
							sendVehicleData(i->second, carId, con);
						}
						//LINFO << "Send Data";
						continue;
				}
				/*if(it->second->controllingClient == i->second.id)
				{
					it->second->currentPos = i->second.position;
					if(it->second->vehicle != -1 && vehicles::isVehicle(it->second->vehicle))
					{
						vehicles::getVehicle(it->second->vehicle).position = it->second->currentPos;
					}
					//LINFO << "@";
					continue;
				}*/
				if(it->second->isStatic)
				{
					//LINFO << "Not sending any data";
					continue;
				}
				else if(it->second->vehicle != -1 && i->second.sVehicles.find(it->second->vehicle) != i->second.sVehicles.end() && 
				vehicles::isVehicle(it->second->vehicle)) {

					//LINFO << "Sending bot vehicle sync";
					vehicles::netVehicle& pv = vehicles::getVehicle(it->second->vehicle);
					bsOut.Reset();
					bsOut.Write((MessageID)IVMP);
					bsOut.Write(CLIENT_RECEIVE_VEHICLE_SYNC);
					bsOut.Write(it->first);
					bsOut.Write(it->second->vehicle);

					bsOut.Write(pv.position.x);
					bsOut.Write(pv.position.y);
					bsOut.Write(pv.position.z);

					bsOut.Write(pv.rotation.x);
					bsOut.Write(pv.rotation.y);
					bsOut.Write(pv.rotation.z);
					//bsOut.Write(pv.rotation.W);

					//bsOut.Write(200);

					//bsOut.Write(pv.gasPedal);
					bsOut.Write(pv.steer);
					bsOut.Write(pv.breakOrGas);
					//bsOut.Write(pv.gear);
					bsOut.Write(pv.rpm);

					bsOut.Write(pv.velocity.x);
					bsOut.Write(pv.velocity.y);
					bsOut.Write(pv.velocity.z);

					bsOut.Write(pv.turnSpeed.x);
					bsOut.Write(pv.turnSpeed.y);
					bsOut.Write(pv.turnSpeed.z);

					//bsOut.Write(pv.rpm);
					//std::cout << pv.gasPedal << ", " << pv.steer << std::endl;

					con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
					continue;
				}
				bsOut.Reset();
				bsOut.Write((MessageID)IVMP);
				bsOut.Write(POSITION_UPDATE);
				bsOut.Write(it->first);

				bsOut.Write(it->second->currentPos.x);
				bsOut.Write(it->second->currentPos.y);
				bsOut.Write(it->second->currentPos.z);

				bsOut.Write(it->second->currentHeading);
				bsOut.Write(it->second->currentAnimation);

				//bsOut.Write(100);

				if(it->second->currentAnimation >= 1 && it->second->currentAnimation <= 4)
				{
					bsOut.Write(it->second->footInputs.at(it->second->currentInput).endAim.x);
					bsOut.Write(it->second->footInputs.at(it->second->currentInput).endAim.y);
					bsOut.Write(it->second->footInputs.at(it->second->currentInput).endAim.z);
					//bsOut.Write(it->second->footInputs.at(it->second->currentInput).bullets);
				}

				con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
			}
			else if(hasStreamData)
			{
				//LINFO << "Stream out";
				/*bsOut.Reset();
				bsOut.Write((MessageID)IVMP);
				bsOut.Write(PLAYER_LEFT_TO_CLIENT);
				bsOut.Write(it->first);
				con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);*/

				removeEntity(i->second.ip, PLAYER_LEFT_TO_CLIENT, it->first);

				int carId = it->second->vehicle;
				if(carId != -1 && i->second.sVehicles.find(carId) != i->second.sVehicles.end())
				{
					removeEntity(i->second.ip, CLIENT_MUST_DELETE_VEHICLE_AND_DRIVER, carId);
				}
			}
		}

		//LINFO << "Done";
	}

	//serverEvents::server::callServerPulse(con->cTime);
}
