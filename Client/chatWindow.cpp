#include "chatWindow.h"
#include "guiManager.h"

#include "../GWEN/Include/Gwen/Input/Windows.h"
#include "../GWEN/Include/Gwen/Controls/TextBox.h"

#include "players.h"
#include "easylogging++.h"
#include "parseConnectionStates.h"
#include "padCallBack.h"
#include "CustomFiberThread.h"
#include "playerList.h"
#include "keyboard.h"
#include "prepareClientToServerPacket.h"
#include "../SharedDefines/packetsIds.h"
#include <mutex>
#include "chatInput.h"

struct chatMsg
{
	std::string msg;
	unsigned int hex;
};

//std::map<int, std::vector<chatMsg> > chatWindowMsgs;
//std::vector<chatMsg> chatDraw[10];
//std::vector<chatMsg> chatWindowMsgs[40];
std::mutex chatMutex;

LONG ACTUAL_FONT_SIZE;
bool chatMapInited = false;
bool fontTested = false;
ID3DXFont* chatFont = NULL;

//Gwen::Controls::TextBox* chatInput = nullptr;
//Gwen::Input::Windows* chatGwenInput = nullptr;

/*unsigned int currentScroll = 1;
unsigned int scrollBegin = 30;
unsigned int scrollEnd = 40;*/

bool chatActive = false;
bool chatVisible = true;
std::mutex chatInputMutex;

char chat_font[30] = "Arial";
int chat_size = 24;
int chat_line_height2 = 1;
int chat_quality = 4;
size_t current_chat = 0;

class chat_class
{
private:
	std::vector<chatMsg>* msgs[40];
	int scroll;

public:
	chat_class()
	{ 
		for(int i = 0; i < 40; i++) this->msgs[i] = new std::vector<chatMsg>;
		scroll = 1;
	}
	~chat_class(){}

	void addMsg(std::vector<chatMsg>* tmsg)
	{
		delete this->msgs[39];
		for(int i = 39; i > 0; i--)
		{
			this->msgs[i] = this->msgs[i - 1];
		}
		this->msgs[0] = tmsg;

		size_t thisMsg = tmsg->size();
		std::string logBuf;
		for(size_t i = 0; i < thisMsg; i++) logBuf.append(tmsg->at(i).msg);
		LINFO << logBuf;
	}

	int render(IDirect3DDevice9* pDevice, RECT r)
	{
		RECT shadow;
		LONG bufLeft = r.left;
		LONG bufRight = r.right;

		chatMutex.lock();
		int cScroll = scroll * 10 - 1;
		int scrollOver = cScroll - 9;

		//Loops buf
		int i = cScroll;
		size_t msgVecSize = 0;
		size_t vecPos = 0;

		//Vector and Chat Line bufs
		chatMsg* bufMsg = NULL;
		std::vector<chatMsg>* bufVector = NULL;
		char* msg = "";

		for(i; i >= scrollOver; i--)
		{
			if(i != cScroll) {
				r.top = r.bottom + 1;
				r.bottom += chat_size;
				r.left = bufLeft, r.right = bufRight; //Because of colored text
			}

			shadow = r;
			shadow.top = r.top + 1;
			shadow.bottom = r.bottom + 1;

			bufVector = this->msgs[i];
			msgVecSize = bufVector->size();
			if(msgVecSize == 0)
			{
				continue;
			}
			if(bufVector->at(0).msg.empty())
			{
				continue;
			}

			vecPos = 0;
			for(vecPos; vecPos < msgVecSize; vecPos++)
			{
				bufMsg = &bufVector->at(vecPos);
				msg = &bufMsg->msg[0u];
				if(chatFont->DrawText(NULL, msg, -1, &r, DT_CALCRECT, 0) != 0)
				{
					ACTUAL_FONT_SIZE = r.bottom - r.top;
					shadow.left = r.left + 1;
					shadow.right = r.right + 1;
					chatFont->DrawText(NULL, msg, -1, &shadow, DT_NOCLIP, 0xFF000000);
					chatFont->DrawText(NULL, msg, -1, &r, DT_NOCLIP, bufMsg->hex);
					r.left = r.right; // offset for next character.
				}
			}
		}
		chatMutex.unlock();
		return shadow.bottom;
	}

	void changeScroll(bool up)
	{
		if((up && this->scroll == 4) || (!up && this->scroll == 1)) return;
		chatMutex.lock();
		this->scroll = up ? scroll + 1 : scroll - 1;
		chatMutex.unlock();
	}

