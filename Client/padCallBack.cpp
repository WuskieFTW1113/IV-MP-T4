#include "padCallBack.h"
#include "CPatcher.h"
#include "gameMemory.h"
#include "players.h"
#include "easylogging++.h"
#include "vehicles.h"
#include "players.h"
#include "padControl.h"
#include "CustomFiberThread.h"
#include "vehicleSync.h"
#include "testVehicleInputs.h"
#include "vehiclesList.h"
#include "dummyVehicles.h"
#include "../clientLibs/execution.h"
#include "Offsets.h"
#include "../ClientManager/ivScripting.h"

DWORD* callBack_porra = 0;
DWORD callBack_m_pAddress;
//bool callBack_continue = false;
int callBack_lastPlayer = -2;
int callBack_lastVehicle = -1;

bool serverPlayerControls = false;

DWORD callBack_currentContext = 0;
DWORD callBack_currentModel = 0;
DWORD getLastContextType(){return callBack_currentContext;}
DWORD getLastContextModel(){return callBack_currentModel;}

//index 0 is the hook address and index 1 is the original function
//GTA IV
/*DWORD inputHookList[4][2] = {
	{VAR_CAutomobile__VFTable_7 + 0x74, 0},
	{VAR_CBike__VFTable_7 + 0x74, 0},
	{VAR_CBoat__VFTable_7 + 0x74, 0},
	{VAR_CHeli__VFTable_7 + 0x74, 0}
};*/

//EFLC
DWORD inputHookList[4][2] = {
	{0, 0}, //VAR_CAutomobile__VFTable
	{0, 0}, //VAR_CBike__VFTable
	{0, 0}, //VAR_CBoat__VFTable
	{0, 0} //VAR_CHeli__VFTable
};

void callbackParams(unsigned int vModel)
{
/*	subTask = 13;

	if(Scripting::IsThisModelABike(vModel))
	{
		subTask = 15;
		callBack_m_pAddress = gameMemory::getModuleHandle() + FUNC_CBike__ProcessInput_7;
		//LINFO << "Bike";
	}
	else if(vehiclesList::isModelAHeli(vModel))
	{
		subTask = 16;
		callBack_m_pAddress = gameMemory::getModuleHandle() + FUNC_CHeli__ProcessInput_7;
		//LINFO << "Heli";
	}
	else if(Scripting::IsThisModelABoat(vModel))
	{
		subTask = 17;
		callBack_m_pAddress = gameMemory::getModuleHandle() + FUNC_CBoat__ProcessInput_7;
		//LINFO << "Boat";
	}
	else if(Scripting::IsThisModelACar(vModel))
	{
		subTask = 18;
		callBack_m_pAddress = gameMemory::getModuleHandle() + FUNC_CAutomobile__ProcessInput_7;
		//LINFO << "Any car";
	}
	else
	{
		callBack_continue = false;
	}
	subTask = 19;*/
}

