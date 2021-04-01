#include "vehicles.h"

void syncVehicle(int vehicleId);
void updateMyLastPos(float x, float y, float z);
void updateEngineFlags(vehicles::netVehicle& v);
void updateVehicleBits(vehicles::netVehicle& v);
void handleVehicleDoors(vehicles::netVehicle& v);