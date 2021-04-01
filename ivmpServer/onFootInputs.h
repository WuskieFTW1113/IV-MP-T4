#include "apiMath.h"

struct onFootInput
{
	//apiMath::Vector3 startPos;
	apiMath::Vector3 endPos;
	apiMath::Vector3 endAim;
	//apiMath::Quaternion startRot;
	float endRot;

	int animation;

	int weapon;
	int bullets;

	int inputTime;
	long inputEnd;
};