#include "chatWindow.h"
#include "../SharedDefines/packetsIds.h"
#include "prepareClientToServerPacket.h"

#include "CustomFiberThread.h"
#include "guiManager.h"
#include "vehicles.h"
#include "players.h"
#include "playerList.h"
#include "keyboard.h"
#include "easylogging++.h"

MSG* msg = NULL;

struct keyStat
{
	bool signWhatState;
	keyboard::keyState func;
};

std::map<unsigned int, std::vector<keyStat> >keyStates;

void keyboard::registerFunc(unsigned int vKey, keyState ks, bool signWhatState)
{
	if(keyStates.find(vKey) == keyStates.end())
	{
		std::vector<keyStat> buf;
		keyStates.insert(std::make_pair(vKey, buf));
	}
	keyStat k;
	k.signWhatState = signWhatState;
	k.func = ks;
	keyStates.at(vKey).push_back(k);
}

bool keyboard::getKey(unsigned int vKey)
{
	return false;
	//return keyMap.at(vKey);
}

bool keyboard::getAlt()
{
	return false;
	//return keyMap.at(VK_MENU);
}

struct serverBind
{
	bool isUp;
	bool changed;
};
std::map<DWORD, serverBind> serverBinds;
void serverKeyBind(RakNet::BitStream& bsIn)
{
	DWORD key = 0;
	bsIn.Read(key);

	int status = 0;
	bsIn.Read(status);
	
	if(status < 1)
	{
		LINFO << "Erasing bing: " << key;
		if(serverBinds.find(key) != serverBinds.end())
		{
			serverBinds.erase(key);
			LINFO << "Erased";
		}
		return;
	}

	serverBind s;
	s.isUp = false;
	s.changed = false;
	serverBinds.insert(std::make_pair(key, s));
	LINFO << "Key " << key << " bind added";
}

void keyboard::pulseServerBinds()
{
	for(std::map<DWORD, serverBind>::iterator i = serverBinds.begin(); i != serverBinds.end(); ++i)
	{
		if(i->second.changed)
		{
			i->second.changed = false;
			RakNet::BitStream b;
			b.Write((MessageID)IVMP);
			b.Write(BIND_PLAYER_KEY);
			b.Write(i->first);
			b.Write((int)i->second.isUp);
			networkManager::sendBitStream(b);
		}
	}
}

bool k_controlDown = false;
void keyboard::receiveWindowMsg(UINT msg, WPARAM key)
{
	executingTask = 88;
	subTask = 0;
	bool isup = msg == WM_KEYUP;
	bool alt = false;
	//LINFO << "Keyboard: " << msg << ", " << key;
	
	if(key == VK_CONTROL) k_controlDown = !isup;
	if(key == VK_MENU && isup && k_controlDown)
	{
		k_controlDown = false;
		key = VK_CONTROL;
		//LINFO << "Control released";
	}

	if(chatWindow::isChatInputActive())
	{
		chatWindow::inputChat(key, !isup, alt);
	}

	if(keyStates.find(key) != keyStates.end())
	{
		size_t vsize = keyStates.at(key).size();
		for(size_t i = 0; i < vsize; i++)
		{
			if(keyStates.at(key).at(i).signWhatState == isup)
			{
				keyStates.at(key).at(i).func(alt);
			}
		}
	}

	if(!chatWindow::isChatInputActive() && serverBinds.find(key) != serverBinds.end() && serverBinds.at(key).isUp != isup)
	{
		serverBinds.at(key).isUp = isup;
		serverBinds.at(key).changed = true;
	}

	//LINFO << "Kb hook";

	executingTask = 0;
	subTask = 0;
}