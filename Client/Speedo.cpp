#include "Speedo.h"
#include <Windows.h>
#include "../ArusHook/ScriptHookManager.h"
#include "../SharedDefines/easylogging++.h"

#include "chatCallbacks.h"

#include <d3dx9math.h>
#include <DxErr.h>

// C RunTime Header Files
#include <stdlib.h>
#include <memory.h>
#include <tchar.h>

#include "Gwen/Gwen.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/UnitTest/UnitTest.h"
#include "Gwen/Input/Windows.h"
#include "Gwen/Renderers/DirectX9.h"
#include "Gwen/Controls/TextBox.h"
#include "Gwen\Controls\CheckBox.h"

#include "../ArusHook/Scripting.h"
#include "players.h"

speedoTest::SpeedoRender* sp = nullptr;

Gwen::Renderer::DirectX9* pRenderer = nullptr;
Gwen::Skin::TexturedBase* pSkin = nullptr;
Gwen::Controls::Canvas* pCanvas = nullptr;
Gwen::Input::Windows* GwenInput = nullptr;
Gwen::Controls::CheckBox* checkBox = nullptr;

Gwen::Controls::TextBox* label;

Gwen::Font* chatFont;
volatile bool isDeviceCreated = false;

std::string chatMsgs[8];
Gwen::Controls::Label* chat[8];

UINT lastKey = 0;

ID3DXFont* dxFont = NULL;

std::vector<MSG> msgs;

float screenSize[2];
volatile bool updatingNameTags = false;
struct nameTagInfo
{
	float x;
	float y;
	const char* text;
};
std::map<int, nameTagInfo> nameTags;

speedoTest::SpeedoRender::SpeedoRender()
{
	m_pSprite = nullptr;
}

void speedoTest::SpeedoRender::InstallHook()
{
    LINFO << "Speedo: installing hook";
	ID3DHookService *pService = ScriptHookManager::RequestService<ID3DHookService>( "D3DHook" );
	LINFO << "Speedo: 2";
    pService->AddDeviceHook( this );
	LINFO << "Speedo: hook is good";
}

void speedoTest::SpeedoRender::RemoveHook()
{
	ID3DHookService *pService = ScriptHookManager::RequestService<ID3DHookService>( "D3DHook" );
    pService->RemoveDeviceHook( this );
}

