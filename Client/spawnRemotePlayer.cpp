#include "players.h"
#include "CustomFiberThread.h"
#include <sstream>
#include "simpleMath.h"
#include <time.h>
#include "easylogging++.h"
#include "playerSkins.h"
#include "playerMemory.h"
#include "gameMemory.h"
#include "modelsHandler.h"
#include "colorHelper.h"

bool spawnRemotePlayer(int id, players::netPlayer& p)
{
	try
	{
		//LINFO << "Spawning player " << p.serverId;

		//LINFO << "Requesting model";
		unsigned long model = p.skin;
		
		/*Scripting::RequestModel(model);
		for(int i = 0; i < 5; i++)
		{
			if(!Scripting::HasModelLoaded(model) && i == 4)
			{
				LINFO << "!Error: player model didnt load";
				return false;
			}
		}*/
		modelsHandler::request(model, clock());
		if(!Scripting::HasModelLoaded(model))
		{
			//LINFO << "!Error: player model didnt load";
			return false;
		}

		LINFO << "model available: " << model;

		//i->second.model = model;
		LINFO << "Spawning char";

		Scripting::CreateChar(1, model, p.pos.x, p.pos.y, p.pos.z, &p.ped, true);
		LINFO << "Char created: " << p.pos.x << ", " << p.pos.y << ", " << p.pos.z;

		if(!Scripting::DoesCharExist(p.ped))
		{
			LINFO << "!Error: player istance wasnt created: count " << players::streamedPlayers() << ", " << players::spawnedPlayers();
			return false;
		}
		LINFO << "Char waiting spawn";
		//Scripting::MarkModelAsNoLongerNeeded(model);
		modelsHandler::unload(model);

		p.memoryAddress = gameMemory::getAddressOfItemInPool( *(DWORD*)(gameMemory::getPlayersPool()), p.ped.Get());
		if(p.memoryAddress == 0)
		{
			LINFO << "!Error: player " << p.serverId << " memory not found";
			return false;
		}

		/*Scripting::eInteriorRoomKey roomKey;
		Scripting::GetKeyForCharInRoom(p.ped, &roomKey);
		Scripting::SetRoomForCharByKey(p.ped, roomKey);*/
		if(p.interior != 0)
		{
			LINFO << "Setting room: " << p.interior;
			//NativeInvoke::Invoke<u32>("SET_ROOM_FOR_CHAR_BY_KEY", p.ped, p.interior);
			Scripting::SetRoomForCharByKey(p.ped, p.interior);
			LINFO << "Interior  defined";
		}

		p.spawned = true;

		Scripting::SetBlockingOfNonTemporaryEvents(p.ped, true);
		Scripting::SetCharInvincible(p.ped, 1);
		Scripting::BlockCharGestureAnims(p.ped, true);
		Scripting::AllowReactionAnims(p.ped, false);
		//NativeInvoke::Invoke<u32>("SET_DONT_ACTIVATE_RAGDOLL_FROM_PLAYER_IMPACT", p.ped, true);
		Scripting::SetDontActivateRagdollFromPlayerImpact(p.ped, true);
		//NativeInvoke::Invoke<u32>("SET_PED_DONT_DO_EVASIVE_DIVES", p.ped, true);
		Scripting::SetPedDontDoEvasiveDives(p.ped, true);
		//NativeInvoke::Invoke<u32>("SET_CHAR_FORCE_DIE_IN_CAR", p.ped, true);
		Scripting::SetCharForceDieInCar(p.ped, true);
		//NativeInvoke::Invoke<u32>("SET_CHAR_GET_OUT_UPSIDE_DOWN_CAR", p.ped, false);
		Scripting::SetCharGetOutUpsideDownCar(p.ped, false);
		//NativeInvoke::Invoke<u32>("ENABLE_PED_HELMET", p.ped, false);
		Scripting::EnablePedHelmet(p.ped, false);
		//NativeInvoke::Invoke<u32>("SET_CHAR_WILL_DO_DRIVEBYS", p.ped, true);
		Scripting::SetCharWillDoDrivebys(p.ped, true);

		Scripting::SetCharAccuracy(p.ped, 100);
		//Scripting::SetCharShootRate(p.ped, 100);

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

		if(id < 1000 && p.shouldAddBlip)
		{
			Scripting::AddBlipForChar(p.ped, &p.blip);
			Scripting::ChangeBlipNameFromAscii(p.blip, p.playerName);

			unsigned int colorHex = (p.color[0]<<24)|(p.color[1]<<16)|(p.color[2]<<8)|p.color[3];
			//NativeInvoke::Invoke<u32>("CHANGE_BLIP_COLOUR", p.blip, colorHex);
			Scripting::ChangeBlipColour(p.blip, colorHex);
		}

		//NativeInvoke::Invoke<u32>("SET_CHAR_DROPS_WEAPONS_WHEN_DEAD", p.ped, false);
		Scripting::SetCharDropsWeaponsWhenDead(p.ped, false);

		//NativeInvoke::Invoke<u32>("GIVE_PED_FAKE_NETWORK_NAME", p.ped, p.playerName.c_str(), 255, 0, 0, 255);
		//char *cstr = &p.playerName[0u];
		//Scripting::RemoveFakeNetworkNameFromPed(p.ped.Get());
		//Scripting::GivePedFakeNetworkName(p.ped, "Alohaa", 0, 110, 220, 255);

		LINFO << "Char spawned";

		if(p.currentWeapon != 0)
		{
			LINFO << "Giving weapon";
			Scripting::GiveWeaponToChar(p.ped, p.currentWeapon, 500, false);
		}
		
		LINFO << "Spawn completed";
		return true;
		//Scripting::SetPedAlpha(i->second.ped, 80.0);
		//Scripting::SetCharCollision(i->second.ped, 0);
	}
	catch(std::exception& e)
	{
		LINFO << "Error (spawnRemotePlayer): " << e.what();
		return false;
	}
}