void processInput()
{
	try
	{
		//callBack_continue = true;
		callBack_lastPlayer = -1;
		callBack_lastVehicle = -1;

		/*if(players::isPlayer(-2))
		{
			players::netPlayer& p = players::getLocalPlayer();

			if(p.inVehicle != 0 && vehicles::isVehicle(p.inVehicle) && 
				*(DWORD*)(vehicles::getVehicle(p.inVehicle).memoryAddress + 0x0FA0) == p.memoryAddress) {

					callBack_lastPlayer = -2;
					callbackParams(vehicles::getVehicle(p.inVehicle).model);
			}
		}*/

		DWORD vehAddr = (DWORD)callBack_porra;
		unsigned int vId = vehicleMemory::getIndexFromHandle(vehAddr);
		if(vId == 0)
		{
			LINFO << "!Error: process vehicle input didnt find vehicle in pool";
			return;
		}

		unsigned int vModel = 0;
		Scripting::ScriptHandle veh(vId);
		//NativeInvoke::Invoke<u32>("GET_CAR_MODEL", veh, &vModel);
		Scripting::GetCarModelByCarId(vId, &vModel);

		callBack_currentModel = vModel; //debugging

		//callbackParams(vModel); NO LONGER NEEDED

		DWORD carDriver = *(DWORD*)(vehAddr + 0x0FA0);
		if(carDriver == players::getLocalPlayer().memoryAddress)
		{
			callBack_lastPlayer = -2;
			//Scripting::PrintStringWithLiteralStringNow("STRING", "DRIVER", 200, true);
		}
		/*else if(carDriver == 0)
		{
			vehicleMemory::setVehicleHandBrake(vehAddr, 76);
		}*/
		
		bool createdByMod = false;
		for(std::map<size_t, vehicles::netVehicle>::iterator i = vehicles::getVehiclesBegin(); i!= vehicles::getVehiclesEnd(); ++i)
		{
			if(i->second.memoryAddress == vehAddr)
			{
				callBack_lastVehicle = i->first;
				createdByMod = true;
				break;
			}
		}

		if(!createdByMod)
		{
			callBack_lastPlayer = -1;
			callBack_lastVehicle = -1;
			
			//NativeInvoke::Invoke<u32>("DELETE_CAR", &veh);
			dummyVehicles::add(vId);
			//LINFO << "Deleted";
		}
	}
	catch(std::exception& e)
	{
		//callBack_continue = false;
		LINFO << "!Error: processInput: " <<  e.what() << ", " << subTask;
	}
}

void Vehicle_ProcessInput_Hook()
{
	subTask = 3;
	processInput();
	subTask = 4;
}

void Vehicle_Sync()
{
	if(callBack_lastVehicle != -1 && callBack_lastPlayer != -2)
	{
		subTask = 5;
		syncVehicle(callBack_lastVehicle);
	}
	subTask = 6;
}

_declspec(naked) void CVehicle_ProcessInput_Hook() //0xC44780
{
	executingTask = 2;
	subTask = 1;
	callBack_currentContext = 1;

	_asm
	{
		mov callBack_porra, ecx
		pushad
	}

	subTask = 2;
	Vehicle_ProcessInput_Hook();
	subTask = 3;

	callBack_m_pAddress = gameMemory::getModuleHandle() + offsets::CARSPROCESS;

	_asm
	{
		popad
		call callBack_m_pAddress
		pushad
	}

	subTask = 4;
	Vehicle_Sync();

	subTask = 7;
	_asm
	{
		popad
		ret
	}
	executingTask = 0;
	subTask = 0;
}

_declspec(naked) void CBike_ProcessInput_Hook() //0xB45490
{
	executingTask = 2;
	subTask = 1;
	callBack_currentContext = 2;

	_asm
	{
		mov callBack_porra, ecx
		pushad
	}

	subTask = 2;
	Vehicle_ProcessInput_Hook();
	subTask = 3;

	callBack_m_pAddress = gameMemory::getModuleHandle() + offsets::BIKEPROCESS;

	_asm
	{
		popad
		call callBack_m_pAddress
		pushad
	}

	subTask = 4;
	Vehicle_Sync();

	subTask = 7;
	_asm
	{
		popad
		ret
	}
	executingTask = 0;
	subTask = 0;
}

_declspec(naked) void CHeli_ProcessInput_Hook() //0x904920
{
	executingTask = 2;
	subTask = 1;
	callBack_currentContext = 3;

	_asm
	{
		mov callBack_porra, ecx
		pushad
	}

	subTask = 2;
	callBack_m_pAddress = gameMemory::getModuleHandle() + offsets::HELIPROCESS;

	_asm
	{
		popad
		call callBack_m_pAddress
		pushad
	}

	Vehicle_ProcessInput_Hook();

	Vehicle_Sync();

	subTask = 7;
	_asm
	{
		popad
		ret
	}
	executingTask = 0;
	subTask = 0;
}