	void wipe()
	{
		for(int i = 0; i < 40; i++) this->msgs[i]->clear();
	}
};

std::map<int, chat_class> cClasses;

void setMsgScroll(unsigned int begin)
{
/*	unsigned int end = begin + 10;
	if(begin > 30 || end > 40 || begin > end)
	{
		LINFO << "!Error: setMsgScroll: " << begin << ", " << end;
		begin = 30;
		end = 40;
	}

	unsigned int i = 0;
	chatMutex.lock();
	for(begin; begin < end; begin++)
	{
		chatDraw[i] = chatWindowMsgs[begin];
		i++;
	}
	chatMutex.unlock();*/
}

void chatWindow::inputChat(DWORD key, bool down, bool alt)
{
	chatInputMutex.lock();
	chatInput::process(key, down, alt);
	chatInputMutex.unlock();
}

void chatCreateDevice(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters)
{
	if(!chatMapInited)
	{
		/*for(int i = 0; i < 40; i++)
		{
			std::vector<chatMsg> trash;
			chatWindowMsgs[i] = trash;
		}*/
		chatMapInited = true;
	}
}

void chatResetDevice(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters)
{
	LINFO << "chatResetDevice";
	/*chatInput = new Gwen::Controls::TextBox(guiManager::getGwenCanvas());
	chatInput->SetPos(guiManager::windowX() * 0.006f, guiManager::windowY() * 0.25f);
	chatInput->SetSize(guiManager::windowX() * 0.2f, guiManager::windowY() * 0.019f);
	chatInput->SetEditable(true);
	chatInput->Hide();

	chatGwenInput = new Gwen::Input::Windows();
	chatGwenInput->Initialize(guiManager::getGwenCanvas());*/
	/*scrollBegin = 32;
	scrollEnd = 40;
	scrollBegin = 32;
	scrollEnd = 40;*/

	if(chat_size == 24) //Standard size
	{	
		chat_size = 13;
		if(guiManager::windowX() > 4000) chat_size = 45;
		else if(guiManager::windowX() > 3000) chat_size = 40;
		else if(guiManager::windowX() > 1900) chat_size = 26;
		else if(guiManager::windowX() > 1600) chat_size = 24;
		else if(guiManager::windowX() > 1000) chat_size = 20;
		else if(guiManager::windowX() > 700) chat_size = 15;
	}
	D3DXCreateFont(pDevice, chat_size, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, chat_font, &chatFont);
	//setMsgScroll(scrollBegin);
}

void chatLostDevice(IDirect3DDevice9* pDevice, bool clientControlled)
{
	if(chatFont == NULL)
	{
		return;
	}

	LINFO << "chatLostDevice";
	//chatInput = nullptr;
	chatFont->Release();
	chatFont = NULL;
}

void chatDestroyDevice(IDirect3DDevice9* pDevice)
{
}

void DrawLine(LPDIRECT3DDEVICE9 Device_Interface,int bx,int by,int bw,D3DCOLOR COLOR)
{
     D3DRECT rec;
    rec.x1 = bx-bw;//makes line longer/shorter going lef
    rec.y1 = by;///base y
     rec.x2 = bx+bw;//makes line longer/shorter going right
    rec.y2 = by+1;//makes line one pixel tall
    Device_Interface->Clear(1,&rec,D3DCLEAR_TARGET,COLOR,0,0);
 }

void DrawTrueLine(IDirect3DDevice9* pDevice, float x1, float y1, float x2, float y2, float width, bool antialias, DWORD color)
{
	ID3DXLine *m_Line;

	D3DXCreateLine(pDevice, &m_Line);
	D3DXVECTOR2 line[] = {D3DXVECTOR2(x1, y1), D3DXVECTOR2(x2, y2)};
	m_Line->SetWidth(width);
	if(antialias) m_Line->SetAntialias(1);
	m_Line->Begin();
	m_Line->Draw(line, 2, color);
	m_Line->End();
	m_Line->Release();
}

