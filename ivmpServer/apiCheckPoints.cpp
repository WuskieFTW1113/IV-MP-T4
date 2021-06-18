#include "API/apiCheckPoints.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "players.h"

apiCheckPoint::checkPoint::checkPoint(int id, apiMath::Vector3 pos, float radius, unsigned int hexColor, int type, int blipType, unsigned int vWorld)
{
	this->id = id;
	this->pos = pos;
	this->radius = radius;
	this->type = type;
	this->hexColor = hexColor;
	this->vWorld = vWorld;
	this->blipType = blipType;
	this->streamDis = 200.0f;
}

apiCheckPoint::checkPoint::~checkPoint()
{
}

apiMath::Vector3 apiCheckPoint::checkPoint::getPos()
{
	return this->pos;
}

float apiCheckPoint::checkPoint::getRadius()
{
	return this->radius;
}

int apiCheckPoint::checkPoint::getType()
{
	return this->type;
}

unsigned int apiCheckPoint::checkPoint::getWorld()
{
	return this->vWorld;
}

unsigned int apiCheckPoint::checkPoint::getColor()
{
	return this->hexColor;
}

int apiCheckPoint::checkPoint::getBlip()
{
	return this->blipType;
}

void apiCheckPoint::checkPoint::setStream(float d)
{
	this->streamDis = d;
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SET_CHECKPOINT_STREAM_DISTANCE);
	bsOut.Write(this->id);
	bsOut.Write(d);

	std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(), pEnd = players::getPlayersEnd();
	for(i; i != pEnd; ++i)
	{
		networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
	}
}

void apiCheckPoint::setBlipTypeForPlayer(int cpId, int player, int type)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SET_CHECK_POINT_BLIP_SPRITE);
	bsOut.Write(cpId);
	bsOut.Write(type);

	uint64_t id = players::isPlayerIdOnline(player);
	if(id == 0)
	{
		return;
	}
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(id).ip, false);
}

void apiCheckPoint::setShowingForPlayer(int cpId, int player, bool showing)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(SET_CHECK_POINT_SHOWING);
	bsOut.Write(cpId);
	bsOut.Write(showing ? 1 : 0);

	uint64_t id = players::isPlayerIdOnline(player);
	if(id == 0)
	{
		return;
	}
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(id).ip, false);
}
