#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "BitStream.h"

#ifndef networkManager_H
#define networkManager_H

namespace networkManager
{
	struct connection
	{
		RakNet::RakPeerInterface *peer;
		RakNet::Packet *packet;

		int connectStat;

		long cTime;
	};

	void initNetwork(int port);
	void closeNetwork();

	connection* getConnection();

	void handlePackts();

	bool isNetworkActive();

	void streamData(connection* con);
}

#endif