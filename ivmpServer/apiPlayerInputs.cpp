#include "API/apiPlayerInputs.h"
#include "receiveFootSync.h"
#include "players.h"
#include "../SharedDefines/packetsIds.h"

apiPlayerInputs::keyInput keyF;

void apiPlayer::player::setKeyHook(unsigned int virtualKey, bool addOrRemove)
{
	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(BIND_PLAYER_KEY);
	bsOut.Write(virtualKey);
	bsOut.Write(addOrRemove ? 1 : 0);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, players::getPlayer(this->engineId).ip, false);
}

void apiPlayerInputs::registerKeyInputs(apiPlayerInputs::keyInput f)
{
	keyF = f;
}

void clientKeyPress(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);
	if(!apiPlayer::isOn(p.id))
	{
		return;
	}

	unsigned int key = 0;
	int status = 0;
	bsIn.Read(key);
	bsIn.Read(status);

	keyF(apiPlayer::get(p.id), key, status == 1);
}
