#include "parseOnFootPlayerPacket.h"
#include "requestAndReceivePlayerData.h"
#include "players.h"
#include "easylogging++.h"
#include "simpleMath.h"
#include "animList.h"
#include "vehicles.h"
#include "playerList.h"
#include "chatWindow.h"
#include "localPlayerAnim.h"
#include "CustomFiberThread.h"
#include "gameMemory.h"
#include "../ClientManager/ivScripting.h"

void parseOnFootSync(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);
				
		if(!players::isPlayer(playerid))
		{
			//requestPlayerDate(playerid);
			return;
		}

		players::netPlayer& p = players::getPlayer(playerid);

		p.noLongerDriving(false);
		if(p.passangerOf != 0)
		{
			LINFO << "Player " << playerid << " is no longer passenger of vehicle " << p.passangerOf;
		}

		p.inVehicle = 0;
		p.passangerOf = 0;
		p.passangerSeat = -1;

		bsIn.Read(p.endPos.x);
		bsIn.Read(p.endPos.y);
		bsIn.Read(p.endPos.z);
		bsIn.Read(p.endHeading);

		/*float bx = p.endPos.x - p.pos.x;
		float by = p.endPos.y - p.pos.y;*/
		p.cAtan = ((p.endHeading + 90) * 3.141592f) / 180.0f;

		int animBuf = 0;
		bsIn.Read(animBuf);

		//if(p.currentAnim != 10003)
		//{
			p.currentAnim = animBuf;
		//}

		//bsIn.Read(p.currentAnim);

		//bsIn.Read(p.polTime);

		if(p.currentAnim >= 1 && p.currentAnim <= 4)
		{
			bsIn.Read(p.endAim.x);
			bsIn.Read(p.endAim.y);
			bsIn.Read(p.endAim.z);

			//bsIn.Read(p.endAmmo);

			if(p.spawned)
			{
				Scripting::SetAmmoInClip(p.ped, p.currentWeapon, 100);		
			}
		}

		clock_t cTime = clock();

		/*if(p.spawned)
		{
			Scripting::GetCharCoordinates(p.ped, &p.startPos.x, &p.startPos.y, &p.startPos.z);
			p.startPos.z -= 1.0f;
			LINFO << "Start: " << p.startPos.x << ", " << p.startPos.y << ", "<< p.startPos.z;
		}
		else
		{
			p.startPos = p.pos;
			LINFO << "Used old";
		}*/
		p.startHeading = p.heading;
		p.startAim = p.aim;
		p.startAmmo = cTime;

		p.startPos = p.pos;
		//LINFO << "Start: " << p.startPos.x << ", " << p.startPos.y << ", "<< p.startPos.z;
		p.posError = p.endPos;
		p.posError.minus(p.startPos);
		
		p.polTime = cTime > p.lastPol + 600 ? 100 : (cTime - p.lastPol);
		if(p.polTime < 1)
		{
			p.polTime = 1;
		}
		//LINFO << "Time: " << p.polTime;
		//std::ostringstream s;
		//s << "Pol time: " << p.polTime;
		//Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 1000, true);
		//LINFO << "Last pol: " << v.polTime;

		float correctBy = 1.0f;//simpleMath::lerp(0.25f, 1.0f, simpleMath::UnlerpClamped(100, p.polTime, 400));
		//LINFO << "Correct: " << correctBy;

		p.posError.x *= correctBy;
		p.posError.y *= correctBy;
		p.posError.z *= correctBy;

		//LINFO << p.posError.x << ", " << p.posError.y << ", "<< p.posError.z;

		p.lastPolProgress = 0.0f;

		p.lastPol = cTime;
		p.hasPol = true;

		//LINFO << "Player " << playerid << " updated: X: " << pi.goX << ", Y: " << pi.goY << ", Z: " << pi.goZ;
		//LINFO << "Player " << playerid << " updated pack: " << rcvPacket;
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseOnFootSync): " << e.what();
	}
}

void parseWeaponChanged(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		if(players::isPlayer(playerid))
		{
			players::netPlayer& p = players::getPlayer(playerid);
			int bufWeapon = -1;
			bsIn.Read(bufWeapon);
			if(bufWeapon != p.currentWeapon)
			{
				p.weaponChanged = true;
				LINFO << "!pGun:" << playerid << "," << p.currentWeapon << "," << bufWeapon;
				p.currentWeapon = bufWeapon;
				p.currentAmmo = 0;
			}
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseWeaponChanged): " << e.what();
	}
}

