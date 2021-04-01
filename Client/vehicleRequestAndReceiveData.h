#include "NetworkManager.h"

void parseVehicleData(RakNet::BitStream& bsIn);
void requestVehicleData(int vehicleId);
void parsePassangerSync(RakNet::BitStream& bsIn);

void parseVehicleTyresData(RakNet::BitStream& bsIn);
void parseVehiclePopTyre(RakNet::BitStream& bsIn);

void parseVehicleHpUpdate(RakNet::BitStream& bsIn);

void parseDebugVehicle(RakNet::BitStream& bsIn);

void parseVehiclTunes(RakNet::BitStream& bsIn);

void parseVehicleColors(RakNet::BitStream& bsIn);

void parseVehicleSiren(RakNet::BitStream& bsIn);

void parseVehicleHorn(RakNet::BitStream& bsIn);

void parseVehicleIndicators(RakNet::BitStream& bsIn);

void parseVehicleVelocity(RakNet::BitStream& bsIn);

void parseVehiclePos(RakNet::BitStream& bsIn);

void parseVehicleRot(RakNet::BitStream& bsIn);

void parseVehicleLivery(RakNet::BitStream& bsIn);

void parseVehicleDirt(RakNet::BitStream& bsIn);

void parseVehicleEngine(RakNet::BitStream& bsIn);

void parseVehicleMemBit(RakNet::BitStream& bsIn);

void parseVehiclBootAndTrunk(RakNet::BitStream& bsIn);

void parseVehiclCol(RakNet::BitStream& bsIn);