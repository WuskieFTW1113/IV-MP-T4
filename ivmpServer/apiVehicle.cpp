#include "apiVehicle.h"
#include "vehicles.h"
#include "vehicleList.h"
#include "players.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "easylogging++.h"
#include "entitySpawned.h"

std::map<int, apiVehicle::vehicle> apiVehicleMap;

void streamVehicleFunction(RakNet::BitStream& vOut, int id, unsigned int vWorld)
{
	networkManager::connection* con = networkManager::getConnection();
	for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		if(i->second.vWorld == vWorld && i->second.sVehicles.find(id) != i->second.sVehicles.end())
		{
			con->peer->Send(&vOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
		}
	}
}

apiVehicle::vehicle::~vehicle(){}
apiVehicle::vehicle::vehicle(int id)
{
	this->id = id;
}

int apiVehicle::vehicle::getId()
{
	return id;
}

int apiVehicle::vehicle::getDriver()
{
	return vehicles::getVehicle(id).driverId;
}

void apiVehicle::vehicle::setTune(unsigned int part, bool on)
{
	vehicles::netVehicle& v = vehicles::getVehicle(id);

	vehicles::getVehicle(id).tunes[part] = on;

	RakNet::BitStream vOut;
	vOut.Write((MessageID)IVMP);
	vOut.Write(VEHICLE_TUNES);
	vOut.Write(id);
	vOut.Write(part);
	vOut.Write(on ? 1 : 0);

	streamVehicleFunction(vOut, this->id, v.vWorld);
}

bool apiVehicle::vehicle::isTuned(unsigned int part)
{
	return vehicles::getVehicle(id).tunes[part];
}

void apiVehicle::vehicle::setColor(int color1, int color2, int color3, int color4)
{
	vehicles::netVehicle& v = vehicles::getVehicle(id);

	v.colors[0] = color1;
	v.colors[1] = color2;
	v.colors[2] = color3;
	v.colors[3] = color4;

	RakNet::BitStream vOut;
	vOut.Write((MessageID)IVMP);
	vOut.Write(VEHICLE_COLOR_CHANGED);
	vOut.Write(id);
	for(unsigned int i = 0; i < 4; i++)
	{
		vOut.Write(v.colors[i]);
	}

	streamVehicleFunction(vOut, this->id, v.vWorld);
}

int apiVehicle::vehicle::getColor(unsigned int slotId)
{
	return vehicles::getVehicle(id).colors[slotId];
}

void apiVehicle::vehicle::setIndicator(unsigned int indicator, bool onOrOff)
{
	RakNet::BitStream vOut;
	vOut.Write((MessageID)IVMP);
	vOut.Write(INDICATOR_STATE_CHANGED);
	vOut.Write(this->id);
	vOut.Write(indicator);
	vOut.Write(onOrOff ? 1 : 0);

	vehicles::netVehicle& v = vehicles::getVehicle(this->id);
	v.indicators[indicator] = onOrOff;
	if(indicator == 0 && v.indicators[1])
	{
		v.indicators[1] = false;
	}
	else if(indicator == 1 && v.indicators[0])
	{
		v.indicators[0] = false;
	}

	streamVehicleFunction(vOut, this->id, v.vWorld);
}

bool apiVehicle::vehicle::getIndicator(unsigned int indicator)
{
	return vehicles::getVehicle(this->id).indicators[indicator];
}

void apiVehicle::vehicle::setPosition(apiMath::Vector3& v)
{
	vehicles::getVehicle(this->id).position = v;

	RakNet::BitStream vOut;
	vOut.Write((MessageID)IVMP);
	vOut.Write(VEHICLE_FORCE_COORDINATES);
	vOut.Write(this->id);
	vOut.Write(v.x);
	vOut.Write(v.y);
	vOut.Write(v.z);

	streamVehicleFunction(vOut, this->id, vehicles::getVehicle(this->id).vWorld);
}

void apiVehicle::vehicle::setRotation(apiMath::Vector3& v)
{
	vehicles::getVehicle(this->id).rotation = v;

	RakNet::BitStream vOut;
	vOut.Write((MessageID)IVMP);
	vOut.Write(VEHICLE_FORCE_ROTATION);
	vOut.Write(this->id);
	vOut.Write(v.x);
	vOut.Write(v.y);
	vOut.Write(v.z);
	//vOut.Write(v.W);

	streamVehicleFunction(vOut, this->id, vehicles::getVehicle(this->id).vWorld);
}

void apiVehicle::vehicle::setVelocity(apiMath::Vector3& v)
{
	vehicles::getVehicle(this->id).velocity = v;

	RakNet::BitStream vOut;
	vOut.Write((MessageID)IVMP);
	vOut.Write(VEHICLE_FORCE_VELOCITY);
	vOut.Write(this->id);
	vOut.Write(v.x);
	vOut.Write(v.y);
	vOut.Write(v.z);

	streamVehicleFunction(vOut, this->id, vehicles::getVehicle(this->id).vWorld);
}

