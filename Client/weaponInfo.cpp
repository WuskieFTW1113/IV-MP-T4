#include "weaponInfo.h"
#include <map>

std::map<int, weaponInfo::wInfo*> gameWeapon;

weaponInfo::wInfo::wInfo(int shootInterval, float range, int clipSize)
{
	this->shootInterval = shootInterval;
	this->range = range;
	this->clipSize = clipSize;
}

void weaponInfo::initGuns()
{
	gameWeapon[7] = new weaponInfo::wInfo(333, 50, 17);
	gameWeapon[9] = new weaponInfo::wInfo(333, 50, 9);
	gameWeapon[10] = new weaponInfo::wInfo(1233, 35, 8);
	gameWeapon[11] = new weaponInfo::wInfo(433, 40, 10);
	gameWeapon[12] = new weaponInfo::wInfo(66, 50, 50);
	gameWeapon[13] = new weaponInfo::wInfo(66, 60, 30);
	gameWeapon[14] = new weaponInfo::wInfo(133, 65, 30);
	gameWeapon[15] = new weaponInfo::wInfo(133, 70, 30);
	gameWeapon[16] = new weaponInfo::wInfo(400, 1500, 10);
	gameWeapon[17] = new weaponInfo::wInfo(333, 35, 5);
	gameWeapon[18] = new weaponInfo::wInfo(800, 100, 1);
	gameWeapon[21] = new weaponInfo::wInfo(800, 28, 1);
	gameWeapon[29] = new weaponInfo::wInfo(333, 50, 8);
	gameWeapon[30] = new weaponInfo::wInfo(133, 70, 20);
	gameWeapon[31] = new weaponInfo::wInfo(1500, 35, 20);
	gameWeapon[32] = new weaponInfo::wInfo(66, 50, 50);
	gameWeapon[34] = new weaponInfo::wInfo(133, 70, 200);
	gameWeapon[35] = new weaponInfo::wInfo(1300, 1500, 10);
}

bool weaponInfo::isWeapon(int id)
{
	return gameWeapon.find(id) != gameWeapon.end();
}

weaponInfo::wInfo* weaponInfo::getWeapon(int id)
{
	return gameWeapon.at(id);
}