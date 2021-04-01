// Windows Header Files:
#include <Windows.h>
#include <Dbghelp.h>
#include "easylogging++.h"
#include <d3d9.h>
#include <d3dx9core.h>
#include <d3dx9math.h>
#include <DxErr.h>

// C RunTime Header Files
#include <stdlib.h>
#include <memory.h>
#include <tchar.h>

#include "Gwen/Gwen.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Skins/TexturedBase.h"
#include "Gwen/UnitTest/UnitTest.h"
#include "Gwen/Input/Windows.h"
#include "Gwen/Renderers/DirectX9.h"
#include "Gwen/Controls/TextBox.h"
#include "Gwen/Controls/ListBox.h"
#include "Gwen\Controls\Button.h"
#include "Gwen/Controls/CheckBox.h"
#include "streamHandler.h"

#include "draw.h"
#include "memoryPipe.h"
#include "version.h"
#include "gameChecks.h"
#include <fstream>
#include <Wininet.h>
#include <thread>
#include "ivTarget.h"
#include "resource.h"
#include <experimental/filesystem>
std::string version = "1.1";
int v_LCC = 2;
bool isEFLC = true;

_INITIALIZE_EASYLOGGINGPP

HWND					g_pHWND = NULL;
LPDIRECT3D9				g_pD3D = NULL;
IDirect3DDevice9*		g_pD3DDevice = NULL;
D3DPRESENT_PARAMETERS	g_D3DParams;


int wx, wy;
float sx = 1, sy = 1;
//#define ICON 1000

HWND CreateGameWindow( void )
{
	WNDCLASS	wc;
	ZeroMemory( &wc, sizeof( wc ) );
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= DefWindowProc;
	wc.hInstance		= GetModuleHandle( NULL );
	wc.lpszClassName	= "IV Multiplayer";
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClass( &wc );

	std::string wName = std::string("IV Multiplayer 0.2 Version ") + version + " - For GTA " + GAMENAME;

	//sx = wx * xS, sy = wy * yS;
	HWND hWindow = CreateWindowEx( ( WS_EX_APPWINDOW | WS_EX_WINDOWEDGE ) , wc.lpszClassName, wName.c_str(), ( WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN ) & ~(/* WS_MINIMIZEBOX | */WS_MAXIMIZEBOX | WS_THICKFRAME ), -1, -1, wx + 1, wy + 1, NULL, NULL, GetModuleHandle( NULL ), NULL );
	ShowWindow( hWindow, SW_SHOW );
	SetForegroundWindow( hWindow );
	SetFocus( hWindow );
	return hWindow;
}

//
// Typical DirectX stuff to create a D3D device
//
void ResetD3DDevice()
{
	g_pD3DDevice->Reset( &g_D3DParams );
}

void CreateD3DDevice()
{
	ZeroMemory( &g_D3DParams, sizeof( g_D3DParams ) );
	RECT ClientRect;
	GetClientRect( g_pHWND, &ClientRect );
	g_D3DParams.Windowed = TRUE;
	g_D3DParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_D3DParams.BackBufferWidth = ClientRect.right;
	g_D3DParams.BackBufferHeight = ClientRect.bottom;
	g_D3DParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	g_D3DParams.BackBufferFormat = D3DFMT_X8R8G8B8;
	//g_D3DParams.EnableAutoDepthStencil = TRUE;
	//g_D3DParams.AutoDepthStencilFormat = D3DFMT_D24S8;
	g_D3DParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	HRESULT hr = g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_pHWND, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_D3DParams, &g_pD3DDevice );

	if ( FAILED( hr ) )
	{
		OutputDebugString( "Couldn't create D3D Device for some reason!" );
		return;
	}
}

RECT newRect(int x, int y, int x2, int y2)
{
	RECT n;
	n.left = x;
	n.top = y;
	n.right = x2;
	n.bottom = y2;
	return n;
}

//
// Program starts here
//

