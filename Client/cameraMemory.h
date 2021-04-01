#include "gameVectors.h"
#include "simpleMath.h"
#include <vector>

namespace cameraMemory
{
	struct CCam
	{
		unsigned char pad0[0x10]; // 000-010
		gameVectors::Matrix34 m_matMatrix; // 010-050
		unsigned char pad1[0xF0]; // 050-140
	};

	CCam* getGameCam();

	void setGameCamMatrix(gameVectors::Matrix34* matrix);
	void getGameCameraMatrix(gameVectors::Matrix34* matrix);
	void GetScreenPositionFromWorldPosition(gameVectors::Vector3& v3, gameVectors::Vector2& v2);

	void getCamTargetedCoords(float &x, float &y, float &z, float range);
	void getTarget(float ax, float az, float &x, float &y, float &z, float range);
	void populateCamera(std::vector<simpleMath::Vector3>& vec, size_t range);
}