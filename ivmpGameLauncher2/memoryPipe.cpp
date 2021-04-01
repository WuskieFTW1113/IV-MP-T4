#include "memoryPipe.h"
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <time.h>
#include "easylogging++.h"
#include "version.h"
#include "gameChecks.h"

TCHAR* newTCHAR(std::string& s)
{
	TCHAR *param = new TCHAR[s.size() + 1];
	param[s.size()] = 0;
	std::copy(s.begin(), s.end(), param);
	return param;
}

void startPipe(HANDLE& hMapFile, LPCTSTR& pBuf, TCHAR* name, TCHAR* def)
{
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		1000,                // maximum object size (low-order DWORD)
		name);                 // name of mapping object

	if(hMapFile == NULL)
	{
		LINFO << "Could not create file mapping object: " << name << ", " << GetLastError();
		return;
	}
	pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		1000);

	if(pBuf == NULL)
	{
		LINFO << "Could not map view of file: " << name << ", " << GetLastError();
		CloseHandle(hMapFile);
		return;
	}

	//CopyMemory((PVOID)pBuf, def, (_tcslen(def) * sizeof(TCHAR)));
}

struct pipeStruct
{
	HANDLE hMapFile;
	LPCTSTR pBuf;
	TCHAR* name;
	TCHAR* defaultTxt;

	pipeStruct(std::string n, std::string def)
	{
		this->name = newTCHAR(n);
		this->defaultTxt = newTCHAR(def);

		startPipe(this->hMapFile, this->pBuf, this->name, this->defaultTxt);
	}
	pipeStruct(){}

	void close()
	{
		UnmapViewOfFile(pBuf);
		CloseHandle(hMapFile);
	}

	void write(std::string s)
	{
		TCHAR *param = newTCHAR(s);

		ZeroMemory((PVOID)pBuf, (_tcslen(pBuf) * sizeof(pBuf)));
		CopyMemory((PVOID)pBuf, param, (_tcslen(param) * sizeof(TCHAR)));

		delete param;
	}

	void read(std::string& s)
	{
		s = std::string(this->pBuf);
	}
};

pipeStruct pipes[3] = {
	pipeStruct("game", ""),
	pipeStruct("ip", ""),
	pipeStruct("ve", "")
};


bool t_isGTA4 = false;

bool memoryPipe::getGTA4()
{
	return t_isGTA4;
}

void memoryPipe::init()
{

}

void memoryPipe::end()
{
	for(int i = 0; i < 3; i++) pipes[i].close();
}

bool is_gameRunning = false;
long lastGamePulse = 0;

void memoryPipe::writePipeMsg(int id, std::string s)
{
	pipes[id].write(s);
}

bool memoryPipe::gameRunning()
{
	return is_gameRunning;
}

void memoryPipe::writeStuff()
{
	std::vector<std::string> v;
	getVersion(v);

	std::string str = "";
	size_t s = v.size();
	for(size_t i = 0; i < s; i++)
	{
		str += v.at(i);
		if(i != s - 1)
		{
			str += std::string(" ");
		}
	}
	pipes[2].write(str.c_str());
	//LINFO << str;
}

clock_t firstNoted = 0;
void memoryPipe::pulse()
{
	std::string msg; 

	pipes[0].read(msg);
	//LINFO << msg;
	if(isWindowActive())
	{
		//LINFO << "Window found: " << msg << ", " << (int)is_gameRunning << ", " << (int)msg.empty();
		if(!is_gameRunning && !msg.empty())
		{
			is_gameRunning = true;
			if(msg == "IV") t_isGTA4 = true;
			else t_isGTA4 = false;
			LINFO << "Window found 2";
		}
	}
	else if(is_gameRunning && !msg.empty()) {
		if(firstNoted == 0) {
			firstNoted = clock();
			return;
		}
		else if(clock() > firstNoted + 3000)
		{
			pipes[0].write("");
			is_gameRunning = false;
			firstNoted = 0;
			LINFO << "Game Window Lost";
		}
	}
}