#include "chatWindow.h"
#include "guiMouse.h"
#include "guiManager.h"
#include "../GWEN/Include/Gwen/Controls/ImagePanel.h"
#include "easylogging++.h"
#include "../clientLibs/gameWindow.h"

LPDIRECT3DTEXTURE9 imagetex = NULL; //textute our image will be loaded into
LPD3DXSPRITE sprite = NULL; //sprite to display our image
D3DXVECTOR3 imagepos; 

//Gwen::Controls::ImagePanel* cursor = nullptr;

void guiMouseCreateDevice(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters)
{
}

void guiMouseResetDevice(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters)
{
	LINFO << "guiMouseResetDevice";
	if(FAILED(D3DXCreateTextureFromFile(pDevice,"comMP/GUI/cursor2.png", &imagetex)))
	{
		LINFO << "Failed to laod the image";
		imagetex = NULL;
		return;
	}
	if(FAILED(D3DXCreateSprite(pDevice,&sprite)))
	{
		LINFO << "Failed to laod the sprite";
		sprite = NULL;
		return;
	}

    imagepos.x=8.0f; //coord x of our sprite
    imagepos.y=18.0f; //coord y of out sprite
    imagepos.z=0.0f; //coord z of out sprite

	/*cursor = new Gwen::Controls::ImagePanel(guiManager::getGwenCanvas());
	cursor->SetImage("comMP/cursor.png");
	cursor->SetHeight(guiManager::windowY() * 0.019f);
	cursor->SetWidth(guiManager::windowX() * 0.0089);*/
}

void guiMouseLostDevice(IDirect3DDevice9* pDevice, bool clientControlled)
{
	LINFO << "guiMouseLostDevice";
	if(sprite)
	{
		sprite->Release();
		sprite = NULL;
	}
	LINFO << "Sprite done";
	if(imagetex)
	{
		imagetex->Release();
		imagetex = NULL;
	}
	LINFO << "Image done";
}

void guiMouseDestroyDevice(IDirect3DDevice9* pDevice)
{
}

void guiMouseRender(IDirect3DDevice9* pDevice)
{
	if(!imagetex || !sprite)
	{
		LINFO << "Failed to draw ->remove this";
		return;
	}
	POINT p = guiManager::mousePos();

	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	D3DXVECTOR2 spriteCentre=D3DXVECTOR2(24.0f, 36.0f); //half the size

	// Screen position of the sprite
	D3DXVECTOR2 trans=D3DXVECTOR2(p.x, p.y);

	// Rotate based on the time passed
	float rotation = 0.0;

	// Build our matrix to rotate, scale and position our sprite
	D3DXMATRIX mat;

	//D3DXVECTOR2 scaling(0.006f, 0.006f);
	D3DXVECTOR2 scaling(guiManager::windowX() * 0.006f / 48, guiManager::windowY() * 0.011f / 72);

	D3DXMatrixTransformation2D(&mat, NULL, 0.0, &scaling, &spriteCentre, rotation, &trans);

	// Tell the sprite about the matrix
	sprite->SetTransform(&mat);
	sprite->Draw(imagetex, NULL, NULL, &imagepos, 0xFFFFFFFF);
	sprite->End();

	//cursor->SetPos(p.x, p.y);
	/*if(!cursor->HasFocus() && !chatWindow::isChatInputActive())
	{
		cursor->Focus();
	}*/
}

void guiMouseUpdateMsg(MSG msg)
{
	//chatGwenInput->ProcessMessage(msg);
}

bool guiManager::isMouseActive()
{
	return imagetex != NULL;
}

void guiMouse::show(bool toggle)
{
	if(toggle)
	{
		guiManager::guiPanel* chatPanel = new guiManager::guiPanel;
		chatPanel->create = &guiMouseCreateDevice;
		chatPanel->destroy = &guiMouseDestroyDevice;
		chatPanel->lost = &guiMouseLostDevice;
		chatPanel->render = &guiMouseRender;
		chatPanel->reset = &guiMouseResetDevice;
		chatPanel->msgUpdate = &guiMouseUpdateMsg;

		guiManager::addPanel(guiManager::GUI_MOUSE, chatPanel);
	}
	else
	{
		guiManager::deletePanel(guiManager::GUI_MOUSE);
	}
}
