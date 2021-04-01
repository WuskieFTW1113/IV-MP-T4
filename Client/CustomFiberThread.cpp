/*#include "chatWindow.h"
#include "NetworkManager.h"
#include "chatWindow.h"
#include "playerList.h"
#include "gameWorld.h"
#include "gameBlips.h"
#include "localPlayerCamera.h"
#include "gameObjects.h"
#include "gameCheckPoints.h"

#include "MainHook.h"

#include "easylogging++.h"
#include <fstream>
#include "players.h"
#include "playerSkins.h"
#include "animList.h"
#include "prepareClientToServerPacket.h"
#include "localPlayerAnim.h"
#include "../SharedDefines/paramHelper.h"
#include "gameMemory.h"
#include "vehicleMemory.h"
#include "vehiclesList.h"
#include "vehicleTasks.h"
#include "padCallBack.h"
#include "padControl.h"
#include "npcRecorder.h"
#include "weaponInfo.h"
#include "guiManager.h"
#include "openMpWindow.h"
#include "modelsHandler.h"
#include "../clientLibs/execution.h"
#include "../clientLibs/EFLCScripts.h"
#include "ivGUI.h"
#include "keyboard.h"
#include "gameAudio.h"
#include "LuaVM.h"
#include "vehicleSync.h"
#include <regex>*/

#include "NetworkManager.h"
#include "MainHook.h"
#include "CustomFiberThread.h"
#include "easylogging++.h"
#include <fstream>
#include "../ClientManager/ivScripting.h"
#include "animList.h"
#include "localPlayerAnim.h"
#include "vehicleTasks.h"
#include "weaponInfo.h"
#include <regex>
#include "modelsHandler.h"
#include "prepareClientToServerPacket.h"
#include "Offsets.h"
#include "gameMemory.h"
#include "guiManager.h"
#include "chatWindow.h"
#include "playerList.h"
#include "keyboard.h"
#include "padCallBack.h"
#include "gameAudio.h"
#include "gameObjects.h"
#include "gameCheckPoints.h"
#include "gameWorld.h"
#include "ivGUI.h"
#include "gameBlips.h"
#include "localPlayerCamera.h"
#include "../clientLibs/EFLCScripts.h"
//#include "npcRecorder.h"
#include "teamSpeak.h"
#include <string>

bool keyboardHook = false;

int executingTask = 0;
int subTask = 0;

clock_t exitSent = 0;
clock_t lastChatInputActive = 0;

u32 partCount = 1;

int myThreadInit()
{
	offsets::define();
	gameMemory::setModuleHandle();
	installPadHooks();
	animList::initAllAnims();
	initAnimLookUpSequences();
	vehicleTasks::initTasks();
	weaponInfo::initGuns();
	registerVehicleEntryKeys();
	setSittingAnim("missfrancis5", "idle_bench_a", 0);

	chatWindow::loadConfig();
	chatWindow::initKeyHook();
	chatWindow::showChatWindow(true);
	gameAudio::registerCb();

	playerList::registerKeys(_strdup(execution::getServerName()));

	guiManager::InstallHook();

	Scripting::Player playerIndex = Scripting::ConvertIntToPlayerIndex(Scripting::GetPlayerId());
	Scripting::Ped ped;
	Scripting::GetPlayerChar(GetPlayer(), &ped);

	std::string playerName = std::string("");

	std::ifstream infile("comMP//config.txt");
	if(infile.is_open())
	{
		std::string line;
		while(std::getline(infile, line))
		{
			if(line.compare(0, 7, std::string("myname=")) == 0)
			{
				line.erase(0, 7);
				playerName = line;
				if(playerName.size() > 22) playerName = playerName.substr(0, 22);
				if(!std::regex_match(playerName, std::regex("^[\x20-\x7F]+$"))) playerName = "";
			}
		}
	}

	LINFO << "Initing RakNet";
	//networkManager::initNetwork(serverIp.c_str(), 8888);
	LINFO << "Server addr: " << execution::getServerIp() << ", " << execution::getServerPort();
	networkManager::initNetwork(execution::getServerIp(), execution::getServerPort());
	LINFO << "RakNet OK";

	players::netPlayer mp;
	mp.spawned = false;
	//mp.playerName = playerName;
	sprintf_s(mp.playerName, 50, "%s", playerName.c_str());
	mp.skin = -1729980128;
	mp.ped = ped;
	mp.playerIndex = playerIndex;

	players::insertPlayer(-2, mp);

	togglePlayerControls(CONTROL_CHAT, true, false);
	Scripting::SetPlayerControl(playerIndex, true);
	Scripting::SetCameraControlsDisabledWithPlayerControls(false);

	LINFO << "Player Init OK";
	return 1;
}

