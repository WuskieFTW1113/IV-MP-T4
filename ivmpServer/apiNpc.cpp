#include "apiNpc.h"
#include "npcChar.h"
#include <time.h>
#include "easylogging++.h"
#include "../SharedDefines/packetsIds.h"
#include "networkManager.h"
#include "players.h"

void apiNpc::clearNpcTaks(int npc)
{
	npcChars::npcChar* c = npcChars::getNpc(npc);
	c->footInputs.clear();
	c->vehicleInputs.clear();
	c->footInputsSize = 0;
	c->vehicleInputsSize = 0;
	c->done = true;
}

void apiNpc::setNpcStartPos(int npc, apiMath::Vector3 newPos, float newHeading)
{
	npcChars::npcChar* c = npcChars::getNpc(npc);
	c->startPos = newPos;
	c->currentPos = newPos;
	c->currentHeading = newHeading;
}

void apiNpc::addFootTask(int npc, apiMath::Vector3 gotoPos, apiMath::Vector3 aim, float gotoHeading, int anim, int weapon, int taskDuration)
{
	onFootInput in;

	in.endPos = gotoPos;

	in.endRot = gotoHeading;

	in.animation = anim;

	in.inputTime = taskDuration;
	//in.inputEnd = totalTime; will be defined at play task

	in.weapon = weapon;

	in.endAim = aim;
	in.bullets = 5;

	npcChars::getNpc(npc)->footInputs.push_back(in);
}

void apiNpc::addVehicleTask(int npc, int vehicleId, apiMath::Vector3 pos, apiMath::Vector3 rot,
	apiMath::Vector3 velo, int vehAnim, float pedals, float steer, int duration) {

	inVehicleInput in;
	in.breakOrGas = vehAnim;
	in.endPos = pos;
	in.endRot = rot;
	in.steer = steer;
	in.inputTime = duration;
	in.rpm = pedals;
	in.endSpeed = velo;

	npcChars::getNpc(npc)->vehicleInputs.push_back(in);
}

void apiNpc::playTasks(int npc, bool repeats)
{
	npcChars::npcChar* c = npcChars::getNpc(npc);
	c->done = false;
	c->repeat = repeats;
	c->isStatic = false;
	c->currentInput = 0;

	if(c->vehicle > 0)
	{
		size_t size = c->vehicleInputs.size();
		c->vehicleInputsSize = size;

		long cTime = clock() + c->vehicleInputs.at(0).inputTime;
		c->polBegin = cTime - c->vehicleInputs.at(0).inputTime;
		c->polEnd = cTime;
		c->startRot = c->vehicleInputs.at(0).endRot;
		c->startPos = c->vehicleInputs.at(0).endPos;
		c->lastRotationAlpha = 0;
		//LINFO << "cTime: " << clock();
		//LINFO << "Pol0 end: " << c->polEnd;
		c->vehicleInputs.at(0).inputEnd = c->polEnd;

		//LINFO << "First input end: " << cTime;

		for(size_t i = 1; i < size; i++)
		{
			cTime += c->vehicleInputs.at(i).inputTime;
			c->vehicleInputs.at(i).inputEnd = cTime;
			//LINFO << "Pol" << i << " end: " << cTime;
		}
		return;
	}

	size_t size = c->footInputs.size();
	c->footInputsSize = size;

	long cTime = clock() + c->footInputs.at(0).inputTime;
	c->polEnd = cTime;
	c->footInputs.at(0).inputEnd = c->polEnd;

	//LINFO << "First input end: " << cTime;

	for(size_t i = 1; i < size; i++)
	{
		cTime += c->footInputs.at(i).inputTime;
		c->footInputs.at(i).inputEnd = cTime;
	}
	//LINFO << "New NPC task: inputs: " << size << ", total time: " << cTime; 
}

bool apiNpc::tasksPlaying(int npc)
{
	return npcChars::getNpc(npc)->done;
}

void apiNpc::assignControl(int npc, int playerid)
{
	npcChars::getNpc(npc)->controllingClient = playerid;
}

void apiNpc::setStreamDis(int npc, float d)
{
	npcChars::getNpc(npc)->streamDis = d;
}

void sendNpcDataToStreamedInPlayer(int& npcId, unsigned int& world, RakNet::BitStream& b)
{
	RakNet::RakPeerInterface* con = networkManager::getConnection()->peer;
	for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
	{
		if(world == it->second.camWorld && it->second.sNpcs.find(npcId) != it->second.sNpcs.end())
		{
			con->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
		}
	}
}

void apiNpc::setClothes(int npc, unsigned int part, unsigned int value)
{
	npcChars::getNpc(npc)->clothes[part] = value;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_CLOTHES);
	bsOut.Write(npc);
	bsOut.Write(part);
	bsOut.Write(value);

	sendNpcDataToStreamedInPlayer(npc, npcChars::getNpc(npc)->vWorld, bsOut);
}

void apiNpc::setProperty(int npc, unsigned int part, unsigned value)
{
	npcChars::getNpc(npc)->props[part] = value;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_PROP_INDEX);
	bsOut.Write(npc);
	bsOut.Write(part);
	bsOut.Write(value);

	sendNpcDataToStreamedInPlayer(npc, npcChars::getNpc(npc)->vWorld, bsOut);
}