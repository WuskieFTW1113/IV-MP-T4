#include "receiveFootSync.h"
#include "players.h"
#include "vehicles.h"
#include "easylogging++.h"
#include "../SharedDefines/packetsIds.h"
////#include "../Server/eventManager.h"
#include "apiPlayerEvents.h"

apiPlayerEvents::vEnterRequest enteringVehFunc = 0;

struct jackData
{
	long time;
	int vehId;

	jackData(long cTime, int vid)
	{
		time = cTime;
		vehId = vid;
	}
	~jackData(){}
};

std::map<int, jackData> carJacks;

bool hasCarJack(int playerid)
{
	return carJacks.find(playerid) != carJacks.end();
}

void getJackData(int playerid, int& vehicle, long& time)
{
	vehicle = carJacks.at(playerid).vehId;
	time = carJacks.at(playerid).time;
}

void removeJack(int playerid)
{
	carJacks.erase(playerid);
}

void clientEnteringVehicle(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(ENTERING_VEHICLE);
	bsOut.Write(p.id);

	int vehicle;
	bsIn.Read(vehicle);
	if(vehicle == -1)
	{
		bsOut.Write(-1);
		//LINFO << "Player " << p.nick << " entering vehicle cancelled";
		if(hasCarJack(p.id))
		{
			removeJack(p.id);
		}
	}
	else if(vehicle == -2)
	{
		bsOut.Write(-2);
		p.movementAnim = 10004;
		players::updateTimeOut(p, con->cTime + 4000);
		//LINFO << "Player " << p.nick << " leaving vehicle";
		if(hasCarJack(p.id))
		{
			removeJack(p.id);
		}
		if(p.currentVehicle != 0 && vehicles::isVehicle(p.currentVehicle))
		{
			vehicles::netVehicle& pv = vehicles::getVehicle(p.currentVehicle);
			//if(pv.engine == 2) pv.engine = 1;
		}
	}
	else if(vehicles::isVehicle(vehicle))
	{
		int seat;
		bsIn.Read(seat);

		bsOut.Write(vehicle);
		bsOut.Write(seat);

		bool carJack = false;
		if(seat == 0)
		{
			int jack = 0;
			bsIn.Read(jack);
			carJack = jack == 1;

			if(carJack)
			{
				if(hasCarJack(p.id))
				{
					carJacks.erase(p.id);
				}
				carJacks.insert(std::make_pair(p.id, jackData(con->cTime, vehicle)));
				if(apiPlayer::isOn(p.id))
				{
					apiPlayer::get(p.id).drawInfoText("~r~You are carjacking", 5000);
				}
			}
		}

		p.movementAnim = 10003;
		players::updateTimeOut(p, con->cTime + 12000);

		//LINFO << "Player " << p.nick << " entering vehicle " << vehicle << " seat " << seat;

		if(enteringVehFunc != 0)
		{
			enteringVehFunc(p.id, vehicle, seat, carJack);
		}
	}
	else
	{
		return;
	}
						
	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(p.id != it->second.id && p.vWorld == it->second.camWorld && it->second.sPlayers.find(p.id) != it->second.sPlayers.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}

void apiPlayerEvents::registerEnteringVehicle(apiPlayerEvents::vEnterRequest f)
{
	enteringVehFunc = f;
}