#include <Windows.h>

#ifndef padData_H
#define padData_H

struct KeyBlock
{
	BYTE m_byteUnknown0;
	BYTE m_byteUnknown1;
	BYTE m_byteUnknown2;
	BYTE m_byteUnknown3;
	BYTE m_byteUnknown4;
	BYTE m_byteUnknown5;
	// Current State
	BYTE m_byteUnknown6;
	// Previous State
	BYTE m_byteUnknown7;
	BYTE m_byteUnknown8;
	BYTE m_byteUnknown9;
	BYTE m_byteUnknown10;
	BYTE m_byteUnknown11;
	BYTE m_byteUnknown12;
	BYTE m_byteUnknown13;
	BYTE m_byteUnknown14;
	BYTE m_byteUnknown15;
};

struct Pad
{
	BYTE pad0[0x2698];
	KeyBlock keyBlocks[191];
};

struct PadState
{
	BYTE byteCurrentKeys[191];
	BYTE bytePreviousKeys[191];

	PadState()
	{
		memset(this, 0, sizeof(PadState));

		for(int x = 12; x < 20; x++)
		{
			byteCurrentKeys[x] = 128;
			bytePreviousKeys[x] = 128;
		}

		for(int x = 24; x < 28; x++)
		{
			byteCurrentKeys[x] = 128;
			bytePreviousKeys[x] = 128;
		}

		for(int x = 30; x < 38; x++)
		{
			byteCurrentKeys[x] = 128;
			bytePreviousKeys[x] = 128;
		}

		for(int x = 72; x < 76; x++)
		{
			byteCurrentKeys[x] = 128;
			bytePreviousKeys[x] = 128;
		}

		byteCurrentKeys[88] = 128;
		bytePreviousKeys[88] = 128;

		byteCurrentKeys[89] = 128;
		bytePreviousKeys[89] = 128;

		byteCurrentKeys[92] = 128;
		bytePreviousKeys[92] = 128;

		byteCurrentKeys[95] = 128;
		bytePreviousKeys[95] = 128;

		byteCurrentKeys[96] = 128;
		bytePreviousKeys[96] = 128;

		byteCurrentKeys[139] = 128;
		bytePreviousKeys[139] = 128;

		byteCurrentKeys[140] = 128;
		bytePreviousKeys[140] = 128;

		byteCurrentKeys[185] = 128;
		bytePreviousKeys[185] = 128;

		byteCurrentKeys[186] = 128;
		bytePreviousKeys[186] = 128;
	}
};

#endif