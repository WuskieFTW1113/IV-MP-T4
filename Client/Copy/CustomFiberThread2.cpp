#include "chatWindow.h"
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
#include <regex>

#include "../ClientManager/ivScripting.h"

bool keyboardHook = false;

int executingTask = 0;
int subTask = 0;

clock_t exitSent = 0;
clock_t lastChatInputActive = 0;

Scripting::Ped bot;

int myThreadInit()
{
	chatWindow::loadConfig();
	chatWindow::initKeyHook();
	registerVehicleEntryKeys();
	playerList::registerKeys(_strdup(execution::getServerName()));
	chatWindow::showChatWindow(true);

	//guiManager::createGuiManager();
	//guiManager::getGuiManager()->InstallHook();
	return 1;
}

int myThreadPulse()
{
	return 30;
}

int myThreadStop()
{
	if(strcmp(execution::getUnloadMsg(), "~b~Exiting server") == 0)
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

	Scripting::DeleteChar(&bot);
}

CustomFiberThread::CustomFiberThread()
{
	/*SetName("CustomFiberThread");

	chatWindow::loadConfig();
	chatWindow::initKeyHook();
	registerVehicleEntryKeys();
	playerList::registerKeys(_strdup(execution::getServerName()));
	chatWindow::showChatWindow(true);

	guiManager::createGuiManager();
	guiManager::getGuiManager()->InstallHook();*/
}

bool CustomFiberThread::SpawnCar(vehicles::netVehicle &veh)
{
	/*if(!vehiclesList::isValidModel(veh.model))
	{
		LINFO << "!Error: tried to spawn invalid vehicle model: " << veh.model;
		return;
	}*/

	//int ivModel = vehiclesList::getModel(veh.model);
	//LINFO << "Requesting vehicle model: " << veh.model;
	
	//Scripting::RequestModel(veh.model);
	modelsHandler::request(veh.model, clock());
	if(!Scripting::HasModelLoaded(veh.model))
	{
		//LINFO << "!Error: vehicle model didnt load";
		return false;
	}

	/*for(int i = 0; i < 5; i++)
	{
		if(!Scripting::HasModelLoaded(veh.model) && i == 4)
		{
			LINFO << "!Error: vehicle model didnt load";
			return false;
		}
	}*/

	LINFO << "Car model " << veh.model << " available... spawning it!";

	veh.driverSeatOccupied = false;
	veh.spawned = false;

	Scripting::CreateCar(veh.model, veh.position.x, veh.position.y, veh.position.z, &veh.vehicle, true);
	//Scripting::MarkModelAsNoLongerNeeded(veh.model);
	modelsHandler::unload(veh.model);

	if(!Scripting::DoesVehicleExist(veh.vehicle))
	{
		LINFO << "!Error: vehicle instance wasnt created: count " << vehicles::streamedVehicles() << ", " << vehicles::spawnedVehicles();
		return false;
	}

	veh.memoryAddress = gameMemory::getAddressOfItemInPool( *(DWORD*)(gameMemory::getVehiclesPool()), veh.vehicle.Get());
	if(veh.memoryAddress == 0)
	{
		LINFO << "!Error: Vehicle memory wasnt found, deleting vehicle";
		Scripting::DeleteCar(&veh.vehicle);
		return false;
	}

	//Scripting::SetVehicleQuaternion(veh.vehicle, veh.rotation.X, veh.rotation.Y, veh.rotation.Z, veh.rotation.W);
	//NativeInvoke::Invoke<u32>("SET_CAR_ENGINE_ON", veh.vehicle, 1, 1);

	veh.vPointer = (vehicleMemory::CIVVehicle*)veh.memoryAddress;
	LINFO << "Testing pointer: " << veh.vPointer->GetEngineHealth();

	vehicleMemory::setVehicleLocalRotation(veh.vPointer, veh.rotation);
	//LINFO << "ROTATION"; //REMOVE
	Scripting::SetCarCoordinates(veh.vehicle, veh.position.x, veh.position.y, veh.position.z);
	//LINFO << "CORDS"; //Coords
	Scripting::ChangeCarColour(veh.vehicle, veh.color[0], veh.color[1]);
	//LINFO << "RCOLOR"; //Color
	Scripting::SetExtraCarColours(veh.vehicle, veh.color[2], veh.color[3]);
	//LINFO << "EXTRA"; //REMOVE

	/*if(Scripting::IsThisModelABike(veh.model) || Scripting::IsThisModelACar(veh.model))
	{
		Scripting::SetCarOnGroundProperly(veh.vehicle);
	}*/

	//Scripting::SetCarEngineOn(veh.vehicle, 0, 1);
	Scripting::SetCarProofs(veh.vehicle, true, true, true, true, true);

	//LINFO << "Car HP IS " << veh.engineHealth; 
	//NativeInvoke::Invoke<u32>("SET_VEHICLE_ALPHA", veh.vehicle, 200);

	LINFO << "Disable extra";
	for(unsigned int i = 0; i < 10; i++)
	{
		NativeInvoke::Invoke<u32>("TURN_OFF_VEHICLE_EXTRA", veh.vehicle, i, !veh.tunes[i]);
	}

	for(unsigned int i = 0; i < 6; i++)
	{
		if(veh.tyrePopped[i][0])
		{
			Scripting::BurstCarTyre(veh.vehicle, i);
		}
	}

	if(veh.siren)
	{
		NativeInvoke::Invoke<u32>("SWITCH_CAR_SIREN", veh.vehicle, true);
	}

	NativeInvoke::Invoke<u32>("SET_VEHICLE_DIRT_LEVEL", veh.vehicle, veh.dirt);
	NativeInvoke::Invoke<u32>("SET_CAR_LIVERY", veh.vehicle, veh.livery);
	NativeInvoke::Invoke<u32>("SET_CAR_CAN_BE_VISIBLY_DAMAGED", veh.vehicle, false);

	Scripting::SetEngineHealth(veh.vehicle, (float)veh.engineHealth);
	if(veh.engineHealth < -1200)
	{
		NativeInvoke::Invoke<u32>("EXPLODE_CAR", veh.vehicle, 0);
	}
	/*if(veh.horn)
	{
		NativeInvoke::Invoke<u32>("SOUND_CAR_HORN", veh.vehicle, 90000);
	}*/

	//*(unsigned char*)(veh.memoryAddress + 0x0F70) |= 8u;

	veh.breakOrGas = 0;
	veh.steer = 0;

	veh.spawned = true;

	updateEngineFlags(veh);

	//veh.lastRotationAlpha = 0.0;

	LINFO << "Vehicle spawned: " << veh.vehicle.Get() << ", " << veh.memoryAddress;

	//LINFO << "Test handle: " << vehicleMemory::getIndexFromHandle(veh.memoryAddress);
	return true;
}