int myThreadPulse()
{
	networkManager::connection* con = networkManager::getConnection();
	if(con == nullptr)
	{
		myThreadInit();
		return 1;
	}
	if(con != nullptr)
	{
		executingTask = 199;

		//LINFO << "Handling packets";
		subTask = 1;
		networkManager::handlePackts();

		if(con->connectStat == 3)
		{
			subTask = 2;
			LINFO << "Spawning local player";

			initPlayerStatus();
		}
		else if(con->connectStat == 4 && players::isPlayer(-2))
		{
			bool Wactive = chatWindow::isChatInputActive();
			if(Wactive || con->cTime < lastChatInputActive + 1000)
			{
				Scripting::DisablePauseMenu(true);
				if(Wactive)
				{
					lastChatInputActive = con->cTime;
				}
			}

			subTask = 3;
			players::netPlayer& player = players::getPlayer(-2);

			//LINFO << "Refresh";
			refreshMyPlayerStatus(player, con->cTime);

			//LINFO << "Packet";
			if(player.inVehicle == 0)
			{
				subTask = 4;
				parseOnFootPlayerPacket(con);
			}
			else
			{
				subTask = 5;
				sendInVehiclePacket(con);
				/*gameVectors::Vector3 v1, v2;
				int someInt = -1;
				bool b = NativeInvoke::Invoke<0x3B5D0F27, bool>(vehicles::getVehicle(player.inVehicle).vehicle, partCount,
					&v1, &v2, &someInt, 1);
			
				
				simpleMath::Vector3 pos = vehicles::getVehicle(player.inVehicle).position;
				//Scripting::DrawSphere(pos.x + v1.x, pos.y + v1.y, pos.z + v1.z, 0.2f);

				char str[50];
				sprintf_s(str, 50, "(%i) Part %u. %.2f, %.2f, %.2f", (int)b, partCount, v1.x, v1.y, v1.z);
				Scripting::PrintStringWithLiteralStringNow("STRING", str, 100, 1);*/
			}

			if(localCamera::isActive())
			{
				localCamera::sendCamSync(con);
			}

			//LINFO << "Manage";
			subTask = 6;
			managePlayers();

			subTask = 7;
			pulseVehicles(player.pos.x, player.pos.y, player.pos.z);

			subTask = 8;
			modelsHandler::clearUnsued(con->cTime);

			subTask = 10;
			keyboard::pulseServerBinds();

			subTask = 11;
			chatWindow::handleKeyPress();

			subTask = 12;
			if(gameAudio::exist())
			{
				gameAudio::pulse(player.pos);
			}

			subTask = 13;

			gameObjects::pulseObjects(con->serverUpTime);
			gameCheckPoints::pulse(con->cTime, player.pos);
			gameWorld::pulseCurrentWorld(con->cTime);
			ivGUI::pulse(con->cTime);

			subTask++;
			EFLCScripts::pulseGTAScripts();
		}
	}
	executingTask = 0;
	subTask = 0;
	return 0;
}

#include "../SharedDefines/packetsIds.h"

int myThreadDisconnect()
{
	teamSpeak::disconnect();
	networkManager::connection* con = networkManager::getConnection();
	if(con != nullptr)
	{
		RakNet::BitStream b;
		b.Write((MessageID)ID_REMOTE_DISCONNECTION_NOTIFICATION);
		b.Write(0);
		networkManager::sendBitStream(b);
	}
	return 1;
}

int myThreadStop()
{
	LINFO << "Client stopping";
	guiManager::RemoveHook();
	uninstallPadHooks();
	networkManager::connection* con = networkManager::getConnection();
	if(con != nullptr)
	{
		delete con->peer;
		con->peer = nullptr;
	}

	for(std::map<int, players::netPlayer>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		//LINFO << "Parachute: " << i->second.parachute;
		if(i->first == -2) continue;
		if(i->second.parachute != 0 && Scripting::DoesObjectIdExist(i->second.parachute))
		{
			LINFO << "Object exists";
			Scripting::DeleteObjectById(&i->second.parachute);
		}

		if(i->second.spawned)
		{
			Scripting::DeleteChar(&i->second.ped);
		}
	}

	LINFO << "Deting game vehicles";
	for(std::map<size_t, vehicles::netVehicle>::iterator i = vehicles::getVehiclesBegin(); i != vehicles::getVehiclesEnd(); ++i)
	{
		if(i->second.spawned)
		{
			Scripting::DeleteCar(&i->second.vehicle);
		}
	}

	gameObjects::wipeObjects();
	gameCheckPoints::wipeCps();
	gameBlips::wipeBlips();
	gameAudio::reallyStop();

	/*if(strcmp(execution::getUnloadMsg(), "~b~Exiting server") == 0)
	{
		if(exitSent == 0)
		{
			exitSent = clock() + 1000;
			sendDisconnectMsg();
			LINFO << "Disconnect msg sent";
			return false;
		}
		else if(clock() < exitSent)
		{
			//LINFO << "Waiting";
			return false;
		}
	}

	LINFO << "Exiting: 1";
	networkManager::connection* con = networkManager::getConnection();
	if(con != nullptr)
	{
		delete con->peer;
		con->peer = nullptr;
	}

	LINFO << "Exiting: Bass";
	gameAudio::reallyStop();

	if(localCamera::isActive())
	{
		localCamera::destroy();
	}

	LINFO << "Exiting: 2";
	uninstallPadHooks();

	LINFO << "Deting game peds";
	for(std::map<int, players::netPlayer>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		//LINFO << "Parachute: " << i->second.parachute;
		if(i->second.parachute != 0 && Scripting::DoesObjectExist(i->second.parachute))
		{
			LINFO << "Object exists";
			Scripting::DeleteObject(i->second.parachute);
		}

		if(i->second.spawned && i->first != -2)
		{
			Scripting::DeleteChar(&i->second.ped);
		}
	}

	LINFO << "Deting game vehicles";
	for(std::map<size_t, vehicles::netVehicle>::iterator i = vehicles::getVehiclesBegin(); i != vehicles::getVehiclesEnd(); ++i)
	{
		if(i->second.spawned)
		{
			Scripting::DeleteCar(&i->second.vehicle);
		}
	}

	gameObjects::wipeObjects();
	gameCheckPoints::wipeCps();
	gameBlips::wipeBlips();

	LINFO << "Exiting: 3";
	if(guiManager::getGuiManager() != nullptr)
	{
		//guiManager::getGuiManager()->RemoveHook();
	}
	LINFO << "Done";

	LINFO << "Requesting module unload";
	//execution::unloadGameThread();
	LINFO << "Requested";

	Scripting::DeleteChar(&bot);*/
	return 1;
}
