#include <d3d9helper.h>
#include "guiManager.h"
#include "easylogging++.h"

#include "../GWEN/Include/Gwen/Gwen.h"
#include "../GWEN/Include/Gwen/Renderers/DirectX9.h"
#include "../GWEN/Include/Gwen\Input\Windows.h"
#include "../clientLibs/d3d.h"
#include "../clientLibs/gameWindow.h"
#include "../ClientManager/ivScripting.h"
#include "keyboard.h"
#include <mutex>
#include "dx11.h"

std::mutex rederMutex;
float guiScreenSize[2];

Gwen::Renderer::DirectX9* guiRenderer = nullptr;
Gwen::Skin::TexturedBase* guiSkin = nullptr;
Gwen::Controls::Canvas* guiCanvas = nullptr;
Gwen::Input::Windows* chatGwenInput = nullptr;

D3DPRESENT_PARAMETERS* currentParams;

std::map<int, guiManager::guiPanel*> panels;

bool gui_ShowMouse = false;

bool isPauseMenu = false;

POINT currentMousePos;
ID3DXSprite *m_pSprite = nullptr;

void guiManager::InstallHook()
{
    LINFO << "guiManager: installing hook";
	d3d::registerClient(OnCreateDevice, OnResetDevice, OnLostDevice, OnRender);

	LINFO << "guiManager: hook is good"; 
	gameWindow::registerWindowMsgs(guiManager::updateManagerMsg);
	//gameWindow::registerWindowMouse(guiManager::updateMouse);
	LINFO << "Window hook is good";
}

void guiManager::RemoveHook()
{
	d3d::registerClient(NULL, NULL, NULL, NULL);
	gameWindow::registerWindowMsgs(NULL);
	//gameWindow::registerWindowMouse(NULL);
	guiManager::OnLostDevice(d3d::getDevice());
}

int t_gwenfont = 12;

int guiManager::getGwenFont(){ return t_gwenfont; }

void guiManager::createGWEN(IDirect3DDevice9 *pDevice)
{
	rederMutex.lock();
	LINFO << "guiManager: GWEN reset";
	guiRenderer = new Gwen::Renderer::DirectX9(pDevice);
	LINFO << "guiManager: device 1";

	int x = windowX();
	t_gwenfont = 12;
	if(x > 3000) t_gwenfont = 28;
	else if(x > 1900) t_gwenfont = 16;
	else if(x > 1400) t_gwenfont = 15;
	else if(x > 600 && x < 900) t_gwenfont = 9;

	guiSkin = new Gwen::Skin::TexturedBase(guiRenderer, t_gwenfont);
	t_gwenfont *= (t_gwenfont != 9 ? 1.2f : 1.44f);
	guiSkin->Init("comMP/GUI/DefaultSkin.png");
	LINFO << "guiManager: device 2";

	guiCanvas = new Gwen::Controls::Canvas(guiSkin);
	guiCanvas->SetSize(guiManager::windowX(), guiManager::windowY());

	chatGwenInput = new Gwen::Input::Windows();
	chatGwenInput->Initialize(guiManager::getGwenCanvas());
	LINFO << "GWEN inited";
	//isDeviceCreated = true;

	rederMutex.unlock();
}

void guiManager::disposeGWEN()
{
	LINFO << "guiManager: disposing";
	if(guiRenderer != nullptr)
	{
		delete guiCanvas;
		delete guiSkin;
		delete guiRenderer;

		guiRenderer = nullptr;
		guiSkin = nullptr;
	}

	LINFO << "guiManager: disposed";
	//isDeviceCreated = false;
}

void resetDevice(IDirect3DDevice9 *pDevice, D3DPRESENT_PARAMETERS *pD3DPRESENT_PARAMETERS, bool created)
{
	guiScreenSize[0] = pD3DPRESENT_PARAMETERS->BackBufferWidth;
	guiScreenSize[1] = pD3DPRESENT_PARAMETERS->BackBufferHeight;

	currentParams = pD3DPRESENT_PARAMETERS;

	LINFO << "guiManager: start gwen";

	guiManager::createGWEN(pDevice);

	if(created) return;
	for(std::map<int, guiManager::guiPanel*>::iterator i = panels.begin(); i != panels.end(); ++i)
	{
		i->second->reset(pDevice, pD3DPRESENT_PARAMETERS);
	}
}

void guiManager::OnCreateDevice(IDirect3DDevice9 *pDevice, D3DPRESENT_PARAMETERS *pD3DPRESENT_PARAMETERS)
{
	LINFO << "guiManager: OnCreateDevice";
	resetDevice(pDevice, pD3DPRESENT_PARAMETERS, true);
	for(std::map<int, guiManager::guiPanel*>::iterator i = panels.begin(); i != panels.end(); ++i)
	{
		i->second->create(pDevice, pD3DPRESENT_PARAMETERS);
	}
}