void parseDuckChanged(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		if(players::isPlayer(playerid))
		{
			players::netPlayer& p = players::getPlayer(playerid);
			int state = 0;
			bsIn.Read(state);
			p.isDucking = state == 1 ? true : false;

		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseDuckChanged): " << e.what();
	}
}

void parseSkinChanged(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		if(players::isPlayer(playerid))
		{
			players::netPlayer& p = players::getPlayer(playerid);
			p.currentWeapon = 0;
			bsIn.Read(p.skin);
			p.toBeDeleted = 2;

			LINFO << "Player " << playerid << " new skin: " << p.skin;

			for(size_t i = 0; i < 9; i++)
			{
				p.clothes[i] = 0;
			}

			for(size_t i = 0; i < 2; i++)
			{
				p.props[i] = -1;
			}
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseSkinChanged): " << e.what();
	}
}

bool hasTped = false;
void gotoNewPos(RakNet::BitStream& bsIn)
{
	try
	{
		players::netPlayer& p = players::getLocalPlayer();
		bsIn.Read(p.pos.x);
		bsIn.Read(p.pos.y);
		bsIn.Read(p.pos.z);

		if(p.inVehicle == 0 && p.passangerOf == 0)
		{
			Scripting::SetCharCoordinates(p.ped, p.pos.x, p.pos.y, p.pos.z);
		}
		else
		{
			//NativeInvoke::Invoke<u32>("WARP_CHAR_FROM_CAR_TO_COORD", p.ped, p.pos.x, p.pos.y, p.pos.z);
			Scripting::WarpCharFromCarToCoord(p.ped, p.pos.x, p.pos.y, p.pos.z);
		}
		gameMemory::closeDoors(gameMemory::getDoorStatus());

		hasTped = true;
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseNewPos): " << e.what();
	}
}

bool recentlyTped(){return hasTped;}
void interiorStreamed(){hasTped = false;}

void parseColorChanged(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		unsigned int hexColor = 0xFFFFFFFF;
		bsIn.Read(hexColor);

		playerList::coloredUpdated(playerid, hexColor);

		if(players::isPlayer(playerid))
		{
			//LINFO << "Player " << playerid << " color changed to " << hexColor;

			players::netPlayer& pn = players::getPlayer(playerid);
			pn.color[0] = (hexColor >> 24) & 0xFF;
			pn.color[1] = (hexColor >> 16) & 0xFF;
			pn.color[2] = (hexColor >> 8) & 0xFF;
			pn.color[3] = hexColor & 0xFF;

			if(pn.spawned && pn.blip.IsValid())
			{
				//NativeInvoke::Invoke<u32>("CHANGE_BLIP_COLOUR", pn.blip, hexColor);
				Scripting::ChangeBlipColour(pn.blip, hexColor);
			}
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseColorChanged): " << e.what();
	}
}

