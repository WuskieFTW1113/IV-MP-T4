#include "gameWorld.h"
#include "Scripting.h"
#include "players.h"
#include "vehicles.h"
#include "gameObjects.h"
#include "gameCheckPoints.h"
#include "gameMemory.h"
#include "gameBlips.h"
#include <time.h>
#include "easylogging++.h"
#include "../clientLibs/execution.h"
#include "Offsets.h"

unsigned int currentWorld = 1;
float spawnPos[3];
bool game_clientFrozen = false;

int hour = 0;
float minutes = 0;
int duration = 0;
long lastTimePulse = 0;

int weather = 0;

unsigned int gameWorld::getCurrentWorld()
{
	return currentWorld;
}

void setGameTime(int hour, int min)
{
	DWORD setTimeFunc = gameMemory::getModuleHandle() + offsets::TIMEFUNC;
	DWORD timeOFDay = gameMemory::getModuleHandle() + offsets::TIMEOFDAY;
	_asm
	{
		push -1;
		push 0;
		push min;
		push hour;
		call setTimeFunc;
		add esp, 10h;
	}
    *(DWORD *)(*(DWORD *)(timeOFDay) + 0x260) = 2;
}

void gameWorld::receivedTime(RakNet::BitStream& bsIn)
{
	int c_minutes;
	bsIn.Read(weather);
	bsIn.Read(hour);
	bsIn.Read(c_minutes);
	bsIn.Read(duration);

	Scripting::ForceWeatherNow(weather);
	minutes = (float)c_minutes;
	setGameTime(hour, c_minutes);
	lastTimePulse = clock();

	LINFO << "New time: " << hour << ":" << c_minutes << ", " << duration;
}

void gameWorld::resetWeather()
{
	Scripting::ForceWeatherNow(weather);
}

void gameWorld::pulseCurrentWorld(long cTime)
{
	if(duration == 0) return setGameTime(hour, (int)minutes);
	minutes += (float)(cTime - lastTimePulse) / duration;
	lastTimePulse = cTime;
	if(minutes > 60.0f)
	{
		minutes = 0;
		hour++;
		if(hour > 23)
		{
			hour = 0;
		}
	}
	//Scripting::ForceTimeOfDay(hour, (u32)minutes);
	setGameTime(hour, (int)minutes);
}

void gameWorld::changedWorld(RakNet::BitStream& bsIn)
{
	int newWorld = 1;
	bsIn.Read(newWorld);
	LINFO << "World change: " << newWorld;
	if(currentWorld == newWorld)
	{
		LINFO << "World change was requested but the new number is still the same";
		return;
	}
	currentWorld = newWorld;

	for(std::map<int, players::netPlayer>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		if(i->first != -2)
		{
			i->second.toBeDeleted = 1;
			//LINFO << "World change player: " << i->first;
		}
	}

	for(std::map<size_t, vehicles::netVehicle>::iterator i = vehicles::getVehiclesBegin(); i != vehicles::getVehiclesEnd(); ++i)
	{
		i->second.toBeDeleted = 1;
	}

	gameObjects::wipeObjects();
	gameCheckPoints::wipeCps();
	gameBlips::wipeBlips();
}

void gameWorld::setLocalSpawnCoords(float x, float y, float z)
{
	spawnPos[0] = x;
	spawnPos[1] = y;
	spawnPos[2] = z;
}

void gameWorld::getLocalSpawnCoords(float &x, float &y, float &z)
{
	x = spawnPos[0];
	y = spawnPos[1];
	z = spawnPos[2];
}

void gameWorld::setPlayerFrozen(bool b)
{
	game_clientFrozen = b;
}

bool gameWorld::isPlayerFrozen()
{
	return game_clientFrozen;
}