#include "API/apiObjects.h"
#include "networkManager.h"
#include "players.h"
#include "../SharedDefines/packetsIds.h"

apiObjects::object::object(int id, apiMath::Vector3 pos, apiMath::Quaternion rot, unsigned int objHex, unsigned int vWorld, bool hasOffset, 
	unsigned int interior, int alpha) {
	this->id = id;
	this->pos = pos;
	this->rot = rot;
	this->objHex = objHex;
	this->vWorld = vWorld;
	this->alpha = alpha;
	this->interior = interior;
	this->hasOffSet = hasOffset;
	this->streamDistance = 200.0f;
}

apiObjects::object::~object()
{
}

apiMath::Vector3 apiObjects::object::getPos()
{
	return this->pos;
}

apiMath::Quaternion apiObjects::object::getRot()
{
	return this->rot;
}

unsigned int apiObjects::object::getWorld()
{
	return this->vWorld;
}

unsigned int apiObjects::object::getModel()
{
	return this->objHex;
}

void apiObjects::object::setPos(apiMath::Vector3 pos)
{
	this->pos = pos;
}

void apiObjects::object::setRot(apiMath::Quaternion rot)
{
	this->rot = rot;
}

bool apiObjects::object::hasOffset()
{
	return this->hasOffSet;
}

unsigned int apiObjects::object::getInterior()
{
	return this->interior;
}

int apiObjects::object::getAlpha()
{
	return this->alpha;	
}

void apiObjects::object::setStreamDis(float dis)
{
	this->streamDistance = dis;
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SET_OBJECT_STREAM_DISTANCE);
	bsOut.Write(this->id);
	bsOut.Write(dis);

	std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(), pEnd = players::getPlayersEnd();
	for(i; i != pEnd; ++i)
	{
		networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
	}
}
