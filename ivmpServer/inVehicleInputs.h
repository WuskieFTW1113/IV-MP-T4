#include "API/apiMath.h"

struct inVehicleInput
{
	//apiMath::Vector3 startPos;
	apiMath::Vector3 endPos;

	//apiMath::Quaternion startRot;
	apiMath::Vector3 endRot;

	//apiMath::Vector3 startSpeed;
	apiMath::Vector3 endSpeed;

	apiMath::Vector3 endTurnSpeed;

	//int animation;

	int inputTime;
	long inputEnd;

	float steer;
	//float gasPedal;

	int breakOrGas;

	//int gear;
	float rpm;
};
