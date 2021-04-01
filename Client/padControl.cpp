#include "padControl.h"
#include "players.h"
#include "gameMemory.h"
#include "cameraMemory.h"
#include "playerMemory.h"
#include "easylogging++.h"

std::map<int, PadState> playerPadStates;
std::map<int, gameVectors::Matrix34> playerCamMatrixs;

CPad* localPad;
IPad* localPadBackUp;

PadState* bufPad;

unsigned int localPlayerIndex = 0;
bool inLocalContext = true;

void initPlayerPadHook(int playerid)
{
	PadState pad;
	playerPadStates.insert(std::make_pair(playerid, pad));

	//gameVectors::Matrix34 g;
	//playerCamMatrixs.insert(std::make_pair(playerid, g));

	LINFO << "Player " << playerid << " pad hook stored";
}

void removePlayerPadHook(int playerid)
{
	playerPadStates.erase(playerid);
	//playerCamMatrixs.erase(playerid);
	LINFO << "Player " << playerid << " pad hook erased";
}

bool hasPlayerPadHook(int playerid)
{
	//return playerCamMatrixs.find(playerid) != playerCamMatrixs.end() && playerPadStates.find(playerid) != playerPadStates.end();
	return playerPadStates.find(playerid) != playerPadStates.end();
}

void * GetInternalPad()
{
	DWORD dwFunc = (gameMemory::getModuleHandle() + 0x7C3900);
	void * pPad = NULL;
	_asm
	{
		call dwFunc
		mov pPad, eax
	}
	return pPad;
}

void GetInternalPadState(PadState * padState)
{
	Pad * pPad = (Pad *)GetInternalPad();

	for(int i = 0; i < 191; i++)
	{
		padState->byteCurrentKeys[i] = pPad->keyBlocks[i].m_byteUnknown6;
		padState->bytePreviousKeys[i] = pPad->keyBlocks[i].m_byteUnknown7;
	}
}

void SetInternalPadState(PadState * padState)
{
	Pad * pPad = (Pad *)GetInternalPad();

	for(int i = 0; i < 191; i++)
	{
		pPad->keyBlocks[i].m_byteUnknown6 = padState->byteCurrentKeys[i];
		pPad->keyBlocks[i].m_byteUnknown7 = padState->bytePreviousKeys[i];
	}
}

void ResetInternalPadState()
{
	SetInternalPadState(&PadState());
}

void ContextSwitch(int playerid, bool bPost)
{
	try
	{
		if(!bPost)
		{
			if(inLocalContext)
			{
				LINFO << "Requestd context switch while using external context";
				return;
			}

			inLocalContext = true;

			*(unsigned int *)(gameMemory::getModuleHandle() + 0xF1CC68) = players::getPlayer(playerid).ped.Get();

			// Context switch the local players keys with the remote players
			GetInternalPadState(&playerPadStates.at(-2));
			SetInternalPadState(&playerPadStates.at(-2));
		}
		else
		{
			SetInternalPadState(&playerPadStates.at(-2));
			*(unsigned int *)(gameMemory::getModuleHandle() + 0xF1CC68) = 0;
			inLocalContext = false;
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error: contextSwitch: " << e.what();
	}
}

CPad* getLocalPad()
{
	return localPad;
}

void initLocalPad()
{
	//localPad = new CPad((IPad *)(gameMemory::getModuleHandle() + 0x1088F68));
	bufPad = new PadState();
}