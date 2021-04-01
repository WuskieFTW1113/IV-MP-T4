#include "execution.h"
#include <string>
#include "easylogging++.h"

bool mustUnloadThread = false;
//bool hasThread = false;

bool isSpawned = false;
int t_isEFLC = 0;

unsigned int SCRIPTFUNCTION = 0;

std::string unloadMsg = "~b~Unloading";

execution::threadFunc threadF[3] = {NULL, NULL, NULL};

void execution::registerThreadFunctions(threadFunc pulse, threadFunc unload, threadFunc disconnect)
{
	threadF[0] = pulse, threadF[1] = unload, threadF[2] = disconnect;
	mustUnloadThread = false;
}

void execution::callUnloadFunction()
{
	mustUnloadThread = false;
	threadF[1]();
	threadF[0] = NULL;
}

bool execution::hasGameThread()
{
	return threadF[0] != NULL;
}

void execution::requestThreadUnload()
{
	mustUnloadThread = true;
}

bool execution::shouldUnloadThread()
{
	return mustUnloadThread;
}

int execution::pulseThread()
{
	return threadF[0] != NULL ? threadF[0]() : 200;
}

void execution::callMustDisconnect()
{
	threadF[2]();
}

bool execution::hasSpawned()
{
	return isSpawned;
}

void execution::setSpawned(bool b)
{
	isSpawned = b;
}

void execution::setUnloadMsg(const char* tunloadMsg)
{
	unloadMsg = tunloadMsg;
	mustUnloadThread = true;
	LINFO << "New unload msg: " << unloadMsg;
}

const char* execution::getUnloadMsg()
{
	return unloadMsg.c_str();
}

std::string serverIp;
std::string serverName;
int serverPort;
void execution::setServerIp(const char* ip, int port, const char* svrName)
{
	serverIp = std::string(ip);
	serverName = std::string(svrName);
	serverPort = port;
}

const char* execution::getServerIp()
{
	return serverIp.c_str();
}

const char* execution::getServerName()
{
	return serverName.c_str();
}

int execution::getServerPort()
{
	return serverPort;
}

std::string g_version;
void execution::setVersion(const char* version)
{
	g_version = version;
}

const char* execution::getVersion()
{
	return g_version.c_str();
}

void execution::setPatch(int i)
{
	t_isEFLC = i;
}

int execution::getPatch()
{
	return t_isEFLC;
}

void execution::defineNatives(unsigned int natives)
{
	SCRIPTFUNCTION = natives;
}

unsigned int execution::getNatives()
{
	return SCRIPTFUNCTION;
}