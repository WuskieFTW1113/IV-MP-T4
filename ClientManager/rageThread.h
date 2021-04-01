#include "memory.h"
#include "easylogging++.h"


sysArray<GtaThread>* GetThreadsArray()
{
	return reinterpret_cast< sysArray<GtaThread>* >(ADDRESS_THREADS_ARRAY);
}

// Получить активный поток...
scrThread* GetActiveThread()
{
	// на выходе должна быть структура
	return *reinterpret_cast< scrThread** >(ADDRESS_ACTIVE_THREAD);
}

// Сменить активный поток...
void SetActiveThread(scrThread* thread)
{
	*reinterpret_cast< scrThread** >(ADDRESS_ACTIVE_THREAD) = thread;
}

void FMPThread::AttachGtaThread(char *s_name)
{
	LINFO << "Thread::AttachGtaThread called";
	while(*(PDWORD)ADDRESS_ACTIVE_THREAD == 0) Sleep(1);
	//LINFO << "No longer sleeping";

	sysArray<GtaThread>* nowThreads = GetThreadsArray();
	//LINFO << "Got Array";

	int i;

	for(i = 0; i < nowThreads->wCount; i++)
	{
		// Проверяем не занят ли слот потока
		if(nowThreads->pData[i]->RetContext()->nThreadId == 0) break;
	}
	if(i == nowThreads->wCount)
	{
		return;
	}

	strcpy(ThreadName, s_name);

	m_pOriginalThread = nowThreads->pData[i];
	m_nThreadIndex = i;

	// Цепляем наш GtaThread в этот слот

	nowThreads->pData[i] = this;

	// получаем хеш названия GtaThread

	unsigned int hash;
	ptr call_this = ptr_cast(ADDRESS_HASH_GET);
	_asm
	{
		mov eax, s_name;
		call call_this;
		mov hash, eax;
	}

	// Ставим чистую инфу в GtaThread
	Reset(hash, NULL, 0);

	// Забираем ид потока..
	m_context.nThreadId = *(PDWORD)ADDRESS_THREAD_ID;
	*(PDWORD)ADDRESS_THREAD_ID = m_context.nThreadId + 1;

	// Говорим игре что у нас есть ещё 1 скрипт 
	*(PDWORD)ADDRESS_SCRIPTS_COUNT++;

	LINFO << "Thread::AttachGtaThread complete";
}

ThreadStates FMPThread::Reset(unsigned int hash, int v2, int i3)
{
	LINFO << "Thread::reset called";
	m_context.dwOpcodeOff = 0;
	m_context.field_10 = 0;
	m_context.nStackOff = 0;
	m_context.nTimerA = 0;
	m_context.nTimerB = 0;
	m_context.nTimerC = 0;
	m_context.field_44 = 0;
	m_context.field_48 = 0;
	m_context.field_4C = 0;
	m_context.field_50 = 0;
	m_context.eThreadState = ThreadStateIdle;
	m_context.dwScriptHash = hash;
	m_pszExitMessage = "Normal exit";
	m_bCanBePaused = true;

	LINFO << "Thread::reset complete";
	return m_context.eThreadState;
}

ThreadStates FMPThread::Run(int i1)
{
	LINFO << "Thread::run called";

	scrThread* oldThread = GetActiveThread();
	SetActiveThread(this);

	if(m_context.eThreadState != ThreadStateKilled)
	{

	}

	SetActiveThread(oldThread);
	LINFO << "Thread::run complete";
	return m_context.eThreadState;
}

ThreadStates FMPThread::Tick(unsigned int msec)
{
	ptr fn = ptr_cast(ADDRESS_THREAD_TICK);
	scrThread *thread = this;
	_asm
	{
		push msec;
		mov ecx, thread;
		call fn;
	}

	return m_context.eThreadState;
}


FMPThread::FMPThread()
{
	strcpy_s(ThreadName, "IVMP");

	memset((void*)(this + 4), 0, sizeof(FMPThread) - 4);

	m_pOriginalThread = NULL;
	m_nThreadIndex = -1;
}

FMPThread::~FMPThread()
{
	LINFO << "Thread::~IVMP called";
	if(m_pOriginalThread != NULL) Kill();
	LINFO << "Thread::~IVMP complete";
}

void FMPThread::Kill()
{
	LINFO << "Thread::Kill called";
	sysArray<GtaThread>* nowThreads = GetThreadsArray();
	nowThreads->pData[m_nThreadIndex] = m_pOriginalThread;

	m_context.eThreadState = ThreadStateKilled;
	m_context.nThreadId = 0;

	m_pOriginalThread = NULL;
	m_nThreadIndex = -1;
	LINFO << "Thread::Kill complete";
}