#include "memoryPipe.h"
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "easylogging++.h"
//#include "..\ArusHook\Scripting.h"
#include "../clientLibs/execution.h"
#include "CustomFiberThread.h"
#include <time.h>
#pragma comment(lib, "user32.lib")

bool t_runAsGTA4 = false;
long lastRead = -1;

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
	pipeStruct() {}

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

void memoryPipe::end()
{
	for(int i = 0; i < 2; i++) pipes[i].close();
}

void getParams(std::vector<std::string>& words, std::string& s, const char* splitAt)
{
	char * pch = NULL;
	char *next = NULL;

	char * writable = new char[s.size() + 1];
	std::copy(s.begin(), s.end(), writable);
	writable[s.size()] = '\0';

	pch = strtok_s(writable, splitAt, &next);
	while(pch != NULL)
	{
		//printf("%s\n", pch);
		words.push_back(pch);
		pch = strtok_s(NULL, splitAt, &next);
	}

	//delete[] pch;
	delete[] writable;
}

void memoryPipe::pulse()
{
	long cTime = clock();
	if(cTime > lastRead)
	{
		std::string s;
		if(lastRead == -1)
		{
			pipes[2].read(s);
			execution::setVersion(s.c_str());
			pipes[2].close();
		}
		lastRead = cTime + 500;
        
		pipes[1].read(s);
		if(!s.empty())
		{
			pipes[1].write("");
			std::vector<std::string> vb;
			getParams(vb, s, "~");
			if(vb.size() != 4)
			{
				LINFO << "Wrong ADDR: " << s;
				return;
			}

			t_runAsGTA4 = vb[0] == "IV";
			pipes[0].write(std::string(t_runAsGTA4 ? "IV" : "EFLC"));
			pipes->read(s);
			LINFO << "Game version set: " << s;
			
			execution::setServerIp(vb[1].c_str(), atoi(vb[2].c_str()), vb[3].c_str());
			LINFO << "INFO: " << vb[0] << ", " << vb[1] << ", " << vb[2] << ", " << vb[3];

			if(!execution::hasGameThread())
			{
				mustLaunch = true;
			}
		}
	}
}

bool memoryPipe::runAsGTA4()
{
	return t_runAsGTA4;
}