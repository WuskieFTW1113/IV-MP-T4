#include "chatWindow.h"
#include "../SharedDefines/packetsIds.h"
#include "prepareClientToServerPacket.h"

#include "CustomFiberThread.h"
#include <Windows.h>
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

void CustomFiberThread::OnKeyboardHookEvent(const IKeyboardHookHandler::KeyEventArgs &args)
{
	executingTask = 88;
	subTask = 0;
	if(chatWindow::isChatInputActive())
	{
		chatWindow::inputChat(args.VirtualKey, !args.IsKeyUpNow, args.IsAltPressed);
	}
	/*if(PeekMessage(&tmsg, NULL, 0, 0, PM_REMOVE))
	{
		//LINFO << "Key";
		subTask = 1;

		//guiManager::updateManagerMsg(tmsg);
		subTask = 2;
		TranslateMessage(&tmsg);
		subTask = 3;
		DispatchMessage(&tmsg);
		subTask = 4;
	}*/

	if(keyStates.find(args.VirtualKey) != keyStates.end())
	{
		size_t vsize = keyStates.at(args.VirtualKey).size();
		for(size_t i = 0; i < vsize; i++)
		{
			if(keyStates.at(args.VirtualKey).at(i).signWhatState == args.IsKeyUpNow)
			{
				keyStates.at(args.VirtualKey).at(i).func(args.IsAltPressed);
			}
		}
	}

	if(!chatWindow::isChatInputActive() && serverBinds.find(args.VirtualKey) != serverBinds.end())
	{
		serverBinds.at(args.VirtualKey).isUp = args.IsKeyUpNow;
		serverBinds.at(args.VirtualKey).changed = true;
	}

	//LINFO << "Kb hook";

	executingTask = 0;
	subTask = 0;

	/*msg->message = 0;

	if(!args.IsKeyUpNow && !args.WasKeyDownBefore)
	{
		return;
	}

	if(args.VirtualKey == VK_F8)
	{
		LINFO << "Speedo: strt";
		speedoTest::initStuff();
		LINFO << "Speedo: strted";
		return;
	}

	msg->wParam = args.VirtualKey;

	if(msg->wParam == VK_BACK)
	{		
		msg->message = WM_KEYDOWN;
	}
	else
	{
		msg->message = WM_CHAR;
	}*/
	//speedoTest::updateMsg(msg);

	//LINFO << "Kb end( " << msg->message << ": " << msg->wParam << ")";
}