void chatRender(IDirect3DDevice9* pDevice)
{
	if(!chatVisible || playerList::isListActive())
	{
		return;
	}
	else if(!chatFont || !chatMapInited)
	{
		LINFO << "Chat error: " << chatFont << ", " << chatMapInited;
		return;
	}

	RECT r;
	r.top = (guiManager::windowY() * 0.028f);
	r.bottom =  r.top + chat_size;
	r.left = guiManager::windowX() * 0.006f;
	r.right = guiManager::windowX();

	r.top = cClasses[current_chat].render(pDevice, r);

	if(chatActive)
	{
		chatInputMutex.lock();
		try
		{
			r.top++;
			if(r.top <= r.bottom || r.bottom > guiManager::windowY()) r.top = r.bottom + ACTUAL_FONT_SIZE + 1;
			r.bottom = r.top + chat_size;
			r.left = guiManager::windowX() * 0.006f;
			r.right = guiManager::windowX();

			if(chatInput::lastRepeat != 0 && !chatInput::lastChar.empty())
			{
				clock_t cTime = clock();
				if(cTime > chatInput::lastRepeat + 1000)
				{
					chatInput::lastRepeat = cTime;
					if(chatInput::lastChar == std::string("back"))
					{
						chatInput::popBackText();
					}
					else
					{
						chatInput::text.append(chatInput::lastChar);
					}
				}
			}

			std::string buf = chatInput::text;
			size_t s = buf.size();
			if(s > 0 && buf.at(s - 1) == ' ')
			{
				buf.at(s - 1) = '_';
				//LINFO << "White space";
			}
			const char* msg = buf.c_str();

			RECT shadow;
			chatFont->DrawText(NULL, msg, -1, &r, DT_CALCRECT, 0);
			shadow.left = r.left + 1;
			shadow.right = r.right + 1;
			shadow.bottom = r.bottom;
			shadow.top = r.top;

			chatFont->DrawText(NULL, msg, -1, &shadow, DT_NOCLIP, 0xFF000000);
			chatFont->DrawText(NULL, msg, -1, &r, DT_NOCLIP, 0xFFFFFFFF);

			shadow.right = r.right + 3;
			DrawLine(pDevice, shadow.right, shadow.bottom, 2, 0xFFFFFFFF);
		}
		catch(std::exception& e)
		{
			LINFO << "!Error: chatInput: " << e.what();
		}
		chatInputMutex.unlock();
	}
}

void chatUpdateMsg(MSG msg)
{
	/*if(chatInput != nullptr && chatInput->Visible())
	{
		if(msg.wParam == VK_BACK)
		{
			std::string newString = std::string(chatInput->GetText().c_str());
			if(newString.size() != 0)
			{
				newString.erase(newString.end() - 1, newString.end());
				chatInput->SetText(newString.c_str());
			}
		}
		else
		{
			chatGwenInput->ProcessMessage(msg);
		}
	}
	else
	{
		chatGwenInput->ProcessMessage(msg);
	}*/
}

void chatWindow::showChatWindow(bool show)
{
	if(show)
	{
		guiManager::guiPanel* chatPanel = new guiManager::guiPanel;
		chatPanel->create = &chatCreateDevice;
		chatPanel->destroy = &chatDestroyDevice;
		chatPanel->lost = &chatLostDevice;
		chatPanel->render = &chatRender;
		chatPanel->reset = &chatResetDevice;
		chatPanel->msgUpdate = &chatUpdateMsg;

		guiManager::addPanel(guiManager::CHAT_WINDOW, chatPanel);
	}
	else
	{
		//guiManager::deletePanel(guiManager::CHAT_WINDOW);
	}
}

bool chatWindow::isChatWindowActive()
{
	return chatFont != NULL;
}

void addMsg(std::vector<chatMsg>& vec)
{
/*	if(!chatMapInited)
	{
		LINFO << "!Warning: colored chat msgs denied (map not ready)";
		return;
	}

	for(size_t i = 0; i < 39; i++)
	{
		chatWindowMsgs[i] = chatWindowMsgs[i + 1];
	}

	chatWindowMsgs[39] = vec;
	std::string str;
	size_t vecSize = vec.size();
	for(size_t i = 0; i < vecSize; ++i)
	{
		str.append(vec.at(i).msg);
	}
	setMsgScroll(scrollBegin);
	LINFO << str;*/
}

void chatWindow::receivedChat(RakNet::BitStream& bsIn)
{
	RakNet::RakString msg;
	bsIn.Read(msg);

	chatMsg m;
	m.msg = std::string(msg);
	bsIn.Read(m.hex);

	std::vector<chatMsg>* nv = new std::vector<chatMsg>;
	nv->push_back(m);
	//addMsg(nv);

	int customChat = 0;
	bsIn.Read(customChat);
	if(customChat != 0 && cClasses.find(customChat) == cClasses.end()) return;

	cClasses.at(customChat).addMsg(nv);
}

