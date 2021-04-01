#include "localPlayerCamera.h"
#include "simpleMath.h"
#include "players.h"
#include "../SharedDefines/packetsIds.h"
#include "Scripting.h"
#include "gameWorld.h"
#include "easylogging++.h"
#include "gameVectors.h"
#include "gameMemory.h"

struct customCam
{
	Scripting::Camera cam;
	simpleMath::Vector3 pos;
	simpleMath::Vector3 look;

	int player;
	long lastPulse;
};

customCam* cCam = nullptr;

/*void loadWorldAtPosition(gameVectors::Vector3* vecPosition)
{
	BYTE * pByteUnknown = &(*(BYTE *)(gameMemory::getModuleHandle() + 0x110DFAC));
	DWORD loadF = gameMemory::getModuleHandle() + 0xB8CE40;

	_asm	push vecPosition;
	_asm	mov ecx, pByteUnknown;
	_asm	call loadF;
}*/

void localCamera::parsePos(RakNet::BitStream& bsIn)
{
	LINFO << "Creating cam";
	
	if(cCam == nullptr)
	{
		cCam = new customCam();
		//NativeInvoke::Invoke<u32>("CREATE_CAM", 14, &cCam->camId);
		Scripting::CreateCam(14, &cCam->cam);
		cCam->lastPulse = 0;
		cCam->player = 0;
	}
	bsIn.Read(cCam->pos.x);
	bsIn.Read(cCam->pos.y);
	bsIn.Read(cCam->pos.z);

	LINFO << "Setting pos";
	//NativeInvoke::Invoke<u32>("SET_CAM_POS", cCam->camId, cCam->pos.x, cCam->pos.y, cCam->pos.z);
	//NativeInvoke::Invoke<u32>("SET_CAM_ACTIVE", cCam->camId, 1);
	//NativeInvoke::Invoke<u32>("SET_CAM_PROPAGATE", cCam->camId, 1);
	//NativeInvoke::Invoke<u32>("ACTIVATE_SCRIPTED_CAMS", 1, 1);
	Scripting::SetCamPos(cCam->cam, cCam->pos.x, cCam->pos.y, cCam->pos.z);
	Scripting::SetCamActive(cCam->cam, true);
	Scripting::SetCamPropagate(cCam->cam, true);
	Scripting::ActivateScriptedCams(1, 1);

	LINFO << "Setting world";
	int camWorld = 1;
	bsIn.Read(camWorld);
	bsIn.Reset();
	bsIn.Write(camWorld);
	gameWorld::changedWorld(bsIn);
	LINFO << "Done";
}

void localCamera::parseLookAt(RakNet::BitStream& bsIn)
{
	if(cCam == nullptr)
	{
		return;
	}
	bsIn.Read(cCam->look.x);
	bsIn.Read(cCam->look.y);
	bsIn.Read(cCam->look.z);

	//NativeInvoke::Invoke<u32>("POINT_CAM_AT_COORD", cCam->camId, cCam->look.x, cCam->look.y, cCam->look.z);
	Scripting::PointCamAtCoord(cCam->cam, cCam->look.x, cCam->look.y, cCam->look.z);
}

void localCamera::parsePlayerAttach(RakNet::BitStream& bsIn)
{
	if(cCam == nullptr)
	{
		LINFO << "Requested cam attach but cam doesnt exist";
		return;
	}
	int playerid = 0;
	bsIn.Read(playerid);

	LINFO << "Attaching cam on player " << playerid;

	if(playerid == -2)
	{
		//NativeInvoke::Invoke<u32>("SET_CHAR_COLLISION", players::getPlayer(-2).ped, true);
		Scripting::SetCharCollision(players::getPlayer(-2).ped, true);
		destroy();
	}
	else
	{
		//NativeInvoke::Invoke<u32>("ATTACH_CAM_TO_PED", cCam->camId, players::getPlayer(playerid).ped);
		//NativeInvoke::Invoke<u32>("SET_CAM_ATTACH_OFFSET_IS_RELATIVE", cCam->camId, 1);
		//NativeInvoke::Invoke<u32>("SET_CAM_ATTACH_OFFSET", cCam->camId, 0.0f, -5.0f, 2.0f);
		cCam->player = playerid;
		//NativeInvoke::Invoke<u32>("SET_CHAR_COLLISION", players::getPlayer(-2).ped, false);
		Scripting::SetCharCollision(players::getPlayer(-2).ped, false);
	}
}

void localCamera::sendCamSync(networkManager::connection* con)
{
	if(cCam->player != 0 && players::isPlayer(cCam->player))
	{
		players::netPlayer& p = players::getPlayer(cCam->player);
		if(!p.spawned)
		{
			return;
		}

		float pi = 3.14159f;
		float heading = 0.0f;
		simpleMath::Vector3 pos;

		Scripting::GetCharHeading(p.ped, &heading);
		Scripting::GetCharCoordinates(p.ped, &pos.x, &pos.y, &pos.z);

		heading = ((heading + 90) * 3.141592f) / 180.0f;

		float invert = heading + pi;
		if(invert > 2 * pi)
		{
			invert = invert - (2 * pi);
		}

		float sphereX = pos.x + 7 * cos(invert);
		float sphereY = pos.y + 7 * sin(invert);

		//NativeInvoke::Invoke<u32>("SET_CAM_POS", cCam->camId, sphereX, sphereY, pos.z + 2.0f);
		//NativeInvoke::Invoke<u32>("POINT_CAM_AT_COORD", cCam->camId, pos.x, pos.y, pos.z);
		Scripting::SetCamPos(cCam->cam, sphereX, sphereY, pos.z + 2.0f);
		Scripting::PointCamAtCoord(cCam->cam, pos.x, pos.y, pos.z);

		Scripting::SetCharCoordinates(players::getLocalPlayer().ped, pos.x, pos.y, pos.z + 30);
	}
	else
	{
		//LINFO << "Player: " << cCam->player;
	}

	/*cCam->lastPulse = con->cTime;
	players::netPlayer& p = players::getPlayer(cCam->player);

	RakNet::BitStream b;
	b.Write((MessageID)IVMP);
	b.Write(CAMERA_POS);
	b.Write(p.pos.x);
	b.Write(p.pos.y);
	b.Write(p.pos.z);
	
	networkManager::sendBitStream(b);*/
}

bool localCamera::isActive()
{
	return cCam != nullptr;
}

void localCamera::destroy()
{
	LINFO << "Destroying cam";
	//NativeInvoke::Invoke<u32>("SET_CAM_ACTIVE", cCam->camId, 0);
	//NativeInvoke::Invoke<u32>("SET_CAM_PROPAGATE", cCam->camId, 0);
	//NativeInvoke::Invoke<u32>("ACTIVATE_SCRIPTED_CAMS", 0, 0);
	//NativeInvoke::Invoke<u32>("DESTROY_CAM", cCam->camId);

	Scripting::SetCamActive(cCam->cam, false);
	Scripting::SetCamPropagate(cCam->cam, false);
	Scripting::ActivateScriptedCams(0, 0);
	Scripting::DestroyCam(cCam->cam);

	delete cCam;
	cCam = nullptr;
	LINFO << "Done";
}