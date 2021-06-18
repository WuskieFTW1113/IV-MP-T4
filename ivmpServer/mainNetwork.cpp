#include "API/mainNetwork.h"
#include "networkManager.h"
#include "API/vehicleList.h"
#include "API/playerList.h"
#include "../SharedDefines/easylogging++.h"
#include "API/apiWorld.h"
#if linux
#include "../ivmpMasterCon/includes.h"
#else
#include "../ivmpServerMasterCon/includes.h"
#endif
#include "players.h"
#include <exporting.h>
#include <string>

#if linux
#include <unistd.h>
#define mySleep(x) (usleep(x * 1000))
#else
#include <Windows.h>
#define mySleep(x) (Sleep(x))
#endif

bool initCheck = false;
#include <time.h>
#include "../SharedDefines/packetsIds.h"
bool initRaknet(int port, const char* svrName, const char* svrLocation, const char* svrSite, bool masterList, bool GTAIV)
{
	networkManager::initNetwork(port);
	mySleep(200);

	if(!networkManager::isNetworkActive())
	{
		mlog("Network core is inactive");
		exit(1);
		return false;
	}
	mlog("Server thread is alive");
	vehicleList::loadList();
	playerList::loadList();
	apiWorld::createWorld(1, 1, 12, 0, 2000);

	if(masterList)
	{
		masterList_setUp();
		masterList_connect(std::to_string(port).c_str(), svrName, svrLocation, svrSite, 1.1f, GTAIV);
	}
	return true;
}

void pulseServer()
{
	networkManager::handlePackts();
	networkManager::streamData(networkManager::getConnection());

	masterList_updateStatus(players::playerCount());
	masterList_pulse();
}

#if linux
#else
int main(int argc, char *argv[])
{
	SetErrorMode(GetErrorMode () | SEM_NOGPFAULTERRORBOX);
	log_set_fp();
	if(initCheck)
	{
		initCheck = true;
	}
	return 1;
}
#endif
