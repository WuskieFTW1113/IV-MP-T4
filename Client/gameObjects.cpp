#include "gameObjects.h"
#include "gameWorld.h"
#include <map>
#include "simpleMath.h"
#include "../ClientManager/ivScripting.h"
#include "easylogging++.h"
#include "CustomFiberThread.h"
#include "../SharedDefines/packetsIds.h"
#include "modelsHandler.h"

/*
	LINFO << "DETECTING OBJECTS";

	for(DWORD i = 0; i < 100000; ++i)
	{
		if(NativeInvoke::Invoke<bool>("DOES_OBJECT_EXIST", i) && NativeInvoke::Invoke<bool>("HAS_OBJECT_BEEN_DAMAGED_BY_CHAR", i, p.ped))
		{
			LINFO << "Damaged";
			float x, y, z;
			NativeInvoke::Invoke<u32>("GET_OBJECT_COORDINATES", i, &x, &y, &z);
			unsigned int model = 0;
			NativeInvoke::Invoke<u32>("GET_OBJECT_MODEL", i, &model);
			LINFO << "Model " << model << ": " << x << "f, " << y << "f, " << z << "f";
		}
	}
	LINFO << "Finished";
*/

struct gObject
{
	simpleMath::Vector3 pos;
	simpleMath::Quaternion rot;
	unsigned int model;
	Scripting::Object obj;
	int hasOffset;
	unsigned int vWorld;
	float streamDistance;

	int alpha;
	unsigned int interior;

	int changePosStatus; //0 - nothing, 1 - updates
};

struct gObjectAudio
{
	std::string name;
	long offSet;
	bool started;
};

//gObject isnt inside gObject because it arrives on different packages with minimum stream reliability. Must persist until the object is deleted
struct gObjectMove
{
	long polStart;
	long polEnd;
	long lastPol;

	simpleMath::Vector3 startPos;
	simpleMath::Quaternion startRot;

	simpleMath::Vector3 endPos;
	simpleMath::Quaternion endRot;
};

std::map<int, gObject> objs;
std::map<int, gObject> streamedObjs;
std::map<int, gObjectAudio> objAudio;
std::map<int, gObjectMove> objMove;
long lastObjectDistanceCheck = 0;

void gameObjects::setObjectAudio(RakNet::BitStream& bsIn)
{
	int obj = -1;
	bsIn.Read(obj);
	//LINFO << "Object audio: " << obj;
	if(objAudio.find(obj) != objAudio.end())
	{
		return;
	}

	gObjectAudio a;
	RakNet::RakString s;
	bsIn.Read(s);
	a.name = s;
	//a.offSet = 50000;
	bsIn.Read(a.offSet);
	a.started = false;
	objAudio.insert(std::make_pair(obj, a));
}

void nowSpawnObject(int id, gObject& o)
{
	if(!modelsHandler::wasRequested(o.model)) return modelsHandler::request(o.model, clock());
	if(!Scripting::HasModelLoaded(o.model)) return;
	
	if(o.hasOffset)
	{
		Scripting::CreateObject(o.model, o.pos.x, o.pos.y, o.pos.z, &o.obj, true);
	}
	else
	{
		Scripting::CreateObjectNoOffset(o.model, o.pos.x, o.pos.y, o.pos.z, &o.obj, true);
	}

	if(Scripting::DoesObjectExist(o.obj))
	{
		Scripting::FreezeObjectPosition(o.obj, true);
		Scripting::SetObjectQuaternion(o.obj, o.rot.X, o.rot.Y, o.rot.Z, o.rot.W);
		if(o.interior != 0)
		{
			Scripting::AddObjectToInteriorRoomByKey(o.obj, o.interior);
		}

		RakNet::BitStream bsIn;
		bsIn.Write((MessageID)IVMP);
		bsIn.Write(OBJECT_CREATE);
		bsIn.Write(id);

		networkManager::connection* con = networkManager::getConnection();
		con->peer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);
		streamedObjs[id] = o;
		//LINFO << "Done";
	}
}

