#include "../SharedDefines/packetsIds.h"

#include "dialogList.h"
#include "guiManager.h"
#include "guiMouse.h"
#include <vector>
#include "../GWEN/Include/Gwen\Controls\WindowControl.h"
#include "../GWEN/Include/Gwen/Controls/ListBox.h"
#include "../GWEN/Include/Gwen\Controls\Button.h"
#include "../GWEN/Include/Gwen/Controls/Label.h"
#include "../SharedDefines/paramHelper.h"
#include "easylogging++.h"
#include "padCallBack.h"
#include "Scripting.h"

struct dialogListRow
{
	std::string txt;
	int customId;
};

struct dialogListStruct
{
	Gwen::Controls::WindowControl* window;
	Gwen::Controls::ListBox* list;
	Gwen::Controls::Button* ok;
	Gwen::Controls::Button* cancel;
	std::vector<Gwen::Controls::Layout::TableRow*> listRows;
	ID3DXFont* font;

	size_t columns;
	std::vector<std::string> headers;
	bool showHeaders;

	std::vector<dialogListRow> rows;

	std::vector<Gwen::Font*> gHeaders;

	int dialogServerId;

	std::string windowName;

	std::string btnsNames[2];

	float wsize;

	dialogListStruct()
	{
		window = nullptr;
		list = nullptr;
		ok = nullptr;
		cancel = nullptr;
		font = NULL;
		columns = 0;
		showHeaders = false;
		dialogServerId = -1;
		wsize = 0;
	}
};

dialogListStruct dList;

void dialogListCreateDevice(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters)
{
}

void dialogListResetDevice(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters)
{
	try
	{
		LINFO << "dialogLisResetDevice";
		LINFO << guiManager::getGwenFont();
		//D3DXCreateFont(pDevice, guiManager::getGwenFont(), 0, FW_BOLD, 1, false, DEFAULT_CHARSET, 
			//OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Microsoft Sans Serif", &dList.font);

		dList.window = new Gwen::Controls::WindowControl(guiManager::getGwenCanvas());
		LINFO << "dialogList: gotCanvas";

		float wsize = guiManager::windowX() * 0.20f + (dList.columns * guiManager::windowX() * 0.05f);
		dList.wsize = (guiManager::windowX() - wsize) * 0.5f;

		dList.window->SetTitle(dList.windowName.c_str());
		dList.window->SetSize(wsize, guiManager::windowY() * 0.35f);
		dList.window->SetPos(dList.wsize, guiManager::windowY() * 0.5f);
		dList.window->SetClosable(false);
		dList.window->SetDeleteOnClose(false);

		dList.list = new Gwen::Controls::ListBox(dList.window);
		dList.list->SetColumnCount(dList.columns);
		dList.list->SetBounds(0.0f, dList.window->GetSize().y * 0.1f, 
			dList.window->GetSize().x * 0.97 - 3, dList.window->GetSize().y * 0.60f - 1);

		size_t headers = dList.headers.size();
		float increase = dList.list->GetSize().x / dList.columns;
		long start = dList.list->GetPos().x + 1;
		for(size_t i = 0; i < headers; i++)
		{
			Gwen::Controls::Label* f = new Gwen::Controls::Label(dList.window);
			f->SetAlignment(Gwen::Pos::Bottom);
			f->SetBounds(start, 0, 1000, dList.list->GetPos().y - 2);
			f->SetText(dList.headers[i].c_str());
			start += increase;
		}

		size_t rows = dList.rows.size();
		LINFO << "Rows count: " << rows;
		std::vector<std::string> vBuf;
		for(size_t i = 0; i < rows; i++)
		{
			paramHelper::getParams(vBuf, dList.rows[i].txt, "~");
			size_t columns = vBuf.size();
			//LINFO << "Row data: " << dList.rows.at(i) << ", row columns: " << columns;
			if(columns == 0)
			{
				LINFO << "Row wont show";
				continue;
			}

			Gwen::Controls::Layout::TableRow* buf = dList.list->AddItem(vBuf.at(0).c_str());
			for(size_t k = 1; k < columns; k++)
			{
				buf->SetCellText(k, vBuf.at(k).c_str());
			}

			dList.listRows.push_back(buf);
			vBuf.clear();
		}

		dList.list->SetAllowMultiSelect(false);
		dList.list->SetKeyboardInputEnabled(true);

		dList.ok = new Gwen::Controls::Button(dList.window);
		dList.ok->SetBounds(dList.window->GetSize().x * 0.2f, dList.window->GetSize().y * 0.73f - 3,
			dList.window->GetSize().x * 0.25f, dList.window->GetSize().y * 0.1f);
		dList.ok->SetText(dList.btnsNames[0].c_str());

		dList.cancel = new Gwen::Controls::Button(dList.window);
		dList.cancel->SetBounds(dList.window->GetSize().x * 0.6f, dList.window->GetSize().y * 0.73f - 3,
			dList.window->GetSize().x * 0.25f, dList.window->GetSize().y * 0.1f);
		dList.cancel->SetText(dList.btnsNames[1].c_str());
	}
	catch(std::exception& e)
	{
		LINFO << "!Error: dialogListResetDevice: " << e.what();
	}
}