Gwen::Controls::ListBox* svrs;
Gwen::Controls::ListBox* svrinfo;
Gwen::Controls::ListBox* svrplayers;
//std::vector<Gwen::Controls::Layout::TableRow*> rows;
std::vector<Gwen::Controls::Layout::TableRow*> i_rows;
std::vector<Gwen::Controls::Layout::TableRow*> p_rows;
Gwen::Controls::Button* listHeader1Btn;
Gwen::Controls::Button* listHeader2Btn;
Gwen::Controls::Button* listHeader3Btn;
Gwen::Controls::Button* favorites;
Gwen::Controls::Button* official;
Gwen::Controls::Button* internet;
Gwen::Controls::Button* addfv;
Gwen::Controls::Button* join;
Gwen::Controls::TextBox* label;
Gwen::Controls::Label* txt;
Gwen::Controls::Label* header;
Gwen::Controls::Button* settings;

std::vector<server> serverList;
std::vector<server> bufList;
int selectedServer = -1;
Gwen::Controls::Layout::TableRow* lastRow = nullptr;

int selectedBtn = 1;
void deleteServers();

std::map<std::string, std::string> favoritess;

void serversDownloaded()
{
	deleteServers();

	/*std::sort(serverList.begin(), serverList.end(), [](const server& lhs, const server& rhs)
	{
		return favoritess.find(lhs.ip) != favoritess.end() && favoritess.at(lhs.ip) == lhs.port;
	});*/

	size_t s = bufList.size();
	LINFO << "Servers: " << s;
	int countPlayers = 0;
	for(size_t i = 0; i < s; i++)
	{
		server sb = bufList[i];
		Gwen::Controls::Layout::TableRow* buf = svrs->AddItem(sb.lock);
		buf->SetCellText(1, sb.name.c_str());
		buf->SetCellText(2, sb.players + "/" + sb.maxPlayers);
		countPlayers += atoi(sb.players.c_str());
		sb.row = buf;
		serverList.push_back(sb);
	}
	bufList.clear();
	std::string str = std::to_string(countPlayers) + " players on " + std::to_string(s) + " servers";
	//txt->SetText(str.c_str());
}

void serverArrived(server s)
{
	/*if(s.version != version)
	{
		return;
	}*/
	s.row = nullptr;
	bufList.push_back(s);
}

void deleteInfoRows()
{
	size_t size = i_rows.size();
	for(size_t i = 0; i < size; i++)
	{
		svrinfo->RemoveItem(i_rows.at(i));
	}
	i_rows.clear();
}

void deleteServers()
{
	LINFO << "Deleting servers";
	selectedServer = -1;
	lastRow = nullptr;
	LINFO << "Deleting info rows";
	deleteInfoRows();
	LINFO << "Deleted";
	size_t s = serverList.size();
	if(s > 0)
	{
		Gwen::Controls::Layout::TableRow* buf = svrs->GetSelectedRow();
		for(size_t i = 0; i < s; i++)
		{
			if(serverList[i].row != nullptr)
			{
				svrs->RemoveItem(serverList[i].row);
			}
		}
	}
	serverList.clear();
	LINFO << "Deleted";
}

void rowSelected()
{
	Gwen::Controls::Layout::TableRow* buf = svrs->GetSelectedRow();
	if(buf == NULL || buf == nullptr)
	{
		return;
	}
	size_t s = serverList.size();
	for(size_t i = 0; i < s; i++)
	{
		if(serverList[i].row == buf && lastRow != serverList[i].row)
		{
			LINFO << "New row selected";
			lastRow = serverList[i].row;
			LINFO << "Deleting server details";
			deleteInfoRows();
			LINFO << "Done";

			buf = svrinfo->AddItem("Location");
			buf->SetCellText(1, serverList[i].location.c_str());
			i_rows.push_back(buf);

			buf = svrinfo->AddItem("Site");
			buf->SetCellText(1, serverList[i].website.c_str());
			i_rows.push_back(buf);

			buf = svrinfo->AddItem("Version");
			buf->SetCellText(1, serverList[i].version.c_str());
			i_rows.push_back(buf);

			selectedServer = i;
		}
	}
}

std::string getIpv4()
{
	WSAData wsaData;
	if(WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	{
		return "";
	}

	char ac[80];
	if(gethostname(ac, sizeof(ac)) == SOCKET_ERROR)
	{
		return "";
	}

	struct hostent *phe = gethostbyname(ac);
	if(phe == 0)
	{
		return "";
	}

	std::string buf;
	for(int i = 0; phe->h_addr_list[i] != 0; ++i)
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		buf = inet_ntoa(addr);
	}
	WSACleanup();
	return buf;
}

