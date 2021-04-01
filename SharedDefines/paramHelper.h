#include <string>
#include <vector>

namespace paramHelper
{
	void getParams(std::vector<std::string>& words, std::string s, const char* splitAt);

	int isInt(std::string s);
	unsigned long isUnsignedInt(std::string s, bool allowHex);

	float isFloat(std::string s);

	std::string reverseSplit(std::vector<std::string>& v, size_t start);

	std::string DwordToString(int* list, int size);
}