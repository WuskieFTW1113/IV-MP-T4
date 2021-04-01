#include "apiPlayer.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "playerFunctions.h"
#include "apiWorld.h"
#include "dialogManager.h"
#include "easylogging++.h"
#include "blipController.h"
#include <string>
#include "streamFunctions.h"
#include "objectsController.h"

bool defaultServerBlips = true;

apiPlayer::player::player(unsigned long long engineId)
{
	this->engineId = engineId;
}

apiPlayer::player::~player(){}


float apiPlayer::player::getHeading()
{
	return players::getPlayer(engineId).heading;
}

void apiPlayer::player::setHeading(float h)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SET_HEADING);
	bsOut.Write(h);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(engineId).ip, false);
}

apiMath::Vector3 apiPlayer::player::getPos()
{
	return players::getPlayer(engineId).position;
}

void apiPlayer::player::setPos(apiMath::Vector3& pos)
{
	networkManager::connection* con = networkManager::getConnection();

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CHAR_GOTO_POS);
	bsOut.Write(pos.x);
	bsOut.Write(pos.y);
	bsOut.Write(pos.z);

	players::getPlayer(engineId).interior = 1; //cancel stream until new interior is set
	players::getPlayer(engineId).lastStaticStream = 0;

	con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(engineId).ip, false);
}

int apiPlayer::player::getID()
{
	return players::getPlayer(engineId).id;
}

void apiPlayer::player::sendMsg(const char* msg, unsigned int hex, int chatId)
{
	sendPlayerMsg(players::getPlayer(engineId), msg, hex, chatId);
}

void apiPlayer::player::sendColoredMsg(int msgId, int chatId)
{
	streamColoredMsg(players::getPlayer(engineId), msgId, chatId);
}

const char* apiPlayer::player::getNick()
{
	return players::getPlayer(engineId).nick.c_str();
}

void apiPlayer::player::setNick(const char* nick)
{
	players::player& p = players::getPlayer(this->engineId);
	p.nick = std::string(nick);

	if(p.spawnReady)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(PLAYER_NAME_CHANGED);
		bsOut.Write(p.id);
		RakNet::RakString str = p.nick.c_str();
		bsOut.Write(str);
		
		networkManager::connection* con = networkManager::getConnection();
		for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}

void apiPlayer::player::setSkin(unsigned int model)
{
	players::player& p = players::getPlayer(engineId);
	p.skin = model;

	if(!p.spawnReady)
	{
		return;
	}

	for(unsigned int i = 0; i < 9; i++)
	{
		p.clothes[i] = 0;
	}

	for(unsigned int i = 0; i < 2; i++)
	{
		p.props[i] = -1;
	}

	networkManager::connection* con = networkManager::getConnection();
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CHAR_SKIN_CHANGE);
	bsOut.Write(-2);
	bsOut.Write(model);

	con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
	//First send to the local player then to the rest

	bsOut.Reset();
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CHAR_SKIN_CHANGE);
	bsOut.Write(p.id);
	bsOut.Write(model);

	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(p.id != it->second.id && p.vWorld == it->second.camWorld && it->second.sPlayers.find(p.id) != it->second.sPlayers.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}

unsigned int apiPlayer::player::getSkin()
{
	return players::getPlayer(engineId).skin;
}

void apiPlayer::player::setWorld(unsigned int vWorld)
{
	players::player& p = players::getPlayer(engineId);
	unsigned int oldWorld = p.vWorld;
	p.vWorld = vWorld;
	p.camWorld = vWorld;
	p.lastStaticStream = 0;

	networkManager::connection* con = networkManager::getConnection();

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(GOTO_NEW_VIRTUAL_WORLD);
	bsOut.Write(vWorld);
	con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);

	players::updateWorldForPlayer(p);

	if(!p.spawnReady)
	{
		return;
	}

	bsOut.Reset();
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_LEFT_TO_CLIENT);
	bsOut.Write(p.id);

	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(p.id != it->second.id && oldWorld == it->second.camWorld && it->second.sPlayers.find(p.id) != it->second.sPlayers.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}

	//NPCs, Players and vehicles deletes will be signaled by the client but I do here just to be sure
	p.sCheckPoints.clear();
	p.sPlayers.clear();
	p.sVehicles.clear();
	p.sNpcs.clear();
	p.sObjects.clear();
	p.sBlips.clear();
	blipController::sendBlipsToPlayer(p);
}

