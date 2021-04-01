#include "vehicleSync.h"
#include "vehicleTasks.h"
#include <sstream>
#include <stdexcept>
#include <time.h>
#include "easylogging++.h"
#include "gameMemory.h"
#include "vehiclesList.h"
#include "CustomFiberThread.h"

simpleMath::Vector3 myLastPos;
gameVectors::Vector3 testPos;
simpleMath::Quaternion rotBuf;

void syncVehicle(int vehicleId)
{
	try
	{
		subTask = 401;
		vehicles::netVehicle& v = vehicles::getVehicle(vehicleId);
		if(!v.hasPol || !Scripting::DoesVehicleExist(v.vehicle))
		{
			return;
		}

		v.threadLock = true;

		long elap = clock();

		float progress = (float)(elap - v.lastPol) / v.polTime;
		progress = simpleMath::ClampF(0.0f, progress, 1.5f);
		//LINFO << "Progress: " << progress;

		if(progress > 1.49f)
		{
			v.hasPol = false;
		}

		float lastProgress = v.lastPolProgress;
		float fCurrentAlpha = (progress - lastProgress);
		v.lastPolProgress = progress;

		simpleMath::Vector3 posCompensation = simpleMath::vecLerp(simpleMath::Vector3(), v.posError, fCurrentAlpha);
		if(/*!Scripting::IsThisModelABike(v.model)*/true)
		{
			v.vPointer->GetPosition(&testPos);
			v.position.x = testPos.x, v.position.y = testPos.y, v.position.z = testPos.z;
		}
		v.position.add(posCompensation);

		//v.position = simpleMath::vecLerp(v.startPos, v.goPos, progress);

		//Position stuff done, now process the rest
		progress = simpleMath::ClampF(0.0f, progress, 1.0f);

		v.steer = simpleMath::lerp(v.startSteer, v.endSteer, progress);
		v.rpm = simpleMath::lerp(v.startRpm, v.endRpm, progress);

		subTask = 402;
		if(v.driver != 0)
		{
			bool isOn = players::isPlayer(v.driver);
			if(!isOn || players::getPlayer(v.driver).inVehicle != vehicleId || 
				!Scripting::DoesCharExist(players::getPlayer(v.driver).ped)) {
			
				v.driver = 0;
				v.fullySpawnedWithDriver = false;
				v.threadLock = false;
			}
			else if(isOn)
			{
				subTask = 403;
				players::getPlayer(v.driver).pos = v.goPos;
			}
		}
		subTask = 404;

		//v.vPointer->RemoveFromWorld();
		if(simpleMath::vecDistance(v.position, v.goPos) > 20.0) //20.0
		{
			v.position = v.goPos;
			v.rotation = v.eRotation;
			//Scripting::SetCarCoordinates(v.vehicle, v.position.x, v.position.y, v.position.z);
		}

		if(v.lastRotationAlpha < 1.0f)
		{
			float rotationAlpha = progress - v.lastRotationAlpha;
			v.lastRotationAlpha = progress;
			vehicleMemory::getVehicleLocalRotation(v.vPointer, v.rotation);
			simpleMath::Vector3 r2 = simpleMath::vecLerp(simpleMath::Vector3(), v.rotationError, rotationAlpha);
			r2.add(v.rotation);
			v.rotation = r2;

			rotBuf = simpleMath::toQuaternion(v.rotation.y, v.rotation.x, v.rotation.z);
		}
		else Scripting::GetVehicleQuaternion(v.vehicle, &rotBuf.X, &rotBuf.Y, &rotBuf.Z, &rotBuf.W);

		//v.speed = simpleMath::vecLerp(v.startSpeed, v.endSpeed, progress);
		//v.turnSpeed = simpleMath::vecLerp(v.startTurnSpeed, v.endTurnSpeed, progress);
		//vehicleMemory::setVehicleVelocity(v.memoryAddress, &v.speed);
		//vehicleMemory::setVehicleTurnVelocity(v.memoryAddress, &v.turnSpeed);

		vehicleMemory::setVehicleGasPedal(v.memoryAddress, v.rpm);
		vehicleMemory::setVehicleSteer(v.memoryAddress, v.steer);

		v.speed = simpleMath::vecLerp(v.startSpeed, v.endSpeed, progress);
		if(!v.collision)
		{
			simpleMath::Vector3 MoveSpeed;
			vehicleMemory::getVehicleVelocity(v.memoryAddress, &MoveSpeed);
			float SpeedXY = std::sqrtf(MoveSpeed.x * MoveSpeed.x + MoveSpeed.y * MoveSpeed.y);
			if(MoveSpeed.z > 0.00 && MoveSpeed.z < 0.02 && MoveSpeed.z > SpeedXY) //Usually commented out
			{
				MoveSpeed.z = SpeedXY;
				vehicleMemory::setVehicleVelocity(v.memoryAddress, &MoveSpeed);
				v.speed.z = SpeedXY;
				Scripting::PrintStringWithLiteralStringNow("STRING", "Ghost", 100, true);
			}
		}

		if(std::abs(v.speed.x) + std::abs(v.speed.y) + std::abs(v.speed.z) > 1.0f)
		{
			vehicleMemory::setVehicleVelocity(v.memoryAddress, &v.speed);
		}
		
		v.turnSpeed = simpleMath::vecLerp(v.startTurnSpeed, v.endTurnSpeed, progress);
		if(std::abs(v.turnSpeed.x) + std::abs(v.turnSpeed.y) + std::abs(v.turnSpeed.z) > 1.0f)
		{
			vehicleMemory::setVehicleTurnVelocity(v.memoryAddress, &v.turnSpeed);
		}

		if(v.horn > elap) // 0 cancel, 1 anim only, 25 continuous
		{
			*(DWORD*)(v.memoryAddress + 0x133C) = 25;//30 * v.horn - elap / 1000;
			//NativeInvoke::Invoke<u32>("SOUND_CAR_HORN", 30 * v.horn - elap / 1000);
		}
		else if(v.horn != 0)
		{
			//NativeInvoke::Invoke<u32>("SOUND_CAR_HORN", 100);
			*(DWORD*)(v.memoryAddress + 0x133C) = 0;
			v.horn = 0;
		}

		if(vehicleTasks::isValidAction(v.breakOrGas))
		{
			vehicleTasks::pedalsAction& action = vehicleTasks::getAction(v.breakOrGas);
			if(v.engineFlags != 0)
			{
				if(action.handBrake)
				{
					vehicleMemory::setVehicleHandBrake(v.memoryAddress, 140);
				}
				else
				{
					vehicleMemory::setVehicleHandBrake(v.memoryAddress, 12);
				}
			}

			if(action.brakePedal)
			{
				vehicleMemory::setVehicleBreakPedal(v.memoryAddress, 1);
			}
			else
			{
				vehicleMemory::setVehicleBreakPedal(v.memoryAddress, 0);
			}

			//vehicleMemory::setVehicleGear(v.memoryAddress, (BYTE)v.gear);
		}
		subTask = 405;
		
		for(unsigned int i = 0; i < 2; i++)
		{
			if(v.indicators[i])
			{
				if(elap < v.lastIndicator - 600)
				{
					*(BYTE*)(v.memoryAddress + 3947 + i) = (BYTE)177;
					continue;
				}
				else if(elap > v.lastIndicator)
				{
					v.lastIndicator = elap + 1200;
				}
			}
			*(BYTE*)(v.memoryAddress + 3947 + i) = (BYTE)0;
		}

		if(simpleMath::vecDistance(myLastPos, v.position) > 150.0f)
		{
			Scripting::SetCarCoordinatesNoOffset(v.vehicle, v.position.x, v.position.y, v.position.z);
		}
		else v.vPointer->SetPosition(&gameVectors::Vector3(v.position.x, v.position.y, v.position.z));
		/*LINFO << "Calling";
		#define SLODWORD(x)  (*((int*)&(x)))
		int(__cdecl *SP)(int a1, float a2, float a3, int a4, char a5, char a6) = (int(__cdecl*)(int a1, float a2, float a3, int a4, char a5, char a6))(gameMemory::getModuleHandle() + 0x9C4320);
		SP(v.memoryAddress, v.position.x, v.position.y, SLODWORD(v.position.z), 0, 0);*/
		Scripting::SetVehicleQuaternion(v.vehicle, rotBuf.X, rotBuf.Y, rotBuf.Z, rotBuf.W);

		if(v.engineFlags == 0 && (*(BYTE*)(v.memoryAddress + 0x0F64) & 8) != 0)
		{
			updateEngineFlags(v);
		}

		/*char str[150] = "";
		sprintf_s(str, "Gear %i", (int)(*(BYTE*)(v.memoryAddress + 4320)));
		Scripting::PrintStringWithLiteralStringNow("STRING", str, 200, true);*/

		//*(BYTE*)(v.memoryAddress + 3948) = v.indicators[0];
		/*std::ostringstream s;
		s << "Ping: " << v.polTime;
		Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 200, true);*/

		v.threadLock = false;
		subTask = 406;
	}
	catch(std::exception& e)
	{
		if(vehicles::isVehicle(vehicleId))
		{
			vehicles::getVehicle(vehicleId).threadLock = false;
		}
		LINFO << "!Error (syncVehicle): " << e.what() << ": " << subTask;
	}
}

