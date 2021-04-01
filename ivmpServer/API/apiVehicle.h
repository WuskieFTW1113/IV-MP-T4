#include "apiMath.h"
#include <exporting.h>
#include <vector>

#ifndef apiVehicle_H
#define apiVehicle_H

namespace apiVehicle
{
	//All data is read only

	class vehicle
	{
		private:
			int id;

		public:
			~vehicle();
			vehicle(int id);

			DLL apiMath::Vector3 getPosition();
			DLL apiMath::Vector3 getVelocity();
			DLL apiMath::Vector3 getRotation();
			DLL apiMath::Vector3 getTurnVelocity();

			DLL int getId();
			DLL int getDriver(); //Returns the current driver ID or 0
			DLL void setTune(unsigned int part, bool on); //part is 0-9
			DLL bool isTuned(unsigned int part);
			DLL void setColor(int color1, int color2, int color3, int color4);
			DLL int getColor(unsigned int slotId);
			DLL void setIndicator(unsigned int indicator, bool onOrOff); //indicators are 0-1
			DLL bool getIndicator(unsigned int indicator);

			DLL void setPosition(apiMath::Vector3& v);
			DLL void setRotation(apiMath::Vector3& v);
			DLL void setVelocity(apiMath::Vector3& v);

			DLL void setLivery(int livery);
			DLL void setDirtLevel(float level);

			DLL int getEngineHealth();
			DLL void setEngineHealth(int hp, bool fixVisual);

			DLL bool getTire(unsigned int index); //Index is 0-5
			DLL void setTire(unsigned int index, bool status);

			DLL float getGasPedal();
			DLL float getSteering();
			DLL int getBreakOrGas();

			DLL void setEngineFlags(int flags); //0 off, 1 off but startable, 2 on
			DLL int getEngineFlags();

			DLL void setMemBit(unsigned int offset, int dataType, float value);

			DLL void setTrunkHood(bool trunk, bool open);
			DLL bool getTrunkHood(bool trunk);

			DLL void setStreamDistance(float f);
	};
	
	//Vehicle model id (mID) must be valid otherwise the server/client may crash
	DLL int addVehicle(int mId, apiMath::Vector3 pos, apiMath::Vector3 rot, int c1, int c2, int c3, int c4, unsigned int vWorld);
	DLL bool deleteVehicle(int vehicleId); //Throws exception
	DLL bool isVehicle(int vehicleId);
	DLL vehicle& getVehicle(int vehicleId); //Throws exception
	DLL vehicle* get(int vehicleId);
	DLL void getAll(std::vector<vehicle*>& vec);
	DLL void toggleAutoComponents(bool b); //This controlls whether the vehicles will spawn with default components.
}

#endif