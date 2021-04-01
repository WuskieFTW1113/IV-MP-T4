#include "requestAndReceivePlayerData.h"
#include "players.h"
#include "easylogging++.h"
#include "../SharedDefines/packetsIds.h"
#include <map>
#include "playerList.h"
#include "parseConnectionStates.h"

std::map<int, long> requestedData;

void receivePlayerData(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		if(players::isPlayer(playerid))
		{
			LINFO << "!Error: Received information for existing player: " << playerid;
			return;
			/*if(players::getPlayer(playerid).spawned)
			{
				players::getPlayer(playerid).toBeDeleted = true;
				return;
			}*/
		}

		RakNet::RakString playerName;
		bsIn.Read(playerName);

		LINFO << "Player " << playerid << " (" << playerName << ") init";

		players::netPlayer pn;

		pn.serverId = playerid;
		pn.hasControl = false;

		/*std::ostringstream os;
		os << "(" << playerid << ")_" << playerName;

		pn.playerName = os.str();*/
		sprintf_s(pn.playerName, 50, "(%i)_%s", playerid, playerName.C_String());
		pn.vehicleEntryRequestTime = 0;

		bsIn.Read(pn.skin);

		unsigned int hexColor = 0xFFFFFFFF; //white
		bsIn.Read(hexColor);
		pn.color[0] = (hexColor >> 24) & 0xFF;
		pn.color[1] = (hexColor >> 16) & 0xFF;
		pn.color[2] = (hexColor >> 8) & 0xFF;
		pn.color[3] = hexColor & 0xFF;

		pn.spawned = false;
		pn.toBeDeleted = 0;

		bsIn.Read(pn.pos.x);
		bsIn.Read(pn.pos.y);
		bsIn.Read(pn.pos.z);

		bsIn.Read(pn.interior);

		pn.startPos = pn.pos;
		pn.endPos = pn.pos;

		bsIn.Read(pn.heading);
		
		pn.startHeading = pn.heading;
		pn.endHeading = pn.heading;

		bsIn.Read(pn.currentAnim);

		/*int animRepeating = 0;
		bsIn.Read(animRepeating);
		pn.animRepeat = animRepeating == 1;*/
		pn.animRepeat = false;

		pn.oldAnim = 0;

		bsIn.Read(pn.currentWeapon);
		pn.weaponChanged = false;
		pn.currentAmmo = 0;
		pn.startAmmo = 0;
		pn.endAmmo = 0;

		pn.isAiming = false;

		pn.inVehicle = 0;
		pn.lastVehicle = 0;
		pn.passangerOf = 0;
		pn.passangerSeat = -1;
		pn.threadLock = false;
		pn.isDucking = false;
		pn.lastPol = networkManager::getConnection()->cTime;
		pn.polTime = 50;
		pn.lastPolProgress = 0.0f;
		pn.lastRagdoll = 0;
		pn.animLastForced = 0;
		pn.lastHpChange = 0;
		pn.serverAnim = 0;
		pn.parachute = 0;
		pn.enteringVehicle[0] = 0, pn.enteringVehicle[1] = 0;

		//bsIn.Read(pn.shouldAddBlip);
		//bsIn.Read(pn.shouldDrawName);
		pn.shouldAddBlip = pn.shouldDrawName = getServerBlips();

		for(size_t i = 0; i < 10; i++)
		{
			pn.clothes[i] = 0;
		}

		for(size_t i = 0; i < 2; i++)
		{
			pn.props[i] = -1;
		}

		if(playerid > 999)
		{
			pn.deathPcktSent = false;
			pn.lastDeathTime = 0;

			pn.lastHealthTime = 0;
			pn.lastHealthPck = 0;
			pn.lastHealthChange = 0;
			pn.lastAmmo = 0;
			pn.lastNpcSync = 0;
		}

		players::insertPlayer(playerid, pn);

		if(requestedData.find(playerid) != requestedData.end())
		{
			requestedData.erase(playerid);
		}

		playerList::coloredUpdated(playerid, hexColor);
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (receivedPlayerData): " << e.what();
	}
}

void requestPlayerDate(int playerid)
{
	try
	{
		networkManager::connection* con = networkManager::getConnection();
		LINFO << "Requesting data for player " << playerid;
	
		if(requestedData.find(playerid) != requestedData.end() && con->cTime < requestedData.at(playerid) + 500)
		{
			LINFO << "Request aborted";
			return;
		}
	
		RakNet::BitStream bsOut;
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(REQUEST_PLAYER_DATA);
		bsOut.Write(playerid);

		con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);

		requestedData[playerid] = con->cTime;
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (requestingPlayerData): " << e.what();
	}
}