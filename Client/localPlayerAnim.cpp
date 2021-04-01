#include "players.h"
#include "CustomFiberThread.h"
#include <sstream>
#include "simpleMath.h"
#include <time.h>
#include "playerSkins.h"
#include "animList.h"
#include "easylogging++.h"
#include "localPlayerAnim.h"

struct animSection
{
	char* name;
	size_t browserId;
};

std::vector<animSection> onFoot;
std::vector<animSection> onFootArmed;
std::vector<animSection> onFootCrouch;
std::vector<animSection> onFootCrouchArmed;
std::vector<animSection> onWater;
std::vector<animSection> onClimb;
std::vector<animSection> onParachute;

std::vector<animSection> buf;

void addAnimToGroup(char* name, std::vector<animSection>& grp)
{
	animSection s;
	s.name = name;
	s.browserId = animList::getGroupByName(name);

	if(s.browserId != 999999)
	{
		grp.push_back(s);
	}
}

void initAnimLookUpSequences()
{
	addAnimToGroup("move_player", onFoot);
	addAnimToGroup("move_player", onFoot); //dont delete this one
	addAnimToGroup("move_away", onFoot);
	addAnimToGroup("remove_balaclave_b", onFoot);
	addAnimToGroup("remove_balaclave_a", onFoot);
	addAnimToGroup("crchsignal_moveout", onFoot);
	addAnimToGroup("move_junk", onFoot);
	addAnimToGroup("move_combat_strafe", onFoot);
	addAnimToGroup("move_combat_strafe_c", onFoot);
	addAnimToGroup("move_cop", onFoot);
	addAnimToGroup("move_cop_fat", onFoot);
	addAnimToGroup("move_cop_search", onFoot);
	/*addAnimToGroup("move_crouch", onFoot);
	addAnimToGroup("move_crouch_hgun", onFoot);
	addAnimToGroup("move_crouch_rifle", onFoot);
	addAnimToGroup("move_crouch_rpg", onFoot);*/
	addAnimToGroup("move_f@armed", onFoot);
	addAnimToGroup("move_f@bness_a", onFoot);
	addAnimToGroup("move_f@bness_b", onFoot);
	addAnimToGroup("move_f@bness_c", onFoot);
	addAnimToGroup("move_f@bness_d", onFoot);
	addAnimToGroup("move_f@bness_e", onFoot);
	addAnimToGroup("move_f@casual", onFoot);
	addAnimToGroup("move_f@casual_b", onFoot);
	addAnimToGroup("move_f@casual_c", onFoot);
	addAnimToGroup("move_f@cower", onFoot);
	addAnimToGroup("move_f@fat", onFoot);
	addAnimToGroup("move_f@generic", onFoot);
	addAnimToGroup("move_f@michelle", onFoot);
	addAnimToGroup("move_f@multiplyr", onFoot);
	addAnimToGroup("move_f@m_p", onFoot);
	addAnimToGroup("move_f@old_a", onFoot);
	addAnimToGroup("move_f@old_b", onFoot);
	addAnimToGroup("move_f@old_c", onFoot);
	addAnimToGroup("move_f@old_d", onFoot);
	addAnimToGroup("move_f@puffer", onFoot);
	addAnimToGroup("move_f@sexy", onFoot);
	addAnimToGroup("move_gng@afro_a", onFoot);
	addAnimToGroup("move_gng@afro_b", onFoot);
	addAnimToGroup("move_gng@afro_c", onFoot);
	addAnimToGroup("move_gng@generic_a", onFoot);
	addAnimToGroup("move_gng@generic_b", onFoot);
	addAnimToGroup("move_gng@generic_c", onFoot);
	addAnimToGroup("move_gng@jam_a", onFoot);
	addAnimToGroup("move_gng@jam_b", onFoot);
	addAnimToGroup("move_gng@jam_c", onFoot);
	addAnimToGroup("move_gng@latino_a", onFoot);
	addAnimToGroup("move_gng@latino_b", onFoot);
	addAnimToGroup("move_gng@latino_c", onFoot);
	addAnimToGroup("move_injured_generic", onFoot);
	addAnimToGroup("move_injured_ground", onFoot);
	addAnimToGroup("move_injured_lower", onFoot);
	addAnimToGroup("move_injured_upper", onFoot);
	addAnimToGroup("move_m@bernie", onFoot);
	addAnimToGroup("move_m@bness_a", onFoot);
	addAnimToGroup("move_m@bness_b", onFoot);
	addAnimToGroup("move_m@bness_c", onFoot);
	addAnimToGroup("move_m@bum", onFoot);
	addAnimToGroup("move_m@case", onFoot);
	addAnimToGroup("move_m@casual", onFoot);
	addAnimToGroup("move_m@casual_b", onFoot);
	addAnimToGroup("move_m@casual_c", onFoot);
	addAnimToGroup("move_m@cower", onFoot);
	addAnimToGroup("move_m@cs_swat", onFoot);
	addAnimToGroup("move_m@eddie", onFoot);
	addAnimToGroup("move_m@fat", onFoot);
	addAnimToGroup("move_m@generic", onFoot);
	addAnimToGroup("move_m@h_cuffed", onFoot);
	addAnimToGroup("move_m@multiplyr", onFoot);
	addAnimToGroup("move_m@old_a", onFoot);
	addAnimToGroup("move_m@old_b", onFoot);
	addAnimToGroup("move_m@old_c", onFoot);
	addAnimToGroup("move_m@playboy", onFoot);
	addAnimToGroup("move_m@roman", onFoot);
	addAnimToGroup("move_m@roman_inj", onFoot);
	addAnimToGroup("move_m@swat", onFoot);
	addAnimToGroup("move_m@tourist", onFoot);

	/*addAnimToGroup("move_melee");
	addAnimToGroup("move_melee_baseball");
	addAnimToGroup("move_melee_knife");;
	addAnimToGroup("move_rifle");
	addAnimToGroup("move_roman_inj");
	addAnimToGroup("move_rpg");*/


	//onFootArmed.push_back(NULL);
	addAnimToGroup("move_rifle", onFootArmed);
	addAnimToGroup("move_rpg", onFootArmed);

	addAnimToGroup("move_crouch", onFootCrouch);

	addAnimToGroup("move_crouch", onFootCrouchArmed);
	addAnimToGroup("move_crouch_hgun", onFootCrouchArmed);
	addAnimToGroup("move_crouch_rifle", onFootCrouchArmed);
	addAnimToGroup("move_crouch_rpg", onFootCrouchArmed);

	addAnimToGroup("swimming", onWater);

	addAnimToGroup("climb_std", onClimb);
	addAnimToGroup("climb", onClimb);

	addAnimToGroup("parachute", onParachute);
}

