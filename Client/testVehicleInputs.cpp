#include "testVehicleInputs.h"
#include "vehicles.h"
#include <time.h>
#include <sstream>
#include "vehicleTasks.h"
#include "../SharedDefines/easylogging++.h"

bool accel = false;
bool brake = false;
bool handBrake = false;

float turnSpeed = 200.0f;
int lastGear = 0;

clock_t lastTurn = 0;
clock_t startTurn = 0;

float turnStartAngle = 0.0f;
float turnEndAngle = 0.0f;

int lastWheelAction = 0;

void processLocalPlayerVehicleInput(int vehicle)
{
	vehicles::netVehicle& v = vehicles::getVehicle(vehicle);
	clock_t cTime = clock();
	if(lastTurn == 0)
	{
		lastTurn = cTime;
	}

	accel = ((GetAsyncKeyState(0x57)) != 0) ? true : false;

	brake = ((GetAsyncKeyState(0x53)) != 0) ? true : false;

	handBrake = ((GetAsyncKeyState(VK_SPACE) & 1) != 0) ? true : false;

	float speed = 0.0f;
	Scripting::GetCarSpeed(v.vehicle, &speed);

	if(GetAsyncKeyState(0x44) != 0)
	{
		if(lastWheelAction != 1)
		{
			turnEndAngle = -0.6f;
			turnStartAngle = v.steer;
			startTurn = cTime;
			lastWheelAction = 1;
			LINFO << "Right";
		}
	}
	else if(GetAsyncKeyState(0x41) != 0)
	{
		if(lastWheelAction != 2)
		{
			turnEndAngle = 0.6f;
			turnStartAngle = v.steer;
			startTurn = cTime;
			lastWheelAction = 2;
			LINFO << "Left";
		}
	}
	else if(lastWheelAction != 0)
	{
		turnEndAngle = 0.0f;
		turnStartAngle = v.steer;
		startTurn = cTime;
		lastWheelAction = 0;
		LINFO << "Not pressing";
	}

	int gear = lastGear;
	if((GetAsyncKeyState(VK_UP) & 1) != 0)
	{
		gear = gear >= 4 ? 5 : gear + 1;
	}
	else if((GetAsyncKeyState(VK_DOWN) & 1) != 0)
	{
		gear = gear <= 1 ? 0 : gear - 1;
	}

	unsigned char gameGear = vehicleMemory::getVehicleGear(v.memoryAddress);
	if(gear != lastGear || gameGear != (unsigned char)gear)
	{
		//vehicleMemory::setVehicleGear(v.memoryAddress, (unsigned char)gear);
		lastGear = gear;
	}

	vehicleMemory::setVehicleHandBrake(v.memoryAddress, (handBrake ? 204 : 76));
	vehicleMemory::setVehicleGasPedal(v.memoryAddress, (accel ? 1.0f : 0.0f));

	if(brake && !accel && speed < 0.5f)
	{
		vehicleMemory::setVehicleGasPedal(v.memoryAddress, -1.0f);
	}
	else
	{
		vehicleMemory::setVehicleBreakPedal(v.memoryAddress, (brake ? 1.0f : 0.0f));
	}

	//*(float*)(v.memoryAddress + 4336) = 0.99f; 


	float alpha = (float)(cTime - startTurn) / turnSpeed;
	alpha = simpleMath::ClampF(0.0f, alpha, 1.0f);
	LINFO << "Alpha: " << alpha << ", Start: " << turnStartAngle << ", End: " << turnEndAngle;
	float wheel = simpleMath::lerp(turnStartAngle, turnEndAngle, alpha);

	/*
	if(speed > 5.0f)
	{
		wheel = simpleMath::ClampF(-0.35f, wheel, 0.25f);
	}*/

	vehicleMemory::setVehicleSteer(v.memoryAddress, wheel);

	v.steer = wheel;
	v.rpm = accel;
	v.breakOrGas = vehicleTasks::findPlayerAction(handBrake, accel, brake);
	vehicleMemory::getVehicleVelocity(v.memoryAddress, &v.speed);
	vehicleMemory::getVehicleTurnVelocity(v.memoryAddress, &v.turnSpeed);
	vehicleMemory::getVehiclePosition(v.memoryAddress, v.position);
	//Scripting::GetVehicleQuaternion(v.vehicle, &v.rotation.X, &v.rotation.Y, &v.rotation.Z, &v.rotation.W);

	std::ostringstream s;
	s << "Gear: " << (int)lastGear << ", Wheel: " << wheel << ", Throttle: " << (accel ? "open" : "closed");
	Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 50, true);
}