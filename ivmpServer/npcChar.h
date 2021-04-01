#include <string>
#include <map>
#include <vector>
#include "inVehicleInputs.h"
#include "onFootInputs.h"

namespace npcChars
{
	struct npcChar
	{
		//apiNpcChar::npcChar* apiModuleHandle;

		unsigned int model;
		int vehicle;

		int controllingClient;

		bool isStatic;
		bool repeat;
		bool done;

		unsigned int vWorld;

		int uselessCount;

		apiMath::Vector3 currentPos;
		float currentHeading;
		int currentAnimation;

		int currentWeapon;

		apiMath::Vector3 startPos;
		apiMath::Vector3 startRot;
		apiMath::Vector3 startSpeed;
		apiMath::Vector3 startTurnSpeed;
		float lastRotationAlpha;
		apiMath::Vector3 rotError;

		std::string nick;

		std::vector<inVehicleInput> vehicleInputs;
		size_t vehicleInputsSize;

		std::vector<onFootInput> footInputs;
		size_t footInputsSize;
		//std::map<unsigned int, inVehicleInput> footInputs;

		size_t currentInput; //works for both inputs types
		
		long polEnd;
		long polBegin;

		float streamDis;

		int clothes[9];
		int props[2];
	};

	void addNpc(int id, npcChar* npc); //using pointer for superior init performance
	bool isNpc(int id);
	void removeNpc(int id);
	npcChar* getNpc(int id);

	int getEmptyNpc();

	std::map<int, npcChar*>::iterator getNpcBegin();
	std::map<int, npcChar*>::iterator getNpcEnd();
}