void onButtonFeatured()
{
	LINFO << "Direct";
	selectedBtn = 2;
	deleteServers();
	if(favorites->GetToggleState()){ favorites->Toggle(); }
	if(internet->GetToggleState()){ internet->SetToggleState(false); }
	if(official->GetToggleState()){ official->SetToggleState(false); }

	std::ifstream f("comMP//config//favorites.txt");
	if(f.is_open())
	{
		std::string line;
		serverList.clear();
		bufList.clear();
		std::vector<std::string> vb;
		while(std::getline(f, line))
		{
			std::string sbuf = line.substr(0, 4);
			if(sbuf != std::string("svr="))
			{
				continue;
			}
			std::string gameType = line.substr(4, 2);
			sbuf = line.substr(7, line.size());
			vb.clear();
			getParams(vb, sbuf, ":");
			if(vb.size() != 2)
			{
				continue;
			}
			if(vb[0] == "local")
			{
				std::string buf = getIpv4();
				if(!buf.empty()) vb[0] = buf;
			}
			server sb;
			sb.players = "X";
			sb.ip = vb[0];
			sb.port = vb[1];
			sb.name = vb[0] + ":" + vb[1];
			sb.maxPlayers = "X";
			sb.lock = gameType;
			//LINFO << gameType << "+" << sb.name;
			Gwen::Controls::Layout::TableRow* buf = svrs->AddItem(sb.lock.c_str());
			buf->SetCellText(1, sb.name);
			buf->SetCellText(2, sb.players + "/" + sb.maxPlayers);
			sb.row = buf;
			serverList.push_back(sb);
		}
	}
	f.close();
}

void onButtonInternet()
{
	LINFO << "INTERNET";
	selectedBtn = 1;
	initStream();
	if(favorites->GetToggleState()){favorites->SetToggleState(false);}
	if(internet->GetToggleState()){internet->Toggle();}
	if(official->GetToggleState()){official->SetToggleState(false);}
	LINFO << "INTERNET done";
}

void client_make_minidump(EXCEPTION_POINTERS* e)
{
    auto hDbgHelp = LoadLibraryA("dbghelp");
    if(hDbgHelp == nullptr)
        return;
    auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
    if(pMiniDumpWriteDump == nullptr)
        return;

	std::ostringstream path;

    SYSTEMTIME t;
    GetSystemTime(&t);

	char cPath[MAX_PATH];
	GetModuleFileName(NULL, cPath, MAX_PATH);

	std::string bufString = cPath;

	path << bufString.substr(0, bufString.find_last_of("\\/")) << 
		t.wYear << "-" << t.wMonth << "-" << t.wDay << "-" << t.wHour << "-" << t.wMinute << "-" << t.wSecond << ".dmp";

	LINFO << path.str();

    auto hFile = CreateFileA(path.str().c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(hFile == INVALID_HANDLE_VALUE)
        return;

    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
    exceptionInfo.ThreadId = GetCurrentThreadId();
    exceptionInfo.ExceptionPointers = e;
    exceptionInfo.ClientPointers = FALSE;

    auto dumped = pMiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
        e ? &exceptionInfo : nullptr,
        nullptr,
        nullptr);

    CloseHandle(hFile);

	LINFO << "Created";
    return;
}

LONG CALLBACK client_unhandled_handler(EXCEPTION_POINTERS* e)
{
	LINFO << "Creating dump";
	client_make_minidump(e);
    return EXCEPTION_EXECUTE_HANDLER;
}

void onButtonServerName()
{
}

void onButtonPlayers()
{
}

void onButtonAddFavorite()
{
	LINFO << "Refresh";
	if(!streamActive() && selectedBtn != 0)
	{
		//setServerOptions(0);
		//deleteServers();
		initStream();
		/*if(favorites->GetToggleState()){favorites->Toggle();}
		if(internet->GetToggleState()){internet->SetToggleState(false);}
		if(official->GetToggleState()){official->SetToggleState(false);}
		selectedBtn = 0;*/
	}
}

