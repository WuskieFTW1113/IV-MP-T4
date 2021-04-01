#include <map>
#include <RakNetTypes.h>
#include "apiPlayer.h"
#include <BitStream.h>
#include <string>
#ifndef PLAYERS_H
#define PLAYERS_H

struct streamEntity
{
	bool hasSpawned;
	long lastPacket;
	int health;

	streamEntity();
};

struct playerWeapon
{
	int weapon;
	int ammo;
};

namespace players
{
	struct player
	{
		player();
		~player();

		int id;
		RakNet::SystemAddress ip;

		float streaming;

		RakNet::BitStream* bs;

		bool spawnReady;
		//apiPlayer::player* moduleHandle;

		unsigned int vWorld;
		unsigned int interior;

		apiMath::Vector3 position;
		apiMath::Vector3 velocity;
		apiMath::Vector3 aim;

		apiMath::Vector3 camPos;
		unsigned int camWorld;
		uint64_t camAttached;
		bool camActive;

		float heading;

		unsigned int skin;
		unsigned int color;
		int hp;
		int armor;

		int movementAnim;
		bool animRepeating;
	
		long lastSentPacket;
		long packetCount;

		int currentWeapon;
		int currentAmmo;
	
		std::string nick;

		size_t currentVehicle;
		size_t passangerOf;
		int passangerSeat;

		long timeOutTime;
		bool timeOutWarned;

		long lastSelfPing;

		bool isDucking;

		//streamed data, long is when spawned by the client
		std::map<int, streamEntity> sPlayers;
		std::map<int, streamEntity> sNpcs;
		std::map<int, streamEntity> sVehicles;

		long lastStaticStream;
		std::map<int, streamEntity> sCheckPoints;
		std::map<int, streamEntity> sObjects;
		std::map<int, streamEntity> sBlips;

		void wentAfk();

		int clothes[9];
		int props[2];

		int c_Game;

		std::vector<std::string> serials;
		std::vector<playerWeapon> playerWeapons;
	};

	bool addPlayer(uint64_t id, RakNet::SystemAddress ip);
	bool isPlayer(uint64_t id);
	uint64_t isPlayerIdOnline(int id);

	void removePlayer(uint64_t id);
	player& getPlayer(uint64_t id);

	//int getEmptyPlayerId();

	std::map<uint64_t, player>::iterator getPlayersBegin();
	std::map<uint64_t, player>::iterator getPlayersEnd();

	void updateWorldForPlayer(player& p);
	void updateTimeOut(player& p, long newTimeOut);

	int playerCount();

	void sendPlayerDataToStreamedInPlayer(int& playerid, unsigned int& world, RakNet::BitStream& b);
}

//void reserveIdForNpc(int id);
//bool isIdReservedForNpc(int id);

#endif