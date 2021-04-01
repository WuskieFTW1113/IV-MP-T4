#include "NetworkManager.h"
#include "RakPeer.h"
#include "players.h"
#include "easylogging++.h"
#include "simpleMath.h"
#include <time.h>

#include "parseOnFootPlayerPacket.h"
#include "parseConnectionStates.h"
#include "requestAndReceivePlayerData.h"
#include "playerDisconnected.h"

#include "vehicleRequestAndReceiveData.h"
#include "parseInVehiclePlayerPacket.h"
#include "removeVehicle.h"

#include "chatWindow.h"
#include "gameWorld.h"
#include "playerServerFunctions.h"
#include "gameObjects.h"
#include "gameCheckPoints.h"
#include "ivGUI.h"
#include "playerList.h"
#include "dialogList.h"
#include "gameBlips.h"
#include "prepareClientToServerPacket.h"
#include "localPlayerCamera.h"
#include "gameLibs.h"
#include "gameAudio.h"
#include "LuaVM.h"
#include "../SharedDefines/packetsIds.h"
#include "teamSpeak.h"

typedef void (*ScriptFunction)(RakNet::BitStream& bsIn);
std::map<int, ScriptFunction> handlers;

std::map<int, std::string> disconnects;

networkManager::connection* cCon = nullptr;

int lastPack = 0;

void getServerUpTime(RakNet::BitStream& bsIn)
{
	bsIn.Read(cCon->serverUpTime);
	LINFO << "Server time: " << cCon->serverUpTime;
}