void onButtonConnect()
{
	if(selectedServer == -1)
	{
		MessageBox(NULL, "You didn't select a server", "Connect error", MB_ICONSTOP);
		return;
	}
	else if(!filesCheck(isWindowActive()))
	{
		MessageBox(NULL, "The game launcher could not check your files integrity. Please Try:\n"
			"1-Quit the server you are playing by using /q in game"
			"\n2-Open the ivmpLauncher.exe with admin rights\n3-Reinstall IVMP", "Connect error", MB_ICONSTOP);
		return;
	}
	else if(serverList.at(selectedServer).version != version && serverList.at(selectedServer).players != std::string("X"))
	{
		MessageBox(NULL, "Your IV:MP version does NOT match the server version"
			"\nVisit: forum.iv-mp.eu for the latest updates", "Version error", MB_ICONSTOP);
		return;
	}
	#if EFLC
		//DO NOTHING
	#else
		else if(serverList[selectedServer].lock == "GT")
		{
			MessageBox(NULL, "You are currently playing GTA 4.\n You must have GTA EFLC to join this server.",
				"You may not join this server", MB_ICONSTOP);
			return;
		}
	#endif
	else if(!isWindowActive())
	{
		std::string buf;
		if(!checkEFLCVersion(buf))
		{
			std::ostringstream s;
			s << "Incorrect " << GAMENAME << " patch detected: \"" << buf << "\"\nYou must use:\n";
			#if EFLC
				s << "EFLC 1.1.2.0 or 1.1.3.0";
			#else
				s << "GTA IV 1.0.7.0 or 1.0.8.0";
			#endif

			MessageBox(NULL, s.str().c_str(), "Incorrect " GAMENAME" patch", MB_ICONSTOP);
			return;
		}
		if(!std::experimental::filesystem::exists("comMP//bin//ClientLauncher.exe"))
		{
			MessageBox(NULL, "This program was unable to find ClientLauncher.exe\n\nDid you install IV:MP at your game folder?",
				"ClientLauncher.exe not found", MB_ICONSTOP);
			return;
		}

		std::string completeEdition = "comMP//CEdition//";
		const char* completeEditionFiles[] = {"binkw32.dll", "gtaEncoder.exe"};
		for (unsigned int i = 0; i < 2; i++)
		{
			std::string full = completeEdition + completeEditionFiles[i];
			if (std::experimental::filesystem::exists(full))
			{
				CopyFile(full.c_str(), completeEditionFiles[i], FALSE);
			}
		}

		system("start comMP//bin//ClientLauncher.exe");
		
		std::string ip = serverList[selectedServer].lock + '~' + serverList.at(selectedServer).ip + '~' +
			serverList.at(selectedServer).port + '~' + serverList.at(selectedServer).name;

		memoryPipe::writePipeMsg(1, ip);

		LINFO << "Connecting to: " << serverList.at(selectedServer).name;
		return;
	}
	else if(!memoryPipe::gameRunning())
	{
		MessageBox(NULL, "The game launcher could not determine your " GAMENAME " version.\n"
			"If your game is open try:\nOpen your game window."
			"\nReturn to the launcher window in a few seconds.", "Connect error", MB_ICONSTOP);
		return;
	}
#if EFLC
	else if(isEFLC && memoryPipe::getGTA4() && serverList[selectedServer].lock != "IV")
	{
		MessageBox(NULL, "You are currently playing EFLC with GTA4 compatibility"
			"\nPlease close your game before joining a Gay Tony server", "Please close your game first", MB_ICONSTOP);
		return;
	}
	else if(isEFLC && !memoryPipe::getGTA4() && serverList[selectedServer].lock == "IV")
	{
		MessageBox(NULL, "You are currently playing EFLC without GTA4 compatibility"
			"\nPlease close your game before joining a GTAIV server", "Please close your game first", MB_ICONSTOP);
		return;
	}
#endif
	std::string ip = serverList[selectedServer].lock + '~' + serverList.at(selectedServer).ip + '~' +
		serverList.at(selectedServer).port + '~' + serverList.at(selectedServer).name;

	memoryPipe::writePipeMsg(1, ip);
	LINFO << "Connecting to: " << serverList.at(selectedServer).name;
}

