#include "NetworkManager.h"

namespace playerList
{
	void showList(bool show);
	bool isListActive();

	void addPlayer(RakNet::BitStream& bsIn);
	void removePlayer(RakNet::BitStream& bsIn);
	void pingUpdated(RakNet::BitStream& bsIn);
	void coloredUpdated(int id, unsigned int hex);
	void nameUpdated(int id, const char* name);
	//void handleKeyPress();
	void registerKeys(char* svrName);

	void scrollUp();
	void scrollDown();
}