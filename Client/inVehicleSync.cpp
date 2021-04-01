#include "NetworkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "CustomFiberThread.h"
#include "players.h"
#include "vehicles.h"
#include "easylogging++.h"
//#include "simpleMath.h"
#include <stdexcept>
#include "vehicleMemory.h"
#include "vehicleTasks.h"
#include "rotationMatrix.h"
#include "prepareClientToServerPacket.h"

clock_t myLastSync = -1;
clock_t lastPassengerSync = 0;

bool isLocalPlayerInThisVehicle(players::netPlayer& p, vehicles::netVehicle& v, int vehicleId)
{
	if(v.spawned && Scripting::IsCharInCar(p.ped, v.vehicle))
	{
		Scripting::Ped bufPed;
		Scripting::GetDriverOfCar(v.vehicle, &bufPed);

		if(bufPed.Get() == p.ped.Get())
		{
			p.inVehicle = vehicleId; 
			v.driver = -2;
			Scripting::SetCarProofs(v.vehicle, false, false, false, false, false);
			//LINFO << "Using vehicle: " << vehicleId;
			//Scripting::PrintStringWithLiteralStringNow("STRING", "Vehicle driver", 2000, true);
		}
		else
		{			
			p.passangerOf = vehicleId;
			p.passangerSeat = 1;
			unsigned int max = 0;
			Scripting::GetMaximumNumberOfPassengers(v.vehicle, &max);
			Scripting::Ped buf;
			for(unsigned int i = 0; i < max; i++)
			{
				Scripting::GetCharInCarPassengerSeat(v.vehicle, i, &buf);
				if(buf == p.ped)
				{
					p.passangerSeat = i + 1;
				}
			}
			
			//LINFO << "Passenger of: " << vehicleId;
		}
		return true;
	}
	return false;
}

void findPlayerVehicle(players::netPlayer& p)
{
	for(std::map<size_t, vehicles::netVehicle>::iterator it = vehicles::getVehiclesBegin(); it != vehicles::getVehiclesEnd(); ++it)
	{
		if(isLocalPlayerInThisVehicle(p, it->second, it->first))
		{
			break;
		}
	}

	if(p.inVehicle == 0 && p.passangerOf == 0)
	{
		Scripting::WarpCharFromCarToCoord(p.ped, p.pos.x, p.pos.y, p.pos.z + 2);
		Scripting::PrintStringWithLiteralStringNow("STRING", "Ejected from invalid vehicle", 2000, true);
	}
}

void getCarStats(vehicles::netVehicle& v)
{
	gameVectors::Vector3 bufPos;
	v.vPointer->GetPosition(&bufPos);
	v.position = simpleMath::Vector3(bufPos.x, bufPos.y, bufPos.z);

	simpleMath::Quaternion q;
	Scripting::GetVehicleQuaternion(v.vehicle, &q.X, &q.Y, &q.Z, &q.W);
	v.rotation = simpleMath::fromQuaternion(q);

	//vehicleMemory::getVehicleLocalRotation(v.vPointer, v.rotation);
	/*std::ostringstream s;
	s << r.x << ", " << r.y << ", " << r.z;
	Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 2500, true);*/

	//v.rotation.X = r.x, v.rotation.Y = r.y, v.rotation.Z = r.z, v.rotation.W = 0.0f;
}

bool nitro = false;
u32 nFtx[2] = {0, 0};

