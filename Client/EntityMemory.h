#include "gameVectors.h"

#ifndef EntityMemory_H
#define EntityMemory_H

class CIVEntity
{
public:
	unsigned int m_dwVFTable;			 // 00-04
	PAD(pad0, 0xC);				 // 04-10
	gameVectors::Vector3 m_vecPosition;	 // 10-1C
	float m_fHeading;			 // 1C-20
	gameVectors::Matrix34 * m_pMatrix;	 // 20-24
	// if(... && *(_unsigned int *)(v4 + 0x24) & 0x8000000)
	unsigned int m_dwFlags1;			 // 24-28 - Bits 0: has col model, 3: don't load cols, 5: visible, 12: draw last
	unsigned int m_dwFlags2;			 // 28-2C - Bits 2: lights, 21: onFire
	PAD(pad1, 0x2);				 // 2C-2E
	unsigned short m_wModelIndex;			 // 2E-30
	CIVEntity * m_pReferences;	 // 30-34
	unsigned int m_pLivery;			 // 34-38 // Not sure about this, all i know is ((ent + 0x34) + 0xD8) = vehicle livery
	unsigned int m_pColModel;			 // 38-3C
	PAD(pad3, 0xC);				 // 3C-48
	unsigned int m_hInterior;			 // 48-4C
	PAD(pad4, 0x17);			 // 4C-63
	unsigned char m_byteAlpha;			 // 63-64
	PAD(pad5, 0x10);			 // 64-74

	void SetMatrix(gameVectors::Matrix34 * matMatrix);
	void GetMatrix(gameVectors::Matrix34 * matMatrix);
	void SetPosition(gameVectors::Vector3 * vecPosition);
	void GetPosition(gameVectors::Vector3 * vecPosition);
	void SetRoll(gameVectors::Vector3 * vecRoll);
	void GetRoll(gameVectors::Vector3 * vecRoll);
	void SetDirection(gameVectors::Vector3 * vecDirection);
	void GetDirection(gameVectors::Vector3 * vecDirection);
	void SetModelIndex(unsigned short wModelIndex);
	unsigned short GetModelIndex();
	void SetAlpha(unsigned char byteAlpha);
	unsigned char GetAlpha();
	bool IsTouchingEntity(CIVEntity * pEntity);
	void AddToWorld();
	void RemoveFromWorld();
	gameVectors::Matrix34* UpdatePhysicsMatrix(bool);
};

class CIVDynamicEntity : public CIVEntity
{
public:
	// 000-074
	PAD(pad0, 0x4);			  // 074-078
	unsigned int m_pAnim;			  // 078-07C
	// 00000080 m_portalTracker CPortalTracker ?
	PAD(pad1, 0x80);		  // 07C-0FC
	float m_fHealth;		  // 0FC-100 // Might be wrong
	PAD(pad3, 0xC);			  // 100-10C
};

#endif