#include "simpleMath.h"
#include <string.h>
#include "gameVectors.h"

#ifndef ROTATION_MATRIX_H
#define ROTATION_MATRIX_H

class Matrix
{
public:
	simpleMath::Vector3 vecRight;
	simpleMath::Vector3 vecForward;
	simpleMath::Vector3 vecUp;
	simpleMath::Vector3 vecPosition;

	Matrix()
	{
		Identity();
	}

	Matrix(simpleMath::Vector3 _vecRight, simpleMath::Vector3 _vecFront, simpleMath::Vector3 _vecUp, simpleMath::Vector3 _vecPosition)
	{
		vecRight    = _vecRight;
		vecForward  = _vecFront;
		vecUp       = _vecUp;
		vecPosition = _vecPosition;
	}

	void Identity()
	{
		vecRight    = simpleMath::Vector3(1.0f, 0.0f, 0.0f);
		vecForward  = simpleMath::Vector3(0.0f, 1.0f, 0.0f);
		vecUp       = simpleMath::Vector3(0.0f, 0.0f, 1.0f);
		vecPosition = simpleMath::Vector3(0.0f, 0.0f, 0.0f);
	}

	Matrix operator* (const Matrix& right) const
	{
		Matrix matMatrix;
		float ** mat1 = (float **)this;
		float ** mat2 = (float **)&matMatrix;
		float ** mat3 = (float **)&right;

		for(int iRow = 0; iRow < 3; iRow++)
		{
			for(int iCol = 0; iCol < 3; iCol++)
			{
				mat2[iRow][iCol] =
					mat1[iRow][0] * mat3[0][iCol] +
					mat1[iRow][1] * mat3[1][iCol] +
					mat1[iRow][2] * mat3[2][iCol];
			}
		}

		return matMatrix;
	}
};

class Vector3_Pad
{
public:
	float x;
	float y;
	float z;
	float f4;

	Vector3_Pad()
	{
		x = y = z = f4 = 0.0f;
	}

	void ToVector3(simpleMath::Vector3 &vec3) const
	{
		vec3.x = x;
		vec3.y = y;
		vec3.z = z;
	}

	void FromVector3(const simpleMath::Vector3 &vec3)
	{
		x = vec3.x;
		y = vec3.y;
		z = vec3.z;
	}
};

// For gta structures
class Matrix34
{
public:
	Vector3_Pad vecRight;    // 00-10
	Vector3_Pad vecForward;  // 10-20
	Vector3_Pad vecUp;       // 20-30
	Vector3_Pad vecPosition; // 30-40

	Matrix34()
	{
		memset(this, 0, sizeof(Matrix34));
	}

	void ToMatrix(Matrix * matMatrix) const
	{
		memcpy(&matMatrix->vecRight, &vecRight, sizeof(simpleMath::Vector3));
		memcpy(&matMatrix->vecForward, &vecForward, sizeof(simpleMath::Vector3));
		memcpy(&matMatrix->vecUp, &vecUp, sizeof(simpleMath::Vector3));
		memcpy(&matMatrix->vecPosition, &vecPosition, sizeof(simpleMath::Vector3));
	}

	void FromMatrix(Matrix * matMatrix)
	{
		memcpy(&vecRight, &matMatrix->vecRight, sizeof(simpleMath::Vector3));
		memcpy(&vecForward, &matMatrix->vecForward, sizeof(simpleMath::Vector3));
		memcpy(&vecUp, &matMatrix->vecUp, sizeof(simpleMath::Vector3));
		memcpy(&vecPosition, &matMatrix->vecPosition, sizeof(simpleMath::Vector3));
	}
};

void ConvertRotationMatrixToEulerAngles(Matrix& matRotation, simpleMath::Vector3& vecRotation);
void ConvertEulerAnglesToRotationMatrix(simpleMath::Vector3& vecRotation, gameVectors::Matrix34& matRotation);

float ConvertRadiansToDegrees(float fRotation);
simpleMath::Vector3 ConvertRadiansToDegrees(simpleMath::Vector3& vecRotation);
void flipVectorRotation(simpleMath::Vector3& v);

float ConvertDegreesToRadians(float fRotation);
void ConvertDegreesToRadians(simpleMath::Vector3& vecRotation);

simpleMath::Vector3 GetOffsetDegrees(simpleMath::Vector3& a, simpleMath::Vector3& b);

#endif