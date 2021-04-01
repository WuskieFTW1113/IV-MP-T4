#include "checkPointsController.h"
#include "players.h"
#include "easylogging++.h"
#include "apiPlayerEvents.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "entitySpawned.h"

std::map<int, apiCheckPoint::checkPoint*> cps;

apiPlayerEvents::pCheckPoint cpEnter = 0;
apiPlayerEvents::pCheckPoint cpExit = 0;

int apiCheckPoint::addNew(apiMath::Vector3 pos, float radius, unsigned int hexColor, int type, int blipType, unsigned int vWorld)
{
	int id = 1;
	for(std::map<int, apiCheckPoint::checkPoint*>::const_iterator i = cps.begin(); i != cps.end(); ++i)
	{
		if(i->first != id)
		{
			id = i->first;
			break;
		}
		id++;
	}

	checkPoint* cp = new checkPoint(id, pos, radius, hexColor, type, blipType, vWorld);
	cps.insert(std::make_pair(id, cp));
	return id;
}

void apiCheckPoint::remove(int id)
{
	int objWorld = cps.at(id)->getWorld();
	networkManager::connection* con = networkManager::getConnection();
	
	RakNet::BitStream vOut;
	vOut.Write((MessageID)IVMP);
	vOut.Write(CHECK_POINT_DELETE);
	vOut.Write(id);

	for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		con->peer->Send(&vOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
		if(i->second.sCheckPoints.find(id) != i->second.sCheckPoints.end())
		{
			i->second.sCheckPoints.erase(id);
			entitySpawned::spawned(i->second.id, 0, 2, id);
		}
	}		
	cps.erase(id);
}

bool apiCheckPoint::isValid(int id)
{
	return cps.find(id) != cps.end();
}

apiCheckPoint::checkPoint* apiCheckPoint::get(int id)
{
	return cps.find(id) != cps.end() ? cps.at(id) : NULL;
}

void apiCheckPoint::getAll(std::vector<checkPoint*>& vec)
{
	std::map<int, apiCheckPoint::checkPoint*>::iterator i = cps.begin(), e = cps.end();
	for(i; i != e; ++i)
	{
		vec.push_back(i->second);
	}
}

std::map<int, apiCheckPoint::checkPoint*>::iterator checkPointsController::getBegin()
{
	return cps.begin();
}

std::map<int, apiCheckPoint::checkPoint*>::iterator checkPointsController::getEnd()
{
	return cps.end();
}

void checkPointsController::checkPointSpawned(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	int cpId = 0;
	bsIn.Read(cpId);

	if(cps.find(cpId) != cps.end())
	{
		players::getPlayer(con->packet->guid.g).sCheckPoints[cpId].hasSpawned = true;
		entitySpawned::spawned(players::getPlayer(con->packet->guid.g).id, 1, 2, cpId);
		//LINFO << "Player " << players::getPlayer(con->packet->guid.g).id << " spawned cp " << cpId;
	}
}

void checkPointsController::checkPointDeleted(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	int cpId = 0;
	bsIn.Read(cpId);

	players::player& p = players::getPlayer(con->packet->guid.g);
	if(p.sCheckPoints.find(cpId) != p.sCheckPoints.end())
	{
		p.sCheckPoints.erase(cpId);
		entitySpawned::spawned(p.id, 0, 2, cpId);
		//LINFO << "Player " << players::getPlayer(con->packet->guid.g).id << " deleted cp " << cpId;
	}
}

void checkPointsController::checkPointEntered(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	int cpId = 0;
	bsIn.Read(cpId);

	if(cpEnter != 0)
	{
		cpEnter(players::getPlayer(con->packet->guid.g).id, cpId);
	}
	//LINFO << "Player " << players::getPlayer(con->packet->guid.g).id << " entered cp " <<cpId;
}

void checkPointsController::checkPointExit(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	int cpId = 0;
	bsIn.Read(cpId);

	if(cpExit != 0)
	{
		cpExit(players::getPlayer(con->packet->guid.g).id, cpId);
	}
	//LINFO << "Player " << players::getPlayer(con->packet->guid.g).id << " exited cp " <<cpId;
}

void apiPlayerEvents::registerPlayerEnterCP(pCheckPoint f)
{
	cpEnter = f;
}

void apiPlayerEvents::registerPlayerExitCP(pCheckPoint f)
{
	cpExit = f;
}