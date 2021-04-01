/*#include "LuaVM.h"
#include <Wininet.h>
#include <thread>
#include <mutex>
#include "easylogging++.h"
#include "gameLibs.h"
#include "../SharedDefines/packetsIds.h"
#include "Scripting.h"

#ifdef __cplusplus
# include <lua.hpp>
#else
# include <lua.h>
# include <lualib.h>
# include <lauxlib.h>
#endif

std::string downloadSite;
std::thread downloadThread;
bool joined = false;
struct downloadInfo
{
	std::string file;
	unsigned int crc;
	unsigned int actualCrc;
	bool checked;
};

std::vector<downloadInfo> downloaded;

int LuaScripts = 0;
bool mainFound = false;
bool mainLoaded = false;

HINTERNET netstart()
{
	const HINTERNET handle = InternetOpenW(0, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
	if(handle == 0)
	{
		const DWORD error = GetLastError();
		LINFO << "InternetOpen(): " << error;
	}
	return (handle);
}

void netclose(HINTERNET object)
{
	const BOOL result = InternetCloseHandle(object);
	if(result == FALSE)
	{
		const DWORD error = GetLastError();
		LINFO << "InternetClose(): " << error;
	}
}

HINTERNET netopen(HINTERNET session, ::LPCWSTR url)
{
	const HINTERNET handle = InternetOpenUrlW(session, url, 0, 0,
		INTERNET_FLAG_NO_CACHE_WRITE, 0);
	if(handle == 0)
	{
		const DWORD error = GetLastError();
		LINFO << "InternetOpenUrl(): " << error;
	}
	return (handle);
}

void netfetch(HINTERNET istream, std::ostream& ostream)
{
	static const DWORD SIZE = 1024;
	DWORD error = ERROR_SUCCESS;
	BYTE data[SIZE];
	DWORD size = 0;
	do {
		BOOL result = InternetReadFile(istream, data, SIZE, &size);
		if(result == FALSE)
		{
			error = GetLastError();
			LINFO << "InternetReadFile(): " << error;
		}
		ostream.write((const char*)data, size);
	} while((error == ERROR_SUCCESS) && (size > 0));
}

static wchar_t* charToWChar(const char* text)
{
	const size_t size = strlen(text) + 1;
	wchar_t* wText = new wchar_t[size];
	mbstowcs(wText, text, size);
	return wText;
}

void downloadList(std::vector<downloadInfo> downloadVec)
{
	size_t s = downloadVec.size();
	LINFO << "Download list: " << s;
	for(size_t i = 0; i < s; i++)
	{
		LINFO << "Defined";
		wchar_t* URL = charToWChar(std::string(downloadSite + downloadVec[i].file).c_str());

		LINFO << "Starting session";
		const HINTERNET session = netstart();
		if(session != 0)
		{
			LINFO << "Starting stream";
			const HINTERNET istream = netopen(session, URL);
			if(istream != 0)
			{
				LINFO << "open file";
				std::ofstream ostream(std::string("comMP//Resources//" + downloadVec[i].file), std::ios::binary);
				if(ostream.is_open())
				{
					netfetch(istream, ostream);
					ostream.close();
					downloadVec[i].actualCrc = gameLibs::crc(std::string("comMP//Resources//" + downloadVec[i].file).c_str());
				}
				else {
					LINFO << "Could not open: " << downloadVec[i].file;
				}
				netclose(istream);

				//downloadMutex.lock();
				downloaded.push_back(downloadVec[i]);
				//downloadMutex.unlock();
			}
			netclose(session);
		}
	}
}

lua_State *state = nullptr;
void print_error(lua_State* state)
{
	// The error message is on top of the stack.
	// Fetch it, print it and then pop it off the stack.
	const char* message = lua_tostring(state, -1);
	LINFO << "LUA: " << message;
	lua_pop(state, 1);
}

int iv_print(lua_State* tstate)
{
	const char* message = lua_tostring(tstate, -1);
	LINFO << "LUA: " << message;
	lua_pop(state, 1);
	return 1;
}

typedef void(_cdecl *NativeCall2)(GameTypes::scrNativeCallContext *cxt);

ptr fTest(lua_State* tstate)
{
	const char* fName = lua_tostring(tstate, 1);
	LINFO << "FName: " << fName;
	return Game::GetNativeAddress(fName);
}

NativeContext parseParams(lua_State* tstate)
{
	NativeContext cxt;
	int n = lua_gettop(tstate) + 1;
	for(int i = 2; i < n; i++)
	{
		if(lua_isinteger(tstate, i))
		{
			int b = (int)lua_tointeger(tstate, i);
			LINFO << "Top: " << i << ", integer: " << b;
			cxt.Push(b);
		}
		else if(lua_isnumber(tstate, i))
		{
			float b = (float)lua_tonumber(tstate, i);
			LINFO << "Top: " << i << ", float: " << b;
			cxt.Push(b);
		}
		else if(lua_isstring(tstate, i))
		{
			const char* c = lua_tostring(tstate, i);
			LINFO << "Top: " << i << ", string: " << c;
			cxt.Push(c);
		}
	}
	return cxt;
}

int iv_u32Invoke(lua_State* tstate)
{
	ptr f = fTest(tstate);
	if(f == 0) return 1;
	
	NativeContext cxt = parseParams(tstate);
	NativeCall2 call = (NativeCall2)f;
	LINFO << "Call: " << call;
	call(&cxt);
	return 1;
}

void LuaVM::download(RakNet::BitStream& bsIn)
{
	std::vector<downloadInfo> downloadVec;

	int count = 0;
	RakNet::RakString site;

	bsIn.Read(count);
	if(count > 0 && count < 100)
	{
		LuaScripts = count;
	}
	bsIn.Read(site);

	downloadSite = site;

	LINFO << "Resources: " << count << ", " << site;
	RakNet::RakString buf;
	for(int i = 0; i < count; i++)
	{
		downloadInfo di;
		bsIn.Read(buf);
		bsIn.Read(di.crc);
		di.file = buf;
		di.actualCrc = 0;
		di.checked = false;
		buf.Clear();

		downloadVec.push_back(di);
		LINFO << "Name: " << di.file;
	}

	downloadThread = std::thread(downloadList, downloadVec);
}

void LuaVM::events(RakNet::BitStream& bsIn)
{
	RakNet::RakString s[2];
	bsIn.Read(s[0]);
	bsIn.Read(s[1]);

	lua_getglobal(state, s[0].C_String());
	if(lua_isfunction(state, -1))
	{
		lua_pushinteger(state, clock());
		lua_pushstring(state, s[1].C_String());
		lua_call(state, 2, 0);
	}
}

//long lastDownloadPulse = 0;

void LuaVM::pulse(long cTime)
{
	if(!joined && downloadThread.joinable())
	{
		downloadThread.join();
		joined = true;
	}
	else if(mainLoaded)
	{
		lua_getglobal(state, "mainPulse");
		if(lua_isfunction(state, -1))
		{
			lua_pushinteger(state, cTime);
			lua_call(state, 1, 0);
		}
	}
	else if(!mainFound)
	{
		size_t dSize = downloaded.size();
		for(size_t i = 0; i < dSize; i++)
		{
			if(downloaded[i].checked)
			{
				continue;
			}

			RakNet::BitStream b;
			RakNet::RakString s;
			s.Clear();
			s = downloaded[i].file.c_str();
			b.Write((MessageID)IVMP);
			b.Write(CLIENT_SCRIPT);
			b.Write(s);
			b.Write(downloaded[i].actualCrc);
			networkManager::sendBitStream(b);

			//if(downloaded[i].actualCrc == downloaded[i].crc)
			//{
				if(downloaded[i].file == std::string("main2.lua"))
				{
					mainFound = true;
				}
				LINFO << "Resources: " << dSize << "/" << LuaScripts;
			//}
		}
	}
	else if(mainFound && LuaScripts == downloaded.size())
	{
		state = luaL_newstate();
		luaL_openlibs(state);

		lua_register(state, "iprint", iv_print);
		lua_register(state, "u32Invoke", iv_u32Invoke);

		LINFO << "Loading script";
		int result = luaL_loadfile(state, "comMP//Resources//main2.lua");
		if(result != LUA_OK) {
			print_error(state);
			return;
		}

		// Finally, execute the program by calling into it.
		// Change the arguments if you're not running vanilla Lua code.
		result = lua_pcall(state, 0, LUA_MULTRET, 0);
		if(result != LUA_OK) {
			print_error(state);
			return;
		}
		mainLoaded = true;
		LINFO << "Loaded";
	}
}*/