/*class ITask
{
public:
	ITask *        m_pParent;
	//PAD(IVTask, pad0, 0x104); // incorrect (probably the biggest task size possible?)
	// 0xE = pPed? // incorrect

	virtual			~ITask();
	virtual ITask* Clone();
	virtual bool	IsSimple();
	virtual int		GetType();
	virtual void	Function4();
	virtual bool	MakeAbortable(DWORD pPed, int iAbortPriority,DWORD pEvent);
	virtual float	Function6(int a1);
	virtual int		Function7(int a1);
	virtual void	Function8(int a1);
	virtual int		Function9();
	virtual bool	Function10(); // *(this + 24) > 0.0;
	virtual int		Function11(); // get this+20
	virtual int		Function12();
	virtual bool	Function13(int a1, int a2);
	virtual int		Function14();
	virtual int		Function15();
	virtual int		Function16(int a2);
};

class IPedTaskManager
{
public:
	ITask * m_pPrimaryTasks[5];    // 00-14
	ITask * m_pSecondaryTasks[6]; // 14-2C
	ITask * m_pMovementTasks[3];   // 2C-38
	DWORD * m_pPed;                                 // 38-3C
	BYTE pad0[0x4];               // 3C-40
};

DWORD taskPool = gameMemory::getModuleHandle() + 0x15C353C;
LINFO << "My char: " << p.memoryAddress << " or " << p.playerIndex << " or " << p.ped.Get();
DWORD PoolAddress = *(DWORD*)taskPool;
DWORD listadr = *(DWORD*)(PoolAddress);
DWORD booladr = *(DWORD*)(PoolAddress + 4);
DWORD maxcount = *(DWORD*)(PoolAddress + 8);
DWORD itemsize = *(DWORD*)(PoolAddress + 12);

DWORD m_pPedIntelligence = *(DWORD*)(p.memoryAddress + 0x224);
LINFO << "m_pPedIntelligence: " << m_pPedIntelligence;

for(DWORD i = 0; i < maxcount; ++i)
{
	DWORD val = (DWORD)*(u8*)(booladr + i);
	if((val&0x80) == 0)
	{
		LINFO << (listadr + i * itemsize) << ", " << ((i << 8) | val);
		ITask* t = (ITask*)(listadr + i * itemsize);
		LINFO << t->GetType();
		if((listadr + i * itemsize) == p.memoryAddress)
		{
			//return (i << 8) | val;
			break;
		}
	}
}*/