unsigned int apiPlayer::player::getWorld()
{
	return players::getPlayer(this->engineId).vWorld;
}

void apiPlayer::player::giveWeapon(unsigned int weaponId, unsigned int ammo)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(GIVE_WEAPON);
	bsOut.Write(weaponId);
	bsOut.Write(ammo);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(engineId).ip, false);
}

void apiPlayer::player::removeWeapon(unsigned int weaponId)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(REMOVE_WEAPON);
	bsOut.Write(weaponId);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(engineId).ip, false);
}

void apiPlayer::player::setHealth(int hp)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SET_HEALTH);
	bsOut.Write(hp);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(engineId).ip, false);
}

int apiPlayer::player::getHealth()
{
	return players::getPlayer(this->engineId).hp;
}

void apiPlayer::player::setColor(unsigned int hex)
{
	players::player& p = players::getPlayer(this->engineId);
	p.color = hex;

	if(p.spawnReady)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(UPDATE_COLOR);
		bsOut.Write(p.id);
		bsOut.Write(hex);
		
		networkManager::connection* con = networkManager::getConnection();
		for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}

unsigned int apiPlayer::player::getColor()
{
	return players::getPlayer(this->engineId).color;
}

void apiPlayer::player::spawn(apiMath::Vector3& pos)
{
	players::player& p = players::getPlayer(this->engineId);
	p.spawnReady = true;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SPAWN_PLAYER);
	bsOut.Write(p.skin);

	bsOut.Write(pos.x);
	bsOut.Write(pos.y);
	bsOut.Write(pos.z);

	bsOut.Write(p.id);
	bsOut.Write(defaultServerBlips);

	networkManager::connection* con = networkManager::getConnection();
	con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);

	players::updateWorldForPlayer(p);
	blipController::sendBlipsToPlayer(p);
	objectsController::streamObjects(con, p.ip);
	streamCheckPoint(p, con);
}

void apiPlayer::player::setRespawnPos(apiMath::Vector3& pos)
{
	players::player& p = players::getPlayer(this->engineId);

	if(!p.spawnReady)
	{
		return;
	}

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(UPDATE_SPAWN_POS);
	bsOut.Write(pos.x);
	bsOut.Write(pos.y);
	bsOut.Write(pos.z);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
}

int apiPlayer::player::getDriving()
{
	return players::getPlayer(this->engineId).currentVehicle;
}

void apiPlayer::player::getPassenger(int& vehicle, int& seat)
{
	vehicle = players::getPlayer(this->engineId).passangerOf;
	seat = players::getPlayer(this->engineId).passangerSeat;
}

int apiPlayer::player::isInVehicle()
{
	players::player p = players::getPlayer(this->engineId);
	return p.currentVehicle > 0 ? p.currentVehicle : p.passangerOf;
}

