#include "NetworkManager.h"

void weaponGiven(RakNet::BitStream& bsIn);
void weaponTaken(RakNet::BitStream& bsIn);
void setHealth(RakNet::BitStream& bsIn);
void setSpawnPos(RakNet::BitStream& bsIn);
void ejectOut(RakNet::BitStream& bsIn);
void tpIntoCarSeat(RakNet::BitStream& bsIn);
void freezeClient(RakNet::BitStream& bsIn);
void getClientWeapons(RakNet::BitStream& bsIn);
void setArmor(RakNet::BitStream& bsIn);
void setClientMoney(RakNet::BitStream& bsIn);
void setClientDoor(RakNet::BitStream& bsIn);
int getClientMoney();
void explodeAt(RakNet::BitStream& bsIn);
void defaultSitIdle(RakNet::BitStream& bsIn);
void taskSitNow(RakNet::BitStream& bsIn);
void playSound(RakNet::BitStream& bsIn);
void stopSound(RakNet::BitStream& bsIn);
void setHeading(RakNet::BitStream& bsIn);
void setHudSettings(RakNet::BitStream& bsIn);