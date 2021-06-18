#include "../DirectX/Include/d3d9.h"

namespace d3d
{
	__declspec(dllexport) typedef void (*onDraw)(IDirect3DDevice9 *m_pDevice);
	__declspec(dllexport) typedef void (*onReset)(IDirect3DDevice9 *m_pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);

	__declspec(dllexport) void registerClient(onReset create, onReset reset, onDraw lost, onDraw draw);

	__declspec(dllexport) void callOnLost(IDirect3DDevice9 *m_pDevice);
	__declspec(dllexport) void callOnDraw(IDirect3DDevice9 *m_pDevice);
	__declspec(dllexport) void callOnReset(IDirect3DDevice9 *m_pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	__declspec(dllexport) void callOnCreate(IDirect3DDevice9 *m_pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);

	__declspec(dllexport) IDirect3DDevice9 *getDevice();
	__declspec(dllexport) D3DPRESENT_PARAMETERS* getParams();
}
