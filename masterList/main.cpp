#include "MessageIdentifiers.h"
#include "RakPeer.h"
#include "BitStream.h"
#include <map>
#include <sstream>
#include <time.h>
#include "../SharedDefines/truelog.h"
#include "../SharedDefines/packetsIds.h"
#include "pipeHandler.h"

_INITIALIZE_EASYLOGGINGPP

struct serverInfo
{
	std::string  ip;
	std::string name;
	std::string location;
	std::string site;
	std::string port;
	unsigned int onlinePlayers;
	float version;
	bool authed;
	unsigned int lastCheck;
	RakNet::SystemAddress rip;
	bool iv;
	unsigned int peak;

	serverInfo(RakNet::SystemAddress rip, std::string ip, std::string name, std::string location,
		unsigned int onlinePlayers, float version, bool authed, std::string site, std::string svrPort, bool gtaIV) {

		this->rip = rip;
		this->ip = ip;
		this->name = name;
		this->location = location;
		this->version = version;
		this->onlinePlayers = onlinePlayers;
		this->authed = authed;
		this->site = site;
		this->port = svrPort;
		this->lastCheck = clock();
		this->iv = gtaIV;
		this->peak = 0;
	}
};

std::map<int, std::string> disconnects;
void initDisconnects()
{
	disconnects.insert(std::make_pair(ID_CONNECTION_LOST, "Connection Lost"));
	disconnects.insert(std::make_pair(ID_REMOTE_CONNECTION_LOST, "Connection Lost"));
	disconnects.insert(std::make_pair(ID_REMOTE_DISCONNECTION_NOTIFICATION, "Remote console closed"));

	disconnects.insert(std::make_pair(ID_CONNECTION_ATTEMPT_FAILED, "Connection attempt failed"));
	disconnects.insert(std::make_pair(ID_DISCONNECTION_NOTIFICATION, "Connection terminated"));
	disconnects.insert(std::make_pair(ID_ALREADY_CONNECTED, "Already connected (terminating)"));
	disconnects.insert(std::make_pair(ID_NO_FREE_INCOMING_CONNECTIONS, "Server is full"));
	//disconnects.insert(std::make_pair(ID_RSA_PUBLIC_KEY_MISMATCH, "Connection denied (Key)"));
	disconnects.insert(std::make_pair(ID_CONNECTION_BANNED, "You are banned from this server"));
	disconnects.insert(std::make_pair(ID_INVALID_PASSWORD, "Invalid passworld"));
	//disconnects.insert(std::make_pair(ID_INCOMPATIBLE_PROTOCOL_VERSION, "Incompatible protocol"));
	disconnects.insert(std::make_pair(ID_IP_RECENTLY_CONNECTED, "The server hasn't removed your old connection yet"));
	//disconnects.insert(std::make_pair(ID_MODIFIED_PACKET, "Invalid packets"));
}

