#include <Windows.h>
#include "gameVectors.h"

struct CCam
{
	BYTE pad0[0x10]; // 000-010
	gameVectors::Matrix34 m_matMatrix; // 010-050
	BYTE pad1[0xF0]; // 050-140
};

