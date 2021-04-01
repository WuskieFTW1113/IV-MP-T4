#include <exporting.h>

#ifndef apimath_h
#define apimath_h

namespace apiMath
{
	struct Vector3
	{	
		float x;
		float y;
		float z;

		DLL void empty(); //sets values to 0.0
		DLL Vector3();
		DLL Vector3(float x, float y, float z);
	};

	struct Quaternion
	{
		float Y;
		float Z;
		float X;
		float W;

		DLL void init(float tx, float ty, float tz, float tw);
		DLL float Length();

		DLL Quaternion(float tx, float ty, float tz, float tw);
		DLL Quaternion();
	};

	DLL float distance3d(Vector3& vecOne, Vector3& vecTwo);

	DLL void slerp(Quaternion& copyVec, Quaternion& left, Quaternion& right, float amount);
	
	DLL Vector3 vecLerp(Vector3 v1, Vector3 v2, float progress);

	DLL float lerp(float x, float xx, float progress);

	DLL float fSlerp(float f1, float f2, float progress);

	DLL float GetOffsetDegrees(float a, float b);

	DLL void secondsToMinsSecs(int seconds, int& nmins, int& nsecs);

	DLL Quaternion toQuaternion(float pitch, float roll, float yaw);
	DLL Vector3 fromQuaternion(Quaternion& q);
}

#endif