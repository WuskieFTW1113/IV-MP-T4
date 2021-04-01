#include "apiMath.h"
#include <vector>
#include <exporting.h>

#ifndef apiPlayer_H
#define apiPlayer_H

namespace apiPlayer
{
	class player
	{
		private:
			unsigned long long engineId;

		public:
			player(unsigned long long engineId);
			~player();

			DLL float getHeading();
			DLL void setHeading(float h);

			DLL apiMath::Vector3 getPos();
			DLL void setPos(apiMath::Vector3& pos);
			DLL int getID();

			DLL const char* getNick();
			DLL void setNick(const char* nick);

			DLL void sendMsg(const char* msg, unsigned int hex, int chatId = 0);
			DLL void sendColoredMsg(int msgId, int chatId = 0);

			DLL void setSkin(unsigned int model);
			DLL unsigned int getSkin();

			DLL void setWorld(unsigned int vWorld);
			DLL unsigned int getWorld();

			DLL void giveWeapon(unsigned int weaponId, unsigned int ammo);
			DLL void removeWeapon(unsigned int weaponId); // Use weapon ID 0 to remove all weapons

			DLL void setHealth(int hp);
			DLL int getHealth(); //Ranges from 0-200 when the player is alive, negative values if dead

			DLL void setColor(unsigned int hex); //RGB
			DLL unsigned int getColor();

			DLL void spawn(apiMath::Vector3& pos); //Must be used when the credentials arrive
			DLL void setRespawnPos(apiMath::Vector3& pos); //Must be used to set a new position only

			DLL int getDriving(); //Returns 0 if on foot
			DLL void getPassenger(int& vehicle, int& seat);
			DLL int isInVehicle(); //Checks for driving and passenger or returns 0 if on foot
			DLL void putInVehicle(int vehicleId, int seatId); //Seats: 0 driver, 1-3 passengers
			DLL void removeFromVehicle();

			//drawText time should be 0 for infinite
			DLL void drawText(int classId, float x, float y, float sx, float sy, const char* txt, int time, unsigned int color);
			DLL void updateText(int classId, const char* txt, unsigned int newColor);
			DLL void drawInfoText(const char* txt, int time);
			DLL void drawRect(int classId, float x, float y, float sx, float sy, unsigned int color);
			DLL void wipeDrawClass(int classId);

			DLL int getWeapon();
			DLL int getAmmo();

			DLL void setFrozen(bool b);

			DLL void debugPlayer(int playerid);
			DLL void debugVehicle(int vehicleid);

			DLL void disconnect();

			DLL int getPing(bool average);

			DLL int getAnim();

			DLL void showDialogList(unsigned int dialogId);

			DLL void setClothes(unsigned int part, unsigned int value); //part is 0-8
			DLL unsigned int getClothes(unsigned int part);
			DLL void setProperty(unsigned int part, unsigned int value); //part is 0-1
			DLL unsigned int getProperty(unsigned int part);

			DLL void forceAnim(const char* animGroup, const char* animName, bool loop);
			DLL void setSitIdleAnim(const char* animGroup, const char* animName, int num);
			DLL void forceSitNow();
			DLL void stopAnims();

			DLL void setKeyHook(unsigned int virtualKey, bool addOrRemove); //Virtual keys can be found online

			DLL const char* getIp();

			DLL void showBlip(int blipId, bool show);

			DLL void cam_setPos(apiMath::Vector3* pos, unsigned int camWorld);
			DLL void cam_setLookAt(apiMath::Vector3 pos);
			DLL void cam_attachOnPlayer(int playerid);

			DLL unsigned int getSerialSize(); //Returns the amount of serials
			DLL const char* getSerial(unsigned int i);

			DLL void checkSum(const char* file); //The result will arrive as a callback for registerPlayerCheckSum
			DLL void checkHacks();

			DLL bool isObjectSpawned(int id); //Returns whether the player has spawned the object
			DLL bool isVehicleSpawned(int id);

			DLL void requestWeapons(); //The result will arrive as a callback for registerPlayerWeaponsArrived
			DLL unsigned int getWeaponsSize();
			DLL void getWeaponData(unsigned int index, int& weapon, int& ammo); //Index throws exception, use getWeaponsSize for array size
			
			DLL unsigned int getRoomKey();

			DLL void playStream(const char* stream, apiMath::Vector3* pos); //Use 0 as pos for a frontend stream
			DLL void stopStreams();

			DLL void setArmor(int i);
			DLL int getArmor();

			DLL void setMoney(int i);
			DLL void setDoorStat(unsigned int model, apiMath::Vector3* pos, bool status);

			DLL int getGame();

			DLL void customChat(int chat_id, int operation);

			DLL void explodeAt(apiMath::Vector3& pos, int typ, float radius, bool audio, bool invisible, float cam);

			DLL void playSound(const char* name, int uniqueId, apiMath::Vector3* pos);
			DLL void stopSound(int uniqueId);

			DLL void ts_toggleVoice(bool b);
			DLL void ts_allowVoiceTo(int playerId, bool b);
			DLL void ts_connect(const char* ip);

			DLL void setVehicleCol(int vehicleid, bool col);
			DLL void setDrawFlags(int playerid, bool blip, bool nameDraw);
			//DLL bool getDrawFlag(int flag);
			DLL void setHudFlags(bool hud, bool radar);

			DLL void copyAim(float* f);

			DLL void setStreamDistance(float f);

			/*DLL void sprite_request(const char* txdClass, int count, const char* name);
			DLL void sprite_Load(int id, const char* txdClass, const char* txd, float posX, float posY, 
				float sizeX, float sizeY, float rot, unsigned int color);

			DLL void deleteSprite(int id);*/
	};

	DLL void sendMsgToAll(const char* msg, unsigned int hex, int chatId = 0);
	DLL void sendColoredMsgToAll(int msgId, int chatId = 0);

	DLL player& get(int id); //Throws exception
	DLL player* getPlayer(int id);
	DLL bool isOn(int id);
	DLL void getPlayers(std::vector<player*>& vec);

	DLL void setDefaultServerBlips(bool b);

	void registerResourcesSite(const char* sitePath);
	void registerResource(const char* fileName);
}

#endif