#include "localPlayerInvent.h"
#include "Scripting.h"
#include "players.h"
#include "easylogging++.h"

struct weapSlot
{
	unsigned int weapon;
	unsigned int ammo;
	unsigned int ammo2;
};

std::map<unsigned int, weapSlot> pWeaps;

void localInvent::addWeapon(unsigned int weapon, unsigned int ammo)
{
	Scripting::GiveWeaponToChar(players::getLocalPlayer().ped, weapon, ammo, true);
}

void localInvent::removeWeapon(unsigned int weapon)
{
	if(weapon == 0)
	{
		Scripting::RemoveAllCharWeapons(players::getLocalPlayer().ped);
	}
	else
	{
		NativeInvoke::Invoke<Scripting::NATIVE_REMOVE_WEAPON_FROM_CHAR, Scripting::ScriptVoid>(players::getLocalPlayer().ped, weapon);
	}
}

void localInvent::storeWeapons()
{
	pWeaps.clear();
	for(unsigned int i = 0; i < 10; i++)
	{
		weapSlot w;
		Scripting::GetCharWeaponInSlot(players::getLocalPlayer().ped, i, &w.weapon, &w.ammo, &w.ammo2);
		pWeaps.insert(std::make_pair(i, w));

		LINFO << "Slot: " << i << ", Weap: " << w.weapon << ", " << w.ammo;
	}
}

void localInvent::refreshWeapons()
{
	players::netPlayer& p = players::getLocalPlayer();
	for(std::map<unsigned int, weapSlot>::iterator i = pWeaps.begin(); i != pWeaps.end(); ++i)
	{
		Scripting::GiveWeaponToChar(p.ped, i->second.weapon, i->second.ammo, false);
	}
}