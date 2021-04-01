#include "chatWindow.h"
#include "../SharedDefines/packetsIds.h"
#include "vehicleSync.h"
#include "easylogging++.h"
#include "networkManager.h"
#include "vehicleRequestAndReceiveData.h"
#include "../SharedDefines/paramHelper.h"
#include "gameMemory.h"

std::map<int, long> requestedVehicleData;

void parseVehicleData(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received information for existing vehicle: " << vehicleid;
			return;
			/*if(vehicles::getVehicle(vehicleid).spawned)
			{
				vehicles::getVehicle(vehicleid).toBeDeleted = true;
				return;
			}*/
		}

		LINFO << "Adding class for vehicle: " << vehicleid;
		vehicles::netVehicle v;
		v.serverId = vehicleid;
		v.canBeSpawned = true;
		v.spawned = false;
		v.collision = true;
		v.fullySpawnedWithDriver = false;
		v.hasPol = false;
		v.toBeDeleted = 0;
		v.vPointer = nullptr;
		//v.gear = (BYTE)0;
		v.lastPol = networkManager::getConnection()->cTime;
		v.polTime = 50;
		v.driver = NULL;
		v.spawnTime = 0;
		v.lastIndicator = 0;
		v.horn = 0;
		v.livery = 0;
		v.dirt = 0.0f;
		v.engineFlags = 1;
		v.lastPolProgress = 0.0f;
		v.lastRotationAlpha = 0.0f;
		v.hood = v.trunk = false;

		v.steer = 0.0f, v.startSteer = 0.0f, v.endSteer = 0.0f;
		v.endRpm = 0.0f, v.rpm = 0.0f, v.startRpm = 0.0f;

		bsIn.Read(v.model);
		//LINFO << "Streamed model: " << v.model;
		//v.model = 418536135;

		bsIn.Read(v.position.x);
		bsIn.Read(v.position.y);
		bsIn.Read(v.position.z);

		bsIn.Read(v.rotation.x);
		bsIn.Read(v.rotation.y);
		bsIn.Read(v.rotation.z);
		//bsIn.Read(v.rotation.W);

		bsIn.Read(v.color[0]);
		bsIn.Read(v.color[1]);
		bsIn.Read(v.color[2]);
		bsIn.Read(v.color[3]);

		bsIn.Read(v.engineHealth);
		bsIn.Read(v.engineFlags);

		RakNet::RakString rs;
		bsIn.Read(rs);
		
		std::vector<std::string> tbuf;
		paramHelper::getParams(tbuf, std::string(rs), ",");
		size_t tuneSize = tbuf.size();

		for(unsigned int i = 0; i < 10; i++)
		{
			v.tunes[i] = false;
		}

		for(size_t k = 0; k < tuneSize; k++)
		{
			int partId = paramHelper::isInt(tbuf.at(k));
			if(partId > -1 && partId < 10)
			{
				//LINFO << "Part " << partId << " added";
				v.tunes[partId] = true;
			}
		}

		v.engineHpChanged = false;

		for(unsigned int i = 0; i < 6; i++)
		{
			v.tyrePopped[i][0] = false;
			v.tyrePopped[i][1] = false;
		}

		v.siren = false;
		for(unsigned int i = 0; i < 2; i++)
		{
			v.indicators[i] = false;
		}

		vehicles::addVehicle(vehicleid, v);

		requestedVehicleData.erase(vehicleid);
		LINFO << "Done";
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleData): " << e.what();
	}
}

void requestVehicleData(int vehicleId)
{
	try
	{
		networkManager::connection* con = networkManager::getConnection();
	
		if(requestedVehicleData.find(vehicleId) != requestedVehicleData.end() && con->cTime < requestedVehicleData.at(vehicleId) + 500)
		{
			return;
		}
	
		RakNet::BitStream bsOut;
		bsOut.Write((MessageID)IVMP);
		bsOut.Write(CLIENT_REQUEST_VEHICLE_DATA);
		bsOut.Write(vehicleId);

		con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);

		requestedVehicleData[vehicleId] = con->cTime;
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (requestVehicleData): " << e.what();
	}
}

void parseVehicleTyresData(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received tyres information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		int buf;

		for(unsigned int i = 0; i < 6; i++)
		{
			bsIn.Read(buf);
			v.tyrePopped[i][0] = buf == 1 ? true : false;

			//LINFO << "Vehicle " << vehicleid << " tyre " << i << " is popped?: " << v.tyrePopped[i][0];
			if(v.spawned && v.tyrePopped[i][0])
			{
				Scripting::BurstCarTyre(v.vehicle, i);
			}
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleTyresData): " << e.what();
	}
}