void apiVehicle::vehicle::setLivery(int livery)
{
	vehicles::netVehicle& v = vehicles::getVehicle(id);
	v.livery = livery;

	RakNet::BitStream vOut;
	vOut.Write((MessageID)IVMP);
	vOut.Write(VEHICLE_LIVERY);
	vOut.Write(id);
	vOut.Write(livery);

	streamVehicleFunction(vOut, this->id, v.vWorld);
}

void apiVehicle::vehicle::setDirtLevel(float level)
{
	vehicles::netVehicle& v = vehicles::getVehicle(id);
	v.dirt = level;

	RakNet::BitStream vOut;
	vOut.Write((MessageID)IVMP);
	vOut.Write(VEHICLE_DIRT_LEVEL);
	vOut.Write(id);
	vOut.Write(level);

	streamVehicleFunction(vOut, this->id, v.vWorld);
}

int apiVehicle::vehicle::getEngineHealth()
{
	return vehicles::getVehicle(id).engineHealth;
}

void apiVehicle::vehicle::setEngineHealth(int hp, bool fixVisual)
{
	vehicles::netVehicle& v = vehicles::getVehicle(id);
	v.engineHealth = hp;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(CAR_HP_UPDATE);
	bsOut.Write(id);
	bsOut.Write(hp);
	if(fixVisual)
	{
		bsOut.Write(1);
		for(unsigned int i = 0; i < 6; i++)
		{
			v.tyrePopped[i] = false;
		}
	}
	streamVehicleFunction(bsOut, this->id, v.vWorld);
}

bool apiVehicle::vehicle::getTire(unsigned int index)
{
	return vehicles::getVehicle(id).tyrePopped[index];
}

void apiVehicle::vehicle::setTire(unsigned int index, bool status)
{
	vehicles::netVehicle& v = vehicles::getVehicle(id);
	v.tyrePopped[index] = status;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);	
	bsOut.Write(SINGLE_TYRE_POPPED);
	bsOut.Write(id);
	bsOut.Write(index);
	bsOut.Write((int)status);
	streamVehicleFunction(bsOut, this->id, v.vWorld);
}

float apiVehicle::vehicle::getGasPedal()
{
	return vehicles::getVehicle(id).rpm;
}

float apiVehicle::vehicle::getSteering()
{
	return vehicles::getVehicle(id).steer;
}

int apiVehicle::vehicle::getBreakOrGas()
{
	return vehicles::getVehicle(id).breakOrGas;
}

void apiVehicle::vehicle::setEngineFlags(int flags)
{
	vehicles::netVehicle& v = vehicles::getVehicle(id);
	v.engine = flags;
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(VEHICLE_ENGINE_FLAGS);
	bsOut.Write(id);
	bsOut.Write(flags);
	streamVehicleFunction(bsOut, this->id, v.vWorld);
}

int apiVehicle::vehicle::getEngineFlags()
{
	return vehicles::getVehicle(id).engine;
}

void apiVehicle::vehicle::setMemBit(unsigned int offset, int dataType, float value)
{
	vehicles::netVehicle& v = vehicles::getVehicle(id);
	if(value < -1000.0f && v.membits.find(offset) != v.membits.end())
	{
		v.membits.erase(offset);
	}
	else if(dataType != -1)
	{
		vehicles::vMem buf;
		buf.mType = dataType;
		buf.value = value;
		v.membits[offset] = buf;
	}
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(VEHICLE_MEMBIT);
	bsOut.Write(id);
	bsOut.Write(offset);
	bsOut.Write(dataType);
	bsOut.Write(value);
	streamVehicleFunction(bsOut, this->id, v.vWorld);
}

void apiVehicle::vehicle::setTrunkHood(bool trunk, bool open)
{
	vehicles::netVehicle& v = vehicles::getVehicle(id);
	if(trunk) v.trunk = open;
	else v.hood = open;

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(VEHICLE_HOOD_TRUNK);
	bsOut.Write(id);
	bsOut.Write(v.hood);
	bsOut.Write(v.trunk);
	streamVehicleFunction(bsOut, this->id, v.vWorld);
}

bool apiVehicle::vehicle::getTrunkHood(bool trunk)
{
	vehicles::netVehicle& v = vehicles::getVehicle(id);
	if(trunk) return v.trunk;
	return v.hood;
}

void aVehicle_setDefaultTunes(vehicles::netVehicle& v)
{
	size_t s = 0;
	std::vector<int> buf;
	vehicleList::getDefaultTune(v.model, s, buf);
	if(s != 0)
	{
		for(size_t i = 0; i < s; i++)
		{
			if(buf[i] > 0 && buf[i] < 10) v.tunes[buf[i]] = true;
		}
	}
}

