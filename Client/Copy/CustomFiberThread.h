#ifndef CustomFiberThread_H
#define CustomFiberThread_H

#include <ScriptThread.h>
#include <ScriptingEnums.h>
#include <ScriptingTypes.h>
#include "../ArusHook/Services/KeyboardHook.h"

#include "players.h"
#include "vehicles.h"

extern int executingTask;
extern int subTask;

class CustomFiberThread : 
	public ScriptThread,
	public IKeyboardHookHandler
{
private:
	Scripting::Player GetPlayer();
	Scripting::Ped GetPlayerPed();

	bool SpawnCar(vehicles::netVehicle &veh);
	void ChangePlayerSkin(Scripting::eModel model);

	void initPlayerStatus();
	void refreshMyPlayerStatus(players::netPlayer& p, long cTime);
	void managePlayers();

	void inVehicleSync(players::netPlayer& p);
	void pulseVehicles(float x, float y, float z);

	//void getTarget(float ax, float az, float &x, float &y, float &z, float range);
	//void getCamTargetedCoords(float &x, float &y, float &z, float range);

	void OnKeyboardHookEvent(const IKeyboardHookHandler::KeyEventArgs &args);

	bool spawnRemotePlayer(int id, players::netPlayer& p);

	size_t getAnimByGroup(char * group);
	size_t searchForAnim(int action);

	bool unloadStuff();

public:
	void Vehicle_ProcessInput_Hook();

protected:
    // We don't want a Tick based script, so we override RunScript.
	void RunScript();

public:
	CustomFiberThread();
	~CustomFiberThread();
};
#endif

bool isGameKeyPressed(int key);
void getMovement(int& x, int& y);

int myThreadInit();
int myThreadPulse();
int myThreadStop();