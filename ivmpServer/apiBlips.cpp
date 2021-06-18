#include "API/apiBlips.h"
#include <map>
#include "networkManager.h"
#include "players.h"
#include "API/apiPlayer.h"
#include "blipController.h"
#include "../SharedDefines/packetsIds.h"

struct apiBlip
{
	apiMath::Vector3 pos;
	int type;
	unsigned int color;
	unsigned int world;
	unsigned int streamDistance;
	bool showAll;
	RakNet::RakString name;
};

std::map<int, apiBlip> blips;

bool apiBlips::isValid(int id)
{
	return blips.find(id) != blips.end();
}

void makeBlipPacket(RakNet::BitStream& bs, apiBlip& b, int id)
{
	bs.Write((MessageID)IVMP);
	bs.Write(SPAWN_BLIP);
	bs.Write(id);
	bs.Write(b.pos.x);
	bs.Write(b.pos.y);
	bs.Write(b.pos.z);
	bs.Write(b.type);
	bs.Write(b.color);
	bs.Write(b.world);
	bs.Write(b.streamDistance);
	bs.Write(b.name);
}

void makeDeleteBlipPacket(RakNet::BitStream& bs, int id)
{
	bs.Write((MessageID)IVMP);
	bs.Write(DELETE_BLIP);
	bs.Write(id);
}

int apiBlips::add(apiMath::Vector3 pos, int type, unsigned int color, unsigned int world, unsigned int streamDistance, bool showAll, const char* name)
{
	apiBlip b;
	b.pos = pos;
	b.type = type;
	b.color = color;
	b.world = world;
	b.streamDistance = streamDistance;
	b.showAll = showAll;
	b.name.Clear();
	b.name = name;

	int id = 1;
	for(std::map<int, apiBlip>::iterator i = blips.begin(); i != blips.end(); ++i)
	{
		if(i->first == id)
		{
			id++;
			continue;
		}
		break;
	}

	if(showAll)
	{
		RakNet::BitStream bs;
		makeBlipPacket(bs, b, id);

		networkManager::connection* con = networkManager::getConnection();
		for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
		{
			if(it->second.spawnReady && it->second.vWorld == world)
			{
				con->peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
			}
		}
	}

	blips.insert(std::make_pair(id, b));
	return id;
}

void apiBlips::setName(int id, const char* name)
{
	blips.at(id).name = name;
}

void apiBlips::remove(int id)
{
	blips.erase(id);
	RakNet::BitStream bs;
	makeDeleteBlipPacket(bs, id);

	networkManager::connection* con = networkManager::getConnection();
	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(it->second.sBlips.find(id) != it->second.sBlips.end())
		{
			con->peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}

void apiPlayer::player::showBlip(int blipId, bool show)
{
	RakNet::BitStream bs;
	if(show)
	{
		makeBlipPacket(bs, blips.at(blipId), blipId);
	}
	else
	{
		makeDeleteBlipPacket(bs, blipId);
	}
	networkManager::getConnection()->peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void blipController::sendBlipsToPlayer(players::player& p)
{
	RakNet::BitStream bs;
	networkManager::connection* con = networkManager::getConnection();
	for(std::map<int, apiBlip>::iterator i = blips.begin(); i != blips.end(); ++i)
	{
		if(i->second.showAll && i->second.world == p.vWorld)
		{
			makeBlipPacket(bs, i->second, i->first);
			con->peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
			bs.Reset();
		}
	}
}

void blipController::blipSpawned(networkManager::connection* con, RakNet::BitStream& b)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	int id = 0;
	b.Read(id);
	if(blips.find(id) == blips.end())
	{
		return;
	}

	players::getPlayer(con->packet->guid.g).sBlips[id].hasSpawned = true;
}

void blipController::blipRemoved(networkManager::connection* con, RakNet::BitStream& b)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	int id = 0;
	b.Read(id);

	players::player& p = players::getPlayer(con->packet->guid.g);
	if(p.sBlips.find(id) != p.sBlips.end())
	{
		p.sBlips.erase(id);
	}
}
