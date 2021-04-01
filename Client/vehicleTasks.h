#include <string>

namespace vehicleTasks
{
	void initTasks();

	bool isValidTask(int task);
	std::string getTaskName(int task);

	struct pedalsAction
	{
		bool handBrake;
		bool gasPedal;
		bool brakePedal;

		pedalsAction(bool handBrake, bool gasPedal, bool brakePedal);
	};

	bool isValidAction(int actionId);
	pedalsAction& getAction(int actionId);
	int findPlayerAction(bool handBrake, bool gasPedal, bool brakePedal);
}

		/*
		0: car becomes unusable for ever

		1: slows down softly

		2: slows down hardly and unusable for ever

		3: slows down softly and then drive backwards

		4: slows down hardly and turns left while slowing down

		5: slows down hardly and turns right while slowing down (4-5 uses handbreak)

		6: slows down hardly

		7: turns (left) full accel 

		8: turns (right) full accel (7-8 does a doughnut)

		9: drive forwards (with a high rpm) - not really high

		10: soft turn (right)

		11: soft turn (left)

		12: handbreak straight

		13: break and then reverse turning to the left

		14: break and then reverse turning to the right

		19: turns left

		20: turns right while breaking

		22: break and reverse

		21: turns left while breaking

		23: full throtle

		25: left turn with handbreak

		26: right turn with handbreak
		*/