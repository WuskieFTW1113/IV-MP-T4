#include <Windows.h>
#include <iostream>
#include "easylogging++.h"
#include <fstream>

_INITIALIZE_EASYLOGGINGPP

#define EFLC 1
#define GTAIV 0

#if EFLC
#define EXE "EFLC"
#define EXAMPLE "Episodes From Liberty City"
#else
#define EXE "GTAIV"
#define EXAMPLE "Grand Theft Auto IV"
#endif

bool InjectLibraryIntoProcess(DWORD dwProcessId, char * szLibraryPath)
{
	bool bReturn = true;
	LINFO << "Launching the launcherHelper";

	// Open our target process
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwProcessId);

	if(!hProcess)
	{
		// Failed to open the process
		LINFO << "Failed to open a new process (" << GetLastError() << ")";
		return false;
	}

	// Get the length of the library path
	size_t sLibraryPathLen = (strlen(szLibraryPath) + 1);

	// Allocate the a block of memory in our target process for the library name
	void * pRemoteLibraryPath = VirtualAllocEx(hProcess, NULL, sLibraryPathLen, MEM_COMMIT, PAGE_READWRITE);

	// Write our library name to the allocated block of memory
	SIZE_T sBytesWritten = 0;
	WriteProcessMemory(hProcess, pRemoteLibraryPath, (void *)szLibraryPath, sLibraryPathLen, &sBytesWritten);

	if(sBytesWritten != sLibraryPathLen)
	{
		LINFO << "Failed to write library path into remote process. (" << GetLastError() << ")";
		bReturn = false;
	}
	else
	{
		// Get the handle of Kernel32.dll
		HMODULE hKernel32 = GetModuleHandle("Kernel32");

		// Get the address of the LoadLibraryA function from Kernel32.dll
		FARPROC pfnLoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");

		// Create a thread inside the target process to load our library
		HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnLoadLibraryA, (LPVOID)pRemoteLibraryPath, 0, NULL);
		
		if(hThread)
		{
			// Wait for the created thread to end
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
		}
		else
		{
			// Thread creation failed
			LINFO << "Failed to create remote thread in remote process (" << GetLastError() << ")";
			bReturn = false;
		}
	}

	// Free the allocated block of memory inside the target process
	VirtualFreeEx(hProcess, pRemoteLibraryPath, sizeof(pRemoteLibraryPath), MEM_RELEASE);

	// If the injection failed terminate the target process
	if(!bReturn)
	{
		TerminateProcess(hProcess, 0);
		LINFO << "Injection simply failed (" << GetLastError() << ")";
	}

	// Close our target process
	CloseHandle(hProcess);

	return bReturn;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	std::ofstream ofs;
	ofs.open("comMP//launcher.log", std::ofstream::out | std::ofstream::trunc);
	ofs.close();

	std::string episodesPath;
	std::string episodesExecutable;

	STARTUPINFO siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;
	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	siStartupInfo.cb = sizeof(siStartupInfo);

	LINFO << "Launching " << EXE;

	if(!CreateProcess("Launch" EXE ".exe", NULL, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL,
		&siStartupInfo, &piProcessInfo)) {

		LINFO << "Unable to start " << EXE << ".exe (" << GetLastError() << ")";
		LINFO << episodesPath << ", " << episodesExecutable;

		MessageBox(NULL, "Launch" EXE ".exe was NOT found.\n"
			"Please extract the IVMP files onto your " EXE " installation folder\n"
			"Example: C:\\Program Files (x86)\\Rockstar Games\\" EXAMPLE, "Unable to start GTA", MB_ICONSTOP);
		return 1;
	}

	if(!InjectLibraryIntoProcess(piProcessInfo.dwProcessId, "comMP//bin//launcherAssist.dll"))
	{
		TerminateProcess(piProcessInfo.hProcess, 0);
		LINFO << "Failed to inject launcher (" << GetLastError() << ")";
		return 1;
	}

	ResumeThread(piProcessInfo.hThread);

	LINFO << "Done";

	return 1;
}