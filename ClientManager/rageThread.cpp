#include "memory.h"
#include "rageThread.h"
#include "easylogging++.h"
#include "CustomFiberThread.h"
#include "../clientLibs/execution.h"

FMPHook::FMPHook() : FMPThread()
{
	m_pPrimaryFiber = NULL;
	m_pScriptFiber = NULL;
	m_bKillRequested = false;
	m_nWaitTick = 0;
}

FMPHook::~FMPHook()
{
	LINFO << "Hook::~Hook called";
}

ThreadStates FMPHook::Reset(unsigned int hash, int v2, int i3)
{
	LINFO << "Hook::reset called";
	if(m_pScriptFiber)
		DeleteFiber(m_pScriptFiber);

	m_pScriptFiber = CreateFiber(0, &FiberStart, this);

	if(!m_pScriptFiber)
	{
		Kill();
		return m_context.eThreadState;
	}
	LINFO << "Hook::reset complete";
	return FMPThread::Reset(hash, v2, i3);
}

void FMPHook::FiberStart(void* parameter)
{
	LINFO << "Hook::FiberStart called";
	FMPHook* Thread = ptr_cast<FMPHook>(parameter);
	Thread->GameThread();
	Thread->m_bKillRequested = true;
	SwitchToFiber(Thread->m_pPrimaryFiber);
	LINFO << "Hook::FiberStart complete";
}

ThreadStates FMPHook::Run(int i1)
{
	if(m_pPrimaryFiber == NULL)
	{

		if(!IsBadReadPtr(GetCurrentFiber(), sizeof(ptr)))
		{
			m_pPrimaryFiber = GetCurrentFiber();
		}
		else
		{
			m_pPrimaryFiber = ConvertThreadToFiber(NULL);
		}

		m_pScriptFiber = CreateFiber(0, &FiberStart, this);

		if(!m_pScriptFiber)
		{
			Kill();
			return m_context.eThreadState;
		}
	}

	scrThread* old = GetActiveThread();
	SetActiveThread(this);

	if(m_context.eThreadState != ThreadStateKilled)
		if(GetTickCount() > m_nWaitTick) SwitchToFiber(m_pScriptFiber);

	if(m_bKillRequested) Kill();

	SetActiveThread(old);
	return m_context.eThreadState;
}

void FMPHook::wait(unsigned int timeMS)
{
	m_nWaitTick = GetTickCount() + timeMS - 1;
	SwitchToFiber(m_pPrimaryFiber);
}

#include <vector>
void FMPHook::Kill()
{
	LINFO << "Hook::Kill called";
	if(GetCurrentFiber() != m_pPrimaryFiber)
	{
		LINFO << "Hook::Kill exited";
		return;
	}

	
	if(execution::hasGameThread())
	{
		LINFO << "Disconnecting first";
		execution::callMustDisconnect();
		Sleep(1000);

		LINFO << "Unloading client";
		execution::callUnloadFunction();
		LINFO << "Done";
	}
	HWND ww = FindWindowW(NULL, L"IV:MP T4");
	if(ww != NULL) SetWindowTextA(ww, "EFLC");

	DeleteFiber(m_pScriptFiber);

	m_pPrimaryFiber = NULL;
	m_pScriptFiber = NULL;
	m_bKillRequested = false;

	FMPThread::Kill();
	LINFO << "Hook::Kill complete";
}

bool FMPHook::IsThreadAlive()
{
	return m_context.eThreadState != ThreadStateKilled;
}

void FMPHook::TerminateThisScript()
{
	LINFO << "Hook::TerminateThisScript called";
	if(GetCurrentFiber() != m_pScriptFiber)
		return;

	m_bKillRequested = true;
	SwitchToFiber(m_pPrimaryFiber);
	LINFO << "Hook::TerminateThisScript complete";
}

#include "natives.h"
#include "../clientLibs/execution.h"

void FMPHook::GameThread()
{
	LINFO << "GameThread";
	StartScript();
	while(IsThreadAlive())
	{
		//LINFO << "PULSE";
		//NativeInvoke::Invoke<0x0CA539D6, u32>("STRING", "~r~Red John", 100, true);
		//NativeInvoke::Invoke<u32>("PRINT_STRING_WITH_LITERAL_STRING_NOW", "STRING", "~r~Red John", 100, true);
		PulseScript();
		wait(execution::pulseThread());
	}
}