#include "npcManager.h"
#include "npcChar.h"
#include "players.h"
#include "vehicles.h"
#include "apiParamHelper.h"
#include <fstream>
//#include "../Server/apiNpcController.h"
#include "easylogging++.h"
#include <time.h>
#include "apiNpc.h"

#include "networkManager.h"
#include "packetsIds.h"

#if linux
#include <string.h>
#define strtok_s strtok_r
#endif

float pi = (float)3.14159265358979323846;
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

int npcIdIncrease = 1000;

apiNpc::tasksFinished apiFinished = 0;

float GetOffsetDegrees(float a, float b)
{
	float c = (b > a) ? b - a : 0.0f - (a - b);

	if(c > 180.0f)
		c = 0.0f - (360.0f - c);
	else if(c <= -180.0f)
		c = (360.0f + c);

	return c;
}

void split(std::vector<std::string>& words, char* s, const char* splitBy)
{
	char * pch = 0;
	char *next = 0;

	pch = strtok_s(s, splitBy, &next);
	while(pch != 0)
	{
		words.push_back(std::string(pch));
		pch = strtok_s(0, splitBy, &next);
	}
}

int createVehicleNpc(const char* nick, int skinModel, apiMath::Vector3 position, int vehicleId, const char* fileName, unsigned int vWorld)
{
	if(vehicleId == -1 || !vehicles::isVehicle(vehicleId))
	{
		//LINFO << "Invalid vehicle id " << vehicleId << " (" << (int)vehicles::isVehicle(vehicleId) << ")";
		mlog("Invalid vehicle id %i (%i)", vehicleId, (int)vehicles::isVehicle(vehicleId));
		return -1;
	}

	int id = npcIdIncrease;

	npcChars::npcChar* ch = new npcChars::npcChar;
	ch->currentPos = position;
	ch->vehicle = vehicleId;
	ch->model = skinModel;
	//ch->isStatic = true;
	ch->nick = nick;
	ch->currentInput = 0;
	ch->footInputsSize = 0;
	ch->lastRotationAlpha = 0.0f;
	ch->currentWeapon = 0;
	ch->vWorld = vWorld;
	ch->done = false;
	ch->repeat = true;
	ch->startPos = vehicles::getVehicle(vehicleId).position;
	ch->startRot = vehicles::getVehicle(vehicleId).rotation;
	ch->polBegin = clock();
	ch->isStatic = false;
	ch->controllingClient = -1;
	ch->streamDis = 200.0f;

	for(int i = 0; i < 9; i++) ch->clothes[i] = 0;
	for(int i = 0; i < 2; i++) ch->props[i] = -1;

	unsigned int totalTime = ch->polBegin;

	if(fileName && fileName[0])
	{
		std::ifstream file(fileName);
		if(!file.is_open())
		{
			mlog("Unable to open %s", fileName);
			return -1;
		}

		std::string line;
		std::vector<std::string> params;

		vehicles::netVehicle v = vehicles::getVehicle(vehicleId);
		ch->currentPos = v.position;

		//apiMath::Vector3 lastPos = v.position;

		//apiMath::Quaternion lastRot = v.rotation;

		//apiMath::Vector3 lastSpeed = v.velocity;
		bool firstInput = true;

		int lineCount = 1;

		while(!file.eof()) 
		{	
			std::getline(file, line);
			if(line.empty()) continue;
			split(params, (char*)line.c_str(), " ");

			if(params.size() != 17)
			{
				file.close();
				//LINFO << "Vehicle input size is " << params.size() << " @" << lineCount << ": " << line;
				mlog("Vehicle input size is %u @%i: %i", params.size(), lineCount, line);
				return -1;
			}
			
			lineCount++;

			//LINFO << "b";

			inVehicleInput in;
			//apiMath::Vector3 qtBuf;

			//in.startPos = lastPos;
			//in.startRot = lastRot;
			//in.startSpeed = lastSpeed;

			in.endPos = apiMath::Vector3(apiParamHelper::isFloat(params.at(0).c_str()), apiParamHelper::isFloat(params.at(1).c_str()), 
				apiParamHelper::isFloat(params.at(2).c_str()));
			//lastPos = in.endPos;

			apiMath::Vector3 qtBuf = apiMath::Vector3(apiParamHelper::isFloat(params.at(3).c_str()), apiParamHelper::isFloat(params.at(4).c_str()),
				apiParamHelper::isFloat(params.at(5).c_str())/*, apiParamHelper::isFloat(params.at(6).c_str())*/);
			in.endRot = qtBuf;
			//lastRot = in.endRot;

			in.inputTime = apiParamHelper::isInt(params.at(7).c_str());
			totalTime += in.inputTime;
			in.inputEnd = totalTime;

			in.steer = apiParamHelper::isFloat(params.at(8).c_str());
			//in.gasPedal = apiParamHelper::isFloat(params.at(9).c_str());

			in.breakOrGas = apiParamHelper::isInt(params.at(9).c_str());

			//in.gear = apiParamHelper::isInt(params.at(10).c_str());
			in.rpm = apiParamHelper::isFloat(params.at(10).c_str());

			in.endSpeed.x = apiParamHelper::isFloat(params.at(11).c_str());
			in.endSpeed.y = apiParamHelper::isFloat(params.at(12).c_str());
			in.endSpeed.z = apiParamHelper::isFloat(params.at(13).c_str());

			in.endTurnSpeed.x = apiParamHelper::isFloat(params.at(14).c_str());
			in.endTurnSpeed.y = apiParamHelper::isFloat(params.at(15).c_str());
			in.endTurnSpeed.z = apiParamHelper::isFloat(params.at(16).c_str());
			//lastSpeed = in.endSpeed;

			firstInput = false;

			ch->vehicleInputs.push_back(in);

			line.clear();
			params.clear();
		}
		file.close();
	}

	/*ch->apiModuleHandle = new apiNpcChar::npcChar;
	ch->apiModuleHandle->id = id;
	ch->apiModuleHandle->model = ch->model;
	ch->apiModuleHandle->position = ch->currentPos;
	ch->apiModuleHandle->repeatExecution = true;
	ch->apiModuleHandle->vehicle = vehicleId;*/

	if(vehicleId != -1)
	{
		ch->vehicleInputsSize = ch->vehicleInputs.size();
		vehicles::getVehicle(vehicleId).driverId = id;

		if(ch->vehicleInputsSize >= 1)
		{
			ch->polEnd = clock() + ch->vehicleInputs.at(0).inputTime;
			ch->isStatic = false;
		}
	}

	//playerIdManager::reserveId(id, true);
	mlog("Is ID %i successfully", id);

	npcChars::addNpc(id, ch);
	npcIdIncrease++;

	//LINFO << "Npc created with id " << id << ", inputs: " << ch->vehicleInputsSize << ", execution time is: " << totalTime / 1000 << " seconds";
	mlog("Npc created with id %i, inputs: %u, execution time is: %i seconds", id, ch->vehicleInputsSize, (int)(totalTime / 1000));

	return id;
}

