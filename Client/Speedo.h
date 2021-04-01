#ifndef Speedo_H
#define Speedo_H

#include "../ArusHook/Services/D3DHook.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "NetworkManager.h"

namespace speedoTest
{
	class SpeedoRender :
		public ID3DDeviceHook
	{
	private:
		HMODULE m_hModule;
		ID3DXSprite *m_pSprite;

	public:
		SpeedoRender();

		inline void SetResourceModule( HMODULE hModule ) { m_hModule = hModule; }

		void InstallHook();
		void RemoveHook();

		void createGWEN(ID3DDevice *pDevice);
		void disposeGWEN();

		void OnCreateDevice(ID3DDevice *pDevice, PresentParameters *pPresentParameters);
		void OnResetDevice(ID3DDevice *pDevice, PresentParameters *pPresentParameters);
		void OnLostDevice(ID3DDevice *pDevice);
		void OnDestroyDevice(ID3DDevice *pDevice);
		void OnRender(ID3DDevice *pDevice);
	};

	void initStuff();
	void unloadHooks();
	void updateMsg(MSG msg);
	void receivedChat(RakNet::BitStream& bsIn);
	void toggleChatInput();

	bool canUpdateNameTags();
	void updateNameTag(int playerid, const char* text, float x, float y);
	void removeNameTag(int playerid);
}

#endif