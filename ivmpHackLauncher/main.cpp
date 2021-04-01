#define _WIN32_WINNT _WIN32_WINNT_WINXP

#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include "easylogging++.h"
#include <sstream>

_INITIALIZE_EASYLOGGINGPP

bool InjectLibraryIntoProcess(DWORD dwProcessId, char * szLibraryPath)
{
	bool bReturn = true;
	LINFO << "Launching Client.dll";

	// Open our target process
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if(hProcess == NULL)
	{
		// Failed to open the process
		LINFO << "Failed to open Client.dll: " << GetLastError();
		return false;
	}

	// Get the length of the library path
	size_t sLibraryPathLen = (strlen(szLibraryPath) + 1);

	// Allocate the a block of memory in our target process for the library name
	void * pRemoteLibraryPath = VirtualAllocEx(hProcess, NULL, sLibraryPathLen, MEM_COMMIT, PAGE_READWRITE);
	if(pRemoteLibraryPath == NULL)
	{
		LINFO << "Failed to Alloc: " << GetLastError();
		return false;
	}

	// Write our library name to the allocated block of memory
	SIZE_T sBytesWritten = 0;
	BOOL wCheck = WriteProcessMemory(hProcess, pRemoteLibraryPath, (void*)szLibraryPath, sLibraryPathLen, &sBytesWritten);
	if(wCheck == 0)
	{
		LINFO << "Failed to Write: " << GetLastError();
		return false;
	}

	if(sBytesWritten != sLibraryPathLen)
	{
		LINFO << "Failed to write library path into remote process. Cannot launch IV:MP.";
		bReturn = false;
	}
	else
	{
		// Get the handle of Kernel32.dll
		HMODULE hKernel32 = GetModuleHandle("Kernel32");

		// Get the address of the LoadLibraryA function from Kernel32.dll
		FARPROC pfnLoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");

		// Create a thread inside the target process to load our library
		HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnLoadLibraryA, pRemoteLibraryPath, 0, NULL);

		if(hThread)
		{
			// Wait for the created thread to end
			WaitForSingleObject(hThread, INFINITE);

			CloseHandle(hThread);
		}
		else
		{
			// Thread creation failed
			LINFO << "Failed to create remote thread in remote process. Cannot launch IV:MP." << GetLastError();
			bReturn = false;
		}
	}

	// Free the allocated block of memory inside the target process
	VirtualFreeEx(hProcess, pRemoteLibraryPath, sizeof(pRemoteLibraryPath), MEM_RELEASE);

	// If the injection failed terminate the target process
	if(!bReturn)
	{
		LINFO << "Shutting down process";
		TerminateProcess(hProcess, 0);
	}

	// Close our target process
	CloseHandle(hProcess);

	return bReturn;
}

//

DWORD m_dwAddress;
int m_iSize;
DWORD m_dwProt;

void Unprotect(DWORD dwAddress, int iSize)
{
	m_dwAddress = dwAddress;
	m_iSize = iSize;
	VirtualProtect((PVOID)dwAddress, iSize, PAGE_EXECUTE_READWRITE, &m_dwProt);
}

void Reprotect()
{
	DWORD dwProt;
	VirtualProtect((PVOID)m_dwAddress, m_iSize, m_dwProt, &dwProt);
}

void * InstallJmpPatch(DWORD dwAddress, DWORD dwJmpAddress, int iSize = 5)
{
	PBYTE pbyteJmpAddress = (PBYTE)dwJmpAddress;
	PBYTE pbyteAddr = (PBYTE)dwAddress;
	PBYTE pbyteJmp = (PBYTE)malloc(iSize + 5);
	Unprotect((DWORD)pbyteAddr, iSize);
	memcpy(pbyteJmp, pbyteAddr, iSize); 
	pbyteJmp += iSize;
	pbyteJmp[0] = 0xE9; // jmp
	*(DWORD*)(pbyteJmp + 1) = (DWORD)(pbyteAddr + iSize - pbyteJmp) - 5;
	pbyteAddr[0] = 0xE9; // jmp
	*(DWORD*)(pbyteAddr + 1) = (DWORD)(pbyteJmpAddress - pbyteAddr) - 5;
	//Reprotect();
	Unprotect((DWORD)(pbyteJmp - iSize), 5);
	return (pbyteJmp - iSize);
}

DWORD GetFunctionAddress(char* szLibrary, char * szFunction)
{
	return (DWORD)GetProcAddress(GetModuleHandle(szLibrary), szFunction);
}

