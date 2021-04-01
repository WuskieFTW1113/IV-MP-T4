#include "CPad.h"
#include "gameMemory.h"
#include "padControl.h"
#include "easylogging++.h"

#define SET_ANALOG_KEY(key, analog) \
	analog = (bCurrent ? m_pPad->m_padData[key].m_byteCurrentValue : m_pPad->m_padData[key].m_byteLastValue)

#define SET_BINARY_KEY(key, binary) \
	binary = (bCurrent ? ((m_pPad->m_padData[key].m_byteCurrentValue == MAX_INPUT_VALUE) ? true : false) : ((m_pPad->m_padData[key].m_byteLastValue == MAX_INPUT_VALUE) ? true : false))

// Macros for the FromControlState function
#define GET_ANALOG_KEY(key, analog) \
	if(bCurrent) { m_pPad->m_padData[key].m_byteCurrentValue = analog; } else { m_pPad->m_padData[key].m_byteLastValue = analog; }

#define GET_BINARY_KEY(key, binary) \
	if(bCurrent) { m_pPad->m_padData[key].m_byteCurrentValue = (binary ? MAX_INPUT_VALUE : DEFAULT_BINARY_INPUT_VALUE); } else { m_pPad->m_padData[key].m_byteLastValue = (binary ? MAX_INPUT_VALUE : DEFAULT_BINARY_INPUT_VALUE); }

CPad::CPad()
{
	m_bCreatedByUs = true;
	// Allocate the new pad
	m_pPad = new IPad;

	((IPad*(__thiscall *) (IPad *))(gameMemory::getModuleHandle() + 0x865DA0))(m_pPad); //Pad Contructor

	((void(__thiscall *) (IPad *, bool))(gameMemory::getModuleHandle() + 0x86B1D0))(m_pPad, false); //Pad init

	// Initialize values
	for (int i = 0; i < INPUT_COUNT; i++)
	{

		// Set the correct xor value
		// Should be -1 for INPUT_MOVE_LEFT | INPUT_MOVE_UP | INPUT_VEH_MOVE_LEFT | INPUT_VEH_MOVE_UP
		// and 0 for the rest
		m_pPad->m_padData[i].m_byteUnknown4 = getLocalPad()->GetPad()->m_padData[i].m_byteUnknown4;

		// This defaults all analog input values to 128 (middle) instead of 0
		if (IsAnalogInput((eInput) i))
		{
			m_pPad->m_padData[i].m_byteCurrentValue = DEFAULT_ANALOG_INPUT_VALUE;
			m_pPad->m_padData[i].m_byteLastValue = DEFAULT_ANALOG_INPUT_VALUE;
		}
		else
		{
			m_pPad->m_padData[i].m_byteCurrentValue = 0;
			m_pPad->m_padData[i].m_byteLastValue = 0;
		}
	}

	LINFO << "Remote pad created (" << m_pPad << ")";
}

CPad::CPad(IPad * pPad)
{
	// Mark as not created by us
	m_bCreatedByUs = false;

	// Set the pad
	SetPad(pPad);

	LINFO << "Local pad created (" << m_pPad << ")";
}

CPad::~CPad()
{
	// Was the pad created by us?
	if(m_bCreatedByUs)
	{
		LINFO << "Destroying pad (" << m_pPad << ")";

		((int(__thiscall *) (IPad *))(gameMemory::getModuleHandle() + 0x7FD6E0))(m_pPad); //destructor

		// Delete the pad
		delete m_pPad;
		LINFO << "Remote pad destoyed (" << m_pPad << ")";
	}
}

