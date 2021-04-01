#include "networkManager.h"
#include "players.h"

void sendPlayerData(players::player& whosData, players::player& toWho, networkManager::connection* con);
void sendNpcData(int npc, players::player& toWho, networkManager::connection* con);
void decideAndSendData(int id, players::player& toWho, networkManager::connection* con); //decides if ID is a player or NPC

void sendPlayerFullData(networkManager::connection* con, RakNet::BitStream &bsIn);
void playerSpawnedPlayer(networkManager::connection* con, RakNet::BitStream &bsIn);
void playerDeletedPlayer(networkManager::connection* con, RakNet::BitStream &bsIn);