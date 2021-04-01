#include "vehicleTasks.h"
#include <map>

std::map<int, std::string> vTasks;
std::map<int, vehicleTasks::pedalsAction> vActions;

vehicleTasks::pedalsAction::pedalsAction(bool handBrake, bool gasPedal, bool brakePedal)
{
	this->brakePedal = brakePedal;
	this->gasPedal = gasPedal;
	this->handBrake = handBrake;
}

void vehicleTasks::initTasks()
{
	vActions.insert(std::make_pair(0, vehicleTasks::pedalsAction(false, false, false))); //nothing

	vActions.insert(std::make_pair(1, vehicleTasks::pedalsAction(false, true, true))); //burn out
	vActions.insert(std::make_pair(2, vehicleTasks::pedalsAction(true, true, true))); //all

	vActions.insert(std::make_pair(3, vehicleTasks::pedalsAction(true, true, false))); //gas & handbrake
	vActions.insert(std::make_pair(4, vehicleTasks::pedalsAction(true, false, true))); //brake & handbrake

	vActions.insert(std::make_pair(5, vehicleTasks::pedalsAction(true, false, false))); //handbrake only

	vActions.insert(std::make_pair(6, vehicleTasks::pedalsAction(false, true, false))); //gas only
	vActions.insert(std::make_pair(7, vehicleTasks::pedalsAction(false, false, true))); //break only

	vTasks[23] = "Full gas, forward";
	vTasks[7] = "Full gas, left";
	vTasks[8] = "Full gas, right";

	vTasks[10] = "Left";
	vTasks[19] = "Right";

	vTasks[1] = "Break";
	vTasks[21] = "Break, right";
	vTasks[20] = "Break, left";

	vTasks[9] = "Drive forward";
}

bool vehicleTasks::isValidTask(int task)
{
	return !(vTasks.find(task) == vTasks.end());
}

std::string vehicleTasks::getTaskName(int task)
{
	return vTasks.at(task);
}

bool vehicleTasks::isValidAction(int actionId)
{
	return vActions.find(actionId) != vActions.end();
}

vehicleTasks::pedalsAction& vehicleTasks::getAction(int actionId)
{
	return vActions.at(actionId);
}

int vehicleTasks::findPlayerAction(bool handBrake, bool gasPedal, bool brakePedal)
{
	for(std::map<int, vehicleTasks::pedalsAction>::iterator it = vActions.begin(); it != vActions.end(); ++it)
	{
		if(it->second.handBrake == handBrake && it->second.gasPedal == gasPedal && it->second.brakePedal == brakePedal)
		{
			return it->first;
		}
	}
	return -1;
}