void chatWindow::receivedPlayerChat(RakNet::BitStream& bsIn)
{
	int msgsCount = 0;
	bsIn.Read(msgsCount);

	if(msgsCount <= 0 || msgsCount > 10)
	{
		LINFO << "!Warning: colored chat msgs denied (invalid size)";
		return;
	}

	std::vector<chatMsg>* nv = new std::vector<chatMsg>;
	RakNet::RakString msg;

	for(int i = 0; i < msgsCount; i++)
	{
		chatMsg m;
		msg.Clear();
		bsIn.Read(msg);
		bsIn.Read(m.hex);
		m.msg = std::string(msg);
		nv->push_back(m);
	}

	int customChat = 0;
	bsIn.Read(customChat);
	if(customChat != 0 && cClasses.find(customChat) == cClasses.end()) return;
	//addMsg(nv);
	cClasses.at(customChat).addMsg(nv);
}

void chatWindow::customChatHandler(RakNet::BitStream& bsIn)
{
	int chatId = 0;
	bsIn.Read(chatId);
	if(chatId < 0) return;

	bool exists = cClasses.find(chatId) != cClasses.end();

	//OP list: 1 new, 2 delete, 3 wipe, 4 switch
	int op = 0;
	bsIn.Read(op);

	//Trying to alter a chat that doesnt exist
	if((op != 1 && !exists) || (op == 1 && exists)) return;

	chatMutex.lock();
	if(op == 1)
	{
		cClasses[chatId] = chat_class();
	}
	else if(op == 2)
	{
		if(current_chat == chatId) current_chat = 0;
		cClasses.erase(chatId);
	}
	else if(op == 3)
	{
		cClasses[chatId].wipe();
	}
	else if(op == 4)
	{
		current_chat = chatId;
	}
	chatMutex.unlock();

	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(CUSTOM_CHAT);
	b.Write(chatId);
	b.Write(op);
	networkManager::sendBitStream(b);
}
	
void chatWindow::toggleChatInput()
{
	//togglePlayerControls(false, false);
	/*chatInput->Show();
	chatInput->Focus();*/
	chatActive = true;
}

bool chatWindow::isChatInputActive()
{
	return chatActive;//chatInput != nullptr ? chatInput->Visible() : false;
}

bool chatWindow::pageScrollAvailable()
{
	return true;
}

void chatWindow::changeScroll(bool scrollUp)
{
	/*if(scrollUp)
	{
		if(currentScroll > 3)
		{
			LINFO << "Scroll up failed: limit reached: " << currentScroll;
			return;
		}
		currentScroll++;
	}
	else if(!scrollUp )
	{
		if(currentScroll < 2)
		{
			LINFO << "Scroll down failed: limit reached: " << currentScroll;
			return;
		}
		currentScroll--;
	}
	scrollBegin = 40 - (currentScroll * 10);
	scrollEnd = scrollBegin + 10;
	setMsgScroll(scrollBegin);*/
	cClasses.at(current_chat).changeScroll(scrollUp);
}

bool PressT = false;
void chatWindowPressT(bool altDown)
{
	PressT = true;
}

bool PressEnter = false;
void chatWindowPressEnter(bool altDown)
{
	PressEnter = true;
}

bool PressPageUp = false;
void chatWindowPressPageUp(bool altDown)
{
	PressPageUp = true;
}

bool PressPageDown = false;
void chatWindowPressPageDown(bool altDown)
{
	PressPageDown = true;
}

bool PressArrowUp = false;
void chatWindowPressArrowUp(bool altDown)
{
	PressArrowUp = true;
}

bool PressArrowDown = false;
void chatWindowPressArrowDown(bool altDown)
{
	PressArrowDown = true;
}

bool PressEscUp = false;
void chatWindowPressEsc(bool altDown)
{
	PressEscUp = true;
}

void chatWindowPressF6(bool altDown)
{
	chatVisible = !chatVisible;
}

void chatWindow::initKeyHook()
{
	keyboard::registerFunc(0x54, chatWindowPressT, false);
	keyboard::registerFunc(0xC0, chatWindowPressT, false);
	keyboard::registerFunc(VK_RETURN, chatWindowPressEnter, true);
	keyboard::registerFunc(0x21, chatWindowPressPageUp, true);
	keyboard::registerFunc(0x22, chatWindowPressPageDown, true);
	keyboard::registerFunc(VK_UP, chatWindowPressArrowUp, true);
	keyboard::registerFunc(VK_DOWN, chatWindowPressArrowDown, true);
	keyboard::registerFunc(VK_ESCAPE, chatWindowPressEsc, false);
	keyboard::registerFunc(VK_F6, chatWindowPressF6, true);
}

