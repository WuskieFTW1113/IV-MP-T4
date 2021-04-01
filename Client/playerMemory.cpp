#include "playerMemory.h"
#include "gameMemory.h"

void playerMemory::getPlayerPosition(int playerAddress, simpleMath::Vector3& vec)
{
	int buf = *(int*)(playerAddress + 32);
	if(buf == 0)
	{
		return;
	}

	vec.x = *(float*)(buf + 0x30); 
	vec.y = *(float*)(buf + 0x34); 
	vec.z = *(float*)(buf + 0x38);
}

void playerMemory::setPlayerPosition(int playerAddress, simpleMath::Vector3& vec)
{
	int buf = *(int*)(playerAddress + 32);
	if(buf == 0)
	{
		return;
	}

	*(float*)(buf + 0x30) = vec.x; 
	*(float*)(buf + 0x34) = vec.y; 
	*(float*)(buf + 0x38) = vec.z;
}

unsigned int playerMemory::getLocalPlayerIndex()
{
	return *(unsigned int*)(gameMemory::getModuleHandle() + 0xF1CC68);
}