void * DetourFunction(char* szLibrary, char * szFunction, DWORD dwFunctionAddress)
{
	return InstallJmpPatch(GetFunctionAddress(szLibrary, szFunction), dwFunctionAddress);
}

//

class CAPIHook
{
private:
	typedef BOOL	  (WINAPI * CreateProcessW_t)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);

	static CAPIHook * m_pSingleton;

	bool			  m_bHooksInstalled;

	CreateProcessW_t  m_pCreateProcessW;

public:
	CAPIHook()
	{
		m_pSingleton = this;
		m_bHooksInstalled = false;
	}

	~CAPIHook()
	{
		UninstallHooks();
	}

	void InstallHooks()
	{
		if(!m_bHooksInstalled)
		{
			m_pCreateProcessW = (CreateProcessW_t)DetourFunction("Kernel32.dll", "CreateProcessW", (DWORD)CreateProcessW_Hook);

			m_bHooksInstalled = true;
		}
	}

	void UninstallHooks()
	{
		if(m_bHooksInstalled)
		{
			if(m_pCreateProcessW)
			{
				//DetourRemove((PBYTE)m_pCreateProcessW, (PBYTE)CreateProcessW_Hook);
			}

			m_bHooksInstalled = false;
		}
	}

	static BOOL WINAPI CreateProcessW_Hook(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
	{
		// CreateProcessW(GTAIV.exe, "path/to/exe.exe", path/to/exe.exe, CREATE_NEW_PROCESS_GROUP)
		// Technically the only call to CreateProcessW from LaunchGTAIV.exe should be to GTAIV.exe
		// however i should add an application name check anyway
		// TODO: Add an application name check
		LINFO << "Window found";
		dwCreationFlags |=  CREATE_SUSPENDED;
		BOOL bReturn = m_pSingleton->m_pCreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, /*dwCreationFlags*/dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
		if(bReturn)
		{
			// Get the full path of Client.dll

			/*char buf[MAX_PATH] = {0}; 
			GetFullPathName("EFLC.exe", MAX_PATH, buf, NULL);
			LINFO << buf;
			
			std::string str = std::string(buf);

			m_uiBaseAddress = reinterpret_cast<unsigned int>(GetModuleHandle(NULL));
			std::ostringstream s;
			s << buf << "//ClientManager.dll";

			char *cstr = &str[0u];
			*/
			LINFO << "Injecting hack";
			// Inject our code into GTAIV.exe
			if(!InjectLibraryIntoProcess(lpProcessInformation->dwProcessId, "comMP//ClientManager.dll"))
			{
				LINFO << "Injection failed: " << GetLastError();
				return FALSE;
			}

			// Resume the GTAIV.exe thread
			LINFO << "Resuming";
			DWORD r = 0;
			r = ResumeThread(lpProcessInformation->hThread);
			LINFO << "Result: " << r << ", " << GetLastError();
		}
		else
		{
			LINFO << "Create process failed: " << GetLastError();
		}
		return bReturn;
	}
};

CAPIHook * CAPIHook::m_pSingleton;

CAPIHook * pHook = NULL;

/*HANDLE fThread = NULL;
DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	while(true)
	{
		//LINFO << "Pulse";
		HWND windowHandle = FindWindowW(NULL, L"EFLC");
		if(windowHandle != NULL)
		{
			DWORD processID = 0;
			GetWindowThreadProcessId(windowHandle, &processID);
			LINFO << "Found Window: " << processID;

			if(!InjectLibraryIntoProcess(processID, "comMP//ClientManager.dll"))
			{
				LINFO << "Injection failed: " << GetLastError();
				return FALSE;
			}
			CloseHandle(hThread);
			break;
		}
		Sleep(50);
	}
	return 1;
}*/

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			// Disable thread library notifications
			DisableThreadLibraryCalls(hModule);
			LINFO << "Launcher is up";

			// Install the API hooks
			pHook = new CAPIHook();
			pHook->InstallHooks();

			//fThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MyThreadFunction, NULL, 0, NULL);

			break;
		}
	case DLL_PROCESS_DETACH:
		{
			// Uninstall our API hooks

			if(pHook)
			{
				pHook->UninstallHooks();
				delete pHook;
				pHook = NULL;
			}
			/*if(fThread)
			{
				CloseHandle(fThread);
			}*/

			break;
		}
	}

	return TRUE;
}