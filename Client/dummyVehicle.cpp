#include "dummyVehicles.h"
#include "../ClientManager/ivScripting.h"
#include <map>
#include "easylogging++.h"

std::map<unsigned int, int> dummies;

void dummyVehicles::add(unsigned int id)
{
	if(dummies.find(id) == dummies.end())
	{
		dummies[id] = 0;
		LINFO << "Vehicle " << id << " wasnt created by the mod, queued";
	}
}

void dummyVehicles::pulse()
{
	std::map<unsigned int, int>::iterator i = dummies.begin();
	while(i != dummies.end())
	{
		if(i->second > 9)
		{
			++i;
			continue;
		}
		
		if(Scripting::DoesVehicleIdExist(i->first))
		{
			Scripting::ScriptHandle veh(i->first);
			Scripting::DeleteCarByScriptHandle(&veh);
			LINFO << "Vehicle " << i->first << " was deleted";
			dummies.erase(i++);
			continue;
		}
		i->second++;
		++i;
	}
}