void inVehicleSync(players::netPlayer& p)
{
	try
	{
		int lastVehicle = p.inVehicle != 0 ? p.inVehicle : p.passangerOf;
		p.inVehicle = 0;
		p.passangerOf = 0;

		if(!vehicles::isVehicle(lastVehicle) || !isLocalPlayerInThisVehicle(p, vehicles::getVehicle(lastVehicle), lastVehicle))
		{
			findPlayerVehicle(p);
		}
		
		if(isPlayerEnteringCar())
		{
			return;
		}
		if(p.inVehicle == 0) //Intentionally added in order to block the execution from this point, correct value is equal 0
		{
			if(p.passangerOf == 0 || !vehicles::isVehicle(p.passangerOf))
			{
				return;
			}
			vehicles::netVehicle& v = vehicles::getVehicle(p.passangerOf);
			if(v.vPointer == nullptr)
			{
				return;
			}
			clock_t cTime = clock();
			if(cTime > lastPassengerSync + 300)
			{
				lastPassengerSync = cTime + 300;

				if(*(DWORD*)(v.memoryAddress + 0x0FA0) == 0) //No driver so lets sync this car
				{		
					getCarStats(v);
					RakNet::BitStream b;
					b.Write((MessageID)IVMP);
					b.Write(PASSENGER_SYNC_WITH_POSITION);
					b.Write(p.passangerOf);
					b.Write(p.passangerSeat);
					b.Write(v.position.x);
					b.Write(v.position.y);
					b.Write(v.position.z);
					b.Write(v.rotation.x);
					b.Write(v.rotation.y);
					b.Write(v.rotation.z);
					//b.Write(v.rotation.W);
					networkManager::sendBitStream(b);
				}
				else
				{
					RakNet::BitStream b;
					b.Write((MessageID)IVMP);
					b.Write(SERVER_RECEIVE_PASSANGER_SYNC);
					b.Write(p.passangerOf);
					b.Write(p.passangerSeat);
					networkManager::sendBitStream(b);
				}
			}
			return;
		}
		vehicles::netVehicle& v = vehicles::getVehicle(p.inVehicle);
		if(v.vPointer == nullptr)
		{
			return;
		}
		//vehicleMemory::getVehiclePosition(v.memoryAddress, v.position);
		//Scripting::GetCarCoordinates(v.vehicle, &v.position.x, &v.position.y, &v.position.z);

		getCarStats(v);
		v.startPos = v.position, v.goPos = v.position;
		v.sRotation = v.rotation, v.eRotation = v.rotation;	

		vehicleMemory::getVehicleVelocity(v.memoryAddress, &v.speed);
		vehicleMemory::getVehicleTurnVelocity(v.memoryAddress, &v.turnSpeed);

		v.steer = vehicleMemory::getVehicleSteer(v.memoryAddress);
		float speed;
		Scripting::GetCarSpeed(v.vehicle, &speed);

		if(speed > 5.0f)
		{
			if(v.steer > 0.25)
			{
				v.steer = 0.25f;
			}
			else if(v.steer < -0.25f)
			{
				v.steer = -0.25f;
			}
		}

		v.endSteer = v.steer, v.startSteer = v.steer;
		v.rpm = vehicleMemory::getVehicleGasPedal(v.memoryAddress), v.startRpm = v.rpm, v.endRpm = v.rpm;

		bool gasPedal = v.rpm > 0.0f || v.rpm < 0.0f;
		bool breakPedal = (int)vehicleMemory::getVehicleBreakPedal(v.memoryAddress) > 0.0f;
		bool handBreak = (int)vehicleMemory::getVehicleHandBrake(v.memoryAddress) == 140;

		v.breakOrGas = vehicleTasks::findPlayerAction(handBreak, gasPedal, breakPedal);
		
		v.engineHealth = (int)Scripting::GetEngineHealth(v.vehicle);
		if(v.engineHealth < -1200)
		{
			RakNet::BitStream b;
			b.Write((MessageID)IVMP);
			b.Write(CAR_HP_UPDATE);
			b.Write(p.inVehicle);
			b.Write(v.engineHealth);
			networkManager::sendBitStream(b);

			//NativeInvoke::Invoke<u32>("EXPLODE_CAR", v.vehicle, 1);
			Scripting::ExplodeCar(v.vehicle, true);
		}

		for(unsigned int i = 0; i < 6; i++)
		{
			if(Scripting::IsCarTyreBurst(v.vehicle, i) && !v.tyrePopped[i][0])
			{
				v.tyrePopped[i][0] = true;
				v.tyrePopped[i][1] = true;
			}
		}

		//bool bufSiren = NativeInvoke::Invoke<bool>("IS_CAR_SIREN_ON", v.vehicle);
		bool bufSiren = Scripting::IsCarSirenOn(v.vehicle);
		if(bufSiren != v.siren)
		{
			v.siren = bufSiren;
			RakNet::BitStream b;
			b.Write((MessageID)IVMP);
			b.Write(SIREN_STATE_CHANGED);
			b.Write(p.inVehicle);
			b.Write((int)v.siren);

			networkManager::sendBitStream(b);
		}

		//bufSiren = NativeInvoke::Invoke<bool>("IS_PLAYER_PRESSING_HORN", p.playerIndex);
		bufSiren = Scripting::IsPlayerPressingHorn(p.playerIndex);
		int hornLong = (int)bufSiren;
		if(hornLong != v.horn)
		{
			v.horn = hornLong;
			RakNet::BitStream b;
			b.Write((MessageID)IVMP);
			b.Write(HORN_STATE_CHANGED);
			b.Write(p.inVehicle);
			b.Write(v.horn);

			networkManager::sendBitStream(b);
		}

		long elap = clock();
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

		float dirt = 0.0f;
		//NativeInvoke::Invoke<u32>("GET_VEHICLE_DIRT_LEVEL", v.vehicle, &dirt);
		Scripting::GetVehicleDirtLevel(v.vehicle, &dirt);
		if(dirt > v.dirt + 1.0f)
		{
			v.dirt = dirt;

			RakNet::BitStream b;
			b.Write((MessageID)IVMP);
			b.Write(VEHICLE_DIRT_LEVEL);
			b.Write(p.inVehicle);
			b.Write(v.dirt);

			networkManager::sendBitStream(b);			
			/*std::ostringstream s;
			s << "Dirt: " << dirt;
			Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 2500, true);*/
		}

		/*if((GetAsyncKeyState(VK_F6) & 1) != 0)
		{
			if(nitro)
			{
				NativeInvoke::Invoke<u32>("STOP_PTFX", nFtx[0]);
				NativeInvoke::Invoke<u32>("SET_CAR_ENGINE_ON", v.vehicle, 0, 0);
				NativeInvoke::Invoke<u32>("SET_PETROL_TANK_HEALTH", v.vehicle, 0.0f);
			}
			else
			{
				NativeInvoke::Invoke<u32>("TRIGGER_PTFX_ON_VEH", "nitro_splutter", v.vehicle, 0.03f, -0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
				nFtx[0] = NativeInvoke::Invoke<u32>("START_PTFX_ON_VEH", "nitro_level", v.vehicle, 0.03f, -0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
				LINFO << "Nitro: " << nFtx[0];
			}
			nitro = !nitro;
		}
		if(nitro)
		{
			float ss = 0.0f;
			Scripting::GetCarSpeed(v.vehicle, &ss);
			simpleMath::Vector3 force(ss, 0.0f, 0.0f);
			ss /= 10.0f;
			if(ss > 1.0f)
			{
				ss = 1.0;
			}
			NativeInvoke::Invoke<u32>("EVOLVE_PTFX", nFtx[0], "nitro", ss);
		}*/
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (inVehicleSync): " << e.what();
	}
}