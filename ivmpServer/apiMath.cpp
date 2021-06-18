#include "API/apiMath.h"
#include <cmath>

void apiMath::Vector3::empty()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

apiMath::Vector3::Vector3()
{
	empty();
}

apiMath::Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

float apiMath::distance3d(Vector3& vecOne, Vector3& vecTwo)
{
	float newx = (vecOne.x - vecTwo.x);
	float newy = (vecOne.y - vecTwo.y);
	float newz = (vecOne.z - vecTwo.z);
	return sqrt(newx * newx + newy * newy + newz * newz);
}

void apiMath::Quaternion::init(float tx, float ty, float tz, float tw)
{
	X = tx;
	Y = ty;
	Z = tz;
	W = tw;
}

float apiMath::Quaternion::Length()
{
	return static_cast<float>(std::sqrt( (X * X) + (Y * Y) + (Z * Z) + (W * W) ) );
}

apiMath::Quaternion::Quaternion(float tx, float ty, float tz, float tw)
{
	X = tx;
	Y = ty;
	Z = tz;
	W = tw;
}

apiMath::Quaternion::Quaternion()
{
	X = 0.0f;
	Y = 0.0f;
	Z = 0.0f;
	W = 0.0f;
}

void apiMath::slerp(Quaternion& copyVec, Quaternion& left, Quaternion& right, float amount)
{
	float inverse = 1.0f - amount;
	float dot = (left.X * right.X) + (left.Y * right.Y) + (left.Z * right.Z) + (left.W * right.W); 

	if( dot >= 0.0f )
	{
		copyVec.X = (inverse * left.X) + (amount * right.X);
		copyVec.Y = (inverse * left.Y) + (amount * right.Y);
		copyVec.Z = (inverse * left.Z) + (amount * right.Z);
		copyVec.W = (inverse * left.W) + (amount * right.W);
	}
	else
	{
		copyVec.X = (inverse * left.X) - (amount * right.X);
		copyVec.Y = (inverse * left.Y) - (amount * right.Y);
		copyVec.Z = (inverse * left.Z) - (amount * right.Z);
		copyVec.W = (inverse * left.W) - (amount * right.W);
	}

	float invLength = 1.0f / copyVec.Length();

	copyVec.X *= invLength;
	copyVec.Y *= invLength;
	copyVec.Z *= invLength;
	copyVec.W *= invLength;
}

float apiMath::lerp(float x, float xx, float progress)
{
	return (1-progress)*x + progress*xx;
}

apiMath::Vector3 apiMath::vecLerp(apiMath::Vector3 v1, apiMath::Vector3 v2, float progress)
{
	return Vector3(lerp(v1.x, v2.x, progress), lerp(v1.y, v2.y, progress), lerp(v1.z, v2.z, progress));
}

float apiMath::fSlerp(float f1, float f2, float progress)
{
	float inverse = 1.0f - progress;
	float returnF;
	
	returnF = (f1 * f2) >= 0.0f ? (inverse * f1) + (progress * f2) : (inverse * f2) + (progress * f1);
	return returnF * (1.0f / std::sqrt((returnF * returnF)));
}

float apiMath::GetOffsetDegrees(float a, float b)
{
	float c = (b > a) ? b - a : 0.0f - (a - b);

	if(c > 180.0f)
		c = 0.0f - (360.0f - c);
	else if(c <= -180.0f)
		c = (360.0f + c);

	return c;
}

void apiMath::secondsToMinsSecs(int seconds, int& nmins, int& nsecs)
{
	nmins = (int)std::floor(seconds * 0.0166);
	nsecs = seconds - nmins * 60;
	if(nsecs == 60)
	{
		nsecs = 0;
		nmins++;
	}
}

apiMath::Quaternion apiMath::toQuaternion(float pitch, float roll, float yaw)
{
	apiMath::Quaternion  q;
        // Abbreviations for the various angular functions

	pitch = (pitch * 3.14159265358979323846f) / 180.0f;
	roll = (roll * 3.14159265358979323846f) / 180.0f;
	yaw = (yaw * 3.14159265358979323846f) / 180.0f;

	float cy = cosf(yaw * 0.5f);
	float sy = sinf(yaw * 0.5f);
	float cr = cosf(roll * 0.5f);
	float sr = sinf(roll * 0.5f);
	float cp = cosf(pitch * 0.5f);
	float sp = sinf(pitch * 0.5f);

	q.W = cy * cr * cp + sy * sr * sp;
	q.X = cy * sr * cp - sy * cr * sp;
	q.Y = cy * cr * sp + sy * sr * cp;
	q.Z = sy * cr * cp - cy * sr * sp;
	return q;
}

apiMath::Vector3 apiMath::fromQuaternion(Quaternion& q)
{
	double euler[3];
	const static double PI_OVER_2 = 3.14159265358979323846 * 0.5;
	const static double EPSILON = 1e-10;
	double sqw, sqx, sqy, sqz;

	// quick conversion to Euler angles to give tilt to user
	sqw = q.W * q.W;
	sqx = q.X * q.X;
	sqy = q.Y * q.Y;
	sqz = q.Z * q.Z;

	euler[1] = asin(2.0 * (q.W * q.Y - q.X * q.Z));
	if(PI_OVER_2 - fabs(euler[1]) > EPSILON) {
		euler[2] = atan2(2.0 * (q.X * q.Y + q.W * q.Z),
			sqx - sqy - sqz + sqw);
		euler[0] = atan2(2.0 * (q.W * q.X + q.Y * q.Z),
			sqw - sqx - sqy + sqz);
	}
	else {
		// compute heading from local 'down' vector
		euler[2] = atan2(2 * q.Y * q.Z - 2 * q.X * q.W,
			2 * q.X * q.Z + 2 * q.Y * q.W);
		euler[0] = 0.0;

		// If facing down, reverse yaw
		if(euler[1] < 0)
			euler[2] = 3.14159265358979323846 - euler[2];
	}
	const static float mp = 180.0f / 3.141592653589793238463f;
	return Vector3((float)euler[0] * mp, (float)euler[1] * mp, (float)euler[2] * mp);
}
