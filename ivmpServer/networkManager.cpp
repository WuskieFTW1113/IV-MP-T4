#include "networkManager.h"
#include "RakPeer.h"
#include <map>
#include "../SharedDefines/easylogging++.h"
#include "../SharedDefines/packetsIds.h"

#include "playerConnectionState.h"
#include "receiveFootSync.h"
#include "receiveClientCredentials.h"
#include "sendClientRequestedData.h"
#include "vehicleSyncDeclarations.h"
#include "receivePlayerChat.h"
#include "vehStreamConfirmation.h"
#include "checkPointsController.h"
#include "objectsController.h"
#include "dialogManager.h"
#include "blipController.h"
#include <time.h>

typedef void (*ScriptFunction)(networkManager::connection* con, RakNet::BitStream &bsIn);
std::map<int, ScriptFunction> handlers;

std::map<int, ScriptFunction> conStates;

networkManager::connection* cCon = NULL;

void networkManager::initNetwork(int port)
{	
	cCon = new networkManager::connection;
	cCon->connectStat = 0;
	cCon->cTime = clock();

	cCon->peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor r(port, 0);
	int rs = cCon->peer->Startup(50, &r, 1);
	if(rs != RakNet::RAKNET_STARTED)
	{
		mlog("Network startup failed: %i", rs);
		return;
	}
	cCon->peer->SetMaximumIncomingConnections(1000);

	conStates.insert(std::make_pair(ID_REMOTE_NEW_INCOMING_CONNECTION, playerConnected));
	conStates.insert(std::make_pair(ID_NEW_INCOMING_CONNECTION, playerConnected));

	conStates.insert(std::make_pair(ID_REMOTE_CONNECTION_LOST, playerDisconnected));
	conStates.insert(std::make_pair(ID_REMOTE_DISCONNECTION_NOTIFICATION, playerDisconnected));
	conStates.insert(std::make_pair(ID_CONNECTION_LOST, playerDisconnected));
	handlers.insert(std::make_pair(ID_REMOTE_DISCONNECTION_NOTIFICATION, playerDisconnected)); //This is actually right, needed for /q

	handlers.insert(std::make_pair(FOOT_SYNC_TO_SERVER, clientFootSync));
	handlers.insert(std::make_pair(WEAPON_CHANGE, clientChangedWeapon));
	handlers.insert(std::make_pair(CHAR_DUCK_STATE, clientDuckStateChanged));
	handlers.insert(std::make_pair(CLIENT_DEATH, clientDeath));
	handlers.insert(std::make_pair(CLIENT_HAS_RESPAWNED, clientRespawned));
	handlers.insert(std::make_pair(SET_HEALTH, clientHpChanged));
	handlers.insert(std::make_pair(PLAYER_UPDATED_INTERIOR, clientInteriorChanged));
	handlers.insert(std::make_pair(PLAYER_WEAPONS_INVENT, clientWeaponsRecieved));
	handlers.insert(std::make_pair(GIVE_WEAPON, clientAmmoUpdated));
	handlers.insert(std::make_pair(ARMOR_CHANGE, clientArmorUpdated));
	handlers.insert(std::make_pair(CUSTOM_CHAT, clientChatUpdated));

	handlers.insert(std::make_pair(ENTERING_VEHICLE, clientEnteringVehicle));

	handlers.insert(std::make_pair(SEND_NICK_TO_SERVER, receiveClientNickName));
	handlers.insert(std::make_pair(RUN_CHECKSUM, receiveCheckSum));
	handlers.insert(std::make_pair(RUN_HACK_CHECK, receiveHackCheck));

	handlers.insert(std::make_pair(REQUEST_PLAYER_DATA, sendPlayerFullData));
	handlers.insert(std::make_pair(CLIENT_SPAWNED_PLAYER, playerSpawnedPlayer));
	handlers.insert(std::make_pair(CLIENT_DELETED_PLAYER, playerDeletedPlayer));

	handlers.insert(std::make_pair(SERVER_RECEIVE_VEHICLE_SYNC, clientVehicleSync));
	handlers.insert(std::make_pair(SERVER_RECEIVE_PASSANGER_SYNC, clientVehiclePassangerSync));
	handlers.insert(std::make_pair(PASSENGER_SYNC_WITH_POSITION, clientVehiclePassangerSyncWithPos));
	handlers.insert(std::make_pair(CLIENT_REQUEST_VEHICLE_DATA, vehicleDataRequested));
	handlers.insert(std::make_pair(CLIENT_SPAWNED_VEHICLE, clientSpawnedVehicle));
	handlers.insert(std::make_pair(CLIENT_DELETED_VEHICLE, clientDeletedVehicle));
	handlers.insert(std::make_pair(SINGLE_TYRE_POPPED, clientVehicleTyrePopped));
	handlers.insert(std::make_pair(CAR_HP_UPDATE, clientVehicleHealth));
	handlers.insert(std::make_pair(SIREN_STATE_CHANGED, clientVehicleSiren));
	handlers.insert(std::make_pair(HORN_STATE_CHANGED, clientVehicleHorn));
	handlers.insert(std::make_pair(VEHICLE_DIRT_LEVEL, clientVehicleDirt));

	handlers.insert(std::make_pair(BILATERAL_MESSAGE, receivePlayerChat));

	handlers.insert(std::make_pair(CHECK_POINT_CREATE, checkPointsController::checkPointSpawned));
	handlers.insert(std::make_pair(CHECK_POINT_DELETE, checkPointsController::checkPointDeleted));
	handlers.insert(std::make_pair(CHECK_POINT_ENTER, checkPointsController::checkPointEntered));
	handlers.insert(std::make_pair(CHECK_POINT_EXIT, checkPointsController::checkPointExit));

	handlers.insert(std::make_pair(OBJECT_CREATE, objectsController::spawnedObject));
	handlers.insert(std::make_pair(OBJECT_DELETE, objectsController::deletedObject));

	handlers.insert(std::make_pair(CLIENT_DIALOG_LIST_RESPONSE, dialogManager::listResponse));

	handlers.insert(std::make_pair(BIND_PLAYER_KEY, clientKeyPress));

	handlers.insert(std::make_pair(SPAWN_BLIP, blipController::blipSpawned));
	handlers.insert(std::make_pair(DELETE_BLIP, blipController::blipRemoved));

	handlers.insert(std::make_pair(TS_CONNECT, playerTeamSpeak));

	cCon->connectStat = 1;
}

