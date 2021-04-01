#include "playerDriveBy.h"
#include <map>
#include <string>

std::map<unsigned int, std::string> dbGroup;

// SET_CHAR_ANIM_CURRENT_TIME( uParam0, "MINI_GOLF", "Swing_Intro", fVar3 - l_U3064 );

void playerDriveBy::init()
{
	dbGroup[418536135] = "veh@drivebylow";
}