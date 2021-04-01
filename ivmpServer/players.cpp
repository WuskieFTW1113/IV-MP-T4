#include "players.h"
#include "easylogging++.h"
//#include "../Server/apiPlayersController.h"
#include "npcChar.h"
#include "apiPlayerEvents.h"
#include "apiWorld.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "apiPlayerManager.h"
#include <algorithm>
#include <time.h>

std::map<uint64_t, players::player> pMap;

std::map<int, long> playerLeftTime;

apiPlayerEvents::pJoin join = 0;
apiPlayerEvents::pAfk afk = 0;

streamEntity::streamEntity()
{
	this->hasSpawned = false;
	this->lastPacket = 0;
	this->health = 0;
}

//std::map<int, bool> restrictedIds;

/*int getEmptyPlayerId()
{
	int id = !restrictedIds.empty() ? restrictedIds.rbegin()->first + 1 : 1;

	for(std::map<uint64_t, player>::const_iterator i = players.begin(); i != players.end(); ++i)
	{
		if(i->second.id == id)
		{
			id++;
		}
		else
		{
			break;
		}
	}

	LINFO << "Returning id: " << id;
	return id;
}*/

bool players::isPlayer(uint64_t id)
{
	if(pMap.find(id) == pMap.end())
	{
		return false;
	}
	return true;
}

uint64_t players::isPlayerIdOnline(int id)
{
	for(std::map<uint64_t, player>::const_iterator i = pMap.begin(); i != pMap.end(); ++i)
	{
		if(i->second.id == id)
		{
			return i->first;
		}
	}
	return 0;
}

players::player::player()
{
}

players::player::~player()
{
}

void players::player::wentAfk()
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_AFK);
	bsOut.Write(this->id);
	bsOut.Write(1);

	networkManager::connection* con = networkManager::getConnection();

	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(this->id != it->second.id && this->vWorld == it->second.camWorld && it->second.sPlayers.find(this->id) != it->second.sPlayers.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}

	if(afk != 0)
	{
		afk(this->id, true);
	}
}

void players::updateTimeOut(players::player& p, long newTimeOut)
{
	p.timeOutTime = newTimeOut;
	if(p.timeOutWarned)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(PLAYER_AFK);
		bsOut.Write(p.id);
		bsOut.Write(0);

		networkManager::connection* con = networkManager::getConnection();
		for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
		{
			if(p.id != it->second.id && p.vWorld == it->second.camWorld && it->second.sPlayers.find(p.id) != it->second.sPlayers.end())
			{
				con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
			}
		}

		p.timeOutWarned = false;
		if(afk != 0)
		{
			afk(p.id, false);
		}
		//LINFO << "Player " << p.id << " is no longer afk";

		players::updateWorldForPlayer(p);
	}
}