int createWalkingNpc(const char* nick, int skinModel, apiMath::Vector3 position, const char* fileName, unsigned int vWorld, float heading)
{
	int id = npcIdIncrease;

	npcChars::npcChar* ch = new npcChars::npcChar;
	ch->currentPos = position;
	ch->vehicle = -1;
	ch->model = skinModel;
	ch->isStatic = true;
	ch->nick = nick;
	ch->currentInput = 0;
	ch->vehicleInputsSize = 0;
	ch->currentWeapon = 0;
	ch->vWorld = vWorld;
	ch->repeat = false;
	ch->done = true;
	ch->controllingClient = -1;
	ch->streamDis = 200.0f;
	ch->currentHeading = heading;
	for(int i = 0; i < 9; i++) ch->clothes[i] = 0;
	for(int i = 0; i < 2; i++) ch->props[i] = -1;

	unsigned int totalTime = 0;

	if(fileName && fileName[0])
	{
		std::ifstream file(fileName);
		if(!file.is_open())
		{
			mlog("Unable to open %s", fileName);
			return -1;
		}

		std::string line;
		std::vector<std::string> params;

		ch->currentPos = position;

		bool firstInput = true;
		int lineCount = 1;

		while(!file.eof()) 
		{	
			std::getline(file, line);
			if(line.empty()) continue;
			split(params, (char*)line.c_str(), " ");
			size_t paramSize = params.size();
			if(paramSize != 7 && paramSize != 11)
			{
				file.close();
				//LINFO << "Foot input size is " << params.size() << " @" << lineCount << ": " << line;
				mlog("Foot input size is %u @%i: %i", params.size(), lineCount, line);
				return -1;
			}
			
			lineCount++;

			onFootInput in;

			in.endPos = apiMath::Vector3(apiParamHelper::isFloat(params.at(0).c_str()), apiParamHelper::isFloat(params.at(1).c_str()), 
				apiParamHelper::isFloat(params.at(2).c_str()));

			in.endRot = apiParamHelper::isFloat(params.at(3).c_str());

			in.animation = apiParamHelper::isInt(params.at(4).c_str());

			in.inputTime = apiParamHelper::isInt(params.at(5).c_str());
			totalTime += in.inputTime;
			in.inputEnd = totalTime;

			firstInput = false;

			in.weapon = apiParamHelper::isInt(params.at(6).c_str());

			if(paramSize == 11)
			{
				in.endAim = apiMath::Vector3(apiParamHelper::isFloat(params.at(7).c_str()), apiParamHelper::isFloat(params.at(8).c_str()), 
				apiParamHelper::isFloat(params.at(9).c_str()));
				
				in.bullets = apiParamHelper::isInt(params.at(10).c_str());

				//LINFO << in.endAim.x << ", " << in.endAim.y << ", " << in.endAim.z;
			}

			ch->footInputs.push_back(in);

			line.clear();
			params.clear();
		}
		ch->repeat = true;
		file.close();
	}

	/*ch->apiModuleHandle = new apiNpcChar::npcChar;
	ch->apiModuleHandle->id = id;
	ch->apiModuleHandle->model = ch->model;
	ch->apiModuleHandle->position = ch->currentPos;
	ch->apiModuleHandle->repeatExecution = true;
	ch->apiModuleHandle->vehicle = -1;*/

	ch->footInputsSize = ch->footInputs.size();
	if(ch->footInputsSize >= 1)
	{
		ch->polEnd = clock() + ch->footInputs.at(0).inputTime;
		ch->isStatic = false;
		ch->done = false;
	}

	//playerIdManager::reserveId(id, true);
	npcIdIncrease++;
	mlog("Is ID %i successfully", id);

	npcChars::addNpc(id, ch);

	//LINFO << "Foot Npc created with id " << id << ", execution time is: " << totalTime / 1000 << " seconds";
	mlog("Foot Npc created with id %i, execution time is: %i seconds", id, (int)(totalTime / 1000));

	return id;
}