void networkManager::initNetwork(std::string s, int port)
{	
	cCon = new networkManager::connection;
	cCon->connectStat = 0;
	cCon->lastSentPck = 0;
	cCon->lastRcvPck = 0;
	cCon->serverUpTime = 0;
	cCon->cTime = clock();

	cCon->peer = RakNet::RakPeerInterface::GetInstance();
	int rs = cCon->peer->Startup(1,  &RakNet::SocketDescriptor(), 1);
	if(rs != RakNet::RAKNET_STARTED)
	{
		LINFO << "!Error: host start: " << rs;
		connectionLost("~r~Invalid server host name");
		return;
	}

	cCon->peer->Connect(s.c_str(), port, 0, 0);
	cCon->connectStat = 1;

	cCon->lastSentPck = cCon->cTime;

	handlers.insert(std::make_pair(SYNC_GAME_TIME, getServerUpTime));

	handlers.insert(std::make_pair(POSITION_UPDATE, parseOnFootSync));
	handlers.insert(std::make_pair(WEAPON_CHANGE, parseWeaponChanged));
	handlers.insert(std::make_pair(CHAR_DUCK_STATE, parseDuckChanged));
	handlers.insert(std::make_pair(CHAR_SKIN_CHANGE, parseSkinChanged));
	handlers.insert(std::make_pair(UPDATE_COLOR, parseColorChanged));
	handlers.insert(std::make_pair(PLAYER_CLOTHES, parseClothesChanged));
	handlers.insert(std::make_pair(PLAYER_HP_CHANGED, parseHpChanged));
	handlers.insert(std::make_pair(PLAYER_SET_ANIM, parseAnimChanged));
	handlers.insert(std::make_pair(PLAYER_ANIM_LOOPING, parseAnimLooping));
	handlers.insert(std::make_pair(PLAYER_PROP_INDEX, parsePropChange));
	handlers.insert(std::make_pair(PLAYER_NAME_CHANGED, parseNameChanged));
	handlers.insert(std::make_pair(PLAYER_AFK, parseAfkChanged));
	handlers.insert(std::make_pair(PLAYER_DRAW_FLAGS, parseDrawFlags));

	//handlers.insert(std::make_pair(ID_CONNECTION_REQUEST_ACCEPTED, connectionAccepted));

	disconnects.insert(std::make_pair(ID_CONNECTION_LOST, "~r~Connection Lost"));
	disconnects.insert(std::make_pair(ID_CONNECTION_ATTEMPT_FAILED, "~r~Connection attempt failed"));
	disconnects.insert(std::make_pair(ID_DISCONNECTION_NOTIFICATION, "~r~Connection terminated"));
	disconnects.insert(std::make_pair(ID_ALREADY_CONNECTED, "~r~Already connected (terminating)"));
	disconnects.insert(std::make_pair(ID_NO_FREE_INCOMING_CONNECTIONS, "~r~Server is full"));
	disconnects.insert(std::make_pair(ID_PUBLIC_KEY_MISMATCH, "~r~Connection denied (Key)"));
	disconnects.insert(std::make_pair(ID_CONNECTION_BANNED, "~r~You are banned from this server"));
	disconnects.insert(std::make_pair(ID_INVALID_PASSWORD, "~r~Invalid passworld"));
	disconnects.insert(std::make_pair(ID_INCOMPATIBLE_PROTOCOL_VERSION, "~r~Incompatible protocol"));
	disconnects.insert(std::make_pair(ID_IP_RECENTLY_CONNECTED, "~r~The server hasnt removed your old connection yet"));
	//disconnects.insert(std::make_pair(ID_MODIFIED_PACKET, "~r~Invalid packets"));

	handlers.insert(std::make_pair(SPAWN_PLAYER, connectionMaySpawn));
	handlers.insert(std::make_pair(CHAR_GOTO_POS, gotoNewPos));
	handlers.insert(std::make_pair(ENTERING_VEHICLE, parseEnteringVehicle));

	handlers.insert(std::make_pair(RECEIVE_PLAYER_DATA, receivePlayerData));
	handlers.insert(std::make_pair(PLAYER_LEFT_TO_CLIENT, playerDisconnected));

	handlers.insert(std::make_pair(CLIENT_RECEIVE_VEHICLE_DATA, parseVehicleData));
	handlers.insert(std::make_pair(CLIENT_RECEIVE_VEHICLE_SYNC, parseInVehicleSync));
	handlers.insert(std::make_pair(CLIENT_RECEIVE_PASSANGER_SYNC, parsePassangerSync));
	handlers.insert(std::make_pair(CLIENT_MUST_DELETE_VEHICLE_AND_DRIVER, removeVehicle));
	handlers.insert(std::make_pair(DELETE_VEHICLE_ONLY, removeVehicleOnly));
	handlers.insert(std::make_pair(TYRE_POPPED_FULL_DATA, parseVehicleTyresData));
	handlers.insert(std::make_pair(SINGLE_TYRE_POPPED, parseVehiclePopTyre));
	handlers.insert(std::make_pair(CAR_HP_UPDATE, parseVehicleHpUpdate));
	handlers.insert(std::make_pair(CLIENT_DEBUG_VEHICLE, parseDebugVehicle));
	handlers.insert(std::make_pair(VEHICLE_TUNES, parseVehiclTunes));
	handlers.insert(std::make_pair(VEHICLE_COLOR_CHANGED, parseVehicleColors));
	handlers.insert(std::make_pair(SIREN_STATE_CHANGED, parseVehicleSiren));
	handlers.insert(std::make_pair(HORN_STATE_CHANGED, parseVehicleHorn));
	handlers.insert(std::make_pair(INDICATOR_STATE_CHANGED, parseVehicleIndicators));
	handlers.insert(std::make_pair(VEHICLE_FORCE_VELOCITY, parseVehicleVelocity));
	handlers.insert(std::make_pair(VEHICLE_FORCE_COORDINATES, parseVehiclePos));
	handlers.insert(std::make_pair(VEHICLE_FORCE_ROTATION, parseVehicleRot));
	handlers.insert(std::make_pair(VEHICLE_LIVERY, parseVehicleLivery));
	handlers.insert(std::make_pair(VEHICLE_DIRT_LEVEL, parseVehicleDirt));
	handlers.insert(std::make_pair(VEHICLE_MEMBIT, parseVehicleMemBit));
	handlers.insert(std::make_pair(VEHICLE_ENGINE_FLAGS, parseVehicleEngine));
	handlers.insert(std::make_pair(VEHICLE_HOOD_TRUNK, parseVehiclBootAndTrunk));
	handlers.insert(std::make_pair(VEHICLE_COL, parseVehiclCol));

	handlers.insert(std::make_pair(CLIENT_RECEIVE_PASSANGER_SYNC, parsePassangerSync));

	handlers.insert(std::make_pair(BILATERAL_MESSAGE, chatWindow::receivedChat));
	handlers.insert(std::make_pair(COLORED_MSG, chatWindow::receivedPlayerChat));
	handlers.insert(std::make_pair(CUSTOM_CHAT, chatWindow::customChatHandler));

	handlers.insert(std::make_pair(UPDATE_WORLD_TIME_WEATHER, gameWorld::receivedTime));
	handlers.insert(std::make_pair(GOTO_NEW_VIRTUAL_WORLD, gameWorld::changedWorld));

	handlers.insert(std::make_pair(GIVE_WEAPON, weaponGiven));
	handlers.insert(std::make_pair(REMOVE_WEAPON, weaponTaken));
	handlers.insert(std::make_pair(SET_HEALTH, setHealth));
	handlers.insert(std::make_pair(UPDATE_SPAWN_POS, setSpawnPos));
	handlers.insert(std::make_pair(EJECT_OUT, ejectOut));
	handlers.insert(std::make_pair(TP_INTO_VEHICLE_SEAT, tpIntoCarSeat));
	handlers.insert(std::make_pair(FREEZE_CLIENT, freezeClient));
	handlers.insert(std::make_pair(PLAYER_WEAPONS_INVENT, getClientWeapons));
	handlers.insert(std::make_pair(ARMOR_CHANGE, setArmor));
	handlers.insert(std::make_pair(SET_MONEY, setClientMoney));
	handlers.insert(std::make_pair(SET_DOOR_STAT, setClientDoor));
	handlers.insert(std::make_pair(CREATE_EXPLOSION, explodeAt));
	handlers.insert(std::make_pair(SET_SIT_IDLE_ANIM, defaultSitIdle));
	handlers.insert(std::make_pair(SIT_IDLE_NOW, taskSitNow));
	handlers.insert(std::make_pair(PLAY_SOUND, playSound));
	handlers.insert(std::make_pair(STOP_SOUND, stopSound));
	handlers.insert(std::make_pair(SET_HEADING, setHeading));
	handlers.insert(std::make_pair(PLAYER_HUD_SETTINGS, setHudSettings));

	handlers.insert(std::make_pair(CLIENT_DEBUG_PLAYER, parseDebugPlayer));
	handlers.insert(std::make_pair(BIND_PLAYER_KEY, serverKeyBind));

	handlers.insert(std::make_pair(OBJECT_CREATE, gameObjects::spawnObject));
	handlers.insert(std::make_pair(OBJECT_DELETE, gameObjects::deleteObject));
	handlers.insert(std::make_pair(OBJECT_MOVE, gameObjects::moveObject));
	handlers.insert(std::make_pair(SET_OBJ_AUDIO, gameObjects::setObjectAudio));
	handlers.insert(std::make_pair(SET_OBJECT_STREAM_DISTANCE, gameObjects::updateStreamDistance));

	handlers.insert(std::make_pair(CHECK_POINT_CREATE, gameCheckPoints::spawnCp));
	handlers.insert(std::make_pair(CHECK_POINT_DELETE, gameCheckPoints::deleteCp));
	handlers.insert(std::make_pair(SET_CHECK_POINT_BLIP_SPRITE, gameCheckPoints::setBlipSprite));
	handlers.insert(std::make_pair(SET_CHECK_POINT_SHOWING, gameCheckPoints::setShowing));
	handlers.insert(std::make_pair(SET_CHECKPOINT_STREAM_DISTANCE, gameCheckPoints::updateStreamDistance));

	handlers.insert(std::make_pair(DISPLAY_INFO_MSG, ivGUI::drawInfoText));
	handlers.insert(std::make_pair(DISPLAY_MSG, ivGUI::drawText));
	handlers.insert(std::make_pair(DISPLAY_RECT, ivGUI::drawRect));
	handlers.insert(std::make_pair(WIPE_DISPLAYS, ivGUI::wipeDrawClass));
	handlers.insert(std::make_pair(DISPLAY_MSG_UPDATE, ivGUI::updateText));

	handlers.insert(std::make_pair(ADD_PLAYER_TO_LIST, playerList::addPlayer));
	handlers.insert(std::make_pair(REMOVE_PLAYER_FROM_LIST, playerList::removePlayer));
	handlers.insert(std::make_pair(PLAYER_LIST_PING, playerList::pingUpdated));

	handlers.insert(std::make_pair(DRAW_DIALOG_LIST, dialogList::showList));

	handlers.insert(std::make_pair(SPAWN_BLIP, gameBlips::spawnBlip));
	handlers.insert(std::make_pair(DELETE_BLIP, gameBlips::deleteBlip));

	handlers.insert(std::make_pair(CAMERA_POS, localCamera::parsePos));
	handlers.insert(std::make_pair(CAMERA_LOOK, localCamera::parseLookAt));
	handlers.insert(std::make_pair(CAMERA_ATTACH, localCamera::parsePlayerAttach));

	handlers.insert(std::make_pair(RUN_CHECKSUM, gameLibs::checkSum));
	handlers.insert(std::make_pair(RUN_HACK_CHECK, gameLibs::moduleCheck));

	handlers.insert(std::make_pair(PLAY_STREAM, gameAudio::create));
	handlers.insert(std::make_pair(STOP_STREAM, gameAudio::stop));

	handlers.insert(std::make_pair(TS_CONNECT, teamSpeak::connect));
	handlers.insert(std::make_pair(TS_TRIGGER_VOICE, teamSpeak::toggle));
	handlers.insert(std::make_pair(TS_WHISPER_LIST, teamSpeak::whisper));

	//handlers.insert(std::make_pair(CLIENT_SCRIPT, LuaVM::download));
	//handlers.insert(std::make_pair(SCRIPT_MSG, LuaVM::events));
}

