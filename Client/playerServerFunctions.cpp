#include "playerServerFunctions.h"
#include "easylogging++.h"
#include "localPlayerInvent.h"
#include "Scripting.h"
#include "players.h"
#include "vehicles.h"
#include "gameWorld.h"
#include "prepareClientToServerPacket.h"
#include "../clientLibs/EFLCScripts.h"
#include "../SharedDefines/packetsIds.h"
#include "gameMemory.h"

void weaponGiven(RakNet::BitStream& bsIn)
{
	try
	{
		unsigned int weap;
		unsigned int ammo;
		
		bsIn.Read(weap);
		bsIn.Read(ammo);

		localInvent::addWeapon(weap, ammo);	
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (weaponGiven): " << e.what();
	}
}

void weaponTaken(RakNet::BitStream& bsIn)
{
	try
	{
		unsigned int weap;
		bsIn.Read(weap);

		localInvent::removeWeapon(weap);
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (weaponTaken): " << e.what();
	}
}

void setHealth(RakNet::BitStream& bsIn)
{
	try
	{
		if(players::getLocalPlayer().spawned)
		{
			int hp;
			bsIn.Read(hp);
		
			Scripting::SetCharHealth(players::getLocalPlayer().ped, hp);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (weaponGiven): " << e.what();
	}
}

void setSpawnPos(RakNet::BitStream& bsIn)
{
	try
	{
		float x, y, z;
		bsIn.Read(x);
		bsIn.Read(y);
		bsIn.Read(z);
		
		gameWorld::setLocalSpawnCoords(x, y, z);
		EFLCScripts::setSpawnPos(x, y, z, 0.0f);
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (weaponGiven): " << e.what();
	}
}

void ejectOut(RakNet::BitStream& bsIn)
{
	try
	{
		players::netPlayer& p = players::getLocalPlayer();
		if(!p.spawned)
		{
			return;
		}
		if(isPlayerEnteringCar())
		{
			vehicleEntryCancelled();
		}
		Scripting::TaskLeaveAnyCar(players::getLocalPlayer().ped);
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (weaponGiven): " << e.what();
	}
}

void tpIntoCarSeat(RakNet::BitStream& bsIn)
{
	try
	{
		int carId;
		int seatId;
		bsIn.Read(carId);
		bsIn.Read(seatId);

		if(vehicles::isVehicle(carId) && vehicles::getVehicle(carId).spawned)
		{
			if(seatId == 0)
			{
				Scripting::WarpCharIntoCar(players::getLocalPlayer().ped, vehicles::getVehicle(carId).vehicle);
			}
			else
			{
				Scripting::WarpCharIntoCarAsPassenger(players::getLocalPlayer().ped, vehicles::getVehicle(carId).vehicle, seatId - 1);
			}
		}

	}
	catch(std::exception& e)
	{
		LINFO << "!Error (weaponGiven): " << e.what();
	}
}

void freezeClient(RakNet::BitStream& bsIn)
{
	try
	{
		int freeze;
		bsIn.Read(freeze);

		if(freeze == 0)
		{
			Scripting::SetPlayerControl(players::getLocalPlayer().playerIndex, true);
			gameWorld::setPlayerFrozen(false);
		}
		else if(freeze == 1)
		{
			Scripting::SetPlayerControl(players::getLocalPlayer().playerIndex, false);
			Scripting::SetCameraControlsDisabledWithPlayerControls(false);
			gameWorld::setPlayerFrozen(true);
		}

	}
	catch(std::exception& e)
	{
		LINFO << "!Error (freezeClien): " << e.what();
	}
}

void getClientWeapons(RakNet::BitStream& bsIn)
{
	try
	{
		RakNet::BitStream b;
		b.Write((MessageID)IVMP);
		b.Write(PLAYER_WEAPONS_INVENT);

		std::map<unsigned int, unsigned int> guns;
		for(unsigned int i = 0; i < 10; i++)
		{
			unsigned int weapon = 0;
			unsigned int ammo = 0;
			unsigned int useless = 0;
			Scripting::GetCharWeaponInSlot(players::getLocalPlayer().ped, i, &weapon, &ammo, &useless);
					
			if(weapon > 0)
			{
				guns[weapon] = ammo;
			}
		}

		b.Write(guns.size());
		for(std::map<unsigned int, unsigned int>::iterator i = guns.begin(); i != guns.end(); ++i)
		{
			b.Write(i->first);
			b.Write(i->second);
		}
		networkManager::sendBitStream(b);
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (getClientWeapons): " << e.what();
	}
}

void setArmor(RakNet::BitStream& bsIn)
{
	try
	{
		if(players::getLocalPlayer().spawned)
		{
			int hp = 0;
			bsIn.Read(hp);

			u32 armor = 0;
			Scripting::GetCharArmour(players::getLocalPlayer().ped, &armor);
			Scripting::AddArmourToChar(players::getLocalPlayer().ped, hp - armor);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (armorChange): " << e.what();
	}
}

int clientMoney = 0;
int getClientMoney()
{
	return clientMoney;
}

void setClientMoney(RakNet::BitStream& bsIn)
{
	bsIn.Read(clientMoney);
	
	if(!players::getLocalPlayer().spawned) return;

	u32 idx = players::getLocalPlayer().playerIndex;
	u32 s;
	Scripting::StoreScore(idx, &s);
	if(clientMoney < 0) clientMoney = 0;
	Scripting::AddScore(idx, clientMoney - s);
}

void setClientDoor(RakNet::BitStream& bsIn)
{
	int model = 0;
	float pos[3] = {0.0f};
	int status = 0;
	bsIn.Read(model);
	if(model != 0)
	{
		bsIn.Read(pos[0]);
		bsIn.Read(pos[1]);
		bsIn.Read(pos[2]);
	}
	bsIn.Read(status);

	if(model != 0)
	{
		//NativeInvoke::Invoke<u32>("SET_STATE_OF_CLOSEST_DOOR_OF_TYPE", model, pos[0], pos[1], pos[2], status == 1 ? 1 : 0, 0.0f);
		Scripting::SetStateOfClosestDoorOfType(model, pos[0], pos[1], pos[2], status == 1 ? 1 : 0, 0.0f);
		return;
	}
	gameMemory::closeDoors(status == 1 ? true : false);
}

void explodeAt(RakNet::BitStream& bsIn)
{
	simpleMath::Vector3 pos;
	int typ;
	float radius, cam;
	bsIn.Read(pos.x);
	bsIn.Read(pos.y);
	bsIn.Read(pos.z);
	bsIn.Read(typ);
	bsIn.Read(radius);
	bsIn.Read(cam);

	Scripting::AddExplosion(pos.x, pos.y, pos.z, typ, radius, true, false, cam);
}

#include "localPlayerAnim.h"
void defaultSitIdle(RakNet::BitStream& bsIn)
{
	RakNet::RakString a, b;
	int c;
	bsIn.Read(a);
	bsIn.Read(b);
	bsIn.Read(c);

	setSittingAnim(std::string(a), std::string(b), c);
}

void taskSitNow(RakNet::BitStream& bsIn)
{
	forceSitNow();
}

std::map<int, int> sounds;

void playSound(RakNet::BitStream& bsIn)
{
	int id = -1, sound = -1;
	bsIn.Read(id);

	if(id != -1) {
		sound = Scripting::GetSoundId();
		sounds[id] = sound;
	}

	RakNet::RakString name;
	bsIn.Read(name);

	bool fromPos = false;
	bsIn.Read(fromPos);

	//LINFO << "Sound: " << id << "(" << sound << "), " << name << ", " << (int)fromPos;

	if(fromPos) {
		float p[3];
		for(int i = 0; i < 3; i++) bsIn.Read(p[i]);
		Scripting::PlaySoundFromPos(sound, name.C_String(), p[0], p[1], p[2]);
	}
	else {
		Scripting::PlaySoundFrontend(sound, name.C_String());
	}
}

void stopSound(RakNet::BitStream& bsIn)
{
	int id = -1;
	bsIn.Read(id);

	if(sounds.find(id) == sounds.end()) return;
	int s = sounds[id];
	Scripting::StopSound(s);
	Scripting::ReleaseSoundId(s);
	sounds.erase(id);
}

void setHeading(RakNet::BitStream& bsIn)
{
	try
	{
		if(players::getLocalPlayer().spawned)
		{
			float h;
			bsIn.Read(h);

			Scripting::SetCharHeading(players::getLocalPlayer().ped, h);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (headingChange): " << e.what();
	}
}

void setHudSettings(RakNet::BitStream& bsIn)
{
	try
	{
		bool set[2];
		for(int i = 0; i < 2; i++) bsIn.Read(set[i]);
		Scripting::DisplayHUD(set[0]);
		Scripting::DisplayRadar(set[1]);
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (hudSettings): " << e.what();
	}
}