#include "streamPlayerList.h"
#include "players.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "easylogging++.h"

void streamPlayerList::playerConnected(unsigned long long int player)
{
	players::player& p = players::getPlayer(player);

	RakNet::BitStream b;
	RakNet::RakString s = p.nick.c_str();
	networkManager::connection* con = networkManager::getConnection();

	RakNet::BitStream mine;
	mine.Write((MessageID)IVMP);
	mine.Write(ADD_PLAYER_TO_LIST);
	mine.Write(p.id);
	mine.Write(s);
	mine.Write(p.color);
	mine.Write(con->peer->GetLastPing(p.ip));

	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(it->second.id != p.id)
		{
			con->peer->Send(&mine, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}

		//LINFO << "Sending " << it->second.id << " to " << p.id;
		b.Write((MessageID)IVMP);
		b.Write(ADD_PLAYER_TO_LIST);
		b.Write(it->second.id);
		s = it->second.nick.c_str();
		b.Write(s);
		b.Write(it->second.color);
		b.Write(con->peer->GetLastPing(it->second.ip));
		con->peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
		b.Reset();
	}
}

void streamPlayerList::playerDisconnected(int playerid)
{
	RakNet::BitStream b;
	networkManager::connection* con = networkManager::getConnection();
	b.Write((MessageID)IVMP);
	b.Write(REMOVE_PLAYER_FROM_LIST);
	b.Write(playerid);

	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(it->second.id != playerid)
		{
			con->peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}