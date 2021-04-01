#include "NetworkManager.h"

#include "MainHook.h"
#include "CustomFiberThread.h"
#include "easylogging++.h"
#include "../clientLibs/execution.h"
#include "../clientLibs/dumpManager.h"
#include "padCallBack.h"

_INITIALIZE_EASYLOGGINGPP

void client_make_minidump(EXCEPTION_POINTERS* e)
{
    auto hDbgHelp = LoadLibraryA("dbghelp");
    if(hDbgHelp == nullptr)
        return;
    auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
    if(pMiniDumpWriteDump == nullptr)
        return;

	std::ostringstream path;

    SYSTEMTIME t;
    GetSystemTime(&t);

	char cPath[MAX_PATH];
	GetModuleFileName(NULL, cPath, MAX_PATH);

	std::string bufString = cPath;

	path << bufString.substr(0, bufString.find_last_of("\\/")) << "\\comMP\\" << 
		t.wYear << "-" << t.wMonth << "-" << t.wDay << "-" << t.wHour << "-" << t.wMinute << "-" << t.wSecond << ".dmp";

	LINFO << path.str();

    auto hFile = CreateFileA(path.str().c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(hFile == INVALID_HANDLE_VALUE)
        return;

    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
    exceptionInfo.ThreadId = GetCurrentThreadId();
    exceptionInfo.ExceptionPointers = e;
    exceptionInfo.ClientPointers = FALSE;

    auto dumped = pMiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
        e ? &exceptionInfo : nullptr,
        nullptr,
        nullptr);

    CloseHandle(hFile);

	LINFO << "Created";
    return;
}

LONG CALLBACK client_unhandled_handler(EXCEPTION_POINTERS* e)
{
 	LINFO << "Executing tasks: " << executingTask << ", " << subTask << ", " << networkManager::getLastPack();
	if(executingTask == 2)
	{
		LINFO << "Context: " << getLastContextType() << ", " << getLastContextModel();
	}
	LINFO << "ClientManager: Creating dump";
	client_make_minidump(e);

	system("taskkill /im eflc.exe");

    return EXCEPTION_EXECUTE_HANDLER;
}

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH: 
        {
			SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)client_unhandled_handler);

			execution::registerThreadFunctions(myThreadPulse, myThreadStop, myThreadDisconnect);
            break;
        }
        case DLL_PROCESS_DETACH:
        {
			break;
        }
    }

    return TRUE;
}