#include "receiveFootSync.h"
#include "players.h"
#include "vehicles.h"
#include "../SharedDefines/easylogging++.h"
#include "../SharedDefines/packetsIds.h"
#include "API/apiPlayerEvents.h"
#include "vehicleSyncDeclarations.h"

apiPlayerEvents::vLeft leftVehFunc = 0;
apiPlayerEvents::pDeath deathFunc = 0;
apiPlayerEvents::pJoin respawnFunc = 0;
apiPlayerEvents::pHp hpFunc = 0;
apiPlayerEvents::pWeap weapFunc = 0;
apiPlayerEvents::pJoin weapInvent = 0;
apiPlayerEvents::pHp armorFunc = 0;
apiPlayerEvents::vLeft chatUpdated = 0;


void callVehicleLeft(int playerid, int vehicleId, int seat)
{
	if(leftVehFunc)
	{
		leftVehFunc(playerid, vehicleId, seat);
	}
}

void clientFootSync(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	//LINFO << "I been synced";=

	players::player& p = players::getPlayer(con->packet->guid.g);

	//LINFO << p.nick << " last anim: " << p.movementAnim;
	bsIn.Read(p.position.x);
	bsIn.Read(p.position.y);
	bsIn.Read(p.position.z);

	if(!p.camActive)
	{
		p.camPos = p.position;
	}

	/*bsIn.Read(p.velocity.x);
	bsIn.Read(p.velocity.y);
	bsIn.Read(p.velocity.z);*/
	bsIn.Read(p.heading);
	bsIn.Read(p.movementAnim);
	p.packetCount++;

	if(p.movementAnim >= 1 && p.movementAnim <= 4)
	{
		bsIn.Read(p.aim.x);
		bsIn.Read(p.aim.y);
		bsIn.Read(p.aim.z);
		bsIn.Read(p.currentAmmo);

		if(p.currentWeapon == 0)
		{
			p.movementAnim = p.movementAnim > 2 ? 5 : 0; //reset anim
		}
	}

	players::updateTimeOut(p, con->cTime + 2000);

	p.bs->Reset();
	p.bs->Write((MessageID)IVMP);
	p.bs->Write(POSITION_UPDATE);
	p.bs->Write(p.id);

	p.bs->Write(p.position.x);
	p.bs->Write(p.position.y);
	p.bs->Write(p.position.z);

	p.bs->Write(p.heading);
	p.bs->Write(p.movementAnim);

	if(p.movementAnim >= 1 && p.movementAnim <= 4)
	{
		p.bs->Write(p.aim.x);
		p.bs->Write(p.aim.y);
		p.bs->Write(p.aim.z);
		//p.bs->Write(p.currentAmmo);
	}

	//p.moduleHandle->position = p.position;

	if(p.currentVehicle != 0)
	{
		if(vehicles::isVehicle(p.currentVehicle))
		{
			if(leftVehFunc)
			{
				leftVehFunc(p.id, p.currentVehicle, 0);
			}
			vehicles::netVehicle& v = vehicles::getVehicle(p.currentVehicle);
			v.driverId = 0;
			if(v.engine == 2) v.engine = 1;
		}
		p.currentVehicle = 0;
		removeBeingJacked(p.id);
	}
	if(p.passangerOf != 0)
	{
		if(leftVehFunc)
		{
			leftVehFunc(p.id, p.passangerOf, p.passangerSeat);
		}
		p.passangerOf = 0;
		p.passangerSeat = -1;
	}
}

void clientChangedWeapon(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	//player& pd = getPlayer(con->packet->guid.g + 1);
	players::player& p = players::getPlayer(con->packet->guid.g);
	players::updateTimeOut(p, con->cTime + 2000);

	int newWeapon = 0;
	int newAmmo = 0;
	bsIn.Read(newWeapon);
	bsIn.Read(newAmmo);

	if(p.currentWeapon != newWeapon && weapFunc != 0 && !weapFunc(p.id, newWeapon, newAmmo))
	{
		p.currentWeapon = 0;
		return;
	}

	p.currentWeapon = newWeapon;
	p.currentAmmo = newAmmo;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(WEAPON_CHANGE);
	bsOut.Write(p.id); //switch to pd
	bsOut.Write(p.currentWeapon);
	//bsOut.Write(p.currentAmmo);
	
	//LINFO << "Player " << p.id << " weapon: " << p.currentWeapon << ", ammo: " << p.currentAmmo;

	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(p.id != it->second.id && p.vWorld == it->second.camWorld && it->second.sPlayers.find(p.id) != it->second.sPlayers.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}

void clientDuckStateChanged(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);

	int buf = 0;
	bsIn.Read(buf);

	p.isDucking = buf == 1 ? true : false;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CHAR_DUCK_STATE);
	bsOut.Write(p.id);
	bsOut.Write(buf);

	//LINFO << "Player " << p.id << " duck state: " << buf;
							
	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(p.id != it->second.id && p.vWorld == it->second.camWorld && it->second.sPlayers.find(p.id) != it->second.sPlayers.end())
		{
			con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}

