#include "animList.h"
#include <vector>
#include <fstream>
#include "easylogging++.h"

//13 aiming
//14 aiming walking
//15 shooting
//16 shooting walking

animList::anims::anims(std::string r_animGroup, std::string r_animName)
{
	animName = new char[r_animName.size() + 1];
	std::copy(r_animName.begin(), r_animName.end(), animName);
	animName[r_animName.size()] = '\0';

	animGroup = new char[r_animGroup.size() + 1];
	std::copy(r_animGroup.begin(), r_animGroup.end(), animGroup);
	animGroup[r_animGroup.size()] = '\0';
}

std::map<size_t, animList::anims> animsVec;
std::vector<animList::animGroup> fastBrowser;

void animList::initAllAnims()
{
	std::ifstream infile;
	infile.open("comMP/config/anims.dat"); //anims
	if(!infile.is_open())
	{
		LINFO << "Error configTwo did not open";
		return;
	}

	std::string line;

	bool closed = true;
	bool nowAnims = false;

	std::string cAnimGroup;
	bool groupChanged = false;
	size_t groupId = 1;

	size_t animPush = 10;

	while(!infile.eof())
	{
		getline(infile, line);

		if(closed && line != std::string("{") && line != std::string("}"))
		{
			cAnimGroup = line;
			closed = false;
			groupChanged = true;
			//LINFO << "Anim group (" << cAnimGroup << ") selected";
		}
		else if(line == std::string("{"))
		{
			nowAnims = true;
		}
		else if(line == std::string("}"))
		{
			closed = true;
			nowAnims = false;

			fastBrowser.at(groupId).endsAtId = animPush - 1;
		}
		else if(nowAnims)
		{
			anims a(cAnimGroup, line);
			animsVec.insert(std::make_pair(animPush, a));
			
			if(groupChanged)
			{
				groupChanged = false;

				animList::animGroup g;
				g.beginsAtId = animPush;
				g.groupName = new char[cAnimGroup.size() + 1];
				std::copy(cAnimGroup.begin(), cAnimGroup.end(), g.groupName);
				g.groupName[cAnimGroup.size()] = '\0';

				fastBrowser.push_back(g);
				groupId = fastBrowser.size() - 1;
			}
			animPush++;
		}
	}
	infile.close();
	LINFO << "Loaded " << animPush - 1 << " anims in " << groupId - 1 << " groups";
}

bool animList::isValidAnim(size_t anim)
{
	return !(animsVec.find(anim) == animsVec.end());
}

animList::anims& animList::getAnimType(size_t listId)
{
	return animsVec.at(listId);
}

size_t animList::getGroupsSize()
{
	return fastBrowser.size();
}

animList::animGroup& animList::getAnimGroup(size_t id)
{
	return fastBrowser.at(id);
}

size_t animList::getGroupByName(char* name)
{
	size_t s = fastBrowser.size();
	for(size_t i = 0; i < s; ++i)
	{
		if(std::strcmp(name, fastBrowser.at(i).groupName) == 0)
		{
			return i;
		}
	}
	return 999999;
}