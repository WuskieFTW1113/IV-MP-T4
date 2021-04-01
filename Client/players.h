#ifndef players_H
#define players_H

#include "../ClientManager/ivScripting.h"
#include "simpleMath.h"
#include <map>
#include <Windows.h>

namespace players
{
	struct netPlayer
	{
		int toBeDeleted; //0 no, 1 delete, 2 just respawn
		
		Scripting::Player playerIndex;
		Scripting::Ped ped;
		DWORD memoryAddress;

		bool isAfk;

		unsigned int interior;

		bool volatile threadLock;

		//std::string playerName;
		char playerName[50];
		bool shouldDrawName;

		int serverId;

		simpleMath::Vector3 pos;
		simpleMath::Vector3 endPos;
		simpleMath::Vector3 startPos;
		simpleMath::Vector3 posError;

		unsigned long skin;
		int color[4];

		float heading;
		float endHeading;
		float startHeading;

		bool spawned;

		int currentAnim;
		int enteringVehicle[2];
		bool animRepeat;
		int oldAnim;
		long animLastForced;
		long lastRagdoll;
		int serverAnim;

		unsigned currentWeapon;
		unsigned int currentAmmo;
		unsigned int endAmmo;
		unsigned int startAmmo;
		bool weaponChanged;
		long lastWeaponFire;

		bool isAiming;
		simpleMath::Vector3 aim;
		simpleMath::Vector3 endAim;
		simpleMath::Vector3 startAim;

		Scripting::Blip blip;
		bool shouldAddBlip;

		size_t inVehicle;
		size_t lastVehicle;
		size_t passangerOf;
		int passangerSeat;

		long polTime;
		long lastPol;
		bool hasPol;
		float lastPolProgress;

		bool isDucking;

		long vehicleEntryRequestTime;

		float cAtan;

		void noLongerDriving(bool resetPassenger);

		int clothes[10];
		int props[2];

		long lastHpChange;

		unsigned int parachute;

		bool hasControl;

		//LOCAL AND NPC SYNC DEFINES
		bool deathPcktSent;
		long lastDeathTime;
		long lastHealthTime;
		unsigned int lastHealthPck;
		long lastHealthChange;
		long lastFreeze;
		u32 lastAmmo;
		long lastNpcSync;
	};

	netPlayer& getLocalPlayer();

	bool isPlayer(int playerid);
	void insertPlayer(int playerid, netPlayer &p);

	void removePlayer(int playerid);
	void removePlayerUsingIterator(std::map<int, netPlayer>::iterator& it);

	netPlayer& getPlayer(int playerid);

	std::map<int, netPlayer>::iterator getPlayersBegin();
	std::map<int, netPlayer>::iterator getPlayersEnd();

	size_t streamedPlayers();
	size_t spawnedPlayers();

	bool respawnLocalPlayer();

	void setClothes(netPlayer &p, int part);
}
#endif