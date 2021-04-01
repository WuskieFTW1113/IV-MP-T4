#include "vehiclesList.h"
#include <fstream>
#include <map>
#include "easylogging++.h"
#include "../SharedDefines/paramHelper.h"

struct vehicleModel
{
	unsigned long model;
	std::string name;
};

std::map<int, vehicleModel> vehicleList;

void vehiclesList::loadList()
{
	std::ifstream infile;
	infile.open("comMP/configThree.dat");
	if(!infile.is_open())
	{
		LINFO << "Error configThree did not open";
		return;
	}

	std::string line;
	std::vector<std::string> params;
	int count = 0;

	while(!infile.eof())
	{
		params.clear();
		getline(infile, line);
		paramHelper::getParams(params, line, " ");
		count++;

		if(params.size() != 2)
		{
			LINFO << "ConfigThree line " << count << " did not load (param count)";
			continue;
		}

		unsigned long skinHash = paramHelper::isUnsignedInt(params.at(1), true);
		if(skinHash == 0)
		{
			LINFO << "ConfigThree line " << count << " did not load (invalid type)";
			continue;
		}

		vehicleModel m;
		m.model = skinHash;
		m.name = params.at(0);

		vehicleList.insert(std::make_pair(count, m));
	}
	infile.close();

	/*for(std::map<int, vehicleModel>::iterator i = vehicleList.begin(); i != vehicleList.end(); ++i)
	{
		LINFO << i->first << ", " << i->second.name << ", " << i->second.model;
	}*/
}

bool vehiclesList::isValidModel(int i)
{
	return !(vehicleList.find(i) == vehicleList.end());
}

int vehiclesList::getModel(int i)
{
	return vehicleList.at(i).model;
}

unsigned int heliModels[7] = {
	0x9D0450CA, 0x1517D4D9, 0x78D70477, 0x31F0B376, 788747387, 0xEBC24DF2, 0x3E48BF23
};

bool vehiclesList::isModelAHeli(unsigned int vModel)
{
	for(int i = 0; i < 7; i++)
	{
		if(heliModels[i] == vModel)
		{
			return true;
		}
	}
	return false;
}