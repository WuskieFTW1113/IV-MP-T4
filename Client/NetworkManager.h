#ifndef NetworkManager_H
#define NetworkManager_H

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"

#include "BitStream.h"
#include "StringCompressor.h"
#include "RakString.h"

#include <iostream>
#include <string>

namespace networkManager
{
	struct connection
	{
		RakNet::RakPeerInterface *peer;
		RakNet::Packet *packet;

		int connectStat;
		RakNet::SystemAddress serverIp;

		long cTime;
		long lastSentPck;
		long lastRcvPck;
		long serverUpTime;
	};

	void initNetwork(std::string s, int port);
	void closeNetwork();

	connection* getConnection();

	void handlePackts();

	bool isNetworkActive();

	void sendBitStream(RakNet::BitStream& b);
	int getLastPack();
}
#endif