#include "API/apiWorld.h"
#include "players.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "../SharedDefines/easylogging++.h"

apiWorld::virtualWorld::virtualWorld(unsigned int id, unsigned int weather, unsigned int hour, unsigned int minutes, unsigned int minuteDuration)
{
	this->id = id;
	this->weather = weather;
	this->hour = hour;
	this->minutes = (float)minutes;
	this->minuteDuration = minuteDuration;
}

apiWorld::virtualWorld::~virtualWorld()
{
}

void apiWorld::virtualWorld::updateWorld(long cTime)
{
	this->minutes += (float)cTime / minuteDuration;
	if(minutes > 60.0f)
	{
		minutes = 0.0f;
		hour++;
		if(hour > 23)
		{
			hour = 0;
		}
		//LINFO << "World " << this->id << " hours changed to " << hour;
	}
}

void apiWorld::virtualWorld::streamWorldChanges()
{
	networkManager::connection* con = networkManager::getConnection();
	
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(UPDATE_WORLD_TIME_WEATHER);
	bsOut.Write(weather);
	bsOut.Write(hour);
	bsOut.Write((unsigned int)minutes);
	bsOut.Write(minuteDuration);

	for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		if(i->second.vWorld == this->id)
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
		}
	}
}

void apiWorld::virtualWorld::setWeather(unsigned int weather)
{
	this->weather = weather;
}

void apiWorld::virtualWorld::setTime(unsigned int hour, unsigned int minutes)
{
	this->hour = hour;
	this->minutes = (float)minutes;
}

void apiWorld::virtualWorld::setMinuteDuration(unsigned int duration)
{
	this->minuteDuration = duration;
}

void apiWorld::virtualWorld::getTime(unsigned int& hour, unsigned int& minutes)
{
	hour = this->hour;
	minutes = (unsigned int)this->minutes;
}

unsigned int apiWorld::virtualWorld::getWeather()
{
	return weather;
}

unsigned int apiWorld::virtualWorld::getMinuteDuration()
{
	return minuteDuration;
}
