#include "pipeHandler.h"
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <string>
#include <map>
#include "../SharedDefines/truelog.h"

class pipeStruct
{
private:
	HANDLE hMapFile;
	LPCTSTR pBuf;

	TCHAR* newTCHAR(std::string& s)
	{
		TCHAR *param = new TCHAR[s.size() + 1];
		param[s.size()] = 0;
		std::copy(s.begin(), s.end(), param);
		return param;
	}

public:
	pipeStruct(const char* name)
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
			//LINFO << "Could not create file mapping object: " << name << ", " << GetLastError();
			return;
		}
		pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
			FILE_MAP_ALL_ACCESS, // read/write permission
			0,
			0,
			1000);

		if(pBuf == NULL)
		{
			//LINFO << "Could not map view of file: " << name << ", " << GetLastError();
			CloseHandle(hMapFile);
			return;
		}
		LINFO << "Pipe started";
	}
	pipeStruct(){}

	void close()
	{
		UnmapViewOfFile(pBuf);
		CloseHandle(hMapFile);
	}

	void write(std::string& s)
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

std::map<int, pipeStruct> pipes;

void pipeHandler::init(int id, const char* name)
{
	pipes[id] = pipeStruct(name);
}

void pipeHandler::write(int id, const char* s)
{
	std::string b = std::string(s);
	pipes[id].write(b);
}

const char* pipeHandler::read(int id)
{
	std::string s;
	pipes[id].read(s);
	return s.c_str();
}