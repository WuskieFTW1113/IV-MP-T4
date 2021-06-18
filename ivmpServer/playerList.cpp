#include "API/playerList.h"
#include <string>
#include <map>
#include <vector>
#include "stdParamHelper.h"
#include "API/apiParamHelper.h"
#include <fstream>
#include "../SharedDefines/easylogging++.h"

struct playerModel
{
	std::string name;
	unsigned long model;
};

std::map<int, playerModel> playerModels;

void playerList::loadList()
{
	std::ifstream infile;
	infile.open("playerModels.dat");
	if(!infile.is_open())
	{
		mlog("Error: playerModels.dat did not open, this may cause server exceptions");
		return;
	}

	std::string line;
	std::vector<std::string> params;
	int count = 0;

	while(!infile.eof())
	{
		params.clear();
		getline(infile, line);

		autoStrtok(params, line, " ");

		if(params.size() != 2)
		{
			continue;
		}

		unsigned long skinHash = apiParamHelper::isUnsignedInt(params.at(1).c_str(), true);
		if(skinHash == 0)
		{
			continue;
		}

		playerModel m;
		m.model = skinHash;
		m.name = params.at(0).erase(0, 6);

		//std::cout << count << ": " << m.name << std::endl;
		playerModels.insert(std::make_pair(count, m));
		count++;
	}
	infile.close();
}

bool playerList::isValidModel(int i)
{
	return playerModels.find(i) != playerModels.end();
}

unsigned long playerList::getModel(int i)
{
	return playerModels.at(i).model;
}

int playerList::getIdByName(const char* carName)
{
	std::string str = std::string(carName);

	for(std::map<int, playerModel>::iterator i = playerModels.begin(); i != playerModels.end(); ++i)
	{
		if(i->second.name == str)
		{
			return i->first;
		}
	}
	return -1;
}

const char* playerList::getModelName(int i)
{
	return playerModels.at(i).name.c_str();
}

int playerList::getIdByHash(unsigned int hash)
{
	for(std::map<int, playerModel>::iterator i = playerModels.begin(); i != playerModels.end(); ++i)
	{
		if(i->second.model == hash) return i->first;
	}
	return -1;
}