bool defaultTunes = true;
void apiVehicle::toggleAutoComponents(bool b)
{
	defaultTunes = b;
}

int apiVehicle::addVehicle(int mId, apiMath::Vector3 pos, apiMath::Vector3 rot, int c1, int c2, int c3, int c4, unsigned int vWorld)
{
	int id = vehicles::getEmptyId();

	vehicles::netVehicle v;
	v.model = vehicleList::getModel(mId);
	v.position = pos;
	v.rotation = rot;
	v.colors[0] = c1;
	v.colors[1] = c2;
	v.colors[2] = c3;
	v.colors[3] = c4;
	v.driverId = 0;
	v.health = 1000.0;
	v.breakOrGas = 0;
	v.lastReceived = 0;
	v.steer = 0.0;
	v.gasPedal = 0.0;
	v.rpm = 0.0;
	v.engineHealth = 1000;
	v.streaming = 200.0;
	v.horn = false;
	v.siren = false;
	v.livery = 0;
	v.dirt = 0.0f;
	v.engine = 1;
	v.trunk = v.hood = false;
	v.driverCheck = networkManager::getConnection()->cTime;

	for(unsigned int i = 0; i < 6; i++)
	{
		v.tyrePopped[i] = false;
	}

	for(unsigned int i = 0; i < 10; i++)
	{
		v.tunes[i] = false;
	}

	if(defaultTunes) aVehicle_setDefaultTunes(v);

	for(unsigned int i = 0; i < 2; i++)
	{
		v.indicators[i] = false;
	}

	v.vWorld = vWorld;

	if(apiVehicleMap.insert(std::make_pair(id, apiVehicle::vehicle(id))).second == false)
	{
		mlog("!Lib: Vehicle %i already exists", id);
		if(vehicles::isVehicle(id))
		{
			mlog("!Lib: Deleting internal vehicle %i", id);
			vehicles::removeVehicle(id);
		}
		if(apiVehicleMap.find(id) != apiVehicleMap.end())
		{
			mlog("!Lib: Deleting api internal vehicle %i", id);
			apiVehicleMap.erase(id);
		}
		return -1;
	}

	vehicles::addVehicle(id, v);
	return id;
}

bool apiVehicle::deleteVehicle(int vehicleId)
{
	if(vehicles::isVehicle(vehicleId))
	{
		vehicles::netVehicle& v = vehicles::getVehicle(vehicleId);
		networkManager::connection* con = networkManager::getConnection();
		RakNet::BitStream vOut;
		vOut.Write((MessageID)IVMP);
		vOut.Write(DELETE_VEHICLE_ONLY);
		vOut.Write(vehicleId);
		for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
		{
			if(i->second.vWorld == v.vWorld && i->second.sVehicles.find(vehicleId) != i->second.sVehicles.end())
			{
				con->peer->Send(&vOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
				i->second.sVehicles.erase(vehicleId);

				entitySpawned::spawned(i->second.id, 0, 1, vehicleId);
			}
		}

		vehicles::removeVehicle(vehicleId);
		apiVehicleMap.erase(vehicleId);
		return true;
	}
	return false;
}

bool apiVehicle::isVehicle(int vehicleId)
{
	return apiVehicleMap.find(vehicleId) != apiVehicleMap.end();
}

apiVehicle::vehicle& apiVehicle::getVehicle(int vehicleId)
{
	return apiVehicleMap.at(vehicleId);
}

apiVehicle::vehicle* apiVehicle::get(int vehicleId)
{
	return apiVehicleMap.find(vehicleId) != apiVehicleMap.end() ? &apiVehicleMap.at(vehicleId) : NULL;
}

void apiVehicle::getAll(std::vector<vehicle*>& vec)
{
	std::map<int, vehicle>::iterator i = apiVehicleMap.begin(), e = apiVehicleMap.end();
	for(i; i != e; ++i)
	{
		vec.push_back(&i->second);
	}
}

apiMath::Vector3 apiVehicle::vehicle::getPosition()
{
	return vehicles::getVehicle(id).position;
}

apiMath::Vector3 apiVehicle::vehicle::getVelocity()
{
	return vehicles::getVehicle(id).velocity;
}

apiMath::Vector3 apiVehicle::vehicle::getRotation()
{
	return vehicles::getVehicle(id).rotation;
}

apiMath::Vector3 apiVehicle::vehicle::getTurnVelocity()
{
	return vehicles::getVehicle(id).turnSpeed;
}

void apiVehicle::vehicle::setStreamDistance(float f)
{
	vehicles::getVehicle(id).streaming = f;
}