int apiNpc::createNpc(const char* nick, int skinModel, apiMath::Vector3 position, int vehicleId, const char* fileName, unsigned int vWorld, float heading)
{
	if(vehicleId != -1)
	{
		return createVehicleNpc(nick, skinModel, position, vehicleId, fileName, vWorld);
	}
	return createWalkingNpc(nick, skinModel, position, fileName, vWorld, heading);
}

void apiNpc::deleteNpc(int npcId)
{
	if(npcChars::isNpc(npcId))
	{
		npcChars::removeNpc(npcId);

		RakNet::BitStream bsOut;
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(PLAYER_LEFT_TO_CLIENT);
		bsOut.Write(npcId);

		for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
		{
			networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
		}
	}
}

bool apiNpc::isValid(int npcId)
{
	return npcChars::isNpc(npcId);
}

void npcManager::interpolateNpcs(long time)
{
	for(std::map<int, npcChars::npcChar*>::iterator i = npcChars::getNpcBegin(); i != npcChars::getNpcEnd(); ++i)
	{
		if(i->second->isStatic || i->second->done)
		{
			continue;
		}
		else if(i->second->vehicle != -1)
		{
			if(i->second->vehicleInputsSize == 0 || !vehicles::isVehicle(i->second->vehicle))
			{
				//LINFO << "NPC " << i->first << " vehicle (" << i->second->vehicle << ") wasnt found";
				mlog("NPC %i vehicle (%i) wasnt found", i->first, i->second->vehicle);
				i->second->vehicle = -1;
				i->second->isStatic = true;
				continue;
			}

			vehicles::netVehicle& veh = vehicles::getVehicle(i->second->vehicle);
			if(time >= i->second->polEnd)
			{
				bool resetPol = true;

				for(size_t it = i->second->currentInput; it < i->second->vehicleInputsSize; ++it)
				{
					if(i->second->vehicleInputs.at(it).inputEnd > time)
					{
						i->second->currentInput = it;
						resetPol = false;
						break;
					}
				}
				//LINFO << "NPC " << i->first << " input changed to " << i->second->currentInput << 
					//". Ends at " << i->second->vehicleInputs.at(i->second->currentInput).inputEnd;

				if(resetPol)
				{
					if(apiFinished != 0)
					{
						apiFinished(i->first, i->second->repeat);
					}
					if(!i->second->repeat)
					{
						i->second->done = true;
						continue;
					}
					//LINFO << "NPC " << i->first << " resetting pol, current input: " << i->second->currentInput;
					long timeCount = time;
					for(size_t it = 0; it < i->second->vehicleInputsSize; ++it)
					{
						timeCount += i->second->vehicleInputs.at(it).inputTime;
						i->second->vehicleInputs.at(it).inputEnd = timeCount;
						//LINFO << it << " now ends at " << i->second->vehicleInputs.at(it).inputEnd;
					}
					i->second->currentInput = 0;
				}
				inVehicleInput& in = i->second->vehicleInputs.at(i->second->currentInput);
				if(!i->second->repeat)
				{
					i->second->polBegin = time;
					i->second->polEnd = in.inputEnd;
				}
				else
				{
					i->second->polEnd = time + in.inputTime;
				}
				i->second->startPos = veh.position;
				i->second->startRot = veh.rotation;
				i->second->startSpeed = veh.velocity;
				i->second->lastRotationAlpha = 0.0f;
			}

			float progress = i->second->repeat ? (time >= i->second->polEnd ? 1.0f : (float)time / i->second->polEnd) :
				(time >= i->second->polEnd ? 1.0f : (float)(time - i->second->polBegin) / (i->second->polEnd - i->second->polBegin));

			//LINFO << i->second->currentInput << ": " << i->second->polBegin << ", " << i->second->polEnd << ", " << time << ", " << progress;

			inVehicleInput& in = i->second->vehicleInputs.at(i->second->currentInput);
			veh.position = apiMath::vecLerp(i->second->startPos, in.endPos, progress);

			//apiMath::slerp(veh.rotation, i->second->startRot, in.endRot, progress);

			/*i->second->rotError = apiMath::Vector3(GetOffsetDegrees(i->second->startRot.x, in.endRot.x),
				GetOffsetDegrees(i->second->startRot.y, in.endRot.y), GetOffsetDegrees(i->second->startRot.z, in.endRot.z));

			float currentAlpha = progress - i->second->lastRotationAlpha;
			apiMath::Vector3 add = apiMath::vecLerp(apiMath::Vector3(), i->second->rotError, currentAlpha);
			veh.rotation.x = veh.rotation.x + add.x;
			veh.rotation.y = veh.rotation.y + add.y;
			veh.rotation.z = veh.rotation.z + add.z;*/
			veh.rotation = in.endRot;

			//LINFO << i->second->lastRotationAlpha << ", " << currentAlpha << ": " << add.x 
				//<< ", " << add.y << ", " << add.z;
			i->second->lastRotationAlpha = progress;
			veh.rpm = in.rpm;
			//veh.gear = i->second->vehicleInputs.at(i->second->currentInput).gear;

			veh.velocity = apiMath::vecLerp(i->second->startSpeed, in.endSpeed, progress);

			veh.turnSpeed = apiMath::vecLerp(i->second->startTurnSpeed, in.endTurnSpeed, progress);

			//veh.action = i->second->vehicleInputs.at(i->second->currentInput).animation;

			veh.steer = in.steer;
			//veh.gasPedal = in.gasPedal;
			veh.breakOrGas = in.breakOrGas;

			i->second->currentPos = veh.position;

			//LINFO << progress << ": " << i->second->currentPos.x << ", " << i->second->currentPos.y << ", " << i->second->currentPos.z;
		}
		else
		{
			//LINFO << "pulse";
			if(time >= i->second->polEnd)
			{
				bool resetPol = true;
				//LINFO << "Searching new task";

				for(size_t it = i->second->currentInput + 1; it < i->second->footInputsSize; ++it)
				{
					//LINFO << it;
					if(i->second->footInputs.at(it).inputEnd > time)
					{
						//LINFO << "NPC " << i->first << " input changed to " << it << ". Ends at " << i->second->footInputs.at(it).inputEnd;
						i->second->currentInput = it;
						resetPol = false;
						break;
					};
				}

				if(resetPol)
				{
					if(apiFinished != 0)
					{
						apiFinished(i->first, i->second->repeat);
					}
					if(!i->second->repeat)
					{
						i->second->done = true;
						i->second->currentAnimation = 0;
						continue;
					}
					//LINFO << "NPC " << i->first << " resetting pol, current input: " << i->second->currentInput << ", total: " << i->second->footInputsSize;
					long timeCount = time;// + 2000;
					for(size_t it = 0; it < i->second->footInputsSize; ++it)
					{
						timeCount += i->second->footInputs.at(it).inputTime;
						i->second->footInputs.at(it).inputEnd = timeCount;
						//LINFO << it << " now ends at " << i->second->footInputs.at(it).inputEnd;
					}
					//LINFO << "NPC " << i->first << " resetting pol, ends in: " << timeCount - time;
					i->second->currentInput = 0;
				}

				i->second->polEnd = time + i->second->footInputs.at(i->second->currentInput).inputTime;
				i->second->startPos = i->second->currentPos;
				i->second->startRot.x = i->second->currentHeading;
			}

			float progress = time >= i->second->polEnd ? (float)1.0 : (float)time / i->second->polEnd;

			i->second->currentPos = apiMath::vecLerp(i->second->startPos, 
				i->second->footInputs.at(i->second->currentInput).endPos, progress);

			float dif = i->second->footInputs.at(i->second->currentInput).endRot - i->second->startRot.x;
			int buf2 = sgn(dif);
			float buf = std::abs(dif);

			if(buf > pi)
			{
				dif = buf2 * (2 * pi - buf) * -1;
			}

			i->second->currentHeading = i->second->footInputs.at(i->second->currentInput).endRot;
			//LINFO << i->second->currentHeading;

			i->second->currentAnimation = i->second->footInputs.at(i->second->currentInput).animation;

			if(i->second->currentWeapon != i->second->footInputs.at(i->second->currentInput).weapon)
			{
				i->second->currentWeapon = i->second->footInputs.at(i->second->currentInput).weapon;

				RakNet::BitStream bsOut;
				bsOut.Write((MessageID)IVMP);
				bsOut.Write(WEAPON_CHANGE);
				bsOut.Write(i->first);
				bsOut.Write(i->second->currentWeapon);
				//bsOut.Write(i->second->footInputs.at(i->second->currentInput).bullets);

				//LINFO << "NPC " << i->first << " new weapon: " << i->second->currentWeapon;
							
				for(std::map<uint64_t, players::player>::iterator it = players::getPlayersBegin(); it != players::getPlayersEnd(); ++it)
				{
					if(it->second.camWorld == i->second->vWorld && it->second.sNpcs.find(i->first) != it->second.sNpcs.end())
					{
						networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, it->second.ip, false);
					}
				}
			}
		}
	}
}

void apiNpc::registerTasksFinished(apiNpc::tasksFinished f)
{
	apiFinished = f;
}