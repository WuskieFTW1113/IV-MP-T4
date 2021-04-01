#include <Windows.h>
#include "easylogging++.h"
_INITIALIZE_EASYLOGGINGPP;
#include "CXLiveHook.h"
#include "CustomFiberThread.h"
#include "cPatcher.h"
//#include "rageThread.h"
#include "memoryPipe.h"

#include "../clientLibs/execution.h"
#include "../clientLibs/EFLCScripts.h"
#include "../clientLibs/dumpManager.h"
#include "../clientLibs/gameWindow.h"

#include <time.h>
#include "memory.h"
#include "d3dHook.h"

#include "ivScripting.h"

unsigned long base = NULL;
unsigned int mouseHookId = NULL;
clock_t shouldShowMenu = 0;
bool menuShowing = false;

bool moduleOn = false;
HINSTANCE hinstDLL, teamSpeakDLL = NULL;

Scripting::Camera gCam;
Scripting::Camera oriCam;
bool camCreated = false;

bool currentSpawnStatus = false;
bool firstLaunch = true;
bool firstMissionDetected = false;

int currentTask = 0;

#include <psapi.h>

/*
CustomFiberThread::CustomFiberThread()
{
	menuShowing = false;
	currentSpawnStatus = false;
	firstLaunch = true;
	firstMissionDetected = false;
	
	//memoryPipe::init();

	SetName("Manolo");

	//mainMenu = new gameMenu();
	//mainMenu->InstallHook();

	//mouseHookId = mouseHook::registerMouseHook(gameMenuMouseHook);

	LINFO << "!Manolo: class init";
}

CustomFiberThread::~CustomFiberThread()
{
	LINFO << "!Manolo: class deleting";

	memoryPipe::end();
	
	LINFO << "!Manolo: class deleted";
}
*/

bool mustLaunch = false;
void memoryPipe::launchGame()
{
	if(!moduleOn)
	{
		mustLaunch = false;
		std::ofstream ofs;
		ofs.open("myeasylog.log", std::ofstream::out | std::ofstream::trunc);
		ofs.close();

		LINFO << "!Manolo: launching";
		hinstDLL = LoadLibrary("comMP//Client.dll");
		teamSpeakDLL = LoadLibrary("comMP//TeamSpeak//tsmain.dll");
		if(hinstDLL == NULL)
		{
			LINFO << "!Manolo: did not launch";
		}
		else
		{
			LINFO << "!Manolo: launched";
			moduleOn = true;
		}
		if(teamSpeakDLL == NULL) {
			LINFO << "!Manolo: TS didnt load";
		}
		else LINFO << "!Manolo: TS did load";
		shouldShowMenu = 0;

		LINFO << "Closing Menu";
	}
}

clock_t waitForDisconnect = 0;
void pulseHasThread()
{
	//EFLCScripts::pulseGTAScripts();
	if(execution::shouldUnloadThread())
	{
		if(waitForDisconnect == 0)
		{
			LINFO << "!Manolo: disconnecting";
			waitForDisconnect = clock() + 1000;
			return execution::callMustDisconnect();
		}
		else if(clock() < waitForDisconnect) return;
		waitForDisconnect = 0;
		LINFO << "!Manolo: Removing the client";
		execution::callUnloadFunction();
		gameWindow::registerWindowCb(NULL);
		LINFO << "!Manolo: Lib";
		FreeLibrary(hinstDLL);
		FreeLibrary(teamSpeakDLL);
		LINFO << "!Manolo: Unloaded";
		hinstDLL = NULL;
		moduleOn = false;
		Scripting::PrintStringWithLiteralStringNow("STRING", execution::getUnloadMsg(), 10000, true);
		//execution::registerThread(false);
		execution::setSpawned(false);
		currentSpawnStatus = false;
		shouldShowMenu = clock() + 5000;
	}

	if(currentSpawnStatus != execution::hasSpawned())
	{
		LINFO << "Setting cam behind player";
		if(camCreated && oriCam.Get() != 0)
		{
			Scripting::ActivateScriptedCams(0, 0);
			Scripting::SetCamActive(oriCam, true);

			LINFO << "Deleting custom cam";
			if(Scripting::DoesCamExist(gCam)) Scripting::DestroyCam(gCam);
			camCreated = false;
		}

		Scripting::DisplayHUD(true);
		Scripting::DisplayRadar(true);

		currentSpawnStatus = true;
	}
	//LINFO << "Done";
}

