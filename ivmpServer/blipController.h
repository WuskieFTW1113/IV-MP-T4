#include "players.h"
#include "networkManager.h"

namespace blipController
{
	void sendBlipsToPlayer(players::player& p);
	void blipSpawned(networkManager::connection* con, RakNet::BitStream& b);
	void blipRemoved(networkManager::connection* con, RakNet::BitStream& b);
}