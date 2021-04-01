#include "players.h"
#include "CustomFiberThread.h"
#include <sstream>
#include "simpleMath.h"
#include <time.h>
#include "animList.h"
#include "padControl.h"
#include "easylogging++.h"
#include "vehicles.h"

std::map<int, players::netPlayer> nPlayers;

void players::netPlayer::noLongerDriving(bool resetPassenger)
{
	if(this->inVehicle != 0)
	{
		this->inVehicle = 0;
		if(vehicles::isVehicle(this->inVehicle))
		{
			vehicles::netVehicle& v = vehicles::getVehicle(this->inVehicle);
			v.driver = 0;
			v.fullySpawnedWithDriver = false;
		}
	}
	if(resetPassenger)
	{
		this->passangerOf = 0;
		this->passangerSeat = 0;
	}
}

bool players::isPlayer(int playerid)
{
	if(nPlayers.find(playerid) == nPlayers.end())
	{
		return false;
	}
	return true;
}

players::netPlayer& players::getLocalPlayer()
{
	return nPlayers.at(-2);
}

players::netPlayer& players::getPlayer(int playerid)
{
	return nPlayers.at(playerid);
}

void players::insertPlayer(int playerid, netPlayer &p)
{
	initPlayerPadHook(playerid);
	nPlayers.insert(std::make_pair(playerid, p));
}

void players::removePlayer(int playerid)
{
	nPlayers.erase(playerid);
	removePlayerPadHook(playerid);
}

void players::removePlayerUsingIterator(std::map<int, players::netPlayer>::iterator& it)
{
	removePlayerPadHook(it->first);

	if(it->second.inVehicle != 0 && vehicles::isVehicle(it->second.inVehicle))
	{
		vehicles::getVehicle(it->second.inVehicle).driver = 0;
	}

	nPlayers.erase(it);
}

std::map<int, players::netPlayer>::iterator players::getPlayersBegin()
{
	return nPlayers.begin();
}

std::map<int, players::netPlayer>::iterator players::getPlayersEnd()
{
	return nPlayers.end();
}

size_t players::streamedPlayers()
{
	return nPlayers.size();
}

size_t players::spawnedPlayers()
{
	size_t in = 0;
	for(std::map<int, players::netPlayer>::iterator i = nPlayers.begin(); i != nPlayers.end(); ++i)
	{
		if(i->second.spawned)
		{
			in++;
		}
	}
	return in;
}