void pulseNoThread()
{
	currentTask = 6;
	//LINFO << "No thread";
	if(Scripting::GET_CUTSCENE_AUDIO_TIME_MS() > 0)
	{
		LINFO << "First mission detected";
		firstMissionDetected = true;
		return;
	}
	else if(firstMissionDetected || !Scripting::IsScreenFadedIn())
	{
		return;
	}

	currentTask = 7;
	//LINFO << "Didnt crash";
	if(firstLaunch)
	{
		currentTask = 8;
		EFLCScripts::turnOffGTAScripts();
		firstLaunch = false;
	}
	currentTask = 9;
	EFLCScripts::pulseGTAScripts();

	if(!camCreated)
	{
		//NativeInvoke::Invoke<u32>("GET_GAME_CAM", &oriCam);
		LINFO << "Get cam";
		Scripting::GetGameCam(&oriCam);

		LINFO << "Create Cam";
		//NativeInvoke::Invoke<u32>("CREATE_CAM", 14, &gCam);
		Scripting::CreateCam(14, &gCam);
		LINFO << "ACTV";
		Scripting::SetCamPos(gCam, -203.221664f, 265.353424f, 86.0f);
		Scripting::PointCamAtCoord(gCam, -228.926285f, 458.884644f, 58.0f);
		//NativeInvoke::Invoke<u32>("SET_CAM_ACTIVE", gCam, true); DO NOT TOUCH

		//NativeInvoke::Invoke<u32>("ACTIVATE_SCRIPTED_CAMS", 1, 1);
		Scripting::ActivateScriptedCams(1, 1);
		//NativeInvoke::Invoke<u32>("DISPLAY_HUD", false);
		Scripting::DisplayHUD(false);
		//NativeInvoke::Invoke<u32>("DISPLAY_RADAR", false);
		Scripting::DisplayRadar(false);

		Scripting::SetCamActive(gCam, true);
		Scripting::SetCamPropagate(gCam, true);

		camCreated = true;
		LINFO << "Done";
	}
	currentTask = 10;

	if(mustLaunch)
	{
		memoryPipe::launchGame();
	}
}

void StartScript()
{
	shouldShowMenu = clock() + 5000;
	LINFO << "!Manolo: manager is live";
	gameWindow::defineWindow();
}

void PulseScript()
{
	try
	{
		//AddScriptToThreadCollection();

		currentTask = 1111;
		memoryPipe::pulse();
		currentTask = 1;
		if(execution::hasGameThread())
		{
			currentTask = 2;
			pulseHasThread();
			currentTask = 3;
		}
		else
		{
			currentTask = 4;
			pulseNoThread();
			currentTask = 5;
		}

		currentTask = 0;
	}
	catch(std::exception& e)
	{
		LINFO << "!Error manolo: " << e.what();
	}
}

bool IS_WORLD_POINT_WITHIN_BRAIN_ACTIVATION_RANGE()
{
	//LINFO << "> IS_WORLD_POINT_WITHIN_BRAIN_ACTIVATION_RANGE";
	return 0;
}

bool IS_OBJECT_WITHIN_BRAIN_ACTIVATION_RANGE(int a1)
{
	//LINFO << "> IS_OBJECT_WITHIN_BRAIN_ACTIVATION_RANGE: " << a1;
	return 0;
}

struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};

