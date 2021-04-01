#include "playerSkins.h"
#include <vector>
#include <fstream>
#include "easylogging++.h"
#include "../SharedDefines/paramHelper.h"

std::map<size_t, playerSkins::skin> skins;

std::vector<playerSkins::anim> anims;

playerSkins::skin::skin(unsigned long r_hash, std::string r_animGroup, std::string r_name)
{
	hash = r_hash;

	animGroup = new char[r_animGroup.size() + 1];
	std::copy(r_animGroup.begin(), r_animGroup.end(), animGroup);
	animGroup[r_animGroup.size()] = '\0';

	name = r_name;
	LINFO << hash << ", " << animGroup << ", " << name;
}

void playerSkins::initAllSkins()
{
	std::ifstream infile;
	infile.open("comMP/configOne.dat"); //skins
	if(!infile.is_open())
	{
		LINFO << "Error configOne did not open";
		return;
	}

	std::string line;
	std::vector<std::string> params;
	int count = 1;

	while(!infile.eof())
	{
		params.clear();
		getline(infile, line);
		paramHelper::getParams(params, line, " ");

		if(params.size() != 4)
		{
			LINFO << "ConfigOne line " << count << " did not load (param count)";
			continue;
		}

		int skinId = paramHelper::isInt(params.at(0));
		if(skinId == -1)
		{
			LINFO << "ConfigOne line " << count << " did not load (invalid type)";
			continue;
		}

		int skinHash = paramHelper::isUnsignedInt(params.at(1), false);
		if(skinHash == 0)
		{
			LINFO << "ConfigOne line " << count << " did not load (invalid type::1)";
			continue;
		}

		skins.insert(std::make_pair(skinId, skin(skinHash, params.at(2), params.at(3))));
	}
	infile.close();
}

bool playerSkins::isValidSkin(size_t skin)
{
	return !(skins.find(skin) == skins.end());
}

playerSkins::skin& playerSkins::getSkin(size_t skin)
{
	return skins.at(skin);
}
