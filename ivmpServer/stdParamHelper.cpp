#include "stdParamHelper.h"
#include "../SharedDefines/exporting.h"

#if linux
#include <string.h>
#define strtok_s strtok_r
#endif

void autoStrtok(std::vector<std::string>& words, std::string s, const char* splitBy)
{
	char * pch = 0;
	char *next = 0;

	char * writable = new char[s.size() + 1];
	std::copy(s.begin(), s.end(), writable);
	writable[s.size()] = '\0';

	pch = strtok_s(writable, splitBy, &next);
	while(pch != 0)
	{
		words.push_back(std::string(pch));
		pch = strtok_s(0, splitBy, &next);
	}
}
