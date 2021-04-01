#include "objectsController.h"
#include "players.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "easylogging++.h"
#include "entitySpawned.h"
#include <time.h>
#include <map>

struct objectMove
{
	clock_t start, end;
	apiMath::Vector3 startPos, endPos;
	apiMath::Quaternion startRot, endRot;
};

struct internalObject
{
	apiObjects::object* obj;
	objectMove* move;
	RakNet::BitStream* bs;

	internalObject(){}
};

std::map<int, internalObject> objs;

int apiObjects::addNew(apiMath::Vector3 pos, apiMath::Quaternion rot, unsigned int objHex, unsigned int vWorld,
	bool hasOffset, unsigned int interior, int alpha, float streamDis) {

	int id = 1;
	for(std::map<int, internalObject>::const_iterator i = objs.begin(); i != objs.end(); ++i)
	{
		if(i->first != id)
		{
			break;
		}
		id++;
	}

	internalObject obj;
	obj.bs = new RakNet::BitStream();
	obj.bs->Write((MessageID)IVMP);
	obj.bs->Write(OBJECT_CREATE);
	obj.bs->Write(id);
	obj.bs->Write(vWorld);
	obj.bs->Write(streamDis);
	obj.bs->Write(objHex);
	obj.bs->Write(pos.x);
	obj.bs->Write(pos.y);
	obj.bs->Write(pos.z);
	obj.bs->Write(rot.X);
	obj.bs->Write(rot.Y);
	obj.bs->Write(rot.Z);
	obj.bs->Write(rot.W);

	obj.bs->Write(hasOffset ? 1 : 0);
	obj.bs->Write(interior);
	obj.bs->Write(alpha);

	obj.obj = new apiObjects::object(id, pos, rot, objHex, vWorld, hasOffset, interior, alpha);
	obj.move = NULL;
	objs[id] = obj;

	std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(), pEnd = players::getPlayersEnd();
	networkManager::connection* con = networkManager::getConnection();
	for(i; i != pEnd; ++i)
	{
		con->peer->Send(obj.bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
	}
	return id;
}

void apiObjects::remove(int id)
{
	internalObject& obj = objs.at(id);
	int objWorld = obj.obj->getWorld();
	networkManager::connection* con = networkManager::getConnection();
	
	RakNet::BitStream vOut;
	vOut.Write((MessageID)IVMP);
	vOut.Write(OBJECT_DELETE);
	vOut.Write(id);

	std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(), oEnd = players::getPlayersEnd();
	for(i; i != oEnd; ++i)
	{
		con->peer->Send(&vOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
		if(i->second.sObjects.find(id) != i->second.sObjects.end())
		{
			i->second.sObjects.erase(id);
			entitySpawned::spawned(i->second.id, 0, 3, id);
		}
	}	

	delete obj.obj;
	if(obj.move) delete obj.move;

	objs.erase(id);
}

bool apiObjects::isValid(int id)
{
	return objs.find(id) != objs.end();
}

apiObjects::object* apiObjects::get(int id)
{
	return objs.find(id) != objs.end() ? objs.at(id).obj : NULL;
}

void apiObjects::getAll(std::vector<object*>& vec)
{
	std::map<int, internalObject>::iterator i = objs.begin(), e = objs.end();
	for(i; i != e; ++i)
	{
		vec.push_back(i->second.obj);
	}
}

void makeObjectMovementBitStream(int id, objectMove* om, RakNet::BitStream& b)
{	
	clock_t cTime = clock();
	b.Write((MessageID)IVMP);
	b.Write(OBJECT_MOVE);
	b.Write(id);
	b.Write(om->startPos.x);
	b.Write(om->startPos.y);
	b.Write(om->startPos.z);
	b.Write(om->endPos.x);
	b.Write(om->endPos.y);
	b.Write(om->endPos.z);

	b.Write(om->startRot.X);
	b.Write(om->startRot.Y);
	b.Write(om->startRot.Z);
	b.Write(om->startRot.W);
	b.Write(om->endRot.X);
	b.Write(om->endRot.Y);
	b.Write(om->endRot.Z);
	b.Write(om->endRot.W);

	b.Write(om->start);
	b.Write(om->end);
}

void objectsController::spawnedObject(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g)) return;

	int cpId = 0;
	bsIn.Read(cpId);

	if(objs.find(cpId) != objs.end())
	{
		players::player& player = players::getPlayer(con->packet->guid.g);

		player.sObjects[cpId].hasSpawned = true;
		entitySpawned::spawned(player.id, 1, 3, cpId);
	}
}

void objectsController::deletedObject(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g)) return;

	int cpId = 0;
	bsIn.Read(cpId);

	players::player& p = players::getPlayer(con->packet->guid.g);
	if(p.sObjects.find(cpId) != p.sObjects.end())
	{
		p.sObjects.erase(cpId);
		entitySpawned::spawned(p.id, 0, 3, cpId);
	}
}

void apiObjects::moveObject(int id, apiMath::Vector3 pos, apiMath::Quaternion rot, unsigned int time)
{
	if(objs.find(id) == objs.end())
	{
		mlog("Object does not exist");
		return;
	}

	internalObject& obj = objs.at(id);
	clock_t cTime = clock();
	if(obj.move)
	{
		//Since its already moving we predict and define its position via interpoloation
		float progress = 0.0f;
		
		if(cTime >= obj.move->end) progress = 1.0f;
		else progress = (float)(cTime - obj.move->start) / (obj.move->end - obj.move->start);
		
		apiMath::Vector3 spos = apiMath::vecLerp(obj.obj->getPos(), obj.move->endPos, progress);
		apiMath::Quaternion q;
		apiMath::slerp(q, obj.obj->getRot(), obj.move->endRot, progress);

		obj.obj->setPos(spos);
		obj.obj->setRot(q);

		delete obj.move;
	}

	obj.move = new objectMove();
	obj.move->endPos = pos;
	obj.move->endRot = rot;
	obj.obj->copyPos(obj.move->startPos);
	obj.obj->copyRot(obj.move->startRot);
	obj.move->start = cTime;
	obj.move->end = cTime + time;

	RakNet::BitStream b;
	networkManager::connection* con = networkManager::getConnection();
	makeObjectMovementBitStream(id, obj.move, b);

	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		con->peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
	}
}

void objectsController::streamObjects(networkManager::connection* con, RakNet::SystemAddress& ip)
{
	RakNet::BitStream b;

	b.Write((MessageID)IVMP);
	b.Write(SYNC_GAME_TIME);
	b.Write(con->cTime);
	networkManager::getConnection()->peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ip, false);
	
	std::map<int, internalObject>::iterator i = objs.begin(), e = objs.end();
	for(i; i != e; ++i)
	{
		con->peer->Send(i->second.bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, ip, false);
		if (i->second.move)
		{	
			b.Reset();
			makeObjectMovementBitStream(i->first, i->second.move, b);
			con->peer->Send(&b, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, ip, false);
		}
	}
}