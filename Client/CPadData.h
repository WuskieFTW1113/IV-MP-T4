#include "gameVectors.h"

#ifndef CPadData_h
#define CPadData_h

class IPadDataHistoryItem
{
public:
	unsigned char m_byteValue;
	unsigned char IVPadDataHistoryItem_pad0[3];
	unsigned int m_dwLastUpdate;
};

class IPadDataHistory
{
public:
	IPadDataHistoryItem	m_historyItems[64];
};

class IPadData
{
public:
	unsigned int m_dwVFTable;
	unsigned char m_byteUnknown4;
	unsigned char m_byteContext;
	unsigned char m_byteCurrentValue;
	unsigned char m_byteLastValue;
	unsigned char m_byteHistoryIndex;
	unsigned char IVPadData_pad0[3];
	IPadDataHistory * m_pHistory;
};

#endif