//int lastId = 1;
bool players::addPlayer(uint64_t id, RakNet::SystemAddress ip)
{
	if(pMap.find(id) != pMap.end())
	{
		mlog("!Lib: internal Player %i already exists 1", id);
		return false;
	}

	int apiid = 1;
	//lastId++;
	if(pMap.size() != 0)
	{
		std::vector<int> ids;
		for(std::map<uint64_t, players::player>::iterator i = pMap.begin(); i != pMap.end(); i++)
		{
			ids.push_back(i->second.id);
		}
		std::sort(ids.begin(), ids.end());

		size_t idsSize = ids.size();
		for(size_t i = 0; i < idsSize; i++)
		{
			if(ids[i] != apiid)
			{
				break;
			}
			apiid++;
		}
	}

	if(apiPlayer::isOn(apiid))
	{
		mlog("!Lib: Api Player %i already exists 1", apiid);
		return false;
	}

	long ctime = clock();

	player p;
	p.id = apiid;
	p.spawnReady = false;
	p.hp = 200;
	p.armor = 0;
	p.color = 0xFFFFFFFF;
	p.interior = 1;

	p.ip = ip;

	p.streaming = 200.0;
	p.lastStaticStream = 0;

	p.position.empty();
	p.camPos = p.position;

	p.skin = 10;

	p.vWorld = 1;
	p.camWorld = 1;
	p.camActive = false;
	p.camAttached = 0;

	p.movementAnim = 0;
	p.animRepeating = false;
	p.isDucking = false;

	p.lastSentPacket = 0;
	p.timeOutTime = clock() + 5000;
	p.timeOutWarned = false;
	p.packetCount = 0;
	p.lastSelfPing = 0;

	p.nick = std::string("");

	p.currentWeapon = 0;
	p.currentAmmo = 0;

	p.currentVehicle = 0;
	p.passangerOf = 0;
	p.passangerSeat = -1;

	for(unsigned int i = 0; i < 9; i++)
	{
		p.clothes[i] = 0;
	}

	for(unsigned int i = 0; i < 2; i++)
	{
		p.props[i] = -1;
	}

	apiPlayer::player apiRef(id);
	if(!apiPlayerManager::add(p.id, apiRef))
	{
		mlog("!Lib: Api Player %i already exists 2", p.id);
		if(pMap.find(id) != pMap.end())
		{
			mlog("!Lib: Deleting internal player %i", p.id);
			pMap.erase(id);
		}
		if(apiPlayer::isOn(p.id))
		{
			mlog("!Lib: Deleting api player %i", p.id);
			apiPlayerManager::remove(p.id);
		}
		return false;
	}

	if(!pMap.insert(std::make_pair(id, p)).second)
	{
		mlog("!Lib: internal Player %i already exists 2", p.id);
		return false;
	}

	player& pref = getPlayer(id);
	pref.bs = new RakNet::BitStream;

	if(join)
	{
		join(pref.id);
	}
	//LINFO << "Lib: Client connected with id: " << pref.id << ", check id: " << (pref.id == apiPlayer::get(pref.id).getID()) << 
		//", ip: "<< ip.ToString(false);

	mlog("Lib: Client connected with id : %i, check id : %i , ip: %s", pref.id,
		(int)(pref.id == apiPlayer::get(pref.id).getID()), ip.ToString(false));
	return true;
}

void players::removePlayer(uint64_t id)
{
	if(isPlayer(id))
	{
		int serverId = pMap.at(id).id;

		mlog("Lib: Client disconnected with id: %i", pMap.at(id).id);
		apiPlayerManager::remove(pMap.at(id).id);

		for(std::map<uint64_t, player>::iterator i = pMap.begin(); i != pMap.end(); ++i)
		{
			if(i->second.id != serverId && i->second.sPlayers.find(serverId) != i->second.sPlayers.end())
			{
				i->second.sPlayers.erase(serverId);
			}
		}
		delete pMap.at(id).bs;

		pMap.erase(id);
		playerLeftTime[serverId] = clock();
		return;
	}
	mlog("Lib: Client disconnected without id");
}

players::player& players::getPlayer(uint64_t id)
{
	return pMap.at(id);
}

std::map<uint64_t, players::player>::iterator players::getPlayersBegin()
{
	return pMap.begin();
}

std::map<uint64_t, players::player>::iterator players::getPlayersEnd()
{
	return pMap.end();
}

int players::playerCount()
{
	return pMap.size();
}

/*void reserveIdForNpc(int id)
{
	restrictedIds[id] = true;
}

bool isIdReservedForNpc(int id)
{
	return !(restrictedIds.find(id) == restrictedIds.end());
}*/

void apiPlayerEvents::registerPlayerJoin(apiPlayerEvents::pJoin f)
{
	join = f;
}

void players::updateWorldForPlayer(players::player& p)
{
	if(apiWorld::isWorld(p.vWorld))
	{
		apiWorld::virtualWorld* v = apiWorld::getWorld(p.vWorld);
		unsigned int hour, minutes;
		v->getTime(hour, minutes);

		RakNet::BitStream bsOut;
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(UPDATE_WORLD_TIME_WEATHER);
		bsOut.Write(v->getWeather());
		bsOut.Write(hour);
		bsOut.Write(minutes);
		bsOut.Write(v->getMinuteDuration());

		networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);

		bsOut.Reset();
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(SYNC_GAME_TIME);
		bsOut.Write((long)clock());
		networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
	}
}

void apiPlayerEvents::registerPlayerAfk(pAfk f)
{
	afk = f;
}

void players::sendPlayerDataToStreamedInPlayer(int& playerid, unsigned int& world, RakNet::BitStream& b)
{
	RakNet::RakPeerInterface* con = networkManager::getConnection()->peer;
	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(playerid != it->second.id && world == it->second.camWorld && it->second.sPlayers.find(playerid) != it->second.sPlayers.end())
		{
			con->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}