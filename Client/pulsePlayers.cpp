#include "NetworkManager.h"
#include "../SharedDefines/packetsIds.h"
#include "prepareClientToServerPacket.h"
#include "players.h"
#include "vehicles.h"
#include "CustomFiberThread.h"
#include <sstream>
#include "simpleMath.h"
#include <time.h>
#include "easylogging++.h"
#include "playerSkins.h"
#include "animList.h"
#include "playerMemory.h"
#include "syncPlayer.h"
#include "guiManager.h"

void putCharInDriver(int playerid, players::netPlayer& p, int localVeh)
{
	p.currentAnim = 0;
	p.vehicleEntryRequestTime = 0;
	p.enteringVehicle[0] = 0, p.enteringVehicle[1] = 0;

	if(vehicles::getVehicle(p.inVehicle).spawned)
	{
		Scripting::Ped bufPed;
		vehicles::netVehicle& pv = vehicles::getVehicle(p.inVehicle);

		Scripting::GetDriverOfCar(pv.vehicle, &bufPed);

		//LINFO << "Warping " << playerid << " to vehicle " << p.inVehicle;
		if((getCarJack() == playerid && getJackTime() + 15000 > clock()) || localVeh == p.inVehicle)
		{
			//p.inVehicle = 0;
			//LINFO << "Warping denied for carjack sync";
			return;
		}
						
		if(bufPed != p.ped)
		{
			LINFO << "Warped";
			Scripting::WarpCharIntoCar(p.ped, pv.vehicle);
			pv.driver = playerid;
			pv.fullySpawnedWithDriver = true;
			//Scripting::SetHeliBladesFullSpeed(pv.vehicle);
			//Scripting::SetCarEngineOn(pv.vehicle, 1, 1);
		}
	}
}

