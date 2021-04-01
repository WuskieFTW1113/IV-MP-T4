#include "EntityMemory.h"
#include "gameMemory.h"
#include <Windows.h>
#include "../ArusHook/ScriptingTypes.h"
#include "Offsets.h"

void CIVEntity::AddToWorld()
{
	ptr fn = ptr_cast(gameMemory::getModuleHandle() + offsets::ADDTOWORLD);//0x817350);
	_asm
	{
		push 0
		push this
		call fn
		add esp, 8;
	}
}

void CIVEntity::RemoveFromWorld()
{
	ptr fn = ptr_cast(gameMemory::getModuleHandle() + offsets::REMOFROMWORLD); //0x8173C0);
	_asm
	{
		push 0
		push this
		call fn
		add esp, 8;
	}
}

void CIVEntity::SetMatrix(gameVectors::Matrix34 * matMatrix)
{
	memcpy(m_pMatrix, matMatrix, sizeof(gameVectors::Matrix34));
}

void CIVEntity::GetMatrix(gameVectors::Matrix34 * matMatrix)
{
	memcpy(matMatrix, m_pMatrix, sizeof(gameVectors::Matrix34));
}

void CIVEntity::SetPosition(gameVectors::Vector3 * vecPosition)
{
	if(m_pMatrix)
		memcpy(&m_pMatrix->vecPosition, vecPosition, sizeof(gameVectors::Vector3));
	else
		memcpy(&m_vecPosition, vecPosition, sizeof(gameVectors::Vector3));
}

void CIVEntity::GetPosition(gameVectors::Vector3 * vecPosition)
{
	if(m_pMatrix)
		memcpy(vecPosition, &m_pMatrix->vecPosition, sizeof(gameVectors::Vector3));
	else
		memcpy(vecPosition, &m_vecPosition, sizeof(gameVectors::Vector3));
}

void CIVEntity::SetRoll(gameVectors::Vector3 * vecRoll)
{
	if(m_pMatrix)
		memcpy(&m_pMatrix->vecRight, vecRoll, sizeof(gameVectors::Vector3));
}

void CIVEntity::GetRoll(gameVectors::Vector3 * vecRoll)
{
	if(m_pMatrix)
		memcpy(vecRoll, &m_pMatrix->vecRight, sizeof(gameVectors::Vector3));
}

void CIVEntity::SetDirection(gameVectors::Vector3 * vecDirection)
{
	//if(m_pMatrix)
		//memcpy(&m_pMatrix->vecForward, vecDirection, sizeof(gameVectors::Vector3));
}

void CIVEntity::GetDirection(gameVectors::Vector3 * vecDirection)
{
	//if(m_pMatrix)
		//memcpy(vecDirection, &m_pMatrix->vecForward, sizeof(gameVectors::Vector3));
}