bool CPad::IsAnalogInput(eInput input)
{
	switch(input)
	{
	case INPUT_MOVE_LEFT:
	case INPUT_MOVE_RIGHT:
	case INPUT_MOVE_UP:
	case INPUT_MOVE_DOWN:
	case INPUT_LOOK_LEFT:
	case INPUT_LOOK_RIGHT:
	case INPUT_LOOK_UP:
	case INPUT_LOOK_DOWN:
	case INPUT_SNIPER_ZOOM_IN:
	case INPUT_SNIPER_ZOOM_OUT:
	case INPUT_SNIPER_ZOOM_IN_ALTERNATE:
	case INPUT_SNIPER_ZOOM_OUT_ALTERNATE:
	case INPUT_VEH_MOVE_LEFT:
	case INPUT_VEH_MOVE_RIGHT:
	case INPUT_VEH_MOVE_UP:
	case INPUT_VEH_MOVE_DOWN:
	case INPUT_MOUSE_UD:
	case INPUT_MOUSE_LR:
	case INPUT_MOVE_KEY_STUNTJUMP:
	case INPUT_FRONTEND_AXIS_UD:
	case INPUT_FRONTEND_AXIS_LR:
	case INPUT_FE_MOUSE_UD:
	case INPUT_FE_MOUSE_LR:
	case INPUT_VEH_MOVE_LEFT_2:
	case INPUT_VEH_MOVE_RIGHT_2:
		return true;
	}

	return false;
}

void CPad::ToControlState(CControlState& controlState, bool bCurrent)
{
	// Do we not have a valid pad?
	if(!m_pPad)
		return;

	// Analog keys
	SET_ANALOG_KEY(INPUT_MOVE_LEFT,         controlState.ucOnFootMove[0]);
	SET_ANALOG_KEY(INPUT_MOVE_RIGHT,        controlState.ucOnFootMove[1]);
	SET_ANALOG_KEY(INPUT_MOVE_UP,           controlState.ucOnFootMove[2]);
	SET_ANALOG_KEY(INPUT_MOVE_DOWN,         controlState.ucOnFootMove[3]);
	SET_ANALOG_KEY(INPUT_VEH_MOVE_LEFT,     controlState.ucInVehicleMove[0]);
	SET_ANALOG_KEY(INPUT_VEH_MOVE_RIGHT,    controlState.ucInVehicleMove[1]);
	SET_ANALOG_KEY(INPUT_VEH_MOVE_UP,       controlState.ucInVehicleMove[2]);
	SET_ANALOG_KEY(INPUT_VEH_MOVE_DOWN,     controlState.ucInVehicleMove[3]);
	SET_ANALOG_KEY(INPUT_VEH_BRAKE,         controlState.ucInVehicleTriggers[0]);
	SET_ANALOG_KEY(INPUT_VEH_ACCELERATE,    controlState.ucInVehicleTriggers[1]);

	// Binary keys
	SET_BINARY_KEY(INPUT_ENTER,             controlState.keys.bEnterExitVehicle);
	SET_BINARY_KEY(INPUT_SPRINT,            controlState.keys.bSprint);
	SET_BINARY_KEY(INPUT_JUMP,              controlState.keys.bJump);
	SET_BINARY_KEY(INPUT_ATTACK,            controlState.keys.bAttack);
	SET_BINARY_KEY(INPUT_ATTACK2,           controlState.keys.bAttack2);
	SET_BINARY_KEY(INPUT_AIM,               controlState.keys.bAim);
	SET_BINARY_KEY(INPUT_FREE_AIM,          controlState.keys.bFreeAim);
	SET_BINARY_KEY(INPUT_MELEE_ATTACK1,     controlState.keys.bMeleeAttack1);
	SET_BINARY_KEY(INPUT_MELEE_ATTACK2,     controlState.keys.bMeleeAttack2);
	SET_BINARY_KEY(INPUT_MELEE_KICK,        controlState.keys.bMeleeKick);
	SET_BINARY_KEY(INPUT_MELEE_BLOCK,       controlState.keys.bMeleeBlock);
	SET_BINARY_KEY(INPUT_VEH_HANDBRAKE,     controlState.keys.bHandbrake);
	SET_BINARY_KEY(INPUT_VEH_HANDBRAKE_ALT, controlState.keys.bHandbrake2);
	SET_BINARY_KEY(INPUT_VEH_HORN,          controlState.keys.bHorn);
	SET_BINARY_KEY(INPUT_VEH_ATTACK,        controlState.keys.bDriveBy);
	SET_BINARY_KEY(INPUT_VEH_ATTACK2,       controlState.keys.bHeliPrimaryFire);
}