void speedoTest::SpeedoRender::createGWEN(ID3DDevice *pDevice)
{
	LINFO << "Speedo: device init";
	pRenderer = new Gwen::Renderer::DirectX9(pDevice);
	LINFO << "Speedo: device 1";

	pSkin = new Gwen::Skin::TexturedBase(pRenderer);
	pSkin->Init("comMP/DefaultSkin.png");
	LINFO << "Speedo: device 2";

	pCanvas = new Gwen::Controls::Canvas( pSkin );
	pCanvas->SetSize(screenSize[0], screenSize[1]);
	//pCanvas->SetDrawBackground(true);
	//pCanvas->SetBackgroundColor(Gwen::Color( 150, 170, 170, 100));
	LINFO << "Speedo: device 3";

	checkBox = new Gwen::Controls::CheckBox(pCanvas);
	checkBox->SetPos(screenSize[0] * 0.006f, screenSize[1] * 0.15f);
	checkBox->SetSize(20, 20);
	checkBox->Show();

	label = new Gwen::Controls::TextBox(pCanvas);
	//label->SetText("Alo caralho");
	label->SetPos(screenSize[0] * 0.006f, screenSize[1] * 0.25f);
	//label->SetSelectAllOnFocus( true );
	label->SetEditable(true);
	label->Hide();
	LINFO << "Speedo: device 4";

	for(size_t i = 0; i < 8; i++)
	{
		chat[i] = new Gwen::Controls::Label(pCanvas);
		chat[i]->SetPos(screenSize[0] * 0.006f, (screenSize[1] * 0.028f) + (i * screenSize[1] * 0.023f));
		chat[i]->SetSize(screenSize[0], 20);
		chat[i]->SetText(chatMsgs[i].c_str());
		chat[i]->GetFont()->size = 16;
		chat[i]->GetFont()->bold = true;
		chat[i]->SetTextColor(Gwen::Color(255, 255, 255, 255));
	}

	GwenInput = new Gwen::Input::Windows();
	GwenInput->Initialize( pCanvas );
	LINFO << "Speedo: device 6";

	D3DXCreateFont(pDevice, 20, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma Bold", &dxFont);

	isDeviceCreated = true;
}

void speedoTest::SpeedoRender::disposeGWEN()
{
	LINFO << "Speedo: disposing";
	if(pRenderer != nullptr)
	{
		label->SetKeyboardInputEnabled(false);

		LINFO << "Deleting chats";
		/*for(size_t i = 0; i < 8; i++)
		{
			chat[i]->SetText("");
		}*/
		LINFO << "Chats deleted";

		pCanvas->Hide();

		//delete label;

		delete pCanvas;
		delete pSkin;
		delete pRenderer;
		//delete GwenInput;

		dxFont->Release();
		dxFont = NULL;

		pRenderer = nullptr;
		label = nullptr;
	}
	LINFO << "Speedo: disposed";
	isDeviceCreated = false;
}

void speedoTest::SpeedoRender::OnCreateDevice(ID3DDevice *pDevice, PresentParameters *pPresentParameters)
{
	LINFO << "Speedo: OnCreateDevice";
	//createGWEN(pDevice);
}

void speedoTest::SpeedoRender::OnResetDevice( ID3DDevice *pDevice, PresentParameters *pPresentParameters )
{
	screenSize[0] = pPresentParameters->BackBufferWidth;
	screenSize[1] = pPresentParameters->BackBufferHeight;

	LINFO << "Speedo: OnResetDevice";
	//disposeGWEN();
	if(isDeviceCreated)
	{
		LINFO << "Speedo: Device is already running";
		return;
	}

	createGWEN(pDevice);
}

void speedoTest::SpeedoRender::OnDestroyDevice(ID3DDevice *pDevice)
{
	LINFO << "Speedo: OnDestroyDevice";
}

void speedoTest::SpeedoRender::OnLostDevice( ID3DDevice *pDevice )
{
	LINFO << "Speedo: OnLostDevice";
	if(!isDeviceCreated)
	{
		LINFO << "Speedo: device wasnt created, releasing hooks";
		return;
	}
	disposeGWEN();
}


void speedoTest::SpeedoRender::OnRender( ID3DDevice *pDevice )
{
	if(label == nullptr)
	{
		return;
	}

	pCanvas->RenderCanvas();

	POINT p;
	if(GetCursorPos(&p))
	{
		//crosstwo(pDevice, p.x, p.y, 5, 5, D3DCOLOR_RGBA(255, 0, 0, 255));
	}

	if(dxFont && !updatingNameTags)
	{
		RECT r;
		updatingNameTags = true;
		for(std::map<int, nameTagInfo>::iterator i = nameTags.begin(); i != nameTags.end(); ++i)
		{
			r.bottom = i->second.y;
			r.top = i->second.y + 50;
			r.left = i->second.x - 20;
			r.right = i->second.x + 200;

			dxFont->DrawText(NULL, i->second.text, -1, &r, DT_LEFT|DT_NOCLIP, 0xFFFFFFFF);
		}
		//nameTags.clear();
		updatingNameTags = false;
	}
}

void speedoTest::initStuff()
{
	if(sp != nullptr)
	{
		return;
	}

	sp = new SpeedoRender();
	sp->InstallHook();
}

void speedoTest::unloadHooks()
{
	if(sp != nullptr)
	{
		LINFO << "Speedo: Unloading hooks";
		//sp->disposeGWEN();
		sp->RemoveHook();
		delete sp;
		sp = nullptr;
	}
}

void speedoTest::receivedChat(RakNet::BitStream& bsIn)
{
	RakNet::RakString chatMsg;
	bsIn.Read(chatMsg);

	for(size_t i = 0; i < 7; i++)
	{
		chatMsgs[i] = chatMsgs[i + 1];
		chat[i]->SetText(chatMsgs[i].c_str());
	}

	chatMsgs[7] = chatMsg;
	chat[7]->SetText(chatMsgs[7].c_str());
	LINFO << chatMsg;
}

void speedoTest::updateMsg(MSG msg)
{
	//msgs.push_back(msg);
	if(label != nullptr)
	{
		if(msg.wParam == VK_BACK)
		{
			std::string newString = std::string(label->GetText().c_str());
			if(newString.size() != 0)
			{
				newString.erase(newString.end() - 1, newString.end());
				label->SetText(newString.c_str());
			}
		}
		else if(msg.wParam == VK_RETURN)
		{
			if(label->GetText().length() != 0)
			{
				//receivedChat(label->GetText().c_str());
				if(networkManager::isNetworkActive())
				{
					RakNet::BitStream bsOut;
					RakNet::RakString text;
					text = label->GetText().c_str();
					bsOut.Write((MessageID)BILATERAL_MESSAGE);
					bsOut.Write(text);

					networkManager::getConnection()->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, 
						networkManager::getConnection()->serverIp, false);
				}
			}
			label->SetText("");
			label->Hide();

			Scripting::SetPlayerControl(players::getLocalPlayer().playerIndex, true);
		}
		else
		{
			GwenInput->ProcessMessage(msg);
		}
		//LINFO << "Processed: " << label->GetText().c_str();
	}
}

void speedoTest::toggleChatInput()
{
	if(label == nullptr || label->Visible())
	{
		return;
	}

	Scripting::SetPlayerControlAdvanced(players::getLocalPlayer().playerIndex, false, false, false);

	label->Show();
	label->Focus();
}

bool speedoTest::canUpdateNameTags()
{
	return updatingNameTags;
}

void speedoTest::updateNameTag(int playerid, const char* text, float x, float y)
{
	if(updatingNameTags)
	{
		return;
	}

	updatingNameTags = true;
	if(nameTags.find(playerid) == nameTags.end())
	{
		nameTagInfo i;
		nameTags.insert(std::make_pair(playerid, i));
	}
	nameTags.at(playerid).text = text;
	nameTags.at(playerid).x = x;
	nameTags.at(playerid).y = y;
	updatingNameTags = false;
}

void speedoTest::removeNameTag(int playerid)
{
	if(updatingNameTags)
	{
		return;
	}
	updatingNameTags = true;
	if(playerid == -1)
	{
		nameTags.clear();
	}
	else if(nameTags.find(playerid) != nameTags.end())
	{
		nameTags.erase(playerid);
	}
	updatingNameTags = false;
}