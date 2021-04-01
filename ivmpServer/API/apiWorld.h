#include <exporting.h>

#ifndef APIWORLD_H
#define APIWORLD_H

namespace apiWorld
{
	class virtualWorld
	{
		private:
			unsigned int id;
			unsigned int weather;
			unsigned int hour;
			float minutes;
			unsigned int minuteDuration; //In microsecs

		public:
			virtualWorld(unsigned int id, unsigned int weather, unsigned int hour, unsigned int minutes, unsigned int minuteDuration);
			~virtualWorld();

			void updateWorld(long cTime);

			DLL void setWeather(unsigned int weather);
			DLL void setTime(unsigned int hour, unsigned int minutes);
			DLL void setMinuteDuration(unsigned int duration);
			DLL void streamWorldChanges();

			DLL void getTime(unsigned int& hour, unsigned int& minutes);
			DLL unsigned int getWeather();
			DLL unsigned int getMinuteDuration();
	};

	DLL bool isWorld(unsigned int id);
	DLL void createWorld(unsigned int id, unsigned int weather, unsigned int hour, unsigned int minutes, unsigned int minuteDuration);
	DLL void deleteWorld(unsigned int id); //Throws exception

	DLL virtualWorld* getWorld(unsigned int id); //Throws exception
}

#endif