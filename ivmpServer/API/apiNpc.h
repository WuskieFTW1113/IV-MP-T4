#include "apiMath.h"
#include <exporting.h>

namespace apiNpc
{
	DLL int createNpc(const char* nick, int skinModel, apiMath::Vector3 position, int vehicleId, const char* fileName, unsigned int vWorld, 
		float heading);
	DLL void deleteNpc(int npcId);
	DLL bool isValid(int npcId);

	DLL void clearNpcTaks(int npc);
	DLL void setNpcStartPos(int npc, apiMath::Vector3 newPos, float newHeading);
	DLL void addFootTask(int npc, apiMath::Vector3 gotoPos, apiMath::Vector3 aim, float gotoHeading, 
		int anim, int weapon, int taskDuration);

	DLL void addVehicleTask(int npc, int vehicleId, apiMath::Vector3 pos, apiMath::Vector3 rot,
		apiMath::Vector3 velo, int vehAnim, float pedals, float steer, int duration);
	DLL void playTasks(int npc, bool repeats);

	DLL bool tasksPlaying(int npc);

	DLL typedef void(*tasksFinished)(int npc, bool willRepeat);
	DLL void registerTasksFinished(tasksFinished f);

	DLL void assignControl(int npc, int playerid);

	DLL void setStreamDis(int npc, float d);

	DLL void setClothes(int npc, unsigned int part, unsigned int value);
	DLL void setProperty(int npc, unsigned int part, unsigned value);
}