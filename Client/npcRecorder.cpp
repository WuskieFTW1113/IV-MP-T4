/*#include "npcRecorder.h"
#include <fstream>
#include <time.h>
#include "players.h"
#include "vehicles.h"
#include "easylogging++.h"

std::ofstream* botFile = nullptr;
clock_t lastBotClock = 0;
bool record_inVehicle = false;

void npcRecorder::startRecording()
{
	botFile = new std::ofstream("comMP//bot.bin", std::ios::out | std::ios::app | std::ios::binary);
	if(!botFile->is_open())
	{
		LINFO << "!Error: bot file didnt open";
		botFile = nullptr;
		return;
	}

	record_inVehicle = players::getLocalPlayer().inVehicle != 0;
}

void npcRecorder::stopRecording()
{
	botFile->clear();
	botFile = nullptr;
}

bool npcRecorder::isRecording()
{
	return botFile != nullptr;
}

void npcRecorder::pulseRecord(long cTime)
{
	if(isRecording())
	{
		players::netPlayer& player = players::getLocalPlayer();
		if(record_inVehicle && player.inVehicle == 0)
		{
			LINFO << "In vehicle recording stopped: not in a vehicle";
			stopRecording();
			return;
		}
		else if(!record_inVehicle && player.inVehicle != 0)
		{
			LINFO << "On foot recording stopped: in a vehicle";
			stopRecording();
			return;
		}

		std::ostringstream s;
		s.precision(5);
		s << std::fixed;

		if(record_inVehicle)
		{
			vehicles::netVehicle& v = vehicles::getVehicle(player.inVehicle);

			s << v.position.x << "<" << v.position.y << "<" << v.position.z << "<" << v.rotation.x << "<" <<
				v.rotation.y << "<" << v.rotation.z << "<0.0" << "<" << cTime - lastBotClock << 
				"<" << v.steer << "<" << v.breakOrGas << "<" << v.rpm << "<" << v.speed.x << "<" << v.speed.y << "<" << v.speed.z <<
				"<" << v.turnSpeed.x << "<" << v.turnSpeed.y << "<" << v.turnSpeed.z;

		}
		else
		{
			s << player.pos.x << "<" << player.pos.y << "<" << player.pos.z << "<" << player.heading << "<" <<
				player.currentAnim << "<" << cTime - lastBotClock << "<" << player.currentWeapon;

			if(player.currentAnim == 1 || player.currentAnim == 2)
			{
				s << "<" << player.aim.x << "<" << player.aim.y << "<" << player.aim.z << "<" << player.currentAmmo;
			}
		}

		lastBotClock = cTime;
		*botFile << s.str() << std::endl;
	}

	lastBotClock = cTime;
}*/