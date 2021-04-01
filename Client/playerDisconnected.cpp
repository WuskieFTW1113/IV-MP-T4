#include "playerDisconnected.h"
#include "players.h"
#include "vehicles.h"
#include "easylogging++.h"

void playerDisconnected(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);
				
		if(players::isPlayer(playerid))
		{
			players::netPlayer&p = players::getPlayer(playerid);
			p.toBeDeleted = 1;

			if(p.inVehicle != 0 && vehicles::isVehicle(p.inVehicle))
			{
				vehicles::getVehicle(p.inVehicle).driver = 0;
			}
			LINFO << "Must delete player " << playerid;
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error: playerDisconnected: " << e.what();
	}
}