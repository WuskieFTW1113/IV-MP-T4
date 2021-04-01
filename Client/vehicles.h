#ifndef vehicles_H
#define vehicles_H

#include "../ClientManager/ivScripting.h"
#include "simpleMath.h"
#include "vehicleMemory.h"
#include <map>

namespace vehicles
{
	struct vMemBit
	{
		int dataType;
		float original;
		float current;

		vMemBit(){}
		~vMemBit(){}
	};

	struct netVehicle
	{
		netVehicle();
		~netVehicle();

		Scripting::Vehicle vehicle;
		unsigned int memoryAddress;
		vehicleMemory::CIVVehicle* vPointer;

		bool volatile threadLock;

		unsigned int model;

		int serverId;
		bool canBeSpawned;
		unsigned int spawnTime;

		int toBeDeleted;

		simpleMath::Vector3 position;
		simpleMath::Vector3 startPos;
		simpleMath::Vector3 goPos;
		simpleMath::Vector3 posError;

		simpleMath::Vector3 rotation;
		simpleMath::Vector3 sRotation;
		simpleMath::Vector3 eRotation;
		simpleMath::Vector3 rotationError;
		float lastRotationAlpha;

		simpleMath::Vector3 speed;
		simpleMath::Vector3 startSpeed;
		simpleMath::Vector3 endSpeed;

		simpleMath::Vector3 turnSpeed;
		simpleMath::Vector3 startTurnSpeed;
		simpleMath::Vector3 endTurnSpeed;

		float steer;
		float startSteer;
		float endSteer;

		float rpm;
		float startRpm;
		float endRpm;

		//int gear;

		int breakOrGas;

		int color[4];
		bool tunes[10];

		bool driverSeatOccupied;
		int driver;

		bool tyrePopped[6][2]; //Tyre id, Status sent to server

		int engineHealth;
		bool engineHpChanged;
		int engineFlags;

		bool siren;
		int horn;
		bool indicators[2];
		long lastIndicator;

		long polTime;
		long lastPol;
		bool hasPol;
		float lastPolProgress;

		bool spawned;

		bool fullySpawnedWithDriver;

		int livery;
		float dirt;

		bool hood;
		bool trunk;
		bool collision;

		std::map<unsigned int, vMemBit> membits;
	};

	bool isVehicle(size_t i);

	void addVehicle(size_t i, netVehicle& v);

	void removeVehicle(size_t i);
	void removeVehicleByIterator(std::map<size_t, netVehicle>::iterator it);

	netVehicle& getVehicle(size_t i);

	std::map<size_t, netVehicle>::iterator getVehiclesBegin();
	std::map<size_t, netVehicle>::iterator getVehiclesEnd();

	size_t streamedVehicles();
	size_t spawnedVehicles();
}

#endif