size_t searchForAnim(int action)
{
	players::netPlayer& p = players::getLocalPlayer();

	/*if(Scripting::IsCharSwimming(p.ped))
	{
		buf = onWater;
	}*/
	if(action == 1)
	{
		buf = onClimb;
		//Scripting::PrintStringWithLiteralStringNow("STRING", "CLIMB", 100, true);
	}
	else if(action == 2)
	{
		buf = onParachute;
	}
	else if(Scripting::IsCharOnFoot(p.ped))
	{		
		if(Scripting::IsCharDucking(p.ped))
		{
			if(p.currentWeapon != 0)
			{
				buf = onFootCrouchArmed;
			}
			else
			{
				buf = onFootCrouch;
			}
		}
		else if(p.currentWeapon != 0)
		{
			buf = onFootArmed;
		}
		else
		{
			buf = onFoot;
			//Scripting::PrintStringWithLiteralStringNow("STRING", "FOOT", 100, true);
		}
	}
	else
	{
		buf = onFoot;
		//Scripting::PrintStringWithLiteralStringNow("STRING", "ELSE FOOT", 100, true);
	}
	//LINFO << "Searching anim";

	size_t sequenceSize = buf.size();
	//size_t groupsSize = animList::getGroupsSize();

	for(size_t i = 0; i < sequenceSize; ++i)
	{
		//LINFO << "Using: " << buf.at(i);
		animList::animGroup& grp = animList::getAnimGroup(buf.at(i).browserId);
		for(size_t find = grp.beginsAtId; find <= grp.endsAtId; find++)
		{
			if(Scripting::IsCharPlayingAnim(p.ped, animList::getAnimType(find).animGroup, animList::getAnimType(find).animName))
			{
				return find;
			}
		}

			//if(std::strcmp(buf.at(i), animList::getAnimGroup(it).groupName) == 0)
			//{
				//LINFO << "Found group: " << animList::getAnimGroup(it).beginsAtId << ", " << animList::getAnimGroup(it).endsAtId;
				//LINFO << animList::getAnimGroup(it).groupName;
				//for(size_t find = animList::getAnimGroup(it).beginsAtId; find <= animList::getAnimGroup(it).endsAtId; ++find)
				//{
					//LINFO << animList::getAnimType(find).animGroup << ", " <<animList::getAnimType(find).animName;
					//if(Scripting::IsCharPlayingAnim(p.ped, animList::getAnimType(find).animGroup, animList::getAnimType(find).animName))
					//{
						/*std::ostringstream s;
						s << "Anim: " << animList::getAnimType(find).animGroup << ", " <<animList::getAnimType(find).animName;
						Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 60000, true);*/
						//return find;
					//}
				//}
	}

	return 0;
}

