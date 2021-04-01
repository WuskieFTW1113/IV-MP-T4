#ifndef playerSkins_H_
#define playerSkins_H_

#include <string>
#include <map>

namespace playerSkins
{
	struct skin
	{
		unsigned long hash;
		char* animGroup;
		std::string name;

		skin(unsigned long r_hash, std::string r_animGroup, std::string r_name);
	};

	struct anim
	{
		int id;
		char* animName;
		char* animGroup;
	};

	void initAllSkins();

	bool isValidSkin(size_t skin);

	skin& getSkin(size_t skin);
}

#endif;