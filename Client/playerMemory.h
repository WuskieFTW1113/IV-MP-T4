#include "simpleMath.h"
#include "PhysicalMemory.h"

#ifndef playerMemory_H
#define playerMemory_H

namespace playerMemory
{
	void getPlayerPosition(int playerAddress, simpleMath::Vector3& vec);
	void setPlayerPosition(int playerAddress, simpleMath::Vector3& vec);

	unsigned int getLocalPlayerIndex();

	class CIVPed : public CIVPhysical // size = F00
	{
	public:
		// 000-210
		// + 0x219 = IsPlayer/PlayerNumber
		PAD(pad0, 0xC);					// 210-21C
		unsigned int m_pPedBase;				// 21C-220
		PAD(pad1, 0x4);					// 220-224
		unsigned int * m_pPedIntelligence;		// 224-228
		PAD(pad2, 0x44);				// 228-26C
		unsigned char m_byteUnknown;				// 26C-26D - Bits 4: in vehicle
		PAD(pad3, 0x8D3);				// 26D-B40
		unsigned int * m_pCurrentVehicle; // B40-B44
		PAD(pad4, 0x3BC);				// B44-F00
		/*PAD(pad0, 0xC);					  // 210-21C
		unsigned int m_pPedBase;				  // 21C-220
		PAD(pad1, 0x4);					  // 220-224
		unsigned int * m_pPedIntelligence; 	  // 224-228
		PAD(pad2, 0x40);				  // 228-268
		unsigned int m_dwUnknown;				  // 268-26C - Bits 4: in vehicle
		PAD(pad3, 0x34);				  // 26C-2A0
		unsigned char weapons[0x110];			  // 2A0-3B0
		unsigned char audio[0x1B0];				  // 3B0-560
		unsigned char speech[0x1A8];				  // 560-708
		PAD(pad4, 0x68);				  // 708-770
		unsigned char audioTracker[0xC];			  // 770-77C
		PAD(pad5, 0x2F4);				  // 77C-A70
		unsigned int m_pMoveBlend;				  // A70-A74
		// 00000AB0 field_AB0       	  Vector4 ?
		// 00000AC0 field_AC0       	  Vector4 ?
		// 00000AD0 field_AD0       	  Vector4 ?
		// 00000AE0 field_AE0       	  Vector4 ?
		// 00000AF0 field_AF0       	  Vector4 ?
		PAD(pad6, 0xAC);				  // A74-B20
		CIVVehicle * m_pCurrentVehicle;	  // B20-B24
		PAD(pad7, 0x7C);				  // B24-BA0
		// 00000B74 field_B74       	  dd ?                    ; pbuffer
		// 00000B78 field_B78       	  dd ?                    ; pbuffer
		unsigned char ikManager[0x170];			  // BA0-D10
		// 00000D60 field_D60       	  Vector4 ?
		// 00000E10 field_E10       	  Vector4 ?
		// 00000E20 field_E20       	  Vector4 ?
		PAD(pad8, 0x13C);				  // D10-E4C*/

		void SetType(unsigned int dwType);
		unsigned int GetType();
		bool IsInVehicle();
		void SetCurrentVehicle(unsigned int pVehicle);
		unsigned int GetCurrentVehicle();
		bool IsDead();
	};
}

#endif