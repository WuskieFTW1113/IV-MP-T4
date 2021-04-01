#include "parseConnectionStates.h"
#include "players.h"
#include <sstream>
#include <stdexcept>
#include <time.h>
#include "easylogging++.h"

#include "players.h"
#include "CustomFiberThread.h"
#include <sstream>
#include "simpleMath.h"
#include <time.h>
#include "easylogging++.h"
#include "playerSkins.h"
#include "animList.h"
#include "playerMemory.h"
#include "syncPlayer.h"
#include <regex>
//#include "NetworkManager.h"
//#include "../SharedDefines/packetsIds.h"

void updateFootNpc(int playerid, players::netPlayer& p)
{
	/*simpleMath::Quaternion pos;
	Scripting::GetCharCoordinates(p.ped, &pos.X, &pos.Y, &pos.Z);
	Scripting::GetCharHeading(p.ped, &pos.W);

	unsigned int weapon = 0, ammoInClip = 0;
	Scripting::GetCurrentCharWeapon(p.ped, &weapon);
	if(weapon != 0) Scripting::GetAmmoInClip(p.ped, weapon, &ammoInClip);

	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(NPC_POS);
	b.Write(pos.X);
	b.Write(pos.Y);
	b.Write(pos.Z);
	b.Write(pos.W);

	b.Write(weapon);
	if(weapon != 0) b.Write(ammoInClip);
*/
}

