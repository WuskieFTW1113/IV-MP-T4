#include "gameBlips.h"
#include "../SharedDefines/packetsIds.h"
#include <map>
#include "CustomFiberThread.h"
#include "easylogging++.h"
#include "../ClientManager/ivScripting.h"

struct apiBlip
{
	Scripting::Blip blip;
	float pos[3];
	int type;
	unsigned int color;
	unsigned int world;
	unsigned int streamDistance;
	bool showAll;
	RakNet::RakString name;
};

std::map<int, apiBlip> blips;

void gameBlips::spawnBlip(RakNet::BitStream& bsIn)
{
	int id = 0;
	bsIn.Read(id);
	if(id < 1 || blips.find(id) != blips.end())
	{
		LINFO << "Invalid blip spawn id: " << id;
		return;
	}

	apiBlip b;
	bsIn.Read(b.pos[0]);
	bsIn.Read(b.pos[1]);
	bsIn.Read(b.pos[2]);
	bsIn.Read(b.type);
	bsIn.Read(b.color);
	bsIn.Read(b.world);
	bsIn.Read(b.streamDistance);
	bsIn.Read(b.name);

	Scripting::AddBlipForCoord(b.pos[0], b.pos[1], b.pos[2], &b.blip);
	if(b.streamDistance == 1)
	{
		//NativeInvoke::Invoke<u32>("SET_BLIP_AS_SHORT_RANGE", b.blip, true);
		Scripting::SetBlipAsShortRange(b.blip, true);
	}
	//NativeInvoke::Invoke<u32>("CHANGE_BLIP_SPRITE", b.blip, b.type);
	Scripting::ChangeBlipSprite(b.blip, b.type);

	if(b.color != 1)
	{
		//NativeInvoke::Invoke<u32>("CHANGE_BLIP_COLOUR", b.blip, b.color); //rgba
		Scripting::ChangeBlipColour(b.blip, b.color);
	}

	if(!b.name.IsEmpty())
	{
		Scripting::ChangeBlipNameFromAscii(b.blip, b.name.C_String());
	}

	//LINFO << "Blip: " << id << ", type: " <<b.type << ", pos: " << b.pos[0] << ", " << b.pos[1] << ", " << b.pos[2];
	//LINFO << "Is short: " << (int)NativeInvoke::Invoke<bool>("IS_BLIP_SHORT_RANGE", b.blip);

	blips.insert(std::make_pair(id, b));

	bsIn.Reset();
	bsIn.Write((MessageID)IVMP);
	bsIn.Write(SPAWN_BLIP);
	bsIn.Write(id);

	networkManager::connection* con = networkManager::getConnection();
	con->peer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);
}

void gameBlips::deleteBlip(RakNet::BitStream& bsIn)
{
	int id = 0;
	bsIn.Read(id);
	if(blips.find(id) == blips.end())
	{
		LINFO << "Invalid blip despawn id: " << id;
		return;
	}

	if(blips.at(id).blip.IsValid())
	{
		//NativeInvoke::Invoke<u32>("REMOVE_BLIP", blips.at(id).blip);
		Scripting::RemoveBlip(blips.at(id).blip);
	}

	blips.erase(id);

	bsIn.Reset();
	bsIn.Write((MessageID)IVMP);
	bsIn.Write(DELETE_BLIP);
	bsIn.Write(id);

	networkManager::connection* con = networkManager::getConnection();
	con->peer->Send(&bsIn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);
}

void gameBlips::wipeBlips()
{
	for(std::map<int, apiBlip>::iterator i = blips.begin(); i != blips.end(); ++i)
	{
		if(i->second.blip.IsValid())
		{
			//NativeInvoke::Invoke<u32>("REMOVE_BLIP", i->second.blip);
			Scripting::RemoveBlip(i->second.blip);
		}
	}
	blips.clear();
}