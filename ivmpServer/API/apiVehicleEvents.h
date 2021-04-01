#include <exporting.h>

namespace apiVehicleEvents
{	
	DLL typedef void(*vHealth)(int vehicleId, int health);
	DLL void registerHealthChange(vHealth f);

	DLL typedef void(*vTyre)(int vehicleId, unsigned int tyre);
	DLL void registerTyrePop(vTyre f);
}