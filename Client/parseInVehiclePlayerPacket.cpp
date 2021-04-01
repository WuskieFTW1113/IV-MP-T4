#include "parseOnFootPlayerPacket.h"
#include "requestAndReceivePlayerData.h"
#include "players.h"
#include "easylogging++.h"
#include "vehicles.h"
#include "vehicleRequestAndReceiveData.h"
#include "vehicleMemory.h"
#include "rotationMatrix.h"

void parseInVehicleSync(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);
				
		if(playerid > 0 && !players::isPlayer(playerid))
		{
			//requestPlayerDate(playerid);
			return;
		}

		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			//requestVehicleData(vehicleid);
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		v.driver = playerid;

		bsIn.Read(v.goPos.x);
		bsIn.Read(v.goPos.y);
		bsIn.Read(v.goPos.z);
		//v.goPos.z += (float)0.01;

		bsIn.Read(v.eRotation.x);
		bsIn.Read(v.eRotation.y);
		bsIn.Read(v.eRotation.z);
		//bsIn.Read(v.eRotation.W);

		//long bufPol;
		//bsIn.Read(bufPol);

		if(playerid > 0)
		{
			if(players::getPlayer(playerid).inVehicle != vehicleid)
			{
				players::getPlayer(playerid).lastVehicle = players::getPlayer(playerid).inVehicle;
				players::getPlayer(playerid).inVehicle = vehicleid;
				LINFO << "Player " << playerid << " vehicle: " << vehicleid;
			}

			players::getPlayer(playerid).pos = v.goPos;

			//bsIn.Read(v.endPedal);
			bsIn.Read(v.endSteer);

			bsIn.Read(v.breakOrGas);

			//bsIn.Read(v.gear);
			bsIn.Read(v.endRpm);

			bsIn.Read(v.endSpeed.x);
			bsIn.Read(v.endSpeed.y);
			bsIn.Read(v.endSpeed.z);

			bsIn.Read(v.endTurnSpeed.x);
			bsIn.Read(v.endTurnSpeed.y);
			bsIn.Read(v.endTurnSpeed.z);

			//bsIn.Read(v.endRpm);

			//v.starPedal = v.gPedal;
			v.startSteer = v.steer;
			v.startRpm = v.rpm;

			/*v.startSpeed->x = v.speed->x;
			v.startSpeed->y = v.speed->y;
			v.startSpeed->z = v.speed->z;*/
		}

		if(v.spawned && v.vPointer != nullptr)
		{
			gameVectors::Vector3 pp;
			v.vPointer->GetPosition(&pp);
			//simpleMath::Vector3 bufPos = simpleMath::Vector3(pp.x, pp.y, pp.z);
			//Scripting::GetCarCoordinates(v.vehicle, &v.position.x, &v.position.y, &v.position.z);
			v.startPos.x = pp.x, v.startPos.y = pp.y, v.startPos.z = pp.z;
			//Scripting::PrintStringWithLiteralStringNow("STRING", usingWhat, 300, true);
			//v.startPos = v.position;

			//vehicleMemory::getVehiclePosition(v.memoryAddress, v.startPos);
			//Scripting::GetVehicleQuaternion(v.vehicle, &v.sRotation.X, &v.sRotation.Y, &v.sRotation.Z, &v.sRotation.W);

			vehicleMemory::getVehicleLocalRotation(v.vPointer, v.sRotation);
			
			vehicleMemory::getVehicleVelocity(v.memoryAddress, &v.startSpeed);
			vehicleMemory::getVehicleTurnVelocity(v.memoryAddress, &v.startTurnSpeed);
			/*if(std::abs(v.endSpeed.x) + std::abs(v.endSpeed.y) + std::abs(v.endSpeed.z) > 1.0f)
			{
				vehicleMemory::setVehicleVelocity(v.memoryAddress, &v.endSpeed);
			}
			if(std::abs(v.endTurnSpeed.x) + std::abs(v.endTurnSpeed.y) + std::abs(v.endTurnSpeed.z) > 1.0f)
			{
				vehicleMemory::setVehicleTurnVelocity(v.memoryAddress, &v.endTurnSpeed);
			}*/
		}
		else
		{
			v.startPos = v.goPos;
			v.sRotation = v.eRotation;
			v.position = v.goPos;
			v.rotation = v.eRotation;
		}

		v.posError = v.goPos;
		v.posError.minus(v.startPos);
		
		clock_t cTime = clock();
		v.polTime = cTime > v.lastPol + 600 ? 100 : (cTime - v.lastPol);
		if(v.polTime == 0)
		{
			v.polTime = 1;
		}
		//LINFO << "Last pol: " << v.polTime;

		float correctBy = simpleMath::lerp(0.25f, 1.0f, simpleMath::UnlerpClamped(100, v.polTime, 400));

		v.posError.x *= correctBy;
		v.posError.y *= correctBy;
		v.posError.z *= correctBy;

		v.rotationError = GetOffsetDegrees(v.sRotation, v.eRotation);

		v.hasPol = true;
		v.lastPol = cTime;
		v.lastPolProgress = 0.0f;
		v.lastRotationAlpha = 0.0f;

		if(!v.canBeSpawned)
		{
			v.canBeSpawned = true;
			v.position = v.goPos;
			v.rotation = v.eRotation;
			v.hasPol = false;
			LINFO << "Vehicle " << vehicleid << " may now be spawned";
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseInVehicleSync): " << e.what();
	}
}

void parsePassangerSync(RakNet::BitStream& bsIn)
{
	try
	{
		int playerid;
		bsIn.Read(playerid);

		if(playerid > 0 && !players::isPlayer(playerid))
		{
			//requestPlayerDate(playerid);
			return;
		}

		size_t vehicleId = -1;
		bsIn.Read(vehicleId);

		if(!vehicles::isVehicle(vehicleId))
		{
			//requestVehicleData(vehicleId);
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleId);
		players::netPlayer& p = players::getPlayer(playerid);

		int seatId = -1;
		bsIn.Read(seatId);

		if(seatId > 0 && seatId < 10)
		{
			if(p.inVehicle != 0 && vehicles::isVehicle(p.inVehicle))
			{
				vehicles::getVehicle(p.inVehicle).driver = 0;
				vehicles::getVehicle(p.inVehicle).fullySpawnedWithDriver = false;
			}

			p.lastVehicle = p.inVehicle;
			p.inVehicle = 0;
			p.passangerOf = vehicleId;
			p.passangerSeat = seatId - 1;

			p.pos = vehicles::getVehicle(vehicleId).position;

			//LINFO << "Player " << playerid << " is passanger of " << p.passangerOf << " on seat " << p.passangerSeat;
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseWeaponChanged): " << e.what();
	}
}