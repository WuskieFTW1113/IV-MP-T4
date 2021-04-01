#include <Windows.h>
#include "detours.h"
#include "d3dHook.h"
#include "d3dProxy.h"
#include "d3dDeviceProxy.h"
#include "easylogging++.h"

typedef	IDirect3D9 * (WINAPI * Direct3DCreate9_t)(UINT);
Direct3DCreate9_t m_pfnDirect3DCreate9;

IDirect3D9 * WINAPI Direct3DCreate9_Hook(UINT SDKVersion)
{
	IDirect3D9 * pD3D = m_pfnDirect3DCreate9(SDKVersion);
	if(!pD3D)
	{
		LINFO << "D3D detour failed";
		return NULL;
	}
	LINFO << "D3D detour OK";
	return new IDirect3D9Proxy(pD3D);
}

void d3dHook::install()
{
	m_pfnDirect3DCreate9 = (Direct3DCreate9_t)DetourFindFunction("D3D9.DLL", "Direct3DCreate9");
	LINFO << "Direct3DCreate9: " << m_pfnDirect3DCreate9;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	LINFO << "Attach: " << DetourAttach((PVOID*)(&m_pfnDirect3DCreate9), (PVOID)Direct3DCreate9_Hook);
	DetourTransactionCommit();
}

void d3dHook::remove()
{
}