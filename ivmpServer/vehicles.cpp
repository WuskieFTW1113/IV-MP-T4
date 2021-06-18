#include "vehicles.h"
#include "../SharedDefines/easylogging++.h"
#include "API/vehicleList.h"
#include "API/apiVehicle.h"

std::map<int, vehicles::netVehicle> cars;

vehicles::netVehicle::netVehicle()
{
}

vehicles::netVehicle::~netVehicle()
{
}

bool vehicles::isVehicle(int i)
{
	return cars.find(i) != cars.end();
}
void vehicles::addVehicle(int i, netVehicle v)
{
	cars[i] = v;
}

void vehicles::removeVehicle(int i)
{
	cars.erase(i);
}

vehicles::netVehicle& vehicles::getVehicle(int i)
{
	return cars.at(i);
}

std::map<int, vehicles::netVehicle>::iterator vehicles::getVehiclesBegin()
{
	return cars.begin();
}

std::map<int, vehicles::netVehicle>::iterator vehicles::getVehiclesEnd()
{
	return cars.end();
}

int vehicles::getEmptyId()
{
	int count = 1;
	for(std::map<int, vehicles::netVehicle>::const_iterator i = cars.begin(); i != cars.end(); ++i)
	{
		if(i->first != count)
		{
			break;
		}
		count++;
	}
	return count;
}
