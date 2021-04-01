#include "NetworkManager.h"
#include "simpleMath.h"

namespace gameCheckPoints
{
	void spawnCp(RakNet::BitStream& bsIn);
	void deleteCp(RakNet::BitStream& bsIn);
	void setBlipSprite(RakNet::BitStream& bsIn);
	void setShowing(RakNet::BitStream& bsIn);
	void updateStreamDistance(RakNet::BitStream& bsIn);

	void wipeCps();
	void pulse(long cTime, simpleMath::Vector3& pos);
}