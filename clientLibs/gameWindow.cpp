#include "gameWindow.h"
#include <Windows.h>
#include <windowsx.h>
#include "execution.h"
#include "easylogging++.h"

WNDPROC m_wWndProc = NULL;
HWND win = NULL;
bool gw_isActive = false;
gameWindow::windowCallBack windowCB = NULL;
gameWindow::windowMsgCallBack windowMsg = NULL;


LRESULT APIENTRY WndProc_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(GetForegroundWindow() == win)
	{
		if(!gw_isActive)
		{ 
			if(windowCB != NULL && execution::hasGameThread()) {
				windowCB(true);
			}
		}
		gw_isActive = true;

		if(windowMsg != NULL)
		{
			windowMsg(hWnd, uMsg, wParam, lParam);
			//windowMsg(win, msg, wp, lp);
		}
	}
	else
	{
		if(gw_isActive)
		{
			if(windowCB != NULL && execution::hasGameThread()) {
				windowCB(false);
			}
			//UnhookWindowsHookEx(mouseHook);
			//mouseHook = NULL;
		}		
		gw_isActive = false;
	}
	return CallWindowProc(m_wWndProc, hWnd, uMsg, wParam, lParam);
}

void gameWindow::registerWindowMsgs(windowMsgCallBack f)
{
	windowMsg = f;
}

void gameWindow::defineWindow()
{
	win = GetForegroundWindow();
	m_wWndProc = SubclassWindow(win, WndProc_Hook);
	//mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProc, NULL, NULL);
	//setMouseHook(true);
}

bool gameWindow::isWindowActive()
{
	return gw_isActive;
}

void gameWindow::registerWindowCb(windowCallBack f)
{
	windowCB = f;
}