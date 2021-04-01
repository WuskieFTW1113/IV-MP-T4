#include "playerList.h"
#include "guiManager.h"
#include "easylogging++.h"
#include <map>
#include <string>
#include "colorHelper.h"
#include "chatWindow.h"
#include "keyboard.h"
#include <mutex>
#include <exception>

std::mutex playerListMutex;
int playerListScroll = 0;

char* listServerName = "";

struct listInfo
{
	unsigned int hex;
	std::string text[3];
	//RECT pos;
};

struct drawInfo
{
	ID3DXFont* serverNameFont;	
	ID3DXFont* tableHeaderFont;
	ID3DXFont* playerNameFont;

	D3DCOLOR tableBackGround;
	D3DCOLOR tableHeaderBackGround;

	RECT backGround;
	RECT backGroundHeader;

	RECT headerPos;
	RECT tableHeaderPos[3];

	std::vector<listInfo> players;

	int heightBtnPlayer;
	int tableColumnsWidth;

	char* tableHeaderText[3];
	std::string serverHeader;

	LONG ACTUAL_FONT_SIZE;
	LONG ACTUAL_SVR_NAME_SIZE;
	LONG LONGEST_NAME_WIDTH;

	drawInfo()
	{
		serverNameFont = NULL;
		tableHeaderFont = NULL;
		playerNameFont = NULL;

		tableHeaderText[0] = "ID";
		tableHeaderText[1] = "Name";
		tableHeaderText[2] = "Ping";

		backGround.top = 0;
		ACTUAL_FONT_SIZE = 0;
		ACTUAL_SVR_NAME_SIZE = 0;
		LONGEST_NAME_WIDTH = 0;
	}
};

drawInfo l_di;
std::map<int, listInfo> playerList_list;
volatile bool playerList_show = false;

void playerListCreateDevice(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters){}

void playerListDestroyDevice(IDirect3DDevice9* pDevice)
{
	LINFO << "playerListDestroyDevice";
}

void playerListResetDevice(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters)
{
	LINFO << "playerListResetDevice";
	playerListMutex.lock();
	playerList_show = false;

	long fontIncrement = 0;
	if(guiManager::windowX() > 3000) fontIncrement = 16;
	else if(guiManager::windowX() > 2000) fontIncrement = 8;
	else if(guiManager::windowX() > 1680) fontIncrement = 4;
	else if(guiManager::windowX() > 1400) fontIncrement = 2;

	D3DXCreateFont(pDevice, 20 + fontIncrement, 0, FW_BOLD, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "bankgothic", &l_di.serverNameFont);

	D3DXCreateFont(pDevice, 15 + fontIncrement, 0, FW_BOLD, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "tahoma", &l_di.tableHeaderFont);

	D3DXCreateFont(pDevice, 15 + fontIncrement, 0, FW_BOLD, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "tahoma", &l_di.playerNameFont);
	playerListMutex.unlock();
}

void playerListLostDevice(IDirect3DDevice9* pDevice, bool clientControlled)
{
	LINFO << "playerListLostDevice";
	if(l_di.serverNameFont != NULL)
	{
		l_di.serverNameFont->Release();
	}
	if(l_di.tableHeaderFont != NULL)
	{
		l_di.tableHeaderFont->Release();
	}
	if(l_di.playerNameFont != NULL)
	{
		l_di.playerNameFont->Release();
	}
	l_di.players.clear();
	playerList_show = false;
	l_di.serverNameFont = NULL;
	l_di.tableHeaderFont = NULL;
	l_di.playerNameFont = NULL;
	LINFO << "Done";
}

struct vertex
{
    FLOAT x, y, z, rhw;
    DWORD color;
};

LPDIRECT3DVERTEXBUFFER9 g_pVB;    // Buffer to hold vertices
LPDIRECT3DINDEXBUFFER9  g_pIB;    // Buffer to hold indices