void guiManager::OnResetDevice(IDirect3DDevice9 *pDevice, D3DPRESENT_PARAMETERS *pD3DPRESENT_PARAMETERS )
{
	LINFO << "guiManager: OnResetDevice";
	resetDevice(pDevice, pD3DPRESENT_PARAMETERS, false);
}

void guiManager::OnDestroyDevice(IDirect3DDevice9 *pDevice)
{
	LINFO << "guiManager: OnDestroyDevice";
	for(std::map<int, guiManager::guiPanel*>::iterator i = panels.begin(); i != panels.end(); ++i)
	{
		i->second->destroy(d3d::getDevice());
	}
}

void guiManager::OnLostDevice(IDirect3DDevice9 *pDevice )
{
	LINFO << "guiManager: OnLostDevice";
	disposeGWEN();

	for(std::map<int, guiManager::guiPanel*>::iterator i = panels.begin(); i != panels.end(); ++i)
	{
		i->second->lost(pDevice, false);
	}
}

long lMouse[2] = {0};
bool wasMouseDown = false;

void guiManager::OnRender(IDirect3DDevice9 *pDevice )
{
	//LINFO << "Rnder called";
	if(guiRenderer == nullptr || Scripting::IsPauseMenuActive())
	{
		return;
	}
	rederMutex.lock();
	if(isMouseActive())
	{
		POINT pos;
		UINT msg;
		bool mouse = false;

		GetCursorPos(&pos);
		if((GetKeyState(VK_LBUTTON) & 0x80) != 0)
		{
			msg = WM_LBUTTONDOWN;
			mouse = true;
		}
		else if(wasMouseDown)
		{
			mouse = false;
			msg = WM_LBUTTONUP;
		}
		else
		{
			msg = WM_MOUSEMOVE;
			//m.lParam = MAKELONG(pos.x, pos.y);
		}

		if(wasMouseDown != mouse || currentMousePos.x != pos.x || currentMousePos.y != pos.y)
		{
			guiManager::updateManagerMsg(GetActiveWindow(), msg, NULL, msg == WM_MOUSEMOVE ? MAKELONG(pos.x, pos.y) : NULL);
			currentMousePos = pos;
			wasMouseDown = mouse;
		}

		guiManager::setMousePos(pos.x, pos.y);
	}

	guiCanvas->RenderCanvas();
	std::map<int, guiManager::guiPanel*>::iterator i = panels.begin();
	while(i != panels.end())
	{
		if(i->second->toBeDeleted)
		{
			i->second->lost(pDevice, true);
			panels.erase(i++);
			continue;
		}
		else if(i->second->firstInit)
		{
			i->second->firstInit = false;
			i->second->create(pDevice, currentParams);
			i->second->reset(pDevice, currentParams);
			continue;
		}
		i->second->render(pDevice);
		++i;
	}
	rederMutex.unlock();
}

Gwen::Controls::Canvas* guiManager::getGwenCanvas()
{
	return guiCanvas;
}

void updateGWENInputs(MSG& m)
{
	for(std::map<int, guiManager::guiPanel*>::iterator i = panels.begin(); i != panels.end(); ++i)
	{
		if(i->second->msgUpdate != NULL)
		{
			i->second->msgUpdate(m);
		}
	}
	chatGwenInput->ProcessMessage(m);
}

void guiManager::updateManagerMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MSG msg;
	msg.hwnd = hWnd;
	msg.lParam = lParam;
	msg.wParam = wParam;
	msg.message = uMsg;

	updateGWENInputs(msg);

	if(uMsg == WM_KEYDOWN || uMsg == WM_KEYUP)
	{
		keyboard::receiveWindowMsg(msg.message, msg.wParam);
	}
}

bool guiManager::addPanel(int id, guiManager::guiPanel* panel)
{
	//clientGui->RemoveHook();
	panels.insert(std::make_pair(id, panel));
	panels.at(id)->toBeDeleted = false;
	panels.at(id)->firstInit = true;
	//clientGui->InstallHook();
	return true;
}

bool guiManager::isPanelActive(int id)
{
	return panels.find(id) != panels.end();
}

bool guiManager::deletePanel(int id)
{
	if(isPanelActive(id))
	{
		panels.at(id)->toBeDeleted = true;
		//panels.erase(id);
		return true;
	}
	return false;
}

float guiManager::windowX()
{
	return guiScreenSize[0];
}

float guiManager::windowY()
{
	return guiScreenSize[1];
}

void guiManager::setMousePos(float x, float y)
{
	currentMousePos.x = x;
	currentMousePos.y = y;
}

POINT guiManager::mousePos()
{
	return currentMousePos;
}

void guiManager::pauseMenu(bool b)
{
	isPauseMenu = b;
}
