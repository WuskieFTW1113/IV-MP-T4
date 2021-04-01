#include "simpleMath.h"
#include <cmath>

float simpleMath::lerp(float x, float xx, float progress)
{
	return (1-progress)*x + progress*xx;
}

float simpleMath::getDistance(float x, float y, float z, float xx, float yy, float zz)
{
	float newx = (xx - x);
	float newy = (yy - y);
	float newz = (zz - z);
	return sqrt(newx * newx + newy * newy + newz * newz);
}

simpleMath::Quaternion::Quaternion()
{
	X = 0.0;
	Y = 0.0;
	Z = 0.0;
	W = 0.0;
}

void simpleMath::Quaternion::init(float tx, float ty, float tz, float tw)
{
	X = tx;
	Y = ty;
	Z = tz;
	W = tw;
}

float simpleMath::Quaternion::Length()
{
	return static_cast<float>( std::sqrt( (X * X) + (Y * Y) + (Z * Z) + (W * W) ) );
}

void simpleMath::Quaternion::Normalize()
{
	float length = 1.0f / Length();
	X *= length;
	Y *= length;
	Z *= length;
	W *= length;
}

void simpleMath::Quaternion::Multiply(const Quaternion& q)
{
	this->X = this->W*q.X + this->Z*q.W + this->Y*q.Z - this->Z*q.Y;
    this->Y = this->W*q.Y + this->Y*q.W + this->Z*q.X - this->X*q.Z;
    this->Z = this->W*q.Z + this->Z*q.W + this->X*q.Y - this->Y*q.X;
	this->W = this->W*q.W - this->X*q.X - this->Y*q.Y - this->Z*q.Z;
}

float simpleMath::quatDot(const Quaternion &q1, const Quaternion &q2)
{
	return q1.X*q2.X + q1.Y*q2.Y + q1.Z*q2.Z + q1.W*q2.W;
}

void simpleMath::Quaternion::Divide(float s)
{
	X /= s;
	Y /= s;
	Z /= s;
	W /= s;
}

void simpleMath::Quaternion::inverse()
{
    conjugate();
	this->Divide(quatDot((*this), (*this)));	
}

void simpleMath::Quaternion::conjugate()
{
    X *= -1;
    Y *= -1;
    Z *= -1;
}

void simpleMath::Quaternion::Diff(const Quaternion& a)
{
	this->inverse();
	this->Multiply(a);
}

simpleMath::Vector3::Vector3()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

simpleMath::Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void simpleMath::Vector3::multiply(Vector3& by)
{
	this->x *= by.x;
	this->y *= by.y;
	this->z *= by.z;
}
void simpleMath::Vector3::divide(Vector3& by)
{
	this->x /= by.x;
	this->y /= by.y;
	this->z /= by.z;
}
void simpleMath::Vector3::add(Vector3& by)
{
	this->x += by.x;
	this->y += by.y;
	this->z += by.z;
}
void simpleMath::Vector3::minus(Vector3& by)
{
	this->x -= by.x;
	this->y -= by.y;
	this->z -= by.z;
}

void simpleMath::slerp(Quaternion& copyVec, Quaternion& left, Quaternion& right, float amount)
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

float simpleMath::fSlerp(float f1, float f2, float progress)
{
	float inverse = 1.0f - progress;
	float returnF;
	
	returnF = (f1 * f2) >= 0.0f ? (inverse * f1) + (progress * f2) : (inverse * f2) + (progress * f1);
	return returnF * (1.0f / std::sqrt((returnF * returnF)));
}

simpleMath::Vector3 simpleMath::vecLerp(simpleMath::Vector3 v1, simpleMath::Vector3 v2, float progress)
{
	return simpleMath::Vector3(simpleMath::lerp(v1.x, v2.x, progress), simpleMath::lerp(v1.y, v2.y, progress), simpleMath::lerp(v1.z, v2.z, progress));
}

float simpleMath::vecDistance(simpleMath::Vector3& v1, simpleMath::Vector3& v2)
{
	float newx = (v1.x - v2.x);
	float newy = (v1.y - v2.y);
	float newz = (v1.z - v2.z);
	return sqrt(newx * newx + newy * newy + newz * newz);
}

int simpleMath::Clamp(int min, int a, int max)
{
	if(a < min)
			return min;

	if(a > max)
			return max;

	return a;
}

float simpleMath::ClampF(float min, float a, float max)
{
	if(a < min)
			return min;

	if(a > max)
			return max;

	return a;
}

float simpleMath::Unlerp(int fStart, int fPos, int fEnd)
{
	if(fStart == fEnd) return 1.0f;

	return (float)(fPos - fStart) / (fEnd - fStart);
}

float simpleMath::UnlerpClamped(int fStart, int fPos, int fEnd)
{
	return ClampF(0.0f, Unlerp(fStart, fPos, fEnd), 1.0f);
}

float simpleMath::GetOffsetDegrees(float a, float b)
{
    float c = ( b > a ) ? b - a : 0.0f - ( a - b );
    if ( c > 180.0f )
        c = 0.0f - ( 360.0f - c );
    else if ( c <= -180.0f )
        c = ( 360.0f + c );
    return c;
}

simpleMath::Quaternion simpleMath::toQuaternion(float pitch, float roll, float yaw)
{
	simpleMath::Quaternion  q;
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

simpleMath::Vector3 simpleMath::fromQuaternion(Quaternion& q)
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