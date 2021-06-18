#include "playerFunctions.h"
#include "networkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "API/apiColoredChat.h"
#include "../SharedDefines/easylogging++.h"

void sendPlayerMsg(players::player& p, const char* text, unsigned int hex, int chatId)
{
	RakNet::BitStream bsOut;
	RakNet::RakString tmsg;
	tmsg.Clear();
	tmsg = text;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(BILATERAL_MESSAGE);
	bsOut.Write(tmsg);
	bsOut.Write(hex);
	if(chatId != 0) bsOut.Write(chatId);
	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
}

void sendAllMsg(const char* text, unsigned int hex, int chatId)
{
	RakNet::BitStream bsOut;
	RakNet::RakString tmsg;
	tmsg.Clear();
	tmsg = text;
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(BILATERAL_MESSAGE);
	bsOut.Write(tmsg);
	bsOut.Write(hex);
	if(chatId != 0) bsOut.Write(chatId);

	for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
	}
}

void streamColoredMsg(players::player& p, int msgId, int chatId)
{
	RakNet::BitStream bsOut;
	RakNet::RakString tmsg;
	tmsg.Clear();

	unsigned int size = apiColoredChat::getMsgsSize(msgId);
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(COLORED_MSG);
	bsOut.Write(size);

	for(unsigned int i = 0; i < size; i++)
	{
		unsigned int hex = 0xFFFFFFFF;
		std::string str;

		apiColoredChat::copyMsg(msgId, i, str, hex);
		tmsg.Clear();
		tmsg = str.c_str();
		bsOut.Write(tmsg);
		bsOut.Write(hex);
	}
	if(chatId != 0) bsOut.Write(chatId);

	networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p.ip, false);
}

void sendAllColoredMsg(int msgId, int chatId)
{
	RakNet::BitStream bsOut;
	RakNet::RakString tmsg;
	tmsg.Clear();

	unsigned int size = apiColoredChat::getMsgsSize(msgId);
	bsOut.Write((MessageID)IVMP);
	bsOut.Write(COLORED_MSG);
	bsOut.Write(size);

	for(unsigned int i = 0; i < size; i++)
	{
		unsigned int hex = 0xFFFFFFFF;
		std::string str;

		apiColoredChat::copyMsg(msgId, i, str, hex);
		tmsg.Clear();
		tmsg = str.c_str();
		bsOut.Write(tmsg);
		bsOut.Write(hex);
	}
	if(chatId != 0) bsOut.Write(chatId);

	for(std::map<uint64_t, players::player>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, i->second.ip, false);
	}
}