void chatWindow::handleKeyPress()
{
	if(PressT) //t
	{
		PressT = false;
		if(isChatWindowActive() && !isChatInputActive())
		{
			subTask = 10;
			//if(!isPlayerEnteringCar())
			//{
				togglePlayerControls(CONTROL_CHAT, false, false);
			//}
			toggleChatInput();
			//NativeInvoke::Invoke<u32>("DISABLE_PAUSE_MENU", 1);
		}
	}
	else if(PressEscUp)
	{
		PressEscUp = false;
		chatActive = false;
		if(!shouldPlayerBeFrozen())
		{
			togglePlayerControls(CONTROL_CHAT, true, false);
		}
	}
	else if(PressEnter) //enter
	{
		PressEnter = false;
		if(!isChatWindowActive() || !isChatInputActive())
		{
			return;
		}
		//NativeInvoke::Invoke<u32>("DISABLE_PAUSE_MENU", 0);

		//std::string inputText = chat_text;//std::string(chatInput->GetText().c_str());
		chatInputMutex.lock();
		try
		{
			bool size = chatInput::text.size() > 1;
			if(size)
			{
				//receivedChat(label->GetText().c_str());
				std::string inputText = chatInput::text.substr(1, chatInput::text.size());
				//LINFO << inputText;
				if(networkManager::isNetworkActive())
				{
					if(inputText == std::string("/q"))
					{
						connectionLost("~b~Exiting server");
						chatInput::enter(size);
						chatInputMutex.unlock();
						//chatInput->SetText("");
						//chatInput->Hide();			
						//Scripting::SetPlayerControl(players::getLocalPlayer().playerIndex, true);
						return;
					}
					RakNet::BitStream bsOut;
					RakNet::RakString text;
					text.Clear();
					text = inputText.length() > 100 ? inputText.substr(0, 100).c_str() : inputText.c_str();
					bsOut.Write((MessageID)IVMP);
					bsOut.Write(BILATERAL_MESSAGE);
					bsOut.Write(text);

					networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
						networkManager::getConnection()->serverIp, false);
				}
			}
			//chatInput->SetText("");
			//chatInput->Hide();
			chatInput::enter(size);
			chatActive = false;

			if(!shouldPlayerBeFrozen())
			{
				togglePlayerControls(CONTROL_CHAT, true, false);
			}
		}
		catch(std::exception& e)
		{
			LINFO << "!Error: ChatInput pressed enter: " << e.what();
		}
		chatInputMutex.unlock();
	}
	else if(PressPageUp/*keyboard::getKey(0x21)*/)
	{
		PressPageUp = false;
		if(playerList::isListActive())
		{
			playerList::scrollUp();
			return;
		}
		else if(!chatWindow::pageScrollAvailable()) //page up
		{
			return;
		}
		subTask = 11;
		changeScroll(true);
	}
	else if(PressPageDown/*keyboard::getKey(0x22)*/)
	{
		PressPageDown = false;
		if(playerList::isListActive())
		{
			playerList::scrollDown();
			return;
		}
		else if(!chatWindow::pageScrollAvailable()) //page up
		{
			return;
		}
		subTask = 121;
		changeScroll(false);
	}
}

#include <fstream>
#include "../SharedDefines/paramHelper.h"

void chatWindow::loadConfig()
{
	cClasses[0] = chat_class();
	
	std::ifstream infile("comMP//config.txt", std::ifstream::in);
	if(infile.is_open())
	{
		std::string line;
		while(std::getline(infile, line))
		{
			if(line.compare(0, 10, std::string("chat_font=")) == 0)
			{
				line.erase(0, 10);
				//chat_font = &line[0u];
				sprintf_s(chat_font, "%s", line.substr(0, std::string::npos).c_str());
			}
			else if(line.compare(0, 10, std::string("chat_size=")) == 0)
			{
				line.erase(0, 10);
				chat_size = paramHelper::isInt(line);
				if(chat_size > 42 || chat_size < 10) chat_size = 24;
			}
			else if(line.compare(0, 17, std::string("chat_line_height=")) == 0)
			{
				line.erase(0, 17);
				chat_line_height2 = paramHelper::isInt(line);
				if(chat_line_height2 < 1 || chat_line_height2 > 10) chat_line_height2 = 1;
			}
			else if(line.compare(0, 13, std::string("chat_quality=")) == 0)
			{
				line.erase(0, 13);
				chat_quality = paramHelper::isInt(line);
			}
		}
	}
	else LINFO << "Config.txt did not open: " << strerror(errno);
	infile.close();
}
