#include "../RakNet2/MessageIdentifiers.h"
#include "../RakNet2/RakPeerInterface.h"
#include "../RakNet2/BitStream.h"
#include <easylogging++.h>
#include "../SharedDefines/packetsIds.h"
#include "includes.h"
#include <string>
#include <map>
#include <time.h>

#if linux
#else
#endif

int onlinePlayers;
void masterList_updateStatus(int currentPlayers)
{
	onlinePlayers = currentPlayers;
}

std::string sName;
std::string sLoc;
std::string sSite;
std::string sPort;
bool sGTAIV;
float sVersion = 0.0f;
clock_t connectionAttempt = 0;

RakNet::RakPeerInterface *peer = nullptr;
RakNet::Packet *packet;
RakNet::SystemAddress ip;
bool ipInit = false;

//const char* listIp = "192.168.1.3";
const char* listIp = "iv-mp.eu";

long cTime = 0;

bool status = false;
bool masterList_isRunning()
{
	return status;
}

std::map<int, std::string> disconnects;

bool masterList_connect(const char* svrPort, const char* svrName, const char* svrLocation, const char* svrSite, float version, bool GTAIV)
{
	sName = svrName;
	sLoc = svrLocation;
	sVersion = version;
	sSite = svrSite;
	sPort = svrPort;
	sGTAIV = GTAIV;

	connectionAttempt = clock();

	peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor rdr = RakNet::SocketDescriptor();
    int rs = peer->Startup(1, &rdr, 1);
    if(rs != RakNet::RAKNET_STARTED)
	{
		mlog("MasterList connection peer failed at startup: %i", rs);
		peer = nullptr;
		return false;
	}

	//TEST PORT IS 9005, MAIN IS 8889
	rs = peer->Connect(listIp, 8889, 0, 0);
	
	return true;
}

void retryConnection(const char* str)
{
	mlog("!Error: MasterList lost connection: %s", str);
	status = false;
	RakNet::RakPeerInterface::DestroyInstance(peer);
	peer = nullptr;

	masterList_connect(sPort.c_str(), sName.c_str(), sLoc.c_str(), sSite.c_str(), sVersion, sGTAIV);
}

int count = 0;
void masterList_pulse()
{
	printf("Run\n");
	if(peer == nullptr)
	{
		return;
	}

	for(packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
	{
		RakNet::BitStream bsIn(packet->data, packet->length, false);
		bsIn.IgnoreBytes(sizeof(MessageID));
		//LINFO << "Packet: " << (int)packet->data[0];

		if(disconnects.find(packet->data[0]) != disconnects.end())
		{
			return retryConnection(disconnects.at(packet->data[0]).c_str());
		}
		else if(packet->data[0] == ID_CONNECTION_REQUEST_ACCEPTED)
		{
			ip = packet->systemAddress;
			ipInit = true;

			status = true;
			RakNet::BitStream b;
			b.Write((MessageID)IVMP);
			b.Write(CREDENTIALS_TO_LIST);
			b.Write(false);

			//Credential, will probably be useless
			RakNet::RakString credential;
			credential.Clear();

			//Server name
			credential = sName.c_str();
			b.Write(credential);

			//Server location
			credential = sLoc.c_str();
			b.Write(credential);

			//Server version
			b.Write(sVersion);

			//Server Site
			credential = sSite.c_str();
			b.Write(credential);

			//Server port;
			credential = sPort.c_str();
			b.Write(credential);

			if(sGTAIV)
			{
				credential = "i";
				b.Write(credential);
			}

			peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			mlog("MasterList connection is good");
		}
		else
		{
			mlog("Message with invalid identifier %i has arrived", (int)packet->data[0]);
		}
	}

	long now = clock();
	if(status && ipInit && now > cTime)
	{
		cTime = now + 5000;
		RakNet::BitStream b;
		b.Write((MessageID)IVMP);
		b.Write(ASK_FOR_SERVERS);
		b.Write(onlinePlayers);
		peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ip, false);
	}
	else if(!status && now > connectionAttempt + 10000)
	{
		retryConnection("Scripted timeout");
	}
}

void masterList_setUp()
{
	disconnects.clear();
	disconnects.insert(std::make_pair(ID_CONNECTION_LOST, "Connection Lost"));
	disconnects.insert(std::make_pair(ID_CONNECTION_ATTEMPT_FAILED, "Connection attempt failed"));
	disconnects.insert(std::make_pair(ID_DISCONNECTION_NOTIFICATION, "Connection terminated"));
	disconnects.insert(std::make_pair(ID_ALREADY_CONNECTED, "Already connected (terminating)"));
	disconnects.insert(std::make_pair(ID_NO_FREE_INCOMING_CONNECTIONS, "Server is full"));
	disconnects.insert(std::make_pair(ID_PUBLIC_KEY_MISMATCH, "Connection denied (Key)"));
	disconnects.insert(std::make_pair(ID_CONNECTION_BANNED, "You are banned from this server"));
	disconnects.insert(std::make_pair(ID_INVALID_PASSWORD, "Invalid passworld"));
	disconnects.insert(std::make_pair(ID_INCOMPATIBLE_PROTOCOL_VERSION, "Incompatible protocol"));
	disconnects.insert(std::make_pair(ID_IP_RECENTLY_CONNECTED, "The server hasnt removed your old connection yet"));
	//disconnects.insert(std::make_pair(ID_MODIFIED_PACKET, "Invalid packets"));

	mlog("Master list events count: %u", disconnects.size());
}

#if linux
#else
int main(int argc, char *argv[])
{
	masterList_setUp();
	return 1;
}
#endif