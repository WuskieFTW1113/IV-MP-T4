#include "NetworkManager.h"

namespace localCamera
{
	void parsePos(RakNet::BitStream& bsIn);
	void parseLookAt(RakNet::BitStream& bsIn);
	void parsePlayerAttach(RakNet::BitStream& bsIn);

	void sendCamSync(networkManager::connection* con);
	bool isActive();
	void destroy();
}