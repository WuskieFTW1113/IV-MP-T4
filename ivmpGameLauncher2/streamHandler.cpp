#include "../Raknet/RakPeerInterface.h"
#include "../Raknet/BitStream.h"
#include "streamHandler.h"
#include "../Raknet/MessageIdentifiers.h"
#include "easylogging++.h"
#include "../SharedDefines/packetsIds.h"
#include <experimental/filesystem>
#include "ivTarget.h"

RakNet::RakPeerInterface *peer = nullptr;
RakNet::Packet *packet;
RakNet::SystemAddress ip;
bool ipInit = false;
const char* listIp = "iv-mp.eu";
//const char* listIp = "192.168.1.3";

bool status = false;
int serverOptions = 2;

int serversToReceive = 0;
int serversReceived = 0;
long lastServerReceived = 0;

typedef void (*ScriptFunction)(RakNet::BitStream& bsIn);
std::map<int, ScriptFunction> handlers;
std::map<int, std::string> disconnects;

clock_t requestTime = 0;

void getParams(std::vector<std::string>& words, std::string s, const char* splitAt)
{
	char * pch = NULL;
	char *next = NULL;

	char * writable = new char[s.size() + 1];
	std::copy(s.begin(), s.end(), writable);
	writable[s.size()] = '\0';

	pch = strtok_s(writable, splitAt, &next);
	while(pch != NULL)
	{
		//printf("%s\n", pch);
		words.push_back(pch);
		pch = strtok_s(NULL, splitAt, &next);
	}

	//delete[] pch;
	delete[] writable;
}

int isInt(std::string s)
{
	try
	{
		return std::stoi(s, nullptr, 10);
	}
	catch(const std::invalid_argument&)
	{
		return -1;
	}
	catch(const std::out_of_range&)
	{
		return -1;
	}
	return -1;
}

float isFloat(std::string s)
{
	try
	{
		return std::stof(s);
	}
	catch(const std::invalid_argument&)
	{
		return -1.0f;
	}
	catch(const std::out_of_range&)
	{
		return -1.0f;
	}
	return -1.0f;
}

void initStream()
{
	LINFO << "Starting connection";
	peer = RakNet::RakPeerInterface::GetInstance();
    int rs = peer->Startup(1, &RakNet::SocketDescriptor(), 1);
    if(rs != RakNet::RAKNET_STARTED)
	{
		LINFO << "Connection failed: " << rs;
		peer = nullptr;
		return;
	}

	//TEST PORT IS 9005, MAIN IS 8889
	peer->Connect(listIp, 8889, 0, 0);

	LINFO << "Peer is active";
	serversToReceive = 0;
	serversReceived = 0;
	lastServerReceived = 0;

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
	LINFO << "Done";
}