void apiPlayer::player::putInVehicle(int vehicleId, int seatId)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(TP_INTO_VEHICLE_SEAT);
	bsOut.Write(vehicleId);
	bsOut.Write(seatId);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::removeFromVehicle()
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(EJECT_OUT);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::drawText(int classId, float x, float y, float sx, float sy, const char* txt, int time, unsigned int color)
{
	RakNet::BitStream bsOut;
	RakNet::RakString tmsg = txt;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(DISPLAY_MSG);
	bsOut.Write(classId);
	bsOut.Write(x);
	bsOut.Write(y);
	bsOut.Write(sx);
	bsOut.Write(sy);
	bsOut.Write(tmsg);
	bsOut.Write(time);
	bsOut.Write(color);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::updateText(int classId, const char* txt, unsigned int newColor)
{
	RakNet::BitStream bsOut;
	RakNet::RakString tmsg;
	tmsg.Clear();
	tmsg = txt;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(DISPLAY_MSG_UPDATE);
	bsOut.Write(classId);
	bsOut.Write(tmsg);
	if(newColor != 0)
	{
		bsOut.Write(newColor);
	}
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::drawInfoText(const char* txt, int time)
{
	RakNet::BitStream bsOut;
	RakNet::RakString tmsg = txt;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(DISPLAY_INFO_MSG);
	bsOut.Write(tmsg);
	bsOut.Write(time);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::drawRect(int classId, float x, float y, float sx, float sy, unsigned int color)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(DISPLAY_RECT);
	bsOut.Write(classId);
	bsOut.Write(x);
	bsOut.Write(y);
	bsOut.Write(sx);
	bsOut.Write(sy);
	bsOut.Write(color);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::wipeDrawClass(int classId)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(WIPE_DISPLAYS);
	bsOut.Write(classId);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

int apiPlayer::player::getWeapon()
{
	return players::getPlayer(this->engineId).currentWeapon;
}

int apiPlayer::player::getAmmo()
{
	return players::getPlayer(this->engineId).currentAmmo;
}

void apiPlayer::player::setFrozen(bool b)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(FREEZE_CLIENT);
	int r = b ? 1 : 0;
	bsOut.Write(r);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::debugPlayer(int playerid)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CLIENT_DEBUG_PLAYER);
	bsOut.Write(playerid);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::debugVehicle(int vehicleid)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CLIENT_DEBUG_VEHICLE);
	bsOut.Write(vehicleid);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::disconnect()
{
	networkManager::getConnection()->peer->CloseConnection(players::getPlayer(this->engineId).ip, false, 0, LOW_PRIORITY);
}

int apiPlayer::player::getPing(bool average)
{
	if(average)
	{
		return networkManager::getConnection()->peer->GetAveragePing(players::getPlayer(this->engineId).ip);
	}
	return networkManager::getConnection()->peer->GetLastPing(players::getPlayer(this->engineId).ip);
}

int apiPlayer::player::getAnim()
{
	return players::getPlayer(this->engineId).movementAnim;
}

void apiPlayer::player::showDialogList(unsigned int dialogId)
{
	networkManager::getConnection()->peer->Send(dialogManager::getList(dialogId), 
		HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::setClothes(unsigned int part, unsigned int value)
{
	if(part < 0 || part > 8)
	{
		mlog("Clothes index out of bound: %u", part);
		return;
	}

	networkManager::connection* con = networkManager::getConnection();
	players::player& p = players::getPlayer(this->engineId);
	p.clothes[part] = value;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_CLOTHES);
	bsOut.Write(-2);
	bsOut.Write(part);
	bsOut.Write(value);
	con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);

	bsOut.Reset();
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_CLOTHES);
	bsOut.Write(p.id);
	bsOut.Write(part);
	bsOut.Write(value);
	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(p.id != it->second.id && p.vWorld == it->second.camWorld && it->second.sPlayers.find(p.id) != it->second.sPlayers.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}

unsigned int apiPlayer::player::getClothes(unsigned int part)
{
	return players::getPlayer(this->engineId).clothes[part];
}

void apiPlayer::player::setProperty(unsigned int part, unsigned int value)
{
	if(part < 0 || part > 1)
	{
		mlog("Property index out of bound: %u", part);
		return;
	}

	networkManager::connection* con = networkManager::getConnection();
	players::player& p = players::getPlayer(this->engineId);
	p.props[part] = value;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_PROP_INDEX);
	bsOut.Write(-2);
	bsOut.Write(part);
	bsOut.Write(value);
	con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);

	bsOut.Reset();
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_PROP_INDEX);
	bsOut.Write(p.id);
	bsOut.Write(part);
	bsOut.Write(value);
	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(p.id != it->second.id && p.vWorld == it->second.camWorld && it->second.sPlayers.find(p.id) != it->second.sPlayers.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}

unsigned int apiPlayer::player::getProperty(unsigned int part)
{
	return players::getPlayer(this->engineId).props[part];
}

void apiPlayer::player::forceAnim(const char* animGroup, const char* animName, bool loop)
{
	RakNet::RakString group = animGroup;
	RakNet::RakString name = animName;

	//p.animRepeating = loop;
	
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_SET_ANIM);
	bsOut.Write(group);
	bsOut.Write(name);
	bsOut.Write(loop ? 1 : 0);	

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::setSitIdleAnim(const char* animGroup, const char* animName, int num)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SET_SIT_IDLE_ANIM);
	bsOut.Write(animGroup);
	bsOut.Write(animName);
	bsOut.Write(num);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::forceSitNow()
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SIT_IDLE_NOW);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::stopAnims()
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_SET_ANIM);
	RakNet::RakString group = "n";
	bsOut.Write(group);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

