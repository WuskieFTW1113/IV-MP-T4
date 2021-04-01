#include "NetworkManager.h"

void parseOnFootPlayerPacket(networkManager::connection* con);
void sendInVehiclePacket(networkManager::connection* con);

void handleEnteringCar();
void nowInsideCar();

bool isPlayerEnteringCar();
void vehicleEntryCancelled();
int getCarJack();
long getJackTime();
void vehicleRequestData(int& vehicle, int& seat);

void registerVehicleEntryKeys();

bool isExitingVehicle();

bool recentlyTped();
void interiorStreamed();

void serverKeyBind(RakNet::BitStream& bsIn);

void sendDisconnectMsg();