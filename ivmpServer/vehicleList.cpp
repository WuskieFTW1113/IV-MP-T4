#include "API/vehicleList.h"
#include <fstream>
#include <map>
#include "../SharedDefines/easylogging++.h"
#include "API/apiParamHelper.h"
#include "stdParamHelper.h"

struct vehicleModel
{
	unsigned long model;
	std::string name;
	std::vector<int> dTunes;
};

std::map<int, vehicleModel> vList;

void vehicleList::loadList()
{
	std::ifstream infile;
	infile.open("vehicleModels.dat");
	if(!infile.is_open())
	{
		mlog("Error: vehicleModels.dat did not open, this may cause server exceptions");
		return;
	}

	std::string line;
	std::vector<std::string> params;
	size_t pSize = 0;
	int count = 0;

	while(!infile.eof())
	{
		params.clear();
		getline(infile, line);

		autoStrtok(params, line, " ");
		count++;

		pSize = params.size();
		if(pSize < (size_t)2)
		{
			continue;
		}

		unsigned long skinHash = apiParamHelper::isUnsignedInt(params.at(1).c_str(), true);
		if(skinHash == 0)
		{
			continue;
		}

		vehicleModel m;
		m.model = skinHash;
		m.name = params.at(0).erase(0, 6);

		if(pSize == 3)
		{
			size_t ds = params[2].size();
			char buf[2] = "a";
			for(size_t i = 0; i < ds; i++)
			{
				buf[0] = params[2][i];
				//LINFO << m.name << " dTune: " << buf;
				m.dTunes.push_back(apiParamHelper::isUnsignedInt(buf, false));
			}
		}

		//std::cout << count << ": " << m.name << std::endl; 

		vList.insert(std::make_pair(count, m));
	}
	infile.close();
}

bool vehicleList::isValidModel(int i)
{
	return vList.find(i) != vList.end();
}

unsigned long vehicleList::getModel(int i)
{
	return vList.at(i).model;
}

int vehicleList::getIdByName(const char* carName)
{
	std::string str = std::string(carName);

	for(std::map<int, vehicleModel>::iterator i = vList.begin(); i != vList.end(); ++i)
	{
		if(i->second.name == str)
		{
			return i->first;
		}
	}
	return -1;
}

const char* vehicleList::getModelName(int i)
{
	return vList.at(i).name.c_str();
}

int vehicleList::getIdFromModel(unsigned long model)
{
	for(std::map<int, vehicleModel>::iterator i = vList.begin(); i != vList.end(); ++i)
	{
		if(i->second.model == model) return i->first;
	}
	return -1;
}

void vehicleList::getDefaultTune(unsigned int i, size_t& defaultSize, std::vector<int>& tunes)
{
	for(std::map<int, vehicleModel>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		if(it->second.model == i)
		{
			defaultSize = it->second.dTunes.size();
			tunes = it->second.dTunes;
			return;
		}
	}
}
