#include "NetworkManager.h"

namespace chatWindow
{
	void showChatWindow(bool show);
	bool isChatWindowActive();
	bool isChatInputActive();

	void receivedChat(RakNet::BitStream& bsIn);
	void receivedPlayerChat(RakNet::BitStream& bsIn);
	void customChatHandler(RakNet::BitStream& bsIn);
	
	void toggleChatInput();

	bool pageScrollAvailable();
	void changeScroll(bool scrollUp);

	//void changeMsgScroll(bool arrowUp);

	void initKeyHook();
	void handleKeyPress();

	void inputChat(DWORD key, bool down, bool alt);

	void loadConfig();
}