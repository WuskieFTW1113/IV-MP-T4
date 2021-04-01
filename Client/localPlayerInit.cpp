#include "NetworkManager.h"
#include "players.h"
#include "CustomFiberThread.h"
#include "easylogging++.h"
#include "playerSkins.h"
#include "gameMemory.h"
#include "modelsHandler.h"
#include "localPlayerAnim.h"
#include "localPlayerInvent.h"
#include "../clientLibs/execution.h"
#include "playerServerFunctions.h"
#include "../clientLibs/EFLCScripts.h"
#include "../clientLibs/execution.h"

bool doorStatus = false;

bool players::respawnLocalPlayer()
{
	try
	{
		players::netPlayer& p = players::getLocalPlayer();
		subTask = 21;
		//LINFO << p.skin;
		if(!modelsHandler::wasRequested(p.skin) || !Scripting::HasModelLoaded(p.skin))
		{
			modelsHandler::request(p.skin, clock());
			return false;
		}

		subTask = 22;
		u32 hp = 200;
		if(p.spawned)
		{
			LINFO << "Storing weaps";
			localInvent::storeWeapons();
			Scripting::GetCharHealth(p.ped, &hp);
		}

		p.spawned = false;
		subTask = 23;

		LINFO << "Defining player char";

		p.playerIndex = Scripting::ConvertIntToPlayerIndex(Scripting::GetPlayerId());
		Scripting::GetPlayerChar(p.playerIndex, &p.ped);

		unsigned int interior = 0;
		p.interior = 1;
		//NativeInvoke::Invoke<u32>("GET_KEY_FOR_CHAR_IN_ROOM", p.ped, &interior);
		Scripting::GetKeyForCharInRoom(p.ped, &interior);
		LINFO << "interior defined: " << interior;

		Scripting::ChangePlayerModel(p.playerIndex, p.skin);
		LINFO << "model changed";

		modelsHandler::unload(p.skin);

		p.playerIndex = Scripting::ConvertIntToPlayerIndex(Scripting::GetPlayerId());
		Scripting::GetPlayerChar(p.playerIndex, &p.ped);
		LINFO << "playerChar ok after model change";

		//Scripting::SetRoomForCharByKey(p.ped, p.interior);
		/*LINFO << "Setting room: " << interior;
		NativeInvoke::Invoke<u32>("SET_ROOM_FOR_CHAR_BY_KEY", p.ped, interior);
		LINFO << "room defined";*/

		Scripting::SetCharCoordinates(p.ped, p.pos.x, p.pos.y, p.pos.z);
		LINFO << "TPed: " << p.pos.x << ", " << p.pos.y << ", " << p.pos.z;

		//Scripting::SetCharDefaultComponentVariation(p.ped);
		players::setClothes(p, -1);
		for(size_t i = 0; i < 2; i++)
		{
			if(p.props[i] != -1)
			{
				//NativeInvoke::Invoke<u32>("SET_CHAR_PROP_INDEX", p.ped, i, p.props[i]);
				Scripting::SetCharPropIndex(p.ped, i, p.props[i]);
			}
		}

		p.currentWeapon = 0;
		p.currentAmmo = 0;
		p.startAmmo = 0;
		p.weaponChanged = false;

		p.currentAnim = 0;
		p.oldAnim = 0;

		p.inVehicle = 0;
		p.lastVehicle = 0;
		p.passangerOf = 0;
		p.passangerSeat = -1;

		p.isDucking = false;
		p.toBeDeleted = 0;

		p.memoryAddress = gameMemory::getAddressOfItemInPool( *(DWORD*)(gameMemory::getPlayersPool()), p.ped.Get());
		if(p.memoryAddress == 0)
		{
			LINFO << "!Error: localPlayer memory not found";
			Scripting::PrintStringWithLiteralStringNow("STRING", "localPlayer memory not found", 5000, true);
			return false;
		}

		p.spawned = true;

		Scripting::SetCharWillDoDrivebys(p.ped, false);
		Scripting::SetPlayerCanDoDriveBy(p.playerIndex, false);
		//NativeInvoke::Invoke<u32>("SET_CHAR_WILL_USE_COVER", p.ped, false);
		Scripting::SetCharWillUseCover(p.ped, false);
		//NativeInvoke::Invoke<u32>("SET_PLAYER_CAN_USE_COVER", p.playerIndex, false);
		Scripting::SetPlayerCanUseCover(p.playerIndex, false);

		//NativeInvoke::Invoke<u32>("DISABLE_PLAYER_VEHICLE_ENTRY", p.playerIndex, false);
		//NativeInvoke::Invoke<u32>("BLOCK_COWERING_IN_COVER", p.ped, true);
		Scripting::BlockCoweringInCover(p.ped, true);

		//NativeInvoke::Invoke<u32>("BLOCK_PEEKING_IN_COVER", p.ped, true);
		Scripting::BlockPeekingInCover(p.ped, true);
		//NativeInvoke::Invoke<u32>("ENABLE_PED_HELMET", p.ped, false);
		Scripting::EnablePedHelmet(p.ped, false);
		//NativeInvoke::Invoke<u32>("ALLOW_STUNT_JUMPS_TO_TRIGGER", false);
		Scripting::AllowStuntJumpsToTrigger(false);
		//NativeInvoke::Invoke<u32>("SET_PLAYER_NEVER_GETS_TIRED", p.playerIndex, false);
		Scripting::SetPlayerNeverGetsTired(p.playerIndex, false);

		//NativeInvoke::Invoke<u32>("SET_PLAYER_INVINCIBLE", p.playerIndex, false);
		Scripting::SetPlayerInvincible(p.playerIndex, false);
		//NativeInvoke::Invoke<u32>("SET_CHAR_DROPS_WEAPONS_WHEN_DEAD", p.ped, false);
		Scripting::SetCharDropsWeaponsWhenDead(p.ped, false);
		//NativeInvoke::Invoke<u32>("SET_DEAD_PEDS_DROP_WEAPONS", 0);
		Scripting::SetDeadPedsDropWeapons(false);
		//NativeInvoke::Invoke<u32>("ALLOW_PLAYER_TO_CARRY_NON_MISSION_OBJECTS", p.playerIndex, 0);
		Scripting::AllowPlayerToCarryNonMissionObjects(p.playerIndex, 0);
		//NativeInvoke::Invoke<u32>("SET_CHAR_MONEY", p.ped, getClientMoney());

		localInvent::refreshWeapons();
		Scripting::SetCharHealth(p.ped, hp);

		subTask = 24;
		const char* grp = Scripting::GetAnimGroupFromChar(p.ped);
		changeDefaultAnimGroup(_strdup(grp));
		LINFO << "Defined default move: " << grp;

		subTask = 25;
		gameMemory::closeDoors(gameMemory::getDoorStatus());

		//NativeInvoke::Invoke<u32>("ENABLE_SAVE_HOUSE", false);
		Scripting::EnableSaveHouse(false);
		return true;
	}
	catch(std::exception& e)
	{
		LINFO << "!Error: respawnLocalPlayer: " << e.what();
		return false;
	}
}

