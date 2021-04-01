#include "entitySpawned.h"
#include "apiPlayerEvents.h"

apiPlayerEvents::pHp entityHandler = 0;

void apiPlayerEvents::registerPlayerSpawnEntity(pHp f)
{
	entityHandler = f;
}

void entitySpawned::spawned(int playerid, int spawned, int entity, int entityId)
{
	if(entityHandler) entityHandler(playerid, spawned, entity, entityId);
}