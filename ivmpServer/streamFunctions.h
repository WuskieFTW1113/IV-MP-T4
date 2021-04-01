#include "networkManager.h"
#include "players.h"

void streamPlayer(uint64_t iteratingId, players::player& iteratingPlayer, networkManager::connection* con);

void streamVehicle(players::player& iteratingPlayer, networkManager::connection* con);

void streamCheckPoint(players::player& iteratingPlayer, networkManager::connection* con);

void removeEntity(RakNet::SystemAddress& ip, int packetType, int targetId);