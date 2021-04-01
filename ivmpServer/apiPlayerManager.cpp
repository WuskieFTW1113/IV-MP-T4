#include "apiPlayerManager.h"
#include <map>

std::map<int, apiPlayer::player> apiPlayerMap;

apiPlayer::player& apiPlayer::get(int id)
{
	return apiPlayerMap.at(id);
}

apiPlayer::player* apiPlayer::getPlayer(int id)
{
	return apiPlayerMap.find(id) != apiPlayerMap.end() ? &apiPlayerMap.at(id) : NULL;
}

bool apiPlayer::isOn(int id)
{
	return apiPlayerMap.find(id) != apiPlayerMap.end();
}

void apiPlayer::getPlayers(std::vector<player*>& vec)
{
	std::map<int, apiPlayer::player>::iterator i = apiPlayerMap.begin(), e = apiPlayerMap.end();
	for(i; i != e; i++)
	{
		vec.push_back(&i->second);
	}
}

bool apiPlayerManager::add(int id, apiPlayer::player& p)
{
	return apiPlayerMap.insert(std::make_pair(id, p)).second;
}

void apiPlayerManager::remove(int id)
{
	apiPlayerMap.erase(id);
}

int apiPlayerManager::getFreeId()
{
	int id = 1;
	for(std::map<int, apiPlayer::player>::const_iterator i = apiPlayerMap.begin(); i != apiPlayerMap.end(); i++)
	{
		if(i->first != id)
		{
			break;
		}
		id++;
	}
	return id;
}