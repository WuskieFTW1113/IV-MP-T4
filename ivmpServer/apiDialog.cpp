#include "API/apiDialog.h"
#include "networkManager.h"
#include <map>
#include "players.h"
#include "API/apiPlayerEvents.h"
#include "dialogManager.h"
#include "../SharedDefines/packetsIds.h"

#if linux
#define _strdup strdup
#endif

struct dialogRowInternal
{
	std::string data;
	int customId;
};

struct dialogListInternal
{
	apiDialog::dialogList* list;
	RakNet::BitStream* b;
	std::vector<dialogRowInternal> rows;
};

std::map<unsigned int, dialogListInternal> dialogLists;

apiDialog::dialogList::dialogList(unsigned int id, char* windowName, unsigned int columns)
{
	this->id = id;
	this->windowName = _strdup(windowName);
	this->columns = columns;
	this->headers = "n";
	this->btnNames[0] = "Ok", this->btnNames[1] = "Cancel";
}

apiDialog::dialogList::~dialogList(){}

void apiDialog::dialogList::setColumnsHeaders(const char* s)
{
	this->headers = _strdup(s);
}

void apiDialog::dialogList::setBtnNames(const char* b1, const char* b2)
{
	this->btnNames[0] = _strdup(b1);
	this->btnNames[1] = _strdup(b2);
}

void apiDialog::dialogList::addRow(const char* s, int customId)
{
	dialogRowInternal r;
	r.customId = customId, r.data = s;
	dialogLists.at(this->id).rows.push_back(r);
}

void apiDialog::dialogList::clearRows()
{
	dialogLists.at(this->id).rows.clear();
}

void apiDialog::dialogList::save()
{
	RakNet::RakString s;
	s.Clear();
	RakNet::BitStream* b = dialogLists.at(this->id).b;
	b->Reset();

	std::vector<dialogRowInternal>& v = dialogLists.at(this->id).rows;
	size_t rows = v.size();

	b->Write((MessageID)IVMP);
	b->Write(DRAW_DIALOG_LIST);
	b->Write(this->id);
	s = std::string(this->windowName).c_str();
	b->Write(s);
	s = std::string(this->headers).c_str();
	b->Write(s);
	for(size_t i = 0; i < 2; i++)
	{
		s = std::string(btnNames[i]).c_str();
		b->Write(s);
	}
	b->Write(this->columns);
	b->Write(rows);

	for(size_t i = 0; i < rows; i++)
	{
		s = v[i].data.c_str();
		b->Write(s);
		b->Write(v[i].customId);
	}
}

bool apiDialog::addNew(unsigned int id, char* windowName, unsigned int columns)
{
	dialogListInternal di;
	di.list = new dialogList(id, windowName, columns);
	di.b = new RakNet::BitStream;
	dialogLists.insert(std::make_pair(id, di));
	return true;
}

void apiDialog::remove(int id)
{
	delete dialogLists.at(id).list;
	delete dialogLists.at(id).b;
	dialogLists.erase(id);
}

bool apiDialog::isValid(int id)
{
	return dialogLists.find(id) != dialogLists.end();
}

apiDialog::dialogList* apiDialog::get(int id)
{
	return dialogLists.at(id).list;
}

RakNet::BitStream* dialogManager::getList(unsigned int id)
{
	return dialogLists.at(id).b;
}

apiPlayerEvents::pDialogListResponse lResponse = 0;

void apiPlayerEvents::registerPlayerDialogListResponse(pDialogListResponse f)
{
	lResponse = f;
}

void dialogManager::listResponse(networkManager::connection* con, RakNet::BitStream &bsIn)
{
	if(lResponse == 0) //why bother
	{
		return;
	}

	if(!players::isPlayer(con->packet->guid.g))
	{
		return;
	}

	unsigned int dialogId;
	bsIn.Read(dialogId);

	if(dialogLists.find(dialogId) == dialogLists.end())
	{
		return;
	}

	int btn, row, custom = -1;
	bsIn.Read(btn);
	bsIn.Read(row);
	bsIn.Read(custom);

	lResponse(players::getPlayer(con->packet->guid.g).id, dialogId, btn, row, custom);
}
