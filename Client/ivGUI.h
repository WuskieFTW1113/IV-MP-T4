#include "NetworkManager.h"

namespace ivGUI
{
	void pulse(long cTime);
	//void clearTextures();

	void drawText(RakNet::BitStream& bsIn);
	void updateText(RakNet::BitStream& bsIn);
	void drawInfoText(RakNet::BitStream& bsIn);
	void drawRect(RakNet::BitStream& bsIn);
	void wipeDrawClass(RakNet::BitStream& bsIn);
	//void drawSprite(RakNet::BitStream& bsIn);
};