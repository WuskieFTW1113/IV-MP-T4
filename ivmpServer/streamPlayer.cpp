#include "streamFunctions.h"
#include "../SharedDefines/packetsIds.h"
#include "sendClientRequestedData.h"
#include "vehicles.h"
#include "vehicleSyncDeclarations.h"

RakNet::BitStream playerBsOut;

bool playerInRange(players::player& one, players::player& two, float dis)
{
	return apiMath::distance3d(one.camPos, two.position) < dis;
}

int getStreamCar(players::player& target)
{
	if(target.passangerOf != 0) return target.passangerOf;
	else if(target.currentVehicle != 0) return target.currentVehicle;
	return -1;
}

void removeEntity(RakNet::SystemAddress& ip, int packetType, int targetId)
{
	playerBsOut.Reset();
	playerBsOut.Write((MessageID)IVMP);
	playerBsOut.Write(packetType);
	playerBsOut.Write(targetId);
	networkManager::getConnection()->peer->Send(&playerBsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ip, false);
}


void streamPlayer(uint64_t iteratingId, players::player& iteratingPlayer, networkManager::connection* con)
{
	std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(), pEnd = players::getPlayersEnd();
	for(it; it != pEnd; ++it)
	{
		if(it->first == iteratingId || it->second.vWorld != iteratingPlayer.camWorld || it->second.streaming < 0.0f || !it->second.spawnReady 
			|| it->second.interior == 1) {
			continue;
		}

		streamEntity* sPlayer = iteratingPlayer.sPlayers.find(it->second.id) != iteratingPlayer.sPlayers.end() ? 
			&iteratingPlayer.sPlayers.at(it->second.id) : NULL;

		if(playerInRange(iteratingPlayer, it->second, it->second.streaming))
		{
			if(!sPlayer || (!sPlayer->hasSpawned && con->cTime > sPlayer->lastPacket + 10000))
			{
				sendPlayerData(it->second, iteratingPlayer, con);

				int carId = getStreamCar(it->second);
				if(carId != -1 && iteratingPlayer.sVehicles.find(carId) == iteratingPlayer.sVehicles.end())
				{
					sendVehicleData(iteratingPlayer, carId, con);
				}
				continue;
			}
			else if(it->second.passangerOf != 0)
			{
				playerBsOut.Reset();
				playerBsOut.Write((MessageID)IVMP);
				playerBsOut.Write(CLIENT_RECEIVE_PASSANGER_SYNC);
				playerBsOut.Write(it->second.id);
				playerBsOut.Write(it->second.passangerOf);
				playerBsOut.Write(it->second.passangerSeat);
				con->peer->Send(&playerBsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, iteratingPlayer.ip, false);
				continue;
			}

			if(con->cTime > sPlayer->lastPacket + 5000)
			{
				sPlayer->lastPacket = con->cTime;

				playerBsOut.Reset();
				playerBsOut.Write((MessageID)IVMP);
				playerBsOut.Write(PLAYER_LIST_PING);
				playerBsOut.Write(it->second.id);
				playerBsOut.Write(con->peer->GetLastPing(it->second.ip));
				con->peer->Send(&playerBsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, iteratingPlayer.ip, false);
			}

			if(sPlayer->health != it->second.hp)
			{
				if(sPlayer->health > it->second.hp)
				{
					playerBsOut.Reset();
					playerBsOut.Write((MessageID)IVMP);
					playerBsOut.Write(PLAYER_HP_CHANGED);
					playerBsOut.Write(it->second.id);
					con->peer->Send(&playerBsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, iteratingPlayer.ip, false);
				}
				sPlayer->health = it->second.hp;
			}

			if(it->second.currentVehicle != 0 && iteratingPlayer.sVehicles.find(it->second.currentVehicle) != iteratingPlayer.sVehicles.end() && 
				vehicles::isVehicle(it->second.currentVehicle)) {

				con->peer->Send(it->second.bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, iteratingPlayer.ip, false);

				continue;
			}
			//LINFO << "Player " << it->second.id << " to " << i->second.id;

			con->peer->Send(it->second.bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, iteratingPlayer.ip, false);

		}
		else if(sPlayer)
		{	
			removeEntity(iteratingPlayer.ip, PLAYER_LEFT_TO_CLIENT, it->second.id);

			int carId = getStreamCar(it->second);
			if(carId != -1 && iteratingPlayer.sVehicles.find(carId) != iteratingPlayer.sVehicles.end())
			{
				removeEntity(iteratingPlayer.ip, CLIENT_MUST_DELETE_VEHICLE_AND_DRIVER, carId);
			}
		}
	}
}