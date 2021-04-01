#include "../SharedDefines/packetsIds.h"
#include "prepareClientToServerPacket.h"
#include "chatWindow.h"
#include "gameWorld.h"
#include "vehicles.h"

#include "players.h"
#include "CustomFiberThread.h"
#include <sstream>
#include "simpleMath.h"
#include <time.h>
#include "playerSkins.h"
#include "animList.h"
#include "easylogging++.h"
#include "playerMemory.h"
#include "weaponInfo.h"
#include "padCallBack.h"
#include "localPlayerAnim.h"
#include "gameMemory.h"
#include "cameraMemory.h"
#include "vehicleSync.h"

const u32 aimBones[10] = {0x4B3/*root*/, 0x4B4/*head*/, 0x4C9/*r forearm*/, 0x4C2/*l forearm*/,
0x4C8/*r upper*/, 0x4C1/*l upper*/, 0x1A2, 0x1A7, 0x1A3, 0x1A8};

Scripting::Vector3 aimBufBones[10];

void GetOffset(players::netPlayer& p, float distance, float &x, float &y, float &z)
{
	Scripting::Vector3 v;
	//NativeInvoke::Invoke<u32>("GET_PED_BONE_POSITION", p.ped, 1232, distance, distance * 0.042f, distance * -0.113f, &v);
	Scripting::GetPedBonePosition(p.ped, 1232, distance, distance * 0.042f, distance * -0.113f, &v);
	x = v.X;
	y = v.Y;
	z = v.Z;
}

bool deathPcktSent = false;
long lastDeathTime = 0;

long lastHealthTime = 0;
unsigned int lastHealthPck = 0;
clock_t lastHealthChange = 0;

long lastFreeze = 0;

u32 lastAmmo = 0;

bool isSmartShooting = true;

void handleDeath(players::netPlayer& p, int agentType, int agentId)
{
	setServerAnim(999999);

	LINFO << "Death reason: " << agentType << ", " << agentId;
	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(CLIENT_DEATH);
	if(clock() > lastHealthChange + 500)
	{
		b.Write(-1);
		b.Write(-1);
	}
	else
	{
		b.Write(agentType);
		b.Write(agentId);
	}

	networkManager::sendBitStream(b);
	lastDeathTime = networkManager::getConnection()->cTime;
	deathPcktSent = true;

	p.currentAnim = 10001; //death anim
}

void localPlayerRespawned(players::netPlayer& p)
{
	LINFO << "Im alive again";
	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(CLIENT_HAS_RESPAWNED);

	networkManager::sendBitStream(b);
	deathPcktSent = false;

	gameWorld::resetWeather();
	gameMemory::closeDoors(gameMemory::getDoorStatus());

	/*float x, y, z;
	gameWorld::getLocalSpawnCoords(x, y, z);
	Scripting::SetCharCoordinates(p.ped, x, y, z);
	Scripting::ClearCharTasksImmediately(p.ped);*/

	for(size_t i = 0; i < 2; i++)
	{
		if(p.props[i] != -1)
		{
			//NativeInvoke::Invoke<u32>("SET_CHAR_PROP_INDEX", p.ped, i, p.props[i]);
			Scripting::SetCharPropIndex(p.ped, i, p.props[i]);
		}
	}
}

int damage_lastEntity = -1;
bool damage_wasVehicle = false;

//Line Of Sight function 009C93D0

