#include <vector>
#include "../GWEN/Include/Gwen/Controls/ListBox.h"

struct server
{
	std::string lock;
	std::string featured;
	std::string players;
	std::string maxPlayers;
	std::string name;
	std::string location;
	std::string website;
	std::string ip;
	std::string version;
	std::string port;

	Gwen::Controls::Layout::TableRow* row;
};

void initStream();
void pulseStream();
void serverArrived(server s);
void setServerOptions(int i);
bool streamActive();
int streamProgress();
void serversDownloaded();

void getParams(std::vector<std::string>& words, std::string s, const char* splitAt);

extern std::string version;
extern int v_LCC;