void BoxFilled(IDirect3DDevice9* pDevice, LONG x, LONG y, LONG x2, LONG y2, D3DCOLOR color)
{
	D3DRECT BarRect = {x, y, x2, y2};
	pDevice->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0);
	/*vertex V[4];
 
   V[0].color = V[1].color = V[2].color = V[3].color = color;
 
   V[0].z = V[1].z = V[2].z = V[3].z = 0;
   V[0].rhw = V[1].rhw = V[2].rhw = V[3].rhw = 0;
 
   V[0].x = x;
   V[0].y = y;
   V[1].x = x2;
   V[1].y = y;
   V[2].x = x2;
   V[2].y = y2;
   V[3].x = x;
   V[3].y = y2;
 
   unsigned short indexes[] = {0, 1, 3, 1, 2, 3};
 
   pDevice->CreateVertexBuffer(4*sizeof(vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW|D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB, NULL);
   pDevice->CreateIndexBuffer(2*sizeof(short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL);
 
   VOID* pVertices;
   g_pVB->Lock(0, sizeof(V), (void**)&pVertices, 0);
   memcpy(pVertices, V, sizeof(V));
   g_pVB->Unlock();
 
   VOID* pIndex;
   g_pIB->Lock(0, sizeof(indexes), (void**)&pIndex, 0);
   memcpy(pIndex, indexes, sizeof(indexes));
   g_pIB->Unlock();
 
   pDevice->SetTexture(0, NULL);
   pDevice->SetPixelShader(NULL);
   pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
   pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
   pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
 
   pDevice->SetStreamSource(0, g_pVB, 0, sizeof(vertex));
   pDevice->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
   pDevice->SetIndices(g_pIB);
 
   pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
 
   g_pVB->Release();
   g_pIB->Release();*/
}

