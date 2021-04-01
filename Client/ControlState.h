#ifndef ControlState_h
#define ControlState_h

class CControlState
{
public:

	unsigned char		ucOnFootMove[4];
	unsigned char		ucInVehicleMove[4];
	unsigned char		ucInVehicleTriggers[2];
	
	enum { KEY_COUNT = 16 };

	struct
	{
		bool bEnterExitVehicle : 1;

		bool bSprint : 1;
		bool bJump : 1;
		bool bAttack : 1;
		bool bAttack2 : 1;
		bool bAim : 1;
		bool bFreeAim : 1;
		bool bMeleeAttack1 : 1;
		bool bMeleeAttack2 : 1;
		bool bMeleeKick : 1;
		bool bMeleeBlock : 1;

		bool bHandbrake : 1;
		bool bHandbrake2 : 1;
		bool bHorn : 1;
		bool bDriveBy : 1;
		bool bHeliPrimaryFire : 1;

	} keys;

	CControlState() { };
	~CControlState() { };

	bool IsInCombat( )						const { return (keys.bMeleeAttack1 || keys.bMeleeAttack2 || keys.bMeleeKick || keys.bMeleeBlock); }
	bool IsFiring( )						const { return (keys.bAttack); }
	bool IsAiming( )						const { return (keys.bAttack2 || keys.bAim || keys.bFreeAim); }
	bool IsSprinting( )						const { return (keys.bSprint); }
	bool IsJumping( )						const { return (keys.bJump); }
	bool IsUsingEnterExitVehicle( )			const { return (keys.bEnterExitVehicle); }
	bool IsUsingHorn()						const { return (keys.bHorn); }

};

#endif