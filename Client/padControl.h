#include <Windows.h>
#include "padData.h"
#include "CPad.h"

void ContextSwitch(int playerid, bool bPost);

void * GetInternalPad();

void initPlayerPadHook(int playerid);
void removePlayerPadHook(int playerid);
bool hasPlayerPadHook(int playerid);

CPad* getLocalPad();
void initLocalPad();