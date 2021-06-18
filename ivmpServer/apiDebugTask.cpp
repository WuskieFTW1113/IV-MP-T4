#include "apiDebugTask.h"
#include <sstream>
#include "../SharedDefines/easylogging++.h"

#if linux
#else
#include <Windows.h>
#include <Dbghelp.h>

void make_minidump(EXCEPTION_POINTERS* e)
{
    auto hDbgHelp = LoadLibraryA("dbghelp");
    if(hDbgHelp == NULL)
        return;
    auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
    if(pMiniDumpWriteDump == NULL)
        return;

	std::ostringstream path;

    SYSTEMTIME t;
    GetSystemTime(&t);

	char cPath[MAX_PATH];
	GetModuleFileName(0, cPath, MAX_PATH);

	std::string bufString = cPath;

	path << bufString.substr(0, bufString.find_last_of("\\/")) << "\\" << 
		t.wYear << "-" << t.wMonth << "-" << t.wDay << "-" << t.wHour << "-" << t.wMinute << "-" << t.wSecond << ".dmp";

	mlog(path.str().c_str());

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
        e ? &exceptionInfo : NULL,
        NULL,
        NULL);

    CloseHandle(hFile);

	mlog("Created");
    return;
}

LONG WINAPI unhandled_handler(EXCEPTION_POINTERS* e)
{
	mlog("!Last executable task: %i, %i", apiDebugTask::getTask(), apiDebugTask::getSubTask());
	mlog("Creating dump");
 	make_minidump(e);

    return EXCEPTION_CONTINUE_SEARCH;
}
#endif

void apiDebugTask::setDump()
{
#if linux
#else
	SetUnhandledExceptionFilter(unhandled_handler);
#endif
}

int serverTask_main = 0;
int serverTask_sub = 0;

int apiDebugTask::getTask()
{
	return serverTask_main;
}

void apiDebugTask::setTask(int i)
{
	serverTask_main = i;
}

int apiDebugTask::getSubTask()
{
	return serverTask_sub;
}

void apiDebugTask::setSubTask(int i)
{
	serverTask_sub = i;
}
