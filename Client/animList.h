#pragma once
#ifndef animList_H_
#define animList_H_

#include <string>
#include <map>
#include <vector>

namespace animList
{
	struct anims
	{
		char* animName;
		char* animGroup;

		anims(std::string r_animGroup, std::string r_animName);
	};

	struct animGroup
	{
		char* groupName;
		size_t beginsAtId;
		size_t endsAtId;
	};

	bool isValidAnim(size_t anim);
	anims& getAnimType(size_t listId);

	size_t getGroupsSize();
	animGroup& getAnimGroup(size_t id);
	size_t getGroupByName(char* name);

	void initAllAnims();
}

#endif