void parseEnteringVehicle(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		if(!players::isPlayer(playerid) || !players::getPlayer(playerid).spawned)
		{
			return;
		}

		players::netPlayer& p = players::getPlayer(playerid);

		int vehicle;
		bsIn.Read(vehicle);
		if(vehicle == -2)
		{
			LINFO << "Forcing player " << playerid << " out of vehicle";
			p.noLongerDriving(true);
			Scripting::TaskLeaveAnyCar(p.ped);
			/*if(p.inVehicle != 0 && vehicles::isVehicle(p.inVehicle) && vehicles::getVehicle(p.inVehicle).spawned)
			{
				Scripting::SetCarEngineOn(vehicles::getVehicle(p.inVehicle).vehicle, 0, 0);
			}*/
			p.vehicleEntryRequestTime = clock() + 2000;
			p.currentAnim = 10004;
			p.enteringVehicle[0] = 0, p.enteringVehicle[1] = 0;
			return;
		}
		else if(vehicle == -1 || !vehicles::isVehicle(vehicle) || !vehicles::getVehicle(vehicle).spawned)
		{
			LINFO << players::getPlayer(playerid).playerName << ": cancelling entry";
			p.currentAnim = 0;
			p.vehicleEntryRequestTime = 0;
			p.noLongerDriving(true);
			Scripting::ClearCharTasksImmediately(p.ped);
			p.enteringVehicle[0] = 0, p.enteringVehicle[1] = 0;
			return;
		}

		int seat;
		bsIn.Read(seat);

		LINFO << "Entry";
		LINFO << playerid;
		LINFO << vehicle;
		LINFO << seat;

		p.enteringVehicle[0] = vehicle;
		p.enteringVehicle[1] = seat;
		if(seat == 0)
		{
			LINFO << "Entering car";
			//NativeInvoke::Invoke<u32>("TASK_ENTER_CAR_AS_DRIVER", p.ped, vehicles::getVehicle(vehicle).vehicle, -1);
			Scripting::TaskEnterCarAsDriver(p.ped, vehicles::getVehicle(vehicle).vehicle, -1);
			p.currentAnim = 10003;
			p.vehicleEntryRequestTime = clock() + 5000;
		}
		else if(seat > 0)
		{
			LINFO << "Entering car (passenger)";
			//NativeInvoke::Invoke<u32>("TASK_ENTER_CAR_AS_PASSENGER", p.ped, vehicles::getVehicle(vehicle).vehicle, 
				//-1, seat - 1);
			Scripting::TaskEnterCarAsPassenger(p.ped, vehicles::getVehicle(vehicle).vehicle, -1, seat - 1);
			p.currentAnim = 10003;
			p.vehicleEntryRequestTime = clock() + 5000;
		}
		else if(seat < 0)
		{
			Scripting::ClearCharTasksImmediately(p.ped);
			p.currentAnim = 0;
			p.vehicleEntryRequestTime = 0;
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseEnteringVehicle): " << e.what();
	}
}

void parseDebugPlayer(RakNet::BitStream& bsIn)
{
	int playerid;
	bsIn.Read(playerid);
	if(!players::isPlayer(playerid))
	{
		return;
	}

	players::netPlayer& p = players::getPlayer(playerid);
	std::ostringstream s;
	RakNet::RakString rs;
	RakNet::BitStream b;

	s << "Player " << playerid << " vehicle(" << p.inVehicle << ", " << p.passangerOf << ", " 
		<< p.passangerSeat << "), anim: " << p.currentAnim << ", updating: " << p.hasPol;
	rs = s.str().c_str();

	b.Write(rs);
	b.Write(0xFFFF0000);
	chatWindow::receivedChat(b);
}

void parseClothesChanged(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		if(players::isPlayer(playerid))
		{
			int variationId = 0;
			bsIn.Read(variationId);
			int variationValue = 0;
			bsIn.Read(variationValue);

			if(variationId < 0 || variationId > 10 || variationValue < 0 || variationValue > 200)
			{
				LINFO << "!Error invalid data for player " << playerid << " variation (" << variationId << ", " << variationValue << ")";
				return;
			}

			LINFO << "Player " << playerid << " variation " << variationId << " changed to " << variationValue;

			players::netPlayer& pn = players::getPlayer(playerid);
			pn.clothes[variationId] = variationValue;
			if(pn.spawned)
			{
				players::setClothes(pn, variationId);
			}
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseClothesChanged): " << e.what();
	}
}

void parseHpChanged(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		if(players::isPlayer(playerid))
		{
			LINFO << "Player " << playerid << " hp changed";
			players::getPlayer(playerid).lastHpChange = networkManager::getConnection()->cTime;
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseHpChanged): " << e.what();
	}
}