void initPlayerStatus()
{
	if(!players::isPlayer(-2))
	{
		LINFO << "Error: Local player not found (PlayerInit)";
		return;
	}

	Scripting::ClearAreaOfChars(0.0f, 0.0f, 0.0f, 10000.0f);
	Scripting::ClearAreaOfCars(0.0f, 0.0f, 0.0f, 1000.0f);

	players::netPlayer& p = players::getLocalPlayer();
	for(size_t i = 0; i < 10; i++)
	{
		p.clothes[i] = 0;
	}

	for(size_t i = 0; i < 2; i++)
	{
		p.props[i] = -1;
	}

	if(!players::respawnLocalPlayer())
	{
		return;
	}

	networkManager::connection* con = networkManager::getConnection();
	if(con == nullptr)
	{
		return;
	}
	con->connectStat = 4;

	p.toBeDeleted = 0;
	
	execution::setSpawned(true);
	
	//*(BYTE *)(gameMemory::getModuleHandle() + 0x1906FB8) = (BYTE)true; player names visible
	//ClearAreaOfCops(0,0,0, 2000);

	LINFO << "LocalPlayer check: " << p.playerIndex << ", " << p.skin;
	gameMemory::closeDoors(false);
	LINFO << "All done";

	EFLCScripts::turnOffGTAScripts();
}

