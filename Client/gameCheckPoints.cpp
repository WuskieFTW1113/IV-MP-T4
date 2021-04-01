#include "gameCheckPoints.h"
#include "gameWorld.h"
#include <map>
#include "simpleMath.h"
#include "../ClientManager/ivScripting.h"
#include "easylogging++.h"
#include "CustomFiberThread.h"
#include "simpleMath.h"
#include "colorHelper.h"
#include "../SharedDefines/packetsIds.h"

struct gCheckPoint
{
	simpleMath::Vector3 pos;
	int type;
	float radius;
	int colors[4];
	int blipType;
	Scripting::Blip blip;
	bool show;
	unsigned int vWorld;
	float streamDistance;
};

std::map<int, gCheckPoint> cps;
std::map<int, gCheckPoint> streamCp;
int insideCp = 0;
long lastCheckPointStream = 0;

void changeCheckPointStatus(int id, gCheckPoint& cp, bool newStatus)
{
	RakNet::BitStream bs;
	bs.Write((MessageID)IVMP);
	bs.Write(newStatus ? CHECK_POINT_CREATE : CHECK_POINT_DELETE);
	bs.Write(id);

	if(newStatus) streamCp[id] = cp;
	else streamCp.erase(id);

	networkManager::connection* con = networkManager::getConnection();
	con->peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);
}

void gameCheckPoints::spawnCp(RakNet::BitStream& bsIn)
{
	int id = -1;
	bsIn.Read(id);

	if(id < 0 || id > 1000000) //crazy id
	{
		return;
	}
	else if(cps.find(id) != cps.end())
	{
		LINFO << "!Warning: CP " << id << " spawn blocked due to duplification";
		return;
	}

	//LINFO << "Reading CP: " << id;
	gCheckPoint cp;
	bsIn.Read(cp.vWorld);
	bsIn.Read(cp.streamDistance);
	bsIn.Read(cp.type);
	bsIn.Read(cp.pos.x);
	bsIn.Read(cp.pos.y);
	bsIn.Read(cp.pos.z);
	bsIn.Read(cp.radius);
	unsigned int hexColor = 0xFFFFFFFF;
	bsIn.Read(hexColor);
	bsIn.Read(cp.blipType);
	cp.show = true;
	colorHelper::breakRgba(hexColor, cp.colors[0], cp.colors[1], cp.colors[2], cp.colors[3]);

	if(cp.blipType != 0)
	{
		Scripting::AddBlipForCoord(cp.pos.x, cp.pos.y, cp.pos.z, &cp.blip);
		//NativeInvoke::Invoke<u32>("CHANGE_BLIP_SPRITE", cp.blip, cp.blipType);
		Scripting::ChangeBlipSprite(cp.blip, cp.blipType);
	}
	cps.insert(std::make_pair(id, cp));
	//LINFO << "Done";
}

void gameCheckPoints::deleteCp(RakNet::BitStream& bsIn)
{
	int id = -1;
	bsIn.Read(id);

	if(cps.find(id) != cps.end())
	{
		if(cps.at(id).blipType != 0)
		{
			//NativeInvoke::Invoke<u32>("REMOVE_BLIP", cps.at(id).blip);
			Scripting::RemoveBlip(cps.at(id).blip);
		}

		changeCheckPointStatus(id, cps.at(id), false);
		cps.erase(id);

		if(insideCp == id)
		{
			insideCp = 0;
		}
		//LINFO << "CP " << id << " deleted";
		return;
	}
}

void gameCheckPoints::wipeCps()
{
	lastCheckPointStream = 0;
	streamCp.clear();
	for(std::map<int, gCheckPoint>::iterator i = cps.begin(); i != cps.end(); ++i)
	{
		if(i->second.blipType != 0)
		{
			//NativeInvoke::Invoke<u32>("REMOVE_BLIP", i->second.blip);
			Scripting::RemoveBlip(i->second.blip);
		}
	}
}

