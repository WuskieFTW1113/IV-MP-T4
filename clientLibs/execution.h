namespace execution
{
	__declspec(dllexport) bool hasGameThread();

	__declspec(dllexport) typedef int(*threadFunc)();
	__declspec(dllexport) void registerThreadFunctions(threadFunc pulse, threadFunc unload, threadFunc disconnect);

	__declspec(dllexport) void requestThreadUnload();
	__declspec(dllexport) void callUnloadFunction();
	__declspec(dllexport) bool shouldUnloadThread();

	__declspec(dllexport) void callMustDisconnect();

	__declspec(dllexport) void setUnloadMsg(const char* tunloadMsg);
	__declspec(dllexport) const char* getUnloadMsg();

	__declspec(dllexport) int pulseThread();

	__declspec(dllexport) bool hasSpawned();
	__declspec(dllexport) void setSpawned(bool b);

	__declspec(dllexport) void setServerIp(const char* ip, int port, const char* svrName);
	__declspec(dllexport) const char* getServerIp();
	__declspec(dllexport) const char* getServerName();
	__declspec(dllexport) int getServerPort();

	__declspec(dllexport) void setVersion(const char* version);
	__declspec(dllexport) const char* getVersion();

	__declspec(dllexport) void setPatch(int i);
	__declspec(dllexport) int getPatch();

	__declspec(dllexport) void setCompatibility();

	__declspec(dllexport) void defineNatives(unsigned int natives);
	__declspec(dllexport) unsigned int getNatives();
}