void parseVehiclePopTyre(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received pop tyre information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		int tyreId = 0;
		int pop = 0;

		bsIn.Read(tyreId);
		bsIn.Read(pop);

		if(tyreId < 0 || tyreId > 5)
		{
			LINFO << "!Error: Received pop tyre information for invalid tyreId: " << tyreId;
			return;
		}

		if(pop != 0 && pop != 1)
		{
			LINFO << "!Error: Received pop tyre information for invalid pop status: " << pop;
			return;
		}

		v.tyrePopped[tyreId][0] = pop == 1 ? true : false;
		v.tyrePopped[tyreId][1] = false;

		//LINFO << "Pop status: " << tyreId << ", " << pop;
		
		if(v.spawned)
		{
			pop == 1 ? Scripting::BurstCarTyre(v.vehicle, tyreId) : Scripting::FixCarTyre(v.vehicle, (unsigned int)tyreId);		
		}
			
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleTyresData): " << e.what();
	}
}

void parseVehicleHpUpdate(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car hp information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		int hp = 1000;
		bsIn.Read(hp);

		if(hp > 1000 || hp < -4000)
		{
			LINFO << "!Error: Received car hp information for invalid hp: " << hp;
			return;
		}

		int oldHp = v.engineHealth;
		v.engineHealth = hp;
		//v.engineHpChanged = true;

		if(v.spawned)
		{
			int visual = 0;
			bsIn.Read(visual);
			if(visual == 1)
			{
				if(oldHp < 0) //blown
				{
					v.toBeDeleted = 2;
				}
				for(unsigned int i = 0; i < 6; i++)
				{
					v.tyrePopped[i][0] = false;
					v.tyrePopped[i][1] = false;
					//NativeInvoke::Invoke<u32>("FIX_CAR_TYRE", v.vehicle, i);
					Scripting::FixCarTyre(v.vehicle, i);
				}
				Scripting::FixCar(v.vehicle);
			}
			Scripting::SetEngineHealth(v.vehicle, (float)v.engineHealth);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleTyresData): " << e.what();
	}
}

void parseDebugVehicle(RakNet::BitStream& bsIn)
{
	int vehicleId;
	bsIn.Read(vehicleId);
	if(!vehicles::isVehicle(vehicleId))
	{
		return;
	}

	vehicles::netVehicle& v = vehicles::getVehicle(vehicleId);
	std::ostringstream s;
	RakNet::RakString rs;
	RakNet::BitStream b;

	s << "Veh " << vehicleId << " driver: " << v.driver << ", anim: " << v.breakOrGas << ", updating: " << v.hasPol;
	rs = s.str().c_str();

	b.Write(rs);
	b.Write(0xFFFF0000);
	chatWindow::receivedChat(b);
}

void parseVehiclTunes(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car tunes information for non existing vehicle: " << vehicleid;
			return;
		}

		int tune = 0;
		int value = 0;
		bsIn.Read(tune);
		bsIn.Read(value);

		if(tune < 0 || tune > 9 || value < 0 || value > 9)
		{
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		v.tunes[tune] = value == 1;
		if(v.spawned)
		{
			//NativeInvoke::Invoke<u32>("TURN_OFF_VEHICLE_EXTRA", v.vehicle, tune, !v.tunes[tune]);
			Scripting::TurnOffVehicleExtra(v.vehicle, tune, !v.tunes[tune]);
			Scripting::SetEngineHealth(v.vehicle, (float)v.engineHealth);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleTunes): " << e.what();
	}
}

void parseVehicleColors(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car color information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		int color = 0;
		for(size_t i = 0; i < 4; i++)
		{
			bsIn.Read(color);
			v.color[i] = color;
		}

		if(v.spawned)
		{
			Scripting::ChangeCarColour(v.vehicle, v.color[0], v.color[1]);
			Scripting::SetExtraCarColours(v.vehicle, v.color[2], v.color[3]);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleColors): " << e.what();
	}
}

void parseVehicleSiren(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car siren information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		int sirenState = 0;
		bsIn.Read(sirenState);

		v.siren = sirenState == 1;

		LINFO << "Vehicle: " << vehicleid << " siren: " << (int)v.siren;

		if(v.spawned)
		{
			LINFO << "Setting status";
			//NativeInvoke::Invoke<u32>("SWITCH_CAR_SIREN", v.vehicle, v.siren);
			Scripting::SwitchCarSiren(v.vehicle, v.siren);
			LINFO << "Done";
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleSiren): " << e.what();
	}
}

void parseVehicleHorn(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car horn information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		int sirenState = 0;
		bsIn.Read(sirenState);

		if(sirenState == 1)
		{
			v.horn = networkManager::getConnection()->cTime + 60000;
		}
		else
		{
			v.horn = 1;
		}

		LINFO << "Horn changed";

		/*if(v.spawned)
		{
			NativeInvoke::Invoke<u32>("SOUND_CAR_HORN", v.vehicle, v.horn ? 50000 : 1);
		}*/
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleHorn): " << e.what();
	}
}

