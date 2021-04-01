#include "CPatcher.h"
#include <Windows.h>
#include "gameMemory.h"

ProtectionInfo CPatcher::Unprotect(unsigned int dwAddress, int iSize)
{
	ProtectionInfo protectionInfo;
	/*protectionInfo.dwAddress = dwAddress;
	protectionInfo.iSize = iSize;
	VirtualProtect((void *)dwAddress, iSize, PAGE_EXECUTE_READWRITE, &protectionInfo.dwOldProtection);*/
	return protectionInfo;
}

void CPatcher::Reprotect(ProtectionInfo protectionInfo)
{
	/*unsigned int dwProtection;
	VirtualProtect((void *)protectionInfo.dwAddress, protectionInfo.iSize, protectionInfo.dwOldProtection, &dwProtection);*/
}

void CPatcher::InstallNopPatch(unsigned int dwAddress, int iSize, bool bAddBase)
{
	unsigned int dwAddr = dwAddress;

	// Add the base if needed
	if(bAddBase)
		dwAddr += gameMemory::getModuleHandle();

	// Unprotect the address memory
	ProtectionInfo protectionInfo = Unprotect(dwAddr, iSize);

	// Write the no operation to the address memory
	memset((void *)dwAddr, X86_NOP, iSize);

	// Re-protect the address memory
	Reprotect(protectionInfo);
}

void * CPatcher::InstallDetourPatchInternal(unsigned int dwAddress, unsigned int dwDetourAddress, unsigned char byteType, int iSize, bool bAddBase)
{
	unsigned char * pbyteAddr = (unsigned char *)dwAddress;

	// Add the base if needed
	if(bAddBase)
		pbyteAddr += gameMemory::getModuleHandle();

	// Allocate the trampoline memory
	unsigned char * pbyteTrampoline = (unsigned char *)malloc(iSize + 5);

	// Unprotect the trampoline memory
	Unprotect((unsigned int)pbyteTrampoline, (iSize + 5));

	// Unprotect the address memory
	ProtectionInfo protectionInfo = Unprotect((unsigned int)pbyteAddr, (iSize + 5));

	// Copy the overwritten address memory to the trampoline memory
	memcpy(pbyteTrampoline, pbyteAddr, iSize);

	// Write the type to the trampoline memory
	pbyteTrampoline[iSize] = byteType;
	*(void **)(&pbyteTrampoline[iSize + 1]) = (void *)((pbyteAddr + iSize) - (pbyteTrampoline + iSize) - 5);

	// Write the type to the address memory
	pbyteAddr[0] = byteType;
	*(void **)(&pbyteAddr[1]) = (void *)((unsigned char *)dwDetourAddress - pbyteAddr - 5);

	// Re-protect the address memory
	Reprotect(protectionInfo);

	return pbyteTrampoline;
}

// TODO: A method to just use the trampoline and jmp function
void CPatcher::UninstallDetourPatchInternal(unsigned int dwAddress, void ** pTrampoline, int iSize, bool bAddBase)
{
	unsigned char * pTramp = (unsigned char *)*pTrampoline;
	unsigned char * pbyteAddr = (unsigned char *)dwAddress;

	// Add the base if needed
	if(bAddBase)
		pbyteAddr += gameMemory::getModuleHandle();

	// Unprotect the address memory
	ProtectionInfo protectionInfo = Unprotect((unsigned int)pbyteAddr, iSize);

	// Copy the trampoline to the address
	memcpy(pbyteAddr, pTramp, iSize);

	// Re-protect the address memory
	Reprotect(protectionInfo);

	// Free trampoline
	free(pTramp);

	// Set trampoline to the address
	*pTrampoline = pbyteAddr;
}

void * CPatcher::InstallJmpPatch(unsigned int dwAddress, unsigned int dwJmpAddress, int iSize, bool bAddBase)
{
	return InstallDetourPatchInternal(dwAddress, dwJmpAddress, X86_JMP, iSize, bAddBase);
}

void * CPatcher::InstallCallPatch(unsigned int dwAddress, unsigned int dwCallAddress, int iSize, bool bAddBase)
{
	return InstallDetourPatchInternal(dwAddress, dwCallAddress, X86_CALL, iSize, bAddBase);
}

void CPatcher::InstallRetPatch(unsigned int dwAddress, bool bAddBase)
{
	unsigned int dwAddr = dwAddress;

	// Add the base if needed
	if(bAddBase)
		dwAddr += gameMemory::getModuleHandle();

	// Unprotect the address memory
	ProtectionInfo protectionInfo = Unprotect(dwAddr, 1);

	// Write the return to the address memory
	*(unsigned char *)dwAddr = X86_RET;

	// Re-protect the address memory
	Reprotect(protectionInfo);
}

void CPatcher::InstallStringPatch(unsigned int dwAddress, char * szString, int iSize, bool bAddBase)
{
	unsigned int dwAddr = dwAddress;

	// Add the base if needed
	if(bAddBase)
		dwAddr += gameMemory::getModuleHandle();

	// Unprotect the address memory
	ProtectionInfo protectionInfo = Unprotect(dwAddr, iSize);

	// Write the string to the address memory
	memcpy((void *)dwAddr, szString, iSize);

	// Re-protect the address memory
	Reprotect(protectionInfo);
}

void CPatcher::InstallMethodPatch(unsigned int dwHookAddress, unsigned int dwFunctionAddress, bool bAddBase)
{
	unsigned int dwHookAddr = dwHookAddress;

	// Add the base if needed
	if(bAddBase)
		dwHookAddr += gameMemory::getModuleHandle();

	// Unprotect the address memory
	ProtectionInfo protectionInfo = Unprotect(dwHookAddr, 4);

	// Write the function to the address memory
	*(unsigned int *)dwHookAddr = (unsigned int)dwFunctionAddress;

	// Re-protect the address memory
	Reprotect(protectionInfo);
}

unsigned int CPatcher::GetFunctionAddress(char * szLibrary, char * szFunction)
{
	return (unsigned int)GetProcAddress(GetModuleHandle(szLibrary), szFunction);
}

unsigned int CPatcher::GetFunctionAddress(char * szLibrary, unsigned int uOrdinal)
{
	return GetFunctionAddress(szLibrary, (char *)MAKELONG(uOrdinal, 0));
}
/*
#include "detours.h"

void * CPatcher::InstallDetourPatch(char * szLibrary, char * szFunction, unsigned int dwFunctionAddress)
{
	return DetourFunction(DetourFindFunction(szLibrary, szFunction), (unsigned char *)dwFunctionAddress);
	//return InstallDetourPatchInternal(GetFunctionAddress(szLibrary, szFunction), dwFunctionAddress, X86_JMP, 5, false);
}

void * CPatcher::InstallDetourPatch(char * szLibrary, unsigned int uOrdinal, unsigned int dwFunctionAddress)
{
	return DetourFunction(DetourFindFunction(szLibrary, (char *)MAKELONG(uOrdinal, 0)), (unsigned char *)dwFunctionAddress);
	//return InstallDetourPatchInternal(GetFunctionAddress(szLibrary, uOrdinal), dwFunctionAddress, X86_JMP, 5, false);
}

bool CPatcher::UninstallDetourPatch(void * pTrampoline, unsigned int dwFunctionAddress)
{
	return DetourRemove((unsigned char *)pTrampoline, (unsigned char *)dwFunctionAddress) != 0;
}*/