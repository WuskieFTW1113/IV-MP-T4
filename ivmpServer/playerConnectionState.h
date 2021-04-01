#include "networkManager.h"

void playerDisconnected(networkManager::connection* con, RakNet::BitStream &bsIn);

void playerConnected(networkManager::connection* con, RakNet::BitStream &bsIn);

void playerTeamSpeak(networkManager::connection* con, RakNet::BitStream &bsIn);