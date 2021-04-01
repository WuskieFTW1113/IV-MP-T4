#include "prepareClientToServerPacket.h"
#include "../ClientManager/ivScripting.h"
#include "easylogging++.h"
#include "CustomFiberThread.h"
#include "chatWindow.h"
#include "keyboard.h"
#include "padCallBack.h"
#include "../SharedDefines/packetsIds.h"
#include "localPlayerAnim.h"

unsigned int enterCarId = 0;

int passenger_car = 0;
int driver_car = 0;

long entryEnd = 0;
long exitRequest = 0;
long targetSeat = -1;
int enteringCarId = 0;
bool passenger = false;
int isEntryCarJack = 0;
long carJackTime = 0;

bool keyG = false;

bool entryFrozen = false;

void nowInsideCar()
{
	enterCarId = 0;
	entryEnd = 0;
	passenger_car = 0;
	driver_car = 0;
	targetSeat = -1;
	passenger = false;

	isEntryCarJack = 0;
	setServerAnim(999999);

	if(entryFrozen)
	{
		togglePlayerControls(CONTROL_VEHICLE_ENTRY, true, false);
		entryFrozen = false;
	}
	else if(!chatWindow::isChatInputActive() && isGameKeyPressed(3) && 
		networkManager::getConnection()->cTime > exitRequest) { //Exit car

		exitRequest = networkManager::getConnection()->cTime + 2000;
		RakNet::BitStream b;
		b.Write((MessageID)IVMP);
		b.Write(ENTERING_VEHICLE);
		b.Write(-2);
		networkManager::sendBitStream(b);
		Scripting::TaskLeaveAnyCar(players::getLocalPlayer().ped);
		/*int vehId = players::getLocalPlayer().inVehicle;
		if(vehicles::isVehicle(vehId)) Scripting::SetCarEngineOn(vehicles::getVehicle(vehId).vehicle, 0, 0);*/
	}
}

void vehicleEntryCancelled()
{
	Scripting::ClearCharTasksImmediately(players::getLocalPlayer().ped);

	nowInsideCar();

	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(ENTERING_VEHICLE);
	b.Write(-1);

	networkManager::sendBitStream(b);
}

size_t findClosestCar()
{
	float closest = 999999.0;
	simpleMath::Vector3& bvec = players::getLocalPlayer().pos;
	size_t closestId = 0;
	simpleMath::Vector3 cPos;

	for(std::map<size_t, vehicles::netVehicle>::iterator i = vehicles::getVehiclesBegin(); i != vehicles::getVehiclesEnd(); ++i)
	{
		if(i->second.spawned)
		{
			Scripting::GetCarCoordinates(i->second.vehicle, &cPos.x, &cPos.y, &cPos.z);
			float buf = simpleMath::vecDistance(bvec, cPos);
			if(buf < closest && buf < 10.0)
			{
				closestId = i->first;
				closest = buf;
			}
		}
	}
	return closestId;
}

bool isSeatRequested(long cTime, int vehicleId, int seatId, bool allowJack)
{
	for(std::map<int, players::netPlayer>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		if(i->first > 0 && (i->second.vehicleEntryRequestTime > cTime &&
			i->second.enteringVehicle[0] == vehicleId && i->second.enteringVehicle[1] == seatId) ||
			(!allowJack &&
			(seatId == 0 && i->second.inVehicle == vehicleId) || 
			(seatId != 0 && i->second.passangerOf == vehicleId && i->second.passangerSeat + 1 == seatId))) {

			return true;
		}
	}
	return false;
}

