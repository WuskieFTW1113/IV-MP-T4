#ifndef CUSTOMFIBERTHREAD_H
#define CUSTOMFIBERTHREAD_H

#include "players.h"
#include "vehicles.h"

extern int executingTask;
extern int subTask;

bool isGameKeyPressed(int key);
void getMovement(int& x, int& y);

int myThreadInit();
int myThreadPulse();
int myThreadStop();
int myThreadDisconnect();

void inVehicleSync(players::netPlayer& p);
size_t searchForAnim(int action);

bool spawnRemotePlayer(int id, players::netPlayer& p);
void managePlayers();

void pulseVehicles(float x, float y, float z);
bool SpawnCar(vehicles::netVehicle& v);

Scripting::Player GetPlayer();
Scripting::Ped GetPlayerPed();

void initPlayerStatus();
void refreshMyPlayerStatus(players::netPlayer& p, long cTime);


#endif