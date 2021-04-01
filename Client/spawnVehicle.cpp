#include "CustomFiberThread.h"
#include "modelsHandler.h"
#include "../ClientManager/ivScripting.h"
#include "easylogging++.h"
#include "gameMemory.h"
#include "vehicleSync.h"

bool SpawnCar(vehicles::netVehicle &veh)
{
	modelsHandler::request(veh.model, clock());
	if(!Scripting::HasModelLoaded(veh.model))
	{
		//LINFO << "!Error: vehicle model didnt load";
		return false;
	}

	LINFO << "Car model " << veh.model << " available... spawning it!";

	veh.driverSeatOccupied = false;
	veh.spawned = false;

	Scripting::CreateCar(veh.model, veh.position.x, veh.position.y, veh.position.z, &veh.vehicle, true);
	//Scripting::MarkModelAsNoLongerNeeded(veh.model);
	modelsHandler::unload(veh.model);

	if(!Scripting::DoesVehicleExist(veh.vehicle))
	{
		LINFO << "!Error: vehicle instance wasnt created: count " << vehicles::streamedVehicles() << ", " << vehicles::spawnedVehicles();
		return false;
	}

	veh.memoryAddress = gameMemory::getAddressOfItemInPool(*(DWORD*)(gameMemory::getVehiclesPool()), veh.vehicle.Get());
	if(veh.memoryAddress == 0)
	{
		LINFO << "!Error: Vehicle memory wasnt found, deleting vehicle";
		Scripting::DeleteCar(&veh.vehicle);
		return false;
	}

	veh.vPointer = (vehicleMemory::CIVVehicle*)veh.memoryAddress;
	//LINFO << "Testing pointer: " << veh.vPointer->GetEngineHealth();

	//LINFO << "ROTATION"; //REMOVE
	Scripting::SetCarCoordinates(veh.vehicle, veh.position.x, veh.position.y, veh.position.z);

	//vehicleMemory::setVehicleLocalRotation(veh.vPointer, veh.rotation);
	simpleMath::Quaternion bufRot = simpleMath::toQuaternion(veh.rotation.y, veh.rotation.x, veh.rotation.z);
	Scripting::SetVehicleQuaternion(veh.vehicle, bufRot.X, bufRot.Y, bufRot.Z, bufRot.W);

	//LINFO << "CORDS"; //Coords
	Scripting::ChangeCarColour(veh.vehicle, veh.color[0], veh.color[1]);
	//LINFO << "RCOLOR"; //Color
	Scripting::SetExtraCarColours(veh.vehicle, veh.color[2], veh.color[3]);
	//LINFO << "EXTRA"; //REMOVE


	//Scripting::SetCarEngineOn(veh.vehicle, 0, 1);
	Scripting::SetCarProofs(veh.vehicle, true, true, true, true, true);

	//LINFO << "Car HP IS " << veh.engineHealth; 
	//NativeInvoke::Invoke<u32>("SET_VEHICLE_ALPHA", veh.vehicle, 200);

	//LINFO << "Disable extra";
	for(unsigned int i = 0; i < 10; i++)
	{
		//NativeInvoke::Invoke<u32>("TURN_OFF_VEHICLE_EXTRA", veh.vehicle, i, !veh.tunes[i]);
		Scripting::TurnOffVehicleExtra(veh.vehicle, i, !veh.tunes[i]);
	}

	for(unsigned int i = 0; i < 6; i++)
	{
		if(veh.tyrePopped[i][0])
		{
			Scripting::BurstCarTyre(veh.vehicle, i);
		}
	}

	if(veh.siren)
	{
		//NativeInvoke::Invoke<u32>("SWITCH_CAR_SIREN", veh.vehicle, true);
		Scripting::SwitchCarSiren(veh.vehicle, true);
	}

	//NativeInvoke::Invoke<u32>("SET_VEHICLE_DIRT_LEVEL", veh.vehicle, veh.dirt);
	Scripting::SetVehicleDirtLevel(veh.vehicle, veh.dirt);
	//NativeInvoke::Invoke<u32>("SET_CAR_LIVERY", veh.vehicle, veh.livery);
	Scripting::SetCarLivery(veh.vehicle, veh.livery);
	//NativeInvoke::Invoke<u32>("SET_CAR_CAN_BE_VISIBLY_DAMAGED", veh.vehicle, false);
	Scripting::SetCarCanBeVisiblyDamaged(veh.vehicle, false);

	Scripting::SetEngineHealth(veh.vehicle, (float)veh.engineHealth);
	if(veh.engineHealth < -1200)
	{
		//NativeInvoke::Invoke<u32>("EXPLODE_CAR", veh.vehicle, 0);
		Scripting::ExplodeCar(veh.vehicle, 0);
	}
	/*if(veh.horn)
	{
	NativeInvoke::Invoke<u32>("SOUND_CAR_HORN", veh.vehicle, 90000);
	}*/

	//*(unsigned char*)(veh.memoryAddress + 0x0F70) |= 8u;

	veh.breakOrGas = 0;
	veh.steer = 0;

	veh.spawned = true;

	updateEngineFlags(veh);
	updateVehicleBits(veh);

	handleVehicleDoors(veh);

	//veh.lastRotationAlpha = 0.0;

	LINFO << "Vehicle spawned: " << veh.vehicle.Get() << ", " << veh.memoryAddress;

	//LINFO << "Test handle: " << vehicleMemory::getIndexFromHandle(veh.memoryAddress);
	return true;
}