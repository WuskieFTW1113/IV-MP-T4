#include <Windows.h>

namespace keyHook
{
	__declspec(dllexport) typedef void(*keyStateFunc)();
}