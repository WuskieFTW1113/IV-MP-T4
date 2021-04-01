#include "npcChar.h"

std::map<int, npcChars::npcChar*> npcs;

void npcChars::addNpc(int id, npcChars::npcChar* npc)
{
	npcs.insert(std::make_pair(id, npc));
}

bool npcChars::isNpc(int id)
{
	return !(npcs.find(id) == npcs.end());
}

void npcChars::removeNpc(int id)
{
	npcs.erase(id);
}

npcChars::npcChar* npcChars::getNpc(int id)
{
	return npcs.at(id);
}

int npcChars::getEmptyNpc()
{
	int id = 1;
	for(std::map<int, npcChars::npcChar*>::const_iterator i = npcs.begin(); i != npcs.end(); ++i)
	{
		if(i->first == id)
		{
			id++;
		}
		break;
	}
	return id;
}

std::map<int, npcChars::npcChar*>::iterator npcChars::getNpcBegin()
{
	return npcs.begin();
}

std::map<int, npcChars::npcChar*>::iterator npcChars::getNpcEnd()
{
	return npcs.end();
}