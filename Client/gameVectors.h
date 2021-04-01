#define PAD(name, size) unsigned char name[size]

#ifndef GAME_VECTORS_H
#define GAME_VECTORS_H

namespace gameVectors
{
	struct Vector3
	{
		float x;
		float y;
		float z;

		Vector3(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		Vector3()
		{
			this->x = 0.0f;
			this->y = 0.0f;
			this->z = 0.0f;
		}
	};

	struct Vector2
	{
		float x;
		float y;
	};

	struct Matrix34
	{
		Vector3 vecRight;		// 00-0C
		unsigned int dwPadRoll;		// 0C-10
		Vector3	vecFront;		// 10-1C
		unsigned int dwPadDirection; // 1C-20
		Vector3	vecUp;			// 20-2C
		unsigned int dwPadWas;		// 2C-30
		Vector3	vecPosition;	// 30-3C
		unsigned int dwPadPosition;	// 3C-40
	};
}

#endif