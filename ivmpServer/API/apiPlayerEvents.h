#include "apiPlayer.h"
#include <exporting.h>

#ifndef apiPlayerEvents_H
#define apiPlayerEvents_H

namespace apiPlayerEvents
{
	DLL typedef void(*vLeft)(int playerid, int vehicleId, int seatId);
	DLL typedef void(*vEnterRequest)(int playerid, int vehicleId, int seatId, bool carjack);
	DLL void registerLeftVehicle(vLeft f);
	DLL void registerEnteringVehicle(vEnterRequest f);
	DLL void registerEnteredVehicle(vLeft f);

	DLL typedef bool(*pJoin)(int playerid);
	DLL void registerPlayerCredentials(pJoin f);
	DLL void registerPlayerJoin(pJoin f); //Triggered when the player requests the connection, can be used to check their IP

	DLL typedef void(*pLeft)(int playerid, int reason); //Reasons: 0 disconnect, 1 timeout
	DLL void registerPlayerLeft(pLeft f);

	DLL typedef bool(*pWeap)(int playerid, int weapon, int ammo);
	DLL void registerPlayerWeaponChanged(pWeap f);
	DLL void registerTeamSpeak(pWeap f);

	DLL typedef void(*pChat)(int playerid, const char* chat);
	DLL void registerPlayerChat(pChat f);

	DLL typedef void(*pAfk)(int playerid, bool status);
	DLL void registerPlayerAfk(pAfk f);

	DLL typedef void(*pDeath)(int playerid, int agentType, int agentId); //Agent types: 0 player, 1 vehicle, other unknown
	DLL typedef void(*pHp)(int playerid, int agentType, int agentId, int boneId);
	DLL void registerPlayerDeath(pDeath f);
	DLL void registerPlayerHpChange(pHp f);
	DLL void registerPlayerArmor(pHp f);
	DLL void registerPlayerSpawnEntity(pHp f);

	DLL void registerPlayerRespawn(pJoin f); //Called once the player respawns after death

	DLL typedef void(*pCheckPoint)(int playerid, int checkPointId); //The ID can be invalid
	DLL void registerPlayerEnterCP(pCheckPoint f);
	DLL void registerPlayerExitCP(pCheckPoint f);

	DLL typedef void(*pDialogListResponse)(int playerid, unsigned int dialogId, int buttonId, int rowId, int customId);
	DLL void registerPlayerDialogListResponse(pDialogListResponse f);

	DLL typedef void(*checkSumResponse)(int playerid, const char* path, unsigned int sum);
	DLL void registerPlayerCheckSum(checkSumResponse f); //Triggered by apiPlayer::checkSum(...)
	DLL void registerPlayerHackCheck(pAfk f);

	DLL void registerPlayerWeaponsArrived(pJoin f); //Triggered by apiPlayer::requestWeapons()

	DLL void registerPlayerChatUpdated(vLeft f);
}

#endif