#include "BitStream.h"

#ifndef dialogManager_H
#define dialogManager_H

namespace dialogManager
{
	RakNet::BitStream* getList(unsigned int id);

	void listResponse(networkManager::connection* con, RakNet::BitStream &bsIn);
}

#endif