#include "NetworkManager.h"

void parseOnFootSync(RakNet::BitStream& bsIn);

void parseWeaponChanged(RakNet::BitStream& bsIn);

void parseDuckChanged(RakNet::BitStream& bsIn);

void parseSkinChanged(RakNet::BitStream& bsIn);

void parseColorChanged(RakNet::BitStream& bsIn);

void gotoNewPos(RakNet::BitStream& bsIn);

void parseEnteringVehicle(RakNet::BitStream& bsIn);

void parseDebugPlayer(RakNet::BitStream& bsIn);

void parseClothesChanged(RakNet::BitStream& bsIn);

void parseHpChanged(RakNet::BitStream& bsIn);

void parseAnimChanged(RakNet::BitStream& bsIn);

void parseAnimLooping(RakNet::BitStream& bsIn);

void parsePropChange(RakNet::BitStream& bsIn);

void parseNameChanged(RakNet::BitStream& bsIn);

void parseAfkChanged(RakNet::BitStream& bsIn);

void parseDrawFlags(RakNet::BitStream& bsIn);