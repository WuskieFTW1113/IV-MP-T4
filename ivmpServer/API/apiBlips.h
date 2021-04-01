#include "apiMath.h"
#include <exporting.h>

#ifndef APIBLIPS_H
#define APIBLIPS_H

namespace apiBlips
{
	//For the full list of blip types see this http://public.sannybuilder.com/GTA4/blips/ 
	DLL int add(apiMath::Vector3 pos, int type, unsigned int color, unsigned int world, unsigned int streamDistance, 
		bool showAll, const char* name = "");
	DLL void setName(int id, const char* name);
	DLL void remove(int id);
	DLL bool isValid(int id);
}

#endif