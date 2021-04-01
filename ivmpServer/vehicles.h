#include <map>
#include "apiMath.h"
#include "apiVehicle.h"
#include <BitStream.h>

namespace vehicles
{
	struct vMem
	{
		int mType;
		float value;
		vMem(){}
		~vMem(){}
	};

	struct netVehicle
	{
		netVehicle();
		~netVehicle();		
		//apiVehicle::vehicle* moduleHandle;

		unsigned int vWorld;
		
		float streaming;

		apiMath::Vector3 position;
		apiMath::Vector3 velocity;
		apiMath::Vector3 turnSpeed;
		apiMath::Vector3 rotation;
		float health;

		int colors[4];
		bool tunes[10];

		unsigned int model;

		int breakOrGas;

		int driverId;

		long lastReceived;

		float steer;
		float gasPedal;
		float rpm;

		int engineHealth;

		bool tyrePopped[6];

		long driverCheck;

		bool horn;
		bool siren;
		bool indicators[2];

		float dirt;
		int livery;
		int engine;

		bool hood, trunk;
		//int gear;

		std::map<unsigned int, vMem> membits;
	};


	bool isVehicle(int i);
	void addVehicle(int i, netVehicle v);
	void removeVehicle(int i);
	
	netVehicle& getVehicle(int i);

	std::map<int, netVehicle>::iterator getVehiclesBegin();
	std::map<int, netVehicle>::iterator getVehiclesEnd();

	int getEmptyId();
}