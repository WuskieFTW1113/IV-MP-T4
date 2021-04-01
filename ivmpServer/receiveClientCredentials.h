#include "networkManager.h"

void receiveClientNickName(networkManager::connection* con, RakNet::BitStream &bsIn);
void receiveCheckSum(networkManager::connection* con, RakNet::BitStream &bsIn);
void receiveHackCheck(networkManager::connection* con, RakNet::BitStream &bsIn);