void syncPlayer(int playerid)
{
	try
	{
		players::netPlayer& p = players::getPlayer(playerid);

		if(p.threadLock || !p.spawned) /*|| Scripting::IsCharInAnyCar(p.ped))*/
		{
			return;
		}
		else if(!p.hasPol)
		{
			//Scripting::PrintStringWithLiteralStringNow("STRING", "NO POL", 100, true);
			simpleMath::Vector3 pos;
			Scripting::GetCharCoordinates(p.ped, &pos.x, &pos.y, &pos.z);
			if(simpleMath::vecDistance(pos, p.endPos) > 3.0f)
			{
				Scripting::SetCharCoordinates(p.ped, p.pos.x, p.pos.y, p.pos.z);
			}
			return;
		}
		else if(playerid > 999 && p.hasControl)
		{

		}

		p.threadLock = true;

		//float progress = 0.0;
		long elap = clock();

		/*if(elap >= p.lastPol + p.polTime)
		{
			progress = 1.0;
			//p.hasPol = false; not used with compensation
		}
		else
		{
			progress = (float)(elap - p.lastPol) / p.polTime;
		}*/
				
		//p.pos = simpleMath::vecLerp(p.startPos, p.endPos, progress);

		float nprogress = (float)(elap - p.lastPol) / p.polTime;
		nprogress = simpleMath::ClampF(0.0f, nprogress, 1.5f);

		float fCurrentAlpha = (nprogress - p.lastPolProgress);
		p.lastPolProgress = nprogress;

		simpleMath::Vector3 posCompensation = simpleMath::vecLerp(simpleMath::Vector3(), p.posError, fCurrentAlpha);
		Scripting::GetCharCoordinates(p.ped, &p.pos.x, &p.pos.y, &p.pos.z);
		p.pos.z -= 1.0f;
		p.pos.add(posCompensation);

		if(simpleMath::vecDistance(p.pos, p.endPos) > 2.0f)
		{
			p.pos = p.endPos;
		}

		if(nprogress > 1.49f)
		{
			p.hasPol = false;
		}

		float progress = simpleMath::ClampF(0.0f, nprogress, 1.0f);

		//simpleMath::Vector3 oldPos = simpleMath::vecLerp(p.startPos, p.endPos, progress);
		//Scripting::DrawSphere(mta_pos.x, mta_pos.y, mta_pos.z, 1.0f);

		/*simpleMath::Quaternion qt1, qt2, qt3;
		qt1.init(p.startHeading, 0.0, 0.0, 0.0);
		qt2.init(p.endHeading, 0.0, 0.0, 0.0);
		simpleMath::slerp(qt3, qt1, qt2, progress);
		p.heading = qt3.X;*/
		p.heading = p.endHeading;

		if(p.weaponChanged)
		{
			if(p.currentWeapon == 0)
			{
				Scripting::RemoveAllCharWeapons(p.ped);
			}
			else
			{
				Scripting::GiveWeaponToChar(p.ped, p.currentWeapon, 600, true);
			}
			p.endAmmo = 0;
			p.startAmmo = 0;
			p.weaponChanged = false;
		}

		if(p.currentAnim == 10003 || p.currentAnim == 10004)
		{
			//LINFO << p.playerName << " Not doing a fucking thing";
			p.threadLock = false;
			return;
		}

		if(p.currentAnim >= 1 && p.currentAnim <= 4)
		{
			simpleMath::Vector3 bufAim;

			if(p.endAim.z < -9999.0f)
			{
				int tPlayer = (int)p.endAim.x;
				if(tPlayer == getLocalPlayerServerId()) {
					tPlayer = -2;
				}

				if(players::isPlayer(tPlayer) && players::getPlayer(tPlayer).spawned) {
					int tBone = (int)p.endAim.y;
					if(tBone != -1) {
						Scripting::Vector3 bAim;
						Scripting::GetPedBonePosition(players::getPlayer(tPlayer).ped, tBone, 0.0f, 0.0f, 0.0f, &bAim);
						bufAim.x = bAim.X, bufAim.y = bAim.Y, bufAim.z = bAim.Z;
					}
					else {
						Scripting::GetCharCoordinates(players::getPlayer(tPlayer).ped, &bufAim.x, &bufAim.y, &bufAim.z);
					}
					p.aim.z -= 1.0f; //Fixes aiming anim glitch with smart sync
					//Scripting::DrawSphere(bufAim.x, bufAim.y, bufAim.z, 0.2f);
				}
			}
			else
			{
				bufAim = simpleMath::vecLerp(p.startAim, p.endAim, progress);

			}
			//unsigned int bufAmmo = (unsigned int)simpleMath::lerp((float)p.startAmmo, (float)p.endAmmo, progress);
			//NativeInvoke::Invoke<u32>("SET_CHAR_AMMO", p.ped, p.currentWeapon, 300);
			//Scripting::SetCharAmmo(p.ped, p.currentWeapon, 300);
			//Scripting::SetAmmoInClip(p.ped, (Scripting::eWeapon)p.currentWeapon, 5);
			//Scripting::DrawSphere(bufAim.x, bufAim.y, bufAim.z, (float)0.2);

			if(!p.isAiming || simpleMath::vecDistance(bufAim, p.aim) > 0.1f)
			{
				p.aim = bufAim;
				if(p.currentAnim < 3 || !p.isAiming)
				{
					Scripting::TaskAimGunAtCoord(p.ped, p.aim.x, p.aim.y, p.aim.z, 45000);
					p.isAiming = true;
				}
				else if(p.isAiming)
				{
					//Scripting::TaskAimGunAtCoord(p.ped, p.aim.x, p.aim.y, p.aim.z, 45000);
					//NativeInvoke::Invoke<u32>("TASK_GO_TO_COORD_WHILE_AIMING", p.ped, 
						//p.endPos.x, p.endPos.y, p.endPos.z, 2, 0, 0, 1, p.aim.x, p.aim.y, p.aim.z, 0);
					Scripting::TaskGotoCoordWhileAiming(p.ped, p.endPos.x, p.endPos.y, p.endPos.z, 2, 0, 0, 1, p.aim.x, p.aim.y, p.aim.z, 0);
					p.isAiming = true;
				}

				/*std::ostringstream s;
				s.precision(1);
				s << std::fixed << bufAim.x << ", " << bufAim.y << ", " << bufAim.z;
				Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 10000, true);*/
			}

			if(p.isAiming && (p.currentAnim == 2 || p.currentAnim == 4/* || bufAmmo < p.currentAmmo*/) && elap > p.startAmmo + 30)
			{
				Scripting::FirePedWeapon(p.ped, p.aim.x, p.aim.y, p.aim.z);
				//bulletCount++;
				p.startAmmo = elap;
				//p.currentAmmo = bufAmmo;
				//Scripting::PrintStringWithLiteralStringNow("STRING", "Shooting", 100, true);
			}
		}
		else if(p.currentAnim > 4 && p.currentAnim < 8) //walk, run, sprint
		{
			p.animLastForced = p.currentAnim;
			//Scripting::TaskFollowNavMeshToCoordNoStop(p.ped, p.pos.x, p.pos.y, p.pos.z, p.currentAnim - 3, -2, -1);
			//Scripting::TaskGoStraightToCoord(p.ped, p.pos.x, p.pos.y, p.pos.z, 4, -2);
			float sphereX = p.pos.x + 3 * cos(p.cAtan);
			float sphereY = p.pos.y + 3 * sin(p.cAtan);
			float nz;
			Scripting::GetGroundZFor3DCoord(sphereX, sphereX, p.pos.z, &nz);

			if(nz < 0.1f || std::abs(p.pos.z - nz) > 3.0)
			{
				nz = p.pos.z;
			}
			//Scripting::DrawSphere(sphereX, sphereY, nz, 1.0f);
			//NativeInvoke::Invoke<u32>("TASK_GO_STRAIGHT_TO_COORD", p.ped, sphereX, sphereY, nz + 1, p.currentAnim - 3, -1);
			Scripting::TaskGoStraightToCoord(p.ped, sphereX, sphereY, nz + 1, p.currentAnim - 3, -1);
			//NativeInvoke::Invoke<u32>("SET_CHAR_KEEP_TASK", p.ped, true);
			Scripting::SetCharKeepTask(p.ped, true);
			//Scripting::PrintStringWithLiteralStringNow("STRING", "Moving", 1000, true);
		}
		else if(animList::isValidAnim(p.currentAnim))
		{
			std::string anim = std::string(animList::getAnimType(p.currentAnim).animName);
			std::string animGrp = std::string(animList::getAnimType(p.currentAnim).animGroup);
			animList::anims& climb = animList::getAnimType(p.currentAnim);
			if(animGrp.compare(0, 9, std::string("climb_std")) == 0)
			{
				p.serverAnim = p.currentAnim;
				if(!Scripting::IsCharPlayingAnim(p.ped, climb.animGroup, climb.animName))
				{
					Scripting::TaskPlayAnimNonInterruptable(p.ped, climb.animName, "climb_std", 1000, false, false, false, true);
				}
				else if(Scripting::HasCharAnimFinished(p.ped, "climb_std", climb.animName))
				{
					if(!std::regex_match(anim, std::regex("idle")))
					{
						Scripting::TaskPlayAnimNonInterruptable(p.ped, climb.animName, "climb_std", 1000, false, false, false, true);
					}
				}
				/*std::ostringstream s;
				s << "Climb anim: " << anim << " (" << animGrp << ")";
				Scripting::PrintStringWithLiteralStringNow("STRING", s.str().c_str(), 100, true);*/
					
				/*Scripting::TaskPlayAnimNonInterruptable(p.ped, animList::getAnimType(p.currentAnim).animName,
						animList::getAnimType(p.currentAnim).animGroup, 10000000, false, false, false, true);*/
				//LINFO << "Invalid speed: " << animList::getAnimType(p.currentAnim).animName;
			}
			else if(!Scripting::IsCharPlayingAnim(p.ped, climb.animGroup, climb.animName))
			{			
				if(animGrp == std::string("parachute") && anim.compare(0, 5, std::string("free_")) != 0 && p.parachute == 0)
				{
					//NativeInvoke::Invoke<u32>("CREATE_OBJECT", 1490460832, p.pos.x, p.pos.y, p.pos.z + 2.0f, &p.parachute, 1);
					Scripting::Object buf;
					Scripting::CreateObject(1490460832, p.pos.x, p.pos.y, p.pos.z + 2.0f, &buf, 1);
					//NativeInvoke::Invoke<u32>("ATTACH_OBJECT_TO_PED", p.parachute, p.ped, 0, 0.025f, -0.125f, 5.45f, 0.0f, 0.0f, 0.0f, 1);
					Scripting::AttachObjectToPed(buf, p.ped, 0, 0.025f, -0.125f, 5.45f, 0.0f, 0.0f, 0.0f, 1);
					p.parachute = buf.Get();
				}
				if(Scripting::HaveAnimsLoaded(climb.animGroup))
				{
					Scripting::TaskPlayAnimNonInterruptable(p.ped, climb.animName, 
						climb.animGroup, 1000, false, false, false, true);
					p.serverAnim = p.currentAnim;
				}
				else
				{
					LINFO << "Request anim group " << climb.animGroup;
					//NativeInvoke::Invoke<u32>("REQUEST_ANIMS", climb.animGroup);
					Scripting::RequestAnims(climb.animGroup);
				}
			}
		}

		if(!p.isAiming && p.currentAnim != 10001 && (p.currentAnim < 5 || p.currentAnim > 7) && (p.animLastForced > 4 && p.animLastForced < 8))
		{
			Scripting::ClearCharTasks(p.ped);
			//Scripting::PrintStringWithLiteralStringNow("STRING", "Canceled 1", 1000, true);
			p.animLastForced = 0;
			p.serverAnim = 0;
		}
		else if(p.isAiming && (p.currentAnim < 1 || p.currentAnim > 4))
		{
			Scripting::ClearCharTasksImmediately(p.ped);
			//Scripting::PrintStringWithLiteralStringNow("STRING", "Canceled 2", 1000, true);
			p.isAiming = false;
		}
		if(p.serverAnim != 0 && p.serverAnim != p.currentAnim)
		{
			if(p.currentAnim == 0)
			{
				Scripting::ClearCharTasksImmediately(p.ped);
			}
			if(p.parachute != 0 && Scripting::DoesObjectIdExist(p.parachute))
			{
				//NativeInvoke::Invoke<u32>("DELETE_OBJECT", &p.parachute);
				Scripting::DeleteObjectById(&p.parachute);
				p.parachute = 0;
			}
			p.serverAnim = 0;
		}

		//playermemory::setplayerPosition(p.memoryAddress, p.pos);
		bool ragdoll = Scripting::IsPedRagdoll(p.ped);
		if(p.currentAnim != 10001 && (!ragdoll || elap > p.lastRagdoll))
		{
			if(ragdoll)
			{
				p.lastRagdoll = elap + 3000;
			}
			Scripting::SetCharCoordinates(p.ped, p.pos.x, p.pos.y, p.pos.z);
			//NativeInvoke::Invoke<u32>("SET_CHAR_COORDINATES_DONT_CLEAR_PLAYER_TASKS", p.ped, p.pos.x, p.pos.y, p.pos.z);
			//NativeInvoke::Invoke<u32>("SET_CHAR_COORDINATES_NO_OFFSET", p.ped, p.pos.x, p.pos.y, p.pos.z + 1);

				/*playerMemory::CIVPed* cv = (playerMemory::CIVPed*)p.memoryAddress;
				cv->RemoveFromWorld();
				gameVectors::Vector3 gv(p.pos.x, p.pos.y, p.pos.z + 1);
				cv->SetPosition(&gv);
				cv->AddToWorld();*/
		}
		if(!p.isAiming)
		{
			Scripting::SetCharHeading(p.ped, p.heading);
		}
		//Scripting::SetCharDesiredHeading(p.ped, p.endHeading);
		p.threadLock = false;
	}
	catch(std::exception& e)
	{
		LINFO << "!Error (syncVehicle): " << e.what();
	}
}