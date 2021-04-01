#include "gameMemory.h"
#include "Scripting.h"
#include <Windows.h>
#include "../clientLibs/execution.h"
#include "Offsets.h"

unsigned int moduleHandle = 0;

//pools for GTA IV patch 1070 ONLY
/*int vehiclesPool = 0x1619240;
int pedsPool = 0x18A82AC;
int objectsPool = 0x1350CE0;
int blipsPool = 0x119ED50;*/

//EFLC

unsigned int gameMemory::getModuleHandle()
{
	return moduleHandle;
}

void gameMemory::setModuleHandle()
{
	moduleHandle = reinterpret_cast<u32>(GetModuleHandle(NULL));
	moduleHandle -= 0x400000;

	offsets::VEHICLESPOOL += moduleHandle;
	offsets::PEDSPOOL += moduleHandle;
	offsets::OBJECTSPOOL += moduleHandle;
	//blipsPool += moduleHandle;
}

unsigned int gameMemory::getAddressOfItemInPool(unsigned int PoolAddress, unsigned int Handle)
{
	if(PoolAddress == 0)
	{
		return 0;
	}

	DWORD listadr = *(DWORD*)(PoolAddress);

	DWORD booladr = *(DWORD*)(PoolAddress + 4);

	DWORD maxcount = *(DWORD*)(PoolAddress + 8);

	DWORD itemsize = *(DWORD*)(PoolAddress + 12);

	DWORD index = Handle >> 8;

	unsigned char flag = *(unsigned char*)(booladr + index);

	return (flag&0x80 || int(flag) != (Handle & 0x000000FF)) ? 0 : (listadr + index*itemsize);
}

unsigned int gameMemory::getVehiclesPool()
{
	return offsets::VEHICLESPOOL;
}

unsigned int gameMemory::getPlayersPool()
{
	return offsets::PEDSPOOL;
}

unsigned long gameMemory::getTimeOfDay()
{
	return *(unsigned long*)(moduleHandle + 0x10A4EA8);
}
