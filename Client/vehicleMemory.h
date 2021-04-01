#include "simpleMath.h"
//#include "gameVectors.h"
#include "PhysicalMemory.h"
#include "playerMemory.h"

#ifndef vehicleMemory_H
#define vehicleMemory_H

namespace vehicleMemory
{
	void getVehiclePosition(unsigned int vehicleAddress, simpleMath::Vector3& vec);
	void setVehiclePosition(unsigned int vehicleAddress, simpleMath::Vector3& vec);

	float getVehicleRPM(unsigned int vehicleAddress);
	void setVehicleRPM(unsigned int vehicleAddress, float rpm);

	float getVehicleSteer(unsigned int vehicleAddress);
	void setVehicleSteer(unsigned int vehicleAddress, float steer);

	float getVehicleGasPedal(unsigned int vehicleAddress);
	void setVehicleGasPedal(unsigned int vehicleAddress, float pedal);

	float getVehicleBreakPedal(unsigned int vehicleAddress);
	void setVehicleBreakPedal(unsigned int vehicleAddress, float pressure);

	void getVehicleVelocity(unsigned int vehicleAddress, simpleMath::Vector3* vec);
	void setVehicleVelocity(unsigned int vehicleAddress, simpleMath::Vector3* vec);

	void getVehicleTurnVelocity(unsigned int vehicleAddress, simpleMath::Vector3* vec);
	void setVehicleTurnVelocity(unsigned int vehicleAddress, simpleMath::Vector3* vec);

	unsigned char getVehicleHandBrake(unsigned int vehicleAddress);
	void setVehicleHandBrake(unsigned int vehicleAddress, unsigned char value);

	void getVehicleRotation(unsigned int vehicleAddress, simpleMath::Vector3& vec);
	void setVehicleRotation(unsigned int vehicleAddress, simpleMath::Vector3& vec);

	unsigned char getVehicleGear(unsigned int vehicleAddress);
	void setVehicleGear(unsigned int vehicleAddress, unsigned char gear);

	class CIVVehicle : public CIVPhysical // Size = 20D0
	{
	private:
		// 0000-0210
		PAD(pad0, 0xD88);		   // 0210-0F98
		unsigned short m_wAlarm;			   // 0F98-0F9A
		PAD(pad1, 0x6);			   // 0F9A-0FA0
		playerMemory::CIVPed * m_pDriver;		   // 0FA0-0FA4
		playerMemory::CIVPed * m_pPassengers[8]; // 0FA4-0FC4
		PAD(pad2, 0x20);		   // 0FC4-0FE4
		unsigned char m_byteColours[4];	   // 0FE4-0FE8
		PAD(pad3, 0xD8);		   // 0FA4-10C0
		unsigned char m_byteMaxPassengers;  // 10C0-10C1
		PAD(pad4, 0x3B);		   // 10C1-10FC
		float m_fEngineHealth;	   // 10FC-1100
		PAD(pad5, 0x18);		   // 1100-1118
		float m_fDirtLevel;		   // 1118-111C
		PAD(pad6, 0xC);			   // 111C-1128
		float m_fPetrolTankHealth; // 1128-112C
		PAD(pad7, 0x1F4);		   // 112C-1320
		unsigned int m_dwDoorLockState;   // 1320-1324
		PAD(pad8, 0xDAC);		   // 1324-20D0
		// F66 Needs To Be Hotwired (byte)
		// F67 Can Burst Tyres (byte)
		// F6C Hazard Lights/Indicator Lights/Interior Light (byte)
		// F6E Police Focus Will Track Car (byte)
		// F6F Blip Throttle Randomly (byte)
		// F70 Has Strong Axles/Can Be Visually Damaged (Flags?)
		// 10D4 Steer Bias (float)
		// 1108 Is Mission Car (bool)
		// 1120-12DC Damage
		// 1112 Lights On (word i *think*)
		// 133C Horn

	public:
		void SetAlarmDuration(unsigned short wDuration);
		unsigned short GetAlarmDuration();
		void SetDriver(playerMemory::CIVPed * pPed);
		playerMemory::CIVPed * GetDriver();
		void SetPassenger(unsigned char bytePassengerSeatId, playerMemory::CIVPed * pPassenger);
		playerMemory::CIVPed * GetPassenger(unsigned char bytePassengerSeatId);
		//void SetColours(VehicleColors colors);
		//void GetColours(VehicleColors * colors);
		void SetMaxPassengers(unsigned char byteMaxPassengers);
		unsigned char GetMaxPasssengers();
		void SetSirenState(bool bStatus);
		bool GetSirenState();
		void SetEngineHealth(float fEngineHealth);
		float GetEngineHealth();
		void SetDirtLevel(float fDirtLevel);
		float GetDirtLevel();
		void SetPetrolTankHealth(float fPetrolTankHealth);
		float GetPetrolTankHealth();
		void SetDoorLockState(unsigned int dwDoorLockState);
		unsigned int GetDoorLockState();
		void SetEngineStatus(bool bStatus, bool bUnknown);
		bool GetEngineStatus();
	};

	void getVehicleLocalRotation(vehicleMemory::CIVVehicle* v, simpleMath::Vector3& vec);
	void setVehicleLocalRotation(vehicleMemory::CIVVehicle* v, simpleMath::Vector3& vec);

	unsigned int getIndexFromHandle(unsigned int handle);
}

#endif