Vector4 GET_SPAWN_LOCATION(Vector4 *pos, float angle, Vector4 *result)
{
	//LINFO << "SPAWN LOCATION";
	EFLCScripts::getSpawnPos(pos->x, pos->y, pos->z, pos->w);
	return *pos;
}

/*signed int loadScript(char* a1) //Load Script Perhaps?
{
	LINFO << a1;
	return 0;
}

void SetString(DWORD address, char* string) 
{
    DWORD oldp;
    VirtualProtect((PVOID)address,2,PAGE_EXECUTE_READWRITE,&oldp);
    strcpy((PCHAR)address,string);
}*/

void getSavefilePath(int __unused, char * pBuffer, char * pszSaveName) {
	//strcpy_s (pBuffer, 256, pszPath);
	//strcat_s (pBuffer, 256, "FMP\\savegames");
	strcpy_s(pBuffer, 256, (execution::getPatch() == 0x1120 || execution::getPatch() == 0x1130 || 
		execution::getPatch() == 0x20000 ? "comMP//TBOGT" : "comMP//GTA4"));

	// check path and create directory if necessary
	DWORD attrs = GetFileAttributesA (pBuffer);
	if (attrs == INVALID_FILE_ATTRIBUTES) 
		CreateDirectoryA (pBuffer, NULL);
	else if (!(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
		//LINFO << "ERROR: unable to create directory " << pBuffer << " file " << pBuffer << " already exists";
		strcpy_s(pBuffer, 256, pszSaveName);
		return;
	}

	if (pszSaveName) {
		strcat_s(pBuffer, 256, "\\");
		strcat_s(pBuffer, 256, pszSaveName);
	}
    //LINFO << "Get Save Path: " << pBuffer;
}

HANDLE MainThreadHandle;
FMPHook HOOK;
void MainThread(void* dummy)
{
	LINFO << "Thread created";
	Sleep(10000);
	HOOK.AttachGtaThread("IVMPT4");
	LINFO << "Attached";

	CloseHandle(MainThreadHandle);
	MainThreadHandle = NULL;
}

unsigned int ADDRESS_THREADS_ARRAY = 0;
unsigned int ADDRESS_ACTIVE_THREAD = 0;
unsigned int ADDRESS_HASH_GET = 0;
unsigned int ADDRESS_THREAD_ID = 0;
unsigned int ADDRESS_SCRIPTS_COUNT = 0;
unsigned int ADDRESS_THREAD_TICK = 0;

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH: 
        {
			std::ofstream ofs;
			ofs.open("comMP//client.log", std::ofstream::out | std::ofstream::trunc);
			ofs.close();

			LINFO << "!Manolo: xlive";
			base = reinterpret_cast<unsigned int>(GetModuleHandle(NULL)) - 0x400000;

			LINFO << "!Manolo: pipe";
			memoryPipe::pulse();

			LINFO << "!Manolo: image";
			BYTE *pImageBase = (BYTE *)(base + 0x400000);
			PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBase;
			PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pImageBase + pDosHeader->e_lfanew);
			PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeader);
			char *pszSectionName; 

			for(int iSection = 0; iSection < pNtHeader->FileHeader.NumberOfSections; iSection++, pSection++)
			{
				// Set value for compare
				pszSectionName = (char*)pSection->Name;

				// Compare and check if the given memory segment identifier compares with our given segments
				if(!strcmp(pszSectionName, ".text") || !strcmp(pszSectionName, ".rdata"))
					CPatcher::Unprotect((DWORD)(pImageBase + pSection->VirtualAddress), ((pSection->Misc.VirtualSize + 4095) & ~4095));
			}

			LINFO << "!Manolo: patches";
			CXLiveHook::Install();
			LINFO << "!Manolo: testing patches";
			DWORD signature = *(DWORD*)(base + 0x608C34);
			LINFO << signature;

			DWORD gName = 0x47316F;
			DWORD gLoad[2] = {0x47342C, 0x4734F7};
			DWORD gSavePath = 0x6DEAB0;
			DWORD gWorldBrain[2] = {0xBCCA80, 0xBCCA20};
			DWORD gSpawnLoc = 0x946AB0;
			DWORD gDisableCars[2] = {0xAF5E50, 0x977B20};
			DWORD c_setup2Xml[2] = {0xD698A4, 0xD6970C};
			DWORD c_definitions[4] = {0xD702B8, 0xD702FC, 0xD70318, 0xD704F0};
			if(signature == 224137203)
			{
				execution::setPatch(0x1120);
				ADDRESS_THREADS_ARRAY = base + 0x186595C;
				ADDRESS_ACTIVE_THREAD = base + 0x16EAD50;
				ADDRESS_HASH_GET = base + 0x4CF830;
				ADDRESS_THREAD_ID = base + 0x16EAD4C;
				ADDRESS_SCRIPTS_COUNT = base + 0x16EAD68;
				ADDRESS_THREAD_TICK = base + 0xC2B880;
				execution::defineNatives(base + 0x4CF760);
			}
			else if(signature == 0x1006e857) //GTA4 1.7
			{
				execution::setPatch(0x1070);
				gName = 0x40287E;
				gLoad[0] = 0x402B3C, gLoad[1] = 0x402C07;
				gSavePath = 0x5B0110;
				gWorldBrain[0] = 0xB5DF20, gWorldBrain[1] = 0xB5DEC0;
				gSpawnLoc = 0x8E65B0;
				gDisableCars[0] = 0xB3EDF0, gDisableCars[1] = 0x9055D0;

				ADDRESS_THREADS_ARRAY = base + 0x1983310;
				ADDRESS_ACTIVE_THREAD = base + 0x1849AE0;
				ADDRESS_HASH_GET = base + 0x5A8290;
				ADDRESS_THREAD_ID = base + 0x1849ADC;
				ADDRESS_SCRIPTS_COUNT = base + 0x1849AF8;
				ADDRESS_THREAD_TICK = base + 0xBBCDF0;
				execution::defineNatives(base + 0x5A76D0);
			}
			else if(signature == 1078661135) //GTA4 1.8
			{
				execution::setPatch(0x1080);
				gName = 0x4900DE;
				gLoad[0] = 0x4903EC, gLoad[1] = 0x4904B7;
				gSavePath = 0x61E7D0;
				gWorldBrain[0] = 0xB0AD00, gWorldBrain[1] = 0xB0ACA0;
				gSpawnLoc = 0x9BC340;
				gDisableCars[0] = 0x9BE430, gDisableCars[1] = 0xCA1B40;

				ADDRESS_THREADS_ARRAY = base + 0x190FDB8;
				ADDRESS_ACTIVE_THREAD = base + 0x1794FB4;
				ADDRESS_HASH_GET = base + 0x6276B0;
				ADDRESS_THREAD_ID = base + 0x1794FB0;
				ADDRESS_SCRIPTS_COUNT = base + 0x1794FCC;
				ADDRESS_THREAD_TICK = base + 0xB60730;
				execution::defineNatives(base + 0x626AF0);
			}
			else if(signature == 79820288) //EFLC 1.3
			{
				execution::setPatch(0x1130);
				gName = 0x56941E;
				gLoad[0] = 0x56972C, gLoad[1] = 0x5697F7;
				gSavePath = 0x5B8900;
				gWorldBrain[0] = 0xAD6600, gWorldBrain[1] = 0xAD65A0;
				gSpawnLoc = 0x986D70;
				gDisableCars[0] = 0x988E60, gDisableCars[1] = 0xC6D490;
				c_setup2Xml[0] = 0xD7CB2C, c_setup2Xml[1] = 0xD7CCC4;
				c_definitions[0] = 0xDADBA0, c_definitions[1] = 0xDB7AF8;
				c_definitions[2] = 0xDB7B3C, c_definitions[3] = 0xDB7B58;

				ADDRESS_THREADS_ARRAY = base + 0x1902AA0;
				ADDRESS_ACTIVE_THREAD = base + 0x17E4D88;
				ADDRESS_HASH_GET = base + 0x54F030;
				ADDRESS_THREAD_ID = base + 0x17E4D84;
				ADDRESS_SCRIPTS_COUNT = base + 0x17E4DA0;
				ADDRESS_THREAD_TICK = base + 0xB2BEE0;
				execution::defineNatives(base + 0x54E470);


			}
			else
			{
				LINFO << "Invalid game version detected";
				return TRUE;
			}

			//*(DWORD*)(base + gName) = (DWORD)("IV:MP T4");

			if(execution::getPatch() == 0x1120 || execution::getPatch() == 0x1130 || execution::getPatch() == 0x20000)
			{
				//Block disabled since its not necessary
				LINFO << "EFLC configs";
				char myStr[11] = "IVstup.xml";
				char str2[25] = "common:/data/ivmpdef.dat";
				if(memoryPipe::runAsGTA4())
				{
					LINFO << "Run EFLC using GTA4 data";
					sprintf_s(myStr, "%s", "IVstu4.xml");
					sprintf_s(str2, "%s", "common:/data/ivmpde4.dat");

					execution::setCompatibility();

				}
				char* buffer = reinterpret_cast<char*>(base + c_setup2Xml[0]);
				memcpy(buffer, myStr, 11);

				buffer = reinterpret_cast<char*>(base + c_setup2Xml[1]);
				memcpy(buffer, myStr, 11);

				for(size_t i = 0; i < 4; i++)
				{
					buffer = reinterpret_cast<char*>(base + c_definitions[i]);
					//LINFO << "Buffer " << i << ": " << buffer;
					memcpy(buffer, str2, 25);
				}
			}

			CPatcher::InstallJmpPatch(base + gLoad[0], base + gLoad[1]); //Sleep, Load saved game
			CPatcher::InstallJmpPatch(base + gSavePath, (DWORD)getSavefilePath);

			//Savegames path 006DEAB0

			CPatcher::InstallJmpPatch(base + gWorldBrain[0], (DWORD)IS_WORLD_POINT_WITHIN_BRAIN_ACTIVATION_RANGE);
			CPatcher::InstallJmpPatch(base + gWorldBrain[1], (DWORD)IS_OBJECT_WITHIN_BRAIN_ACTIVATION_RANGE);

			CPatcher::InstallJmpPatch(base + gSpawnLoc, (DWORD)GET_SPAWN_LOCATION);

			//disable parked cars and fake
			CPatcher::InstallRetnPatch(base + gDisableCars[0]);
			CPatcher::InstallRetnPatch(base + gDisableCars[1]);

			LINFO << "!Manolo: class initing";
			/*_customThread = new CustomFiberThread();
			ScriptHookManager::RegisterThread(_customThread);*/
			
			d3dHook::setWindowed();
			d3dHook::install();

			DWORD threadId = 0;
			MainThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&MainThread, 0, 0, (LPDWORD)&threadId);

			SetDllDirectoryA("comMP//TeamSpeak");
			LoadLibrary("comMP//TeamSpeak//IVMP-TS.dll");

			//CPatcher::InstallJmpPatch(base + 0x833700, (DWORD)loadScript);
			//SetString(base + 0xD5EC80, "main"); //start script name
            break;
        }
        case DLL_PROCESS_DETACH:
        {
			//memoryPipe::writePipeMsg(std::string("GONE"));
			/*if(_customThread != nullptr)
			{
				delete _customThread;
				_customThread = nullptr;
			}*/
			if(MainThreadHandle != NULL)
			{
				TerminateThread(MainThreadHandle, 1);
				CloseHandle(MainThreadHandle);
			}
			LINFO << "Last Manager Task: " << currentTask;
			break;
        }
    }

    return TRUE;
}

unsigned int getBase()
{
	return base;
}