void networkManager::closeNetwork()
{
	cCon->peer->~RakPeerInterface();
	cCon->connectStat = 0;
	delete cCon;
}

networkManager::connection* networkManager::getConnection()
{
	return cCon;
}

void networkManager::handlePackts()
{
	cCon->cTime = clock();

	for(cCon->packet = cCon->peer->Receive(); cCon->packet; cCon->peer->DeallocatePacket(cCon->packet), cCon->packet=cCon->peer->Receive())
	{
		RakNet::BitStream bsIn(cCon->packet->data, cCon->packet->length, false);
		bsIn.IgnoreBytes(sizeof(MessageID));

		if(conStates.find(cCon->packet->data[0]) != conStates.end())
		{
			conStates.at(cCon->packet->data[0])(cCon, bsIn);
			continue;
		}
		else if(cCon->packet->data[0] != IVMP)
		{
			mlog("Message with invalid identifier %i has arrived", (int)cCon->packet->data[0]);
			continue;
		}
		int messageId = -1;
		bsIn.Read(messageId);
		try
		{
			if(handlers.find(messageId) != handlers.end()) handlers.at(messageId)(cCon, bsIn);
		}
		catch(std::exception&)
		{
			mlog("Message with invalid IVMP identifier %i has arrived", messageId);
		}
	}
}

bool networkManager::isNetworkActive()
{
	if(cCon != NULL && cCon->connectStat > 0)
	{
		return true;
	}
	return false;
}
