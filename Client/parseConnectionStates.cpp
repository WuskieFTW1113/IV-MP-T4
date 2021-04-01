#include "parseConnectionStates.h"
#include "players.h"
#include "parseOnFootPlayerPacket.h"
#include "easylogging++.h"
#include "animList.h"
#include "localPlayerAnim.h"
#include "playerSkins.h"
#include "gameWorld.h"
#include "../clientLibs/execution.h"
#include "../SharedDefines/packetsIds.h"
#include "../SharedDefines/paramHelper.h"
#include "../clientLibs/EFLCScripts.h"

void connectionAccepted(RakNet::BitStream& bsIn)
{
	networkManager::connection* cCon = networkManager::getConnection();
	cCon->connectStat = 2;
	cCon->serverIp = cCon->packet->systemAddress;
	LINFO << "RakNet is now connected to server";

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SEND_NICK_TO_SERVER);

	RakNet::RakString name;
	name.Clear();
	name = players::getLocalPlayer().playerName;
	bsOut.Write(name);

	bsOut.Write(26); //version

	bsOut.Write(execution::getPatch() > 0x1080 ? 1 : 0);

	std::string s = execution::getVersion();
	//LINFO << s;
	std::vector<std::string> v;
	paramHelper::getParams(v, s, " ");

	size_t serialSize = v.size();
	bsOut.Write(serialSize);

	for(size_t i = 0; i < serialSize; i++)
	{
		name.Clear();
		name = v.at(i).c_str();
		bsOut.Write(name);		
	}

	cCon->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, cCon->serverIp, false);
}

void connectionLost(const char* msg)
{
	execution::setUnloadMsg(msg);
	networkManager::closeNetwork();
}

int localPlayerServerId = -1;
bool serverDefaultBlips;

void connectionMaySpawn(RakNet::BitStream& bsIn)
{
	LINFO << "Received spawn order";
	players::netPlayer& p = players::getLocalPlayer();
	
	bsIn.Read(p.skin);
	bsIn.Read(p.pos.x);
	bsIn.Read(p.pos.y);
	bsIn.Read(p.pos.z);

	bsIn.Read(localPlayerServerId);
	bsIn.Read(serverDefaultBlips);

	gameWorld::setLocalSpawnCoords(p.pos.x, p.pos.y, p.pos.z);
	EFLCScripts::setSpawnPos(p.pos.x, p.pos.y, p.pos.z, 0.0f);

	networkManager::getConnection()->connectStat = 3;
}

int getLocalPlayerServerId()
{
	return localPlayerServerId;
}

bool getServerBlips()
{
	return serverDefaultBlips;
}