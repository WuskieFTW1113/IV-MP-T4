#include "apiPlayer.h"

namespace apiPlayerManager
{
	bool add(int id, apiPlayer::player& p);
	void remove(int id);

	int getFreeId();
}