void networkManager::closeNetwork()
{
	cCon->connectStat = -1;
	execution::requestThreadUnload();
}

networkManager::connection* networkManager::getConnection()
{
	return cCon;
}

void networkManager::handlePackts()
{
	long clientTime = clock();
	cCon->serverUpTime += clientTime - cCon->cTime;
	cCon->cTime = clientTime;

	if(cCon->cTime >= cCon->lastRcvPck + 20)
	{
		for(cCon->packet = cCon->peer->Receive(); cCon->packet; cCon->peer->DeallocatePacket(cCon->packet), cCon->packet=cCon->peer->Receive())
		{
			RakNet::BitStream bsIn(cCon->packet->data, cCon->packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));

			if(cCon->packet->data[0] == ID_CONNECTION_REQUEST_ACCEPTED)
			{
				connectionAccepted(bsIn);
				continue;
			}
			else if(disconnects.find(cCon->packet->data[0]) != disconnects.end())
			{
				connectionLost(disconnects.at(cCon->packet->data[0]).c_str());
				continue;
			}
			else if(cCon->packet->data[0] != IVMP)
			{
				LINFO << "Message with invalid identifier " << (int)cCon->packet->data[0] << " has arrived";
				continue;
			}

			int messageId = -1;
			bsIn.Read(messageId);

			try
			{
				//LINFO << "Packet: " << (int)cCon->packet->data[0];
				lastPack = messageId;
				handlers.at(messageId)(bsIn);
				cCon->lastRcvPck = cCon->cTime;
			}
			catch(std::exception&)
			{
				LINFO << "Message with invalid IVMP identifier " << messageId << " has arrived";
			}
		}
	}
}

bool networkManager::isNetworkActive()
{
	if(cCon != nullptr && cCon->connectStat > 0 && cCon->peer->IsActive())
	{
		return true;
	}
	return false;
}

void networkManager::sendBitStream(RakNet::BitStream& b)
{
	cCon->peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, cCon->serverIp, false);
}

int networkManager::getLastPack()
{
	return lastPack;
}