void CPad::FromControlState(CControlState controlState, bool bCurrent)
{
	// Do we not have a valid pad?
	if(!m_pPad)
		return;

	// Analog keys
	GET_ANALOG_KEY(INPUT_MOVE_LEFT,         controlState.ucOnFootMove[0]);
	GET_ANALOG_KEY(INPUT_MOVE_RIGHT,        controlState.ucOnFootMove[1]);
	GET_ANALOG_KEY(INPUT_MOVE_UP,           controlState.ucOnFootMove[2]);
	GET_ANALOG_KEY(INPUT_MOVE_DOWN,         controlState.ucOnFootMove[3]);
	GET_ANALOG_KEY(INPUT_VEH_MOVE_LEFT,     controlState.ucInVehicleMove[0]);
	GET_ANALOG_KEY(INPUT_VEH_MOVE_RIGHT,    controlState.ucInVehicleMove[1]);
	GET_ANALOG_KEY(INPUT_VEH_MOVE_UP,       controlState.ucInVehicleMove[2]);
	GET_ANALOG_KEY(INPUT_VEH_MOVE_DOWN,     controlState.ucInVehicleMove[3]);
	GET_ANALOG_KEY(INPUT_VEH_BRAKE,         controlState.ucInVehicleTriggers[0]);
	GET_ANALOG_KEY(INPUT_VEH_ACCELERATE,    controlState.ucInVehicleTriggers[1]);

	// Binary keys
	GET_BINARY_KEY(INPUT_ENTER,             controlState.keys.bEnterExitVehicle);
	GET_BINARY_KEY(INPUT_SPRINT,            controlState.keys.bSprint);
	GET_BINARY_KEY(INPUT_JUMP,              controlState.keys.bJump);
	GET_BINARY_KEY(INPUT_ATTACK,            controlState.keys.bAttack);
	GET_BINARY_KEY(INPUT_ATTACK2,           controlState.keys.bAttack2);
	GET_BINARY_KEY(INPUT_AIM,               controlState.keys.bAim);
	GET_BINARY_KEY(INPUT_FREE_AIM,          controlState.keys.bFreeAim);
	GET_BINARY_KEY(INPUT_MELEE_ATTACK1,     controlState.keys.bMeleeAttack1);
	GET_BINARY_KEY(INPUT_MELEE_ATTACK2,     controlState.keys.bMeleeAttack2);
	GET_BINARY_KEY(INPUT_MELEE_KICK,        controlState.keys.bMeleeKick);
	GET_BINARY_KEY(INPUT_MELEE_BLOCK,       controlState.keys.bMeleeBlock);
	GET_BINARY_KEY(INPUT_VEH_HANDBRAKE,     controlState.keys.bHandbrake);
	GET_BINARY_KEY(INPUT_VEH_HANDBRAKE_ALT, controlState.keys.bHandbrake2);
	GET_BINARY_KEY(INPUT_VEH_HORN,          controlState.keys.bHorn);
	GET_BINARY_KEY(INPUT_VEH_ATTACK,        controlState.keys.bDriveBy);
	GET_BINARY_KEY(INPUT_VEH_ATTACK2,       controlState.keys.bHeliPrimaryFire);
}

void CPad::SetCurrentControlState(CControlState controlState)
{
	if(m_pPad)
		FromControlState(controlState, true);
}

void CPad::GetCurrentControlState(CControlState& controlState)
{
	if(m_pPad)
		ToControlState(controlState, true);
}

void CPad::SetLastControlState(CControlState controlState)
{
	if(m_pPad)
		FromControlState(controlState, false);
}

void CPad::GetLastControlState(CControlState& controlState)
{
	if(m_pPad)
		ToControlState(controlState, false);
}