void clientDeath(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);

	int agentType = -1;
	int agentId = -1;

	bsIn.Read(agentType);
	bsIn.Read(agentId);

	players::updateTimeOut(p, con->cTime + 10000);
	p.movementAnim = 10001; //death anim

	if(deathFunc != 0)
	{
		deathFunc(p.id, agentType, agentId);
	}
}

void clientRespawned(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	if(respawnFunc != 0)
	{
		players::player& p = players::getPlayer(con->packet->guid.g);
		respawnFunc(p.id);
	}
}

void clientHpChanged(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);
	
	bsIn.Read(p.hp);

	int entity = -1, isCar = -1, bone = -1;
	bsIn.Read(entity);

	if(entity != -1)
	{
		isCar = -1;
		bsIn.Read(isCar);
		bsIn.Read(bone);
	}

	if(hpFunc != 0)
	{
		hpFunc(p.id, isCar, entity, bone);
	}
}

void clientInteriorChanged(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}
	
	bsIn.Read(players::getPlayer(con->packet->guid.g).interior);	
}

void clientWeaponsRecieved(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(weapInvent == 0)
	{
		return;
	}

	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}
	
	int weaponCount = 0;
	bsIn.Read(weaponCount);

	players::player& p = players::getPlayer(con->packet->guid.g);
	p.playerWeapons.clear();

	for(int i = 0; i < weaponCount; i++)
	{
		playerWeapon pw;
		bsIn.Read(pw.weapon);
		bsIn.Read(pw.ammo);
		p.playerWeapons.push_back(pw);
	}

	weapInvent(p.id);
}

void apiPlayerEvents::registerLeftVehicle(vLeft f)
{
	leftVehFunc = f;
}

void apiPlayerEvents::registerPlayerDeath(pDeath f)
{
	deathFunc = f;
}

void apiPlayerEvents::registerPlayerRespawn(pJoin f)
{
	respawnFunc = f;
}

void apiPlayerEvents::registerPlayerHpChange(pHp f)
{
	hpFunc = f;
}

void apiPlayerEvents::registerPlayerWeaponChanged(pWeap f)
{
	weapFunc = f;
}

void apiPlayerEvents::registerPlayerWeaponsArrived(pJoin f)
{
	weapInvent = f;
}

void apiPlayerEvents::registerPlayerArmor(pHp f)
{
	armorFunc = f;
}

void clientAmmoUpdated(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	//player& pd = getPlayer(con->packet->guid.g + 1);
	players::player& p = players::getPlayer(con->packet->guid.g);

	int newWeapon = 0;
	int newAmmo = 0;
	bsIn.Read(newWeapon);
	bsIn.Read(newAmmo);

	if(p.currentWeapon != newWeapon || newAmmo < 0 || newAmmo > 2000)
	{
		return;
	}

	p.currentAmmo = newAmmo;
	//LINFO << "Player " << p.id << " updated ammo to " << newAmmo;
}

void clientArmorUpdated(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);
	bsIn.Read(p.armor);

	int entity = -1, isCar = -1, bone = -1;
	bsIn.Read(entity);

	if(entity != -1)
	{
		isCar = -1;
		bsIn.Read(isCar);
		bsIn.Read(bone);
	}

	if(armorFunc != 0)
	{
		armorFunc(p.id, isCar, entity, bone);
	}
}

void apiPlayerEvents::registerPlayerChatUpdated(vLeft f)
{
	chatUpdated = f;
}

void clientChatUpdated(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}
	int chatId, chatOp = 0;
	bsIn.Read(chatId);
	bsIn.Read(chatOp);

	if(chatUpdated != 0) chatUpdated(players::getPlayer(con->packet->guid.g).id, chatId, chatOp);
}
