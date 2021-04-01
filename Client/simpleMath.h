#ifndef simpleMathHeader_
#define simpleMathHeader_

namespace simpleMath
{
	float lerp(float x, float xx, float progress);
	float getDistance(float x, float y, float z, float xx, float yy, float zz);

	struct Quaternion
	{
		float Y;
		float Z;
		float X;
		float W;

		//stackoverflow.com/questions/22157435/difference-between-the-two-quaternions
		Quaternion();
		void init(float tx, float ty, float tz, float tw);
		float Length();
		void Normalize();
		void Multiply(const Quaternion& q);
		void inverse();
		void conjugate();
		void Divide(float s);
		void Diff(const Quaternion& a);
	};

	float quatDot(const Quaternion &q1, const Quaternion &q2);

	struct Vector3
	{
		float x;
		float y;
		float z;

		Vector3();
		Vector3(float x, float y, float z);

        void multiply(Vector3& by);
		void divide(Vector3& by);
		void add(Vector3& by);
		void minus(Vector3& by);

	};

	void slerp(Quaternion& copyVec, Quaternion& left, Quaternion& right, float amount);
	float fSlerp(float f1, float f2, float progress);
	
	Vector3 vecLerp(Vector3 v1, Vector3 v2, float progress);

	float vecDistance(Vector3& v1, Vector3& v2);

	int Clamp(int min, int a, int max);
	float ClampF(float min, float a, float max);

	float Unlerp(int fStart, int fPos, int fEnd);
	float UnlerpClamped(int fStart, int fPos, int fEnd);

	float GetOffsetDegrees(float a, float b);

	Quaternion toQuaternion(float pitch, float roll, float yaw);
	Vector3 fromQuaternion(Quaternion& q);
}

#endif