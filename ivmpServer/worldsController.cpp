#include "worldsController.h"
#include "apiWorld.h"
#include <map>

std::map<unsigned int, apiWorld::virtualWorld*> vWorlds;
long lastUpdate = 0;

bool apiWorld::isWorld(unsigned int id)
{
	return vWorlds.find(id) != vWorlds.end();
}

void apiWorld::createWorld(unsigned int id, unsigned int weather, unsigned int hour, unsigned int minutes, unsigned int minuteDuration)
{
	apiWorld::virtualWorld* v = new apiWorld::virtualWorld(id, weather, hour, minutes, minuteDuration);
	vWorlds.insert(std::make_pair(id, v));
}

void apiWorld::deleteWorld(unsigned int id)
{
	delete vWorlds.at(id);
	vWorlds.erase(id);
}

apiWorld::virtualWorld* apiWorld::getWorld(unsigned int id)
{
	return vWorlds.at(id);
}

void worldsController::updateWorlds(long cTime)
{
	long increase = cTime - lastUpdate;
	lastUpdate = cTime;
	for(std::map<unsigned int, apiWorld::virtualWorld*>::iterator i = vWorlds.begin(); i != vWorlds.end(); ++i)
	{
		if(i->second->getMinuteDuration() == 0) continue;
		i->second->updateWorld(increase);
	}
}