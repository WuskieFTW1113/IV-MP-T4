#include "modelsHandler.h"
#include "../ClientManager/ivScripting.h"
#include <map>
#include "easylogging++.h"

long lastPulse = 0;

struct modelStruct
{
	long requestTime;
	int requests;

	modelStruct(long requestTime, int requests)
	{
		this->requestTime = requestTime;
		this->requests = requests;
	}
};

std::map<unsigned long, modelStruct> gModels;

void modelsHandler::request(unsigned long model, long cTime)
{
	if(gModels.find(model) == gModels.end())
	{
		//LINFO << "!Models: Requesting: " << model;
		Scripting::RequestModel(model);
		gModels.insert(std::make_pair(model, modelStruct(cTime, 1)));
	}
	else
	{
		//LINFO << "!Models: Requesting a requested model: " << model;
		gModels.at(model).requestTime = cTime;
		gModels.at(model).requests++;
	}
}

bool modelsHandler::wasRequested(unsigned long model)
{
	return gModels.find(model) != gModels.end();
}

void modelsHandler::unload(unsigned long model)
{
	if(wasRequested(model))
	{
		//LINFO << "!Models: model no longer needed: " << model << ", being used count: " << gModels.at(model).requests - 1;
		gModels.at(model).requests--;
	}
}

void modelsHandler::clearUnsued(long cTime)
{
	if(cTime > lastPulse + 1000)
	{
		std::map<unsigned long, modelStruct>::iterator i = gModels.begin();
		while(i != gModels.end())
		{
			if(i->second.requestTime + 15000 < cTime)
			{
				Scripting::MarkModelAsNoLongerNeeded(i->first);
				LINFO << "!Models: model deleted: " << i->first;
				gModels.erase(i++);
				continue;
			}
			++i;
		}
		lastPulse = cTime;
	}
}