#include "ivGUI.h"
#include "Scripting.h"
#include <vector>
#include <map>
#include "colorHelper.h"
#include <time.h>
#include "CustomFiberThread.h"

struct textDraw
{
	float size[2];
	float pos[2];
	int color[4];
	std::string text;
	int time;
};

struct recDraw
{
	float pos[2];
	float size[2];
	int color[4];
};

std::map<int, textDraw> texts;
std::map<int, recDraw> rects;
std::vector<textDraw> timedTexts;

struct ivTxd
{
	std::string cacheName;
	u32 txd;

	ivTxd(){}
	ivTxd(std::string& s, int t) { cacheName = s; txd = t; }
};

/*std::map<std::string, u32> txdCache;
std::map<int, ivTxd> txds;

void popTexture(std::string& cache)
{
	if(txdBank.find(bank) == txdBank.end())
	{
		LINFO << "Unknown txd bank: " << bank;
		return;
	}

	ivTxdBank& v = txdBank.at(bank);
	std::map<int, std::string>::iterator i = v.txds.begin();
	std::map<int, std::string>::iterator e = v.txds.end();

	while(i != e)
	{
		if(name == NULL || name->compare(i->second) == 0)
		{
			Scripting::ReleaseTexture(i->first);
			v.txds.erase(i++);
			continue;
		}
		++i;
	}

	if(name == NULL || v.txds.size() == 0)
	{
		Scripting::RemoveTxd(v.txdId);
		txdBank.erase(bank);
	}
}

void ivGUI::drawSprite(RakNet::BitStream& bsIn)
{
	int op = 0, id = 0;
	bsIn.Read(op);
	bsIn.Read(id);
	LINFO << "TXD: " << id;
	if(op == 1)
	{
		if(txds.find(id) == txds.end())
		{
			LINFO << "Unknown txd class: " << id;
		}

		popTexture(txds[id].cacheName);
		return;
	}
	else if(op == 2)
	{
		RakNet::RakString s;
		std::string names[2];
		for(int i = 0; i < 2; i++)
		{
			bsIn.Read(s);
			names[i] = s;
		}

		std::string cache = names[0] + '?' + names[1];
		if(txdCache.find(cache) == txdCache.end())
		{
			u32 txdId = Scripting::GetTxd(names[0].c_str());
			txdCache[cache] = txdId;		
		}

		txds[id] = ivTxd(cache, Scripting::GetTexture(txdCache[cache], names[1].c_str()));
	}
	else if(op == 3)
	{

	}
}*/

void ivGUI::pulse(long cTime)
{
	executingTask = 6;
	subTask = 1;
	for(std::map<int, textDraw>::iterator i = texts.begin(); i != texts.end(); ++i)
	{
		Scripting::SetTextScale(i->second.size[0], i->second.size[1]);
		Scripting::SetTextDropshadow(false, 0, 0, 0, 0);
		Scripting::SetTextColour(i->second.color[0], i->second.color[1], i->second.color[2], i->second.color[3]);
		Scripting::DisplayTextWithLiteralString(i->second.pos[0], i->second.pos[1], "STRING", i->second.text.c_str());		
	}

	subTask = 2;
	for(std::map<int, recDraw>::iterator i = rects.begin(); i != rects.end(); ++i)
	{
		Scripting::DrawRect(i->second.pos[0], i->second.pos[1],	i->second.size[0], i->second.size[1],
			i->second.color[0], i->second.color[1], i->second.color[2], i->second.color[3]);
	}

	size_t s = timedTexts.size();
	size_t i = 0;
	subTask = 3;
	while(i < s)
	{
		textDraw& t = timedTexts.at(i);
		if(t.time != 0 && cTime > t.time)
		{
			timedTexts.erase(timedTexts.begin() + i);
			i--;
			continue;
		}
		Scripting::SetTextScale(t.size[0], t.size[1]);
		Scripting::SetTextDropshadow(false, 0, 0, 0, 0);
		Scripting::SetTextColour(t.color[0], t.color[1], t.color[2], t.color[3]);
		Scripting::DisplayTextWithLiteralString(t.pos[0], t.pos[1], "STRING", t.text.c_str());
		i++;
	}
	subTask = 0;
	executingTask = 0;
}

void ivGUI::drawText(RakNet::BitStream& bsIn)
{
	textDraw t;
	int classId = -1;
	bsIn.Read(classId);
	bsIn.Read(t.pos[0]);
	bsIn.Read(t.pos[1]);
	bsIn.Read(t.size[0]);
	bsIn.Read(t.size[1]);
	RakNet::RakString msg;
	bsIn.Read(msg);
	t.text = msg;
	bsIn.Read(t.time);
	t.time += clock();
	unsigned int hex = 0xFFFFFFFF;
	bsIn.Read(hex);
	colorHelper::breakRgba(hex, t.color[0], t.color[1], t.color[2], t.color[3]);
	classId == -1 ? timedTexts.push_back(t) : texts.insert(std::make_pair(classId, t));
}

void ivGUI::updateText(RakNet::BitStream& bsIn)
{
	int id = -1;
	bsIn.Read(id);
	if(texts.find(id) != texts.end())
	{
		RakNet::RakString msg;
		bsIn.Read(msg);
		if(msg.GetLength() != 0)
		{
			texts.at(id).text = msg;
		}
		unsigned int bColor = 0;
		bsIn.Read(bColor);
		if(bColor != 0)
		{
			textDraw& t = texts.at(id);
			colorHelper::breakRgba(bColor, t.color[0], t.color[1], t.color[2], t.color[3]);
		}
	}
}

void ivGUI::drawInfoText(RakNet::BitStream& bsIn)
{
	RakNet::RakString msg;
	int time = 0;
	bsIn.Read(msg);
	bsIn.Read(time);
	Scripting::PrintStringWithLiteralStringNow("STRING", msg, time, true);
}

void ivGUI::drawRect(RakNet::BitStream& bsIn)
{
	recDraw t;
	int classId = -1;
	bsIn.Read(classId);
	bsIn.Read(t.pos[0]);
	bsIn.Read(t.pos[1]);
	bsIn.Read(t.size[0]);
	bsIn.Read(t.size[1]);

	unsigned int hex = 0xFFFFFFFF;
	bsIn.Read(hex);
	colorHelper::breakRgba(hex, t.color[0], t.color[1], t.color[2], t.color[3]);

	rects.insert(std::make_pair(classId, t));
}

void ivGUI::wipeDrawClass(RakNet::BitStream& bsIn)
{
	int id = 0;
	bsIn.Read(id);

	if(id == -1)
	{
		texts.clear();
		rects.clear();
		return;
	}

	std::map<int, textDraw>::iterator i = texts.begin();
	while(i != texts.end())
	{
		if(i->first == id)
		{
			texts.erase(i++);
			continue;
		}
		++i;
	}

	std::map<int, recDraw>::iterator it = rects.begin();
	while(it != rects.end())
	{
		if(it->first == id)
		{
			rects.erase(it++);
			continue;
		}
		++it;
	}
}