void playerListRender(IDirect3DDevice9* pDevice)
{
	if(!playerList_show || !l_di.serverNameFont || !l_di.tableHeaderFont || !l_di.playerNameFont || l_di.backGround.top == 0)
	{
		//LINFO << "Player list error: " << l_di.serverNameFont << ", " << l_di.tableHeaderFont << ", " << l_di.playerNameFont;
		return;
	}
	playerListMutex.lock();

	try
	{
		if(l_di.ACTUAL_SVR_NAME_SIZE == 0)
		{
			//Redefines Header Size and Positions also (ID, Name, Ping) header position
			RECT bounds = l_di.headerPos;
			//char bufStr[15];
			//for(size_t i = 0; i < 15; i++) bufStr[i] = 'A';

			if(l_di.serverNameFont->DrawText(NULL, l_di.serverHeader.c_str(), -1, &bounds, DT_CALCRECT, 0xFFFFFFFF != 0))
			{
				l_di.headerPos = bounds;
				long headerBorders = (long)((bounds.right - bounds.left) * 0.15f);
				l_di.backGround.top = bounds.top - 2;
				l_di.backGroundHeader.bottom = bounds.bottom + 2;

				l_di.tableHeaderPos[0].top = l_di.tableHeaderPos[1].top = l_di.tableHeaderPos[2].top = l_di.backGroundHeader.bottom;

				//l_di.backGround.left = l_di.backGroundHeader.left = l_di.tableHeaderPos[0].left = bounds.left;
				//l_di.backGround.right = l_di.backGroundHeader.right = bounds.right;

				l_di.ACTUAL_SVR_NAME_SIZE = bounds.bottom - bounds.top;
			}

			//Longest ID
			bounds = l_di.tableHeaderPos[0];
			long columnSpace = 20;
			if(l_di.playerNameFont->DrawText(NULL, "999", -1, &bounds, DT_CALCRECT, 0xFFFFFFFF != 0))
			{
				columnSpace = (bounds.right - bounds.left) * 2;
				//LINFO << "ColumnSpace: " << columnSpace;
				l_di.backGround.left = l_di.backGroundHeader.left = bounds.left - (columnSpace / 2);
				l_di.tableHeaderPos[1].left = bounds.right + columnSpace;
			}

			//calculate longest player name possible
			bounds = l_di.tableHeaderPos[1];
			long namesSpace = 100;
			if(l_di.playerNameFont->DrawText(NULL, "AAAAAAAAAAAAAAAAAAAAAAAAA", -1, &bounds, DT_CALCRECT, 0xFFFFFFFF != 0))
			{
				l_di.ACTUAL_FONT_SIZE = bounds.bottom - bounds.top + 1;
				long minBottom = l_di.tableHeaderPos[0].top + ((l_di.players.size() + 3) * l_di.ACTUAL_FONT_SIZE);
				if(l_di.backGround.bottom < minBottom) l_di.backGround.bottom = minBottom;

				/*if(bounds.right > l_di.tableHeaderPos[2].left)
				{
					l_di.tableHeaderPos[2].left = bounds.right + 20;
					l_di.tableHeaderPos[2].right = l_di.tableHeaderPos[2].left + 20;
					l_di.backGround.right = l_di.tableHeaderPos[2].right + 20;
				}*/
				namesSpace = bounds.right - bounds.left + columnSpace;
				l_di.tableHeaderPos[2].left = bounds.right + columnSpace;

				//Updates bottom for players drawing
				l_di.tableHeaderPos[0].bottom = bounds.bottom;
			}

			//Checks if the ping will fit
			bounds = l_di.tableHeaderPos[2];
			if(l_di.playerNameFont->DrawText(NULL, "10000", -1, &bounds, DT_CALCRECT, 0xFFFFFFFF != 0))
			{
				l_di.backGround.right = l_di.backGroundHeader.right = bounds.right + 2;
			}

			//Center Window
			long cLeft = (guiManager::windowX() - (l_di.backGround.right - l_di.backGround.left)) / 2;
			long cRight = l_di.backGround.right - l_di.backGround.left;

			l_di.backGround.left = l_di.backGroundHeader.left = cLeft;
			l_di.backGround.right = l_di.backGroundHeader.right = cLeft + cRight;
			l_di.tableHeaderPos[0].left = l_di.backGround.left + (columnSpace / 2);
			l_di.tableHeaderPos[1].left = l_di.tableHeaderPos[0].left + columnSpace;
			l_di.tableHeaderPos[2].left = l_di.tableHeaderPos[1].left + namesSpace;

			//Center the server title
			long cHeader = l_di.headerPos.right - l_di.headerPos.left;
			//LINFO << columnSpace << ", " << (l_di.backGround.right - l_di.backGround.left) << ", " << l_di.headerPos.left;
			l_di.headerPos.left = l_di.backGround.left + (((l_di.backGround.right - l_di.backGround.left) - cHeader) / 2);
		}

		BoxFilled(pDevice, l_di.backGround.left, l_di.backGround.top, l_di.backGround.right, l_di.backGround.bottom, l_di.tableBackGround);

		BoxFilled(pDevice, l_di.backGround.left, l_di.backGround.top, l_di.backGround.right,
			l_di.backGroundHeader.bottom, l_di.tableHeaderBackGround);

		l_di.serverNameFont->DrawText(NULL, l_di.serverHeader.c_str(), -1, &l_di.headerPos, DT_LEFT | DT_NOCLIP, 0xFFFFFFFF);
		//LINFO << "Server name";

		for(int i = 0; i < 3; i++)
		{
			l_di.playerNameFont->DrawText(NULL, l_di.tableHeaderText[i], -1, &l_di.tableHeaderPos[i], DT_LEFT | DT_NOCLIP, 0xFFFFFFFF);
		}
		//LINFO << "Columns";

		std::ostringstream s;
		RECT buf;
		int cHeight = l_di.tableHeaderPos[0].bottom + 2;
		size_t pSize = l_di.players.size();

		for(size_t i = 0; i < pSize; i++)
		{
			for(int k = 0; k < 3; k++)
			{
				buf = l_di.tableHeaderPos[k];
				buf.top = cHeight;

				//LINFO << "Player " << i << ", column " << k;
				//LINFO << "TEST: " << l_di.players.at(i).text[k];

				unsigned int color = k == 1 ? l_di.players.at(i).hex : 0xFFFFFFFF;
				l_di.playerNameFont->DrawText(NULL, l_di.players.at(i).text[k].c_str(), -1, &buf, DT_LEFT | DT_NOCLIP, color);
			}
			cHeight += l_di.ACTUAL_FONT_SIZE;
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error: playerList draw: " << e.what();
	}
	playerListMutex.unlock();
}

size_t writePlayerList()
{
	try
	{
		l_di.players.clear();
		if(playerListScroll < 0 || playerListScroll > 200)
		{
			playerListScroll = 0;
		}

		size_t players = 1;
		size_t loops = 0;

		for(std::map<int, listInfo>::const_iterator i = playerList_list.begin(); i != playerList_list.end(); ++i)
		{
			if(players > 20)
			{
				//LINFO << "Enough";
				break;
			}
			else if(i->first < playerListScroll)
			{
				continue;
			}
			listInfo bufList = playerList_list.at(i->first);
			l_di.players.push_back(bufList);
			players++;
		}
		/*for(size_t i = 0; i < 20; i++)
		{
			listInfo b;
			b.text[0] = std::to_string(i);
			b.text[1] = "AAAAAAAAAAAAAAAAAAAAAAAAA";
			b.text[2] = "1000";
			b.hex = 0xFFFFFFFF;
			l_di.players.push_back(b);
		}*/
		return 5;
	}
	catch(std::exception& e)
	{
		LINFO << "!Error: writePlayerList: " << e.what();
	}
	return 0;
}

void playerList::showList(bool show)
{
	playerList_show = show;
	if(show == true)
	{
		playerListMutex.lock();
		try
		{
			playerListScroll = 0;

			l_di.ACTUAL_FONT_SIZE = 0;
			l_di.ACTUAL_SVR_NAME_SIZE = 0;
			l_di.LONGEST_NAME_WIDTH = 0;

			l_di.tableHeaderBackGround = D3DCOLOR_ARGB(190, 138, 145, 104);
			l_di.tableBackGround = D3DCOLOR_ARGB(190, 0, 0, 0);

			l_di.heightBtnPlayer = guiManager::windowY() * 0.02f;
			l_di.tableColumnsWidth = guiManager::windowX() * 0.11f;

			size_t players = writePlayerList();
			l_di.backGround.left = guiManager::windowX() * 0.33f;
			l_di.backGround.top = guiManager::windowY() * 0.12;
			l_di.backGround.right = guiManager::windowX() * 0.67f;
			l_di.backGround.bottom = l_di.backGround.top + ((guiManager::windowY() * 0.1) + (players * l_di.heightBtnPlayer));

			l_di.backGroundHeader = l_di.backGround;
			l_di.backGroundHeader.bottom = guiManager::windowY() * 0.225;

			l_di.headerPos.top = guiManager::windowY() * 0.122;
			l_di.headerPos.left = guiManager::windowX() * 0.33f + (l_di.tableColumnsWidth * 0.7f);
			l_di.headerPos.right = l_di.headerPos.left + 30;
			l_di.headerPos.bottom = l_di.headerPos.top - 20;

			RECT buf;
			buf.top = guiManager::windowY() * 0.228;
			buf.left = guiManager::windowX() * 0.33f + (l_di.tableColumnsWidth * 0.2f);
			buf.bottom = buf.top + 20;
			buf.right = buf.left + 50;

			l_di.tableHeaderPos[0] = buf;
			for(int i = 1; i < 3; i++)
			{
				l_di.tableHeaderPos[i] = l_di.tableHeaderPos[i - 1];
				l_di.tableHeaderPos[i].left += l_di.tableColumnsWidth;
				l_di.tableHeaderPos[i].right = l_di.tableHeaderPos[i].left + 50;
			}

			std::ostringstream s;
			s << listServerName << " | Players: " << playerList_list.size();
			l_di.serverHeader = s.str();
			LINFO << "Done: " << l_di.serverHeader;
		}
		catch(std::exception& e)
		{
			LINFO << "!Error: playerList show: " << e.what();
		}
		playerListMutex.unlock();
	}
	else if(show == false && l_di.playerNameFont != NULL)
	{
		//guiManager::deletePanel(guiManager::PLAYER_LIST);
		l_di.players.clear();
	}
}

bool playerList::isListActive()
{
	return playerList_show;
}

void playerList::addPlayer(RakNet::BitStream& bsIn)
{
	int id = -1;
	bsIn.Read(id);
	if(id == -1)
	{
		return;
	}
	
	RakNet::RakString str;
	listInfo i;
	int ping;
	unsigned int color;

	bsIn.Read(str);
	bsIn.Read(color);
	bsIn.Read(ping);
	
	i.hex = colorHelper::argb(color);

	std::ostringstream s;
	s << id;
	i.text[0] = s.str();
	i.text[1] = std::string(str);
	
	s.str(std::string(""));
	s << ping;
	i.text[2] = s.str();

	playerList_list[id] = i;
}

void playerList::removePlayer(RakNet::BitStream& bsIn)
{
	int id = -1;
	bsIn.Read(id);

	if(playerList_list.find(id) != playerList_list.end())
	{
		playerList_list.erase(id);
	}
}

void playerList::pingUpdated(RakNet::BitStream& bsIn)
{
	int id = -1;
	bsIn.Read(id);
	//LINFO << "Ping id: " << id;
	if(playerList_list.find(id) != playerList_list.end())
	{
		int pingN = 0;
		bsIn.Read(pingN);
		//LINFO << "Ping value: " << pingN;

		std::ostringstream ping;
		ping << pingN;

		//LINFO << "Write";
		playerList_list.at(id).text[2] = ping.str();

		//LINFO << "Done";
	}
}

void playerList::coloredUpdated(int id, unsigned int hex)
{
	if(playerList_list.find(id) != playerList_list.end())
	{
		//LINFO << "Player " << id << " new list color: " << hex;
		playerList_list.at(id).hex = colorHelper::argb(hex);
	}
}

void playerList::nameUpdated(int id, const char* name)
{
	if(playerList_list.find(id) != playerList_list.end())
	{
		//LINFO << "Player " << id << " new list color: " << hex;
		playerList_list.at(id).text[1] = std::string(name);
	}
}

/*void playerList::handleKeyPress()
{
	if(keyboard::getKey(VK_MENU) && keyboard::getAlt()) //alt
	{
		if(!isListActive())
		{
			playerList::showList(true);
			//LINFO << "DOWN";
		}
	}
	else if(isListActive())
	{
		playerList::showList(false);
	}
}*/

bool playerListTab = false;
void playerListTabDown(bool alt)
{
	if(alt || playerList::isListActive() || chatWindow::isChatInputActive())
	{
		//LINFO << "TAB FAILED";
		return;
	}

	playerListTab = true;
	playerList::showList(true);
}

void playerListTabUp(bool alt)
{
	playerListTab = false;
	if(playerList::isListActive())
	{
		playerList::showList(false);
	}
}

void playerList::scrollDown()
{
	LINFO << "PlayerList Scroll Up";
	if(!playerList_show || playerListScroll + 20 > (int)playerList_list.size())
	{
		return;
	}
	playerListMutex.lock();

	playerListScroll += 20;
	writePlayerList();

	playerListMutex.unlock();
	LINFO << "All done";
}

void playerList::scrollUp()
{
	LINFO << "PlayerList Scroll down";
	if(!playerList_show || playerListScroll - 20 < 0)
	{
		return;
	}
	playerListMutex.lock();

	playerListScroll -= 20;
	writePlayerList();

	playerListMutex.unlock();
	LINFO << "All done";
}

void playerList::registerKeys(char* svrName)
{
	listServerName = svrName;
	keyboard::registerFunc(VK_TAB, playerListTabDown, false);
	keyboard::registerFunc(VK_TAB, playerListTabUp, true);

	guiManager::guiPanel* chatPanel = new guiManager::guiPanel;
	chatPanel->create = &playerListCreateDevice;
	chatPanel->destroy = &playerListDestroyDevice;
	chatPanel->lost = &playerListLostDevice;
	chatPanel->render = &playerListRender;
	chatPanel->reset = &playerListResetDevice;
	chatPanel->msgUpdate = NULL;

	guiManager::addPanel(guiManager::PLAYER_LIST, chatPanel);
}