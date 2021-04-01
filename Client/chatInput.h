#include <string>

namespace chatInput
{
	void process(unsigned long key, bool down, bool alt);
	void popBackText();
	void enter(bool hasMsg);

	extern long lastRepeat;
	extern std::string text;
	extern std::string lastChar;
}