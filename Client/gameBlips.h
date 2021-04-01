#include "NetworkManager.h"

namespace gameBlips
{
	void spawnBlip(RakNet::BitStream& bsIn);
	void deleteBlip(RakNet::BitStream& bsIn);
	void wipeBlips();
}