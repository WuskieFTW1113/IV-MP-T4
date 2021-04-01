#include "vehicleMemory.h"
#include "gameMemory.h"
#include "../ArusHook/Types.h"
#include "easylogging++.h"
#include "rotationMatrix.h"
#include "../clientLibs/execution.h"
#include "Offsets.h"

void vehicleMemory::getVehiclePosition(unsigned int vehicleAddress, simpleMath::Vector3& vec)
{
	int buf = *(unsigned int*)(vehicleAddress + 32);
	if(buf == 0)
	{
		return;
	}

	vec.x = *(float*)(buf + 0x30); 
	vec.y = *(float*)(buf + 0x34); 
	vec.z = *(float*)(buf + 0x38);
}

void vehicleMemory::setVehiclePosition(unsigned int vehicleAddress, simpleMath::Vector3& vec)
{
	int buf = *(unsigned int*)(vehicleAddress + 32);
	if(buf == 0)
	{
		return;
	}

	*(float*)(buf + 0x30) = vec.x; 
	*(float*)(buf + 0x34) = vec.y; 
	*(float*)(buf + 0x38) = vec.z;
}

float vehicleMemory::getVehicleRPM(unsigned int vehicleAddress)
{
	//return *(float*)(vehicleAddress + 0x688);
	return *(float*)(vehicleAddress + 3172);
}

void vehicleMemory::setVehicleRPM(unsigned int vehicleAddress, float rpm)
{
	// *(float*)(vehicleAddress + 0x688) = rpm;
	*(float*)(vehicleAddress + 3172) = rpm;
	*(float*)(vehicleAddress + 4756) = rpm;
	*(float*)(vehicleAddress + 4336) = rpm;
}

float vehicleMemory::getVehicleSteer(unsigned int vehicleAddress)
{
	//Range is -0.61 to 0.61 where negative is right and positive is left
	return *(float*)(vehicleAddress + 0x10D8);
}

void vehicleMemory::setVehicleSteer(unsigned int vehicleAddress, float steer)
{
	*(float*)(vehicleAddress + 0x10D8) = steer;
}

float vehicleMemory::getVehicleGasPedal(unsigned int vehicleAddress)
{
	//returns -1 to 1
	return *(float*)(vehicleAddress + 4340);
}

void vehicleMemory::setVehicleGasPedal(unsigned int vehicleAddress, float pedal)
{
	*(float*)(vehicleAddress + 0x10C8) = pedal;
}

float vehicleMemory::getVehicleBreakPedal(unsigned int vehicleAddress)
{
	/*returns 0 or 1, IDK why IVMP says its an INT, unless its game pad related
	Reversing doesnt count as break
	*/
	return *(float*)(vehicleAddress + 0x10CC);
}

void vehicleMemory::setVehicleBreakPedal(unsigned int vehicleAddress, float pressure)
{
	*(float*)(vehicleAddress + 0x10CC) = pressure;
}

void vehicleMemory::getVehicleVelocity(unsigned int vehicleAddress, simpleMath::Vector3* vec)
{
	ptr func = ptr_cast(gameMemory::getModuleHandle() + offsets::VEHGETVELOCITY);
	_asm
	{
		push vec
		mov ecx, vehicleAddress
		call func
	}
}

void vehicleMemory::setVehicleVelocity(unsigned int vehicleAddress, simpleMath::Vector3* vec)
{
	ptr func = ptr_cast(gameMemory::getModuleHandle() + offsets::VEHSETVELOCITY);
	_asm
	{
		push vec
		mov ecx, vehicleAddress
		call func
	}
}

void vehicleMemory::getVehicleTurnVelocity(unsigned int vehicleAddress, simpleMath::Vector3* vec)
{
	ptr func = ptr_cast(gameMemory::getModuleHandle() + offsets::VEHGETTURNVELOCITY);
	_asm
	{
		push vec
		mov ecx, vehicleAddress
		call func
	}
}

void vehicleMemory::setVehicleTurnVelocity(unsigned int vehicleAddress, simpleMath::Vector3* vec)
{
	ptr func = ptr_cast(gameMemory::getModuleHandle() + offsets::VEHSETTURNVELOCITY);
	_asm
	{
		push vec
		mov ecx, vehicleAddress
		call func
	}
}

