#include "networkManager.h"

void clientSpawnedVehicle(networkManager::connection* con, RakNet::BitStream &bsIn);
void clientDeletedVehicle(networkManager::connection* con, RakNet::BitStream &bsIn);