void parseVehicleIndicators(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car indicators information for non existing vehicle: " << vehicleid;
			return;
		}

		int indicator = 0;
		bsIn.Read(indicator);
		if(indicator != 0 && indicator != 1)
		{
			LINFO << "!Error: invalid indicators vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);

		int status = 0;
		bsIn.Read(status);
		v.indicators[indicator] = status == 1;

		LINFO << "Vehicle " << vehicleid << " indicator " << indicator << " new status: " << status;
		if(indicator == 0 && v.indicators[1])
		{
			v.indicators[1] = false;
			LINFO << "Indicator 1 was shut off";
		}
		else if(indicator == 1 && v.indicators[0])
		{
			v.indicators[0] = false;
			LINFO << "Indicator 0 was shut off";
		}

		/*if(v.spawned)
		{
			NativeInvoke::Invoke<u32>("SOUND_CAR_HORN", v.vehicle, v.horn ? 50000 : 1);
		}*/
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleIndicators): " << e.what();
	}
}

void parseVehicleVelocity(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car velocity information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		bsIn.Read(v.speed.x);
		bsIn.Read(v.speed.y);
		bsIn.Read(v.speed.z);

		if(v.spawned)
		{
			vehicleMemory::setVehicleVelocity(v.memoryAddress, &v.speed);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleVelocity): " << e.what();
	}
}

void parseVehiclePos(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car pos information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		bsIn.Read(v.position.x);
		bsIn.Read(v.position.y);
		bsIn.Read(v.position.z);

		if(v.spawned)
		{
			Scripting::SetCarCoordinates(v.vehicle, v.position.x, v.position.y, v.position.z);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehiclePos): " << e.what();
	}
}

void parseVehicleRot(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car rotation information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		bsIn.Read(v.rotation.x);
		bsIn.Read(v.rotation.y);
		bsIn.Read(v.rotation.z);
		if(v.spawned)
		{
			simpleMath::Quaternion q = simpleMath::toQuaternion(v.rotation.y, v.rotation.x, v.rotation.z);
			Scripting::SetVehicleQuaternion(v.vehicle, q.X, q.Y, q.Z, q.W);

			//v.vPointer->RemoveFromWorld();
			//vehicleMemory::setVehicleLocalRotation(v.vPointer, v.rotation);
			//v.vPointer->AddToWorld();

		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleRot): " << e.what();
	}
}

void parseVehicleLivery(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car livery information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		bsIn.Read(v.livery);

		if(v.spawned)
		{
			//NativeInvoke::Invoke<u32>("SET_CAR_LIVERY", v.vehicle, v.livery);
			Scripting::SetCarLivery(v.vehicle, v.livery);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleLivery): " << e.what();
	}
}

void parseVehicleDirt(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car dirt information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		bsIn.Read(v.dirt);

		if(v.spawned)
		{
			//NativeInvoke::Invoke<u32>("SET_VEHICLE_DIRT_LEVEL", v.vehicle, v.dirt);
			Scripting::SetVehicleDirtLevel(v.vehicle, v.dirt);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleDirt): " << e.what();
	}
}

void parseVehicleEngine(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car engine information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		bsIn.Read(v.engineFlags);
		if(v.spawned) updateEngineFlags(v);
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleEngine): " << e.what();
	}
}

void parseVehicleMemBit(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car bit information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		unsigned int off = 0;
		vehicles::vMemBit buf;
		bsIn.Read(off);
		bsIn.Read(buf.dataType);
		bsIn.Read(buf.current);

		if(v.membits.find(off) == v.membits.end()) buf.original = -1000.0f;
		else buf.original = v.membits[off].original;
		v.membits[off] = buf;

		if(v.spawned) updateVehicleBits(v);
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehicleMemBit): " << e.what();
	}
}

void parseVehiclBootAndTrunk(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car boot information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		bsIn.Read(v.hood);
		bsIn.Read(v.trunk);

		if(v.spawned)
		{
			handleVehicleDoors(v);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehiclBoot): " << e.what();
	}
}

void parseVehiclCol(RakNet::BitStream& bsIn)
{
	try
	{
		int vehicleid;
		bsIn.Read(vehicleid);

		if(!vehicles::isVehicle(vehicleid))
		{
			LINFO << "!Error: Received car col information for non existing vehicle: " << vehicleid;
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(vehicleid);
		bsIn.Read(v.collision);

		if(v.spawned)
		{
			Scripting::SetCarCollision(v.vehicle, v.collision);
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (parseVehiclBoot): " << e.what();
	}
}