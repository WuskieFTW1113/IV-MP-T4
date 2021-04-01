#include "NetworkManager.h"

void connectionAccepted(RakNet::BitStream& bsIn);

void connectionLost(const char* msg);

void connectionMaySpawn(RakNet::BitStream& bsIn);

int getLocalPlayerServerId();
bool getServerBlips();