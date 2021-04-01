#include "teamSpeak.h"
#include "easylogging++.h"
#include "NetworkManager.h"
#include "../SharedDefines/packetsIds.h"

HMODULE ts_Module = NULL;

bool checkFunctions()
{
	if(!ts_Module)
	{
		ts_Module = GetModuleHandleA("IVMP-TS.dll");
		if(!ts_Module) return false;
	}
	LINFO << "TS FOUND";
	return true;
}

void teamSpeak::disconnect()
{
	if(!checkFunctions()) return;

	typedef void(*ftyp)();
	ftyp fn = (ftyp)GetProcAddress(ts_Module, "ts_dll_close");
	fn();
}

void teamSpeak::connect(RakNet::BitStream& bsIn)
{
	if(!checkFunctions()) return;

	RakNet::RakString s;
	int playerid;
	bsIn.Read(s);
	bsIn.Read(playerid);

	typedef void(*ftyp)(const char* ip, int clientId, const char* DLL, int isExe);
	ftyp fn = (ftyp)GetProcAddress(ts_Module, "ts_dll_connect");
	if(fn) fn(s, playerid, "Client.dll", 0);
}

void teamSpeak::whisper(RakNet::BitStream& bsIn)
{
	if(!checkFunctions()) return;
	int playerid;
	bool toggle;
	bsIn.Read(playerid);
	bsIn.Read(toggle);

	typedef void(*ftyp)(int id, int bola);
	ftyp fn = (ftyp)GetProcAddress(ts_Module, "ts_dll_whispher");
	if(fn) {
		fn(playerid, toggle ? 1 : 0);
	}
}

void teamSpeak::toggle(RakNet::BitStream& bsIn)
{
	bool i;
	bsIn.Read(i);

	typedef void(*ftyp)(int bola);
	ftyp fn = (ftyp)GetProcAddress(ts_Module, "ts_dll_voice");
	if(fn) fn(i ? 1 : 0);
}

void teamSpeak::windowChanged(int active)
{
	typedef void(*ftyp)(int bola);
	ftyp fn = (ftyp)GetProcAddress(ts_Module, "ts_dll_active");
	if(fn) fn(active);
}

extern "C"
{
	__declspec(dllexport) void ts_client_status(int type, int value)
	{
		LINFO << "TS Status: " << type << " = " << value;
		if(!networkManager::isNetworkActive()) return;
		RakNet::BitStream b;
		b.Write((MessageID)IVMP);
		b.Write(TS_CONNECT);
		b.Write(type);
		b.Write(value);
		networkManager::sendBitStream(b);
	}
}