void pulseStream()
{
	if(peer == nullptr)
	{
		return;
	}

	for(packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
	{
		if(strcmp(packet->systemAddress.ToString(false), listIp) == 0)
		{
			ip = packet->systemAddress;
			ipInit = true;
		}

		RakNet::BitStream bsIn(packet->data, packet->length, false);
		bsIn.IgnoreBytes(sizeof(MessageID));

		if(disconnects.find(packet->data[0]) != disconnects.end())
		{
			LINFO << "!Error: MasterList lost connection: " << disconnects.at(packet->data[0]);
			status = false;
			delete peer;
			peer = nullptr;
			break;
		}
		else if(packet->data[0] == ID_CONNECTION_REQUEST_ACCEPTED)
		{
			status = true;
			LINFO << "MasterList connection is good";
			requestTime = clock();

			RakNet::BitStream b;
			b.Write((MessageID)IVMP);
			b.Write(ASK_FOR_SERVERS);
			b.Write(serverOptions);
			peer->Send(&b, HIGH_PRIORITY, RELIABLE_ORDERED, NULL, packet->systemAddress, false);
			LINFO << "Requesting servers with option: " << serverOptions;
			continue;
		}

		int messageId = -1;
		bsIn.Read(messageId);

		if(messageId == SERVERS_AMOUNT)
		{
			bsIn.Read(serversToReceive);
			RakNet::RakString rs;
			bsIn.Read(rs);

			int blcc = 0;
			bsIn.Read(blcc);

			float vs = isFloat(std::string(rs.C_String()));
			LINFO << "Latest version: " << vs;
			bool needsUpdate = false;
			if(vs > isFloat(version) || blcc != v_LCC)
			{
				needsUpdate = true;
			}

			if(needsUpdate)
			{
				if(MessageBox(NULL, "Would you like to update IV:MP now?\n"
					"You can also update the client manually at www.iv-mp.eu", "Your client is outdated",
					MB_YESNO | MB_ICONQUESTION) == IDYES) {

					namespace fs = std::experimental::filesystem;
					fs::path sourceFile = "comMP//bin//ivmpUpdater.exe";
					fs::path targetParent = "";
					auto target = targetParent / sourceFile.filename(); 
					try
					{
						fs::create_directories(targetParent);
						fs::copy_file(sourceFile, target, fs::copy_options::overwrite_existing);

						HMODULE hModule = GetModuleHandle(NULL);
						CHAR path[MAX_PATH];
						GetModuleFileName(hModule, path, MAX_PATH);
						std::string buf = path;
						buf = buf.substr(0, buf.size() - 20);
						std::ostringstream cmd;
						cmd << "start ivmpUpdater.exe " << GAMENAME << (blcc != v_LCC ? " LCC" : " N") << " \""  << buf << "\\\"";
						//LINFO << cmd.str();

						system(cmd.str().c_str());
						exit(1);
					}
					catch(std::exception& e) 
					{
						LINFO << e.what();
						MessageBox(NULL, "IV:MP was unable to find or execute its updater\n\n"
							"You can also update your client manually at www.iv-mp.eu",
							"Update fail", MB_ICONSTOP);
					}
				}
			}
			else 
			{
				//LINFO << "Deleting updater";
				DeleteFileA("ivmpUpdater.exe");
			}

			if(serversToReceive == 0)
			{
				status = false;
				delete peer;
				peer = nullptr;
				LINFO << "No servers online";
				break;
			}
		}
		else if(messageId == ASK_FOR_SERVERS)
		{
			RakNet::RakString rs;
			std::string buf;
			std::vector<std::string> vbuf;

			bsIn.Read(rs);
			buf = std::string(rs);
			getParams(vbuf, buf, "~");

			size_t vbufsize = vbuf.size();
			//LINFO << vbufsize << ": " << buf;
			if(vbufsize >= (size_t)9) //whats the exact size?
			{
				server svr;
				svr.lock = (vbuf.at(0) == "0" ? "GT" : "IV");
				svr.featured = vbuf.at(1);
				svr.players = vbuf.at(2);
				svr.maxPlayers = vbuf.at(3);
				svr.name = vbuf.at(4);
				svr.location = vbuf.at(5);
				svr.website = vbuf.at(6);
				svr.ip = vbuf.at(7);
				svr.version = vbuf.at(8);
				svr.port = vbufsize == 10 ? vbuf.at(9) : "8888";

				serverArrived(svr);
			}
			//LINFO << "vbufsize: " << vbufsize << "(" << rs << ")";
			serversReceived++;

			if(serversReceived >= serversToReceive || clock() > requestTime + 5000)
			{
				status = false;
				delete peer;
				peer = nullptr;
				LINFO << "Received all servers";
				serversDownloaded();
				break;
			}
		}
		else
		{
			LINFO << "Message with invalid identifier " << (int)messageId << " has arrived";
		}
	}
}

void setServerOptions(int i)
{
	serverOptions = i;
}

bool streamActive()
{
	return peer != nullptr;
}

int streamProgress()
{
	return (int)(serversReceived / serversToReceive * 100);
}
