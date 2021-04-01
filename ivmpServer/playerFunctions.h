#include "players.h"

void sendAllMsg(const char* text, unsigned int hex, int chatId);
void sendAllColoredMsg(int msgId, int chatId);
void sendPlayerMsg(players::player& p, const char* text, unsigned int hex, int chatId);
void streamColoredMsg(players::player& p, int msgId, int chatId);