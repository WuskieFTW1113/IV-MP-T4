#include "chatInput.h"
#include <Windows.h>
#include <time.h>
#include "easylogging++.h"
#include <regex>

long chatInput::lastRepeat = 0;
bool isControlDown = false;
std::string chatInput::text = ">";
std::string chatInput::lastChar = "";


BYTE kstate[256] = { 0 };

std::string myLastMsgs[10];
unsigned int myLastMsgScroll = 11;

void chatInput::popBackText()
{
	if(chatInput::text.size() > 1)
	{
		chatInput::text.pop_back();
	}
}

void chatInput_keyUp()
{
	chatInput::lastChar = "";
	chatInput::lastRepeat = 0;
}

void chatInput_backSpace()
{
	chatInput::popBackText();
	chatInput::lastChar = "back";
	chatInput::lastRepeat = clock();
	//Scripting::PrintStringWithLiteralStringNow("STRING", chat_text.c_str(), 4000, true);
}

void chatInput_paste()
{
	if(OpenClipboard(NULL))
	{
		//LINFO << "Control Ve";
		HANDLE clip;
		clip = GetClipboardData(CF_TEXT);
		if(clip != NULL)
		{
			std::string buf = (LPSTR)GlobalLock(clip);
			if(std::regex_match(buf, std::regex("^[\x20-\x7F]+$")))
			{
				chatInput::text.append(buf);
				if(chatInput::text.size() > 100)
				{
					chatInput::text = chatInput::text.substr(0, 100);
				}
				chatInput::text.erase(std::remove(chatInput::text.begin(), chatInput::text.end(), '\n'), chatInput::text.end());
			}
			//LINFO << chat_text;
			GlobalUnlock(clip);
		}
		CloseClipboard();
	}
}

void chatInput_getInput(unsigned long key)
{
	if(chatInput::text.size() > 100 || GetKeyboardState(kstate) == 0)
	{
		return;
	}
	
	/*if(isControlDown)
	{
		return;
	}*/

	HKL keyboard_layout = GetKeyboardLayout(0);
	unsigned int scan = MapVirtualKey(key, MAPVK_VK_TO_VSC);
	wchar_t buffer[5];
	int result = ToUnicodeEx(key, scan, kstate, buffer, 5, 0, keyboard_layout);
	if(result == 1)
	{
		std::wstring bufString(buffer);
		std::string tString(bufString.begin(), bufString.end());
		if((tString == std::string("!") && key != 0x31) || !std::regex_match(tString, std::regex("^[\x20-\x7F]+$")))
		{
			return;
		}
		chatInput::text.append(tString);
		//LINFO << chatInput::text;
		chatInput::lastChar = tString;
		chatInput::lastRepeat = clock();
		//LINFO << tString << ", " << chat_text;
		//Scripting::PrintStringWithLiteralStringNow("STRING", chat_text.c_str(), 4000, true);
	}
}

void chatInput_changeMsgScroll(bool arrowUp)
{
	unsigned int newScroll = 0;
	if(arrowUp)
	{
		LINFO << "My msgs: UP: " << myLastMsgScroll;
		newScroll = myLastMsgScroll == 11 || myLastMsgScroll + 1 > 9 ? 0 : myLastMsgScroll + 1;
	}
	else if(!arrowUp)
	{
		LINFO << "My msgs: DOWN: " << myLastMsgScroll;
		if(myLastMsgScroll == 11 || myLastMsgScroll == 0)
		{
			chatInput::text = ">";
			return;
		}
		newScroll = myLastMsgScroll == 11 || myLastMsgScroll == 0 ? 0 : myLastMsgScroll - 1;
	}

	if(myLastMsgs[newScroll].empty())
	{
		return;
	}
	myLastMsgScroll = newScroll;
	chatInput::text = myLastMsgs[myLastMsgScroll];

	chatInput::lastChar = "";
	chatInput::lastRepeat = 0;
}

void chatInput::process(unsigned long key, bool down, bool alt)
{
	if(!down)
	{
		chatInput_keyUp();
		if(key == VK_CONTROL)
		{
			isControlDown = false;
			//LINFO << "Control: up";
		}
	}
	else if(key == VK_BACK)
	{
		chatInput_backSpace();
	}
	else if(key == VK_SPACE)
	{
		text.push_back(' ');
		return;
	}
	else if(key == VK_UP)
	{
		chatInput_changeMsgScroll(true);
	}
	else if(key == VK_DOWN)
	{
		chatInput_changeMsgScroll(false);
	}
	else if(key == VK_CONTROL)
	{
		isControlDown = true;
		//LINFO << "Control: down";
	}
	else if(key == 0x56 && isControlDown) //V
	{
		chatInput_paste();
	}
	else if((key >= 0x01 && key <= 0x2F) || (key >= VK_F1 && key <= VK_F24) || (key >= VK_NUMLOCK && key <= VK_SCROLL) ||
		(key >= VK_LSHIFT && key <= VK_LAUNCH_APP2)) {
		return;
	}
	else
	{
		chatInput_getInput(key);
	}
}

void chatInput::enter(bool hasMsg)
{
	myLastMsgScroll = 11;
	if(hasMsg)
	{
		for(unsigned int l = 9; l >= 1; l--)
		{
			myLastMsgs[l] = myLastMsgs[l - 1];
		}
		myLastMsgs[0] = chatInput::text;
	}
	text = ">";
	lastChar = "";
	lastRepeat = 0;
}