void getLastDamage(players::netPlayer& p, int& entityId, bool& wasVehicle, int& bone)
{
	for(std::map<int, players::netPlayer>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
	{
		if(i->second.spawned && Scripting::HasCharBeenDamagedByChar(p.ped, i->second.ped, false) && i->second.passangerOf == 0)
		{
			entityId = i->first;
			wasVehicle = false;
			lastHealthChange = clock();
			Scripting::GetCharLastDamageBone(p.ped, &bone);
			Scripting::ClearCharLastDamageBone(p.ped);
			return;
		}
	}

	for(std::map<size_t, vehicles::netVehicle>::iterator i = vehicles::getVehiclesBegin(); i != vehicles::getVehiclesEnd(); ++i)
	{
		if(i->second.spawned && Scripting::HasCharBeenDamagedByCar(p.ped, i->second.vehicle))
		{
			entityId = i->first;
			wasVehicle = true;
			lastHealthChange = clock();
			return;
		}
	}
	entityId = -1;
}

void healthStream(players::netPlayer& p, long cTime)
{
	unsigned int hp = 0;
	Scripting::GetCharHealth(p.ped, &hp);

	if(hp != lastHealthPck/* && cTime >= lastHealthTime + 1000*/)
	{
		int damageEntity = -1;
		bool wasCar = false;
		int bone = -1;
		getLastDamage(p, damageEntity, wasCar, bone);

		damage_lastEntity = damageEntity;
		damage_wasVehicle = wasCar;
		
		RakNet::BitStream b;
		b.Write((MessageID)IVMP);
		b.Write(SET_HEALTH);
		b.Write(hp);
		if(damageEntity == -1)
		{
			b.Write(-1);
		}
		else
		{
			b.Write(damageEntity);
			b.Write(wasCar ? 1 : 0);
			b.Write(bone);
		}
		networkManager::sendBitStream(b);

		lastHealthPck = hp;
		lastHealthTime = cTime;

		//NativeInvoke::Invoke<u32>("CLEAR_CHAR_LAST_DAMAGE_ENTITY", p.ped);
		Scripting::ClearCharLastDamageEntity(p.ped);
	}
}

void interiorStream(players::netPlayer& p)
{
	unsigned int cInterior = 0;
	//NativeInvoke::Invoke<u32>("GET_KEY_FOR_CHAR_IN_ROOM", p.ped, &cInterior);
	Scripting::GetKeyForCharInRoom(p.ped, &cInterior);

	/*std::ostringstream s;
	s << "Char: " << cInterior;
	Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 200, true);*/
	if(cInterior != p.interior || recentlyTped())
	{
		p.interior = cInterior;
		interiorStreamed();

		RakNet::BitStream b;
		b.Write((MessageID)IVMP);
		b.Write(PLAYER_UPDATED_INTERIOR);
		b.Write(p.interior);

		networkManager::sendBitStream(b);
	}
}

u32 lastArmor = 0;
void armorStream(players::netPlayer& p)
{
	u32 armor = 0;
	Scripting::GetCharArmour(p.ped, &armor);

	if(armor != lastArmor)
	{
		int damageEntity = -1;
		bool wasCar = false;
		int bone = -1;
		getLastDamage(p, damageEntity, wasCar, bone);

		RakNet::BitStream b;
		b.Write((MessageID)IVMP);
		b.Write(ARMOR_CHANGE);
		b.Write(armor);

		if(damageEntity == -1)
		{
			b.Write(-1);
		}
		else
		{
			b.Write(damageEntity);
			b.Write(wasCar ? 1 : 0);
			b.Write(bone);
		}

		networkManager::sendBitStream(b);

		lastArmor = armor;

		Scripting::ClearCharLastDamageEntity(p.ped);
	}
}

void changeCarProofs(int id)
{
	if(vehicles::isVehicle(id))
	{
		vehicles::netVehicle& v = vehicles::getVehicle(id);
		v.driver = 0;
		if(v.spawned)
		{
			Scripting::SetCarProofs(v.vehicle, true, true, true, true, true);
		}
	}
}

void refreshMyPlayerStatus(players::netPlayer& p, long cTime)
{
	try
	{
		if(p.memoryAddress == 0)
		{
			return;
		}

		if(p.toBeDeleted == 2)
		{
			players::respawnLocalPlayer();
			return;
		}

		/*Scripting::DrawSphere((float)997.8, (float)1999.96, (float)1.74, (float)1.0);
		Scripting::DrawSphere((float)981.0, (float)2045.0, (float)1.0, (float)1.0);
		Scripting::DrawSphere((float)955.4, (float)2002.7, (float)4.2, (float)1.0);*/

		//playerMemory::getPlayerPosition(p.memoryAddress, p.pos);
		Scripting::GetCharCoordinates(p.ped, &p.pos.x, &p.pos.y, &p.pos.z);
		p.pos.z -= 1.0;
		updateMyLastPos(p.pos.x, p.pos.y, p.pos.z);

		healthStream(p, cTime);
		armorStream(p);
		interiorStream(p);

		/*float ox, oy, oz;
		GetOffset(p, 50.0f, ox, oy, oz);
		Scripting::DrawSphere(ox, oy, oz, 1.0f);*/

		if(Scripting::IsCharDead(p.ped) || lastHealthPck < 100)
		{
			if(deathPcktSent)
			{
				return;
			}
			if(lastHealthPck < 100)
			{
				Scripting::TaskDie(p.ped);
			}
			setServerAnim(999999);
			LINFO << "Im dead";

			if(damage_lastEntity == -1)
			{
				handleDeath(p, -1, -1);
			}
			else if(damage_wasVehicle)
			{
				handleDeath(p, 1, damage_lastEntity);
			}
			else
			{
				handleDeath(p, 0, damage_lastEntity);
			}
			return;
		}

		if(Scripting::IsCharInAnyCar(p.ped))
		{
			inVehicleSync(p);
			if(isPlayerEnteringCar())
			{
				int eRequest[2] = {0};
				vehicleRequestData(eRequest[0], eRequest[1]);
				if(eRequest[1] == 0 && p.passangerOf != 0) //Entering car from passenger seat
				{
					p.passangerOf = 0, p.passangerSeat = 0;
					//Scripting::PrintStringWithLiteralStringNow("STRING", "WRONG SEAT", 100, true);
					return;
				}
			}
			nowInsideCar();
			//enterCarId = 0; //enter was successful
			return;
		}

		handleEnteringCar();

		if(p.inVehicle != 0)
		{
			//player is no longer inside a vehicle
			changeCarProofs(p.inVehicle);
			p.lastVehicle = p.inVehicle;
			//Scripting::PrintStringWithLiteralStringNow("STRING", "No longer the driver", 2000, true);
			LINFO << "Using vehicle: 0";
		}
		else if(p.passangerOf != 0)
		{
			changeCarProofs(p.passangerOf);
		}

		p.inVehicle = 0;
		p.passangerOf = 0;
		p.passangerSeat = -1;
		//p.currentAnim = 10002; //car exit anim

		if(deathPcktSent)
		{
			localPlayerRespawned(p);
		}

		//Scripting::GetCharVelocity(p.ped, &p.vx, &p.vy, &p.vz);
		Scripting::GetCharHeading(p.ped, &p.heading);

		//p.heading = cp->m_fHeading;

		unsigned int cWeapon;
		Scripting::GetCurrentCharWeapon(p.ped, &cWeapon);

		int nAnim = 0;

		p.isDucking = Scripting::IsCharDucking(p.ped);
		
		if(cWeapon == 0 && p.currentWeapon != 0)
		{
			p.weaponChanged = true;
			p.currentWeapon = 0;
		}
		else if(weaponInfo::isWeapon(cWeapon))
		{
			//unsigned int currentAmmo = 0;
			unsigned int ammoInClip = 0;
			Scripting::GetAmmoInCharWeapon(p.ped, cWeapon, &p.currentAmmo);
			Scripting::GetAmmoInClip(p.ped, cWeapon, &ammoInClip);

			bool isAiming = isGameKeyPressed(87) && cWeapon != 0;
			bool isShooting = (isGameKeyPressed(4) || Scripting::IsCharShooting(p.ped)) && ammoInClip > 0;

			if(cWeapon != p.currentWeapon)
			{
				p.weaponChanged = true;
				//p.currentAmmo = currentAmmo;
				p.currentWeapon = cWeapon;
				lastAmmo = p.currentAmmo;
			}

			if(isAiming || (isShooting && cWeapon != 0))
			{
				//LINFO << "Aim stat";
				bool checkAimAnyWay = true;
				if(isSmartShooting)
				{
					for(std::map<int, players::netPlayer>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
					{
						if(i->first == -2 || !i->second.spawned || !Scripting::IsPlayerFreeAimingAtChar(p.playerIndex, i->second.ped))
						{
							continue;
						}
						checkAimAnyWay = false;
						p.aim.z = -10000.0f;
						p.aim.x = (float)i->first;
						p.aim.y = -1;

						for(u32 bi = 0; bi < 10; bi++)
						{
							Scripting::GetPedBonePosition(i->second.ped, aimBones[bi], 0.0f, 0.0f, 0.0f, &aimBufBones[bi]);
							if(bi > 1) aimBufBones[bi].Z -= 0.2f;
							//Scripting::DrawSphere(aimBufBones[bi].X, aimBufBones[bi].Y, aimBufBones[bi].Z, bi == 0 ? 0.4f : 0.2f);
						}

						float aimTargetX, aimTargetY, aimTargetZ;
						cameraMemory::getCamTargetedCoords(aimTargetX, aimTargetY, aimTargetZ, simpleMath::vecDistance(p.pos, i->second.pos) + 0.5f);
						//Scripting::DrawSphere(aimTargetX, aimTargetY, aimTargetZ, 0.2f);

						float closest = 9999.0f, bufFindBone;
						u32 closestBone = 90;
						for(u32 bi = 0; bi < 10; bi++)
						{
							bufFindBone = simpleMath::getDistance(aimTargetX, aimTargetY, aimTargetZ, 
								aimBufBones[bi].X, aimBufBones[bi].Y, aimBufBones[bi].Z);
							if(/*bufFindBone < 0.6f && --So the crouching free aim check doesnt bug it*/bufFindBone < closest) {
								closest = bufFindBone;
								closestBone = bi;
							}
						}
						if(closestBone != 90) {
							if(!i->second.isDucking || closest < 0.4f) {
								p.aim.y = (float)aimBones[closestBone];
							}
							else checkAimAnyWay = true;
							/*char str[40];
							sprintf_s(str, 40, "Any: %i~n~Bone: %i~n~Dis: %f", (int)checkAimAnyWay, closestBone, closest);
							Scripting::PrintStringWithLiteralStringNow("STRING", str, 2000, true);*/
						}
						//std::string out = "Smart Aim: " + std::to_string(closestBone);
						break;
					}
				}
				if(checkAimAnyWay)
				{
					size_t aimIndex = 20000;
					//float bufDis = 0.0f;
					size_t maxRange = (size_t)weaponInfo::getWeapon(cWeapon)->range;

					std::vector<simpleMath::Vector3> aimList;
					cameraMemory::populateCamera(aimList, maxRange);
					size_t listSize = aimList.size();
					simpleMath::Vector3 torso;

					for(std::map<int, players::netPlayer>::iterator i = players::getPlayersBegin(); i != players::getPlayersEnd(); ++i)
					{
						if(i->first == -2 || !i->second.spawned || (size_t)simpleMath::vecDistance(i->second.pos, p.pos) > maxRange + 5)
						{
							continue;
						}
						torso = i->second.pos;
						torso.z += 1;

						for(size_t j = 0; j < listSize; j++)
						{
							if(simpleMath::vecDistance(torso, aimList[j]) < 1.5f)
							{
								aimIndex = j;
								break;
							}
						}
					}

					if(listSize != 0)
					{
						maxRange = listSize - 1; //Reusing var
						p.aim = aimIndex == 20000 || aimIndex + 2 > maxRange ? aimList.at(maxRange) : aimList.at(aimIndex + 2);
					}
				}
				
				//Scripting::DrawSphere(p.aim.x, p.aim.y, p.aim.z, (float)0.3);

				if(isShooting && p.currentAmmo > 0)
				{
					nAnim = 2;
					//Scripting::PrintStringWithLiteralStringNow("STRING", "Shooting", 2000, true);
				}
				else if(isAiming && nAnim == 0)
				{
					nAnim = 1;
				}

				float move = 0.0f;
				Scripting::GetCharSpeed(p.ped, &move);
				if(move > 1.0f)
				{
					nAnim += 2;
				}

				/*LINFO << "Anim: " << nAnim << ", aimIndex: " << aimIndex << ", maxRange: " << maxRange + 1 << 
					", pos: " << p.aim.x << ", " << p.aim.y << ", " << p.aim.z;*/

				/*std::ostringstream s;
				s << "Aim dis: " << aimIndex; //simpleMath::vecDistance(p.pos, p.aim);
				Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 50, true);*/
			}
			else if(p.currentWeapon != 0 && p.currentAmmo != lastAmmo && !p.weaponChanged)
			{
				lastAmmo = p.currentAmmo;
				RakNet::BitStream b;
				b.Write((MessageID)IVMP);
				b.Write(GIVE_WEAPON);
				b.Write(p.currentWeapon);
				b.Write(p.currentAmmo);
				networkManager::sendBitStream(b);
			}
		}

		if(isPlayerEnteringCar())
		{
			nAnim = 10003;
		}
		else if(nAnim == 0)
		{
			//clock_t sBegin = clock();
			//LINFO << "LocalPlayer";
			//bool climbing = NativeInvoke::Invoke<bool>("IS_PED_CLIMBING", p.ped);
			bool climbing = Scripting::IsPedClimbing(p.ped);
			bool onFoot = Scripting::IsCharOnFoot(p.ped);
			if(getServerAnim() != 999999 && !Scripting::IsPedRagdoll(p.ped))
			{
				float sx, sy, sz;
				Scripting::GetCharVelocity(p.ped, &sx, &sy, &sz);
				
				nAnim = getServerAnim();
				animList::anims& sAnim = animList::getAnimType(nAnim);
				if(sz > -0.3f && !Scripting::IsCharPlayingAnim(p.ped, sAnim.animGroup, sAnim.animName))
				{
					if(Scripting::HaveAnimsLoaded(sAnim.animGroup)/*NativeInvoke::Invoke<bool>("HAVE_ANIMS_LOADED", sAnim.animGroup)*/)
					{
						Scripting::TaskPlayAnimNonInterruptable(p.ped, sAnim.animName, 
							sAnim.animGroup, 1000, doesServerAnimRepeat(), false, false, true);

						//NativeInvoke::Invoke<u32>("BLOCK_PED_WEAPON_SWITCHING", p.ped, true);
						Scripting::BlockPedWeaponSwitching(p.ped, true);
						Scripting::GiveWeaponToChar(p.ped, 0, 1, true);
					}
					else
					{
						LINFO << "Request anim group " << sAnim.animGroup;
						//NativeInvoke::Invoke<u32>("REQUEST_ANIMS", sAnim.animGroup);
						Scripting::RequestAnims(sAnim.animGroup);
					}
				}
				//LINFO << "Server anim: " << nAnim;
			}
			else if(!climbing && onFoot)	
			{
				if(Scripting::IsCharSittingIdle(p.ped)) {
					nAnim = getSitting();
					//LINFO << "yes, " << nAnim;
				}
				else
				{
					size_t anim = searchForAnim(2);
					if(anim > 0 && animList::isValidAnim(anim))
					{
						nAnim = anim;
						//Scripting::PrintStringWithLiteralStringNow("STRING", "Parachute", 100, true);
					}
					else if(getPlayerControls() && (isGameKeyPressed(1090) || isGameKeyPressed(1091) ||
						isGameKeyPressed(1092) || isGameKeyPressed(1093))) {

						if(isGameKeyPressed(1)) //sprint
						{
							float speed;
							Scripting::GetCharSpeed(p.ped, &speed);
							nAnim = speed < 6.5f ? 6 : 7;
						}
						else
						{
							nAnim = 5;
						}

						/*std::ostringstream s;
						s << "Anim: " << nAnim;
						Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 100, true);*/
					}
				}
			}
			else if(climbing)
			{
				size_t anim = searchForAnim(1);
				//std::ostringstream s;
				if(anim > 0 && animList::isValidAnim(anim))
				{
					nAnim = anim;
				}
				//s << "Anim: " << animList::getAnimType(anim).animName << ", " << animList::getAnimType(anim).animGroup;
				//Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 100, true);
			}
		}

		if(nAnim != p.currentAnim)
		{
			p.oldAnim = p.currentAnim;
		}

		p.currentAnim = nAnim;
	}
	catch(std::exception& e)
	{
		LINFO << "Error(refreshMyPlayerStatus): " << e.what();
	}
}

void sendDisconnectMsg()
{
	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(ID_REMOTE_DISCONNECTION_NOTIFICATION);
	b.Write(1000);

	if(players::isPlayer(-2) && players::getLocalPlayer().spawned)
	{
		std::map<unsigned int, unsigned int> guns;
		for(unsigned int i = 0; i < 10; i++)
		{
			unsigned int weapon = 0;
			unsigned int ammo = 0;
			unsigned int useless = 0;
			Scripting::GetCharWeaponInSlot(players::getLocalPlayer().ped, i, &weapon, &ammo, &useless);
					
			if(weapon > 0)
			{
				guns[weapon] = ammo;
			}
		}				
		b.Write(guns.size());
		for(std::map<unsigned int, unsigned int>::iterator i = guns.begin(); i != guns.end(); ++i)
		{
			b.Write(i->first);
			b.Write(i->second);
		}
	}
	else
	{
		b.Write(0);
	}
	networkManager::sendBitStream(b);
}