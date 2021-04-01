#include "networkManager.h"
#include "players.h"

void clientVehicleSync(networkManager::connection* con, RakNet::BitStream &bsIn);
void clientVehiclePassangerSync(networkManager::connection* con, RakNet::BitStream &bsIn);
void clientVehiclePassangerSyncWithPos(networkManager::connection* con, RakNet::BitStream &bsIn);

//vehicleDataRequested
void sendVehicleData(players::player& p, int vehicleId, networkManager::connection* con);
void vehicleDataRequested(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientVehicleTyrePopped(networkManager::connection* con, RakNet::BitStream &bsIn);
void clientVehicleHealth(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientVehicleSiren(networkManager::connection* con, RakNet::BitStream &bsIn);
void clientVehicleHorn(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientVehicleDirt(networkManager::connection* con, RakNet::BitStream &bsIn);

void removeBeingJacked(int playerid);