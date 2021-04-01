#include "CPadData.h"
#include "padKeys.h"
#include "ControlState.h"

#define	MIN_INPUT_VALUE	0
#define	DEFAULT_BINARY_INPUT_VALUE 0
#define	DEFAULT_ANALOG_INPUT_VALUE 128
#define	MAX_INPUT_VALUE	255

#ifndef CPad_h
#define CPad_h

class IPadConfig
{
public:
	unsigned int dwCount;
	unsigned char IVPadConfig_pad0[0x7B4];
};

class IPad
{
public:
	IPadConfig m_padConfig[5];
	IPadData m_padData[INPUT_COUNT];
	IPadData m_otherPadData0;
	IPadData m_otherPadData1;
	IPadData m_otherPadData2;
	IPadData m_otherPadData3;
	unsigned char IVPad_pad0[0x4];
	bool m_bIsUsingController;
	unsigned char IVPad_pad1[0x3];
	IPadData m_otherPadData4;
	unsigned char IVPad_pad2[0xC];
	IPadConfig m_textPadConfig;
	unsigned char IVPad_pad3[0x8];
	unsigned int m_dwLastUpdateTime;
	unsigned char IVPad_pad4[0x14];
};

class CPad
{
private:
	bool m_bCreatedByUs;
	IPad* m_pPad;

public:
	CPad();
	CPad(IPad * pPad);
	~CPad();

	void SetPad(IPad * pPad) { m_pPad = pPad; }
	IPad* GetPad() { return m_pPad; }

	bool IsAnalogInput(eInput input);

	void ToControlState(CControlState& controlState, bool bCurrent);
	void FromControlState(CControlState controlState, bool bCurrent);

	void SetCurrentControlState(CControlState controlState);
	void GetCurrentControlState(CControlState& controlState);

	void SetLastControlState(CControlState controlState);
	void GetLastControlState(CControlState& controlState);
};

#endif