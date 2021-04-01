#include "NetworkManager.h"

namespace gameLibs
{
	unsigned int crc(const char* path);
	void checkSum(RakNet::BitStream& bsIn);
	void moduleCheck(RakNet::BitStream& bsIn);
}