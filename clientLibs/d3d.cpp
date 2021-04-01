#include "d3d.h"

d3d::onReset createDevice = NULL;
d3d::onDraw lostDevice = NULL;
d3d::onDraw drawDevice = NULL;
d3d::onReset resetDevice = NULL;

IDirect3DDevice9 *lastDevice;
D3DPRESENT_PARAMETERS* lastParams;

void d3d::registerClient(onReset create, onReset reset, onDraw lost, onDraw draw)
{
	createDevice = create, lostDevice = lost, drawDevice = draw, resetDevice = reset;
	if(createDevice != NULL) createDevice(lastDevice, lastParams);
}

void d3d::callOnLost(IDirect3DDevice9 *m_pDevice)
{
	lastDevice = m_pDevice;
	if(lostDevice != NULL)
	{
		lostDevice(m_pDevice);
	}
}

void d3d::callOnDraw(IDirect3DDevice9 *m_pDevice)
{
	if(drawDevice != NULL)
	{
		drawDevice(m_pDevice);
	}
}
void d3d::callOnReset(IDirect3DDevice9 *m_pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	lastDevice = m_pDevice;
	lastParams = pPresentationParameters;
	if(resetDevice != NULL)
	{
		resetDevice(m_pDevice, pPresentationParameters);
	}
}
void d3d::callOnCreate(IDirect3DDevice9 *m_pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	lastDevice = m_pDevice;
	lastParams = pPresentationParameters;
	if(createDevice != NULL)
	{
		createDevice(m_pDevice, pPresentationParameters);
	}
}

IDirect3DDevice9 *d3d::getDevice()
{
	return lastDevice;
}

D3DPRESENT_PARAMETERS* d3d::getParams()
{
	return lastParams;
}
