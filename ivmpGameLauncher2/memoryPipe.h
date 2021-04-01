#include <string>

namespace memoryPipe
{
	void init();
	void pulse();
	void end();
	void writePipeMsg(int id, std::string s);
	bool gameRunning();

	bool getGTA4();

	void writeStuff();
}