void CustomFiberThread::ChangePlayerSkin(Scripting::eModel model)
{
	Scripting::RequestModel(model);

	while(!Scripting::HasModelLoaded(model))
	{
		Wait(0);
	}

	Scripting::eInteriorRoomKey roomKey;
	
	Scripting::GetKeyForCharInRoom(GetPlayerPed(), &roomKey);

	Scripting::ChangePlayerModel(GetPlayer(), model);

	Scripting::SetRoomForCharByKey(GetPlayerPed(), roomKey);

	Scripting::MarkModelAsNoLongerNeeded(model);

}

void CustomFiberThread::RunScript()
{	
	gameMemory::setModuleHandle();
	//guiManager::createGuiManager();

	int count = 0;
	//playerSkins::initAllSkins();
	animList::initAllAnims();
	initAnimLookUpSequences();
	//vehiclesList::loadList();
	vehicleTasks::initTasks();
	initLocalPad();
	//speedoTest::initStuff();
	weaponInfo::initGuns();
	EFLCScripts::turnOffGTAScripts();
	EFLCScripts::disableGTAStats();

    IKeyboardHookService *kbhService = ScriptHookManager::RequestService<IKeyboardHookService>( "KeyboardHook" );
    kbhService->AddHandler(this);
	keyboardHook = true;

	//std::ostringstream s;
	//s << "Vehicle pool: " << gameMemory::getVehiclesPool();
	LINFO << gameMemory::getVehiclesPool();
	Scripting::PrintStringWithLiteralStringNow("STRING", "Module loaded", 10000, true);

	installPadHooks();

	//execution::registerThread(true);

	if(networkManager::getConnection() == nullptr)
	{
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
		mp.playerName = playerName;
		mp.skin = 0x879495E2;

		players::insertPlayer(-2, mp);
		LINFO << "Player Init OK";
	}

	while(IsThreadAlive())
	{
		try
		{
			executingTask = 1;
			subTask = 1;
			EFLCScripts::pulseGTAScripts();
			subTask = 2;

			/*POINT pos;
			if(guiManager::isMouseActive() && GetCursorPos(&pos))
			{
				MSG m;
				m.hwnd = GetActiveWindow();

				if((GetKeyState(VK_LBUTTON) & 0x80) != 0)
				{
					m.message = WM_LBUTTONDOWN;
					wasMouseDown = true;
				}
				else if(wasMouseDown)
				{
					wasMouseDown = false;
					m.message = WM_LBUTTONUP;
				}
				else
				{
					m.message = WM_MOUSEMOVE;
					m.lParam = MAKELONG(pos.x, pos.y);
				}

				guiManager::setMousePos(pos.x, pos.y);
				guiManager::updateManagerMsg(m);
			}*/
			subTask = 3;
			
			if((GetAsyncKeyState(VK_F7) & 1) != 0)
			{
				players::netPlayer& p = players::getLocalPlayer();
				Scripting::CreateChar(1, 1878085135, p.pos.x, p.pos.y, p.pos.z, &bot, true);
				//Scripting::CreateRandomChar(p.pos.x, p.pos.y, p.pos.z, &bot);
				Scripting::SetCharDefaultComponentVariation(bot);
				//NativeInvoke::Invoke<u32>("TASK_WANDER_STANDARD", b);

				/*if(!NativeInvoke::Invoke<bool>("HAVE_ANIMS_LOADED", "PARACHUTE"))
				{
					NativeInvoke::Invoke<u32>("REQUEST_ANIMS", "PARACHUTE");
					Scripting::PrintStringWithLiteralStringNow("STRING", "Loading", 3000, true);
				}
				else
				{
					players::netPlayer& p = players::getPlayer(-2);
					NativeInvoke::Invoke<u32>("TASK_PLAY_ANIM_NON_INTERRUPTABLE", p.ped, "Hang_Idle", "PARACHUTE", 1000.0f, 1, 1, 1, 0, 0);
				}*/
				/*players::netPlayer& p = players::getPlayer(-2);
				if(NativeInvoke::Invoke<bool>("IS_SITTING_OBJECT_NEAR", p.pos.x, p.pos.y, p.pos.z, 0))
				{
					NativeInvoke::Invoke<u32>("TASK_SIT_DOWN_ON_NEAREST_OBJECT", p.ped, 2, 0, p.pos.x, p.pos.y, p.pos.z + 1.0f, 0, 0.0f, -2, 1);
				}*/
			}
			if((GetAsyncKeyState(VK_F9) & 1) != 0)
			{
				Scripting::WarpCharFromCarToCar(bot, vehicles::getVehicle(players::getLocalPlayer().passangerOf).vehicle, 0);
				NativeInvoke::Invoke<u32>("TASK_CAR_DRIVE_WANDER", bot, vehicles::getVehicle(players::getLocalPlayer().passangerOf).vehicle,
					12.0f, 0);
				/*for(int i = 0; i < 4; i++)
				{
					togglePlayerControls(i, true, false);
				}*/
				//NativeInvoke::Invoke<u32>("SET_PLAYER_CONTROL_FOR_TEXT_CHAT", players::getLocalPlayer().playerIndex, false);
				/*players::netPlayer& p = players::getPlayer(1000);
				NativeInvoke::Invoke<u32>("REQUEST_ANIMS", "veh@drivebystd");
				//NativeInvoke::Invoke<u32>("TASK_PLAY_ANIM_SECONDARY_IN_CAR", p.ped, "ds_aim_loop", "veh@drivebystd", 8.0f, 0, 0, 0, 1, 0);
				Scripting::TaskPlayAnimNonInterruptable(p.ped, "ds_aim_loop", "veh@drivebystd", 1000, false, false, false, true);
				float animTotal = 0;
				NativeInvoke::Invoke<u32>("GET_CHAR_ANIM_TOTAL_TIME", p.ped, "veh@drivebystd", "ds_aim_loop", &animTotal);
				LINFO << "ANIM TOTAL TIME: " << animTotal;*/
				/*std::ostringstream s;
				s << "Vehicle: " << *(DWORD*)(players::getLocalPlayer().memoryAddress + 0xB40);
				Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 3000, true);*/
			}
			else if((GetAsyncKeyState(VK_F5) & 1) != 0)
			{		
				//players::netPlayer& p = players::getPlayer(1000);
				//NativeInvoke::Invoke<u32>("SET_CHAR_ANIM_CURRENT_TIME", p.ped, "veh@drivebystd", "ds_aim_loop", 0.5f);
				/*float animTotal = 1;
				NativeInvoke::Invoke<u32>("GET_CHAR_ANIM_CURRENT_TIME", p.ped, "veh@drivebystd", "ds_aim_loop", &animTotal);
				std::ostringstream s;
				s << "Time: " << animTotal;
				Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 4000, true);*/
				/*players::netPlayer& p = players::getLocalPlayer();
				LINFO << "DETECTING OBJECTS";
				std::ostringstream s;
				for(DWORD i = 0; i < 300000; ++i)
				{
					if(NativeInvoke::Invoke<bool>("DOES_OBJECT_EXIST", i) && NativeInvoke::Invoke<bool>("HAS_OBJECT_BEEN_DAMAGED_BY_CHAR", i, p.ped))
					{
						//LINFO << "Damaged";
						float x, y, z;
						NativeInvoke::Invoke<u32>("GET_OBJECT_COORDINATES", i, &x, &y, &z);
						//if(simpleMath::getDistance(x, y, z, p.pos.x, p.pos.y, p.pos.z) < 20.0f)
						//{
							simpleMath::Quaternion q;
							NativeInvoke::Invoke<u32>("GET_OBJECT_QUATERNION", i, &q.X, &q.Y, &q.Z, &q.W);

							unsigned int model = 0;
							NativeInvoke::Invoke<u32>("GET_OBJECT_MODEL", i, &model);

							//LINFO << "Name";
							const ch* name = NativeInvoke::Invoke<const ch*>("GET_STRING_FROM_HASH_KEY", model);

							s << "apiObjects::addNew(apiMath::Vector3(" << x << "f, " << y << "f, " << z << 
								"f), apiMath::Quaternion(" << q.X << "f, " << q.Y << "f, " << q.Z << "f, " << q.W << "f), " <<
								model << ", 1, 0, 0, 255); // " << name << std::endl;
						//}
			
					}
						//LINFO << "Model " << model << ": " << x << "f, " << y << "f, " << z << "f";
				}
				LINFO << s.str();
				
				LINFO << "Finished";*/
				
			}
			else if((GetAsyncKeyState(VK_F8) & 1) != 0)
			{
				/*if(!npcRecorder::isRecording())
				{
					npcRecorder::startRecording();
				}
				else
				{
					npcRecorder::stopRecording();
				}*/
			}

			networkManager::connection* con = networkManager::getConnection();
			if(con != nullptr)
			{
				executingTask = 199;

				//LINFO << "Handling packets";
				subTask = 1;
				networkManager::handlePackts();

				bool Wactive = chatWindow::isChatInputActive();
				if(Wactive || con->cTime < lastChatInputActive + 1000)
				{
					NativeInvoke::Invoke<u32>("DISABLE_PAUSE_MENU", 1);
					if(Wactive)
					{
						lastChatInputActive = con->cTime;
					}
				}
				//LINFO << "Packet done";

				if(con->connectStat == -1)
				{
					//LINFO << "Unloading stuff";
					if(unloadStuff())
					{
						Scripting::PrintStringWithLiteralStringNow("STRING", "~r~Unloading game", 4000, true);
						return;
					}
					Wait(0);
					//LINFO << "Unloaded?";
					continue;
				}
				else if(con->connectStat == 3)
				{
					subTask = 2;
					LINFO << "Spawning local player";

					initPlayerStatus();
				}
				else if(con->connectStat == 4 && players::isPlayer(-2))
				{
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

					subTask = 9;
					npcRecorder::pulseRecord(con->cTime);

					subTask = 10;
					keyboard::pulseServerBinds();

					subTask = 11;
					chatWindow::handleKeyPress();
					
					subTask = 12;
					if(gameAudio::exist())
					{
						gameAudio::pulse(player.pos.x, player.pos.y, player.pos.z);
					}

					subTask = 13;
					//LuaVM::pulse(con->cTime);

					/*if(!chatWindow::isChatInputActive())
					{
						subTask = 11;
						playerList::handleKeyPress();
					}*/

					gameObjects::pulseObjects(con->cTime);
					gameCheckPoints::pulse(player.pos.x, player.pos.y, player.pos.z);
					gameWorld::pulseCurrentWorld(con->cTime);
					ivGUI::pulse(con->cTime);

					//LINFO << "Done";
				}
			}
			executingTask = 0;
			subTask = 0;
			Wait(0);
		}
		catch(std::exception& e)
		{
			LINFO << "Error: " << e.what() << ", (" << executingTask << ", " << subTask << ")";
		}
	}
}