void Panel(RECT r)
{
	Line(r.left, r.top, r.left, r.bottom, 1, true, D3DCOLOR_ARGB(255, 0, 0, 0));
	Line(r.left, r.bottom, r.right, r.bottom, 1, true, D3DCOLOR_ARGB(255, 0, 0, 0));
	Line(r.right, r.bottom, r.right, r.top, 1, true, D3DCOLOR_ARGB(255, 0, 0, 0));
	Line(r.right, r.top, r.left, r.top, 1, true, D3DCOLOR_ARGB(255, 0, 0, 0));
}

clock_t lastPrint = 0;
void printStatus(clock_t cTime, bool hasMsg)
{
	if(cTime > lastPrint)
	{
		lastPrint = cTime + 1000;
		//LINFO << "Last loop action id: " << (int)hasMsg;
	}
}

Gwen::Renderer::DirectX9* pRenderer;
Gwen::Skin::TexturedBase* pSkin;
Gwen::Controls::Canvas* pCanvas;
Gwen::Input::Windows* GwenInput;

Gwen::Controls::WindowControl* s_window;
Gwen::Controls::Button* s_b[2];

struct config
{
	Gwen::Controls::Label* label;
	Gwen::Controls::TextBox* input;
	Gwen::Controls::CheckBox* input2;
	char* header;
	char* configName;

	config(char* header, char* config){ this->header = header, configName = config; }
	~config(){}
};

const size_t configSize = 5;
config configs[configSize] = {
	config("Nickname(22 characters max)", "myname"),
	config("Font name", "chat_font"),
	config("Font size", "chat_size"),
	config("Font height spacing", "chat_line_height"),
	config("Windowed mode", "windowed"),
};

struct configFile
{
	char* fName;
	bool loaded;
	std::map<std::string, std::string> v;

	configFile()
	{
		fName = "comMP//config.txt";
		v["myname"] = "", v["chat_font"] = "Arial", v["chat_size"] = "24", v["chat_line_height"] = "1",
			v["chat_quality"] = "4", v["windowed"] = "0";
	}
	void open()
	{
		std::ifstream fileOpen(fName, std::ifstream::in);
		if(fileOpen.is_open())
		{
			std::string line;
			while(std::getline(fileOpen, line))
			{
				size_t find = line.find("=");
				if(find != std::string::npos)
				{
					v[line.substr(0, find)] = line.substr(find + 1, line.size());
					//LINFO << line.substr(0, find) << "," << line.substr(find + 1, line.size());
				}
			}
			loaded = true;
		}
		fileOpen.close();
	}

	void save()
	{
		std::ofstream ofs(fName, std::ios::out | std::ios::trunc); // clear contents
		if(ofs.is_open())
		{
			std::string buf;
			for(std::map<std::string, std::string>::iterator i = v.begin(); i != v.end(); ++i)
			{
				for(size_t j = 0; j < configSize; j++)
				{
					if(i->first == std::string(configs[j].configName))
					{
						if(configs[j].input)
						{
							buf = configs[j].input->GetText().c_str();
							i->second = buf.size() > 22 ? buf.substr(0, 22) : buf;
							configs[j].input->SetText(i->second.c_str());
						}
						else if(configs[j].input2)
						{
							i->second = configs[j].input2->IsChecked() ? "1" : "0";
						}
					}
				}
				ofs << i->first << "=" << i->second << std::endl;
			}
			MessageBox(NULL, "Your new settings were saved", "Success", MB_ICONINFORMATION);
		}
		ofs.close();
	}
};

configFile configtxt;

bool isSettingsOpen = false;

