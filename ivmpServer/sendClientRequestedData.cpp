#include "sendClientRequestedData.h"
#include "npcChar.h"
#include "../SharedDefines/easylogging++.h"
#include "../SharedDefines/packetsIds.h"
#include "API/playerList.h"
#include "entitySpawned.h"

void sendPlayerData(players::player& whosData, players::player& toWho, networkManager::connection* con)
{
	//LINFO << "Sending player " << whosData.id << " data to " << toWho.id;
	toWho.sPlayers[whosData.id].lastPacket = con->cTime;
	toWho.sPlayers[whosData.id].hasSpawned = false;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(RECEIVE_PLAYER_DATA);
	bsOut.Write(whosData.id);
						
	RakNet::RakString name;
	name = whosData.nick.c_str();
	bsOut.Write(name);

	bsOut.Write(whosData.skin);
	bsOut.Write(whosData.color);

	bsOut.Write(whosData.position.x);
	bsOut.Write(whosData.position.y);
	bsOut.Write(whosData.position.z);

	bsOut.Write(whosData.interior);

	bsOut.Write(whosData.heading);

	bsOut.Write(whosData.movementAnim);
	//bsOut.Write(whosData.animRepeating ? 1 : 0);

	bsOut.Write(whosData.currentWeapon);

	con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, toWho.ip, false);

	for(unsigned int i = 0; i < 9; i++)
	{
		if(whosData.clothes[i] != 0)
		{
			bsOut.Reset();
			bsOut.Write((MessageID)IVMP);
			bsOut.Write(PLAYER_CLOTHES);
			bsOut.Write(whosData.id);
			bsOut.Write(i);
			bsOut.Write(whosData.clothes[i]);
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, toWho.ip, false);
		}
	}

	for(unsigned int i = 0; i < 2; i++)
	{
		if(whosData.props[i] != -1)
		{
			bsOut.Reset();
			bsOut.Write((MessageID)IVMP);
			bsOut.Write(PLAYER_PROP_INDEX);
			bsOut.Write(whosData.id);
			bsOut.Write(i);
			bsOut.Write(whosData.props[i]);
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, toWho.ip, false);
		}
	}

	//LINFO << "Sent player data";
}

void sendNpcData(int npc, players::player& toWho, networkManager::connection* con)
{
	//LINFO << "Sending NPC " << npc << " data to " << toWho.id;	
	npcChars::npcChar* pd = npcChars::getNpc(npc);

	toWho.sNpcs[npc].lastPacket = con->cTime;
	toWho.sNpcs[npc].hasSpawned = false;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(RECEIVE_PLAYER_DATA);
	bsOut.Write(npc);
						
	RakNet::RakString name;
	name = pd->nick.c_str();
	bsOut.Write(name);
	bsOut.Write(pd->model);
	bsOut.Write(0xb71141FF);

	bsOut.Write(pd->currentPos.x);
	bsOut.Write(pd->currentPos.y);
	bsOut.Write(pd->currentPos.z);

	bsOut.Write(0); //interior
	bsOut.Write(pd->currentHeading);
	bsOut.Write(0); //anim
	bsOut.Write(pd->currentWeapon);

	con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, toWho.ip, false);

	for(unsigned int i = 0; i < 9; i++)
	{
		if(pd->clothes[i] != 0)
		{
			bsOut.Reset();
			bsOut.Write((MessageID)IVMP);
			bsOut.Write(PLAYER_CLOTHES);
			bsOut.Write(npc);
			bsOut.Write(i);
			bsOut.Write(pd->clothes[i]);
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, toWho.ip, false);
		}
	}

	for(unsigned int i = 0; i < 2; i++)
	{
		if(pd->props[i] != -1)
		{
			bsOut.Reset();
			bsOut.Write((MessageID)IVMP);
			bsOut.Write(PLAYER_PROP_INDEX);
			bsOut.Write(npc);
			bsOut.Write(i);
			bsOut.Write(pd->props[i]);
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, toWho.ip, false);
		}
	}
	//LINFO << "Sent NPC data";
}

void decideAndSendData(int id, players::player& toWho, networkManager::connection* con)
{
	if(id > 999 && npcChars::isNpc(id))
	{
		sendNpcData(id, toWho, con);
		return;
	}

	uint64_t isPlayer = players::isPlayerIdOnline(id);
	if(isPlayer != 0)
	{
		sendPlayerData(players::getPlayer(isPlayer), toWho, con);
	}
}

void sendPlayerFullData(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	int requestedPlayer;
	bsIn.Read(requestedPlayer);

	//LINFO << "Player data requested for id " << requestedPlayer;
	decideAndSendData(requestedPlayer, players::getPlayer(con->packet->guid.g), con);
}

void playerSpawnedPlayer(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	int id = -1;
	bsIn.Read(id);

	if(id > 999 && npcChars::isNpc(id))
	{
		players::player& p = players::getPlayer(con->packet->guid.g);
		p.sNpcs[id].lastPacket = con->cTime;
		p.sNpcs[id].hasSpawned = true;
		entitySpawned::spawned(p.id, 1, 4, id);
		return;
	}

	uint64_t isPlayer = players::isPlayerIdOnline(id);
	if(isPlayer != 0)
	{
		players::player& p = players::getPlayer(con->packet->guid.g);
		p.sPlayers[id].lastPacket = con->cTime;
		p.sPlayers[id].hasSpawned = true;
		p.sPlayers[id].health = players::getPlayer(isPlayer).hp;

		entitySpawned::spawned(p.id, 1, 0, id);
		//LINFO << "Player " << p.id << " spawned player " << id;
	}
}

void playerDeletedPlayer(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	int id = -1;
	bsIn.Read(id);

	if(id > 999 && npcChars::isNpc(id))
	{
		players::player& p = players::getPlayer(con->packet->guid.g);
		if(p.sNpcs.find(id) != p.sNpcs.end())
		{
			p.sNpcs.erase(id);
			entitySpawned::spawned(p.id, 0, 4, id);
			//LINFO << "Player " << p.id << " deleted npc " << id;
		}
		return;
	}

	uint64_t isPlayer = players::isPlayerIdOnline(id);
	if(isPlayer != 0)
	{
		players::player& p = players::getPlayer(con->packet->guid.g);
		if(p.sPlayers.find(id) != p.sPlayers.end())
		{
			p.sPlayers.erase(id);
			entitySpawned::spawned(p.id, 0, 0, id);
			//LINFO << "Player " << p.id << " deleted player " << id;
		}
	}
}
