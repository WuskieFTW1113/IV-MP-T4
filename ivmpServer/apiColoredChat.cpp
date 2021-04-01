#include "apiColoredChat.h"
#include <map>
#include <vector>

struct coloredMsg
{
	std::string msg;
	unsigned int hex;
};

std::map<int, std::vector<coloredMsg> > coloredMsgs;

void apiColoredChat::create(int id)
{
	std::vector<coloredMsg> v;
	coloredMsgs[id] = v;
}

void apiColoredChat::dispose(int id)
{
	coloredMsgs.erase(id);
}

bool apiColoredChat::isValid(int id)
{
	return coloredMsgs.find(id) != coloredMsgs.end();
}

void apiColoredChat::addMsg(int id, const char* msg, unsigned int hexColor)
{
	coloredMsg m;
	m.msg = std::string(msg);
	m.hex = hexColor;
	coloredMsgs[id].push_back(m);
}
	
unsigned int apiColoredChat::getMsgsSize(int id)
{
	return coloredMsgs.at(id).size();
}

void apiColoredChat::copyMsg(int msgId, int msgSubId, std::string& msg, unsigned int& hexColor)
{
	msg = coloredMsgs.at(msgId).at(msgSubId).msg;
	hexColor = coloredMsgs.at(msgId).at(msgSubId).hex;
}