void nowDestroyObject(int id, gObject& o)
{
	Scripting::DeleteObject(&o.obj);
	
	RakNet::BitStream bsIn;
	bsIn.Write((MessageID)IVMP);
	bsIn.Write(OBJECT_DELETE);
	bsIn.Write(id);

	networkManager::connection* con = networkManager::getConnection();
	con->peer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);

	if(streamedObjs.find(id) != streamedObjs.end()) streamedObjs.erase(id);
}

void gameObjects::spawnObject(RakNet::BitStream& bsIn)
{
	gObject o;
	int id = -1;
	bsIn.Read(id);

	if(id < 0 || id > 1000000) //crazy id
	{
		return;
	}
	else if(objs.find(id) != objs.end())
	{
		LINFO << "!Warning: obj " << id << " spawn blocked due to duplification";
		return;
	}

	//LINFO << "Reading obj " << id;

	bsIn.Read(o.vWorld);
	bsIn.Read(o.streamDistance);
	bsIn.Read(o.model);
	
	bsIn.Read(o.pos.x);
	bsIn.Read(o.pos.y);
	bsIn.Read(o.pos.z);
	bsIn.Read(o.rot.X);
	bsIn.Read(o.rot.Y);
	bsIn.Read(o.rot.Z);
	bsIn.Read(o.rot.W);

	//aditional
	bsIn.Read(o.hasOffset);
	bsIn.Read(o.interior);
	bsIn.Read(o.alpha);
	o.changePosStatus = 0;

	objs.insert(std::make_pair(id, o));
}

void gameObjects::deleteObject(RakNet::BitStream& bsIn)
{
	int id = -1;
	bsIn.Read(id);

	if(objAudio.find(id) != objAudio.end())
	{
		objAudio.erase(id);
		//NativeInvoke::Invoke<u32>("STOP_STREAM");
	}

	if(objs.find(id) != objs.end())
	{		
		//LINFO << "Deleting obj: " << id;
		gObject& o = objs.at(id);
		if(Scripting::DoesObjectExist(o.obj)) nowDestroyObject(id, o);
		objs.erase(id);
	}
}

void gameObjects::updateStreamDistance(RakNet::BitStream& bsIn)
{
	try
	{
		int id = -1;
		bsIn.Read(id);

		float dis;
		bsIn.Read(dis);
		objs.at(id).streamDistance = dis;
	}
	catch(std::exception&){}
}

void gameObjects::wipeObjects()
{
	LINFO << "Wiping objcts";
	lastObjectDistanceCheck = 0;

	for(std::map<int, gObject>::iterator i = objs.begin(); i != objs.end(); ++i)
	{
		if(Scripting::DoesObjectExist(i->second.obj))
		{
			Scripting::DeleteObject(&i->second.obj);
			//LINFO << "Wiping obj: " << i->first;
			//NativeInvoke::Invoke<u32>("DELETE_OBJECT", &i->second.obj);
		}
	}
	streamedObjs.clear();
	//LINFO << "Done";
}

void gameObjects::moveObject(RakNet::BitStream& bsIn)
{
	int id = -1;
	bsIn.Read(id);

	//Must not check if the object id exists

	gObjectMove o;

	bsIn.Read(o.startPos.x);
	bsIn.Read(o.startPos.y);
	bsIn.Read(o.startPos.z);
	bsIn.Read(o.endPos.x);
	bsIn.Read(o.endPos.y);
	bsIn.Read(o.endPos.z);

	bsIn.Read(o.startRot.X);
	bsIn.Read(o.startRot.Y);
	bsIn.Read(o.startRot.Z);
	bsIn.Read(o.startRot.W);
	bsIn.Read(o.endRot.X);
	bsIn.Read(o.endRot.Y);
	bsIn.Read(o.endRot.Z);
	bsIn.Read(o.endRot.W);

	networkManager::connection* con = networkManager::getConnection();
	bsIn.Read(o.polStart);
	bsIn.Read(o.polEnd);

	o.lastPol = con->cTime;
	objMove[id] = o;
}