void updateEngineFlags(vehicles::netVehicle& v)
{
	//LINFO << "Vehicle " << v.serverId << " engine: " << v.engineFlags;
	//NativeInvoke::Invoke<u32>("SET_CAR_ENGINE_ON", v.vehicle, v.engineFlags == 2, 0);
	Scripting::SetCarEngineOn(v.vehicle, v.engineFlags == 2, 0);
	if(v.engineFlags == 0)
	{
		//NativeInvoke::Invoke<u32>("SET_PETROL_TANK_HEALTH", v.vehicle, 0.0f);
		Scripting::SetPetrolTankHealth(v.vehicle, 0.0f);
	}
}

void setMemBitAuto(DWORD memory, DWORD bit, int dataType, float value)
{
	if(dataType == 0) *(DWORD*)(memory + bit) = (DWORD)value;
	else if(dataType == 1) *(float*)(memory + bit) = value;
	else if(dataType == 2) *(BYTE*)(memory + bit) = (BYTE)value;
	//LINFO << "MemBitter: " << bit << ", " << dataType << ", " << value;
}

void updateVehicleBits(vehicles::netVehicle& v)
{
	//LINFO << "Updating mem bits: " << v.serverId;
	for(std::map<unsigned int, vehicles::vMemBit>::iterator i = v.membits.begin(); i != v.membits.end(); ++i)
	{
		if(i->second.original < - 999.0f)
		{
			if(i->second.dataType == 0) i->second.original = (float)(*(DWORD*)(v.memoryAddress + i->first));
			else if(i->second.dataType == 1) i->second.original = *(float*)(v.memoryAddress + i->first);
			else if(i->second.dataType == 2) i->second.original = (float)(*(BYTE*)(v.memoryAddress + i->first));
			//LINFO << "Original bit set: " << i->first << ", " << i->second.original;
		}

		//if(i->second.current < -999.0f) setMemBitAuto(v.memoryAddress, i->first, i->second.dataType, i->second.original);
		//else 
		//{
			float buf = i->second.original;
			if(i->second.dataType != 1) buf = buf + i->second.current;
			else if(i->second.dataType == 1) buf = buf * i->second.current;
			setMemBitAuto(v.memoryAddress, i->first, i->second.dataType, buf);
		//}
	}
}

void updateMyLastPos(float x, float y, float z)
{
	myLastPos.x = x, myLastPos.y = y, myLastPos.z = z;
}

void handleVehicleDoors(vehicles::netVehicle& v)
{
	if(v.hood) Scripting::OpenCarDoor(v.vehicle, (Scripting::eVehicleDoor)4);
	else Scripting::ShutCarDoor(v.vehicle, 4);

	if(v.trunk) Scripting::OpenCarDoor(v.vehicle, (Scripting::eVehicleDoor)5);
	else Scripting::ShutCarDoor(v.vehicle, 5);
}