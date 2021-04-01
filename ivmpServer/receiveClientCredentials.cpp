#include "receiveClientCredentials.h"
#include "players.h"
#include "easylogging++.h"
#include "../SharedDefines/packetsIds.h"
//#include "../Server/eventManager.h"
#include "apiPlayerEvents.h"
#include "playerList.h"
#include "apiWorld.h"
#include "streamPlayerList.h"

apiPlayerEvents::pJoin credential = 0;
apiPlayerEvents::checkSumResponse checkSum = 0;
apiPlayerEvents::pAfk hackResponse = 0;

RakNet::RakString rsitePath;
std::vector<RakNet::RakString> resources;

void receiveClientNickName(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	//player& pd = getPlayer(con->packet->guid.g + 1);
	players::player& p = players::getPlayer(con->packet->guid.g);

	RakNet::RakString pName;
	bsIn.Read(pName);

	int version = 0;
	bsIn.Read(version);

	if(version != 26)
	{
		con->peer->CloseConnection(p.ip, false, 0, LOW_PRIORITY);
		mlog("Lib: Player %s connection dropped due to incorrect version: %i", p.nick.c_str(), version);
		//playerIdManager::idNoLongerUsed(p.id);
		//players::removePlayer(con->packet->guid.g);
		return;
	}

	bsIn.Read(p.c_Game);

	p.nick = pName;

	int serialSize = 0;
	bsIn.Read(serialSize);

	for(int i = 0; i < serialSize; i++)
	{
		pName.Clear();
		bsIn.Read(pName);
		//LINFO << "Serial " << i << ": " << pName;
		p.serials.push_back(std::string(pName));
	}

	p.skin = -1729980128;

	/*size_t rs = resources.size();
	if(rs != 0)
	{
		bsIn.Reset();
		bsIn.Write((MessageID)IVMP);
		bsIn.Write(CLIENT_SCRIPT);
		bsIn.Write((int)rs);
		bsIn.Write(rsitePath);
		for(size_t k = 0; k < rs; k++)
		{
			bsIn.Write(resources[k]);
		}
		con->peer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
	}*/

	if(credential)
	{
		credential(p.id);
		streamPlayerList::playerConnected(con->packet->guid.g);
		return;
	}

	if(apiPlayer::isOn(p.id))
	{
		apiMath::Vector3 spawnPos(-186.6f, 617.1f, 14.2f);
		apiPlayer::get(p.id).spawn(spawnPos);
		streamPlayerList::playerConnected(con->packet->guid.g);
	}

	//serverEvents::playerEvents::callPlayerReceivedDetails(p.id);
}

void receiveCheckSum(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(checkSum != 0)
	{
		if(!players::isPlayer(con->packet->guid.g))
		{
			return;
		}

		unsigned int buf = 0;
		bsIn.Read(buf);

		RakNet::RakString str;
		bsIn.Read(str);

		checkSum(players::getPlayer(con->packet->guid.g).id, str, buf);
	}
}

void receiveHackCheck(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(hackResponse != 0)
	{
		if(!players::isPlayer(con->packet->guid.g))
		{
			return;
		}

		int buf = 0;
		bsIn.Read(buf);

		hackResponse(players::getPlayer(con->packet->guid.g).id, buf == 1);
	}
}

void apiPlayerEvents::registerPlayerCredentials(pJoin f)
{
	credential = f;
}

void apiPlayerEvents::registerPlayerCheckSum(checkSumResponse f)
{
	checkSum = f;
}

void apiPlayerEvents::registerPlayerHackCheck(pAfk f)
{
	hackResponse = f;
}

void apiPlayer::registerResourcesSite(const char* sitePath)
{
	rsitePath.Clear();
	rsitePath = sitePath;
}

void apiPlayer::registerResource(const char* fileName)
{
	RakNet::RakString b;
	b.Clear();
	b = fileName;
	resources.push_back(fileName);
}