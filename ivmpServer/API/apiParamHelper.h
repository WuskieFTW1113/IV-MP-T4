#include <string>
#include <vector>
#include <exporting.h>

#ifndef apiparamhelper_h
#define apiparamhelper_h

namespace apiParamHelper
{
	DLL int isInt(const char* s);
	DLL unsigned long isUnsignedInt(const char*, bool allowHex);

	DLL float isFloat(const char* s);
}

#endif