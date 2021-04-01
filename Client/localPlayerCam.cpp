#include "players.h"
#include "easylogging++.h"
#include "cameraMemory.h"

void cameraMemory::getTarget(float ax, float az, float &x, float &y, float &z, float range)
{
	/*
	//c = (float)(Scripting::Sqrt((float)2.8 * 2.0) * (range * 0.15) * 3);
	float c = range * 1.0647f;

	//LINFO << c << " : " << (float)(Scripting::Sqrt((float)2.8 * 2.0) * (range * 0.15) * 3);
	
	float s = c * Scripting::Cos(ax);
	float b = s * Scripting::Cos(az);
	float a = Scripting::Sqrt( (b*b) + (s*s) - ( 2 * b * s * Scripting::Cos(az) ) );
	c = Scripting::Sqrt( ((s * s) / (Scripting::Cos(ax) * Scripting::Cos(ax))) - (s*s) );

	if(ax < 0 || ax >= 180)
	{
		c = c * -1;
	}
	if(((az >= 90 && az <= 270) || az <= -90) && b > 0)
	{
		b = b * -1;
	}
	if(((az >= 0 && az <= 180) || az <= - 180) && az > 0)
	{
		a = a * -1;
	}

	x = a;
	y = b;
	z = c;*/
}

void cameraMemory::getCamTargetedCoords(float &x, float &y, float &z, float range)
{
	/*Scripting::Camera cam;
	float cx, cy, cz;
	float ax, ay, az;
	float a, b, c;

	Scripting::GetGameCam(&cam);
	Scripting::GetCamPos(cam, &cx, &cy, &cz);
	Scripting::GetCamRot(cam, &ax, &ay, &az);

	getTarget(ax, az, a, b, c, range);
	x = a + cx;
	y = b + cy;
	z = c + cz;*/
	Scripting::Camera cam;
	float cx, cy, cz;
	float ax, ay, az;
	//float a, b, c;

	Scripting::GetGameCam(&cam);
	Scripting::GetCamPos(cam, &cx, &cy, &cz);
	Scripting::GetCamRot(cam, &ax, &ay, &az);

	float tZ = az * 0.01745f;
	float tX = ax * 0.01745f;
	float absX = abs(cos(tX));

	x = cx + (-sin(tZ) * absX) * range;
	y = cy + (cos(tZ) * absX) * range;
	z = cz + (sin(tX) * range);
}

void cameraMemory::populateCamera(std::vector<simpleMath::Vector3>& vec, size_t range)
{
	Scripting::Camera cam;
	float cx, cy, cz;
	float ax, ay, az;
	//float a, b, c;

	Scripting::GetGameCam(&cam);
	Scripting::GetCamPos(cam, &cx, &cy, &cz);
	Scripting::GetCamRot(cam, &ax, &ay, &az);

	float tZ = az * 0.01745f;
	float tX = ax * 0.01745f;
	float absX = abs(cos(tX));

	for(size_t k = 1; k < range + 5; k++)
	{
		//getTarget(ax, az, a, b, c, (float)range);
		//vec.push_back(simpleMath::Vector3(a + cx, b + cy, c + cz));

		vec.push_back(simpleMath::Vector3(cx + (-sin(tZ) * absX) * k, cy + (cos(tZ) * absX) * k, cz + (sin(tX)) * k));
	}
}