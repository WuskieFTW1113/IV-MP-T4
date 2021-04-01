namespace gameMemory
{
	unsigned int getModuleHandle();
	void setModuleHandle();

	unsigned int getAddressOfItemInPool(unsigned int PoolAddress, unsigned int Handle);

	unsigned int getVehiclesPool();
	unsigned int getPlayersPool();

	unsigned long getTimeOfDay();
	void closeDoors(bool status);
	bool getDoorStatus();
}