size_t getAnimByGroup(char * group)
{
	players::netPlayer& p = players::getLocalPlayer();

	/*for(std::map<size_t, animList::anims>::iterator i = animList::getAnimsBegin(); i != animList::getAnimsEnd(); ++i)
	{
		if(std::strcmp(group, i->second.animGroup) == 0 && Scripting::IsCharPlayingAnim(p.ped, group, i->second.animName))
		{
			return i->first;
		}
	}*/
	return 0;
}

void changeDefaultAnimGroup(char* c)
{
	size_t buf = animList::getGroupByName(c);
	if(buf != 999999)
	{
		onFoot.at(0).name = c;
		onFoot.at(0).browserId = buf;
		//onFootArmed.at(0) = c;
	}
}

unsigned int getAnimIdByName(char* group, char* name)
{
	size_t groupId = animList::getGroupByName(group);
	if(groupId == 999999)
	{
		LINFO << "Anim group (" << group << ") does not exist";
		return 999999;
	}

	animList::animGroup& grp = animList::getAnimGroup(groupId);
	for(size_t find = grp.beginsAtId; find <= grp.endsAtId; find++)
	{
		if(strcmp(name, animList::getAnimType(find).animName) == 0 && strcmp(group, animList::getAnimType(find).animGroup) == 0)
		{
			return find;
		}
	}
	return 999999;
}

unsigned int serverAnim = 999999;
bool severAnimRepeat = false;

void setServerAnim(size_t animId)
{
	serverAnim = animId;
}

void setServerAnimRepeat(bool b)
{
	severAnimRepeat = b;
}

bool doesServerAnimRepeat()
{
	return severAnimRepeat;
}

unsigned int getServerAnim()
{
	return serverAnim;
}

unsigned int sittingAnim = 0;
char* sittingGroup = "missfrancis5";
char* sittingName = "idle_bench_a";
unsigned int somethingSitting = 0;

unsigned int getSitting()
{
	return sittingAnim;
}


void setSittingAnim(std::string group, std::string name, unsigned int so)
{
	char *st1 = &group[0u];
	char *st2 = &name[0u];
	unsigned int i = getAnimIdByName(st1, st2);
	if(i != 999999)
	{
		sittingAnim = i;
		sittingGroup = st1, sittingName = st2;
		somethingSitting = so;
	}
}

void forceSitNow()
{
	players::netPlayer& p = players::getLocalPlayer();
	if(Scripting::IS_SITTING_OBJECT_NEAR(p.pos.x, p.pos.y, p.pos.z))
	{
		Scripting::TASK_SIT_DOWN_ON_NEAREST_OBJECT(p.ped, p.pos.x, p.pos.y, p.pos.z);
		Scripting::ChangeCharSitIdleAnim(p.ped, sittingGroup, sittingName, somethingSitting);
	}
}