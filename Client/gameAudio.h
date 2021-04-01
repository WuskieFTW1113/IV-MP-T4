#include "NetworkManager.h"
#include "simpleMath.h"

namespace gameAudio
{
	void create(RakNet::BitStream& bsIn);
	void remove();
	void stop(RakNet::BitStream& bsIn);
	void reallyStop();
	bool exist();

	void pulse(simpleMath::Vector3& v);
	
	void registerCb();
}