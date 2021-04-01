#include "rotationMatrix.h"
#include "gameMemory.h"
#include <cmath>
#include "../clientLibs/execution.h"
#include "Offsets.h"

#define DOUBLE_PI    6.283185307179586232f
#define PI           (DOUBLE_PI * 0.5f)
#define HALF_PI      (DOUBLE_PI * 0.25f)
#define RADS_PER_DEG (DOUBLE_PI / 360.0f)
#define DEGS_PER_RAD (360.0f / DOUBLE_PI)

void ConvertRotationMatrixToEulerAngles(Matrix& matRotation, simpleMath::Vector3& vecRotation)
{
	// This function uses RADIANS not DEGREES
	// Matrix->XYZEuler
	if(matRotation.vecRight.z < 1.0f)
	{
		if(matRotation.vecRight.z > -1.0f)
		{
			vecRotation = simpleMath::Vector3(atan2(-matRotation.vecForward.z, matRotation.vecUp.z), asin(matRotation.vecRight.z), 
				atan2(-matRotation.vecRight.y, matRotation.vecRight.x));
		}
		else
		{
			// WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
			vecRotation = simpleMath::Vector3(-atan2(matRotation.vecForward.x, matRotation.vecForward.y), -HALF_PI, 0.0f);
		}
	}
	else
	{
		// WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
		vecRotation = simpleMath::Vector3(atan2(matRotation.vecForward.x, matRotation.vecForward.y), HALF_PI, 0.0f);
	}
}

void ConvertEulerAnglesToRotationMatrix(simpleMath::Vector3& vecRotation, gameVectors::Matrix34& matRotation)
{
	/*Matrix34 matGameRotation;
	matGameRotation.FromMatrix(&matRotation);*/
	gameVectors::Matrix34 * pMatGameRotation = &matRotation;
	simpleMath::Vector3* pVecGameRotation = &vecRotation;
	unsigned int dwFunc = gameMemory::getModuleHandle() + offsets::CONVERTMATRIX;
	_asm
	{
		push pVecGameRotation
		mov ecx, pMatGameRotation
		call dwFunc
	}
	//matGameRotation.ToMatrix(&matRotation);
}

float WrapAround(float fValue, float fHigh)
{
	return fValue - (fHigh * floor((float)(fValue / fHigh)));
}

float ConvertRadiansToDegrees(float fRotation)
{
	return WrapAround((fRotation * 180.0f / PI + 360.0f), 360.0f);
}

simpleMath::Vector3 ConvertRadiansToDegrees(simpleMath::Vector3& vecRotation)
{
	return simpleMath::Vector3(ConvertRadiansToDegrees(vecRotation.x), 
		ConvertRadiansToDegrees(vecRotation.y), 
		ConvertRadiansToDegrees(vecRotation.z));
}

void flipVectorRotation(simpleMath::Vector3& v)
{
	v.x = ((2 * PI) - v.x);
	v.y = ((2 * PI) - v.y);
	v.z = ((2 * PI) - v.z);
}

float ConvertDegreesToRadians(float fRotation)
{
	return WrapAround((fRotation * PI / 180.0f + 2 * PI), DOUBLE_PI);
}

void ConvertDegreesToRadians(simpleMath::Vector3& vecRotation)
{
	vecRotation.x = ConvertDegreesToRadians(vecRotation.x);
	vecRotation.y = ConvertDegreesToRadians(vecRotation.y);
	vecRotation.z = ConvertDegreesToRadians(vecRotation.z);
}

float GetOffsetDegrees(float a, float b)
{
	float c = (b > a) ? b - a : 0.0f - (a - b);

	if(c > 180.0f)
		c = 0.0f - (360.0f - c);
	else if(c <= -180.0f)
		c = (360.0f + c);

	return c;
}

simpleMath::Vector3 GetOffsetDegrees(simpleMath::Vector3& a, simpleMath::Vector3& b)
{
	return simpleMath::Vector3(GetOffsetDegrees(a.x, b.x), GetOffsetDegrees(a.y, b.y), GetOffsetDegrees(a.z, b.z));
}