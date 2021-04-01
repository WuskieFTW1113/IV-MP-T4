struct ProtectionInfo
{
	unsigned int dwAddress;
	unsigned int dwOldProtection;
	int iSize;
};

enum assemblerOpCodes
{
	X86_NOP = 0x90,
	X86_RET = 0xC3,
	X86_CALL = 0xE8,
	X86_JMP = 0xE9,
};

class CPatcher
{
public:
	static ProtectionInfo Unprotect(unsigned int dwAddress, int iSize);
	static void Reprotect(ProtectionInfo protectionInfo);
	static void InstallNopPatch(unsigned int dwAddress, int iSize = 1, bool bAddBase = true);
	static void * InstallDetourPatchInternal(unsigned int dwAddress, unsigned int dwDetourAddress, unsigned char byteType, 
		int iSize = 5, bool bAddBase = true);
	static void UninstallDetourPatchInternal(unsigned int dwAddress, void ** pTrampoline, int iSize = 5, bool bAddBase = true);
	static void * InstallJmpPatch(unsigned int dwAddress, unsigned int dwJmpAddress, int iSize = 5, bool bAddBase = true);
	static void * InstallCallPatch(unsigned int dwAddress, unsigned int dwCallAddress, int iSize = 5, bool bAddBase = true);
	static void InstallRetPatch(unsigned int dwAddress, bool bAddBase = true);
	static void InstallStringPatch(unsigned int dwAddress, char * szString, int iSize, bool bAddBase = true);
	static void InstallMethodPatch(unsigned int dwHookAddress, unsigned int dwFunctionAddress, bool bAddBase = true);
	static unsigned int GetFunctionAddress(char * szLibrary, char * szFunction);
	static unsigned int GetFunctionAddress(char * szLibrary, unsigned int uOrdinal);
	static void * InstallDetourPatch(char * szLibrary, char * szFunction, unsigned int dwFunctionAddress);
	static void * InstallDetourPatch(char * szLibrary, unsigned int uOrdinal, unsigned int dwFunctionAddress);
	static bool UninstallDetourPatch(void * pTrampoline, unsigned int dwFunctionAddress);
};