int main()
{
	pipeHandler::init(1, "masterlistPipe");

	RakNet::RakPeerInterface *peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet *packet;
	
	//TEST PORT IS 9005, MAIN IS 8889
	int rs = peer->Startup(1000, &RakNet::SocketDescriptor(8889, 0), 1);
	if(rs != RakNet::RAKNET_STARTED)
	{
		LINFO << "MasterList startup failed:" << rs;
		return 1;
	}
	Sleep(200);
	peer->SetMaximumIncomingConnections(1000);

	initDisconnects();

	std::map<uint64_t, serverInfo> servers;

	//std::map<std::string, unsigned int> serversCredentials;
	//serversCredentials.insert(std::make_pair(std::string("abelha"), 32));

	const size_t authListSize = 1;
	const char* authList[authListSize][2] = {"Argonath RPG 2.1", "145.239.183.131"};
	
	unsigned int pulses = 0;
	unsigned int nextCheck = 0;

	RakNet::RakString version;
	version.Clear();
	version = "1.1";

	int v_LCC = 2;

	while(true)
	{
		pulses++;
		if(pulses > nextCheck)
		{
			nextCheck += 20;
			try
			{
				std::map<uint64_t, serverInfo>::iterator i = servers.begin();
				std::ostringstream s;
				while(i != servers.end())
				{
					if(pulses > i->second.lastCheck)
					{
						LINFO << "Disconnecting inactive IP: " << i->second.rip.ToString();
						peer->CloseConnection(i->second.rip, false, NULL, LOW_PRIORITY);
						servers.erase(i++);
						continue;
					}

					if(i->second.onlinePlayers > i->second.peak) {
						i->second.peak = i->second.onlinePlayers;
					}

					s << i->second.name << "," << i->second.location << "," << i->second.onlinePlayers << "," 
						<< i->second.peak << "," << i->second.site << "\n";

					++i;
				}
				pipeHandler::write(1, s.str().c_str());
			}
			catch(std::exception& e)
			{
				LINFO << "CHECK: " << e.what();
			}
		}
		
		for(packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));

			if(packet->data[0] == ID_NEW_INCOMING_CONNECTION)
			{
				//LINFO << "New connection incoming: " << packet->systemAddress.ToString(false);
				continue;
			}
			else if(disconnects.find(packet->data[0]) != disconnects.end())
			{
				if(servers.find(packet->guid.g) != servers.end())
				{
					servers.erase(packet->guid.g);
					//LINFO << "Server ip " << packet->systemAddress.ToString(false) << " info erased";
				}
				continue;
			}
			else if(packet->data[0] != IVMP)
			{
				//LINFO << "Message with invalid identifier " << (int)packet->data[0] << " has arrived";				
				continue;
			}

			int messageId = -1;
			bsIn.Read(messageId);

			if(messageId == CREDENTIALS_TO_LIST)
			{
				bool client;
				bsIn.Read(client);
				
				if(!client)
				{
					//RakNet::RakString credential;
					RakNet::RakString svrName;
					RakNet::RakString svrLocation;
					RakNet::RakString svrSite;
					RakNet::RakString svrPort;
					RakNet::RakString isIV;
					float version;

					//bsIn.Read(credential);
					/*std::string buf = std::string(credential);
					LINFO << "Credential arrived: " << buf;
					bool auth = false;
					if(serversCredentials.find(buf) != serversCredentials.end())
					{
						//LINFO << "Server crenditals match";
						auth = true;
					}*/

					bsIn.Read(svrName);
					bsIn.Read(svrLocation);
					bsIn.Read(version);
					bsIn.Read(svrSite);
					bsIn.Read(svrPort);
					bsIn.Read(isIV);

					bool isAuth = false;
					for(size_t i = 0; i < authListSize; i++)
					{
						if(strcmp(svrName.C_String(), authList[i][0]) == 0 && 
							strcmp(packet->systemAddress.ToString(false), authList[i][1]) == 0) {
							isAuth = true;
							break;
						}
					}

					servers.insert(std::make_pair(packet->guid.g, serverInfo(packet->systemAddress, packet->systemAddress.ToString(false),
						std::string(svrName), std::string(svrLocation), 0, version, isAuth, std::string(svrSite), std::string(svrPort),
						strcmp(isIV, "i") == 0)));

					LINFO << "Server " << svrName << " created on ip " << packet->systemAddress.ToString(false);
				}
			}
			else if(messageId == ASK_FOR_SERVERS)
			{
				if(servers.find(packet->guid.g) != servers.end()) //its a server
				{
					serverInfo& server = servers.at(packet->guid.g);
					bsIn.Read(server.onlinePlayers);
					server.lastCheck = pulses + 110;
					//LINFO << server.name << " players updated to " << server.onlinePlayers;
				}
				else
				{
					RakNet::BitStream b;
					b.Write((MessageID)IVMP);
					b.Write(SERVERS_AMOUNT);
					b.Write((int)servers.size());
					b.Write(version);
					b.Write(v_LCC);
					peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, NULL, packet->systemAddress, false);

					std::ostringstream buf;
					RakNet::RakString s;
					for(std::map<uint64_t, serverInfo>::iterator i = servers.begin(); i != servers.end(); ++i)
					{
						//follows struct sequence at @streamHandler.h (ivmpGameLauncher2 project)
						b.Reset();
						b.Write((MessageID)IVMP);
						b.Write(ASK_FOR_SERVERS);
						//LINFO << "ASKED: " << (int)i->second.iv;
						buf << (int)i->second.iv << "~" << 1 << "~" << i->second.onlinePlayers << "~" << 50 <<
							"~" << i->second.name << "~" << i->second.location << "~" << i->second.site << "~" << 
							i->second.ip << "~" << i->second.version << "~" << i->second.port;

						s = buf.str().c_str();
						b.Write(s);
						buf.str(std::string(""));
						peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, NULL, packet->systemAddress, false);
					}
				}
			}
		}
		Sleep(300);
	}
	
	return 1;
}
