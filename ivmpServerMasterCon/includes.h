#include <exporting.h>

DLL void masterList_updateStatus(int currentPlayers);
DLL bool masterList_isRunning();
DLL bool masterList_connect(const char* svrPort, const char* svrName, const char* svrLocation,
		const char* svrSite, float version, bool GTAIV);
DLL void masterList_pulse();
DLL void masterList_setUp();