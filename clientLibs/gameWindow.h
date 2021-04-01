#include <Windows.h>

namespace gameWindow
{
	__declspec(dllexport) void defineWindow();

	__declspec(dllexport) bool isWindowActive();

	__declspec(dllexport) typedef void(*windowCallBack)(bool active);
	__declspec(dllexport) void registerWindowCb(windowCallBack f);

	__declspec(dllexport) typedef void(*windowMsgCallBack)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	__declspec(dllexport) void registerWindowMsgs(windowMsgCallBack f);

	__declspec(dllexport) typedef void(*mouseMsgCallBack)(MSG& m);
	__declspec(dllexport) void registerWindowMouse(mouseMsgCallBack f);
}