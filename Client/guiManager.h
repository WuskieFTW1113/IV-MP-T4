#include "../DirectX/Include/d3d9.h"
#include "../DirectX/Include/d3dx9.h"
#include "../GWEN/Include/Gwen/Skins/TexturedBase.h"

namespace guiManager
{
	void InstallHook();
	void RemoveHook();

	void createGWEN(IDirect3DDevice9 *pDevice);
	void disposeGWEN();

	int getGwenFont();

	void OnCreateDevice(IDirect3DDevice9 *pDevice, D3DPRESENT_PARAMETERS*pPresentParameters);
	void OnResetDevice(IDirect3DDevice9 *pDevice, D3DPRESENT_PARAMETERS *pPresentParameters);
	void OnLostDevice(IDirect3DDevice9 *pDevice);
	void OnDestroyDevice(IDirect3DDevice9 *pDevice);
	void OnRender(IDirect3DDevice9 *pDevice);

	Gwen::Controls::Canvas* getGwenCanvas();

	typedef void(*createDeviceCb)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters);
	typedef void(*resetDeviceCb)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters);
	typedef void(*lostDeviceCb)(IDirect3DDevice9* pDevice, bool clientControlled);
	typedef void(*destroyDeviceCb)(IDirect3DDevice9* pDevice);
	typedef void(*renderDeviceCb)(IDirect3DDevice9* pDevice);
	typedef void(*updateDeviceMsgCb)(MSG msg);

	void updateManagerMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	struct guiPanel
	{
		createDeviceCb create;
		resetDeviceCb reset;
		lostDeviceCb lost;
		destroyDeviceCb destroy;
		renderDeviceCb render;
		updateDeviceMsgCb msgUpdate;
		bool toBeDeleted;
		bool firstInit;
	};

	bool addPanel(int id, guiPanel* panel);
	bool isPanelActive(int id);
	bool deletePanel(int id);

	float windowX();
	float windowY();

	void setMousePos(float x, float y);
	bool isMouseActive();
	POINT mousePos();

	enum panelIds
	{
		CHAT_WINDOW = 99,
		GUI_MOUSE = 1,
		DIALOG_LIST = 2,
		PLAYER_LIST = 3,
	};

	void pauseMenu(bool b);
}