void handleEnteringCar()
{
	players::netPlayer& p = players::getLocalPlayer();

	if(entryEnd != 0)
	{
		if(networkManager::getConnection()->cTime > entryEnd)
		{
			vehicleEntryCancelled();
		}
		else if(keyG)
		{
			keyG = false;
			if(networkManager::getConnection()->cTime - entryEnd > -11000)
			{
				vehicleEntryCancelled();
			}
		}
		/*std::ostringstream s;
		s << "Key: " << (int)isGameKeyPressed(54) << ", Time: " << networkManager::getConnection()->cTime - entryEnd;
		Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 500, true);*/
	}
	else if(isGameKeyPressed(54) && !chatWindow::isChatInputActive())
	{
		size_t closestId = findClosestCar();
		if(closestId == 0 || closestId == passenger_car)
		{
			LINFO << "No cars were found";
			return;
		}

		vehicles::netVehicle& v = vehicles::getVehicle(closestId);
		unsigned int maxP = 0;
		unsigned int warpSeat = 99;
		Scripting::GetMaximumNumberOfPassengers(v.vehicle, &maxP);
		//LINFO << "Seats: " << maxP;
		for(unsigned int i = 0; i < maxP; i++)
		{
			if(Scripting::IsCarPassengerSeatFree(v.vehicle, i))
			{
				warpSeat = i;
				break;
			}
		}

		if(warpSeat == 99)
		{
			LINFO << "No vehicle seats available";
			return;
		}

		if(isSeatRequested(clock(), closestId, warpSeat + 1, false))
		{
			return;
		}

		//NativeInvoke::Invoke<u32>("LOCK_CAR_DOORS", v.vehicle, 1);
		Scripting::LockCarDoors(v.vehicle, 1);
		//NativeInvoke::Invoke<u32>("TASK_ENTER_CAR_AS_PASSENGER", players::getPlayer(-2).ped, v.vehicle, 15000, warpSeat);
		Scripting::TaskEnterCarAsPassenger(players::getPlayer(-2).ped, v.vehicle, 15000, warpSeat);
		setServerAnim(999999);

		passenger = true;
		entryEnd = networkManager::getConnection()->cTime + 12000;
		passenger_car = closestId;
		targetSeat = warpSeat + 1;
		keyG = false;

		RakNet::BitStream b;
		b.Write((MessageID)IVMP);
		b.Write(ENTERING_VEHICLE);
		b.Write(passenger_car);
		b.Write(warpSeat + 1);
		networkManager::sendBitStream(b);

		togglePlayerControls(CONTROL_VEHICLE_ENTRY, false, false);
		entryFrozen = true;
		return;
	}
	else if(isGameKeyPressed(3) && !chatWindow::isChatInputActive())
	{	
		size_t closestId = findClosestCar();
		if(closestId != 0 && closestId != driver_car)
		{
			if(isSeatRequested(clock(), closestId, 0, true))
			{
				Scripting::ClearCharTasksImmediately(p.ped);
				return;
			}		

			isEntryCarJack = 0;

			DWORD driver = *(DWORD*)(vehicles::getVehicle(closestId).memoryAddress + 0x0FA0);
			//LINFO << "Driver: " << driver;
			if(driver != NULL)
			{
				for(std::map<int, players::netPlayer>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); i++)
				{
					if(i->first > 0 && i->second.spawned && driver == i->second.memoryAddress)
					{
						if(i->second.isAfk || i->first > 999)
						{
							Scripting::ClearCharTasksImmediately(p.ped);
							return;
						}
						isEntryCarJack = i->first;
						carJackTime = clock();
					}
				}
			}

			//NativeInvoke::Invoke<u32>("LOCK_CAR_DOORS", vehicles::getVehicle(closestId).vehicle, 1);
			Scripting::LockCarDoors(vehicles::getVehicle(closestId).vehicle, 1);
			//NativeInvoke::Invoke<u32>("TASK_ENTER_CAR_AS_DRIVER", players::getPlayer(-2).ped, vehicles::getVehicle(closestId).vehicle,
				//15000);
			Scripting::TaskEnterCarAsDriver(players::getPlayer(-2).ped, vehicles::getVehicle(closestId).vehicle, 15000);

			setServerAnim(999999);

			passenger = false;
			entryEnd = networkManager::getConnection()->cTime + 12000;
			driver_car = closestId;
			keyG = false;

			RakNet::BitStream b;
			b.Write((MessageID)IVMP);
			b.Write(ENTERING_VEHICLE);
			b.Write(closestId);
			b.Write(0);
			b.Write(isEntryCarJack != 0 ? 1 : 0);

			networkManager::sendBitStream(b);

			togglePlayerControls(CONTROL_VEHICLE_ENTRY, false, false);
			entryFrozen = true;
		}
	}
}

bool isPlayerEnteringCar()
{
	return entryEnd != 0;
}

void pressedKeyG(bool alt)
{
	keyG = true;
}

void registerVehicleEntryKeys()
{
	keyboard::registerFunc(0x46, pressedKeyG, true); // actually f
}

bool isExitingVehicle()
{
	return networkManager::getConnection()->cTime < exitRequest;
}

int getCarJack()
{
	return isEntryCarJack;
}

long getJackTime()
{
	return carJackTime;
}

void vehicleRequestData(int& vehicle, int& seat)
{
	if(!passenger)
	{
		vehicle = driver_car;
		seat = 0;
		return;
	}
	vehicle = passenger_car;
	seat = targetSeat;
}