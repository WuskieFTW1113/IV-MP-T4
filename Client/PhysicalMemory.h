#include "gameVectors.h"
#include "EntityMemory.h"

#ifndef PhsyicalMemory_H
#define PhsyicalMemory_H

class CIVPhysical : public CIVDynamicEntity
{
public:
	// 000-10C
	PAD(pad0, 0xD8);				 // 10C-1E4
	CIVEntity * m_pLastDamageEntity; // 1E4-1E8
	PAD(pad1, 0x8);					 // 1E8-1F0
	unsigned int m_uHealth;			 // 1F0-1F4
	PAD(pad2, 0x1C);				 // 1F4-210
	// 0000016C field_16C       dd ?                    ; CEntity *
	// 000001BC field_1BC       dd ?                    ; CEntity *
	// 000001E4 field_1E4       dd ?                    ; CEntity * // Last Damage Entity?
	// 000001F8 field_1F8       dd ?                    ; CEntity *

	void SetMoveSpeed(gameVectors::Vector3 * vecMoveSpeed);
	void GetMoveSpeed(gameVectors::Vector3 * vecMoveSpeed);
	void SetTurnSpeed(gameVectors::Vector3 * vecTurnSpeed);
	void GetTurnSpeed(gameVectors::Vector3 * vecTurnSpeed);
	void SetLastDamageEntity(CIVEntity * pLastDamageEntity);
	CIVEntity * GetLastDamageEntity();
	void SetHealth(unsigned int uHealth);
	unsigned int GetHealth();
};

#endif