void gameObjects::pulseObjects(long cTime)
{
	executingTask = 4;
	
	if(cTime > lastObjectDistanceCheck)
	{
		lastObjectDistanceCheck = cTime + 2000;
		simpleMath::Vector3& pos = players::getLocalPlayer().pos;
		std::map<int, gObject>::iterator i = objs.begin(), objsEnd = objs.end();

		for(i; i != objsEnd; ++i)
		{
			if(i->second.vWorld != 0 && i->second.vWorld != gameWorld::getCurrentWorld()) continue;

			float distance = simpleMath::vecDistance(pos, i->second.pos);
			bool exist = Scripting::DoesObjectExist(i->second.obj);
			if(distance < i->second.streamDistance)
			{
				if(!exist) nowSpawnObject(i->first, i->second);
			}
			else if(exist) nowDestroyObject(i->first, i->second);
		}
	}

	std::map<int, gObjectMove>::iterator moveObj = objMove.begin(), moveObjEnd = objMove.end();
	while(moveObj != moveObjEnd)
	{
		gObject* o = objs.find(moveObj->first) != objs.end() ? &objs.at(moveObj->first) : NULL;
		if (!o)
		{
			++moveObj;
			continue;
		}

		//Update the object whether its spawned or not. It may go in streaming range!
		//If spawned then change its position

		float progress = cTime >= moveObj->second.polEnd ? progress = 1.0f : 
			(float)(cTime - moveObj->second.polStart) / (moveObj->second.polEnd - moveObj->second.polStart);
	
		o->pos = simpleMath::vecLerp(moveObj->second.startPos, moveObj->second.endPos, progress);
		simpleMath::slerp(o->rot, moveObj->second.startRot, moveObj->second.endRot, progress);

		gObject* spawned = streamedObjs.find(moveObj->first) != streamedObjs.end() ? &streamedObjs.at(moveObj->first) : NULL;
		if (spawned)
		{
			spawned->changePosStatus = 1;
			spawned->pos = o->pos;
			spawned->rot = o->rot;
		}

		if (progress == 1.0f)
		{
			objMove.erase(moveObj++);
			continue;
		}

		//LINFO << "(" << cTime << ") " << progress << "->" << o->pos.x << ", " << o->pos.y << ", " << o->pos.z;
		++moveObj;
	}
	

	std::map<int, gObject>::iterator i = streamedObjs.begin(), objsEnd = streamedObjs.end();
	for(i; i != objsEnd; ++i)
	{
		if(!Scripting::DoesObjectExist(i->second.obj)) continue;

		subTask = 1;

		if(i->second.alpha != 255)
		{
			//NativeInvoke::Invoke<u32>("SET_OBJECT_ALPHA", i->second.obj.Get(), i->second.alpha);
			Scripting::SetObjectAlpha(i->second.obj, i->second.alpha);
		}

		if(objAudio.find(i->first) != objAudio.end())
		{
			gObjectAudio& go = objAudio.at(i->first);
			if(!go.started)
			{
				/*if(NativeInvoke::Invoke<bool>("PRELOAD_STREAM_WITH_START_OFFSET", go.name.c_str(), go.offSet))
				{
					NativeInvoke::Invoke<u32>("PLAY_STREAM_FROM_OBJECT", i->second.obj.Get());
					go.started;
				}*/
			}
		}
		if(i->second.changePosStatus)
		{
			i->second.changePosStatus = 0;

			Scripting::SetObjectCoordinates(i->second.obj, i->second.pos.x, i->second.pos.y, i->second.pos.z);
			Scripting::SetObjectQuaternion(i->second.obj, i->second.rot.X, i->second.rot.Y, i->second.rot.Z, i->second.rot.W);
		}
	}
	executingTask = 0;
	subTask = 0;
}