const char* apiPlayer::player::getIp()
{
	return players::getPlayer(this->engineId).ip.ToString(false);
}

void apiPlayer::player::cam_setPos(apiMath::Vector3* pos, unsigned int camWorld)
{
	players::player& p = players::getPlayer(this->engineId);
	p.camWorld = camWorld;
	if(pos == NULL) return;
	p.camPos.x = pos->x, p.camPos.y = pos->y, p.camPos.z = pos->z;
	p.camActive = true;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CAMERA_POS);
	bsOut.Write(pos->x);
	bsOut.Write(pos->y);
	bsOut.Write(pos->z);
	bsOut.Write(camWorld);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
}

void apiPlayer::player::cam_setLookAt(apiMath::Vector3 pos)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CAMERA_LOOK);
	bsOut.Write(pos.x);
	bsOut.Write(pos.y);
	bsOut.Write(pos.z);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::cam_attachOnPlayer(int playerid)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CAMERA_ATTACH);
	bsOut.Write(playerid == -1 ? -2 : playerid);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);

	if(playerid == -1)
	{
		players::getPlayer(this->engineId).camActive = false;
		players::getPlayer(this->engineId).camAttached = 0;
	}
	else
	{
		players::getPlayer(this->engineId).camAttached = players::isPlayerIdOnline(playerid);
	}
}

void apiPlayer::player::checkSum(const char* file)
{
	RakNet::BitStream bsOut;
	RakNet::RakString s = file;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(RUN_CHECKSUM);
	bsOut.Write(s);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);	
	//..\\common\\data\\handling.dat
}

void apiPlayer::player::checkHacks()
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(RUN_HACK_CHECK);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

bool apiPlayer::player::isObjectSpawned(int id)
{
	players::player& p = players::getPlayer(this->engineId);
	return p.sObjects.find(id) != p.sObjects.end() && p.sObjects.at(id).hasSpawned;
}

bool apiPlayer::player::isVehicleSpawned(int id)
{
	players::player& p = players::getPlayer(this->engineId);
	return p.sVehicles.find(id) != p.sVehicles.end() && p.sVehicles.at(id).hasSpawned;
}

void apiPlayer::player::requestWeapons()
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_WEAPONS_INVENT);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);	
}

unsigned int apiPlayer::player::getWeaponsSize()
{
	return players::getPlayer(this->engineId).playerWeapons.size();
}

void apiPlayer::player::getWeaponData(unsigned int index, int& weapon, int& ammo)
{
	players::player& p = players::getPlayer(this->engineId);
	weapon = p.playerWeapons.at(index).weapon;
	ammo = p.playerWeapons.at(index).ammo;
}

unsigned int apiPlayer::player::getSerialSize()
{
	return players::getPlayer(this->engineId).serials.size();
}

const char* apiPlayer::player::getSerial(unsigned int i)
{
	return players::getPlayer(this->engineId).serials.at(i).c_str();
}

unsigned int apiPlayer::player::getRoomKey()
{
	return players::getPlayer(this->engineId).interior;
}

