#include "playerConnectionState.h"
#include "players.h"
#include "vehicles.h"
////#include "../Server/eventManager.h"
#include "../SharedDefines/packetsIds.h"
#include "API/apiPlayerEvents.h"
#include "../SharedDefines/easylogging++.h"
#include "streamPlayerList.h"
#include "vehicleSyncDeclarations.h"

apiPlayerEvents::pLeft left = 0;
apiPlayerEvents::pWeap tsState = 0;

void playerDisconnected(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	int leftInt = 1;
	bsIn.Read(leftInt);

	players::player& p = players::getPlayer(con->packet->guid.g);
	//serverEvents::playerEvents::callPlayerDisconnect(p.id, 0);

	if(p.currentVehicle != 0 && vehicles::isVehicle(p.currentVehicle))
	{
		vehicles::netVehicle& v = vehicles::getVehicle(p.currentVehicle);
		v.driverId = 0;
		//if(v.engine == 2) v.engine = 1;
	}

	//removePlayer(con->packet->guid.g + 1);

	RakNet::BitStream bsOut;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(PLAYER_LEFT_TO_CLIENT);
	bsOut.Write(players::getPlayer(con->packet->guid.g).id);

	for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
	}

	if(left)
	{
		left(p.id, leftInt);
	}

	streamPlayerList::playerDisconnected(p.id);
	removeBeingJacked(p.id);
	players::removePlayer(con->packet->guid.g);
}

void playerConnected(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::addPlayer(con->packet->guid.g, con->packet->systemAddress))
	{
		con->peer->CloseConnection(con->packet->systemAddress, false, 0, LOW_PRIORITY);
		mlog("Lib: Player connection failed");
	}
	//addPlayer(con->packet->guid.g + 1, con->packet->systemAddress);

	//serverEvents::playerEvents::callPlayerConnect(getPlayer(con->packet->guid.g).id);
}

void playerTeamSpeak(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(tsState && players::isPlayer(con->packet->guid.g))
	{
		int i, j;
		bsIn.Read(i);
		bsIn.Read(j);
		tsState(players::getPlayer(con->packet->guid.g).id, i, j);
	}
}

void apiPlayerEvents::registerPlayerLeft(apiPlayerEvents::pLeft f)
{
	left = f;
}

void apiPlayerEvents::registerTeamSpeak(apiPlayerEvents::pWeap f)
{
	tsState = f;
}
