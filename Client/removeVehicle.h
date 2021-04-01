#include "NetworkManager.h"

void removeVehicle(RakNet::BitStream& bsIn);
void removeVehicleOnly(RakNet::BitStream& bsIn);

void emptyVehicleOccupants(int vehicleId, bool changeStat);