_declspec(naked) void CBoat_ProcessInput_Hook() //0xB41900
{
	executingTask = 2;
	subTask = 1;
	callBack_currentContext = 4;

	_asm
	{
		mov callBack_porra, ecx
		pushad
	}

	subTask = 2;
	Vehicle_ProcessInput_Hook();
	subTask = 3;

	callBack_m_pAddress = gameMemory::getModuleHandle() + offsets::BOATPROCESS;

	_asm
	{
		popad
		call callBack_m_pAddress
		pushad
	}

	subTask = 4;
	Vehicle_Sync();

	subTask = 7;
	_asm
	{
		popad
		ret
	}
	executingTask = 0;
	subTask = 0;
}

void installPadHooks()
{
	LINFO << "Installing pad hooks";
	inputHookList[0][0] = offsets::CARSVTABLE + 0x74;
	inputHookList[1][0] = offsets::BIKEVTABLE + 0x74;
	inputHookList[2][0] = offsets::BOATSVTABLE + 0x74;
	inputHookList[3][0] = offsets::HELIVTABLE + 0x74;

	for(int i = 0; i < 4; i++)
	{
		inputHookList[i][1] = *(DWORD*)(inputHookList[i][0] + gameMemory::getModuleHandle());
		LINFO << "Replacing address " << inputHookList[i][0] << " (" << inputHookList[i][1] << ")";

		if(i == 0)
		{
			CPatcher::InstallMethodPatch(inputHookList[i][0], (DWORD)CVehicle_ProcessInput_Hook);
		}
		else if(i == 1)
		{
			CPatcher::InstallMethodPatch(inputHookList[i][0], (DWORD)CBike_ProcessInput_Hook);
		}
		else if(i == 2)
		{
			CPatcher::InstallMethodPatch(inputHookList[i][0], (DWORD)CBoat_ProcessInput_Hook);
		}
		else if(i == 3)
		{
			CPatcher::InstallMethodPatch(inputHookList[i][0], (DWORD)CHeli_ProcessInput_Hook);
		}
	}

	LINFO << "Done";
	// CPlane::ProcessInput
	//CPatcher::InstallMethodPatch((VAR_CPlane__VFTable_7 + 0x74), (DWORD)Vehicle_ProcessInput_Hook);
}

void uninstallPadHooks()
{
	LINFO << "Removing pad hooks";

	for(int i = 0; i < 4; i++)
	{
		CPatcher::InstallMethodPatch(inputHookList[i][0], (DWORD)inputHookList[i][1]);
	}
	LINFO << "Done";
}

bool controlType[4] = {true, true, true, true};
bool isFrozen = false;

void togglePlayerControls(unsigned int typ, bool b, bool serverRequest)
{
	if(b)
	{
		controlType[typ] = true;
		bool gameUnfreeze = true;
		for(unsigned int i = 0; i < 4; i++)
		{
			if(!controlType[i])
			{
				gameUnfreeze = false;
				break;
			}
		}

		if(gameUnfreeze)
		{
			//NativeInvoke::Invoke<u32>("SET_PLAYER_CONTROL_FOR_TEXT_CHAT", players::getLocalPlayer().playerIndex, false);
			Scripting::SetPlayerControlForTextChat(players::getLocalPlayer().playerIndex, false);
			isFrozen = false;
		}
	}
	else
	{
		controlType[typ] = false;
		isFrozen = true;
		//NativeInvoke::Invoke<u32>("SET_PLAYER_CONTROL_FOR_TEXT_CHAT", players::getLocalPlayer().playerIndex, true);
		Scripting::SetPlayerControlForTextChat(players::getLocalPlayer().playerIndex, true);
	}
	serverPlayerControls = serverRequest;
	//NativeInvoke::Invoke<u32>("SET_GAME_CAMERA_CONTROLS_ACTIVE", b);
	Scripting::SetGameCameraControlsActive(b);
	//Scripting::SetCameraControlsDisabledWithPlayerControls(false);
}

bool getPlayerControls()
{
	return !isFrozen;
}

bool shouldPlayerBeFrozen()
{
	return serverPlayerControls;
}