void forceNewClothe(players::netPlayer &p, int part)
{
	unsigned char ucClothesIdx = 0;
	//unsigned int uiDrawableVariations = NativeInvoke::Invoke<u32>("GET_NUMBER_OF_CHAR_DRAWABLE_VARIATIONS", p.ped, part);
	unsigned int uiDrawableVariations = Scripting::GetNumberOfCharDrawableVariations(p.ped, part);
	//LINFO << "uiDrawableVariations: " << uiDrawableVariations;

	for(unsigned int uiDrawable = 0; uiDrawable < uiDrawableVariations; ++uiDrawable)
	{
		//unsigned int uiTextureVariations = NativeInvoke::Invoke<u32>("GET_NUMBER_OF_CHAR_TEXTURE_VARIATIONS", p.ped, part, uiDrawable);
		unsigned int uiTextureVariations = Scripting::GetNumberOfCharTextureVariations(p.ped, part, uiDrawable);
		//LINFO << "uiTextureVariations: " << uiTextureVariations;

		for(unsigned int uiTexture = 0; uiTexture < uiTextureVariations; ++uiTexture)
		{
			if(ucClothesIdx == p.clothes[part])
			{
				//NativeInvoke::Invoke<u32>("SET_CHAR_COMPONENT_VARIATION", p.ped, part, uiDrawable, uiTexture);
				Scripting::SetCharComponentVariation(p.ped, part, uiDrawable, uiTexture);
				//CLogFile::Printf(__FILE__,__LINE__,"CNetworkPlayer::SetClothes body: %d variat: %d text: %d", ucBodyPart, uiDrawable, uiTexture);
				//Scripting::SetCharComponentVariation(GetScriptingHandle(), (Scripting::ePedComponent)ucBodyPart, uiDrawable, uiTexture);
				//m_ucClothes[ucBodyPart] = ucClothes;
				return;
			}

			++ucClothesIdx;
		}
	}
}

void players::setClothes(netPlayer &p, int part)
{
	//NativeInvoke::Invoke<u32>("SET_CHAR_COMPONENT_VARIATION", pn.ped, variationId, variationValue, variationValue);
	if(part != -1)
	{
		forceNewClothe(p, part);
	}
	else
	{
		for(unsigned int i = 0; i < 10; i++)
		{
			forceNewClothe(p, i);
		}
	}
}