void openSettings()
{
	s_window = new Gwen::Controls::WindowControl(pCanvas);
	s_window->SetBounds(273, 50, 200, 280);
	s_window->SetClosable(false);
	s_window->DisableResizing();
	s_window->SetTitle("Settings");
	
	float y = 10;
	for(size_t i = 0; i < configSize; i++)
	{
		configs[i].label = new Gwen::Controls::Label(s_window);
		configs[i].label->SetBounds(20, y, 150, 15);
		configs[i].label->SetText(configs[i].header);

		if(strcmp(configs[i].configName, "windowed") != 0)
		{
			configs[i].input = new Gwen::Controls::TextBox(s_window);
			configs[i].input->SetBounds(20, y + 15, 150, 20);
			configs[i].input->SetText(configtxt.v[configs[i].configName]);
			y += 40;
		}
		else
		{
			configs[i].input2 = new Gwen::Controls::CheckBox(s_window);
			configs[i].input2->SetBounds(130, y, 15, 15);
			configs[i].input2->SetChecked(configtxt.v[configs[i].configName] == std::string("1"));
			y += 30;
		}
	}

	s_b[0] = new Gwen::Controls::Button(s_window);
	s_b[0]->SetBounds(30, y, 50, 25);
	s_b[0]->SetText("Save");

	s_b[1] = new Gwen::Controls::Button(s_window);
	s_b[1]->SetBounds(100, y, 50, 25);
	s_b[1]->SetText("Cancel");

	isSettingsOpen = true;
}

void resetDevice()
{
	pRenderer = new Gwen::Renderer::DirectX9(g_pD3DDevice);
	//
	// Create a GWEN skin
	//
	pSkin = new Gwen::Skin::TexturedBase(pRenderer, 12);
	pSkin->Init("comMP//GUI//DefaultSkin.png");
	//pSkin->GetDefaultFont()->bold = true;
	//
	// Create a Canvas (it's root, on which all other GWEN panels are created)
	//

	pCanvas = new Gwen::Controls::Canvas(pSkin);
	pCanvas->SetSize(wx, wy);
	pCanvas->SetDrawBackground(true);
	pCanvas->SetBackgroundColor(Gwen::Color(240, 240, 240, 255));

	svrs = new Gwen::Controls::ListBox(pCanvas);
	svrs->SetSize(435 * sx, (346 - 20) * sy);
	svrs->SetPos(12 * sx, (40 + 15) * sy);
	svrs->SetColumnCount(3);
	svrs->SetColumnWidth(0, 30 * sx);
	svrs->SetColumnWidth(1, 250 * sx);
	svrs->SetAllowMultiSelect(false);

	svrinfo = new Gwen::Controls::ListBox(pCanvas);
	svrinfo->SetSize(204 * sx, (271 - 26) * sy);
	svrinfo->SetPos(sx * 452, sy * (40 + 15));
	svrinfo->SetColumnCount(2);
	svrinfo->SetColumnWidth(0, svrinfo->GetSize().x * 0.5);
	svrinfo->SetColumnWidth(1, svrinfo->GetSize().x * 0.5);
	svrinfo->SetAllowMultiSelect(false);

	listHeader1Btn = new Gwen::Controls::Button(pCanvas);
	listHeader1Btn->SetPos(svrs->GetPos().x, 32 * sy + 1);
	listHeader1Btn->SetSize(30 * sx, 23 * sy);
	listHeader1Btn->SetText("EP");

	listHeader2Btn = new Gwen::Controls::Button(pCanvas);
	listHeader2Btn->SetPos(svrs->GetPos().x + listHeader1Btn->GetSize().x, listHeader1Btn->GetPos().y);
	listHeader2Btn->SetSize(246 * sx, listHeader1Btn->GetSize().y);
	listHeader2Btn->SetText("Server name");

	listHeader3Btn = new Gwen::Controls::Button(pCanvas);
	listHeader3Btn->SetPos(listHeader2Btn->GetSize().x + listHeader2Btn->GetPos().x, listHeader2Btn->GetPos().y);
	listHeader3Btn->SetSize(159 * sx, listHeader1Btn->GetSize().y);
	listHeader3Btn->SetText("Players");

	Gwen::Controls::Button* infoHeader1 = new Gwen::Controls::Button(pCanvas);
	infoHeader1->SetPos(svrinfo->GetPos().x, 32 * sy + 1);
	infoHeader1->SetSize(svrinfo->GetSize().x / 2, 23 * sy);
	infoHeader1->SetText("Detail");

	Gwen::Controls::Button* infoHeader2 = new Gwen::Controls::Button(pCanvas);
	infoHeader2->SetPos(infoHeader1->GetPos().x + infoHeader1->GetSize().x, infoHeader1->GetPos().y);
	infoHeader2->SetSize(infoHeader1->GetSize().x, infoHeader1->GetSize().y);
	infoHeader2->SetText("Value");

	addfv = new Gwen::Controls::Button(pCanvas);
	addfv->SetPos(svrinfo->GetPos().x, svrinfo->GetPos().y + svrinfo->GetSize().y);
	addfv->SetSize(svrinfo->GetSize().x, 55 * sy);
	addfv->SetText("Settings");

	join = new Gwen::Controls::Button(pCanvas);
	join->SetPos(addfv->GetPos().x, addfv->GetPos().y + addfv->GetSize().y);
	join->SetSize(addfv->GetSize().x, addfv->GetSize().y);
	join->SetText("Connect");

	internet = new Gwen::Controls::Button(pCanvas);
	internet->SetPos(12 * sx, 379 * sy);
	internet->SetSize(145 * sx, 32 * sy);
	internet->SetText("Internet");
	internet->SetIsToggle(true);

	official = new Gwen::Controls::Button(pCanvas);
	official->SetPos(internet->GetPos().x + internet->GetSize().x, internet->GetPos().y);
	official->SetSize(internet->GetSize().x, internet->GetSize().y);
	official->SetText("Direct");
	official->SetIsToggle(true);

	favorites = new Gwen::Controls::Button(pCanvas);
	favorites->SetPos(official->GetPos().x + official->GetSize().x, internet->GetPos().y);
	favorites->SetSize(internet->GetSize().x, internet->GetSize().y);
	favorites->SetText("Refresh");
	favorites->SetIsToggle(false);

	LINFO << "Re-adding rows";
	size_t size = serverList.size();
	size_t countPlayers = 0;
	for(size_t i = 0; i < size; i++)
	{
		Gwen::Controls::Layout::TableRow* buf = svrs->AddItem(serverList[i].lock);
		//LINFO << s.name;
		buf->SetCellText(1, serverList[i].name.c_str());
		buf->SetCellText(2, serverList[i].players + "/" + serverList[i].maxPlayers);
		serverList[i].row = buf;
		countPlayers += atoi(serverList[i].players.c_str());
	}

	LINFO << "Done";

	if(selectedBtn == 1){internet->Toggle();}
	else if(selectedBtn == 2){ official->Toggle(); }

	if(isSettingsOpen) openSettings();
	/*Gwen::Controls::Button* favorites = new Gwen::Controls::Button(pCanvas);
	favorites->SetPos(listHeader1.left, listHeader1.top);
	favorites->SetSize(tableW.right * 0.15f, tableW.bottom * 0.15);
	favorites->SetText("Favorites");*/
	
	//
	// Create our unittest control (which is a Window with controls in it)
	//
	
	//tableW.right -= 1;
	setDevice(g_pD3DDevice);

	//
	// Create a Windows Control helper
	// (Processes Windows MSG's and fires input at GWEN)
	//
	GwenInput = new Gwen::Input::Windows();
	GwenInput->Initialize(pCanvas);
}

