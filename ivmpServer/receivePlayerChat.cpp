#include "receivePlayerChat.h"
#include "easylogging++.h"
#include "players.h"
#include "apiPlayerEvents.h"

apiPlayerEvents::pChat chat = 0;

void receivePlayerChat(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	players::player& p = players::getPlayer(con->packet->guid.g);
	
	RakNet::RakString msg;
	bsIn.Read(msg);

	if(chat)
	{
		chat(p.id, msg);
	}
}

void apiPlayerEvents::registerPlayerChat(pChat f)
{
	chat = f;
}