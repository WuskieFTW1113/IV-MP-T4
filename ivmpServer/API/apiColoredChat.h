#include <string>
#include <exporting.h>

namespace apiColoredChat
{
	DLL void create(int id);
	DLL void dispose(int id);
	DLL bool isValid(int id);

	DLL void addMsg(int id, const char* msg, unsigned int hexColor); //Hex is ARGB
	
	unsigned int getMsgsSize(int id);
	void copyMsg(int msgId, int msgSubId, std::string& msg, unsigned int& hexColor);
}