void deviceLost()
{
	delete pCanvas;
	delete pSkin;
	delete pRenderer;

	i_rows.clear();
	lastRow = nullptr;
	size_t size = serverList.size();
	for(size_t i = 0; i < size; i++)
	{
		serverList[i].row = nullptr;
	}
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	//
	// Create a window and attach directx to it
	//
	//SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)client_unhandled_handler);

	if(!std::experimental::filesystem::exists("comMP//config.txt"))
	{
		MessageBox(NULL, "IV:MP was unable to access its own files\n\n"
			"Make sure ivmpGameLauncher.exe has access to the comMP folder\n\n"
			"Or try running ivmpGameLauncher as admin", "comMP folder is not accessible", MB_ICONSTOP);
		return 1;
	}
	std::ofstream ofs;
	ofs.open("comMP//serverlist.log", std::ofstream::out | std::ofstream::trunc);
	ofs.close();

	memoryPipe::init();

	versionInit();
	memoryPipe::writeStuff(); //Loads serials on pipe

	configtxt.open();

	RECT screen;
	HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &screen);

	wx = 673;
	wy = 473;
	//sx = wx / 673.0f, sy = wy / 473.0f;

	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	g_pHWND = CreateGameWindow();
	CreateD3DDevice();
	RECT FrameBounds;
	GetClientRect(g_pHWND, &FrameBounds);

	//NEW SHIT
	//startLua();
	//downloadThread = std::thread(downloadList);
	//downloadList();
	
	resetDevice();

	initStream();

	//
	// Begin the main game loop
	//
	MSG msg;
	HRESULT hr;
	bool m_bDeviceLost = false;

	/*TutorialApp app;
	app.Run();*/

	while(true)
	{
		// If we have a message from windows..
		if(!IsWindow(g_pHWND))
		{
			LINFO << "EXIT: Window handle no longer exists";
			memoryPipe::end();
			break; 
		}

		clock_t cTime = clock();
		printStatus(cTime, false);
		pulseStream();
		memoryPipe::pulse();

		if(m_bDeviceLost)
		{
			// is it ok to render again yet?
			if(FAILED(hr = g_pD3DDevice->TestCooperativeLevel()))
			{
				// the device has been lost but cannot be reset at this time
				if(hr == D3DERR_DEVICELOST)
				{
					// request repaint and exit
					InvalidateRect(hDesktop, &FrameBounds, true);
					//LINFO << "Device lost";
					Sleep(200);
					continue;
				}

				// the device has been lost and can be reset
				if(hr == D3DERR_DEVICENOTRESET)
				{
					// do lost/reset/restore cycle
					deviceLost();
					hr = g_pD3DDevice->Reset(&g_D3DParams);
					if(FAILED(hr))
					{
						// reset failed, try again later
						InvalidateRect(hDesktop, &FrameBounds, true);
						LINFO << "Device Reset";
						Sleep(200);
						continue;
					}
					resetDevice();
				}
			}

			LINFO << "Draw starts on next frame";
			// flag device status ok now
			m_bDeviceLost = false;
			Sleep(200);
			continue;
		}

		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//printStatus(cTime, true);
			// .. give it to the input handler to process
			if(msg.message == WM_LBUTTONDOWN || msg.message == WM_LBUTTONUP)
			{
				if(streamActive())
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					continue;
				}
			}
			if(msg.message == WM_LBUTTONUP)
			{
				if(official->IsHovered() && official->IsDepressed())
				{
					onButtonFeatured();
				}
				else if(favorites->IsHovered() && favorites->IsDepressed())
				{
					if(selectedBtn == 1)
					{
						onButtonInternet();
						internet->Toggle();
					}
					else if(selectedBtn == 2)
					{
						onButtonFeatured();
						official->Toggle();
					}
				}
				else if(internet->IsHovered() && internet->IsDepressed())
				{
					onButtonInternet();
				}
				else if(join->IsHovered() && join->IsDepressed())
				{
					if(configtxt.v["myname"].empty())
					{
						MessageBox(NULL, "You dont have a nickname!\nPlease open the Settings box and create one", "Configuration error", MB_ICONERROR);
					}
					else
					{
						onButtonConnect();
					}
				}
				else if(addfv->IsHovered() && addfv->IsDepressed() && !isSettingsOpen)
				{
					openSettings();
				}
				else if(isSettingsOpen)
				{
					if(s_b[0]->IsHovered() && s_b[0]->IsDepressed())
					{
						configtxt.save();
						delete s_window;
						isSettingsOpen = false;
					}
					else if(s_b[1]->IsHovered() && s_b[1]->IsDepressed())
					{
						delete s_window;
						isSettingsOpen = false;
					}
				}
			}
			GwenInput->ProcessMessage(msg);

			// if it's QUIT then quit..
			if(msg.message == WM_QUIT )
			{
				LINFO << "EXIT: Windows msg quit";
				memoryPipe::end();
				break;
			}

			// Handle the regular window stuff..
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			rowSelected();
	
			// Normal DirectX rendering loop
			g_pD3DDevice->BeginScene();
			g_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1, 0 );

			pCanvas->RenderCanvas();

			g_pD3DDevice->EndScene();
			hr = g_pD3DDevice->Present( NULL, NULL, NULL, NULL );
			m_bDeviceLost = (hr == D3DERR_DEVICELOST);
			Sleep(30);
		}
	}

	LINFO << "Draw has stopped";

	delete pCanvas;
	delete pSkin;
	delete pRenderer;

	/*if(listFont)
	{
		listFont->Release();
	}*/

	if ( g_pD3DDevice )
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	if ( g_pD3D )
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
	return 1;
}