void gameMemory::closeDoors(bool status)
{
	doorStatus = status;
	LINFO << "Main doors: " << (int)doorStatus;

	double doors[42][4] = {
		{2842627855, -970.178f, 883.922f, 19.254f},
		{2842627855, -1247.28f, 1539.76f, 26.3143f},
		{2842627855, -1245.77f, 1559.83f, 26.3143f},
		{1033979537, -127.086f, 1501.06f, 23.0294f},
		{106751028, -548.015f, 1249.79f, 97.7845f},
		{2842627855, -439.768f, 1390.39f, 16.7084f},
		{2690137464, -435.109f, 1391.41f, 16.7111f},
		{30401623, 602.424f, 1404.52f, 17.7216f},
		{2842627855, 865.637f, -517.823f, 16.5235f},
		{257820338, 850.785f, -517.827f, 16.5312f},
		{2124429686, 1382.95f, 529.195f, 33.3645f},
		{807349477, 1370.57f, 532.18f, 41.8674f},
		{2181386400, 895.992f, -504.236f, 15.4044f},
		{3351646913, 893.318f, -501.519f, 19.6403f},
		{804737190, 561.959f, 1392.26f, 31.1051f},
		{3869956964, -155.838f, 593.187f, 118.962f},
		{807349477, 693.752f, 1457.79f, 15.1073f},
		{261592072, 1149.67f, 735.311f, 35.7645f},
		{1285262331, 885.155f, -481.797f, 16.1545f},
		{3232592925, 891.73f, -485.4f, 16.1483f},
		{387699963, 928.401f, -489.281f, 15.7347f},
		{387699963, 947.649f, -491.424f, 15.7348f},
		{1985756882, 958.987f, -289.154f, 20.1272f},
		{2762578800, 955.987f, -289.154f, 20.1272f},
		{2881168431, 968.165f, -260.185f, 22.0196f},
		{2881168431, 968.165f, -257.181f, 22.0196f},
		{1542565804, 1474.93f, 53.7353f, 25.4617f},
		{903328250, 1192.81f, 374.842f, 25.343f},
		{2862357381, 1189.81f, 374.842f, 25.343f},
		{149395267, 27.0681f, 979.575f, 14.8975f},
		{4114682006, -440.552f, 456.534f, 10.6446f},
		{3986654918, 95.4047f, -681.855f, 15.0167f},
		{487482787, 95.4081f, -684.855f, 15.0167f},
		{807349477, -618.09f, 1203.31f, 6.35166f},
		{4233865074, -1721.13f, 353.753f, 26.5913f},
		{1643309849, -1726.41f, 341.832f, 26.5816f},
		{807349477, -1729.52f, 326.074f, 42.657f},
		{2690137464, 98.8124f, -436.12f, 20.5958f},
		{3863802474, -27.9202f, -462.903f, 15.641f},
		{866127123, -27.9202f, -465.906f, 15.641f},
		{3863802474, -27.9202f, -466.891f, 15.641f},
		{866127123, -27.9202f, -469.893f, 15.641f}
	};

	//NativeInvoke::Invoke<u32>("SET_STATE_OF_CLOSEST_DOOR_OF_TYPE", 172341554, 54.4855f, 809.273f, 15.6563f, 0, 0.0f);
	//NativeInvoke::Invoke<u32>("SET_STATE_OF_CLOSEST_DOOR_OF_TYPE", 1943833694, 51.805f, 806.593f, 15.6563f, 0, 0.0f);
	Scripting::SetStateOfClosestDoorOfType(172341554, 54.4855f, 809.273f, 15.6563f, status ? 1 : 0, 0.0f);
	Scripting::SetStateOfClosestDoorOfType(1943833694, 51.805f, 806.593f, 15.6563f, status ? 1 : 0, 0.0f);

	//NativeInvoke::Invoke<u32>("SET_STATE_OF_CLOSEST_DOOR_OF_TYPE", 725112888, -398.313f, 397.589f, 14.6575f, 1, 0.0f);
	//NativeInvoke::Invoke<u32>("SET_STATE_OF_CLOSEST_DOOR_OF_TYPE", 4119540397, -468.057f, 152.635f, 10.1434f, 1, 0.0f);
	//NativeInvoke::Invoke<u32>("SET_STATE_OF_CLOSEST_DOOR_OF_TYPE", 1316667213, -440.296f, 357.688f, 11.965f, 1, 0.0f);
	if(execution::getPatch() > 0x1080)
	{
		Scripting::SetStateOfClosestDoorOfType(725112888, -398.313f, 397.589f, 14.6575f, status ? 1 : 0, 0.0f);
		Scripting::SetStateOfClosestDoorOfType(4119540397, -468.057f, 152.635f, 10.1434f, status ? 1 : 0, 0.0f);
		Scripting::SetStateOfClosestDoorOfType(1316667213, -440.296f, 357.688f, 11.965f, status ? 1 : 0, 0.0f);
	}

	for(size_t i = 0; i < 42; i++)
	{
		//LINFO << "Locking " << i;
		//NativeInvoke::Invoke<u32>("SET_STATE_OF_CLOSEST_DOOR_OF_TYPE", (unsigned int)doors[i][0], 
			//(float)doors[i][1], (float)doors[i][2], (float)doors[i][3], status ? 1 : 0, 0.0f);
		if(!Scripting::IsModelInCdImage((u32)doors[i][0])) continue;

		Scripting::SetStateOfClosestDoorOfType((unsigned int)doors[i][0],
			(float)doors[i][1], (float)doors[i][2], (float)doors[i][3], status ? 1 : 0, 0.0f);
	}
}

bool gameMemory::getDoorStatus()
{
	return doorStatus;
}