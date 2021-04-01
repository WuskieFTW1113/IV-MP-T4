#include "networkManager.h"

void clientFootSync(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientChangedWeapon(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientDuckStateChanged(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientDeath(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientRespawned(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientHpChanged(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientInteriorChanged(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientEnteringVehicle(networkManager::connection* con, RakNet::BitStream &bsIn);
bool hasCarJack(int playerid);
void getJackData(int playerid, int& vehicle, long& time);
void removeJack(int playerid);
void callVehicleLeft(int playerid, int vehicleId, int seat);

void clientKeyPress(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientWeaponsRecieved(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientAmmoUpdated(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientArmorUpdated(networkManager::connection* con, RakNet::BitStream &bsIn);

void clientChatUpdated(networkManager::connection* con, RakNet::BitStream &bsIn);