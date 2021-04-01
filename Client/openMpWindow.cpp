#include "openMpWindow.h"
#include "guiManager.h"
#include "easylogging++.h"

#include "Gwen/Input/Windows.h"
#include "Gwen\Controls\WindowControl.h"
#include "Gwen\Controls\Button.h"
#include "Gwen\Controls\Label.h"
#include "Gwen/Controls/ListBox.h"

Gwen::Controls::Button* mpWindowLaunch = nullptr;
Gwen::Controls::Button* mpWindowClose = nullptr;
Gwen::Controls::Label* mpWindowLabel = nullptr;
Gwen::Input::Windows* mpWindowInput = nullptr;
Gwen::Controls::WindowControl* pWindow = nullptr;

void mpWindowCreateDevice(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters){}

void mpWindowDestroyDevice(IDirect3DDevice9* pDevice){}

void RowSelected(Gwen::Controls::Base* pControl)
{
	Gwen::Controls::ListBox* ctrl = ( Gwen::Controls::ListBox* ) pControl;
	LINFO << "Listbox Item Selected: " << ctrl->GetSelectedRow()->GetText(0).c_str();
}
void mpWindowResetDevice(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters)
{
	LINFO << "mpWindowResetDevice";
	pWindow = new Gwen::Controls::WindowControl(guiManager::getGwenCanvas());
	LINFO << "mpWindowResetDevice: gotCanvas";
	pWindow->SetTitle(L"HELLO");
	pWindow->SetSize(guiManager::windowX() * 0.25f, guiManager::windowY() * 0.15f);
	pWindow->SetPos(guiManager::windowX() * 0.25f, guiManager::windowY() * 0.5f);
	pWindow->SetClosable(false);
	pWindow->SetDeleteOnClose(false);

	mpWindowLabel = new Gwen::Controls::Label(pWindow);
	mpWindowLabel->SetText(L" Would would you like to launch IV-MP?");
	//mpWindowLabel->SetPos(0.25f, 0.1f);
	mpWindowLabel->SetSize(pWindow->GetSize().x, pWindow->GetSize().y);
	LINFO << "mpWindowResetDevice: label set";

	mpWindowLaunch = new Gwen::Controls::Button(pWindow);
	mpWindowLaunch->SetText(L"Yes");
	mpWindowLaunch->SetPos(pWindow->GetSize().x * 0.2, pWindow->GetSize().y * 0.5);
	mpWindowLaunch->SetSize(pWindow->GetSize().x * 0.2, pWindow->GetSize().y * 0.15);
	LINFO << "mpWindowResetDevice: launch set";

	mpWindowClose = new Gwen::Controls::Button(pWindow);
	mpWindowClose->SetText(L"No");
	mpWindowClose->SetPos(pWindow->GetSize().x * 0.6, pWindow->GetSize().y * 0.5);
	mpWindowClose->SetSize(pWindow->GetSize().x * 0.2, pWindow->GetSize().y * 0.15);
	LINFO << "mpWindowResetDevice: close set";

}

void mpWindowLostDevice(IDirect3DDevice9* pDevice, bool clientControlled)
{
	if(pWindow == nullptr)
	{
		return;
	}

	LINFO << "mpWindowLostDevice";
	pWindow = nullptr;
}

void crosstwo(LPDIRECT3DDEVICE9 pDevice,int tx,int ty,int tw,int th,D3DCOLOR COLORZ)
{
	D3DRECT RECTONE;
	RECTONE.x1 = tx-tw;
	RECTONE.y1 = ty;
	RECTONE.x2 = tx+tw;
	RECTONE.y2 = ty+1;
	D3DRECT RECTTWO;
	RECTTWO.x1 = tx;
	RECTTWO.y1 = ty-th;
	RECTTWO.x2 = tx+1;
	RECTTWO.y2 = ty+th;
	pDevice->Clear(1,&RECTONE,D3DCLEAR_TARGET,COLORZ,0,0);
	pDevice->Clear(1,&RECTTWO,D3DCLEAR_TARGET,COLORZ,0,0);

}

void mpWindowRender(IDirect3DDevice9* pDevice)
{
	POINT mp = guiManager::mousePos();
	crosstwo(pDevice, mp.x, mp.y, 5, 5, D3DCOLOR_RGBA(255, 0, 0, 255));
}

void mpWindowUpdateMsg(MSG msg)
{
}

void openMpWindow::showWindow(bool show)
{
	if(show)
	{
		guiManager::guiPanel* chatPanel = new guiManager::guiPanel;
		chatPanel->create = &mpWindowCreateDevice;
		chatPanel->destroy = &mpWindowDestroyDevice;
		chatPanel->lost = &mpWindowLostDevice;
		chatPanel->render = &mpWindowRender;
		chatPanel->reset = &mpWindowResetDevice;
		chatPanel->msgUpdate = &mpWindowUpdateMsg;

		guiManager::addPanel(2, chatPanel);
	}
	else
	{
		guiManager::deletePanel(2);
	}
}