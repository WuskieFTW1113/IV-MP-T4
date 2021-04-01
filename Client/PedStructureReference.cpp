#include "CustomFiberThread.h"
#include "players.h"
#include "easylogging++.h"

Scripting::Player GetPlayer()
{
	Scripting::Player playerIndex = Scripting::ConvertIntToPlayerIndex(Scripting::GetPlayerId());
	return playerIndex;
}

Scripting::Ped GetPlayerPed()
{
	Scripting::Ped ped;
	Scripting::GetPlayerChar(GetPlayer(), &ped);
	return ped;
}