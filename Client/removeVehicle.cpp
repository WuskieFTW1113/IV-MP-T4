#include "removeVehicle.h"
#include "vehicles.h"
#include "players.h"
#include "easylogging++.h"

void emptyVehicleOccupants(int vehicleId, bool changeStat)
{
	vehicles::netVehicle& v = vehicles::getVehicle(vehicleId);
	if(v.driver > 0 && players::isPlayer(v.driver))
	{
		players::netPlayer& p = players::getPlayer(v.driver);
		if(changeStat)
		{
			p.inVehicle = 0;
		}
		if(p.spawned)
		{
			//NativeInvoke::Invoke<u32>("WARP_CHAR_FROM_CAR_TO_COORD", p.ped, p.pos.x, p.pos.y, p.pos.z);
			Scripting::WarpCharFromCarToCoord(p.ped, p.pos.x, p.pos.y, p.pos.z);
		}
	}

	for(std::map<int, players::netPlayer>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		if(i->second.passangerOf == vehicleId)
		{
			if(changeStat)
			{
				i->second.passangerOf = 0;
			}
			if(i->first > 0 && i->second.spawned)
			{
				//NativeInvoke::Invoke<u32>("WARP_CHAR_FROM_CAR_TO_COORD", i->second.ped, i->second.pos.x, i->second.pos.y, i->second.pos.z);
				Scripting::WarpCharFromCarToCoord(i->second.ped, i->second.pos.x, i->second.pos.y, i->second.pos.z);
			}
		}
	}
}
void removeVehicle(RakNet::BitStream& bsIn)
{
	try
	{
		int veh;
		bsIn.Read(veh);

		//LINFO << "Must delete vehicle " << veh;
				
		if(vehicles::isVehicle(veh))
		{
			vehicles::netVehicle& v = vehicles::getVehicle(veh);
			v.toBeDeleted = 1;
			emptyVehicleOccupants(veh, true);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error: removeVehicle: " << e.what();
	}
}

void removeVehicleOnly(RakNet::BitStream& bsIn)
{
	try
	{
		int veh;
		bsIn.Read(veh);

				
		if(vehicles::isVehicle(veh))
		{
			vehicles::netVehicle& v = vehicles::getVehicle(veh);
			v.toBeDeleted = 1;
			emptyVehicleOccupants(veh, true);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error: removeVehicleOnly: " << e.what();
	}
}