void managePlayers()
{
	//LINFO << "Looping players";

	try
	{
		players::netPlayer& mp = players::getLocalPlayer();
		std::map<int, players::netPlayer>::iterator i = players::getPlayersBegin();

		int loopCount = 0;
		int viewPort;
		Scripting::GetGameViewportId(&viewPort);
		float screenX, screenY;

		subTask = 601;
		//speedoTest::removeNameTag(-1);
		subTask = 602;

		while(i != players::getPlayersEnd())
		{
			loopCount++;
			if(loopCount > 500)
			{
				LINFO << "!Error: Pulse players was stuck in a loop";
				break;
			}

			if(i->first == -2 /*|| i->second.threadLock*/ || (i->second.spawned && i->second.memoryAddress == 0))
			{
				++i;
				continue;
			}

			if(i->second.spawned && !Scripting::DoesCharExist(i->second.ped))
			{
				LINFO << "!Error: Player " << i->first << " was spawned yet char doesnt exist";
				i->second.spawned = false;
				i->second.toBeDeleted = 2;
				++i;
				continue;
			}

			if(!i->second.spawned)
			{
				subTask = 603;
				//i->second.threadLock = true;
				if(spawnRemotePlayer(i->first, i->second))
				{
					RakNet::BitStream bsOut;
					bsOut.Write((MessageID)IVMP);
					bsOut.Write(CLIENT_SPAWNED_PLAYER);
					bsOut.Write(i->first);
					networkManager::connection* con = networkManager::getConnection();
					con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);
				}
				//i->second.threadLock = false;
			}
			else if(i->second.spawned && i->second.toBeDeleted > 0)
			{	
				subTask = 604;
				if(i->second.toBeDeleted == 2)
				{
					//NativeInvoke::Invoke<u32>("GET_KEY_FOR_CHAR_IN_ROOM", i->second.ped, &i->second.interior);
					Scripting::GetKeyForCharInRoom(i->second.ped, &i->second.interior);
				}

				//LINFO << "Removing player " << i->second.serverId;
				//i->second.threadLock = true;

				if(i->second.parachute != 0 && Scripting::DoesObjectIdExist(i->second.parachute))
				{
					//NativeInvoke::Invoke<u32>("DELETE_OBJECT", &i->second.parachute);
					Scripting::DeleteObjectById(&i->second.parachute);
					i->second.parachute = 0;
				}

				Scripting::DeleteChar(&i->second.ped);
				i->second.spawned = false;
				//LINFO << "Deleted";

				if(i->second.toBeDeleted != 2)
				{
					RakNet::BitStream bsOut;
					bsOut.Write((MessageID)IVMP);
					bsOut.Write(CLIENT_DELETED_PLAYER);
					bsOut.Write(i->first);
					networkManager::connection* con = networkManager::getConnection();
					con->peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);

					//speedoTest::removeNameTag(i->first);

					players::removePlayerUsingIterator(i++);
					//LINFO << "Continuing";
					continue;
				}
				i->second.toBeDeleted = 0;
			}
			else
			{
				if(i->second.currentAnim == 10001 && !Scripting::IsCharDead(i->second.ped))
				{
					i->second.isAiming = false;
					i->second.isDucking = false;
					Scripting::ClearCharTasksImmediately(i->second.ped);
					Scripting::TaskDie(i->second.ped);
				}
				else if(i->second.currentAnim != 10001 && Scripting::IsCharDead(i->second.ped))
				{
					//Scripting::ClearCharTasksImmediately(p.ped);
					i->second.toBeDeleted = 2;
				}

				//i->second.threadLock = true;
				int camInVehicle = -1;

				/*comment section
				std::ostringstream s;
				s << "Player " << i->first << " vehicle: " << i->second.inVehicle;
				Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 50, true);*/

				if(i->second.inVehicle != 0 && i->second.currentAnim != 10004 && vehicles::isVehicle(i->second.inVehicle))
				{
					vehicles::netVehicle& tv = vehicles::getVehicle(i->second.inVehicle);
					if(i->second.vehicleEntryRequestTime == 0 || clock() > i->second.vehicleEntryRequestTime ||
						std::abs(tv.endSpeed.x) + std::abs(tv.endSpeed.y) + std::abs(tv.endSpeed.z) > 0.2f) {

							subTask = 605;					
							putCharInDriver(i->first, i->second, mp.inVehicle);
					}
				}
				else if(i->second.passangerOf != 0 && i->second.passangerSeat != -1)
				{
					subTask = 606;
					if(vehicles::isVehicle(i->second.passangerOf) && vehicles::getVehicle(i->second.passangerOf).spawned) 
					{
						camInVehicle = i->second.passangerOf;
						i->second.enteringVehicle[0] = 0, i->second.enteringVehicle[1] = 0;
						vehicles::netVehicle& veh = vehicles::getVehicle(i->second.passangerOf);
						if(!Scripting::IsCharInCar(i->second.ped, veh.vehicle))
						{
							LINFO << "Warping " << i->first << " to vehicle " << i->second.passangerOf << ", seat: " << i->second.passangerSeat;
							Scripting::WarpCharIntoCarAsPassenger(i->second.ped, veh.vehicle, i->second.passangerSeat);
							i->second.currentAnim = 0;
						}
					}
				}
				else
				{
					subTask = 607;
					if(Scripting::IsCharInAnyCar(i->second.ped))
					{
						if(i->second.currentAnim != 10004 && i->second.currentAnim != 10003 && 
							(i->second.vehicleEntryRequestTime == 0 || clock() > i->second.vehicleEntryRequestTime)) {

							LINFO << "Removing " << i->first << " from vehicle";
							i->second.enteringVehicle[0] = 0, i->second.enteringVehicle[1] = 0;
							subTask = 608;
							Scripting::Vehicle veh;
							Scripting::Ped bufPed;
							Scripting::GetCarCharIsUsing(i->second.ped, &veh);
							Scripting::GetDriverOfCar(veh, &bufPed);
							/*if(bufPed.Get() == i->second.ped.Get())
							{
								Scripting::SetCarEngineOn(veh, 0, 0);
							}*/
							//Scripting::WarpCharFromCarToCoord(i->second.ped, i->second.endPos.x, i->second.endPos.y, i->second.endPos.z);
							//NativeInvoke::Invoke<u32>("WARP_CHAR_FROM_CAR_TO_COORD", i->second.ped, 
								//i->second.endPos.x, i->second.endPos.y, i->second.endPos.z);
							Scripting::WarpCharFromCarToCoord(i->second.ped, i->second.endPos.x, i->second.endPos.y, i->second.endPos.z);

							i->second.currentAnim = 0;
							i->second.vehicleEntryRequestTime = 0;

							//Scripting::PrintStringWithLiteralStringNow("STRING", "REMOVED FROM CAR", 3000, true);
						}
					}
					else
					{
						/*if(Scripting::IsPedRagdoll(i->second.ped))
						{
							Scripting::SwitchPedToAnimated(i->second.ped, true);
						}*/

						bool duckBuf = Scripting::IsCharDucking(i->second.ped);
						if(i->second.isDucking && !duckBuf)
						{
							Scripting::SetCharDucking(i->second.ped, true);
						}
						else if(!i->second.isDucking && duckBuf)
						{
							Scripting::SetCharDucking(i->second.ped, false);
						}

						//i->second.threadLock = false;
						syncPlayer(i->first);
					}
				}
				subTask = 609;
				
				if(i->first < 1000 && i->second.shouldDrawName && simpleMath::vecDistance(i->second.pos, mp.pos) < 30.0 &&
					((camInVehicle == -1 && Scripting::IsCharOnScreen(i->second.ped)) 
					|| (camInVehicle != -1 && Scripting::IsCarOnScreen(vehicles::getVehicle(camInVehicle).vehicle)))) {

						subTask = 610;
						//float x, y, z;
						//Scripting::GetCharCoordinates(i->second.ped, &x, &y, &z);
						Scripting::Vector3 hpos;
						//NativeInvoke::Invoke<u32>("GET_PED_BONE_POSITION", i->second.ped, 1205, 0.0f, 0.0f, 0.0f, &hpos);
						Scripting::GetPedBonePosition(i->second.ped, 1205, 0.0f, 0.0f, 0.0f, &hpos);
						Scripting::GetViewportPositionOfCoord(hpos.X, hpos.Y, hpos.Z + 0.3f, viewPort, &screenX, &screenY);

						float correction = (screenX / guiManager::windowX()) - (strlen(i->second.playerName) * 5 / guiManager::windowX());

						Scripting::SetTextScale(0.2f, 0.3f);
						Scripting::SetTextDropshadow(true, 0, 0, 0, 200);
						Scripting::SetTextColour(i->second.color[0], i->second.color[1], i->second.color[2], i->second.color[3]);
						Scripting::DisplayTextWithLiteralString(correction, screenY / guiManager::windowY(), 
							"STRING", i->second.playerName);

						if(i->second.lastHpChange != 0) //life stuff
						{
							if(networkManager::getConnection()->cTime > i->second.lastHpChange + 1000)
							{
								i->second.lastHpChange = 0;
							}
							subTask = 611;
							Scripting::GetViewportPositionOfCoord(hpos.X, hpos.Y, hpos.Z - 0.5f, viewPort, &screenX, &screenY);

							float hpX = screenX / guiManager::windowX();
							float hpY = screenY / guiManager::windowY();
							Scripting::DrawRect(hpX, hpY, 0.01f, 0.01f, 255, 255, 255, 200);

							Scripting::DrawRect(hpX, hpY, 0.003f, 0.01f, 255, 0, 0, 200);

							Scripting::DrawRect(hpX, hpY, 0.01f, 0.003f, 255, 0, 0, 200);
						}

						//Scripting::DrawSphere(x, y, z + 1, 0.2f);

						/*std::ostringstream s;
						s << "X: " << x << ", Y: " << y;
						Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 100, true);*/

						//speedoTest::updateNameTag(i->first, i->second.playerName.c_str(), screenX, screenY);
				}
			}
			//i->second.threadLock = false;
			++i;
		}
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (pulsePlayers): " << e.what();
	}
}