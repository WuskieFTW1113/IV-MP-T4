#include "cameraMemory.h"
#include "gameMemory.h"
#include "easylogging++.h"

cameraMemory::CCam* cameraMemory::getGameCam()
{
	void** unkn = (void**)(gameMemory::getModuleHandle() + 0xF21A6C);
	DWORD dwFunc = (gameMemory::getModuleHandle() + 0xC4E4C0);
	cameraMemory::CCam * pCam = NULL;
	_asm
	{
		mov ecx, unkn
		call dwFunc
		mov pCam, eax
	}
	return pCam;
}

void cameraMemory::setGameCamMatrix(gameVectors::Matrix34* matrix)
{
	CCam * pCam = getGameCam();

	if(pCam)
	{
		memcpy(&pCam->m_matMatrix, matrix, sizeof(gameVectors::Matrix34));
	}
}

void cameraMemory::getGameCameraMatrix(gameVectors::Matrix34* matrix)
{
	CCam * pCam = getGameCam();

	if(pCam)
	{
		memcpy(matrix, &pCam->m_matMatrix, sizeof(gameVectors::Matrix34));
	}
}

void cameraMemory::GetScreenPositionFromWorldPosition(gameVectors::Vector3& v3, gameVectors::Vector2& v2)
{
	gameVectors::Vector3 * pWorldPosition = &v3;
	gameVectors::Vector2 * pScreenPosition = &v2;
	DWORD func = gameMemory::getModuleHandle() + 0xAF94C0;
	LINFO << "All set";

	_asm push pScreenPosition;
	_asm push 2;
	_asm push pWorldPosition;
	_asm call func;
}