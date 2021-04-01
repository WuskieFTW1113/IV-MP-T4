#include "CustomFiberThread.h"
#include "easylogging++.h"
#include "players.h"

void getMovement(int& x, int& y)
{
	int x2 = 0; 
	int y2 = 0;

	Scripting::GetPositionOfAnalogueSticks(0, &x, &y, &x2, &y2);
	if(x == 0 && y == 0)
	{
		Scripting::GetKeyboardMoveInput(&x, &y);
	}
}

bool isGameKeyPressed(int key)
{
	if(key < 1000)
	{
		return Scripting::IsControlPressed(0, key);
	}
	else
	{
		int x = 0;
		int y = 0;

		getMovement(x, y);

		switch(key)
		{
			case 1090: return (y < -32); //moveForward
			case 1091: return (y > 32); //back
			case 1092: return (x < -32); //left
			case 1093: return (x > 32); //right
		}
		return false;
	}
}