void dialogListLostDevice(IDirect3DDevice9* pDevice, bool clientControlled)
{
	if(dList.font != NULL)
	{
		//dList.font->Release();
		dList.font = NULL;
	}

	//dList.dialogServerId = -1;
	if(clientControlled)
	{
		delete dList.window;
	}
	dList.window = nullptr;
}

void dialogListDestroyDevice(IDirect3DDevice9* pDevice)
{
}

void dialogListRender(IDirect3DDevice9* pDevice)
{
	/*if(dList.showHeaders && dList.font != NULL && dList.columns > 0)
	{
		RECT r;
		r.left = dList.wsize + dList.window->GetSize().x * 0.01f + 1;
		r.bottom = guiManager::windowY() * 0.50f + dList.window->GetSize().y * 0.1f;
		r.top = r.bottom + 10;
		r.right = r.left + 40;
		
		float increase = dList.list->GetSize().x / dList.columns;
		for(size_t i = 0; i < dList.columns; i++)
		{
			if(i > 0)
			{
				r.left += increase;
				r.right= r.left + 40;
			}
			dList.font->DrawText(NULL, dList.headers[i].c_str(), -1, &r, DT_LEFT|DT_NOCLIP, 0xFF000000);
		}
	}*/
}

int dialogListGetRow()
{
	Gwen::Controls::Layout::TableRow* buf = dList.list->GetSelectedRow();
	size_t size = dList.listRows.size();
	for(size_t i = 0; i < size; i++)
	{
		if(dList.listRows[i] == buf) //just a test
		{
			return i;
		}
	}
	return -1;
}

void dialogListUpdateMsg(MSG msg)
{
	//LINFO << "Dialog MSG: " << dList.window << ", " << msg.message;
	if(dList.window == NULL)
	{
		return;
	}
	if(msg.message == WM_LBUTTONUP)
	{
		//LINFO << "OK BTN WHILE DOWN: " << dList.ok->IsDepressed();
		int mustClose = 0;
		if(dList.ok->IsDepressed())
		{
			//LINFO << "ok";
			mustClose = 1;
		}
		else if(dList.cancel->IsDepressed())
		{
			//LINFO << "cancel";
			mustClose = 2;
		}

		if(mustClose != 0)
		{
			RakNet::BitStream b;
			b.Write((MessageID)IVMP);
			b.Write(CLIENT_DIALOG_LIST_RESPONSE);
			b.Write(dList.dialogServerId);
			b.Write(mustClose); //btn id

			int rowId = dialogListGetRow();
			b.Write(rowId);
			if(rowId > -1 && rowId < dList.rows.size() && dList.rows[rowId].customId != -1)
			{
				b.Write(dList.rows[rowId].customId);
			}
			
			networkManager::sendBitStream(b);
			
			guiManager::deletePanel(guiManager::DIALOG_LIST);
			guiMouse::show(false);
			togglePlayerControls(CONTROL_GUI, true, false);
		}
	}
}

void dialogList::show()
{
	if(true)
	{
		guiManager::guiPanel* chatPanel = new guiManager::guiPanel;
		chatPanel->create = &dialogListCreateDevice;
		chatPanel->destroy = &dialogListDestroyDevice;
		chatPanel->lost = &dialogListLostDevice;
		chatPanel->render = &dialogListRender;
		chatPanel->reset = &dialogListResetDevice;
		chatPanel->msgUpdate = &dialogListUpdateMsg;

		guiManager::addPanel(guiManager::DIALOG_LIST, chatPanel);

		guiMouse::show(true);
	}
	else
	{
		guiManager::deletePanel(guiManager::DIALOG_LIST);
	}
}

void dialogList::showList(RakNet::BitStream& bsIn)
{
	LINFO << "dialogList::ShowList";
	if(dList.window != nullptr)
	{
		LINFO << "!Error: dialogList: New dialog arrived while this is still open";
		return;
	}

	dList.headers.clear();
	dList.rows.clear();

	dList.dialogServerId = -1;
	bsIn.Read(dList.dialogServerId);

	RakNet::RakString str;
	bsIn.Read(str); //window name
	dList.windowName = std::string(str);

	str.Clear();
	bsIn.Read(str); // "n" if no headers
	std::string hBuf = std::string(str);

	for(int i = 0; i < 2; i++)
	{
		str.Clear();
		bsIn.Read(str);
		dList.btnsNames[i] = str;
	}

	bsIn.Read(dList.columns);

	int rows = 0;
	bsIn.Read(rows);

	dList.listRows.clear();

	LINFO << "List info: " << hBuf << ", " << dList.columns << ", " << rows;

	if(hBuf != std::string("n"))
	{
		std::vector<std::string> vBuf;
		paramHelper::getParams(vBuf, hBuf, "~");
		size_t s = vBuf.size();
		LINFO << "Headers count: " << s;
		for(size_t i = 0; i < s; i++)
		{
			dList.headers.push_back(std::string(vBuf.at(i)));
		}
		dList.showHeaders = true;
	}
	else
	{
		dList.showHeaders = false;
	}

	for(int i = 0; i < rows; i++)
	{
		dialogListRow row;
		bsIn.Read(str);
		bsIn.Read(row.customId);
		row.txt = str;

		dList.rows.push_back(row);
		str.Clear();
	}

	show();
	togglePlayerControls(CONTROL_GUI, false, false);
}