void gameCheckPoints::setBlipSprite(RakNet::BitStream& bsIn)
{
	int id = -1;
	bsIn.Read(id);
	if(cps.find(id) != cps.end())
	{
		int sprite = 0;
		bsIn.Read(sprite);

		gCheckPoint& g = cps.at(id);

		if(sprite == 0 && g.blipType != 0)
		{
			//NativeInvoke::Invoke<u32>("REMOVE_BLIP", g.blip);
			Scripting::RemoveBlip(g.blip);
		}
		else if(sprite > 0 && g.show)
		{
			if(g.blipType == 0)
			{
				Scripting::AddBlipForCoord(g.pos.x, g.pos.y, g.pos.z, &g.blip);
			}
			//NativeInvoke::Invoke<u32>("CHANGE_BLIP_SPRITE", g.blip, sprite);
			Scripting::ChangeBlipSprite(g.blip, sprite);
		}
		g.blipType = sprite;
	}
}

void gameCheckPoints::setShowing(RakNet::BitStream& bsIn)
{
	int id = -1;
	bsIn.Read(id);
	if(cps.find(id) != cps.end())
	{
		int show = 0;
		bsIn.Read(show);

		gCheckPoint& g = cps.at(id);

		if(show == 0 && streamCp.find(id) != streamCp.end())
		{
			changeCheckPointStatus(id, g, false);
		}

		g.show = show == 1 ? true : false;
		if(!g.show && g.blipType != 0)
		{
			//NativeInvoke::Invoke<u32>("REMOVE_BLIP", g.blip);
			Scripting::RemoveBlip(g.blip);
		}
		else if(g.show && g.blipType != 0 && !g.blip.IsValid())
		{
			Scripting::AddBlipForCoord(g.pos.x, g.pos.y, g.pos.z, &g.blip);
			//NativeInvoke::Invoke<u32>("CHANGE_BLIP_SPRITE", g.blip, g.blipType);
			Scripting::ChangeBlipSprite(g.blip, g.blipType);
		}
	}
}

void gameCheckPoints::pulse(long cTime, simpleMath::Vector3& pos)
{
	executingTask = 5;

	if(cTime > lastCheckPointStream)
	{
		lastCheckPointStream = cTime + 2000;
		std::map<int, gCheckPoint>::iterator i = cps.begin(), cpsEnd = cps.end();
		gCheckPoint* cp;
		for(i; i != cpsEnd; ++i)
		{
			if(!i->second.show || i->second.vWorld != gameWorld::getCurrentWorld())
			{
				continue;
			}

			float distance = simpleMath::vecDistance(i->second.pos, pos);
			cp = streamCp.find(i->first) != streamCp.end() ? &streamCp.at(i->first) : NULL;

			if(distance > i->second.streamDistance)
			{
				if(cp) changeCheckPointStatus(i->first, i->second, false);
				continue;
			}
			else if(!cp) changeCheckPointStatus(i->first, i->second, true);
		}
	}

	std::map<int, gCheckPoint>::iterator i = streamCp.begin(), cpsEnd = streamCp.end();
	for(i; i != cpsEnd; ++i)
	{
		subTask = 1;
		if(i->second.colors[3] != 0) {
			Scripting::DrawCheckPointWithAlpha(i->second.pos.x, i->second.pos.y, i->second.pos.z,
				i->second.radius, i->second.colors[0], i->second.colors[1], i->second.colors[2], i->second.colors[3]);
		}

		if(simpleMath::vecDistance(pos, i->second.pos) < i->second.radius * 0.5f)
		{
			if(i->first != insideCp)
			{
				RakNet::BitStream out;
				out.Write((MessageID)IVMP);
				out.Write(CHECK_POINT_ENTER);
				out.Write(i->first);

				networkManager::connection* con = networkManager::getConnection();
				con->peer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);

				insideCp = i->first;
			}
		}
		else if(insideCp == i->first)
		{
			insideCp = 0;
			RakNet::BitStream out;
			out.Write((MessageID)IVMP);
			out.Write(CHECK_POINT_EXIT);
			out.Write(i->first);

			networkManager::connection* con = networkManager::getConnection();
			con->peer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED, 0, con->serverIp, false);
		}
	}
	subTask = 0;
	executingTask = 0;
}

void gameCheckPoints::updateStreamDistance(RakNet::BitStream& bsIn)
{
	try
	{
		int id = -1;
		bsIn.Read(id);

		float dis;
		bsIn.Read(dis);
		cps.at(id).streamDistance = dis;
	}
	catch(std::exception&){}
}