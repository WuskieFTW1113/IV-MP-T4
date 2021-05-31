#include "../RakNet2/MessageIdentifiers.h"

typedef unsigned char MessageID;

enum GameMessages
{
	SPAWN_POSITION=ID_USER_PACKET_ENUM+1,
	POSITION_UPDATE=ID_USER_PACKET_ENUM+2,
	YOUR_TURN=ID_USER_PACKET_ENUM+3,
	FOOT_SYNC_TO_SERVER=ID_USER_PACKET_ENUM+4,
	WEAPON_CHANGE=ID_USER_PACKET_ENUM+5,
	REQUEST_PLAYER_DATA=ID_USER_PACKET_ENUM+6,
	RECEIVE_PLAYER_DATA=ID_USER_PACKET_ENUM+7,
	SEND_NICK_TO_SERVER=ID_USER_PACKET_ENUM+8,
	SPAWN_PLAYER=ID_USER_PACKET_ENUM+9,
	SERVER_RECEIVE_VEHICLE_SYNC=ID_USER_PACKET_ENUM+10,
	CLIENT_RECEIVE_VEHICLE_DATA=ID_USER_PACKET_ENUM+11,
	CLIENT_REQUEST_VEHICLE_DATA=ID_USER_PACKET_ENUM+12,
	CLIENT_RECEIVE_VEHICLE_SYNC=ID_USER_PACKET_ENUM+13,
	SERVER_RECEIVE_PASSANGER_SYNC=ID_USER_PACKET_ENUM+14,
	CLIENT_RECEIVE_PASSANGER_SYNC=ID_USER_PACKET_ENUM+15,
	BILATERAL_MESSAGE=ID_USER_PACKET_ENUM+16,
	PLAYER_LEFT_TO_CLIENT=ID_USER_PACKET_ENUM+17,
	CLIENT_SPAWNED_PLAYER=ID_USER_PACKET_ENUM+18,
	CLIENT_DELETED_PLAYER=ID_USER_PACKET_ENUM+19,
	CLIENT_SPAWNED_VEHICLE=ID_USER_PACKET_ENUM+20,
	CLIENT_DELETED_VEHICLE=ID_USER_PACKET_ENUM+21,
	CLIENT_MUST_DELETE_VEHICLE_AND_DRIVER=ID_USER_PACKET_ENUM+22,
	TYRE_POPPED_FULL_DATA=ID_USER_PACKET_ENUM+23,
	SINGLE_TYRE_POPPED=ID_USER_PACKET_ENUM+24,
	CAR_HP_UPDATE=ID_USER_PACKET_ENUM+25,
	CHAR_DUCK_STATE=ID_USER_PACKET_ENUM+26,
	CHAR_SKIN_CHANGE=ID_USER_PACKET_ENUM+27,
	CHAR_GOTO_POS=ID_USER_PACKET_ENUM+28,
	GOTO_NEW_VIRTUAL_WORLD=ID_USER_PACKET_ENUM+29,
	SPAWN_OBJECT=ID_USER_PACKET_ENUM+30,
	DELETE_OBJECT=ID_USER_PACKET_ENUM+31,
	DELETE_VEHICLE_ONLY=ID_USER_PACKET_ENUM+32,
	UPDATE_WORLD_TIME_WEATHER=ID_USER_PACKET_ENUM+33,
	SYNC_GAME_TIME=ID_USER_PACKET_ENUM+34,
	CLIENT_DEATH=ID_USER_PACKET_ENUM+35,
	CLIENT_HAS_RESPAWNED=ID_USER_PACKET_ENUM+36,
	GIVE_WEAPON=ID_USER_PACKET_ENUM+37,
	REMOVE_WEAPON=ID_USER_PACKET_ENUM+38,
	SET_HEALTH=ID_USER_PACKET_ENUM+39,
	COLORED_TEXT=ID_USER_PACKET_ENUM+40,
	UPDATE_COLOR=ID_USER_PACKET_ENUM+41,
	UPDATE_SPAWN_POS=ID_USER_PACKET_ENUM+42,
	OBJECT_CREATE=ID_USER_PACKET_ENUM+43,
	OBJECT_DELETE=ID_USER_PACKET_ENUM+44,
	CHECK_POINT_CREATE=ID_USER_PACKET_ENUM+45,
	CHECK_POINT_DELETE=ID_USER_PACKET_ENUM+46,
	CHECK_POINT_ENTER=ID_USER_PACKET_ENUM+47,
	CHECK_POINT_EXIT=ID_USER_PACKET_ENUM+48,
	COLORED_MSG=ID_USER_PACKET_ENUM+49,
	EJECT_OUT=ID_USER_PACKET_ENUM+50,
	TP_INTO_VEHICLE_SEAT=ID_USER_PACKET_ENUM+51,
	DISPLAY_INFO_MSG=ID_USER_PACKET_ENUM+52,
	DISPLAY_MSG=ID_USER_PACKET_ENUM+53,
	DISPLAY_RECT=ID_USER_PACKET_ENUM+54,
	WIPE_DISPLAYS=ID_USER_PACKET_ENUM+55,
	FREEZE_CLIENT=ID_USER_PACKET_ENUM+56,
	SET_CHECK_POINT_BLIP_SPRITE=ID_USER_PACKET_ENUM+57,
	SET_CHECK_POINT_SHOWING=ID_USER_PACKET_ENUM+58,
	ENTERING_VEHICLE=ID_USER_PACKET_ENUM+59,
	CLIENT_AIM_AT_ELEMENT=ID_USER_PACKET_ENUM+60,
	ADD_PLAYER_TO_LIST=ID_USER_PACKET_ENUM+61,
	REMOVE_PLAYER_FROM_LIST=ID_USER_PACKET_ENUM+62,
	DRAW_DIALOG_LIST=ID_USER_PACKET_ENUM+63,
	DRAW_DIALOG_MSGBOX=ID_USER_PACKET_ENUM+64,
	CLIENT_DIALOG_LIST_RESPONSE=ID_USER_PACKET_ENUM+65,
	CLIENT_DIALOG_MSGBOX_RESPONSE=ID_USER_PACKET_ENUM+66,
	CLIENT_DEBUG_VEHICLE=ID_USER_PACKET_ENUM+67,
	CLIENT_DEBUG_PLAYER=ID_USER_PACKET_ENUM+68,
	CREDENTIALS_TO_LIST=ID_USER_PACKET_ENUM+69,
	ASK_FOR_SERVERS=ID_USER_PACKET_ENUM+70,
	SERVERS_AMOUNT=ID_USER_PACKET_ENUM+71,
	PLAYER_CLOTHES=ID_USER_PACKET_ENUM+72,
	VEHICLE_TUNES=ID_USER_PACKET_ENUM+73,
	PLAYER_LIST_PING=ID_USER_PACKET_ENUM+74,
	PLAYER_HP_CHANGED=ID_USER_PACKET_ENUM+75,
	PLAYER_SET_ANIM=ID_USER_PACKET_ENUM+76,
	PLAYER_ANIM_LOOPING=ID_USER_PACKET_ENUM+77,
	PLAYER_UPDATED_INTERIOR=ID_USER_PACKET_ENUM+78,
	PLAYER_PROP_INDEX=ID_USER_PACKET_ENUM+79,
	VEHICLE_COLOR_CHANGED=ID_USER_PACKET_ENUM+80,
	SIREN_STATE_CHANGED=ID_USER_PACKET_ENUM+81,
	HORN_STATE_CHANGED=ID_USER_PACKET_ENUM+82,
	INDICATOR_STATE_CHANGED=ID_USER_PACKET_ENUM+83,
	BIND_PLAYER_KEY=ID_USER_PACKET_ENUM+84,
	IVMP=ID_USER_PACKET_ENUM+85,
	VEHICLE_FORCE_VELOCITY=ID_USER_PACKET_ENUM+86,
	VEHICLE_FORCE_COORDINATES=ID_USER_PACKET_ENUM+87,
	VEHICLE_FORCE_ROTATION=ID_USER_PACKET_ENUM+88,
	PLAYER_NAME_CHANGED=ID_USER_PACKET_ENUM+89,
	SPAWN_BLIP=ID_USER_PACKET_ENUM+90,
	DELETE_BLIP=ID_USER_PACKET_ENUM+91,
	VEHICLE_DIRT_LEVEL=ID_USER_PACKET_ENUM+92,
	VEHICLE_LIVERY=ID_USER_PACKET_ENUM+93,
	CAMERA_POS=ID_USER_PACKET_ENUM+94,
	CAMERA_LOOK=ID_USER_PACKET_ENUM+95,
	CAMERA_ATTACH=ID_USER_PACKET_ENUM+96,
	OBJECT_MOVE=ID_USER_PACKET_ENUM+97,
	RUN_CHECKSUM=ID_USER_PACKET_ENUM+98,
	RUN_HACK_CHECK=ID_USER_PACKET_ENUM+99,
	PLAYER_WEAPONS_INVENT=ID_USER_PACKET_ENUM+100,
	PASSENGER_SYNC_WITH_POSITION=ID_USER_PACKET_ENUM+101,
	SET_OBJ_AUDIO=ID_USER_PACKET_ENUM+102,
	PLAY_STREAM=ID_USER_PACKET_ENUM+103,
	STOP_STREAM=ID_USER_PACKET_ENUM+104,
	PLAYER_AFK=ID_USER_PACKET_ENUM+105,
	ARMOR_CHANGE = ID_USER_PACKET_ENUM+106,
	SET_MONEY = ID_USER_PACKET_ENUM + 107,
	SET_DOOR_STAT = ID_USER_PACKET_ENUM + 108,
	SERVER_STAT = ID_USER_PACKET_ENUM + 109,
	CLIENT_SCRIPT = ID_USER_PACKET_ENUM + 110,
	SCRIPT_MSG = ID_USER_PACKET_ENUM + 111,
	DISPLAY_MSG_UPDATE = ID_USER_PACKET_ENUM + 112,
	VEHICLE_ENGINE_FLAGS = ID_USER_PACKET_ENUM + 113,
	NPC_POS = ID_USER_PACKET_ENUM + 114,
	NPC_VEHICLE_POS = ID_USER_PACKET_ENUM + 115,
	NPC_HEALTH = ID_USER_PACKET_ENUM + 116,
	NPC_AMMO = ID_USER_PACKET_ENUM + 117,
	NPC_INTERIOR = ID_USER_PACKET_ENUM + 118,
	VEHICLE_MEMBIT = ID_USER_PACKET_ENUM + 119,
	CUSTOM_CHAT = ID_USER_PACKET_ENUM + 120,
	VEHICLE_HOOD_TRUNK = ID_USER_PACKET_ENUM + 121,
	CREATE_EXPLOSION = ID_USER_PACKET_ENUM + 122,
	SET_SIT_IDLE_ANIM = ID_USER_PACKET_ENUM + 123,
	SIT_IDLE_NOW = ID_USER_PACKET_ENUM + 124,
	PLAY_SOUND = ID_USER_PACKET_ENUM + 125,
	STOP_SOUND = ID_USER_PACKET_ENUM + 126,
	SET_HEADING = ID_USER_PACKET_ENUM + 127,
	TS_CONNECT = ID_USER_PACKET_ENUM + 128,
	TS_TRIGGER_VOICE = ID_USER_PACKET_ENUM + 129,
	TS_WHISPER_LIST = ID_USER_PACKET_ENUM + 130,
	PLAYER_DRAW_FLAGS = ID_USER_PACKET_ENUM + 131,
	PLAYER_HUD_SETTINGS = ID_USER_PACKET_ENUM + 132,
	PLAYER_SPRITE_DRAW = ID_USER_PACKET_ENUM + 133,
	VEHICLE_COL = ID_USER_PACKET_ENUM + 133,
	SET_CHECKPOINT_STREAM_DISTANCE = ID_USER_PACKET_ENUM + 134,
	SET_OBJECT_STREAM_DISTANCE = ID_USER_PACKET_ENUM + 134
};
