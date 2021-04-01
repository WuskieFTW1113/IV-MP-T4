namespace EFLCScripts
{
	__declspec(dllexport) void disableGTAStats();
	__declspec(dllexport) void turnOffGTAScripts();
	__declspec(dllexport) void pulseGTAScripts();

	__declspec(dllexport) void setSpawnPos(float x, float y, float z, float heading);
	__declspec(dllexport) void getSpawnPos(float &x, float &y, float &z, float &heading);
}