void parseAnimChanged(RakNet::BitStream& bsIn)
{
	try
	{
		players::netPlayer& lp = players::getLocalPlayer();

		RakNet::RakString rgroup;
		bsIn.Read(rgroup);

		if(strcmp(rgroup, "n") == 0)
		{
			LINFO << "Server ordered anim clean up";
			setServerAnim(999999);
			if(lp.spawned)
			{
				Scripting::ClearCharTasksImmediately(lp.ped);
				//NativeInvoke::Invoke<u32>("BLOCK_PED_WEAPON_SWITCHING", lp.ped, false);
				Scripting::BlockPedWeaponSwitching(lp.ped, false);
			}
			return;
		}

		if(lp.inVehicle != 0 || lp.passangerOf != 0)
		{
			return;
		}
		LINFO << "Debug: player anim changed";

		RakNet::RakString rname;
		bsIn.Read(rname);

		int repeat = 0;
		bsIn.Read(repeat);
			
		char* bufGroup = _strdup(rgroup);
		char* bufName = _strdup(rname);

		unsigned int anim = getAnimIdByName(bufGroup, bufName);
		LINFO << "Server ordered anim: " << bufGroup << ", " << bufName << ". Result: " << anim;

		if(anim != 999999)
		{
			setServerAnim(anim);
			setServerAnimRepeat(repeat == 1);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseAnimChanged): " << e.what();
	}
}

void parseAnimLooping(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		if(players::isPlayer(playerid))
		{
			LINFO << "Player " << playerid << " anim is looping";

			int looping = 0;
			bsIn.Read(looping);
			players::getPlayer(playerid).animRepeat = looping == 1;
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseAnimLooping): " << e.what();
	}
}

void parsePropChange(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		if(players::isPlayer(playerid))
		{
			int variationId = 0;
			bsIn.Read(variationId);
			int variationValue = 0;
			bsIn.Read(variationValue);

			if(variationId < 0 || variationId > 1 || variationValue < -1 || variationValue > 200)
			{
				LINFO << "!Error invalid data for player " << playerid << " property (" << variationId << ", " << variationValue << ")";
				return;
			}

			LINFO << "Player " << playerid << " property " << variationId << " changed to " << variationValue - 1;

			players::netPlayer& pn = players::getPlayer(playerid);
			pn.props[variationId] = variationValue - 1;
			if(pn.spawned)
			{
				if(variationValue > 0)
				{
					//NativeInvoke::Invoke<u32>("SET_CHAR_PROP_INDEX", pn.ped, variationId, pn.props[variationId]);
					Scripting::SetCharPropIndex(pn.ped, variationId, pn.props[variationId]);
				}
				else
				{
					//NativeInvoke::Invoke<u32>("CLEAR_CHAR_PROP", pn.ped, variationId);
					Scripting::ClearCharProp(pn.ped, variationId);
				}
			}
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parsePropChange): " << e.what();
	}
}

void parseNameChanged(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		RakNet::RakString str;
		bsIn.Read(str);

		playerList::nameUpdated(playerid, str);

		if(players::isPlayer(playerid))
		{
			//LINFO << "Player " << playerid << " color changed to " << hexColor;

			players::netPlayer& pn = players::getPlayer(playerid);
			sprintf_s(pn.playerName, 50, "(%i)_%s", playerid, str.C_String());
			/*std::ostringstream os;
			os << "(" << playerid << ")_" << str;
			pn.playerName = os.str();*/
			if(pn.spawned)
			{
				Scripting::ChangeBlipNameFromAscii(pn.blip, pn.playerName);
			}
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseColorChanged): " << e.what();
	}
}

void parseAfkChanged(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid = 0;
		bsIn.Read(playerid);

		int status = 0;
		bsIn.Read(status);

		if(!players::isPlayer(playerid))
		{
			return;
		}

		players::getPlayer(playerid).isAfk = status == 1;
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseAfkChanged): " << e.what();
	}
}

void parseDrawFlags(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid = 0;
		bsIn.Read(playerid);

		if(!players::isPlayer(playerid))
		{
			return;
		}
		players::netPlayer& p = players::getPlayer(playerid);
		bool blipIt;
		bsIn.Read(blipIt);
		bsIn.Read(p.shouldDrawName);

		if(p.spawned && blipIt != p.shouldAddBlip)
		{
			if(!blipIt) {
				Scripting::RemoveBlip(p.blip);
			}
			else {
				Scripting::AddBlipForChar(p.ped, &p.blip);
				Scripting::ChangeBlipNameFromAscii(p.blip, p.playerName);

				unsigned int colorHex = (p.color[0] << 24) | (p.color[1] << 16) | (p.color[2] << 8) | p.color[3];
				Scripting::ChangeBlipColour(p.blip, colorHex);
			}
		}
		p.shouldAddBlip = blipIt;

	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseDrawFlags): " << e.what();
	}
}