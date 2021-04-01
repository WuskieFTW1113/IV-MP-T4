#include <Windows.h>

//#define FUNC_GetCurrentPad_6 0x7C3B10
#define FUNC_GetCurrentPad_7 0x7C3900
//#define VAR_CPlayerPed__VFTable_6 0xD809EC
#define VAR_CPlayerPed__VFTable_7 0xD819C4
//#define VAR_CAutomobile__VFTable_6 0xDB2BFC
#define VAR_CAutomobile__VFTable_7 0xDB3C3C
//#define VAR_CBike__VFTable_6 0xDB302C
#define VAR_CBike__VFTable_7 0xDB4064
//#define VAR_CBoat__VFTable_6 0xDC90DC
#define VAR_CBoat__VFTable_7 0xDCA0E4
//#define VAR_CTrain__VFTable_6 0xD77D74
#define VAR_CTrain__VFTable_7 0xD78D4C
//#define VAR_CHeli__VFTable_6 0xD6A1DC
#define VAR_CHeli__VFTable_7 0xD6B1CC
//#define VAR_CPlane__VFTable_6 0xD85114
#define VAR_CPlane__VFTable_7 0xD86104
//#define FUNC_CPlayerPed__ProcessInput_6 0x9C0B80
#define FUNC_CPlayerPed__ProcessInput_7 0x9C0B00
//#define FUNC_CAutomobile__ProcessInput_6 0xBFE6B0
#define FUNC_CAutomobile__ProcessInput_7 0xC44780 // IV 0xBFE870
//#define FUNC_CBike__ProcessInput_6 0xC059E0
#define FUNC_CBike__ProcessInput_7 0xB45490 //IV 0xC05C30
//#define FUNC_CBoat__ProcessInput_6 0xCA8920
#define FUNC_CBoat__ProcessInput_7 0xB41900 // IV 0xCA8C60
//#define FUNC_CTrain__ProcessInput_6 0x94D0E0
#define FUNC_CTrain__ProcessInput_7 0x94CE70
//#define FUNC_CHeli__ProcessInput_6 0x8B9480
#define FUNC_CHeli__ProcessInput_7 0x904920 // IV 0x8B9290
//#define FUNC_CPlane__ProcessInput_6 0xA1B140
#define FUNC_CPlane__ProcessInput_7 0xA1B080

void installPadHooks();
void uninstallPadHooks();

void ContextSwitch(int playerid, bool bPost);

enum controlType
{
	CONTROL_CHAT = 0,
	CONTROL_VEHICLE_ENTRY = 1,
	CONTROL_GUI = 2
};

void togglePlayerControls(unsigned int typ, bool b, bool serverRequest);
bool getPlayerControls();
bool shouldPlayerBeFrozen();

DWORD getLastContextType();
DWORD getLastContextModel();