void apiPlayer::player::playStream(const char* stream, apiMath::Vector3* pos)
{
	RakNet::BitStream b;
	RakNet::RakString s = stream;
	b.Write((MessageID)IVMP);
	b.Write(PLAY_STREAM);
	b.Write(s);
	if(pos == 0)
	{
		b.Write(-1.0f);
	}
	else
	{
		b.Write(pos->x);
		b.Write(pos->y);
		b.Write(pos->z);
	}
	networkManager::getConnection()->peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);	
}
void apiPlayer::player::stopStreams()
{
	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(STOP_STREAM);
	networkManager::getConnection()->peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::setArmor(int i)
{
	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(ARMOR_CHANGE);
	b.Write(i);
	networkManager::getConnection()->peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

int apiPlayer::player::getArmor()
{
	return players::getPlayer(this->engineId).armor;
}

void apiPlayer::player::setMoney(int i)
{
	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(SET_MONEY);
	b.Write(i);
	networkManager::getConnection()->peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::setDoorStat(unsigned int model, apiMath::Vector3* pos, bool status)
{
	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(SET_DOOR_STAT);
	b.Write(model);
	if(model != 0)
	{
		b.Write(pos->x);
		b.Write(pos->y);
		b.Write(pos->z);
	}
	b.Write((int)status);
	networkManager::getConnection()->peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

int apiPlayer::player::getGame()
{
	return players::getPlayer(this->engineId).c_Game;
}

void apiPlayer::player::customChat(int chat_id, int operation)
{
	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(CUSTOM_CHAT);
	b.Write(chat_id);
	b.Write(operation);
	networkManager::getConnection()->peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::sendMsgToAll(const char* msg, unsigned int hex, int chatId)
{
	sendAllMsg(msg, hex, chatId);
}

void apiPlayer::sendColoredMsgToAll(int msgId, int chatId)
{
	sendAllColoredMsg(msgId, chatId);
}

void apiPlayer::player::explodeAt(apiMath::Vector3& pos, int typ, float radius, bool audio, bool invisible, float cam)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CREATE_EXPLOSION);
	bsOut.Write(pos.x);
	bsOut.Write(pos.y);
	bsOut.Write(pos.z);
	bsOut.Write(typ);
	bsOut.Write(radius);
	bsOut.Write(cam);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::playSound(const char* name, int uniqueId, apiMath::Vector3* pos)
{
	RakNet::BitStream bsOut;
	RakNet::RakString rs;
	rs.Clear();
	rs = name;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAY_SOUND);
	bsOut.Write(uniqueId);
	bsOut.Write(name);
	bsOut.Write(pos != 0);
	if(pos != 0)
	{
		bsOut.Write(pos->x);
		bsOut.Write(pos->y);
		bsOut.Write(pos->z);
	}
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::stopSound(int uniqueId)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(STOP_SOUND);
	bsOut.Write(uniqueId);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::ts_toggleVoice(bool b)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(TS_TRIGGER_VOICE);
	bsOut.Write(b);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::ts_allowVoiceTo(int playerId, bool b)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(TS_WHISPER_LIST);
	bsOut.Write(playerId);
	bsOut.Write(b);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::ts_connect(const char* ip)
{
	RakNet::BitStream bsOut;
	RakNet::RakString s;
	s.Clear();
	s = ip;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(TS_CONNECT);
	bsOut.Write(s);
	bsOut.Write(this->getID());

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::setVehicleCol(int vehicleid, bool col)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(VEHICLE_COL);
	bsOut.Write(vehicleid);
	bsOut.Write(col);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::setDrawFlags(int playerid, bool blip, bool nameDraw)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_DRAW_FLAGS);
	bsOut.Write(playerid);
	bsOut.Write(blip);
	bsOut.Write(nameDraw);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

/*bool apiPlayer::player::getDrawFlag(int flag)
{
	return players::getPlayer(this->engineId).drawFlags[flag];
}*/

void apiPlayer::player::setHudFlags(bool hud, bool radar)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_HUD_SETTINGS);
	bsOut.Write(hud);
	bsOut.Write(radar);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::copyAim(float* f)
{
	players::player& p = players::getPlayer(this->engineId);
	f[0] = p.aim.x, f[1] = p.aim.y, f[2] = p.aim.z;
}

void apiPlayer::player::setStreamDistance(float f)
{
	players::getPlayer(this->engineId).streaming = f;
}

/*void apiPlayer::player::drawSprite(int id, const char* txdClass, const char* txd, float posX, float posY,
	float sizeX, float sizeY, float rot, unsigned int color) {

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_SPRITE_DRAW);
	bsOut.Write(id);
	RakNet::RakString s;
	s.Clear();
	s = txdClass;
	bsOut.Write(s);
	s = txd;
	bsOut.Write(s);
	bsOut.Write(posX);
	bsOut.Write(posY);
	bsOut.Write(sizeX);
	bsOut.Write(sizeY);
	bsOut.Write(rot);
	bsOut.Write(color);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayer::player::deleteSprite(int id)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_SPRITE_DRAW);
	bsOut.Write(id * -1);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}*/

void apiPlayer::setDefaultServerBlips(bool b)
{
	defaultServerBlips = b;
}