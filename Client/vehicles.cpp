#include "vehicles.h"
#include "easylogging++.h"

std::map<size_t, vehicles::netVehicle> vehs;

vehicles::netVehicle::netVehicle()
{
	threadLock = false;
}

vehicles::netVehicle::~netVehicle()
{
}

bool vehicles::isVehicle(size_t i)
{
	return !(vehs.find(i) == vehs.end());
}

void vehicles::addVehicle(size_t i, vehicles::netVehicle& v)
{
	vehs.insert(std::make_pair(i, v));
}

void cleanPointers(vehicles::netVehicle& v)
{
}

void vehicles::removeVehicle(size_t i)
{
	cleanPointers(vehs.at(i));
	vehs.erase(i);
}

void vehicles::removeVehicleByIterator(std::map<size_t, vehicles::netVehicle>::iterator it)
{
	cleanPointers(vehs.at(it->first));
	vehs.erase(it);
}

vehicles::netVehicle& vehicles::getVehicle(size_t i)
{
	return vehs.at(i);
}

std::map<size_t, vehicles::netVehicle>::iterator vehicles::getVehiclesBegin()
{
	return vehs.begin();
}

std::map<size_t, vehicles::netVehicle>::iterator vehicles::getVehiclesEnd()
{
	return vehs.end();
}

size_t vehicles::streamedVehicles()
{
	return vehs.size();
}

size_t vehicles::spawnedVehicles()
{
	size_t in = 0;
	for(std::map<size_t, netVehicle>::iterator i = vehs.begin(); i != vehs.end(); ++i)
	{
		if(i->second.spawned)
		{
			in++;
		}
	}
	return in;
}