/*#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"

#include "../SharedDefines/packetsIds.h"
#include "BitStream.h"
#include "StringCompressor.h"
#include "RakString.h"

#include <iostream>
#include <string>
#include <Windows.h>

void readPacket(RakNet::BitStream& in)
{
	int id;
	in.Read(id);
	float pos;
	in.Read(pos);


	RakNet::RakString r;
	in.Read(r);


	int antiBug;
	in.Read(antiBug);

	RakNet::RakString r2;
	in.Read(r2);


	std::cout << id << ", " << pos << ", " << r.C_String() << ", AntiBug: " << antiBug << ", " << r2 << std::endl;
}

int main()
{
    RakPeerInterface *peer = RakNetworkFactory::GetRakPeerInterface();
    Packet *packet;

    peer->Startup(1,30,&SocketDescriptor(), 1);


    printf("Starting the client.\n");
    peer->Connect("192.168.0.107", 9999, 0, 0);

	RakNet::RakString rs;

	while(1)
	{
		for(packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));

			std::cout << packet->length << std::endl;
			
			switch(packet->data[0])
			{
				case ID_CONNECTION_REQUEST_ACCEPTED:
					std::cout << "Server accepted my connection" << std::endl;
					break;

				default:
					printf("Message with identifier %i has arrived.\n", packet->data[0]);
					readPacket(bsIn);
					break;
			}
			rs.Clear();
		}

		Sleep(20);
	}
	return 1;
}*/