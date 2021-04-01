#include <Windows.h>
#include "dumpManager.h"
#include "easylogging++.h"

_INITIALIZE_EASYLOGGINGPP;

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH: 
        {	
			break;
        }
        case DLL_PROCESS_DETACH:
        {
			break;
        }
    }

    return TRUE;
}