CustomFiberThread::~CustomFiberThread()
{
	//execution::registerThread(false);
	LINFO << "Last task: " << executingTask << " (" << subTask << ")";
}

bool CustomFiberThread::unloadStuff()
{
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
	if(keyboardHook)
	{
		IKeyboardHookService *kbhService = ScriptHookManager::RequestService<IKeyboardHookService>( "KeyboardHook" );
		kbhService->RemoveHandler( this );
	}

	uninstallPadHooks();

	//if(IsThreadAlive())
	//{
		LINFO << "Deting game peds";
		for(std::map<int, players::netPlayer>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
		{
			//LINFO << "Parachute: " << i->second.parachute;
			if(i->second.parachute != 0 && NativeInvoke::Invoke<bool>("DOES_OBJECT_EXIST", i->second.parachute))
			{
				LINFO << "Object exists";
				NativeInvoke::Invoke<u32>("DELETE_OBJECT", &i->second.parachute);
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
	//}

	LINFO << "Exiting: 3";
	if(guiManager::getGuiManager() != nullptr)
	{
		//chatWindow::showChatWindow(false);
		guiManager::getGuiManager()->RemoveHook();
	}
	LINFO << "Done";

	LINFO << "Requesting module unload";
	//execution::unloadGameThread();
	LINFO << "Requested";

	Scripting::DeleteChar(&bot);*/

	return true;
}