unsigned char vehicleMemory::getVehicleHandBrake(unsigned int vehicleAddress)
{
	//76 off, 204 on
	return *(unsigned char*)(vehicleAddress + 0x0F64);
}

void vehicleMemory::setVehicleHandBrake(unsigned int vehicleAddress, unsigned char value)
{
	*(unsigned char*)(vehicleAddress + 0x0F64) = value;
}

void vehicleMemory::getVehicleRotation(unsigned int vehicleAddress, simpleMath::Vector3& vec)
{
	int buf = *(unsigned int*)(vehicleAddress + 50);
	if(buf == 0)
	{
		return;
	}
	
	//LINFO << "OK";

	vec.x = *(float*)(buf + 0x30);
	//LINFO << "OK1";
}

void vehicleMemory::setVehicleRotation(unsigned int vehicleAddress, simpleMath::Vector3& vec)
{
	/*int buf = *(unsigned int*)(vehicleAddress + 208);
	if(buf == 0)
	{
		return;
	}

	*(float*)(buf + 0x00) = vec.x;
	*(float*)(buf + 0x04) = vec.y;
	*(float*)(buf + 0x08) = vec.z;*/
}

BYTE vehicleMemory::getVehicleGear(unsigned int vehicleAddress)
{
	return *(BYTE*)(vehicleAddress + 4320);
}

void vehicleMemory::setVehicleGear(unsigned int vehicleAddress, BYTE gear)
{
	*(BYTE*)(vehicleAddress + 4320) = gear;
}

float vehicleMemory::CIVVehicle::GetEngineHealth()
{
	return m_fEngineHealth;
}

void vehicleMemory::getVehicleLocalRotation(vehicleMemory::CIVVehicle* v, simpleMath::Vector3& vec)
{
	gameVectors::Matrix34 matMatrix;
	memcpy(&matMatrix, v->m_pMatrix, sizeof(gameVectors::Matrix34));

	simpleMath::Vector3 vRight(matMatrix.vecRight.x, matMatrix.vecRight.y, matMatrix.vecRight.z);
	simpleMath::Vector3 vFront(matMatrix.vecFront.x, matMatrix.vecFront.y, matMatrix.vecFront.z);
	simpleMath::Vector3 vUp(matMatrix.vecUp.x, matMatrix.vecUp.y, matMatrix.vecUp.z);
	simpleMath::Vector3 vPos(matMatrix.vecPosition.x, matMatrix.vecPosition.y, matMatrix.vecPosition.z);

	Matrix mx(vRight, vFront, vUp, vPos);

	simpleMath::Vector3 vecNewRotation;
	ConvertRotationMatrixToEulerAngles(mx, vecNewRotation);

	flipVectorRotation(vecNewRotation);

	vec = ConvertRadiansToDegrees(vecNewRotation);
}

void vehicleMemory::setVehicleLocalRotation(vehicleMemory::CIVVehicle* v, simpleMath::Vector3& vec)
{
	// Get the vehicle matrix
	gameVectors::Matrix34 matMatrix;
	v->GetMatrix(&matMatrix);

	// Convert the rotation from degrees to radians
	simpleMath::Vector3 vecNewRotation = vec;
	ConvertDegreesToRadians(vecNewRotation);

	// Apply the rotation to the vehicle matrix
	ConvertEulerAnglesToRotationMatrix(vecNewRotation, matMatrix);

	// Set the new vehicle matrix
	v->SetMatrix(&matMatrix);
}

unsigned int vehicleMemory::getIndexFromHandle(unsigned int handle)
{
	unsigned int PoolAddress = *(unsigned int*)gameMemory::getVehiclesPool();
	unsigned int listadr = *(unsigned int*)(PoolAddress);
	unsigned int booladr = *(unsigned int*)(PoolAddress + 4);
	unsigned int maxcount = *(unsigned int*)(PoolAddress + 8);
	unsigned int itemsize = *(unsigned int*)(PoolAddress + 12);

	for(unsigned int i = 0; i < maxcount; ++i)
	{
		unsigned int val = (unsigned int)*(u8*)(booladr + i);
		if((val&0x80) == 0)
		{
			if((listadr + i * itemsize) == handle)
			{
				return (i << 8) | val;
			}
		}
	}
	return 0;
}