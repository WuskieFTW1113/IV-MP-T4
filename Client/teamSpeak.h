#include "NetworkManager.h"

namespace teamSpeak
{
	void connect(RakNet::BitStream& bsIn);
	void disconnect();
	void whisper(RakNet::BitStream& bsIn);
	void toggle(RakNet::BitStream& bsIn);
	void windowChanged(int active);
}