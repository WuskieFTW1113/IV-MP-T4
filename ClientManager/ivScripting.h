#ifndef IVSCRIPTING_H
#define IVSCRIPTING_H

#include "natives.h"
#include "nativesList.h"
#include "ivEnums.h"

#define DefineScriptHandle(name) typedef struct : ScriptHandle {} name
#define DefineInheritedScriptHandle(name, baseName) typedef struct : baseName {} name

namespace Scripting
{

	typedef u32 ScriptVoid;     // We shall use ScriptVoid instead of explicit void because some 
								// of our template code return values.

	typedef u32 ScriptAny;      // ScriptAny for any types that we don't know yet :)

	typedef struct _ScriptHandle
	{
	private:
		u32 Handle;             // Handles are usually just uint32s

	public:
		_ScriptHandle() : Handle(0) {}
		_ScriptHandle(u32 handle) : Handle(handle) {}
		inline b8 IsNull() { return Handle == 0; }
		inline b8 IsValid() { return Handle != 0; }
		inline u32 Get() { return Handle; }

		// The comparison operation is only valid for handles of the same type!
		inline b8 operator == (const _ScriptHandle &other) { return Handle == other.Handle; }
		inline b8 operator != (const _ScriptHandle &other) { return Handle != other.Handle; }

		inline b8 operator == (const u32 handle) { return Handle == handle; }
		inline b8 operator != (const u32 handle) { return Handle != handle; }
	} ScriptHandle;

	// Explicitly defined handles
	// Use structs instead of just typedefing as ScriptHandle to enforce a bit of type-safety

	typedef u32 Player;     // Left as u32 for legacy support
	typedef u32 FireId;

	DefineScriptHandle(Ped);
	DefineScriptHandle(Vehicle);
	DefineScriptHandle(Cam);
	DefineScriptHandle(CarGenerator);
	DefineScriptHandle(Group);
	DefineInheritedScriptHandle(Train, Vehicle);
	DefineScriptHandle(Pickup);
	DefineScriptHandle(Object);
	DefineScriptHandle(Interior);
	DefineScriptHandle(Blip);
	DefineScriptHandle(Texture);
	DefineScriptHandle(TextureDict);
	DefineScriptHandle(CoverPoint);
	DefineScriptHandle(DecisionMaker);
	DefineScriptHandle(Camera);
	DefineScriptHandle(TaskSequence);

	// For legacy script support...
	typedef Ped Char;
	typedef Vehicle Car;

	// See http://www.gtamodding.com/index.php?title=Carcols.dat#GTA4 for more info
	typedef u32 ColourIndex;    // 0 thru 133 inclusive

								// Some real structures... these are never passed in directly, but only
								// by reference.

	typedef struct { f32 X, Y, Z; } Vector3;

}

namespace Scripting
{
	// These are properly documented functions that were moved from ScriptingDirty.h and cleaned up
	// Beware... not all of them are tested.

	// If you need to add your own functions, add it to a separate file in your own project, not here!
	// This file may be updated very often as new functions are documented.
	static void SetHeliBladesFullSpeed(Vehicle car)
	{
		NativeInvoke::Invoke<NATIVE_SET_HELI_BLADES_FULL_SPEED, ScriptVoid>(car);
	}

	static void SetVehIndicatorLight(Vehicle car, bool on)
	{
		NativeInvoke::Invoke<NATIVE_SET_VEH_INDICATORLIGHTS, ScriptVoid>(car, on);
	}

	static void SetCanBurstCarTyres(Vehicle car, bool can)
	{
		NativeInvoke::Invoke<NATIVE_SET_CAN_BURST_CAR_TYRES, ScriptVoid>(car, can);
	}

	static void SetCharDucking(Ped player, bool toggle)
	{
		NativeInvoke::Invoke<NATIVE_SET_CHAR_DUCKING, ScriptVoid>(player, toggle);
	}

	static void DrawCheckPointWithAlpha(float pX, float pY, float pZ, float radius, int r, int g, int b, int a)
	{
		NativeInvoke::Invoke<NATIVE_DRAW_CHECKPOINT_WITH_ALPHA, ScriptVoid>(pX, pY, pZ, radius, r, g, b, a);
	}

	static void GetGameViewportId(int* viewPort) { NativeInvoke::Invoke<NATIVE_GET_GAME_VIEWPORT_ID, ScriptVoid>(viewPort); }

	static void SetMaxWantedLevel(unsigned int max) { NativeInvoke::Invoke<NATIVE_SET_MAX_WANTED_LEVEL, ScriptVoid>(max); }
	static void SetSleepModeActive(bool active) { NativeInvoke::Invoke<NATIVE_SET_SLEEP_MODE_ACTIVE, bool>(active); }

	static void GetViewPortPosAndSize(int viewPort, float* posX, float* posY, float* sizeX, float* sizeY)
	{
		NativeInvoke::Invoke<NATIVE_GET_VIEWPORT_POS_AND_SIZE, ScriptVoid>(viewPort, posX, posY, sizeX, sizeY);
	}

	static void GetViewportPositionOfCoord(float gameX, float gameY, float gameZ, int viewPort, float* screenX, float* screenY)
	{
		NativeInvoke::Invoke<NATIVE_GET_VIEWPORT_POSITION_OF_COORD, ScriptVoid>(gameX, gameY, gameZ, viewPort, screenX, screenY);
	}

	static void RemoveFakeNetworkNameFromPed(unsigned int ped) { NativeInvoke::Invoke<NATIVE_REMOVE_FAKE_NETWORK_NAME_FROM_PED, ScriptVoid>(ped); }
	static void DisplayPlayerNames(bool toggle) { NativeInvoke::Invoke<NATIVE_DISPLAY_PLAYER_NAMES, ScriptVoid>(toggle); }

	static void SetPedNonCreationArea(float sx, float sy, float sz, float ex, float ey, float ez) { NativeInvoke::Invoke<NATIVE_SET_PED_NON_CREATION_AREA, ScriptVoid>(sx, sy, sz, ex, ey, ez); }
	static void DisableCarGenerators(ScriptAny p0) { NativeInvoke::Invoke<NATIVE_DISABLE_CAR_GENERATORS, ScriptVoid>(p0); }
	static void SetNoResprays(bool no) { NativeInvoke::Invoke<NATIVE_SET_NO_RESPRAYS, ScriptVoid>(no); }
	static void TerminateAllScriptsWithThisName(char * name) { NativeInvoke::Invoke<NATIVE_TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME, ScriptVoid>(name); }
	static void GivePedFakeNetworkNameTwo(Ped ped, char * name, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) { NativeInvoke::Invoke<NATIVE_GIVE_PED_FAKE_NETWORK_NAME, ScriptVoid>(ped, name, red, green, blue, alpha); }

	static void REGISTER_WORLD_POINT_SCRIPT_BRAIN(char * name, float range)
	{
		NativeInvoke::Invoke<NATIVE_REGISTER_WORLD_POINT_SCRIPT_BRAIN, ScriptVoid>(name, range);
	}

	static void ALLOCATE_SCRIPT_TO_OBJECT(char * name, int objModel, float range)
	{
		NativeInvoke::Invoke<NATIVE_ALLOCATE_SCRIPT_TO_OBJECT, ScriptVoid>(name, objModel, 100, range, -1);
	}

	static void SetVehicleSteerBias(Vehicle car, float val)
	{
		NativeInvoke::Invoke<NATIVE_SET_VEHICLE_STEER_BIAS, ScriptVoid>(car, val);
	}

	static int GET_CUTSCENE_AUDIO_TIME_MS()
	{
		return NativeInvoke::Invoke<NATIVE_GET_CUTSCENE_AUDIO_TIME_MS, int>();
	}

	static bool IS_SITTING_OBJECT_NEAR(float x, float y, float z)
	{
		return NativeInvoke::Invoke<NATIVE_IS_SITTING_OBJECT_NEAR, bool>(x, y, z, 0);
	}

	static void TASK_SIT_DOWN_ON_NEAREST_OBJECT(Ped ped, float x, float y, float z)
	{
		NativeInvoke::Invoke<NATIVE_TASK_SIT_DOWN_ON_NEAREST_OBJECT, u32>(ped, 2, 0, x, y, z + 1.0f, 0, 0.0f, -2, 1);
	}

	static void ChangeCharSitIdleAnim(Ped ped, char* group, char* name, u32 something)
	{
		NativeInvoke::Invoke<NATIVE_CHANGE_CHAR_SIT_IDLE_ANIM, u32>(ped, group, name, something);
	}

	static void SetCarDensityMultiplier(unsigned int* density) { NativeInvoke::Invoke<NATIVE_SET_CAR_DENSITY_MULTIPLIER, ScriptVoid>(density); }
	static void SetPedDensityMultiplier(unsigned int* density) { NativeInvoke::Invoke<NATIVE_SET_PED_DENSITY_MULTIPLIER, ScriptVoid>(density); }
	static void SetRandomCarDensityMultiplier(unsigned int* density) { NativeInvoke::Invoke<NATIVE_SET_RANDOM_CAR_DENSITY_MULTIPLIER, ScriptVoid>(density); }
	static void SetScenarioPedDensityMultiplier(unsigned int* scenario, unsigned int*density) { NativeInvoke::Invoke<NATIVE_SET_SCENARIO_PED_DENSITY_MULTIPLIER, ScriptVoid>(scenario, density); }
	static void SetParkedCarDensityMultiplier(unsigned int* density) { NativeInvoke::Invoke<NATIVE_SET_PARKED_CAR_DENSITY_MULTIPLIER, ScriptVoid>(density); }


	static void TaskCarDriveToCoord(Ped p, Vehicle car, Vector3& pos, float speed)
	{
		NativeInvoke::Invoke<NATIVE_TASK_CAR_DRIVE_TO_COORD, ScriptVoid>(p, car, pos.X, pos.Y, pos.Z, speed, 0, 0, 0, 10.0, -1);
	}

	static void GetCarAnimCurrentTime(Vehicle car, char* animGroup, char* animName, float *time) { NativeInvoke::Invoke<NATIVE_GET_CAR_ANIM_CURRENT_TIME, ScriptVoid>(car, animGroup, animName, time); }

	static void TaskCarTempAction(Ped p, Vehicle v, int task, int duration) { NativeInvoke::Invoke<NATIVE_TASK_CAR_TEMP_ACTION, ScriptVoid>(p, v, task, duration); }

	static void SetCarTraction(Vehicle v, float f) { NativeInvoke::Invoke<NATIVE_SET_CAR_TRACTION, ScriptVoid>(v, f); }

	static void SetCarEngineOn(Vehicle v, int p1, int p2) { NativeInvoke::Invoke<NATIVE_SET_CAR_ENGINE_ON, ScriptVoid>(v, p1, p2); }

	static void TurnOffVehicleExtra(Vehicle v, int p1, b8 p2) { NativeInvoke::Invoke<NATIVE_TURN_OFF_VEHICLE_EXTRA, ScriptVoid>(v, p1, p2); }
	static void SwitchCarSiren(Vehicle v, b8 p1) { NativeInvoke::Invoke<NATIVE_SWITCH_CAR_SIREN, ScriptVoid>(v, p1); }
	static void SetCarLivery(Vehicle v, int p1) { NativeInvoke::Invoke<NATIVE_SET_CAR_LIVERY, ScriptVoid>(v, p1); }

	static void GetVehicleQuaternion(Vehicle v, f32* pX, f32* pY, f32* pZ, f32* pW) { NativeInvoke::Invoke<NATIVE_GET_VEHICLE_QUATERNION, ScriptVoid>(v, pX, pY, pZ, pW); }
	static void SetVehicleQuaternion(Vehicle v, f32 pX, f32 pY, f32 pZ, f32 pW) { NativeInvoke::Invoke<NATIVE_SET_VEHICLE_QUATERNION, ScriptVoid>(v, pX, pY, pZ, pW); }
	//static bool GetVehicleComponentInfo(Vehicle v, u32 partId, f32* pX, f32* pY, f32* pZ, int someBool){ NativeInvoke::Invoke<NATIVE_GET_VEHICLE_COMPONENT_INFO, bool>(v, partId, pX, pY, pZ, someBool); }

	// Player
	static void GivePedFakeNetworkName(Ped ped, char * name, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) { NativeInvoke::Invoke<NATIVE_GIVE_PED_FAKE_NETWORK_NAME, ScriptVoid>(ped, name, red, green, blue, alpha); }

	static void DrawSphere(float x, float y, float z, float radius) { NativeInvoke::Invoke<NATIVE_DRAW_SPHERE, ScriptVoid>(x, y, z, radius); }

	static void TaskShootAtCoord(Ped ped, float x, float y, float z, unsigned int time_ms, unsigned int type_5) { NativeInvoke::Invoke<NATIVE_TASK_SHOOT_AT_COORD, ScriptVoid>(ped, x, y, z, time_ms, type_5); }

	static void GetCharMoveAnimSpeed(Ped p, float *v) { NativeInvoke::Invoke<NATIVE_GET_CHAR_MOVE_ANIM_SPEED_MULTIPLIER, ScriptVoid>(p, v); }
	static void SetCharMoveAnimSpeed(Ped p, float v) { NativeInvoke::Invoke<NATIVE_SET_CHAR_MOVE_ANIM_SPEED_MULTIPLIER, ScriptVoid>(p, v); }
	static void SetAnimGroupForChar(Ped p, const char * s) { NativeInvoke::Invoke<NATIVE_SET_ANIM_GROUP_FOR_CHAR, ScriptVoid>(p, s); }

	static void AllowReactionAnims(Ped p, bool b) { NativeInvoke::Invoke<NATIVE_ALLOW_REACTION_ANIMS, ScriptVoid>(p, b); }

	static void AddScore(Player playerIndex, i32 score) { NativeInvoke::Invoke<NATIVE_ADD_SCORE, ScriptVoid>(playerIndex, score); }
	static void AllowPlayerToCarryNonMissionObjects(Player playerIndex, b8 allow) { NativeInvoke::Invoke<NATIVE_ALLOW_PLAYER_TO_CARRY_NON_MISSION_OBJECTS, ScriptVoid>(playerIndex, allow); }
	static void AlterWantedLevel(Player playerIndex, u32 level) { NativeInvoke::Invoke<NATIVE_ALTER_WANTED_LEVEL, ScriptVoid>(playerIndex, level); }
	static void AlterWantedLevelNoDrop(Player playerIndex, u32 level) { NativeInvoke::Invoke<NATIVE_ALTER_WANTED_LEVEL_NO_DROP, ScriptVoid>(playerIndex, level); }
	static void ApplyWantedLevelChangeNow(Player playerIndex) { NativeInvoke::Invoke<NATIVE_APPLY_WANTED_LEVEL_CHANGE_NOW, ScriptVoid>(playerIndex); }
	static void ChangePlayerModel(Player playerIndex, unsigned int model) { NativeInvoke::Invoke<NATIVE_CHANGE_PLAYER_MODEL, ScriptVoid>(playerIndex, model); }
	static void ClearPlayerHasDamagedAtLeastOnePed(Player playerIndex) { NativeInvoke::Invoke<NATIVE_CLEAR_PLAYER_HAS_DAMAGED_AT_LEAST_ONE_PED, ScriptVoid>(playerIndex); }
	static Player ConvertIntToPlayerIndex(u32 playerId) { return NativeInvoke::Invoke<NATIVE_CONVERT_INT_TO_PLAYERINDEX, Player>(playerId); }
	static void ClearWantedLevel(Player playerIndex) { NativeInvoke::Invoke<NATIVE_CLEAR_WANTED_LEVEL, ScriptVoid>(playerIndex); }
	static void CreatePlayer(u32 playerId, f32 x, f32 y, f32 z, Player *pPlayerIndex) { NativeInvoke::Invoke<NATIVE_CREATE_PLAYER, ScriptVoid>(playerId, x, y, z, pPlayerIndex); }
	static void DisablePlayerLockon(Player playerIndex, b8 disabled) { NativeInvoke::Invoke<NATIVE_DISABLE_PLAYER_LOCKON, ScriptVoid>(playerIndex, disabled); }
	static void DisablePlayerSprint(Player playerIndex, b8 disabled) { NativeInvoke::Invoke<NATIVE_DISABLE_PLAYER_SPRINT, ScriptVoid>(playerIndex, disabled); }
	static void GetPlayerChar(Player playerIndex, Ped *pPed) { NativeInvoke::Invoke<NATIVE_GET_PLAYER_CHAR, ScriptVoid>(playerIndex, pPed); }
	static void GetPlayerGroup(Player playerIndex, Group *pGroup) { NativeInvoke::Invoke<NATIVE_GET_PLAYER_GROUP, ScriptVoid>(playerIndex, pGroup); }
	static u32 GetPlayerId() { return NativeInvoke::Invoke<NATIVE_GET_PLAYER_ID, u32>(); }
	static void GetPlayersLastCarNoSave(Vehicle* pVehicle) { NativeInvoke::Invoke<NATIVE_GET_PLAYERS_LAST_CAR_NO_SAVE, ScriptVoid>(pVehicle); }
	static void GetPlayerMaxArmour(Player playerIndex, u32 *pMaxArmour) { NativeInvoke::Invoke<NATIVE_GET_PLAYER_MAX_ARMOUR, ScriptVoid>(playerIndex, pMaxArmour); }
	static const ch *GetPlayerName(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_GET_PLAYER_NAME, const ch *>(playerIndex); }
	static eModel GetPlayerSettingsModelChoice() { return NativeInvoke::Invoke<NATIVE_GET_PLAYERSETTINGS_MODEL_CHOICE, eModel>(); }
	static ScriptAny GetPlayerToPlaceBombInCar(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_GET_PLAYER_TO_PLACE_BOMB_IN_CAR, ScriptAny>(vehicle); }
	static u32 GetTimeSincePlayerDroveAgainstTraffic(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_GET_TIME_SINCE_PLAYER_DROVE_AGAINST_TRAFFIC, u32>(playerIndex); }
	static u32 GetTimeSincePlayerDroveOnPavement(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_GET_TIME_SINCE_PLAYER_DROVE_ON_PAVEMENT, u32>(playerIndex); }
	static u32 GetTimeSincePlayerHitBuilding(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_GET_TIME_SINCE_PLAYER_HIT_BUILDING, u32>(playerIndex); }
	static u32 GetTimeSincePlayerHitCar(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_GET_TIME_SINCE_PLAYER_HIT_CAR, u32>(playerIndex); }
	static u32 GetTimeSincePlayerHitObject(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_GET_TIME_SINCE_PLAYER_HIT_OBJECT, u32>(playerIndex); }
	static u32 GetTimeSincePlayerHitPed(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_GET_TIME_SINCE_PLAYER_HIT_PED, u32>(playerIndex); }
	static u32 GetTimeSincePlayerRanLight(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_GET_TIME_SINCE_PLAYER_RAN_LIGHT, u32>(playerIndex); }
	static b8 HasPlayerCollectedPickup(Player playerIndex, Pickup pikcup) { return NativeInvoke::Invoke<NATIVE_HAS_PLAYER_COLLECTED_PICKUP, b8>(playerIndex, pikcup); }
	static b8 HasPlayerDamagedAtLeastOnePed(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_HAS_PLAYER_DAMAGED_AT_LEAST_ONE_PED, b8>(playerIndex); }
	static b8 HasPlayerDamagedAtLeastOneVehicle(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_HAS_PLAYER_DAMAGED_AT_LEAST_ONE_VEHICLE, b8>(playerIndex); }
	static b8 IsPlayerClimbing(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_CLIMBING, b8>(playerIndex); }
	static b8 IsPlayerControlOn(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_CONTROL_ON, b8>(playerIndex); }
	static b8 IsPlayerDead(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_DEAD, b8>(playerIndex); }
	static b8 IsPlayerFreeAimingAtChar(Player playerIndex, Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_FREE_AIMING_AT_CHAR, b8>(playerIndex, ped); }
	static b8 IsPlayerFreeForAmbientTask(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_FREE_FOR_AMBIENT_TASK, b8>(playerIndex); }
	static b8 IsPlayerPlaying(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_PLAYING, b8>(playerIndex); }
	static b8 IsPlayerPressingHorn(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_PRESSING_HORN, b8>(playerIndex); }
	static b8 IsPlayerTargettingAnything(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_TARGETTING_ANYTHING, b8>(playerIndex); }
	static b8 IsPlayerTargettingChar(Player playerIndex, Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_TARGETTING_CHAR, b8>(playerIndex, ped); }
	static b8 IsPlayerTargettingObject(Player playerIndex, Object obj) { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_TARGETTING_OBJECT, b8>(playerIndex, obj); }
	static b8 IsScoreGreater(Player playerIndex, u32 score) { return NativeInvoke::Invoke<NATIVE_IS_SCORE_GREATER, b8>(playerIndex, score); }
	static b8 IsWantedLevelGreater(Player playerIndex, u32 level) { return NativeInvoke::Invoke<NATIVE_IS_WANTED_LEVEL_GREATER, b8>(playerIndex, level); }
	static b8 PlayerHasChar(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_PLAYER_HAS_CHAR, b8>(playerIndex); }
	static b8 PlayerHasFlashingStarsAboutToDrop(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_PLAYER_HAS_FLASHING_STARS_ABOUT_TO_DROP, b8>(playerIndex); }
	static b8 PlayerHasGreyedOutStars(Player playerIndex) { return NativeInvoke::Invoke<NATIVE_PLAYER_HAS_GREYED_OUT_STARS, b8>(playerIndex); }
	static void RegisterPlayerRespawnCoords(Player playerIndex, f32 x, f32 y, f32 z) { NativeInvoke::Invoke<NATIVE_REGISTER_PLAYER_RESPAWN_COORDS, ScriptVoid>(playerIndex, x, y, z); }
	static void SetEveryoneIgnorePlayer(Player playerIndex, b8 value) { NativeInvoke::Invoke<NATIVE_SET_EVERYONE_IGNORE_PLAYER, ScriptVoid>(playerIndex, value); }
	static void SetPlayerCanBeHassledByGangs(Player playerIndex, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_CAN_BE_HASSLED_BY_GANGS, ScriptVoid>(playerIndex, value); }
	static void SetPlayerCanDoDriveBy(Player playerIndex, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_CAN_DO_DRIVE_BY, ScriptVoid>(playerIndex, value); }
	static void SetPlayerCanUseCover(Player playerIndex, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_CAN_USE_COVER, ScriptVoid>(playerIndex, value); }
	static void SetGameCameraControlsActive(b8 value) { NativeInvoke::Invoke<NATIVE_SET_GAME_CAMERA_CONTROLS_ACTIVE, ScriptVoid>(value); }
	static void SetPlayerControl(Player playerIndex, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_CONTROL, ScriptVoid>(playerIndex, value); }
	static void SetPlayerControlForTextChat(Player playerIndex, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_CONTROL_FOR_TEXT_CHAT, ScriptVoid>(playerIndex, value); }
	static void SetPlayerControlAdvanced(Player playerIndex, b8 unknown1, b8 unknown2, b8 unknown3) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_CONTROL_ADVANCED, ScriptVoid>(playerIndex, unknown1, unknown2, unknown3); }
	static void SetPlayerFastReload(Player playerIndex, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_FAST_RELOAD, ScriptVoid>(playerIndex, value); }
	static void SetPlayerGroupToFollowAlways(Player playerIndex, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_GROUP_TO_FOLLOW_ALWAYS, ScriptVoid>(playerIndex, value); }
	static void SetPlayerInvincible(Player playerIndex, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_INVINCIBLE, ScriptVoid>(playerIndex, value); }
	static void SetPlayerMoodNormal(Player playerIndex) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_MOOD_NORMAL, ScriptVoid>(playerIndex); }
	static void SetPlayerMoodPissedOff(Player playerIndex, u32 unknown150) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_MOOD_PISSED_OFF, ScriptVoid>(playerIndex, unknown150); }
	static void SetPlayerNeverGetsTired(Player playerIndex, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PLAYER_NEVER_GETS_TIRED, ScriptVoid>(playerIndex, value); }
	static void SetPlayerSettingsModelVariationsChoice(Player playerIndex) { NativeInvoke::Invoke<NATIVE_SET_PLAYERSETTINGS_MODEL_VARIATIONS_CHOICE, ScriptVoid>(playerIndex); }
	static void SetPoliceIgnorePlayer(Player playerIndex, b8 value) { NativeInvoke::Invoke<NATIVE_SET_POLICE_IGNORE_PLAYER, ScriptVoid>(playerIndex, value); }
	static void StoreScore(Player playerIndex, u32* value) { NativeInvoke::Invoke<NATIVE_STORE_SCORE, ScriptVoid>(playerIndex, value); }
	static void StoreWantedLevel(Player playerIndex, u32* value) { NativeInvoke::Invoke<NATIVE_STORE_WANTED_LEVEL, ScriptVoid>(playerIndex, value); }
	static void RemovePlayerHelmet(Player playerIndex, b8 remove) { NativeInvoke::Invoke<NATIVE_REMOVE_PLAYER_HELMET, ScriptVoid>(playerIndex, remove); }

	// Ped
	static void AddAmmoToChar(Ped ped, u32 weapon, u32 amount) { NativeInvoke::Invoke<NATIVE_ADD_AMMO_TO_CHAR, ScriptVoid>(ped, weapon, amount); }
	static void AddArmourToChar(Ped ped, u32 amount) { NativeInvoke::Invoke<NATIVE_ADD_ARMOUR_TO_CHAR, ScriptVoid>(ped, amount); }
	static void ApplyForceToPed(Ped ped, u32 unknown0_3, f32 x, f32 y, f32 z, f32 spinX, f32 spinY, f32 spinZ, u32 unknown4_0, u32 unknown5_1, u32 unknown6_1, u32 unknown7_1) { NativeInvoke::Invoke<NATIVE_APPLY_FORCE_TO_PED, ScriptVoid>(ped, unknown0_3, x, y, z, spinX, spinY, spinZ, unknown4_0, unknown5_1, unknown6_1, unknown7_1); }
	static void AttachPedToCar(Ped ped, Vehicle vehicle, u32 unknown0_0, f32 offsetX, f32 offsetY, f32 offsetZ, f32 unknown1_276, f32 unknown2_0, u32 unknown3_0, u32 unknown4_0) { NativeInvoke::Invoke<NATIVE_ATTACH_PED_TO_CAR, ScriptVoid>(ped, vehicle, unknown0_0, offsetX, offsetY, offsetZ, unknown1_276, unknown2_0, unknown3_0, unknown4_0); }
	static void BlockCharGestureAnims(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_BLOCK_CHAR_GESTURE_ANIMS, ScriptVoid>(ped, value); }
	static void BlockPedWeaponSwitching(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_BLOCK_PED_WEAPON_SWITCHING, ScriptVoid>(ped, value); }
	static void BlockCoweringInCover(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_BLOCK_COWERING_IN_COVER, ScriptVoid>(ped, value); }
	static void BlockPeekingInCover(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_BLOCK_PEEKING_IN_COVER, ScriptVoid>(ped, value); }
	static void CancelCurrentlyPlayingAmbientSpeech(Ped ped) { NativeInvoke::Invoke<NATIVE_CANCEL_CURRENTLY_PLAYING_AMBIENT_SPEECH, ScriptVoid>(ped); }
	static void ClearAllCharProps(Ped ped) { NativeInvoke::Invoke<NATIVE_CLEAR_ALL_CHAR_PROPS, ScriptVoid>(ped); }
	static void ClearCharLastDamageBone(Ped ped) { NativeInvoke::Invoke<NATIVE_CLEAR_CHAR_LAST_DAMAGE_BONE, ScriptVoid>(ped); }
	static void ClearCharLastDamageEntity(Ped ped) { NativeInvoke::Invoke<NATIVE_CLEAR_CHAR_LAST_DAMAGE_ENTITY, ScriptVoid>(ped); }
	static void ClearCharLastWeaponDamage(Ped ped) { NativeInvoke::Invoke<NATIVE_CLEAR_CHAR_LAST_WEAPON_DAMAGE, ScriptVoid>(ped); }
	static void ClearCharProp(Ped ped, b8 unknown) { NativeInvoke::Invoke<NATIVE_CLEAR_CHAR_PROP, ScriptVoid>(ped, unknown); }
	static void ClearCharSecondaryTask(Ped ped) { NativeInvoke::Invoke<NATIVE_CLEAR_CHAR_SECONDARY_TASK, ScriptVoid>(ped); }
	static void ClearCharTasks(Ped ped) { NativeInvoke::Invoke<NATIVE_CLEAR_CHAR_TASKS, ScriptVoid>(ped); }
	static void ClearCharTasksImmediately(Ped ped) { NativeInvoke::Invoke<NATIVE_CLEAR_CHAR_TASKS_IMMEDIATELY, ScriptVoid>(ped); }
	static void ClearRoomForChar(Ped ped) { NativeInvoke::Invoke<NATIVE_CLEAR_ROOM_FOR_CHAR, ScriptVoid>(ped); }
	static void CreateChar(u32 type, unsigned int model, f32 x, f32 y, f32 z, Ped *pPed, b8 unknownTrue) { NativeInvoke::Invoke<NATIVE_CREATE_CHAR, ScriptVoid>(type, model, x, y, z, pPed, unknownTrue); }
	static void CreateRandomChar(f32 x, f32 y, f32 z, Ped *pPed) { NativeInvoke::Invoke<NATIVE_CREATE_RANDOM_CHAR, ScriptVoid>(x, y, z, pPed); }
	static void CreateRandomCharAsDriver(Vehicle vehicle, Ped *pPed) { NativeInvoke::Invoke<NATIVE_CREATE_RANDOM_CHAR_AS_DRIVER, ScriptVoid>(vehicle, pPed); }
	static void CreateRandomFemaleChar(f32 x, f32 y, f32 z, Ped *pPed) { NativeInvoke::Invoke<NATIVE_CREATE_RANDOM_FEMALE_CHAR, ScriptVoid>(x, y, z, pPed); }
	static void CreateRandomMaleChar(f32 x, f32 y, f32 z, Ped *pPed) { NativeInvoke::Invoke<NATIVE_CREATE_RANDOM_MALE_CHAR, ScriptVoid>(x, y, z, pPed); }
	static void DamageChar(Ped ped, u32 hitPoints, b8 unknown) { NativeInvoke::Invoke<NATIVE_DAMAGE_CHAR, ScriptVoid>(ped, hitPoints, unknown); }
	static void DamagePedBodyPart(Ped ped, ePedBodyPart part, u32 hitPoints) { NativeInvoke::Invoke<NATIVE_DAMAGE_PED_BODY_PART, ScriptVoid>(ped, part, hitPoints); }
	static void DeleteChar(Ped *pPed) { NativeInvoke::Invoke<NATIVE_DELETE_CHAR, ScriptVoid>(pPed); }
	static void DetachPed(Ped ped, b8 unknown) { NativeInvoke::Invoke<NATIVE_DETACH_PED, ScriptVoid>(ped, unknown); }
	static void DetachPedFromWithinCar(Ped ped, b8 unknown) { NativeInvoke::Invoke<NATIVE_DETACH_PED_FROM_WITHIN_CAR, ScriptVoid>(ped, unknown); }
	static b8 DoesCharExist(Ped ped) { return NativeInvoke::Invoke<NATIVE_DOES_CHAR_EXIST, b8>(ped); }
	static void DropObject(Ped ped, b8 unknownTrue) { NativeInvoke::Invoke<NATIVE_DROP_OBJECT, ScriptVoid>(ped, unknownTrue); }
	static void ExplodeCharHead(Ped ped) { NativeInvoke::Invoke<NATIVE_EXPLODE_CHAR_HEAD, ScriptVoid>(ped); }
	static void ExtinguishCharFire(Ped ped) { NativeInvoke::Invoke<NATIVE_EXTINGUISH_CHAR_FIRE, ScriptVoid>(ped); }
	static void FirePedWeapon(Ped ped, f32 x, f32 y, f32 z) { NativeInvoke::Invoke<NATIVE_FIRE_PED_WEAPON, ScriptVoid>(ped, x, y, z); }
	static void ForceCharToDropWeapon(Ped ped) { NativeInvoke::Invoke<NATIVE_FORCE_CHAR_TO_DROP_WEAPON, ScriptVoid>(ped); }
	static ScriptAny ForcePedPinnedDown(Ped ped, b8 force, u32 timerMaybe) { return NativeInvoke::Invoke<NATIVE_FORCE_PED_PINNED_DOWN, ScriptAny>(ped, force, timerMaybe); }
	static void ForcePedToFleeWhilstDrivingVehicle(Ped ped, Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_FORCE_PED_TO_FLEE_WHILST_DRIVING_VEHICLE, ScriptVoid>(ped, vehicle); }
	static void FreezeCharPosition(Ped ped, b8 frozen) { NativeInvoke::Invoke<NATIVE_FREEZE_CHAR_POSITION, ScriptVoid>(ped, frozen); }
	static void FreezeCharPositionAndDontLoadCollision(Ped ped, b8 frozen) { NativeInvoke::Invoke<NATIVE_FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION, ScriptVoid>(ped, frozen); }
	static void GetAmmoInCharWeapon(Ped ped, u32 weapon, u32 *pAmmo) { NativeInvoke::Invoke<NATIVE_GET_AMMO_IN_CHAR_WEAPON, ScriptVoid>(ped, weapon, pAmmo); }
	static b8 GetAmmoInClip(Ped ped, u32 weapon, u32 *pAmmo) { return NativeInvoke::Invoke<NATIVE_GET_AMMO_IN_CLIP, b8>(ped, weapon, pAmmo); }
	static const ch *GetAnimGroupFromChar(Ped ped) { return NativeInvoke::Invoke<NATIVE_GET_ANIM_GROUP_FROM_CHAR, const ch *>(ped); }
	static void GetCharCoordinates(Ped ped, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_CHAR_COORDINATES, ScriptVoid>(ped, pX, pY, pZ); }
	static void GetCharHealth(Ped ped, u32 *pHealth) { NativeInvoke::Invoke<NATIVE_GET_CHAR_HEALTH, ScriptVoid>(ped, pHealth); }
	static void GetCharAnimCurrentTime(Ped ped, const ch *animGroup, const ch *animName, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CHAR_ANIM_CURRENT_TIME, ScriptVoid>(ped, animGroup, animName, pValue); }
	static void GetCharAnimTotalTime(Ped ped, const ch *animGroup, const ch *animName, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CHAR_ANIM_TOTAL_TIME, ScriptVoid>(ped, animGroup, animName, pValue); }
	static bool HasCharAnimFinished(Ped ped, const ch *animGroup, const ch *animName) { return NativeInvoke::Invoke<NATIVE_HAS_CHAR_ANIM_FINISHED, bool>(ped, animGroup, animName); }
	static void GetCharArmour(Ped ped, u32 *pArmour) { NativeInvoke::Invoke<NATIVE_GET_CHAR_ARMOUR, ScriptVoid>(ped, pArmour); }
	static u32 GetCharDrawableVariation(Ped ped, ePedComponent component) { return NativeInvoke::Invoke<NATIVE_GET_CHAR_DRAWABLE_VARIATION, ScriptAny>(ped, component); }
	static void GetCharExtractedDisplacement(Ped ped, b8 unknown, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_CHAR_EXTRACTED_DISPLACEMENT, ScriptVoid>(ped, unknown, pX, pY, pZ); }
	static void GetCharHeading(Ped ped, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CHAR_HEADING, ScriptVoid>(ped, pValue); }
	static void GetCharHeightAboveGround(Ped ped, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CHAR_HEIGHT_ABOVE_GROUND, ScriptVoid>(ped, pValue); }
	static ScriptAny GetCharLastDamageBone(Ped ped, int* pBone) { return NativeInvoke::Invoke<NATIVE_GET_CHAR_LAST_DAMAGE_BONE, ScriptAny>(ped, pBone); }
	static b8 GetCharMeleeActionFlag0(Ped ped) { return NativeInvoke::Invoke<NATIVE_GET_CHAR_MELEE_ACTION_FLAG0, b8>(ped); }
	static b8 GetCharMeleeActionFlag1(Ped ped) { return NativeInvoke::Invoke<NATIVE_GET_CHAR_MELEE_ACTION_FLAG1, b8>(ped); }
	static void GetCharModel(Ped ped, eModel *pModel) { NativeInvoke::Invoke<NATIVE_GET_CHAR_MODEL, ScriptVoid>(ped, pModel); }
	static u32 GetCharMoney(Ped ped) { return NativeInvoke::Invoke<NATIVE_GET_CHAR_MONEY, u32>(ped); }
	static void GetCharPropIndex(Ped ped, b8 unknown, u32 *pIndex) { NativeInvoke::Invoke<NATIVE_GET_CHAR_PROP_INDEX, ScriptVoid>(ped, unknown, pIndex); }
	static b8 GetCharReadyToBeExecuted(Ped ped) { return NativeInvoke::Invoke<NATIVE_GET_CHAR_READY_TO_BE_EXECUTED, b8>(ped); }
	static b8 GetCharReadyToBeStunned(Ped ped) { return NativeInvoke::Invoke<NATIVE_GET_CHAR_READY_TO_BE_STUNNED, b8>(ped); }
	static void GetCharSpeed(Ped ped, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CHAR_SPEED, ScriptVoid>(ped, pValue); }
	static u32 GetCharTextureVariation(Ped ped, ePedComponent component) { return NativeInvoke::Invoke<NATIVE_GET_CHAR_TEXTURE_VARIATION, u32>(ped, component); }
	static void GetCharVelocity(Ped ped, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_CHAR_VELOCITY, ScriptVoid>(ped, pX, pY, pZ); }
	static void GetCharWeaponInSlot(Ped ped, u32 slot, u32 *pWeapon, ScriptAny *pUnknown1, ScriptAny *pUnknown2) { NativeInvoke::Invoke<NATIVE_GET_CHAR_WEAPON_IN_SLOT, ScriptVoid>(ped, slot, pWeapon, pUnknown1, pUnknown2); }
	static b8 GetCharWillCowerInsteadOfFleeing(Ped ped) { return NativeInvoke::Invoke<NATIVE_GET_CHAR_WILL_COWER_INSTEAD_OF_FLEEING, b8>(ped); }
	static ScriptAny GetCurrentCharWeapon(Ped ped, u32 *pWeapon) { return NativeInvoke::Invoke<NATIVE_GET_CURRENT_CHAR_WEAPON, ScriptAny>(ped, pWeapon); }
	static u32 GetDamageToPedBodyPart(Ped ped, ePedBodyPart part) { return NativeInvoke::Invoke<NATIVE_GET_DAMAGE_TO_PED_BODY_PART, u32>(ped, part); }
	static void SetDeadPedsDropWeapons(b8 b) { NativeInvoke::Invoke<NATIVE_SET_DEAD_PEDS_DROP_WEAPONS, ScriptVoid>(b); }
	static void GetDeadCharCoordinates(Ped ped, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_DEAD_CHAR_COORDINATES, ScriptVoid>(ped, pX, pY, pZ); }
	static void GetDeadCharPickupCoords(Ped ped, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_DEAD_CHAR_PICKUP_COORDS, ScriptVoid>(ped, pX, pY, pZ); }
	static void GetKeyForCharInRoom(Ped ped, u32 *pKey) { NativeInvoke::Invoke<NATIVE_GET_KEY_FOR_CHAR_IN_ROOM, ScriptVoid>(ped, pKey); }
	static ScriptAny GetMaxAmmo(Ped ped, u32 weapon, u32 *pMaxAmmo) { return NativeInvoke::Invoke<NATIVE_GET_MAX_AMMO, ScriptAny>(ped, weapon, pMaxAmmo); }
	static void GetMaxAmmoInClip(Ped ped, u32 weapon, u32 *pMaxAmmo) { NativeInvoke::Invoke<NATIVE_GET_MAX_AMMO_IN_CLIP, ScriptVoid>(ped, weapon, pMaxAmmo); }
	static u32 GetNumberOfCharDrawableVariations(Ped ped, u32 component) { return NativeInvoke::Invoke<NATIVE_GET_NUMBER_OF_CHAR_DRAWABLE_VARIATIONS, u32>(ped, component); }
	static u32 GetNumberOfCharTextureVariations(Ped ped, u32 component, u32 unknown1) { return NativeInvoke::Invoke<NATIVE_GET_NUMBER_OF_CHAR_TEXTURE_VARIATIONS, u32>(ped, component, unknown1); }
	static Object GetObjectPedIsHolding(Ped ped) { return NativeInvoke::Invoke<NATIVE_GET_OBJECT_PED_IS_HOLDING, Object>(ped); }
	static void GetOffsetFromCharInWorldCoords(Ped ped, f32 x, f32 y, f32 z, f32 *pOffX, f32 *pOffY, f32 *pOffZ) { NativeInvoke::Invoke<NATIVE_GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS, ScriptVoid>(ped, x, y, z, pOffX, pOffY, pOffZ); }
	static ePedClimbState GetPedClimbState(Ped ped) { return NativeInvoke::Invoke<NATIVE_GET_PED_CLIMB_STATE, ePedClimbState>(ped); }
	static void GetPedBonePosition(Ped ped, u32 bone, f32 x, f32 y, f32 z, Vector3 *pPosition) { NativeInvoke::Invoke<NATIVE_GET_PED_BONE_POSITION, ScriptVoid>(ped, bone, x, y, z, pPosition); }
	static void GetPedGroupIndex(Ped ped, u32* pIndex) { NativeInvoke::Invoke<NATIVE_GET_PED_GROUP_INDEX, ScriptVoid>(ped, pIndex); }
	static void GetPedType(Ped ped, ePedType *pType) { NativeInvoke::Invoke<NATIVE_GET_PED_TYPE, ScriptVoid>(ped, pType); }
	static void GivePedHelmet(Ped ped) { NativeInvoke::Invoke<NATIVE_GIVE_PED_HELMET, ScriptVoid>(ped); }
	static void GiveWeaponToChar(Ped ped, u32 weapon, u32 ammo, b8 unknown0) { NativeInvoke::Invoke<NATIVE_GIVE_WEAPON_TO_CHAR, ScriptVoid>(ped, weapon, ammo, unknown0); }
	static b8 HasCharBeenDamagedByCar(Ped ped, Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_HAS_CHAR_BEEN_DAMAGED_BY_CAR, b8>(ped, vehicle); }
	static b8 HasCharBeenDamagedByChar(Ped ped, Ped otherChar, b8 unknownFalse) { return NativeInvoke::Invoke<NATIVE_HAS_CHAR_BEEN_DAMAGED_BY_CHAR, b8>(ped, otherChar, unknownFalse); }
	static b8 HasCharBeenDamagedByWeapon(Ped ped, eWeapon weapon) { return NativeInvoke::Invoke<NATIVE_HAS_CHAR_BEEN_DAMAGED_BY_WEAPON, b8>(ped, weapon); }
	static b8 HasCharGotWeapon(Ped ped, eWeapon weapon) { return NativeInvoke::Invoke<NATIVE_HAS_CHAR_GOT_WEAPON, b8>(ped, weapon); }
	static b8 HasCharSpottedChar(Ped ped, Ped otherChar) { return NativeInvoke::Invoke<NATIVE_HAS_CHAR_SPOTTED_CHAR, b8>(ped, otherChar); }
	static b8 HasCharSpottedCharInFront(Ped ped, Ped otherChar) { return NativeInvoke::Invoke<NATIVE_HAS_CHAR_SPOTTED_CHAR_IN_FRONT, b8>(ped, otherChar); }
	static b8 IsAmbientSpeechPlaying(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_AMBIENT_SPEECH_PLAYING, b8>(ped); }

	static bool IsControlPressed(int padIndex, int key)
	{
		return NativeInvoke::Invoke<NATIVE_IS_CONTROL_PRESSED, b8>(padIndex, key);
	}

	static void GetKeyboardMoveInput(int* moveX, int* moveY)
	{
		NativeInvoke::Invoke<NATIVE_GET_KEYBOARD_MOVE_INPUT, ScriptVoid>(moveX, moveY);
	}

	static b8 IsCharArmed(Ped ped, eWeaponSlot slot) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_ARMED, b8>(ped, slot); }
	static b8 IsCharDead(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_DEAD, b8>(ped); }
	static b8 IsCharDucking(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_DUCKING, b8>(ped); }
	static b8 IsCharFacingChar(Ped ped, Ped otherChar, f32 angle) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_FACING_CHAR, b8>(ped, otherChar, angle); }
	static b8 IsCharFatallyInjured(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_FATALLY_INJURED, b8>(ped); }
	static b8 IsCharGesturing(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_GESTURING, b8>(ped); }
	static b8 IsCharGettingInToACar(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_GETTING_IN_TO_A_CAR, b8>(ped); }
	static b8 IsCharGettingUp(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_GETTING_UP, b8>(ped); }
	static b8 IsCharHealthGreater(Ped ped, u32 health) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_HEALTH_GREATER, b8>(ped, health); }
	static b8 IsCharInAir(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_AIR, b8>(ped); }
	static b8 IsCharInAngledArea2D(Ped ped, f32 x1, f32 y1, f32 x2, f32 y2, f32 unknown, b8 unknownFalse) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_ANGLED_AREA_2D, b8>(ped, x1, y1, x2, y2, unknown, unknownFalse); }
	static b8 IsCharInAngledArea3D(Ped ped, f32 x1, f32 y1, f32 z1, f32 x2, f32 y2, f32 z2, f32 unknown, b8 unknownFalse) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_ANGLED_AREA_3D, b8>(ped, x1, y1, z1, x2, y2, z2, unknown, unknownFalse); }
	static b8 IsCharInAnyBoat(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_ANY_BOAT, b8>(ped); }
	static b8 IsCharInAnyCar(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_ANY_CAR, b8>(ped); }
	static b8 IsCharInAnyHeli(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_ANY_HELI, b8>(ped); }
	static b8 IsCharInAnyPlane(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_ANY_PLANE, b8>(ped); }
	static b8 IsCharInAnyPoliceVehicle(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_ANY_POLICE_VEHICLE, b8>(ped); }
	static b8 IsCharInAnyTrain(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_ANY_TRAIN, b8>(ped); }
	static b8 IsCharInArea2D(Ped ped, f32 x1, f32 y1, f32 x2, f32 y2, b8 unknownFalse) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_AREA_2D, b8>(ped, x1, y1, x2, y2, unknownFalse); }
	static b8 IsCharInArea3D(Ped ped, f32 x1, f32 y1, f32 z1, f32 x2, f32 y2, f32 z2, b8 unknownFalse) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_AREA_3D, b8>(ped, x1, y1, z1, x2, y2, z2, unknownFalse); }
	static b8 IsCharInAreaOnFoot2D(Ped ped, f32 x1, f32 y1, f32 x2, f32 y2, b8 unknownFalse) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_AREA_ON_FOOT_2D, b8>(ped, x1, y1, x2, y2, unknownFalse); }
	static b8 IsCharInCar(Ped ped, Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_CAR, b8>(ped, vehicle); }
	static b8 IsCharInFlyingVehicle(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_FLYING_VEHICLE, b8>(ped); }
	static b8 IsCharInMeleeCombat(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_MELEE_COMBAT, b8>(ped); }
	static b8 IsCharInModel(Ped ped, eModel model) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_MODEL, b8>(ped, model); }
	static b8 IsCharInTaxi(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_TAXI, b8>(ped); }
	static b8 IsCharInWater(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_IN_WATER, b8>(ped); }
	static b8 IsCharInjured(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_INJURED, b8>(ped); }
	static b8 IsCharMale(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_MALE, b8>(ped); }
	static b8 IsCharModel(Ped ped, eModel model) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_MODEL, b8>(ped, model); }
	static b8 IsCharOnAnyBike(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_ON_ANY_BIKE, b8>(ped); }
	static b8 IsCharOnFire(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_ON_FIRE, b8>(ped); }
	static b8 IsCharOnFoot(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_ON_FOOT, b8>(ped); }
	static b8 IsCharOnScreen(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_ON_SCREEN, b8>(ped); }
	static b8 IsCharPlayingAnim(Ped ped, ch *animSet, ch *animName) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_PLAYING_ANIM, b8>(ped, animSet, animName); }
	//static b8 IsCharPlayingAnim(Ped ped, ch *animSet, ch *animName) { return NativeInvoke::Invoke<b8>("IS_CHAR_PLAYING_ANIM", ped, animSet, animName); }
	static b8 IsCharShooting(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_SHOOTING, b8>(ped); }
	static b8 IsCharShootingInArea(Ped ped, f32 x1, f32 y1, f32 x2, f32 y2, b8 unknownFalse) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_SHOOTING_IN_AREA, b8>(ped, x1, y1, x2, y2, unknownFalse); }
	static b8 IsCharSittingIdle(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_SITTING_IDLE, b8>(ped); }
	static b8 IsCharSittingInAnyCar(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_SITTING_IN_ANY_CAR, b8>(ped); }
	static b8 IsCharSittingInCar(Ped ped, Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_SITTING_IN_CAR, b8>(ped, vehicle); }
	static b8 IsCharStopped(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_STOPPED, b8>(ped); }
	static b8 IsCharStuckUnderCar(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_STUCK_UNDER_CAR, b8>(ped); }
	static b8 IsCharSwimming(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_SWIMMING, b8>(ped); }
	static b8 IsCharTouchingChar(Ped ped, Ped otherChar) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_TOUCHING_CHAR, b8>(ped, otherChar); }
	static b8 IsCharTouchingObject(Ped ped, Object obj) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_TOUCHING_OBJECT, b8>(ped, obj); }
	static b8 IsCharTouchingObjectOnFoot(Ped ped, Object obj) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_TOUCHING_OBJECT_ON_FOOT, b8>(ped, obj); }
	static b8 IsCharTouchingVehicle(ScriptAny p0, ScriptAny p1) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_TOUCHING_VEHICLE, b8>(p0, p1); }
	static b8 IsCharTryingToEnterALockedCar(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_TRYING_TO_ENTER_A_LOCKED_CAR, b8>(ped); }
	static b8 IsCharUsingAnyScenario(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_USING_ANY_SCENARIO, b8>(ped); }
	static b8 IsCharUsingScenario(Ped ped, const ch *scenarioName) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_USING_SCENARIO, b8>(ped, scenarioName); }
	static b8 IsCharVisible(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_VISIBLE, b8>(ped); }
	static b8 IsCharWaitingForWorldCollision(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_CHAR_WAITING_FOR_WORLD_COLLISION, b8>(ped); }
	static b8 IsPedAMissionPed(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_A_MISSION_PED, b8>(ped); }
	static b8 IsPedAttachedToAnyCar(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_ATTACHED_TO_ANY_CAR, b8>(ped); }
	static b8 IsPedAttachedToObject(Ped ped, Object obj) { return NativeInvoke::Invoke<NATIVE_IS_PED_ATTACHED_TO_OBJECT, b8>(ped, obj); }
	static b8 IsPedBeingJacked(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_BEING_JACKED, b8>(ped); }
	static b8 IsPedDoingDriveby(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_DOING_DRIVEBY, b8>(ped); }
	static b8 IsPedFleeing(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_FLEEING, b8>(ped); }
	static b8 IsPedHoldingAnObject(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_HOLDING_AN_OBJECT, b8>(ped); }
	static b8 IsPedInCombat(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_IN_COMBAT, b8>(ped); }
	static b8 IsPedInCover(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_IN_COVER, b8>(ped); }
	static b8 IsPedInGroup(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_IN_GROUP, b8>(ped); }
	static b8 IsPedJacking(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_JACKING, b8>(ped); }
	static b8 IsPedLookingAtPed(Ped ped, Ped otherChar) { return NativeInvoke::Invoke<NATIVE_IS_PED_LOOKING_AT_PED, b8>(ped, otherChar); }
	static b8 IsPedRagdoll(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_RAGDOLL, b8>(ped); }
	static b8 IsPedRetreating(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_RETREATING, b8>(ped); }
	static b8 IsPedClimbing(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_PED_CLIMBING, b8>(ped); }
	static b8 IsScriptedSpeechPlaying(Ped ped) { return NativeInvoke::Invoke<NATIVE_IS_SCRIPTED_SPEECH_PLAYING, b8>(ped); }
	static void MarkCharAsNoLongerNeeded(Ped *pPed) { NativeInvoke::Invoke<NATIVE_MARK_CHAR_AS_NO_LONGER_NEEDED, ScriptVoid>(pPed); }
	static void ModifyCharMoveState(Ped ped, ePedMoveState state) { NativeInvoke::Invoke<NATIVE_MODIFY_CHAR_MOVE_STATE, ScriptVoid>(ped, state); }
	static void RemoveAllCharWeapons(Ped ped) { NativeInvoke::Invoke<NATIVE_REMOVE_ALL_CHAR_WEAPONS, ScriptVoid>(ped); }
	static void RemoveCharDefensiveArea(Ped ped) { NativeInvoke::Invoke<NATIVE_REMOVE_CHAR_DEFENSIVE_AREA, ScriptVoid>(ped); }
	static void RemoveCharElegantly(Ped ped) { NativeInvoke::Invoke<NATIVE_REMOVE_CHAR_ELEGANTLY, ScriptVoid>(ped); }
	static void RemoveCharFromGroup(Ped ped) { NativeInvoke::Invoke<NATIVE_REMOVE_CHAR_FROM_GROUP, ScriptVoid>(ped); }
	static void ReviveInjuredPed(Ped ped) { NativeInvoke::Invoke<NATIVE_REVIVE_INJURED_PED, ScriptVoid>(ped); }
	static void SayAmbientSpeech(Ped ped, ch *phraseName, ScriptAny unknown0_1, ScriptAny unknown1_1, ScriptAny unknown2_0) { NativeInvoke::Invoke<NATIVE_SAY_AMBIENT_SPEECH, ScriptVoid>(ped, phraseName, unknown0_1, unknown1_1, unknown2_0); }
	static ScriptAny SetAmmoInClip(Ped ped, u32 weapon, u32 ammo) { return NativeInvoke::Invoke<NATIVE_SET_AMMO_IN_CLIP, ScriptAny>(ped, weapon, ammo); }
	static void SetBlockingOfNonTemporaryEvents(Ped ped, bool value) { NativeInvoke::Invoke<NATIVE_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS, ScriptVoid>(ped, value); }
	static void SetCharAccuracy(Ped ped, u32 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_ACCURACY, ScriptVoid>(ped, value); }
	static void SetCharShootRate(Ped ped, u32 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_SHOOT_RATE, ScriptVoid>(ped, value); }
	static void SetCharAmmo(Ped ped, u32 weapon, u32 ammo) { NativeInvoke::Invoke<NATIVE_SET_CHAR_AMMO, ScriptVoid>(ped, weapon, ammo); }
	static void SetCharAsEnemy(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_AS_ENEMY, ScriptVoid>(ped, value); }
	static void SetCharAsMissionChar(Ped ped) { NativeInvoke::Invoke<NATIVE_SET_CHAR_AS_MISSION_CHAR, ScriptVoid>(ped); }
	static void SetCharCanBeKnockedOffBike(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_CAN_BE_KNOCKED_OFF_BIKE, ScriptVoid>(ped, value); }
	static void SetCharCanBeShotInVehicle(Ped ped, b8 enabled) { NativeInvoke::Invoke<NATIVE_SET_CHAR_CAN_BE_SHOT_IN_VEHICLE, ScriptVoid>(ped, enabled); }
	static void SetCharCantBeDraggedOut(Ped ped, b8 enabled) { NativeInvoke::Invoke<NATIVE_SET_CHAR_CANT_BE_DRAGGED_OUT, ScriptVoid>(ped, enabled); }
	static void SetCharComponentVariation(Ped ped, u32 component, u32 modelVariation, u32 textureVariation) { NativeInvoke::Invoke<NATIVE_SET_CHAR_COMPONENT_VARIATION, ScriptVoid>(ped, component, modelVariation, textureVariation); }
	static void SetCharCoordinates(Ped ped, f32 x, f32 y, f32 z) { NativeInvoke::Invoke<NATIVE_SET_CHAR_COORDINATES, ScriptVoid>(ped, x, y, z); }
	static void SetCharCollision(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_COLLISION, ScriptVoid>(ped, value); }
	static void SetCharDefaultComponentVariation(Ped ped) { NativeInvoke::Invoke<NATIVE_SET_CHAR_DEFAULT_COMPONENT_VARIATION, ScriptVoid>(ped); }
	static void SetCharDesiredHeading(Ped ped, f32 heading) { NativeInvoke::Invoke<NATIVE_SET_CHAR_DESIRED_HEADING, ScriptVoid>(ped, heading); }
	static void SetCharDropsWeaponsWhenDead(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_DROPS_WEAPONS_WHEN_DEAD, ScriptVoid>(ped, value); }
	static void SetCharDruggedUp(Ped ped, b8 drugged) { NativeInvoke::Invoke<NATIVE_SET_CHAR_DRUGGED_UP, ScriptVoid>(ped, drugged); }
	static void SetCharFireDamageMultiplier(Ped ped, f32 multiplier) { NativeInvoke::Invoke<NATIVE_SET_CHAR_FIRE_DAMAGE_MULTIPLIER, ScriptVoid>(ped, multiplier); }
	static void SetCharGravity(Ped ped, f32 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_GRAVITY, ScriptVoid>(ped, value); }
	static void SetCharHeading(Ped ped, f32 heading) { NativeInvoke::Invoke<NATIVE_SET_CHAR_HEADING, ScriptVoid>(ped, heading); }
	static void SetCharHealth(Ped ped, u32 health) { NativeInvoke::Invoke<NATIVE_SET_CHAR_HEALTH, ScriptVoid>(ped, health); }
	static void SetCharInvincible(Ped ped, b8 enable) { NativeInvoke::Invoke<NATIVE_SET_CHAR_INVINCIBLE, ScriptVoid>(ped, enable); }
	static void SetCharIsTargetPriority(Ped ped, b8 enable) { NativeInvoke::Invoke<NATIVE_SET_CHAR_IS_TARGET_PRIORITY, ScriptVoid>(ped, enable); }
	static void SetCharMaxHealth(Ped ped, u32 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_MAX_HEALTH, ScriptVoid>(ped, value); }
	static void SetCharMoney(Ped ped, int amount) { NativeInvoke::Invoke<NATIVE_SET_CHAR_MONEY, ScriptVoid>(ped, amount); }
	static void SetCharNeverLeavesGroup(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_NEVER_LEAVES_GROUP, ScriptVoid>(ped, value); }
	static void SetCharProofs(Ped ped, b8 unknown0, b8 fallingDamage, b8 unknown1, b8 unknown2, b8 unknown3) { NativeInvoke::Invoke<NATIVE_SET_CHAR_PROOFS, ScriptVoid>(ped, unknown0, fallingDamage, unknown1, unknown2, unknown3); }
	static void SetCharPropIndex(Ped ped, u32 propType, u32 index) { NativeInvoke::Invoke<NATIVE_SET_CHAR_PROP_INDEX, ScriptVoid>(ped, propType, index); }
	static void SetCharRandomComponentVariation(Ped ped) { NativeInvoke::Invoke<NATIVE_SET_CHAR_RANDOM_COMPONENT_VARIATION, ScriptVoid>(ped); }
	static void SetCharSphereDefensiveArea(Ped ped, f32 x, f32 y, f32 z, f32 radius) { NativeInvoke::Invoke<NATIVE_SET_CHAR_SPHERE_DEFENSIVE_AREA, ScriptVoid>(ped, x, y, z, radius); }
	static void SetCharSuffersCriticalHits(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_SUFFERS_CRITICAL_HITS, ScriptVoid>(ped, value); }
	static void SetCharVelocity(Ped ped, f32 x, f32 y, f32 z) { NativeInvoke::Invoke<NATIVE_SET_CHAR_VELOCITY, ScriptVoid>(ped, x, y, z); }
	static void SetCharVisible(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_VISIBLE, ScriptVoid>(ped, value); }
	static void SetCharWantedByPolice(Ped ped, b8 wanted) { NativeInvoke::Invoke<NATIVE_SET_CHAR_WANTED_BY_POLICE, ScriptVoid>(ped, wanted); }
	static void SetCharWillDoDrivebys(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_WILL_DO_DRIVEBYS, ScriptVoid>(ped, value); }
	static void SetDontActivateRagdollFromPlayerImpact(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_DONT_ACTIVATE_RAGDOLL_FROM_PLAYER_IMPACT, ScriptVoid>(ped, value); }
	static void SetPedDontDoEvasiveDives(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PED_DONT_DO_EVASIVE_DIVES, ScriptVoid>(ped, value); }
	static void SetCharForceDieInCar(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_FORCE_DIE_IN_CAR, ScriptVoid>(ped, value); }
	static void SetCharGetOutUpsideDownCar(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_GET_OUT_UPSIDE_DOWN_CAR, ScriptVoid>(ped, value); }
	static void SetCharWillFlyThroughWindscreen(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_WILL_FLY_THROUGH_WINDSCREEN, ScriptVoid>(ped, value); }
	static void SetCharWillMoveWhenInjured(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_WILL_MOVE_WHEN_INJURED, ScriptVoid>(ped, value); }
	static void SetCharWillUseCarsInCombat(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_WILL_USE_CARS_IN_COMBAT, ScriptVoid>(ped, value); }
	static void SetCharWillUseCover(Ped ped, b8 coverPoint) { NativeInvoke::Invoke<NATIVE_SET_CHAR_WILL_USE_COVER, ScriptVoid>(ped, coverPoint); }
	static void SetCurrentCharWeapon(Ped ped, eWeapon w, b8 unknownTrue) { NativeInvoke::Invoke<NATIVE_SET_CURRENT_CHAR_WEAPON, ScriptVoid>(ped, w, unknownTrue); }
	static void SetGroupCharDucksWhenAimedAt(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_GROUP_CHAR_DUCKS_WHEN_AIMED_AT, ScriptVoid>(ped, value); }
	static void SetIgnoreLowPriorityShockingEvents(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_IGNORE_LOW_PRIORITY_SHOCKING_EVENTS, ScriptVoid>(ped, value); }
	static void SetPedDiesWhenInjured(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PED_DIES_WHEN_INJURED, ScriptVoid>(ped, value); }
	static void SetPedAlpha(Ped ped, float value) { NativeInvoke::Invoke<NATIVE_SET_PED_ALPHA, ScriptVoid>(ped, value); }
	static void SetPedIsBlindRaging(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PED_IS_BLIND_RAGING, ScriptVoid>(ped, value); }
	static void SetPedIsDrunk(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PED_IS_DRUNK, ScriptVoid>(ped, value); }
	static void SetPedPathMayDropFromHeight(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PED_PATH_MAY_DROP_FROM_HEIGHT, ScriptVoid>(ped, value); }
	static void SetPedPathMayUseClimbovers(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PED_PATH_MAY_USE_CLIMBOVERS, ScriptVoid>(ped, value); }
	static void SetPedPathMayUseLadders(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_PED_PATH_MAY_USE_LADDERS, ScriptVoid>(ped, value); }
	static void SetRoomForCharByKey(Ped ped, u32 key) { NativeInvoke::Invoke<NATIVE_SET_ROOM_FOR_CHAR_BY_KEY, ScriptVoid>(ped, key); }
	static void SetSenseRange(Ped ped, f32 value) { NativeInvoke::Invoke<NATIVE_SET_SENSE_RANGE, ScriptVoid>(ped, value); }
	static void SwitchPedToAnimated(Ped ped, b8 unknownTrue) { NativeInvoke::Invoke<NATIVE_SWITCH_PED_TO_ANIMATED, ScriptVoid>(ped, unknownTrue); }
	static void SwitchPedToRagdoll(Ped ped, int p1, int p2, int p3, int p4, int p5, int p6) { NativeInvoke::Invoke<NATIVE_SWITCH_PED_TO_RAGDOLL, ScriptAny>(ped, p1, p2, p3, p4, p5, p6); }
	static void RemovePedHelmet(Ped ped, b8 removed) { NativeInvoke::Invoke<NATIVE_REMOVE_PED_HELMET, ScriptVoid>(ped, removed); }
	static void RemoveWeaponFromChar(Ped ped, eWeapon weapon) { NativeInvoke::Invoke<NATIVE_REMOVE_WEAPON_FROM_CHAR, ScriptVoid>(ped, weapon); }
	static void UnlockRagdoll(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_UNLOCK_RAGDOLL, ScriptVoid>(ped, value); }
	static void WarpCharFromCarToCoord(Ped ped, f32 x, f32 y, f32 z) { NativeInvoke::Invoke<NATIVE_WARP_CHAR_FROM_CAR_TO_COORD, ScriptVoid>(ped, x, y, z); }
	static void WarpCharIntoCar(Ped ped, Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_WARP_CHAR_INTO_CAR, ScriptVoid>(ped, vehicle); }
	static void WarpCharIntoCarAsPassenger(Ped ped, Vehicle vehicle, u32 seatIndex) { NativeInvoke::Invoke<NATIVE_WARP_CHAR_INTO_CAR_AS_PASSENGER, ScriptVoid>(ped, vehicle, seatIndex); }
	static void WarpCharFromCarToCar(Ped ped, Vehicle vehicle, u32 seatIndex) { NativeInvoke::Invoke<NATIVE_WARP_CHAR_FROM_CAR_TO_CAR, ScriptVoid>(ped, vehicle, seatIndex); }


	// Ped Tasks
	static void SetCharKeepTask(Ped ped, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CHAR_KEEP_TASK, ScriptVoid>(ped, value); }
	static void SetDriveTaskCruiseSpeed(Ped ped, f32 speed) { NativeInvoke::Invoke<NATIVE_SET_DRIVE_TASK_CRUISE_SPEED, ScriptVoid>(ped, speed); }
	static void TaskAchieveHeading(Ped ped, f32 heading) { NativeInvoke::Invoke<NATIVE_TASK_ACHIEVE_HEADING, ScriptVoid>(ped, heading); }
	static void TaskAimGunAtChar(Ped ped, Ped targetPed, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_AIM_GUN_AT_CHAR, ScriptVoid>(ped, targetPed, duration); }
	static void TaskAimGunAtCoord(Ped ped, f32 tX, f32 tY, f32 tZ, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_AIM_GUN_AT_COORD, ScriptVoid>(ped, tX, tY, tZ, duration); }
	static void TaskCarDriveWander(Ped ped, Vehicle vehicle, f32 speed, eVehicleDrivingStyle drivingStyle) { NativeInvoke::Invoke<NATIVE_TASK_CAR_DRIVE_WANDER, ScriptVoid>(ped, vehicle, speed, drivingStyle); }
	static void TaskCarMission(Ped ped, Vehicle vehicle, ScriptHandle targetEntity, u32 missionType, f32 speed, eVehicleDrivingStyle drivingStyle, u32 unknown6_10, u32 unknown7_5) { NativeInvoke::Invoke<NATIVE_TASK_CAR_MISSION, ScriptVoid>(ped, vehicle, targetEntity, missionType, speed, drivingStyle, unknown6_10, unknown7_5); } // target is whatever missiontype requires (ie. vehicle or just 0). missiontypes: 5=wait(), 21=drivetoplayer()
	static void TaskCarMissionNotAgainstTraffic(Ped ped, Vehicle vehicle, ScriptHandle targetEntity, u32 missionType, f32 speed, eVehicleDrivingStyle  drivingStyle, u32 unknown6_10, u32 unknown7_5) { NativeInvoke::Invoke<NATIVE_TASK_CAR_MISSION_NOT_AGAINST_TRAFFIC, ScriptVoid>(ped, vehicle, targetEntity, missionType, speed, drivingStyle, unknown6_10, unknown7_5); }
	static void TaskCarMissionCoorsTarget(Ped ped, Vehicle vehicle, f32 x, f32 y, f32 z, u32 unknown0_4, f32 speed, u32 unknown2_1, u32 unknown3_5, u32 unknown4_10) { NativeInvoke::Invoke<NATIVE_TASK_CAR_MISSION_COORS_TARGET, ScriptVoid>(ped, vehicle, x, y, z, unknown0_4, speed, unknown2_1, unknown3_5, unknown4_10); }
	static void TaskCarMissionCoorsTargetNotAgainstTraffic(Ped ped, Vehicle vehicle, f32 x, f32 y, f32 z, u32 unknown0_4, f32 speed, u32 unknown2_1, u32 unknown3_5, u32 unknown4_10) { NativeInvoke::Invoke<NATIVE_TASK_CAR_MISSION_COORS_TARGET_NOT_AGAINST_TRAFFIC, ScriptVoid>(ped, vehicle, x, y, z, unknown0_4, speed, unknown2_1, unknown3_5, unknown4_10); }
	static void TaskCarMissionPedTarget(Ped ped, Vehicle vehicle, Ped target, u32 unknown0_4, f32 speed, u32 unknown2_1, u32 unknown3_5, u32 unknown4_10) { NativeInvoke::Invoke<NATIVE_TASK_CAR_MISSION_PED_TARGET, ScriptVoid>(ped, vehicle, target, unknown0_4, speed, unknown2_1, unknown3_5, unknown4_10); }
	static void TaskCarTempAction(Ped ped, Vehicle vehicle, u32 action, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_CAR_TEMP_ACTION, ScriptVoid>(ped, vehicle, action, duration); } // action 6 = wait?
	static void TaskCombat(Ped ped, Ped target) { NativeInvoke::Invoke<NATIVE_TASK_COMBAT, ScriptVoid>(ped, target); }
	static void TaskCombatHatedTargetsAroundChar(Ped ped, f32 radius) { NativeInvoke::Invoke<NATIVE_TASK_COMBAT_HATED_TARGETS_AROUND_CHAR, ScriptVoid>(ped, radius); }
	static void TaskCombatHatedTargetsAroundCharTimed(Ped ped, f32 radius, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_COMBAT_HATED_TARGETS_AROUND_CHAR_TIMED, ScriptVoid>(ped, radius, duration); }
	static void TaskCombatTimed(Ped ped, Char target, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_COMBAT_TIMED, ScriptVoid>(ped, target, duration); }
	static void TaskDie(Ped ped) { NativeInvoke::Invoke<NATIVE_TASK_DIE, ScriptVoid>(ped); }
	static void TaskEnterCarAsDriver(Ped ped, Vehicle vehicle, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_ENTER_CAR_AS_DRIVER, ScriptVoid>(ped, vehicle, duration); }
	static void TaskEnterCarAsPassenger(Ped ped, Vehicle vehicle, u32 duration, u32 seatIndex) { NativeInvoke::Invoke<NATIVE_TASK_ENTER_CAR_AS_PASSENGER, ScriptVoid>(ped, vehicle, duration, seatIndex); }
	static void TaskEveryoneLeaveCar(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_TASK_EVERYONE_LEAVE_CAR, ScriptVoid>(vehicle); }
	static void TaskFollowNavMeshToCoord(Ped ped, f32 x, f32 y, f32 z, u32 unknown0_2, u32 unknown1_minus1, f32 unknown2_1) { NativeInvoke::Invoke<NATIVE_TASK_FOLLOW_NAV_MESH_TO_COORD, ScriptVoid>(ped, x, y, z, unknown0_2, unknown1_minus1, unknown2_1); }

	static void TaskFollowNavMeshToCoordNoStop(Ped ped, f32 x, f32 y, f32 z, int moveType, u32 unknown1_minus1, f32 unknown2_1)
	{
		NativeInvoke::Invoke<NATIVE_TASK_FOLLOW_NAV_MESH_TO_COORD_NO_STOP, ScriptVoid>(ped, x, y, z, moveType, unknown1_minus1, unknown2_1);
	}

	static void TaskGoStraightToCoord(Ped ped, float x, float y, float z, int moveType, int unk) { NativeInvoke::Invoke<NATIVE_TASK_GO_STRAIGHT_TO_COORD, ScriptVoid>(ped, x, y, z, moveType, unk); }
	static void TaskGotoCoordWhileAiming(Ped ped, float x, float y, float z, int u1, int u2, int u3, int u4, float ax, float ay, float az, int u5) { NativeInvoke::Invoke<NATIVE_TASK_GO_TO_COORD_WHILE_AIMING, ScriptVoid>(ped, x, y, z, u1, u2, u3, u4, ax, ay, az, u5); }

	static void TaskGotoCharOffset(Ped ped, Ped target, u32 duration, f32 offsetRight, f32 offsetFront) { NativeInvoke::Invoke<NATIVE_TASK_GOTO_CHAR_OFFSET, ScriptVoid>(ped, target, duration, offsetRight, offsetFront); }
	static void TaskGuardCurrentPosition(Ped ped, f32 unknown0_15, f32 unknown1_10, u32 unknown2_1) { NativeInvoke::Invoke<NATIVE_TASK_GUARD_CURRENT_POSITION, ScriptVoid>(ped, unknown0_15, unknown1_10, unknown2_1); }
	static void TaskHandsUp(Ped ped, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_HANDS_UP, ScriptVoid>(ped, duration); }
	static void TaskHeliMission(Ped ped, Vehicle heli, u32 uk0_0, u32 uk1_0, f32 pX, f32 pY, f32 pZ, u32 uk2_4, f32 speed, u32 uk3_5, f32 uk4_minus1, u32 uk5_round_z_plus_1, u32 uk6_40) { NativeInvoke::Invoke<NATIVE_TASK_HELI_MISSION, ScriptVoid>(ped, heli, uk0_0, uk1_0, pX, pY, pZ, uk2_4, speed, uk3_5, uk4_minus1, uk5_round_z_plus_1, uk6_40); }
	static void TaskLeaveAnyCar(Ped ped) { NativeInvoke::Invoke<NATIVE_TASK_LEAVE_ANY_CAR, ScriptVoid>(ped); }
	static void TaskLeaveCar(Ped ped, Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_TASK_LEAVE_CAR, ScriptVoid>(ped, vehicle); }
	static void TaskLeaveCarDontCloseDoor(Ped ped, Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_TASK_LEAVE_CAR_DONT_CLOSE_DOOR, ScriptVoid>(ped, vehicle); }
	static void TaskLeaveCarImmediately(Ped ped, Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_TASK_LEAVE_CAR_IMMEDIATELY, ScriptVoid>(ped, vehicle); }
	static void TaskLookAtChar(Ped ped, Ped targetPed, u32 duration, u32 unknown_0) { NativeInvoke::Invoke<NATIVE_TASK_LOOK_AT_CHAR, ScriptVoid>(ped, targetPed, duration, unknown_0); }
	static void TaskLookAtCoord(Ped ped, f32 x, f32 y, f32 z, u32 duration, u32 unknown_0) { NativeInvoke::Invoke<NATIVE_TASK_LOOK_AT_COORD, ScriptVoid>(ped, x, y, z, duration, unknown_0); }
	static void TaskLookAtObject(Ped ped, Object targetObject, u32 duration, u32 unknown_0) { NativeInvoke::Invoke<NATIVE_TASK_LOOK_AT_OBJECT, ScriptVoid>(ped, targetObject, duration, unknown_0); }
	static void TaskLookAtVehicle(Ped ped, Vehicle targetVehicle, u32 duration, u32 unknown_0) { NativeInvoke::Invoke<NATIVE_TASK_LOOK_AT_VEHICLE, ScriptVoid>(ped, targetVehicle, duration, unknown_0); }
	static void TaskOpenDriverDoor(Ped ped, Vehicle vehicle, u32 unknown0) { NativeInvoke::Invoke<NATIVE_TASK_OPEN_DRIVER_DOOR, ScriptVoid>(ped, vehicle, unknown0); }
	static void TaskOpenPassengerDoor(Ped ped, Vehicle vehicle, u32 seatIndex, u32 unknown0) { NativeInvoke::Invoke<NATIVE_TASK_OPEN_PASSENGER_DOOR, ScriptVoid>(ped, vehicle, seatIndex, unknown0); }
	static void TaskPause(Ped ped, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_PAUSE, ScriptVoid>(ped, duration); }
	static void TaskPlayAnimWithFlags(Ped ped, const ch *animName, const ch *animSet, f32 unknown0_8, u32 unknown1_0, u32 flags) { NativeInvoke::Invoke<NATIVE_TASK_PLAY_ANIM_WITH_FLAGS, ScriptVoid>(ped, animName, animSet, unknown0_8, unknown1_0, flags); }

	static void TaskPlayAnim(Ped ped, const ch *animName, const ch *animSet, float speed, bool loop, float x, float y, float z, int time) { NativeInvoke::Invoke<NATIVE_TASK_PLAY_ANIM, ScriptVoid>(ped, animName, animSet, speed, loop, x, y, z, time); }
	//static void TaskPlayAnim(Ped ped, const ch *animName, const ch *animSet, float speed, bool loop, float x, float y, float z, int time) { NativeInvoke::Invoke<ScriptVoid>("TASK_PLAY_ANIM", ped, animName, animSet, speed, loop, x, y, z, time); }
	static void TaskPlayAnimWithAdvancedFlags(Ped ped, const ch *animName, const ch *animSet, float speed, bool loop, bool camera, bool sCamera, bool returnToPos, bool dontReturn, bool sCameraTwo, bool rCamera, int playback)
	{
		NativeInvoke::Invoke<NATIVE_TASK_PLAY_ANIM, ScriptVoid>(ped, animName, animSet, speed, loop, camera, sCamera, returnToPos, dontReturn, rCamera, playback);
	}

	static void TaskPlayAnimNonInterruptable(Ped ped, const ch *animName, const ch *animSet, float speed, bool loop, bool loop2, bool returnToPos, bool freezeFrame)
	{
		NativeInvoke::Invoke<NATIVE_TASK_PLAY_ANIM_NON_INTERRUPTABLE, ScriptVoid>(
			ped, animName, animSet, speed, loop, loop2, returnToPos, freezeFrame, -1);
	}

	static void TaskSetCharDecisionMaker(Ped ped, DecisionMaker dm) { NativeInvoke::Invoke<NATIVE_TASK_SET_CHAR_DECISION_MAKER, ScriptVoid>(ped, dm); }
	static void TaskSetCombatDecisionMaker(Ped ped, DecisionMaker dm) { NativeInvoke::Invoke<NATIVE_TASK_SET_COMBAT_DECISION_MAKER, ScriptVoid>(ped, dm); }
	static void TaskSmartFleeChar(Ped ped, Ped fleeFromPed, f32 unknown0_100, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_SMART_FLEE_CHAR, ScriptVoid>(ped, fleeFromPed, unknown0_100, duration); }
	static void TaskSmartFleeCharPreferringPavements(Ped ped, Ped fleeFromPed, f32 unknown0_100, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_SMART_FLEE_CHAR_PREFERRING_PAVEMENTS, ScriptVoid>(ped, fleeFromPed, unknown0_100, duration); }
	static void TaskSmartFleePoint(Ped ped, f32 x, f32 y, f32 z, f32 unknown0_100, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_SMART_FLEE_POINT, ScriptVoid>(ped, x, y, x, unknown0_100, duration); }
	static void TaskStandStill(Ped ped, int duration) { NativeInvoke::Invoke<NATIVE_TASK_STAND_STILL, ScriptVoid>(ped, duration); }
	static void TaskSwapWeapon(Ped ped, eWeapon weapon) { NativeInvoke::Invoke<NATIVE_TASK_SWAP_WEAPON, ScriptVoid>(ped, weapon); }
	static void TaskTurnCharToFaceChar(Ped ped, Ped targetPed) { NativeInvoke::Invoke<NATIVE_TASK_TURN_CHAR_TO_FACE_CHAR, ScriptVoid>(ped, targetPed); }
	static void TaskTurnCharToFaceCoord(Ped ped, f32 x, f32 y, f32 z) { NativeInvoke::Invoke<NATIVE_TASK_TURN_CHAR_TO_FACE_COORD, ScriptVoid>(ped, x, y, z); }
	static void TaskUseMobilePhone(Ped ped, b8 use) { NativeInvoke::Invoke<NATIVE_TASK_USE_MOBILE_PHONE, ScriptVoid>(ped, use); }
	static void TaskUseMobilePhoneTimed(Ped ped, u32 duration) { NativeInvoke::Invoke<NATIVE_TASK_USE_MOBILE_PHONE_TIMED, ScriptVoid>(ped, duration); }
	static void TaskWanderStandard(Ped ped) { NativeInvoke::Invoke<NATIVE_TASK_WANDER_STANDARD, ScriptVoid>(ped); }
	static void TaskWarpCharIntoCarAsDriver(Ped ped, Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_TASK_WARP_CHAR_INTO_CAR_AS_DRIVER, ScriptVoid>(ped, vehicle); }
	static void TaskWarpCharIntoCarAsPassenger(Ped ped, Vehicle vehicle, u32 seatIndex) { NativeInvoke::Invoke<NATIVE_TASK_WARP_CHAR_INTO_CAR_AS_PASSENGER, ScriptVoid>(ped, vehicle, seatIndex); }

	// Task Sequence
	static void OpenSequenceTask(TaskSequence *pTaskSequence) { NativeInvoke::Invoke<NATIVE_OPEN_SEQUENCE_TASK, ScriptVoid>(pTaskSequence); }
	static void CloseSequenceTask(TaskSequence taskSequence) { NativeInvoke::Invoke<NATIVE_CLOSE_SEQUENCE_TASK, ScriptVoid>(taskSequence); }
	static void TaskPerformSequence(Ped ped, TaskSequence taskSequence) { NativeInvoke::Invoke<NATIVE_TASK_PERFORM_SEQUENCE, ScriptVoid>(ped, taskSequence); }
	static void ClearSequenceTask(TaskSequence taskSequence) { NativeInvoke::Invoke<NATIVE_CLEAR_SEQUENCE_TASK, ScriptVoid>(taskSequence); }

	// Ped Search
	static void AllowScenarioPedsToBeReturnedByNextCommand(b8 value) { NativeInvoke::Invoke<NATIVE_ALLOW_SCENARIO_PEDS_TO_BE_RETURNED_BY_NEXT_COMMAND, ScriptVoid>(value); }
	static void BeginCharSearchCriteria() { NativeInvoke::Invoke<NATIVE_BEGIN_CHAR_SEARCH_CRITERIA, ScriptVoid>(); }
	static void EndCharSearchCriteria() { NativeInvoke::Invoke<NATIVE_END_CHAR_SEARCH_CRITERIA, ScriptVoid>(); }
	static b8 GetClosestChar(f32 x, f32 y, f32 z, f32 radius, u32 unknown1, u32 unknown2, Ped *pPed) { return NativeInvoke::Invoke<NATIVE_GET_CLOSEST_CHAR, b8>(x, y, z, radius, unknown1, unknown2, pPed); }
	static void GetRandomCharInAreaOffsetNoSave(f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, Ped *pPed) { NativeInvoke::Invoke<NATIVE_GET_RANDOM_CHAR_IN_AREA_OFFSET_NO_SAVE, ScriptVoid>(x, y, z, sx, sy, sz, pPed); }
	static void SearchCriteriaConsiderPedsWithFlagTrue(u32 flagId) { NativeInvoke::Invoke<NATIVE_SEARCH_CRITERIA_CONSIDER_PEDS_WITH_FLAG_TRUE, ScriptVoid>(flagId); }
	static void SearchCriteriaRejectPedsWithFlagTrue(u32 flagId) { NativeInvoke::Invoke<NATIVE_SEARCH_CRITERIA_REJECT_PEDS_WITH_FLAG_TRUE, ScriptVoid>(flagId); }

	// Ped Groups
	static void CreateGroup(b8 unknownFalse, Group *pGroup, b8 unknownTrue) { NativeInvoke::Invoke<NATIVE_CREATE_GROUP, ScriptVoid>(unknownFalse, pGroup, unknownTrue); }
	static b8 DoesGroupExist(Group group) { return NativeInvoke::Invoke<NATIVE_DOES_GROUP_EXIST, b8>(group); }
	static void GetGroupLeader(Group group, Ped *pPed) { NativeInvoke::Invoke<NATIVE_GET_GROUP_LEADER, ScriptVoid>(group, pPed); }
	static void GetGroupMember(Group group, u32 index, Ped *pPed) { NativeInvoke::Invoke<NATIVE_GET_GROUP_MEMBER, ScriptVoid>(group, index, pPed); }
	static void GetGroupSize(Group group, u32 *pStartIndex, u32 *pCount) { NativeInvoke::Invoke<NATIVE_GET_GROUP_SIZE, ScriptVoid>(group, pStartIndex, pCount); }
	static b8 IsGroupLeader(Ped ped, Group group) { return NativeInvoke::Invoke<NATIVE_IS_GROUP_LEADER, b8>(ped, group); }
	static b8 IsGroupMember(Ped ped, Group g) { return NativeInvoke::Invoke<NATIVE_IS_GROUP_MEMBER, b8>(ped, g); }
	static void RemoveGroup(Group group) { NativeInvoke::Invoke<NATIVE_REMOVE_GROUP, ScriptVoid>(group); }
	static void SetGroupFollowStatus(Group group, u32 status) { NativeInvoke::Invoke<NATIVE_SET_GROUP_FOLLOW_STATUS, ScriptVoid>(group, status); }
	static void SetGroupFormation(Group group, u32 formation) { NativeInvoke::Invoke<NATIVE_SET_GROUP_FORMATION, ScriptVoid>(group, formation); }
	static void SetGroupFormationSpacing(Group group, f32 space) { NativeInvoke::Invoke<NATIVE_SET_GROUP_FORMATION_SPACING, ScriptVoid>(group, space); }
	static void SetGroupLeader(Group group, Ped leader) { NativeInvoke::Invoke<NATIVE_SET_GROUP_LEADER, ScriptVoid>(group, leader); }
	static void SetGroupMember(Group group, Ped member) { NativeInvoke::Invoke<NATIVE_SET_GROUP_MEMBER, ScriptVoid>(group, member); }
	static void SetGroupSeparationRange(Group group, f32 seperation) { NativeInvoke::Invoke<NATIVE_SET_GROUP_SEPARATION_RANGE, ScriptVoid>(group, seperation); }

	// Ped Relationships
	static void AllowGangRelationshipsToBeChangedByNextCommand(b8 value) { NativeInvoke::Invoke<NATIVE_ALLOW_GANG_RELATIONSHIPS_TO_BE_CHANGED_BY_NEXT_COMMAND, ScriptVoid>(value); }
	static void SetCharNotDamagedByRelationshipGroup(Ped ped, u32 relationshipGroup, b8 enable) { NativeInvoke::Invoke<NATIVE_SET_CHAR_NOT_DAMAGED_BY_RELATIONSHIP_GROUP, ScriptVoid>(ped, relationshipGroup, enable); }
	static void SetCharRelationship(Ped ped, u32 relationshipLevel, u32 relationshipGroup) { NativeInvoke::Invoke<NATIVE_SET_CHAR_RELATIONSHIP, ScriptVoid>(ped, relationshipLevel, relationshipGroup); }
	static void SetCharRelationshipGroup(Ped ped, u32 relationshipGroup) { NativeInvoke::Invoke<NATIVE_SET_CHAR_RELATIONSHIP_GROUP, ScriptVoid>(ped, relationshipGroup); }
	static void SetRelationship(u32 relationshipLevel, u32 relationshipGroup1, u32 relationshipGroup2) { NativeInvoke::Invoke<NATIVE_SET_RELATIONSHIP, ScriptVoid>(relationshipLevel, relationshipGroup1, relationshipGroup2); }

	// Vehicle
	static void AddUpsideDownCarCheck(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_ADD_UPSIDEDOWN_CAR_CHECK, ScriptVoid>(vehicle); }
	static void AnchorBoat(Vehicle boat, b8 anchor) { NativeInvoke::Invoke<NATIVE_ANCHOR_BOAT, ScriptVoid>(boat, anchor); }
	static void ApplyForceToCar(Vehicle vehicle, u32 unknown0_3, f32 x, f32 y, f32 z, f32 spinX, f32 spinY, f32 spinZ, u32 unknown4_0, u32 unknown5_1, u32 unknown6_1, u32 unknown7_1) { NativeInvoke::Invoke<NATIVE_APPLY_FORCE_TO_CAR, ScriptVoid>(vehicle, unknown0_3, x, y, z, spinX, spinY, spinZ, unknown4_0, unknown5_1, unknown6_1, unknown7_1); }
	static b8 AreTaxiLightsOn(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_ARE_TAXI_LIGHTS_ON, b8>(vehicle); }
	static void BreakCarDoor(Vehicle vehicle, eVehicleDoor door, b8 unknownFalse) { NativeInvoke::Invoke<NATIVE_BREAK_CAR_DOOR, ScriptVoid>(vehicle, door, unknownFalse); }
	static void BurstCarTyre(Vehicle vehicle, unsigned int tyre) { NativeInvoke::Invoke<NATIVE_BURST_CAR_TYRE, ScriptVoid>(vehicle, tyre); }
	static void CreateCar(u32 nameHash, f32 x, f32 y, f32 z, Vehicle *pVehicle, b8 unknownTrue) { NativeInvoke::Invoke<NATIVE_CREATE_CAR, ScriptVoid>(nameHash, x, y, z, pVehicle, unknownTrue); }
	static void CreateCharAsPassenger(Vehicle vehicle, ePedType charType, eModel model, u32 passengerIndex, Ped *pPed) { NativeInvoke::Invoke<NATIVE_CREATE_CHAR_AS_PASSENGER, ScriptVoid>(vehicle, charType, model, passengerIndex, pPed); }
	static void CreateCharInsideCar(Vehicle vehicle, ePedType charType, eModel model, Ped *pPed) { NativeInvoke::Invoke<NATIVE_CREATE_CHAR_INSIDE_CAR, ScriptVoid>(vehicle, charType, model, pPed); }
	static void ChangeCarColour(Vehicle vehicle, ColourIndex colour1, ColourIndex colour2) { NativeInvoke::Invoke<NATIVE_CHANGE_CAR_COLOUR, ScriptVoid>(vehicle, colour1, colour2); }
	static void ClearCarLastDamageEntity(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_CLEAR_CAR_LAST_DAMAGE_ENTITY, ScriptVoid>(vehicle); }
	static void ClearCarLastWeaponDamage(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_CLEAR_CAR_LAST_WEAPON_DAMAGE, ScriptVoid>(vehicle); }
	static void ClearRoomForCar(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_CLEAR_ROOM_FOR_CAR, ScriptVoid>(vehicle); }
	static void CloseAllCarDoors(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_CLOSE_ALL_CAR_DOORS, ScriptVoid>(vehicle); }
	static void ControlCarDoor(Vehicle vehicle, u32 door, u32 unknown_maybe_open, f32 angle) { NativeInvoke::Invoke<NATIVE_CONTROL_CAR_DOOR, ScriptVoid>(vehicle, door, unknown_maybe_open, angle); }
	static b8 CreateEmergencyServicesCar(eModel model, f32 x, f32 y, f32 z) { return NativeInvoke::Invoke<NATIVE_CREATE_EMERGENCY_SERVICES_CAR, b8>(model, x, y, z); }
	static void CreateRandomCharAsPassenger(Vehicle vehicle, u32 seat, Ped *pPed) { NativeInvoke::Invoke<NATIVE_CREATE_RANDOM_CHAR_AS_PASSENGER, ScriptVoid>(vehicle, seat, pPed); }
	static void DeleteCar(Vehicle *pVehicle) { NativeInvoke::Invoke<NATIVE_DELETE_CAR, ScriptVoid>(pVehicle); }
	static void DetachCar(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_DETACH_CAR, ScriptVoid>(vehicle); }
	static b8 DoesCarHaveRoof(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_DOES_CAR_HAVE_ROOF, b8>(vehicle); }
	static b8 DoesCarHaveStuckCarCheck(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_DOES_CAR_HAVE_STUCK_CAR_CHECK, b8>(vehicle); }
	static b8 DoesVehicleExist(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_DOES_VEHICLE_EXIST, b8>(vehicle); }

	static b8 DoesVehicleIdExist(u32 carId) { return NativeInvoke::Invoke<NATIVE_DOES_VEHICLE_EXIST, b8>(carId); }
	static void DeleteCarByScriptHandle(ScriptHandle* car) { NativeInvoke::Invoke<NATIVE_DELETE_CAR, ScriptVoid>(car); }


	static void SetCarCollision(Vehicle car, bool b) { NativeInvoke::Invoke<NATIVE_SET_CAR_COLLISION, ScriptVoid>(car, b); }
	static void EnableSaveHouse(b8 enable) { NativeInvoke::Invoke<NATIVE_ENABLE_SAVE_HOUSE, ScriptVoid>(enable); }
	static void EnablePedHelmet(Ped ped, b8 enable) { NativeInvoke::Invoke<NATIVE_ENABLE_PED_HELMET, ScriptVoid>(ped, enable); }
	static void ExplodeCar(Vehicle vehicle, b8 unknownTrue) { NativeInvoke::Invoke<NATIVE_EXPLODE_CAR, ScriptVoid>(vehicle, unknownTrue); }
	static void ExtinguishCarFire(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_EXTINGUISH_CAR_FIRE, ScriptVoid>(vehicle); }
	static void FixCar(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_FIX_CAR, ScriptVoid>(vehicle); }
	static void FixCarTyre(Vehicle vehicle, unsigned int tyre) { NativeInvoke::Invoke<NATIVE_FIX_CAR_TYRE, ScriptVoid>(vehicle, tyre); }
	static void ForceVehicleLights(Vehicle vehicle, eVehicleLights lights) { NativeInvoke::Invoke<NATIVE_FORCE_CAR_LIGHTS, ScriptVoid>(vehicle, lights); }
	static void FreezeCarPosition(Vehicle vehicle, b8 frozen) { NativeInvoke::Invoke<NATIVE_FREEZE_CAR_POSITION, ScriptVoid>(vehicle, frozen); }
	static void FreezeCarPositionAndDontLoadCollision(Vehicle vehicle, b8 frozen) { NativeInvoke::Invoke<NATIVE_FREEZE_CAR_POSITION_AND_DONT_LOAD_COLLISION, ScriptVoid>(vehicle, frozen); }
	static void GetCarCharIsUsing(Ped ped, Vehicle *pVehicle) { NativeInvoke::Invoke<NATIVE_GET_CAR_CHAR_IS_USING, ScriptVoid>(ped, pVehicle); }
	static void GetCarColours(Vehicle vehicle, ColourIndex *pColour1, ColourIndex *pColour2) { NativeInvoke::Invoke<NATIVE_GET_CAR_COLOURS, ScriptVoid>(vehicle, pColour1, pColour2); }
	static void GetCarCoordinates(Vehicle vehicle, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_CAR_COORDINATES, ScriptVoid>(vehicle, pX, pY, pZ); }
	static void GetCarDeformationAtPos(Vehicle vehicle, f32 x, f32 y, f32 z, Vector3 *pDeformation) { NativeInvoke::Invoke<NATIVE_GET_CAR_DEFORMATION_AT_POS, ScriptVoid>(vehicle, x, y, z, pDeformation); }
	static void GetCarDoorLockStatus(Vehicle vehicle, eVehicleDoorLock *pValue) { NativeInvoke::Invoke<NATIVE_GET_CAR_DOOR_LOCK_STATUS, ScriptVoid>(vehicle, pValue); }
	static void GetCarForwardX(Vehicle vehicle, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CAR_FORWARD_X, ScriptVoid>(vehicle, pValue); }
	static void GetCarForwardY(Vehicle vehicle, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CAR_FORWARD_Y, ScriptVoid>(vehicle, pValue); }
	static void GetCarHeading(Vehicle vehicle, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CAR_HEADING, ScriptVoid>(vehicle, pValue); }
	static void GetCarHealth(Vehicle vehicle, u32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CAR_HEALTH, ScriptVoid>(vehicle, pValue); }
	static void GetCarModel(Vehicle vehicle, eModel *pValue) { NativeInvoke::Invoke<NATIVE_GET_CAR_MODEL, ScriptVoid>(vehicle, pValue); }
	static void GetCarModelByCarId(u32 vehicle, u32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CAR_MODEL, ScriptVoid>(vehicle, pValue); }
	static void GetCarPitch(Vehicle vehicle, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CAR_PITCH, ScriptVoid>(vehicle, pValue); }
	static void GetCarRoll(Vehicle vehicle, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CAR_ROLL, ScriptVoid>(vehicle, pValue); }
	static void GetCarSpeed(Vehicle vehicle, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CAR_SPEED, ScriptVoid>(vehicle, pValue); }
	static void GetCarSpeedVector(Vehicle vehicle, Vector3 *pVector, b8 unknownFalse) { NativeInvoke::Invoke<NATIVE_GET_CAR_SPEED_VECTOR, ScriptVoid>(vehicle, pVector, unknownFalse); }
	static void GetCarUprightValue(Vehicle vehicle, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GET_CAR_UPRIGHT_VALUE, ScriptVoid>(vehicle, pValue); }
	static void GetCharInCarPassengerSeat(Vehicle vehicle, u32 seatIndex, Ped *pPed) { NativeInvoke::Invoke<NATIVE_GET_CHAR_IN_CAR_PASSENGER_SEAT, ScriptVoid>(vehicle, seatIndex, pPed); }
	static Vehicle GetClosestCar(f32 x, f32 y, f32 z, f32 radius, u32 unknown0_0, u32 unknown1_70) { return NativeInvoke::Invoke<NATIVE_GET_CLOSEST_CAR, Vehicle>(x, y, z, radius, unknown0_0, unknown1_70); }
	static b8 GetClosestCarNode(f32 x, f32 y, f32 z, f32 *pResX, f32 *pResY, f32 *pResZ) { return NativeInvoke::Invoke<NATIVE_GET_CLOSEST_CAR_NODE, b8>(x, y, z, pResX, pResY, pResZ); }
	static b8 GetClosestCarNodeWithHeading(f32 x, f32 y, f32 z, f32 *pResX, f32 *pResY, f32 *pResZ, f32 *pHeading) { return NativeInvoke::Invoke<NATIVE_GET_CLOSEST_CAR_NODE_WITH_HEADING, b8>(x, y, z, pResX, pResY, pResZ, pHeading); }
	static b8 GetClosestMajorCarNode(f32 x, f32 y, f32 z, f32 *pResX, f32 *pResY, f32 *pResZ) { return NativeInvoke::Invoke<NATIVE_GET_CLOSEST_MAJOR_CAR_NODE, b8>(x, y, z, pResX, pResY, pResZ); }
	static b8 GetNthClosestCarNodeWithHeading(f32 x, f32 y, f32 z, u32 nodeNum, f32 *pResX, f32 *pResY, f32 *pResZ, f32 *pHeading) { return NativeInvoke::Invoke<NATIVE_GET_NTH_CLOSEST_CAR_NODE_WITH_HEADING, b8>(x, y, z, nodeNum, pResX, pResY, pResZ, pHeading); }
	static b8 GetNthClosestCarNodeWithHeadingOnIsland(f32 x, f32 y, f32 z, u32 nodeNum, u32 areaId, f32 *pResX, f32 *pResY, f32 *pResZ, f32 *pHeading, u32 *pUnknownMaybeAreaId) { return NativeInvoke::Invoke<NATIVE_GET_NTH_CLOSEST_CAR_NODE_WITH_HEADING_ON_ISLAND, b8>(x, y, z, nodeNum, areaId, pResX, pResY, pResZ, pHeading, pUnknownMaybeAreaId); }
	static void GetDeadCarCoordinates(Vehicle vehicle, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_DEAD_CAR_COORDINATES, ScriptVoid>(vehicle, pX, pY, pZ); }
	static void GetDoorAngleRatio(Vehicle vehicle, eVehicleDoor door, f32 *pAngleRatio) { NativeInvoke::Invoke<NATIVE_GET_DOOR_ANGLE_RATIO, ScriptVoid>(vehicle, door, pAngleRatio); }
	static void GetDriverOfCar(Vehicle vehicle, Ped *pPed) { NativeInvoke::Invoke<NATIVE_GET_DRIVER_OF_CAR, ScriptVoid>(vehicle, pPed); }
	static f32 GetEngineHealth(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_GET_ENGINE_HEALTH, f32>(vehicle); }
	static void GetExtraCarColours(Vehicle vehicle, ColourIndex *pColour1, ColourIndex *pColour2) { NativeInvoke::Invoke<NATIVE_GET_EXTRA_CAR_COLOURS, ScriptVoid>(vehicle, pColour1, pColour2); }
	static f32 GetHeightOfVehicle(Vehicle vehicle, f32 x, f32 y, f32 z, b8 unknownTrue1, b8 unknownTrue2) { return NativeInvoke::Invoke<NATIVE_GET_HEIGHT_OF_VEHICLE, f32>(vehicle, x, y, z, unknownTrue1, unknownTrue2); }
	static void GetKeyForCarInRoom(Vehicle vehicle, u32 *pKey) { NativeInvoke::Invoke<NATIVE_GET_KEY_FOR_CAR_IN_ROOM, ScriptVoid>(vehicle, pKey); }
	static void GetMaximumNumberOfPassengers(Vehicle vehicle, u32 *pMax) { NativeInvoke::Invoke<NATIVE_GET_MAXIMUM_NUMBER_OF_PASSENGERS, ScriptVoid>(vehicle, pMax); }
	static void GetNearestCableCar(f32 x, f32 y, f32 z, f32 radius, Vehicle *pVehicle) { NativeInvoke::Invoke<NATIVE_GET_NEAREST_CABLE_CAR, ScriptVoid>(x, y, z, radius, pVehicle); }
	static void GetNumCarColours(Vehicle vehicle, u32 *pNumColours) { NativeInvoke::Invoke<NATIVE_GET_NUM_CAR_COLOURS, ScriptVoid>(vehicle, pNumColours); }
	static void GetNumberOfPassengers(Vehicle vehicle, u32 *pNumPassengers) { NativeInvoke::Invoke<NATIVE_GET_NUMBER_OF_PASSENGERS, ScriptVoid>(vehicle, pNumPassengers); }
	static void GetOffsetFromCarGivenWorldCoords(Vehicle vehicle, f32 x, f32 y, f32 z, f32 *pOffX, f32 *pOffY, f32 *pOffZ) { NativeInvoke::Invoke<NATIVE_GET_OFFSET_FROM_CAR_GIVEN_WORLD_COORDS, ScriptVoid>(vehicle, x, y, z, pOffX, pOffY, pOffZ); }
	static void GetOffsetFromCarInWorldCoords(Vehicle vehicle, f32 x, f32 y, f32 z, f32 *pOffX, f32 *pOffY, f32 *pOffZ) { NativeInvoke::Invoke<NATIVE_GET_OFFSET_FROM_CAR_IN_WORLD_COORDS, ScriptVoid>(vehicle, x, y, z, pOffX, pOffY, pOffZ); }
	static f32 GetPetrolTankHealth(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_GET_PETROL_TANK_HEALTH, f32>(vehicle); }
	static void GetRandomCarModelInMemory(b8 unknownTrue, u32 *pHash, ScriptAny *pUnknown) { NativeInvoke::Invoke<NATIVE_GET_RANDOM_CAR_MODEL_IN_MEMORY, ScriptVoid>(unknownTrue, pHash, pUnknown); }
	static void GetVehicleDirtLevel(Vehicle vehicle, f32* pIntensity) { NativeInvoke::Invoke<NATIVE_GET_VEHICLE_DIRT_LEVEL, ScriptVoid>(vehicle, pIntensity); }
	static b8 HasCarBeenDamagedByCar(Vehicle vehicle, Vehicle otherCar) { return NativeInvoke::Invoke<NATIVE_HAS_CAR_BEEN_DAMAGED_BY_CAR, b8>(vehicle, otherCar); }
	static b8 HasCarBeenDamagedByChar(Vehicle vehicle, Ped ped) { return NativeInvoke::Invoke<NATIVE_HAS_CAR_BEEN_DAMAGED_BY_CHAR, b8>(vehicle, ped); }
	static b8 HasCarBeenDamagedByWeapon(Vehicle vehicle, eWeapon weapon) { return NativeInvoke::Invoke<NATIVE_HAS_CAR_BEEN_DAMAGED_BY_WEAPON, b8>(vehicle, weapon); }
	static b8 HasCarBeenResprayed(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_HAS_CAR_BEEN_RESPRAYED, b8>(vehicle); }
	static b8 IsBigVehicle(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_BIG_VEHICLE, b8>(vehicle); }
	static b8 IsCarAMissionCar(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_A_MISSION_CAR, b8>(vehicle); }
	static b8 IsCarAttached(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_ATTACHED, b8>(vehicle); }
	static b8 IsCarDead(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_DEAD, b8>(vehicle); }
	static b8 IsCarDoorDamaged(Vehicle vehicle, eVehicleDoor door) { return NativeInvoke::Invoke<NATIVE_IS_CAR_DOOR_DAMAGED, b8>(vehicle, door); }
	static b8 IsCarDoorFullyOpen(Vehicle vehicle, eVehicleDoor door) { return NativeInvoke::Invoke<NATIVE_IS_CAR_DOOR_FULLY_OPEN, b8>(vehicle, door); }
	static b8 IsCarInAirProper(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_IN_AIR_PROPER, b8>(vehicle); }
	static b8 IsCarInArea2D(Vehicle vehicle, f32 x1, f32 y1, f32 x2, f32 y2, b8 unknownFalse) { return NativeInvoke::Invoke<NATIVE_IS_CAR_IN_AREA_2D, b8>(vehicle, x1, y1, x2, y2, unknownFalse); }
	static b8 IsCarInArea3D(Vehicle vehicle, f32 x1, f32 y1, f32 z1, f32 x2, f32 y2, f32 z2, u8 unknownFalse) { return NativeInvoke::Invoke<NATIVE_IS_CAR_IN_AREA_3D, b8>(vehicle, x1, y1, z1, x2, y2, z2, unknownFalse); }
	static b8 IsCarInWater(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_IN_WATER, b8>(vehicle); }
	static b8 IsCarModel(Vehicle vehicle, eModel model) { return NativeInvoke::Invoke<NATIVE_IS_CAR_MODEL, b8>(vehicle, model); }
	static b8 IsCarOnFire(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_ON_FIRE, b8>(vehicle); }
	static b8 IsCarOnScreen(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_ON_SCREEN, b8>(vehicle); }
	static b8 IsCarPassengerSeatFree(Vehicle vehicle, u32 seatIndex) { return NativeInvoke::Invoke<NATIVE_IS_CAR_PASSENGER_SEAT_FREE, b8>(vehicle, seatIndex); }
	static b8 IsCarSirenOn(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_SIREN_ON, b8>(vehicle); }
	static b8 IsCarStopped(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_STOPPED, b8>(vehicle); }
	static b8 IsCarStoppedAtTrafficLights(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_STOPPED_AT_TRAFFIC_LIGHTS, b8>(vehicle); }
	static b8 IsCarStuckOnRoof(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_STUCK_ON_ROOF, b8>(vehicle); }
	static b8 IsCarTouchingCar(Vehicle vehicle, Vehicle otherCar) { return NativeInvoke::Invoke<NATIVE_IS_CAR_TOUCHING_CAR, b8>(vehicle, otherCar); }
	static b8 IsCarTyreBurst(Vehicle vehicle, unsigned int tyre) { return NativeInvoke::Invoke<NATIVE_IS_CAR_TYRE_BURST, b8>(vehicle, tyre); }
	static b8 IsCarUpright(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_UPRIGHT, b8>(vehicle); }
	static b8 IsCarUpsidedown(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_UPSIDEDOWN, b8>(vehicle); }
	static b8 IsCarWaitingForWorldCollision(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_WAITING_FOR_WORLD_COLLISION, b8>(vehicle); }
	static b8 IsVehDriveable(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_VEH_DRIVEABLE, b8>(vehicle); }
	static b8 IsVehWindowIntact(Vehicle vehicle, eVehicleWindow window) { return NativeInvoke::Invoke<NATIVE_IS_VEH_WINDOW_INTACT, b8>(vehicle, window); }
	static b8 IsVehicleExtraTurnedOn(Vehicle vehicle, eVehicleExtra extra) { return NativeInvoke::Invoke<NATIVE_IS_VEHICLE_EXTRA_TURNED_ON, b8>(vehicle, extra); }
	static b8 IsVehicleOnAllWheels(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_VEHICLE_ON_ALL_WHEELS, b8>(vehicle); }
	static void KnockPedOffBike(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_KNOCK_PED_OFF_BIKE, ScriptVoid>(vehicle); }
	static void LockCarDoors(Vehicle vehicle, u32 value) { NativeInvoke::Invoke<NATIVE_LOCK_CAR_DOORS, ScriptVoid>(vehicle, value); }
	static void MarkCarAsConvoyCar(Vehicle vehicle, b8 convoyCar) { NativeInvoke::Invoke<NATIVE_MARK_CAR_AS_CONVOY_CAR, ScriptVoid>(vehicle, convoyCar); }
	static void MarkCarAsNoLongerNeeded(Vehicle *pVehicle) { NativeInvoke::Invoke<NATIVE_MARK_CAR_AS_NO_LONGER_NEEDED, ScriptVoid>(pVehicle); }
	static void OpenCarDoor(Vehicle vehicle, eVehicleDoor door) { NativeInvoke::Invoke<NATIVE_OPEN_CAR_DOOR, ScriptVoid>(vehicle, door); }
	static void PopCarBoot(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_POP_CAR_BOOT, ScriptVoid>(vehicle); }
	static void RemoveVehicleWindow(Vehicle vehicle, eVehicleWindow window) { NativeInvoke::Invoke<NATIVE_REMOVE_CAR_WINDOW, ScriptVoid>(vehicle, window); }
	static void RemoveStuckCarCheck(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_REMOVE_STUCK_CAR_CHECK, ScriptVoid>(vehicle); }
	static void RemoveUpsidedownCarCheck(Vehicle vehicle) { NativeInvoke::Invoke<NATIVE_REMOVE_UPSIDEDOWN_CAR_CHECK, ScriptVoid>(vehicle); }

	static void SetCarCoordinates(Vehicle vehicle, f32 pX, f32 pY, f32 pZ) { NativeInvoke::Invoke<NATIVE_SET_CAR_COORDINATES, ScriptVoid>(vehicle, pX, pY, pZ); }
	static void SetCarCoordinatesNoOffset(Vehicle vehicle, f32 pX, f32 pY, f32 pZ) { NativeInvoke::Invoke<NATIVE_SET_CAR_COORDINATES_NO_OFFSET, ScriptVoid>(vehicle, pX, pY, pZ); }

	static void SetCarHealth(Vehicle vehicle, u32 pValue) { NativeInvoke::Invoke<NATIVE_SET_CAR_HEALTH, ScriptVoid>(vehicle, pValue); }
	static void SetPetrolTankHealth(Vehicle vehicle, f32 value) { NativeInvoke::Invoke<NATIVE_SET_PETROL_TANK_HEALTH, ScriptVoid>(vehicle, value); }
	static void SetCarCanBeDamaged(Vehicle vehicle, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CAR_CAN_BE_DAMAGED, ScriptVoid>(vehicle, value); }
	static void SetCarCanBeVisiblyDamaged(Vehicle vehicle, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CAR_CAN_BE_VISIBLY_DAMAGED, ScriptVoid>(vehicle, value); }
	static void SetCarForwardSpeed(Vehicle vehicle, f32 speed) { NativeInvoke::Invoke<NATIVE_SET_CAR_FORWARD_SPEED, ScriptVoid>(vehicle, speed); }
	static void SetCarHeading(Vehicle vehicle, f32 dir) { NativeInvoke::Invoke<NATIVE_SET_CAR_HEADING, ScriptVoid>(vehicle, dir); }
	static ScriptAny SetCarOnGroundProperly(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_SET_CAR_ON_GROUND_PROPERLY, ScriptAny>(vehicle); }
	static void SetCarProofs(Vehicle vehicle, b8 bulletProof, b8 fireProof, b8 explosionProof, b8 collisionProof, b8 meleeProof) { NativeInvoke::Invoke<NATIVE_SET_CAR_PROOFS, ScriptVoid>(vehicle, bulletProof, fireProof, explosionProof, collisionProof, meleeProof); }
	static void SetCarStrong(Vehicle vehicle, b8 strong) { NativeInvoke::Invoke<NATIVE_SET_CAR_STRONG, ScriptVoid>(vehicle, strong); }
	static void SetCarVisible(Vehicle vehicle, b8 value) { NativeInvoke::Invoke<NATIVE_SET_CAR_VISIBLE, ScriptVoid>(vehicle, value); }
	static void SetExtraCarColours(Vehicle vehicle, ColourIndex colour1, ColourIndex colour2) { NativeInvoke::Invoke<NATIVE_SET_EXTRA_CAR_COLOURS, ScriptVoid>(vehicle, colour1, colour2); }
	static void SetEngineHealth(Vehicle vehicle, f32 health) { NativeInvoke::Invoke<NATIVE_SET_ENGINE_HEALTH, ScriptVoid>(vehicle, health); }
	static void SetVehHazardLights(Vehicle vehicle, b8 on) { NativeInvoke::Invoke<NATIVE_SET_VEH_HAZARDLIGHTS, ScriptVoid>(vehicle, on); }
	static void SetVehicleDirtLevel(Vehicle vehicle, f32 intensity) { NativeInvoke::Invoke<NATIVE_SET_VEHICLE_DIRT_LEVEL, ScriptVoid>(vehicle, intensity); }
	static void ShutCarDoor(Vehicle vehicle, u32 door) { NativeInvoke::Invoke<NATIVE_SHUT_CAR_DOOR, ScriptVoid>(vehicle, door); }
	static void SoundCarHorn(Vehicle vehicle, u32 duration) { NativeInvoke::Invoke<NATIVE_SOUND_CAR_HORN, ScriptVoid>(vehicle, duration); }
	static void WashVehicleTextures(Vehicle vehicle, u32 intensity) { NativeInvoke::Invoke<NATIVE_WASH_VEHICLE_TEXTURES, ScriptVoid>(vehicle, intensity); }

	// Train
	static void CreateMissionTrain(u32 unknown1, f32 x, f32 y, f32 z, b8 unknown2, Train *pTrain) { NativeInvoke::Invoke<NATIVE_CREATE_MISSION_TRAIN, ScriptVoid>(unknown1, x, y, z, unknown2, pTrain); }
	static void DeleteMissionTrain(Train *pTrain) { NativeInvoke::Invoke<NATIVE_DELETE_MISSION_TRAIN, ScriptVoid>(pTrain); }
	static eTrainStation GetCurrentStationForTrain(Train train) { return NativeInvoke::Invoke<NATIVE_GET_CURRENT_STATION_FOR_TRAIN, eTrainStation>(train); }
	static eTrainStation GetNextStationForTrain(Train train) { return NativeInvoke::Invoke<NATIVE_GET_NEXT_STATION_FOR_TRAIN, eTrainStation>(train); }
	static const ch *GetStationName(Train train, eTrainStation station) { return NativeInvoke::Invoke<NATIVE_GET_STATION_NAME, const ch *>(train, station); }
	static void MarkMissionTrainAsNoLongerNeeded(Train train) { NativeInvoke::Invoke<NATIVE_MARK_MISSION_TRAIN_AS_NO_LONGER_NEEDED, ScriptVoid>(train); }
	static void MarkMissionTrainsAsNoLongerNeeded() { NativeInvoke::Invoke<NATIVE_MARK_MISSION_TRAINS_AS_NO_LONGER_NEEDED, ScriptVoid>(); }

	// Object
	static void AddObjectToInteriorRoomByKey(Object obj, u32 roomKey) { NativeInvoke::Invoke<NATIVE_ADD_OBJECT_TO_INTERIOR_ROOM_BY_KEY, ScriptVoid>(obj, roomKey); }
	static void ApplyForceToObject(Object obj, u32 uk0_3, f32 pX, f32 pY, f32 pZ, f32 spinX, f32 spinY, f32 spinZ, u32 uk4_0, u32 uk5_1, u32 uk6_1, u32 uk7_1) { NativeInvoke::Invoke<NATIVE_APPLY_FORCE_TO_OBJECT, ScriptVoid>(obj, uk0_3, pX, pY, pZ, spinX, spinY, spinZ, uk4_0, uk5_1, uk6_1, uk7_1); }
	static void AttachObjectToCar(Object obj, Vehicle v, u32 unknown0_0, f32 pX, f32 pY, f32 pZ, f32 rX, f32 rY, f32 rZ) { NativeInvoke::Invoke<NATIVE_ATTACH_OBJECT_TO_CAR, ScriptVoid>(obj, v, unknown0_0, pX, pY, pZ, rX, rY, rZ); }
	static void AttachObjectToPed(Object obj, Ped c, u32 bone, f32 pX, f32 pY, f32 pZ, f32 rX, f32 rY, f32 rZ, u32 unknown1_0) { NativeInvoke::Invoke<NATIVE_ATTACH_OBJECT_TO_PED, ScriptVoid>(obj, c, bone, pX, pY, pZ, rX, rY, rZ, unknown1_0); }
	static void ClearObjectLastDamageEntity(Object obj) { NativeInvoke::Invoke<NATIVE_CLEAR_OBJECT_LAST_DAMAGE_ENTITY, ScriptVoid>(obj); }
	static void ClearRoomForObject(Object obj) { NativeInvoke::Invoke<NATIVE_CLEAR_ROOM_FOR_OBJECT, ScriptVoid>(obj); }
	static void CreateObject(unsigned int model, f32 x, f32 y, f32 z, Object *pObj, b8 unknownTrue) { NativeInvoke::Invoke<NATIVE_CREATE_OBJECT, ScriptVoid>(model, x, y, z, pObj, unknownTrue); }
	static void CreateObjectNoOffset(unsigned int model, f32 x, f32 y, f32 z, Object *pObj, b8 unknownTrue) { NativeInvoke::Invoke<NATIVE_CREATE_OBJECT_NO_OFFSET, ScriptVoid>(model, x, y, z, pObj, unknownTrue); }
	static void DeleteObject(Object *pObj) { NativeInvoke::Invoke<NATIVE_DELETE_OBJECT, ScriptVoid>(pObj); }
	static void DeleteObjectById(u32* pObj) { NativeInvoke::Invoke<NATIVE_DELETE_OBJECT, ScriptVoid>(pObj); }
	static void SetStateOfClosestDoorOfType(u32 model, f32 x, f32 y, f32 z, u32 lock, f32 angle) { NativeInvoke::Invoke<NATIVE_SET_STATE_OF_CLOSEST_DOOR_OF_TYPE, ScriptVoid>(model, x, y, z, lock, angle); }
	static void DetachObject(Object obj, b8 unknown) { NativeInvoke::Invoke<NATIVE_DETACH_OBJECT, ScriptVoid>(obj, unknown); }
	static b8 DoesObjectExist(Object obj) { return NativeInvoke::Invoke<NATIVE_DOES_OBJECT_EXIST, b8>(obj); }
	static b8 DoesObjectIdExist(u32 obj) { return NativeInvoke::Invoke<NATIVE_DOES_OBJECT_EXIST, b8>(obj); }
	static b8 DoesObjectHavePhysics(Object obj) { return NativeInvoke::Invoke<NATIVE_DOES_OBJECT_HAVE_PHYSICS, b8>(obj); }
	static b8 DoesObjectOfTypeExistAtCoords(f32 x, f32 y, f32 z, f32 radius, eModel model) { return NativeInvoke::Invoke<NATIVE_DOES_OBJECT_OF_TYPE_EXIST_AT_COORDS, b8>(x, y, z, radius, model); }
	static void ExtinguishObjectFire(Object obj) { NativeInvoke::Invoke<NATIVE_EXTINGUISH_OBJECT_FIRE, ScriptVoid>(obj); }
	static void FreezeObjectPosition(Object obj, b8 frozen) { NativeInvoke::Invoke<NATIVE_FREEZE_OBJECT_POSITION, ScriptVoid>(obj, frozen); }
	static Vehicle GetCarObjectIsAttachedTo(Object obj) { return NativeInvoke::Invoke<NATIVE_GET_CAR_OBJECT_IS_ATTACHED_TO, Vehicle>(obj); }
	static void GetObjectCoordinates(u32 id, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_OBJECT_COORDINATES, ScriptVoid>(id, pX, pY, pZ); }
	static void GetObjectQuaternion(u32 id, f32 *rX, f32 *rY, f32 *rZ, f32 *rW) { NativeInvoke::Invoke<NATIVE_GET_OBJECT_QUATERNION, ScriptVoid>(id, rX, rY, rZ, rW); }
	static f32 GetObjectFragmentDamageHealth(Object obj, b8 unknown) { return NativeInvoke::Invoke<NATIVE_GET_OBJECT_FRAGMENT_DAMAGE_HEALTH, f32>(obj, unknown); }
	static void GetObjectHeading(Object obj, f32 *pHeading) { NativeInvoke::Invoke<NATIVE_GET_OBJECT_HEADING, ScriptVoid>(obj, pHeading); }
	static void GetObjectHealth(Object obj, f32 *pHealth) { NativeInvoke::Invoke<NATIVE_GET_OBJECT_HEALTH, ScriptVoid>(obj, pHealth); }
	static void GetObjectModel(u32 id, u32* model) { NativeInvoke::Invoke<NATIVE_GET_OBJECT_MODEL, ScriptVoid>(id, model); }
	static void GetObjectRotationVelocity(Object obj, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_OBJECT_ROTATION_VELOCITY, ScriptVoid>(obj, pX, pY, pZ); }
	static void GetObjectSpeed(Object obj, f32 *pSpeed) { NativeInvoke::Invoke<NATIVE_GET_OBJECT_SPEED, ScriptVoid>(obj, pSpeed); }
	static void GetObjectVelocity(Object obj, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_OBJECT_VELOCITY, ScriptVoid>(obj, pX, pY, pZ); }
	static void GetOffsetFromObjectInWorldCoords(Object obj, f32 x, f32 y, f32 z, f32 *pOffX, f32 *pOffY, f32 *pOffZ) { NativeInvoke::Invoke<NATIVE_GET_OFFSET_FROM_OBJECT_IN_WORLD_COORDS, ScriptVoid>(obj, x, y, z, pOffX, pOffY, pOffZ); }
	static Ped GetPedObjectIsAttachedTo(Object obj) { return NativeInvoke::Invoke<NATIVE_GET_PED_OBJECT_IS_ATTACHED_TO, Ped>(obj); }
	static b8 HasObjectBeenDamaged(Object obj) { return NativeInvoke::Invoke<NATIVE_HAS_OBJECT_BEEN_DAMAGED, b8>(obj); }
	static b8 HasObjectBeenDamagedByCar(Object obj, Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_HAS_OBJECT_BEEN_DAMAGED_BY_CAR, b8>(obj, vehicle); }
	static b8 HasObjectBeenDamagedByChar(Object obj, Ped ped) { return NativeInvoke::Invoke<NATIVE_HAS_OBJECT_BEEN_DAMAGED_BY_CHAR, b8>(obj, ped); }
	static b8 HasObjectIdBeenDamagedByChar(u32 obj, Ped ped) { return NativeInvoke::Invoke<NATIVE_HAS_OBJECT_BEEN_DAMAGED_BY_CHAR, b8>(obj, ped); }
	static b8 HasObjectBeenUprooted(Object obj) { return NativeInvoke::Invoke<NATIVE_HAS_OBJECT_BEEN_UPROOTED, b8>(obj); }
	static b8 HasObjectCollidedWithAnything(Object obj) { return NativeInvoke::Invoke<NATIVE_HAS_OBJECT_COLLIDED_WITH_ANYTHING, b8>(obj); }
	static b8 HasPoolObjectCollidedWithCushion(Object obj) { return NativeInvoke::Invoke<NATIVE_HAS_POOL_OBJECT_COLLIDED_WITH_CUSHION, b8>(obj); }
	static b8 HasPoolObjectCollidedWithObject(Object obj, Object otherObj) { return NativeInvoke::Invoke<NATIVE_HAS_POOL_OBJECT_COLLIDED_WITH_OBJECT, b8>(obj, otherObj); }
	static b8 IsObjectAttached(Object obj) { return NativeInvoke::Invoke<NATIVE_IS_OBJECT_ATTACHED, b8>(obj); }
	static b8 IsObjectInWater(Object obj) { return NativeInvoke::Invoke<NATIVE_IS_OBJECT_IN_WATER, b8>(obj); }
	static b8 IsObjectOnFire(Object obj) { return NativeInvoke::Invoke<NATIVE_IS_OBJECT_ON_FIRE, b8>(obj); }
	static b8 IsObjectOnScreen(Object obj) { return NativeInvoke::Invoke<NATIVE_IS_OBJECT_ON_SCREEN, b8>(obj); }
	static b8 IsObjectStatic(Object obj) { return NativeInvoke::Invoke<NATIVE_IS_OBJECT_STATIC, b8>(obj); }
	static b8 IsObjectUpright(Object obj, f32 angle) { return NativeInvoke::Invoke<NATIVE_IS_OBJECT_UPRIGHT, b8>(obj, angle); }
	static b8 IsObjectWithinBrainActivationRange(Object obj) { return NativeInvoke::Invoke<NATIVE_IS_OBJECT_WITHIN_BRAIN_ACTIVATION_RANGE, b8>(obj); }
	static void LoadAllObjectsNow() { NativeInvoke::Invoke<NATIVE_LOAD_ALL_OBJECTS_NOW, ScriptVoid>(); }
	static void MakeObjectTargettable(Object obj, b8 targettable) { NativeInvoke::Invoke<NATIVE_MAKE_OBJECT_TARGETTABLE, ScriptVoid>(obj, targettable); }
	static void MarkObjectAsNoLongerNeeded(Object *pObj) { NativeInvoke::Invoke<NATIVE_MARK_OBJECT_AS_NO_LONGER_NEEDED, ScriptVoid>(pObj); }
	static void SetObjectCollision(Object obj, b8 value) { NativeInvoke::Invoke<NATIVE_SET_OBJECT_COLLISION, ScriptVoid>(obj, value); }
	static void SetObjectCoordinates(Object obj, f32 pX, f32 pY, f32 pZ) { NativeInvoke::Invoke<NATIVE_SET_OBJECT_COORDINATES, ScriptVoid>(obj, pX, pY, pZ); }
	static void SetObjectHeading(Object obj, f32 value) { NativeInvoke::Invoke<NATIVE_SET_OBJECT_HEADING, ScriptVoid>(obj, value); }
	static void SetObjectQuaternion(Object obj, f32 x, f32 y, f32 z, f32 w) { NativeInvoke::Invoke<NATIVE_SET_OBJECT_QUATERNION, ScriptVoid>(obj, x, y, z, w); }
	static void SetObjectVisible(Object obj, b8 value) { NativeInvoke::Invoke<NATIVE_SET_OBJECT_VISIBLE, ScriptVoid>(obj, value); }
	static ScriptAny StartObjectFire(Object obj) { return NativeInvoke::Invoke<NATIVE_START_OBJECT_FIRE, ScriptAny>(obj); }
	static void SetObjectRecordsCollisions(Object obj, b8 value) { NativeInvoke::Invoke<NATIVE_SET_OBJECT_RECORDS_COLLISIONS, ScriptVoid>(obj, value); }
	static void SetObjectAlpha(Object obj, int value) { NativeInvoke::Invoke<NATIVE_SET_OBJECT_ALPHA, ScriptVoid>(obj, value); }
	// Models
	static b8 HasModelLoaded(unsigned int model) { return NativeInvoke::Invoke<NATIVE_HAS_MODEL_LOADED, b8>(model); }
	static void RequestModel(unsigned int model) { NativeInvoke::Invoke<NATIVE_REQUEST_MODEL, ScriptVoid>(model); }
	static bool IsModelInCdImage(unsigned int model){ return NativeInvoke::Invoke<NATIVE_IS_MODEL_IN_CDIMAGE, b8>(model); }
	static void GetCurrentBasicCopModel(eModel *pModel) { NativeInvoke::Invoke<NATIVE_GET_CURRENT_BASIC_COP_MODEL, ScriptVoid>(pModel); }
	static void GetCurrentBasicPoliceCarModel(eModel *pModel) { NativeInvoke::Invoke<NATIVE_GET_CURRENT_BASIC_POLICE_CAR_MODEL, ScriptVoid>(pModel); }
	static void GetCurrentCopModel(eModel *pModel) { NativeInvoke::Invoke<NATIVE_GET_CURRENT_COP_MODEL, ScriptVoid>(pModel); }
	static void GetCurrentPoliceCarModel(eModel *pModel) { NativeInvoke::Invoke<NATIVE_GET_CURRENT_POLICE_CAR_MODEL, ScriptVoid>(pModel); }
	static void GetCurrentTaxiCarModel(eModel *pModel) { NativeInvoke::Invoke<NATIVE_GET_CURRENT_TAXI_CAR_MODEL, ScriptVoid>(pModel); }
	static const ch *GetDisplayNameFromVehicleModel(eModel model) { return NativeInvoke::Invoke<NATIVE_GET_DISPLAY_NAME_FROM_VEHICLE_MODEL, const ch *>(model); }
	static void GetModelDimensions(eModel model, Vector3 *pMinVector, Vector3 *pMaxVector) { NativeInvoke::Invoke<NATIVE_GET_MODEL_DIMENSIONS, ScriptVoid>(model, pMinVector, pMaxVector); }
	static const ch *GetModelNameForDebug(eModel model) { return NativeInvoke::Invoke<NATIVE_GET_MODEL_NAME_FOR_DEBUG, const ch *>(model); }
	static b8 IsThisModelABike(unsigned int model) { return NativeInvoke::Invoke<NATIVE_IS_THIS_MODEL_A_BIKE, b8>(model); }
	static b8 IsThisModelABoat(unsigned int model) { return NativeInvoke::Invoke<NATIVE_IS_THIS_MODEL_A_BOAT, b8>(model); }
	static b8 IsThisModelACar(unsigned int model) { return NativeInvoke::Invoke<NATIVE_IS_THIS_MODEL_A_CAR, b8>(model); }
	static b8 IsThisModelAHeli(unsigned int model) { return NativeInvoke::Invoke<NATIVE_IS_THIS_MODEL_A_HELI, b8>(model); }
	static b8 IsThisModelAPed(unsigned int model) { return NativeInvoke::Invoke<NATIVE_IS_THIS_MODEL_A_PED, b8>(model); }
	static b8 IsThisModelAPlane(unsigned int model) { return NativeInvoke::Invoke<NATIVE_IS_THIS_MODEL_A_PLANE, b8>(model); }
	static b8 IsThisModelATrain(unsigned int model) { return NativeInvoke::Invoke<NATIVE_IS_THIS_MODEL_A_TRAIN, b8>(model); }
	static b8 IsThisModelAVehicle(unsigned int model) { return NativeInvoke::Invoke<NATIVE_IS_THIS_MODEL_A_VEHICLE, b8>(model); }
	static void MarkModelAsNoLongerNeeded(unsigned int model) { NativeInvoke::Invoke<NATIVE_MARK_MODEL_AS_NO_LONGER_NEEDED, ScriptVoid>(model); }

	// Interiors
	static void ActivateInterior(Interior interior, b8 unknownTrue) { NativeInvoke::Invoke<NATIVE_ACTIVATE_INTERIOR, ScriptVoid>(interior, unknownTrue); }
	static void GetInteriorAtCoords(f32 x, f32 y, f32 z, Interior *pInterior) { NativeInvoke::Invoke<NATIVE_GET_INTERIOR_AT_COORDS, ScriptVoid>(x, y, z, pInterior); }
	static void GetInteriorFromCar(Vehicle vehicle, Interior *pInterior) { NativeInvoke::Invoke<NATIVE_GET_INTERIOR_FROM_CAR, ScriptVoid>(vehicle, pInterior); }
	static void GetInteriorFromChar(Ped ped, Interior *pInterior) { NativeInvoke::Invoke<NATIVE_GET_INTERIOR_FROM_CHAR, ScriptVoid>(ped, pInterior); }
	static void GetInteriorHeading(Interior interior, f32 *pHeading) { NativeInvoke::Invoke<NATIVE_GET_INTERIOR_HEADING, ScriptVoid>(interior, pHeading); }
	static void GetOffsetFromInteriorInWorldCoords(Interior interior, f32 x, f32 y, f32 z, f32 *pOffset) { NativeInvoke::Invoke<NATIVE_GET_OFFSET_FROM_INTERIOR_IN_WORLD_COORDS, ScriptVoid>(interior, x, y, z, pOffset); }
	static void GetRoomKeyFromObject(Object obj, eInteriorRoomKey *pRoomKey) { NativeInvoke::Invoke<NATIVE_GET_ROOM_KEY_FROM_OBJECT, ScriptVoid>(obj, pRoomKey); }
	static void RequestInteriorModels(eModel model, const ch *interiorName) { NativeInvoke::Invoke<NATIVE_REQUEST_INTERIOR_MODELS, ScriptVoid>(model, interiorName); }

	// Decision Maker
	static void AddCharDecisionMakerEventResponse(DecisionMaker dm, u32 eventid, u32 responseid, f32 param1, f32 param2, f32 param3, f32 param4, u32 unknown0_1, u32 unknown1_1) { NativeInvoke::Invoke<NATIVE_ADD_CHAR_DECISION_MAKER_EVENT_RESPONSE, ScriptVoid>(dm, eventid, responseid, param1, param2, param3, param4, unknown0_1, unknown1_1); }
	//static void AddCombatDecisionMakerEventResponse(DecisionMaker dm, u32 eventid, u32 responseid, f32 param1, f32 param2, f32 param3, f32 param4, u32 unknown0_1, u32 unknown1_1) { NativeInvoke::Invoke<NATIVE_ADD_COMBAT_DECISION_MAKER_EVENT_RESPONSE, ScriptVoid>(dm, eventid, responseid, param1, param2, param3, param4, unknown0_1, unknown1_1); }
	static void AddGroupDecisionMakerEventResponse(DecisionMaker dm, u32 eventid, u32 responseid, f32 param1, f32 param2, f32 param3, f32 param4, u32 unknown0_1, u32 unknown1_1) { NativeInvoke::Invoke<NATIVE_ADD_GROUP_DECISION_MAKER_EVENT_RESPONSE, ScriptVoid>(dm, eventid, responseid, param1, param2, param3, param4, unknown0_1, unknown1_1); }
	static void ClearCharDecisionMakerEventResponse(DecisionMaker dm, u32 eventid) { NativeInvoke::Invoke<NATIVE_CLEAR_CHAR_DECISION_MAKER_EVENT_RESPONSE, ScriptVoid>(dm, eventid); }
	//static void ClearCombatDecisionMakerEventResponse(DecisionMaker dm, u32 eventid) { NativeInvoke::Invoke<NATIVE_CLEAR_COMBAT_DECISION_MAKER_EVENT_RESPONSE, ScriptVoid>(dm, eventid); }
	//static void ClearGroupDecisionMakerEventResponse(DecisionMaker dm, u32 eventid) { NativeInvoke::Invoke<NATIVE_CLEAR_GROUP_DECISION_MAKER_EVENT_RESPONSE, ScriptVoid>(dm, eventid); }
	static void CopyCharDecisionMaker(u32 type, DecisionMaker *pDM) { NativeInvoke::Invoke<NATIVE_COPY_CHAR_DECISION_MAKER, ScriptVoid>(type, pDM); }
	static void CopyCombatDecisionMaker(u32 type, DecisionMaker *pDM) { NativeInvoke::Invoke<NATIVE_COPY_COMBAT_DECISION_MAKER, ScriptVoid>(type, pDM); }
	static void CopyGroupCharDecisionMaker(u32 type, DecisionMaker *pDM) { NativeInvoke::Invoke<NATIVE_COPY_GROUP_CHAR_DECISION_MAKER, ScriptVoid>(type, pDM); }
	static void CopyGroupCombatDecisionMaker(u32 type, DecisionMaker *pDM) { NativeInvoke::Invoke<NATIVE_COPY_GROUP_COMBAT_DECISION_MAKER, ScriptVoid>(type, pDM); }
	static void CopySharedCharDecisionMaker(u32 type, DecisionMaker *pDM) { NativeInvoke::Invoke<NATIVE_COPY_SHARED_CHAR_DECISION_MAKER, ScriptVoid>(type, pDM); }
	static void CopySharedCombatDecisionMaker(u32 type, DecisionMaker *pDM) { NativeInvoke::Invoke<NATIVE_COPY_SHARED_COMBAT_DECISION_MAKER, ScriptVoid>(type, pDM); }
	static b8 DoesDecisionMakerExist(DecisionMaker dm) { return NativeInvoke::Invoke<NATIVE_DOES_DECISION_MAKER_EXIST, b8>(dm); }
	static void LoadCharDecisionMaker(u32 type, DecisionMaker *pDM) { NativeInvoke::Invoke<NATIVE_LOAD_CHAR_DECISION_MAKER, ScriptVoid>(type, pDM); }
	//static void LoadCombatDecisionMaker(u32 type, DecisionMaker *pDM) { NativeInvoke::Invoke<NATIVE_LOAD_COMBAT_DECISION_MAKER, ScriptVoid>(type, pDM); }
	/* nullsub in 104 */ // static void LoadGroupDecisionMaker(u32 type, DecisionMaker *pDM) { NativeInvoke::Invoke<NATIVE_LOAD_GROUP_DECISION_MAKER, ScriptVoid>(type, pDM); }
	static void RemoveDecisionMaker(DecisionMaker dm) { NativeInvoke::Invoke<NATIVE_REMOVE_DECISION_MAKER, ScriptVoid>(dm); }
	static void SetCharDecisionMaker(Ped ped, DecisionMaker dm) { NativeInvoke::Invoke<NATIVE_SET_CHAR_DECISION_MAKER, ScriptVoid>(ped, dm); }
	static void SetCharDecisionMakerToDefault(Ped ped) { NativeInvoke::Invoke<NATIVE_SET_CHAR_DECISION_MAKER_TO_DEFAULT, ScriptVoid>(ped); }
	static void SetCombatDecisionMaker(Ped ped, DecisionMaker dm) { NativeInvoke::Invoke<NATIVE_SET_COMBAT_DECISION_MAKER, ScriptVoid>(ped, dm); }
	//static void SetDecisionMakerAttributeCanChangeTarget(DecisionMaker dm, b8 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_CAN_CHANGE_TARGET, ScriptVoid>(dm, value); }
	static void SetDecisionMakerAttributeCaution(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_CAUTION, ScriptVoid>(dm, value); }
	static void SetDecisionMakerAttributeFireRate(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_FIRE_RATE, ScriptVoid>(dm, value); }
	static void SetDecisionMakerAttributeLowHealth(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_LOW_HEALTH, ScriptVoid>(dm, value); }
	//static void SetDecisionMakerAttributeMovementStyle(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_MOVEMENT_STYLE, ScriptVoid>(dm, value); }
	//static void SetDecisionMakerAttributeNavigationStyle(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_NAVIGATION_STYLE, ScriptVoid>(dm, value); }
	//static void SetDecisionMakerAttributeRetreatingBehaviour(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_RETREATING_BEHAVIOUR, ScriptVoid>(dm, value); }
	//static void SetDecisionMakerAttributeSightRange(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_SIGHT_RANGE, ScriptVoid>(dm, value); }
	//static void SetDecisionMakerAttributeStandingStyle(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_STANDING_STYLE, ScriptVoid>(dm, value); }
	//static void SetDecisionMakerAttributeTargetInjuredReaction(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_TARGET_INJURED_REACTION, ScriptVoid>(dm, value); }
	//static void SetDecisionMakerAttributeTargetLossResponse(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_TARGET_LOSS_RESPONSE, ScriptVoid>(dm, value); }
	static void SetDecisionMakerAttributeTeamwork(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_TEAMWORK, ScriptVoid>(dm, value); }
	//static void SetDecisionMakerAttributeWeaponAccuracy(DecisionMaker dm, u32 value) { NativeInvoke::Invoke<NATIVE_SET_DECISION_MAKER_ATTRIBUTE_WEAPON_ACCURACY, ScriptVoid>(dm, value); }
	static void SetGroupCharDecisionMaker(Group group, DecisionMaker dm) { NativeInvoke::Invoke<NATIVE_SET_GROUP_CHAR_DECISION_MAKER, ScriptVoid>(group, dm); }
	static void SetGroupCombatDecisionMaker(Group group, DecisionMaker dm) { NativeInvoke::Invoke<NATIVE_SET_GROUP_COMBAT_DECISION_MAKER, ScriptVoid>(group, dm); }
	/* nullsub in 104 */ // static void SetGroupDecisionMaker(Group group, DecisionMaker dm) { NativeInvoke::Invoke<NATIVE_SET_GROUP_DECISION_MAKER, ScriptVoid>(group, dm); }

						 // Blips
	static void AddBlipForCar(Vehicle vehicle, Blip *pBlip) { NativeInvoke::Invoke<NATIVE_ADD_BLIP_FOR_CAR, ScriptVoid>(vehicle, pBlip); }
	static void AddBlipForChar(Ped ped, Blip *pBlip) { NativeInvoke::Invoke<NATIVE_ADD_BLIP_FOR_CHAR, ScriptVoid>(ped, pBlip); }
	static void AddBlipForContact(f32 x, f32 y, f32 z, Blip *pBlip) { NativeInvoke::Invoke<NATIVE_ADD_BLIP_FOR_CONTACT, ScriptVoid>(x, y, z, pBlip); }
	static void AddBlipForCoord(f32 x, f32 y, f32 z, Blip *pBlip) { NativeInvoke::Invoke<NATIVE_ADD_BLIP_FOR_COORD, ScriptVoid>(x, y, z, pBlip); }
	static void AddBlipForObject(Object obj, Blip *pBlip) { NativeInvoke::Invoke<NATIVE_ADD_BLIP_FOR_OBJECT, ScriptVoid>(obj, pBlip); }
	static void AddBlipForPickup(Pickup pickup, Blip *pBlip) { NativeInvoke::Invoke<NATIVE_ADD_BLIP_FOR_PICKUP, ScriptVoid>(pickup, pBlip); }
	static void AddSimpleBlipForPickup(Pickup pickup) { NativeInvoke::Invoke<NATIVE_ADD_SIMPLE_BLIP_FOR_PICKUP, ScriptVoid>(pickup); }
	static void ChangeBlipAlpha(Blip blip, ScriptAny alpha) { NativeInvoke::Invoke<NATIVE_CHANGE_BLIP_ALPHA, ScriptVoid>(blip, alpha); }
	static void ChangeBlipColour(Blip blip, u32 rgba) { NativeInvoke::Invoke<NATIVE_CHANGE_BLIP_COLOUR, ScriptVoid>(blip, rgba); }
	static void ChangeBlipDisplay(Blip blip, eBlipDisplay display) { NativeInvoke::Invoke<NATIVE_CHANGE_BLIP_DISPLAY, ScriptVoid>(blip, display); }
	static void ChangeBlipNameFromAscii(Blip blip, const ch *blipName) { NativeInvoke::Invoke<NATIVE_CHANGE_BLIP_NAME_FROM_ASCII, ScriptVoid>(blip, blipName); }
	static void ChangeBlipNameFromTextFile(Blip blip, const ch *gxtName) { NativeInvoke::Invoke<NATIVE_CHANGE_BLIP_NAME_FROM_TEXT_FILE, ScriptVoid>(blip, gxtName); }
	static void ChangeBlipPriority(Blip blip, eBlipPriority priority) { NativeInvoke::Invoke<NATIVE_CHANGE_BLIP_PRIORITY, ScriptVoid>(blip, priority); }
	static void ChangeBlipScale(Blip blip, f32 scale) { NativeInvoke::Invoke<NATIVE_CHANGE_BLIP_SCALE, ScriptVoid>(blip, scale); }
	static void ChangeBlipSprite(Blip blip, u32 sprite) { NativeInvoke::Invoke<NATIVE_CHANGE_BLIP_SPRITE, ScriptVoid>(blip, sprite); }
	static void DimBlip(Blip blip, b8 unknownTrue) { NativeInvoke::Invoke<NATIVE_DIM_BLIP, ScriptVoid>(blip, unknownTrue); }
	static b8 DoesBlipExist(Blip blip) { return NativeInvoke::Invoke<NATIVE_DOES_BLIP_EXIST, b8>(blip); }
	static void FlashBlip(Blip blip, b8 on) { NativeInvoke::Invoke<NATIVE_FLASH_BLIP, ScriptVoid>(blip, on); }
	static void FlashBlipAlt(Blip blip, b8 on) { NativeInvoke::Invoke<NATIVE_FLASH_BLIP_ALT, ScriptVoid>(blip, on); }
	static void GetBlipColour(Blip blip, ColourIndex *pColour) { NativeInvoke::Invoke<NATIVE_GET_BLIP_COLOUR, ScriptVoid>(blip, pColour); }
	static void GetBlipCoords(Blip blip, Vector3 *pVector) { NativeInvoke::Invoke<NATIVE_GET_BLIP_COORDS, ScriptVoid>(blip, pVector); }
	static eBlipType GetBlipInfoIdType(Blip blip) { return NativeInvoke::Invoke<NATIVE_GET_BLIP_INFO_ID_TYPE, eBlipType>(blip); }
	static Vehicle GetBlipInfoIdCarIndex(Blip blip) { return NativeInvoke::Invoke<NATIVE_GET_BLIP_INFO_ID_CAR_INDEX, Vehicle>(blip); }
	static u32 GetBlipInfoIdDisplay(Blip blip) { return NativeInvoke::Invoke<NATIVE_GET_BLIP_INFO_ID_DISPLAY, b8>(blip); }
	static Object GetBlipInfoIdObjectIndex(Blip blip) { return NativeInvoke::Invoke<NATIVE_GET_BLIP_INFO_ID_OBJECT_INDEX, Object>(blip); }
	static Ped GetBlipInfoIdPedIndex(Blip blip) { return NativeInvoke::Invoke<NATIVE_GET_BLIP_INFO_ID_PED_INDEX, Ped>(blip); }
	static Pickup GetBlipInfoIdPickupIndex(Blip blip) { return NativeInvoke::Invoke<NATIVE_GET_BLIP_INFO_ID_PICKUP_INDEX, Pickup>(blip); }
	/* No longer in 104 */ // static const ch *GetBlipName(Blip blip) { return NativeInvoke::Invoke<NATIVE_GET_BLIP_NAME, const ch *>(blip); }
	static eBlipSprite GetBlipSprite(Blip blip) { return NativeInvoke::Invoke<NATIVE_GET_BLIP_SPRITE, eBlipSprite>(blip); }
	static Blip GetFirstBlipInfoId(eBlipSprite type) { return NativeInvoke::Invoke<NATIVE_GET_FIRST_BLIP_INFO_ID, Blip>(type); }
	static Blip GetNextBlipInfoId(eBlipSprite type) { return NativeInvoke::Invoke<NATIVE_GET_NEXT_BLIP_INFO_ID, Blip>(type); }
	static b8 IsBlipShortRange(Blip blip) { return NativeInvoke::Invoke<NATIVE_IS_BLIP_SHORT_RANGE, b8>(blip); }
	static void RemoveBlip(Blip blip) { NativeInvoke::Invoke<NATIVE_REMOVE_BLIP, ScriptVoid>(blip); }
	static void SetBlipAsFriendly(Blip blip, b8 value) { NativeInvoke::Invoke<NATIVE_SET_BLIP_AS_FRIENDLY, ScriptVoid>(blip, value); }
	static void SetBlipAsShortRange(Blip blip, b8 value) { NativeInvoke::Invoke<NATIVE_SET_BLIP_AS_SHORT_RANGE, ScriptVoid>(blip, value); }
	static void SetRoute(Blip blip, b8 value) { NativeInvoke::Invoke<NATIVE_SET_ROUTE, ScriptVoid>(blip, value); }

	// Pickups
	static void AddPickupToInteriorRoomByName(Pickup pickup, const ch *roomName) { NativeInvoke::Invoke<NATIVE_ADD_PICKUP_TO_INTERIOR_ROOM_BY_NAME, ScriptVoid>(pickup, roomName); }
	static void CreateMoneyPickup(f32 x, f32 y, f32 z, u32 amount, b8 unknownTrue, Pickup *pPickup) { NativeInvoke::Invoke<NATIVE_CREATE_MONEY_PICKUP, ScriptVoid>(x, y, z, amount, unknownTrue, pPickup); }
	static void CreatePickup(eModel model, ePickupType pickupType, f32 x, f32 y, f32 z, Pickup *pPickup, b8 unknownFalse) { NativeInvoke::Invoke<NATIVE_CREATE_PICKUP, ScriptVoid>(model, pickupType, x, y, z, pPickup, unknownFalse); }
	static void CreatePickupRotate(eModel model, ePickupType pickupType, u32 unknown, f32 x, f32 y, f32 z, f32 rX, f32 rY, f32 rZ, Pickup *pPickup) { NativeInvoke::Invoke<NATIVE_CREATE_PICKUP_ROTATE, ScriptVoid>(model, pickupType, unknown, x, y, z, rX, rY, rZ, pPickup); }
	static void CreatePickupWithAmmo(eModel model, ePickupType pickupType, u32 unknown, f32 x, f32 y, f32 z, Pickup *pPickup) { NativeInvoke::Invoke<NATIVE_CREATE_PICKUP_WITH_AMMO, ScriptVoid>(model, pickupType, unknown, x, y, z, pPickup); }
	static b8 DoesPickupExist(Pickup pickup) { return NativeInvoke::Invoke<NATIVE_DOES_PICKUP_EXIST, b8>(pickup); }
	static void GetPickupCoordinates(Pickup pickup, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_PICKUP_COORDINATES, ScriptVoid>(pickup, pX, pY, pZ); }
	static b8 HasPickupBeenCollected(Pickup pickup) { return NativeInvoke::Invoke<NATIVE_HAS_PICKUP_BEEN_COLLECTED, b8>(pickup); }
	static void PickupsPassTime(u32 time) { NativeInvoke::Invoke<NATIVE_PICKUPS_PASS_TIME, ScriptVoid>(time); }
	static void RemovePickup(Pickup pickup) { NativeInvoke::Invoke<NATIVE_REMOVE_PICKUP, ScriptVoid>(pickup); }
	static void RemoveTemporaryRadarBlipsForPickups() { NativeInvoke::Invoke<NATIVE_REMOVE_TEMPORARY_RADAR_BLIPS_FOR_PICKUPS, ScriptVoid>(); }
	static void RenderWeaponPickupsBigger(b8 value) { NativeInvoke::Invoke<NATIVE_RENDER_WEAPON_PICKUPS_BIGGER, ScriptVoid>(value); }

	// Camera
	static b8 CamIsSphereVisible(Camera camera, f32 pX, f32 pY, f32 pZ, f32 radius) { return NativeInvoke::Invoke<NATIVE_CAM_IS_SPHERE_VISIBLE, b8>(camera, pX, pY, pZ, radius); }
	static void CreateCam(u32 camtype_usually14, Camera *camera) { NativeInvoke::Invoke<NATIVE_CREATE_CAM, ScriptVoid>(camtype_usually14, camera); }
	static void DestroyCam(Camera camera) { NativeInvoke::Invoke<NATIVE_DESTROY_CAM, ScriptVoid>(camera); }
	static b8 DoesCamExist(Camera camera) { return NativeInvoke::Invoke<NATIVE_DOES_CAM_EXIST, b8>(camera); }
	static void SetCamActive(Camera camera, bool value) { NativeInvoke::Invoke<NATIVE_SET_CAM_ACTIVE, ScriptVoid>(camera, value); }
	static void GetCamFov(Camera camera, f32 *fov) { NativeInvoke::Invoke<NATIVE_GET_CAM_FOV, ScriptVoid>(camera, fov); }
	static void GetCamPos(Camera camera, f32 *pX, f32 *pY, f32 *pZ) { NativeInvoke::Invoke<NATIVE_GET_CAM_POS, ScriptVoid>(camera, pX, pY, pZ); }
	static void GetCamRot(Camera camera, f32 *angleX, f32 *angleY, f32 *angleZ) { NativeInvoke::Invoke<NATIVE_GET_CAM_ROT, ScriptVoid>(camera, angleX, angleY, angleZ); }
	static void GetGameCam(Camera *camera) { NativeInvoke::Invoke<NATIVE_GET_GAME_CAM, ScriptVoid>(camera); }
	static void GetGameCamChild(Camera *camera) { NativeInvoke::Invoke<NATIVE_GET_GAME_CAM_CHILD, ScriptVoid>(camera); }
	static bool IsCamActive(Camera camera) { return NativeInvoke::Invoke<NATIVE_IS_CAM_ACTIVE, bool>(camera); }
	static bool IsCamInterpolating() { return NativeInvoke::Invoke<NATIVE_IS_CAM_INTERPOLATING, bool>(); }
	static bool IsCamPropagating(Camera camera) { return NativeInvoke::Invoke<NATIVE_IS_CAM_PROPAGATING, bool>(camera); }
	static void SetCamBehindPed(Ped ped) { NativeInvoke::Invoke<NATIVE_SET_CAM_BEHIND_PED, ScriptVoid>(ped); }
	static void SetCamFov(Camera camera, f32 fov) { NativeInvoke::Invoke<NATIVE_SET_CAM_FOV, ScriptVoid>(camera, fov); }
	static void SetCamInFrontOfPed(Ped ped) { NativeInvoke::Invoke<NATIVE_SET_CAM_IN_FRONT_OF_PED, ScriptVoid>(ped); }
	static void SetCamPos(Camera camera, f32 pX, f32 pY, f32 pZ) { NativeInvoke::Invoke<NATIVE_SET_CAM_POS, ScriptVoid>(camera, pX, pY, pZ); }
	static void SetCamPropagate(Camera camera, bool value) { NativeInvoke::Invoke<NATIVE_SET_CAM_PROPAGATE, ScriptVoid>(camera, value); }
	static void SetCamRot(Camera camera, f32 angleX, f32 angleY, f32 angleZ) { NativeInvoke::Invoke<NATIVE_SET_CAM_ROT, ScriptVoid>(camera, angleX, angleY, angleZ); }
	static void SetCamTargetPed(Camera camera, Ped ped) { NativeInvoke::Invoke<NATIVE_SET_CAM_TARGET_PED, ScriptVoid>(camera, ped); }

	static void ActivateScriptedCams(int b, int b1) { NativeInvoke::Invoke<NATIVE_ACTIVATE_SCRIPTED_CAMS, ScriptVoid>(b, b1); }
	static void PointCamAtCoord(Camera cam, float pX, float pY, float pZ) { NativeInvoke::Invoke<NATIVE_POINT_CAM_AT_COORD, ScriptVoid>(cam, pX, pY, pZ); }

	// Network
	static b8 NetworkIsGameRanked() { return NativeInvoke::Invoke<NATIVE_NETWORK_IS_GAME_RANKED, b8>(); }
	static b8 NetworkIsSessionStarted() { return NativeInvoke::Invoke<NATIVE_NETWORK_IS_SESSION_STARTED, b8>(); }

	// World
	static b8 AreAllNavmeshRegionsLoaded() { return NativeInvoke::Invoke<NATIVE_ARE_ALL_NAVMESH_REGIONS_LOADED, b8>(); }
	static void ClearArea(f32 x, f32 y, f32 z, f32 radius, b8 unknown) { NativeInvoke::Invoke<NATIVE_CLEAR_AREA, ScriptVoid>(x, y, z, radius, unknown); }
	static void ClearAreaOfCars(f32 x, f32 y, f32 z, f32 radius) { NativeInvoke::Invoke<NATIVE_CLEAR_AREA_OF_CARS, ScriptVoid>(x, y, z, radius); }
	static void ClearAreaOfChars(f32 x, f32 y, f32 z, f32 radius) { NativeInvoke::Invoke<NATIVE_CLEAR_AREA_OF_CHARS, ScriptVoid>(x, y, z, radius); }
	static void ClearAreaOfCops(f32 x, f32 y, f32 z, f32 radius) { NativeInvoke::Invoke<NATIVE_CLEAR_AREA_OF_COPS, ScriptVoid>(x, y, z, radius); }
	static void ClearAreaOfObjects(f32 x, f32 y, f32 z, f32 radius) { NativeInvoke::Invoke<NATIVE_CLEAR_AREA_OF_OBJECTS, ScriptVoid>(x, y, z, radius); }
	static void ClearPedNonCreationArea() { NativeInvoke::Invoke<NATIVE_CLEAR_PED_NON_CREATION_AREA, ScriptVoid>(); }
	static void ClearPedNonRemovalArea() { NativeInvoke::Invoke<NATIVE_CLEAR_PED_NON_REMOVAL_AREA, ScriptVoid>(); }
	static void ExtinguishFireAtPoint(f32 x, f32 y, f32 z, f32 radius) { NativeInvoke::Invoke<NATIVE_EXTINGUISH_FIRE_AT_POINT, ScriptVoid>(x, y, z, radius); }
	static void ForceWeather(eWeather weather) { NativeInvoke::Invoke<NATIVE_FORCE_WEATHER, ScriptVoid>(weather); }
	static void ForceWeatherNow(u32 weather) { NativeInvoke::Invoke<NATIVE_FORCE_WEATHER_NOW, ScriptVoid>(weather); }
	static void ForceWind(f32 wind) { NativeInvoke::Invoke<NATIVE_FORCE_WIND, ScriptVoid>(wind); }
	static void FreezePositionOfClosestObjectOfType(f32 x, f32 y, f32 z, f32 radius, eModel model, b8 frozen) { NativeInvoke::Invoke<NATIVE_FREEZE_POSITION_OF_CLOSEST_OBJECT_OF_TYPE, ScriptVoid>(x, y, z, radius, model, frozen); }
	static Vehicle GetClosestCar(f32 x, f32 y, f32 z, f32 radius, b8 unknownFalse, u32 unknown70) { return NativeInvoke::Invoke<NATIVE_GET_CLOSEST_CAR, Vehicle>(x, y, z, radius, unknownFalse, unknown70); }
	static b8 GetClosestChar(f32 x, f32 y, f32 z, f32 radius, b8 unknown1, b8 unknown2, Ped *pPed) { return NativeInvoke::Invoke<NATIVE_GET_CLOSEST_CHAR, b8>(x, y, z, radius, unknown1, unknown2, pPed); }
	static void GetCurrentWeather(eWeather *pWeather) { NativeInvoke::Invoke<NATIVE_GET_CURRENT_WEATHER, ScriptVoid>(pWeather); }
	static ScriptAny GetGroundZFor3DCoord(f32 x, f32 y, f32 z, f32 *pGroundZ) { return NativeInvoke::Invoke<NATIVE_GET_GROUND_Z_FOR_3D_COORD, ScriptAny>(x, y, z, pGroundZ); }
	static u32 GetNumberOfFiresInRange(f32 x, f32 y, f32 z, f32 radius) { return NativeInvoke::Invoke<NATIVE_GET_NUMBER_OF_FIRES_IN_RANGE, u32>(x, y, z, radius); }
	static void GetSafePickupCoords(f32 x, f32 y, f32 z, f32 *pSafeX, f32 *pSafeY, f32 *pSafeZ) { NativeInvoke::Invoke<NATIVE_GET_SAFE_PICKUP_COORDS, ScriptVoid>(x, y, z, pSafeX, pSafeY, pSafeZ); }
	static b8 GetSafePositionForChar(f32 x, f32 y, f32 z, b8 unknownTrue, f32 *pSafeX, f32 *pSafeY, f32 *pSafeZ) { return NativeInvoke::Invoke<NATIVE_GET_SAFE_POSITION_FOR_CHAR, b8>(x, y, z, unknownTrue, pSafeX, pSafeY, pSafeZ); }
	//static b8 HasClosestObjectOfTypeBeenDamagedByChar(f32 x, f32 y, f32 z, f32 radius, eModel objectModel, Ped ped) { return NativeInvoke::Invoke<NATIVE_HAS_CLOSEST_OBJECT_OF_TYPE_BEEN_DAMAGED_BY_CHAR, b8>(x, y, z, radius, objectModel, ped); }
	static b8 IsAreaOccupied(f32 x1, f32 y1, f32 z1, f32 x2, f32 y2, f32 z2, b8 unknownFalse1, b8 unknownTrue, b8 unknownFalse2, b8 unknownFalse3, b8 unknownFalse4) { return NativeInvoke::Invoke<NATIVE_IS_AREA_OCCUPIED, b8>(x1, y1, z1, x2, y2, z2, unknownFalse1, unknownTrue, unknownFalse2, unknownFalse3, unknownFalse4); }
	static b8 IsBulletInArea(f32 x, f32 y, f32 z, f32 radius, b8 unknownTrue) { return NativeInvoke::Invoke<NATIVE_IS_BULLET_IN_AREA, b8>(x, y, z, radius, unknownTrue); }
	static b8 IsBulletInBox(f32 x1, f32 y1, f32 z1, f32 x2, f32 y2, f32 z2, b8 unknown) { return NativeInvoke::Invoke<NATIVE_IS_BULLET_IN_BOX, b8>(x1, y1, z1, x2, y2, z2, unknown); }
	static b8 IsPointObscuredByAMissionEntity(f32 pX, f32 pY, f32 pZ, f32 sizeX, f32 sizeY, f32 sizeZ) { return NativeInvoke::Invoke<NATIVE_IS_POINT_OBSCURED_BY_A_MISSION_ENTITY, b8>(pX, pY, pZ, sizeX, sizeY, sizeZ); }
	static void LoadScene(f32 x, f32 y, f32 z) { NativeInvoke::Invoke<NATIVE_LOAD_SCENE, ScriptVoid>(x, y, z); }
	static void RemoveAllPickupsOfType(unsigned int type) { NativeInvoke::Invoke<NATIVE_REMOVE_ALL_PICKUPS_OF_TYPE, ScriptVoid>(type); }
	static void RequestCollisionAtPosn(f32 x, f32 y, f32 z) { NativeInvoke::Invoke<NATIVE_REQUEST_COLLISION_AT_POSN, ScriptVoid>(x, y, z); }
	static void SwitchAmbientPlanes(b8 on) { NativeInvoke::Invoke<NATIVE_SWITCH_AMBIENT_PLANES, ScriptVoid>(on); }
	static void SwitchArrowAboveBlippedPickups(b8 on) { NativeInvoke::Invoke<NATIVE_SWITCH_ARROW_ABOVE_BLIPPED_PICKUPS, ScriptVoid>(on); }
	static void SwitchRandomBoats(b8 on) { NativeInvoke::Invoke<NATIVE_SWITCH_RANDOM_BOATS, ScriptVoid>(on); }
	static void SwitchRandomTrains(b8 on) { NativeInvoke::Invoke<NATIVE_SWITCH_RANDOM_TRAINS, ScriptVoid>(on); }
	static void SwitchGarbageTrucks(b8 on) { NativeInvoke::Invoke<NATIVE_SWITCH_GARBAGE_TRUCKS, ScriptVoid>(on); }
	static void SwitchMadDrivers(b8 on) { NativeInvoke::Invoke<NATIVE_SWITCH_MAD_DRIVERS, ScriptVoid>(on); }

	// Garages
	static void CloseGarage(const ch *garageName) { NativeInvoke::Invoke<NATIVE_CLOSE_GARAGE, ScriptVoid>(garageName); }
	static b8 IsCarInGarageArea(const ch *garageName, Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_IS_CAR_IN_GARAGE_AREA, b8>(garageName, vehicle); }
	static ScriptAny IsGarageClosed(const ch *garageName) { return NativeInvoke::Invoke<NATIVE_IS_GARAGE_CLOSED, ScriptAny>(garageName); }
	static ScriptAny IsGarageOpen(const ch *garageName) { return NativeInvoke::Invoke<NATIVE_IS_GARAGE_OPEN, ScriptAny>(garageName); }
	static void OpenGarage(const ch *name) { NativeInvoke::Invoke<NATIVE_OPEN_GARAGE, ScriptVoid>(name); }

	// Text (GXT stuff)
	static void DisplayText(f32 x, f32 y, const ch *gxtName) { NativeInvoke::Invoke<NATIVE_DISPLAY_TEXT, ScriptVoid>(x, y, gxtName); }
	static void DisplayTextWith2Numbers(f32 x, f32 y, const ch *gxtName, i32 number1, i32 number2) { NativeInvoke::Invoke<NATIVE_DISPLAY_TEXT_WITH_2_NUMBERS, ScriptVoid>(x, y, gxtName, number1, number2); }
	static void DisplayTextWithFloat(f32 x, f32 y, const ch *gxtName, f32 value, u32 unknown) { NativeInvoke::Invoke<NATIVE_DISPLAY_TEXT_WITH_FLOAT, ScriptVoid>(x, y, gxtName, value, unknown); }
	static void DisplayTextWithLiteralString(f32 x, f32 y, const ch *gxtName, const ch *literalStr) { NativeInvoke::Invoke<NATIVE_DISPLAY_TEXT_WITH_LITERAL_STRING, ScriptVoid>(x, y, gxtName, literalStr); }
	static void DisplayTextWithNumber(f32 x, f32 y, const ch *gxtName, i32 value) { NativeInvoke::Invoke<NATIVE_DISPLAY_TEXT_WITH_NUMBER, ScriptVoid>(x, y, gxtName, value); }
	static void DisplayTextWithString(f32 x, f32 y, const ch *gxtName, const ch *gxtStringName) { NativeInvoke::Invoke<NATIVE_DISPLAY_TEXT_WITH_STRING, ScriptVoid>(x, y, gxtName, gxtStringName); }
	static void DisplayTextWithTwoLiteralStrings(f32 x, f32 y, const ch *gxtName, const ch *literalStr1, const ch *literalStr2) { NativeInvoke::Invoke<NATIVE_DISPLAY_TEXT_WITH_TWO_LITERAL_STRINGS, ScriptVoid>(x, y, gxtName, literalStr1, literalStr2); }
	static void DisplayTextWithTwoStrings(f32 x, f32 y, const ch *gxtName, const ch *gxtStringName1, const ch *gxtStringName2) { NativeInvoke::Invoke<NATIVE_DISPLAY_TEXT_WITH_TWO_STRINGS, ScriptVoid>(x, y, gxtName, gxtStringName1, gxtStringName2); }
	static u32 GetLengthOfStringWithThisTextLabel(const ch *gxtName) { return NativeInvoke::Invoke<NATIVE_GET_LENGTH_OF_STRING_WITH_THIS_TEXT_LABEL, u32>(gxtName); }
	static const ch *GetFirstNCharactersOfString(const ch *gxtName, u32 chars) { return NativeInvoke::Invoke<NATIVE_GET_FIRST_N_CHARACTERS_OF_STRING, const ch *>(gxtName, chars); }
	static void GetMobilePhoneRenderId(u32 *pRenderId) { NativeInvoke::Invoke<NATIVE_GET_MOBILE_PHONE_RENDER_ID, ScriptVoid>(pRenderId); }
	static const ch *GetNthIntegerInString(const ch *gxtName, u32 index) { return NativeInvoke::Invoke<NATIVE_GET_NTH_INTEGER_IN_STRING, const ch *>(gxtName, index); }
	static void GetScriptRenderTargetRenderId(u32 *pRenderId) { NativeInvoke::Invoke<NATIVE_GET_SCRIPT_RENDERTARGET_RENDER_ID, ScriptVoid>(pRenderId); }
	static const ch *GetStringFromHashKey(u32 hashKey) { return NativeInvoke::Invoke<NATIVE_GET_STRING_FROM_HASH_KEY, const ch *>(hashKey); }
	static ScriptAny GetStringWidth(const ch *gxtName) { return NativeInvoke::Invoke<NATIVE_GET_STRING_WIDTH, ScriptAny>(gxtName); }
	static ScriptAny GetStringWidthWithNumber(const ch *gxtName, i32 number) { return NativeInvoke::Invoke<NATIVE_GET_STRING_WIDTH_WITH_NUMBER, ScriptAny>(gxtName, number); }
	static ScriptAny GetStringWidthWithString(const ch *gxtName, const ch *literalString) { return NativeInvoke::Invoke<NATIVE_GET_STRING_WIDTH_WITH_STRING, ScriptAny>(gxtName, literalString); }
	static b8 HasAdditionalTextLoaded(u32 textIndex) { return NativeInvoke::Invoke<NATIVE_HAS_ADDITIONAL_TEXT_LOADED, b8>(textIndex); }
	static b8 HasThisAdditionalTextLoaded(const ch *textName, u32 textIndex) { return NativeInvoke::Invoke<NATIVE_HAS_THIS_ADDITIONAL_TEXT_LOADED, b8>(textName, textIndex); }
	static b8 IsFontLoaded(eTextFont font) { return NativeInvoke::Invoke<NATIVE_IS_FONT_LOADED, b8>(font); }
	static b8 IsStreamingAdditionalText(u32 textIndex) { return NativeInvoke::Invoke<NATIVE_IS_STREAMING_ADDITIONAL_TEXT, b8>(textIndex); }
	static void LoadAdditionalText(const ch *textName, u32 textIndex) { NativeInvoke::Invoke<NATIVE_LOAD_ADDITIONAL_TEXT, ScriptVoid>(textName, textIndex); }
	static void LoadTextFont(eTextFont font) { NativeInvoke::Invoke<NATIVE_LOAD_TEXT_FONT, ScriptVoid>(font); }
	static void Print(const ch *gxtName, u32 timeMS, b8 enable) { NativeInvoke::Invoke<NATIVE_PRINT, ScriptVoid>(gxtName, timeMS, enable); }
	static void PrintBig(const ch *gxtName, u32 timeMS, b8 enable) { NativeInvoke::Invoke<NATIVE_PRINT_BIG, ScriptVoid>(gxtName, timeMS, enable); }
	static void PrintHelp(const ch *gxtName) { NativeInvoke::Invoke<NATIVE_PRINT_HELP, ScriptVoid>(gxtName); }
	static void PrintHelpForever(const ch *gxtName) { NativeInvoke::Invoke<NATIVE_PRINT_HELP_FOREVER, ScriptVoid>(gxtName); }
	static void PrintHelpForeverWithNumber(const ch *gxtName, i32 value) { NativeInvoke::Invoke<NATIVE_PRINT_HELP_FOREVER_WITH_NUMBER, ScriptVoid>(gxtName, value); }
	static void PrintHelpForeverWithString(const ch *gxtName, const ch *gxtText) { NativeInvoke::Invoke<NATIVE_PRINT_HELP_FOREVER_WITH_STRING, ScriptVoid>(gxtName, gxtText); }
	static void PrintHelpForeverWithStringNoSound(const ch *gxtName, const ch *gxtText) { NativeInvoke::Invoke<NATIVE_PRINT_HELP_FOREVER_WITH_STRING_NO_SOUND, ScriptVoid>(gxtName, gxtText); }
	static void PrintHelpWithNumber(const ch *gxtName, i32 value) { NativeInvoke::Invoke<NATIVE_PRINT_HELP_WITH_NUMBER, ScriptVoid>(gxtName, value); }
	static void PrintHelpWithString(const ch *gxtName, const ch *gxtText) { NativeInvoke::Invoke<NATIVE_PRINT_HELP_WITH_STRING, ScriptVoid>(gxtName, gxtText); }
	static void PrintHelpWithStringNoSound(const ch *gxtName, const ch *gxtText) { NativeInvoke::Invoke<NATIVE_PRINT_HELP_WITH_STRING_NO_SOUND, ScriptVoid>(gxtName, gxtText); }
	static void PrintNow(const ch *gxtName, u32 timeMS, b8 enable) { NativeInvoke::Invoke<NATIVE_PRINT_NOW, ScriptVoid>(gxtName, timeMS, enable); }
	static void PrintStringInString(const ch *gxtName, const ch *gxtText, u32 timeMS, b8 enable) { NativeInvoke::Invoke<NATIVE_PRINT_STRING_IN_STRING, ScriptVoid>(gxtName, gxtText, timeMS, enable); }
	static void PrintStringInStringNow(const ch *gxtName, const ch *gxtText, u32 timeMS, b8 enable) { NativeInvoke::Invoke<NATIVE_PRINT_STRING_IN_STRING_NOW, ScriptVoid>(gxtName, gxtText, timeMS, enable); }
	static void PrintStringWithLiteralStringNow(const ch *gxtName, const char *text, u32 timeMS, b8 enable) { NativeInvoke::Invoke<NATIVE_PRINT_STRING_WITH_LITERAL_STRING_NOW, ScriptVoid>(gxtName, text, timeMS, enable); }
	static void PrintWith2Numbers(const ch *gxtName, i32 value1, i32 value2, u32 timeMS, b8 enable) { NativeInvoke::Invoke<NATIVE_PRINT_WITH_2_NUMBERS, ScriptVoid>(gxtName, value1, value2, timeMS, enable); }
	static void PrintWith2NumbersNow(const ch *gxtName, i32 value1, i32 value2, u32 timeMS, b8 enable) { NativeInvoke::Invoke<NATIVE_PRINT_WITH_2_NUMBERS_NOW, ScriptVoid>(gxtName, value1, value2, timeMS, enable); }
	static void PrintWithNumber(const ch *gxtName, i32 value, u32 timeMS, b8 enable) { NativeInvoke::Invoke<NATIVE_PRINT_WITH_NUMBER, ScriptVoid>(gxtName, value, timeMS, enable); }
	static void PrintWithNumberBig(const ch *gxtName, i32 value, u32 timeMS, b8 enable) { NativeInvoke::Invoke<NATIVE_PRINT_WITH_NUMBER_BIG, ScriptVoid>(gxtName, value, timeMS, enable); }
	static void PrintWithNumberNow(const ch *gxtName, i32 value, u32 timeMS, b8 enable) { NativeInvoke::Invoke<NATIVE_PRINT_WITH_NUMBER_NOW, ScriptVoid>(gxtName, value, timeMS, enable); }
	static void PrintFloat(f32 value) { NativeInvoke::Invoke<NATIVE_PRINTFLOAT, ScriptVoid>(value); }
	static void PrintInt(i32 value) { NativeInvoke::Invoke<NATIVE_PRINTINT, ScriptVoid>(value); }
	static void PrintNL() { NativeInvoke::Invoke<NATIVE_PRINTNL, ScriptVoid>(); }
	static void PrintString(const ch *value) { NativeInvoke::Invoke<NATIVE_PRINTSTRING, ScriptVoid>(value); }
	static void PrintVector(f32 x, f32 y, f32 z) { NativeInvoke::Invoke<NATIVE_PRINTVECTOR, ScriptVoid>(x, y, z); }
	static void RequestAdditionalText(const ch *textName, u32 textIndex) { NativeInvoke::Invoke<NATIVE_REQUEST_ADDITIONAL_TEXT, ScriptVoid>(textName, textIndex); }
	static void SetTextScale(f32 w, f32 h) { NativeInvoke::Invoke<NATIVE_SET_TEXT_SCALE, ScriptVoid>(w, h); }
	static void SetTextBackground(b8 value) { NativeInvoke::Invoke<NATIVE_SET_TEXT_BACKGROUND, ScriptVoid>(value); }
	static void SetTextCentre(b8 value) { NativeInvoke::Invoke<NATIVE_SET_TEXT_CENTRE, ScriptVoid>(value); }
	static void SetTextColour(u8 r, u8 g, u8 b, u8 a) { NativeInvoke::Invoke<NATIVE_SET_TEXT_COLOUR, ScriptVoid>(r, g, b, a); }
	static void SetTextDrawBeforeFade(b8 value) { NativeInvoke::Invoke<NATIVE_SET_TEXT_DRAW_BEFORE_FADE, ScriptVoid>(value); }
	static void SetTextDropshadow(b8 displayShadow, u8 r, u8 g, u8 b, u8 a) { NativeInvoke::Invoke<NATIVE_SET_TEXT_DROPSHADOW, ScriptVoid>(displayShadow, r, g, b, a); }
	static void SetTextEdge(b8 displayEdge, u8 r, u8 g, u8 b, u8 a) { NativeInvoke::Invoke<NATIVE_SET_TEXT_EDGE, ScriptVoid>(displayEdge, r, g, b, a); }
	static void SetTextFont(eTextFont font) { NativeInvoke::Invoke<NATIVE_SET_TEXT_FONT, ScriptVoid>(font); }
	static void SetTextJustify(b8 value) { NativeInvoke::Invoke<NATIVE_SET_TEXT_JUSTIFY, ScriptVoid>(value); }
	//static void SetTextLineDisplay(u32 unk1, u32 unk2) { NativeInvoke::Invoke<NATIVE_SET_TEXT_LINE_DISPLAY, ScriptVoid>(unk1, unk2); }
	//static void SetTextLineHeightMult(f32 lineHeight) { NativeInvoke::Invoke<NATIVE_SET_TEXT_LINE_HEIGHT_MULT, ScriptVoid>(lineHeight); }
	static void SetTextProportional(b8 value) { NativeInvoke::Invoke<NATIVE_SET_TEXT_PROPORTIONAL, ScriptVoid>(value); }
	static void SetTextRenderId(u32 renderId) { NativeInvoke::Invoke<NATIVE_SET_TEXT_RENDER_ID, ScriptVoid>(renderId); }
	static void SetTextRightJustify(b8 value) { NativeInvoke::Invoke<NATIVE_SET_TEXT_RIGHT_JUSTIFY, ScriptVoid>(value); }
	//static void SetTextToUseTextFileColours(b8 value) { NativeInvoke::Invoke<NATIVE_SET_TEXT_TO_USE_TEXT_FILE_COLOURS, ScriptVoid>(value); }
	static void SetTextUseUnderscore(b8 value) { NativeInvoke::Invoke<NATIVE_SET_TEXT_USE_UNDERSCORE, ScriptVoid>(value); }
	static void SetTextWrap(f32 unk1, f32 unk2) { NativeInvoke::Invoke<NATIVE_SET_TEXT_WRAP, ScriptVoid>(unk1, unk2); }

	// Textures
	static u32 GetTexture(u32 dictionary, const ch *textureName) { return NativeInvoke::Invoke<NATIVE_GET_TEXTURE, u32>(dictionary, textureName); }
	static Texture GetTextureFromStreamedTxd(const ch *txdName, const ch *textureName) { return NativeInvoke::Invoke<NATIVE_GET_TEXTURE_FROM_STREAMED_TXD, Texture>(txdName, textureName); }
	static u32 GetTxd(const ch *txdName) { return NativeInvoke::Invoke<NATIVE_GET_TXD, u32>(txdName); }
	static b8 HasStreamedTxdLoaded(const ch *txdName) { return NativeInvoke::Invoke<NATIVE_HAS_STREAMED_TXD_LOADED, b8>(txdName); }
	static u32 LoadTxd(const ch *txdName) { return NativeInvoke::Invoke<NATIVE_LOAD_TXD, u32>(txdName); }
	static void MarkStreamedTxdAsNoLongerNeeded(const ch *txdName) { NativeInvoke::Invoke<NATIVE_MARK_STREAMED_TXD_AS_NO_LONGER_NEEDED, ScriptVoid>(txdName); }
	static void ReleaseTexture(u32 texture) { NativeInvoke::Invoke<NATIVE_RELEASE_TEXTURE, ScriptVoid>(texture); }
	static void RequestStreamedTxd(const ch *txdName, b8 unknown) { NativeInvoke::Invoke<NATIVE_REQUEST_STREAMED_TXD, ScriptVoid>(txdName, unknown); }
	static void RemoveTxd(u32 txd) { NativeInvoke::Invoke<NATIVE_REMOVE_TXD, ScriptVoid>(txd); }

	// Stats
	static void DecrementIntStat(eIntStatistic stat, u32 amount) { NativeInvoke::Invoke<NATIVE_DECREMENT_INT_STAT, ScriptVoid>(stat, amount); }
	static u32 GetIntStat(eIntStatistic stat) { return NativeInvoke::Invoke<NATIVE_GET_INT_STAT, u32>(stat); }
	static void IncrementFloatStatNoMessage(eFloatStatistic stat, f32 value) { NativeInvoke::Invoke<NATIVE_INCREMENT_FLOAT_STAT_NO_MESSAGE, ScriptVoid>(stat, value); }
	static void IncrementIntStat(eIntStatistic stat, u32 value) { NativeInvoke::Invoke<NATIVE_INCREMENT_INT_STAT, ScriptVoid>(stat, value); }
	static void IncrementIntStatNoMessage(eIntStatistic stat, u32 value) { NativeInvoke::Invoke<NATIVE_INCREMENT_INT_STAT_NO_MESSAGE, ScriptVoid>(stat, value); }
	static void SetIntStat(eIntStatistic stat, u32 value) { NativeInvoke::Invoke<NATIVE_SET_INT_STAT, ScriptVoid>(stat, value); }

	// Pad (controls)
	static b8 GetPadPitchRoll(u32 padIndex, f32 *pPitch, f32 *pRoll) { return NativeInvoke::Invoke<NATIVE_GET_PAD_PITCH_ROLL, b8>(padIndex, pPitch, pRoll); }
	static void GetPositionOfAnalogueSticks(u32 padIndex, int *pLeftX, int *pLeftY, int *pRightX, int *pRightY) { NativeInvoke::Invoke<NATIVE_GET_POSITION_OF_ANALOGUE_STICKS, ScriptVoid>(padIndex, pLeftX, pLeftY, pRightX, pRightY); }
	static b8 IsButtonJustPressed(u32 padIndex, ePadButton button) { return NativeInvoke::Invoke<NATIVE_IS_BUTTON_JUST_PRESSED, b8>(padIndex, button); }
	static b8 IsButtonPressed(u32 padIndex, ePadButton button) { return NativeInvoke::Invoke<NATIVE_IS_BUTTON_PRESSED, b8>(padIndex, button); }

	// Sound
	static int GetSoundId() { return NativeInvoke::Invoke<NATIVE_GET_SOUND_ID, int>(); }
	static b8 HasSoundFinished(eSound sound) { return NativeInvoke::Invoke<NATIVE_HAS_SOUND_FINISHED, b8>(sound); }
	static void PlayAudioEvent(ch *name) { NativeInvoke::Invoke<NATIVE_PLAY_AUDIO_EVENT, ScriptVoid>(name); }
	static void PlaySoundFrontend(int sound, const char* soundName) { NativeInvoke::Invoke<NATIVE_PLAY_SOUND_FRONTEND, ScriptVoid>(sound, soundName); }
	static void PlaySoundFromPos(int sound, const char* soundName, float x, float y, float z) { NativeInvoke::Invoke<NATIVE_PLAY_SOUND_FROM_POSITION, ScriptVoid>(sound, soundName, x, y, z); }
	static void SetVariableOnSound(eSound sound, ch *varname, f32 value) { NativeInvoke::Invoke<NATIVE_SET_VARIABLE_ON_SOUND, ScriptVoid>(sound, varname, value); }
	static void StopSound(int sound) { NativeInvoke::Invoke<NATIVE_STOP_SOUND, ScriptVoid>(sound); }
	static void ReleaseSoundId(int sound) { NativeInvoke::Invoke<NATIVE_RELEASE_SOUND_ID, ScriptVoid>(sound); }

	// Time
	static void ForceTimeOfDay(u32 hour, u32 minute) { NativeInvoke::Invoke<NATIVE_FORCE_TIME_OF_DAY, ScriptVoid>(hour, minute); }
	static void ForwardToTimeOfDay(u32 hour, u32 minute) { NativeInvoke::Invoke<NATIVE_FORWARD_TO_TIME_OF_DAY, ScriptVoid>(hour, minute); }
	static void GetCurrentDate(u32 *day, u32 *month) { NativeInvoke::Invoke<NATIVE_GET_CURRENT_DATE, ScriptVoid>(day, month); }
	static u32 GetCurrentDayOfWeek() { return NativeInvoke::Invoke<NATIVE_GET_CURRENT_DAY_OF_WEEK, u32>(); }
	static u32 GetHoursOfDay() { return NativeInvoke::Invoke<NATIVE_GET_HOURS_OF_DAY, u32>(); }
	static u32 GetMinutesOfDay() { return NativeInvoke::Invoke<NATIVE_GET_MINUTES_OF_DAY, u32>(); }
	static u32 GetMinutesToTimeOfDay(u32 hour, u32 minute) { return NativeInvoke::Invoke<NATIVE_GET_MINUTES_TO_TIME_OF_DAY, u32>(hour, minute); }
	static void GetTimeOfDay(u32 *hour, u32 *minute) { NativeInvoke::Invoke<NATIVE_GET_TIME_OF_DAY, ScriptVoid>(hour, minute); }
	static void ReleaseTimeOfDay() { NativeInvoke::Invoke<NATIVE_RELEASE_TIME_OF_DAY, ScriptVoid>(); }
	static void SetTimeOfDay(u32 hour, u32 minute) { NativeInvoke::Invoke<NATIVE_SET_TIME_OF_DAY, ScriptVoid>(hour, minute); }
	static void SetTimeOneDayBack() { NativeInvoke::Invoke<NATIVE_SET_TIME_ONE_DAY_BACK, ScriptVoid>(); }
	static void SetTimeOneDayForward() { NativeInvoke::Invoke<NATIVE_SET_TIME_ONE_DAY_FORWARD, ScriptVoid>(); }
	static void SetTimeScale(f32 scale) { NativeInvoke::Invoke<NATIVE_SET_TIME_SCALE, ScriptVoid>(scale); }

	// Fires
	static b8 IsScriptFireExtinguished(FireId fire) { return NativeInvoke::Invoke<NATIVE_IS_SCRIPT_FIRE_EXTINGUISHED, b8>(fire); }
	static void RemoveScriptFire(FireId fire) { NativeInvoke::Invoke<NATIVE_REMOVE_SCRIPT_FIRE, ScriptVoid>(fire); }
	//static FireId StartCarFire(Vehicle vehicle) { return NativeInvoke::Invoke<NATIVE_START_CAR_FIRE, ScriptAny>(vehicle); }
	static FireId StartCharFire(Ped ped) { return NativeInvoke::Invoke<NATIVE_START_CHAR_FIRE, ScriptAny>(ped); }
	static FireId StartScriptFire(f32 x, f32 y, f32 z, u8 numGenerationsAllowed = 0, u32 strength = 1) { return NativeInvoke::Invoke<NATIVE_START_SCRIPT_FIRE, u32>(x, y, z, numGenerationsAllowed, strength); }

	// Radio
	static void DisableFrontEndRadio() { NativeInvoke::Invoke<NATIVE_DISABLE_FRONTEND_RADIO, ScriptVoid>(); }
	static void EnableFrontEndRadio() { NativeInvoke::Invoke<NATIVE_ENABLE_FRONTEND_RADIO, ScriptVoid>(); }
	static void FreezeRadioStation(const ch *stationName) { NativeInvoke::Invoke<NATIVE_FREEZE_RADIO_STATION, ScriptVoid>(stationName); }
	static eRadioStation GetPlayerRadioStationIndex() { return NativeInvoke::Invoke<NATIVE_GET_PLAYER_RADIO_STATION_INDEX, eRadioStation>(); }
	static const ch *GetPlayerRadioStationName() { return NativeInvoke::Invoke<NATIVE_GET_PLAYER_RADIO_STATION_NAME, const ch *>(); }
	static b8 IsRadioRetuning() { return NativeInvoke::Invoke<NATIVE_IS_RADIO_RETUNING, b8>(); }
	static void RetuneRadioToStationIndex(eRadioStation radioStation) { NativeInvoke::Invoke<NATIVE_RETUNE_RADIO_TO_STATION_INDEX, ScriptVoid>(radioStation); }

	// Game/Misc
	static void AbortAllGarageActivity() { NativeInvoke::Invoke<NATIVE_ABORT_ALL_GARAGE_ACTIVITY, ScriptVoid>(); }
	static void ActivateCheat(eCheat cheat) { NativeInvoke::Invoke<NATIVE_ACTIVATE_CHEAT, ScriptVoid>(cheat); }
	static void ActivateFrontEnd() { NativeInvoke::Invoke<NATIVE_ACTIVATE_FRONTEND, u32>(); }
	static void ActivateSaveMenu() { NativeInvoke::Invoke<NATIVE_ACTIVATE_SAVE_MENU, ScriptVoid>(); }
	static void AddExplosion(f32 x, f32 y, f32 z, u32 p3, f32 p4, u32 p5, u32 p6, f32 p7) { NativeInvoke::Invoke<NATIVE_ADD_EXPLOSION, ScriptVoid>(x, y, z, p3, p4, p5, p6, p7); }
	static void AllowEmergencyServices(b8 allow) { NativeInvoke::Invoke<NATIVE_ALLOW_EMERGENCY_SERVICES, ScriptVoid>(allow); }
	static void AllowGameToPauseForStreaming(b8 allow) { NativeInvoke::Invoke<NATIVE_ALLOW_GAME_TO_PAUSE_FOR_STREAMING, ScriptVoid>(allow); }
	static void AllowStuntJumpsToTrigger(b8 allow) { NativeInvoke::Invoke<NATIVE_ALLOW_STUNT_JUMPS_TO_TRIGGER, ScriptVoid>(allow); }
	static b8 AreWidescreenBordersActive() { return NativeInvoke::Invoke<NATIVE_ARE_WIDESCREEN_BORDERS_ACTIVE, b8>(); }
	static ScriptAny AwardAchievement(eAchievement achievement) { return NativeInvoke::Invoke<NATIVE_AWARD_ACHIEVEMENT, ScriptAny>(achievement); }
	static b8 CanPhoneBeSeenOnScreen() { return NativeInvoke::Invoke<NATIVE_CAN_PHONE_BE_SEEN_ON_SCREEN, b8>(); }
	static void CancelOverrideRestart() { NativeInvoke::Invoke<NATIVE_CANCEL_OVERRIDE_RESTART, ScriptVoid>(); }
	static void ClearShakePlayerPadWhenControllerDisabled() { NativeInvoke::Invoke<NATIVE_CLEAR_SHAKE_PLAYERPAD_WHEN_CONTROLLER_DISABLED, ScriptVoid>(); }
	static void ClearTimeCycleModifier() { NativeInvoke::Invoke<NATIVE_CLEAR_TIMECYCLE_MODIFIER, ScriptVoid>(); }
	static void DeactivateFrontEnd() { NativeInvoke::Invoke<NATIVE_DEACTIVATE_FRONTEND, ScriptVoid>(); }
	static b8 DidSaveCompleteSuccessfully() { return NativeInvoke::Invoke<NATIVE_DID_SAVE_COMPLETE_SUCCESSFULLY, b8>(); }
	static void DisablePauseMenu(b8 disabled) { NativeInvoke::Invoke<NATIVE_DISABLE_PAUSE_MENU, ScriptVoid>(disabled); }
	static void DisablePoliceScanner() { NativeInvoke::Invoke<NATIVE_DISABLE_POLICE_SCANNER, ScriptVoid>(); }
	static void DisplayAmmo(b8 display) { NativeInvoke::Invoke<NATIVE_DISPLAY_AMMO, ScriptVoid>(display); }
	static void DisplayAreaName(b8 display) { NativeInvoke::Invoke<NATIVE_DISPLAY_AREA_NAME, ScriptVoid>(display); }
	static void DisplayCash(b8 display) { NativeInvoke::Invoke<NATIVE_DISPLAY_CASH, ScriptVoid>(display); }
	static void DisplayFrontEndMapBlips(b8 display) { NativeInvoke::Invoke<NATIVE_DISPLAY_FRONTEND_MAP_BLIPS, ScriptVoid>(display); }
	static void DisplayHUD(b8 display) { NativeInvoke::Invoke<NATIVE_DISPLAY_HUD, ScriptVoid>(display); }
	static void DisplayRadar(b8 display) { NativeInvoke::Invoke<NATIVE_DISPLAY_RADAR, ScriptVoid>(display); }
	static void DisplaySniperScopeThisFrame() { NativeInvoke::Invoke<NATIVE_DISPLAY_SNIPER_SCOPE_THIS_FRAME, ScriptVoid>(); }
	static void DoAutoSave() { NativeInvoke::Invoke<NATIVE_DO_AUTO_SAVE, ScriptVoid>(); }
	static void DoScreenFadeIn(u32 timeMS) { NativeInvoke::Invoke<NATIVE_DO_SCREEN_FADE_IN, ScriptVoid>(timeMS); }
	static void DoScreenFadeInUnhacked(u32 timeMS) { NativeInvoke::Invoke<NATIVE_DO_SCREEN_FADE_IN_UNHACKED, ScriptVoid>(timeMS); }
	static void DoScreenFadeOut(u32 timeMS) { NativeInvoke::Invoke<NATIVE_DO_SCREEN_FADE_OUT, ScriptVoid>(timeMS); }
	static void DoScreenFadeOutUnhacked(u32 timeMS) { NativeInvoke::Invoke<NATIVE_DO_SCREEN_FADE_OUT_UNHACKED, ScriptVoid>(timeMS); }
	static void DrawRect(f32 x1, f32 y1, f32 x2, f32 y2, u8 r, u8 g, u8 b, u8 a) { NativeInvoke::Invoke<NATIVE_DRAW_RECT, ScriptVoid>(x1, y1, x2, y2, r, g, b, a); }
	static void EnableMaxAmmoCap(b8 enable) { NativeInvoke::Invoke<NATIVE_ENABLE_MAX_AMMO_CAP, ScriptVoid>(enable); }
	static void EnablePoliceScanner() { NativeInvoke::Invoke<NATIVE_ENABLE_POLICE_SCANNER, ScriptVoid>(); }
	static void EnableSceneStreaming(b8 enable) { NativeInvoke::Invoke<NATIVE_ENABLE_SCENE_STREAMING, ScriptVoid>(enable); }
	static void FakeDeathArrest() { NativeInvoke::Invoke<NATIVE_FAKE_DEATHARREST, ScriptVoid>(); }
	static void FindStreetNameAtPosition(f32 pX, f32 pY, f32 pZ, u32 *strHash0, u32 *strHash1) { NativeInvoke::Invoke<NATIVE_FIND_STREET_NAME_AT_POSITION, ScriptVoid>(pX, pY, pZ, strHash0, strHash1); }
	static void FireSingleBullet(f32 x, f32 y, f32 z, f32 targetX, f32 targetY, f32 targetZ, u32 unknown) { NativeInvoke::Invoke<NATIVE_FIRE_SINGLE_BULLET, ScriptVoid>(x, y, z, targetX, targetY, targetZ, unknown); }
	static void FlashWeaponIcon(b8 on) { NativeInvoke::Invoke<NATIVE_FLASH_WEAPON_ICON, ScriptVoid>(on); }
	static void ForceInitialPlayerStation(const ch *stationName) { NativeInvoke::Invoke<NATIVE_FORCE_INITIAL_PLAYER_STATION, ScriptVoid>(stationName); }
	static void ForceLoadingScreen(b8 force) { NativeInvoke::Invoke<NATIVE_FORCE_LOADING_SCREEN, ScriptVoid>(force); }
	static void GetCorrectedColour(u32 r, u32 g, u32 b, u32 *pR, u32 *pG, u32 *pB) { NativeInvoke::Invoke<NATIVE_GET_CORRECTED_COLOUR, ScriptVoid>(r, g, b, pR, pG, pB); }
	static b8 GetCreateRandomCops() { return NativeInvoke::Invoke<NATIVE_GET_CREATE_RANDOM_COPS, b8>(); }
	static u32 GetCurrentEpisode() { return NativeInvoke::Invoke<NATIVE_GET_CURRENT_EPISODE, u32>(); }
	static eLanguage GetCurrentLanguage() { return NativeInvoke::Invoke<NATIVE_GET_CURRENT_LANGUAGE, eLanguage>(); }
	static u32 GetEpisodeIndexFromSummons() { return NativeInvoke::Invoke<NATIVE_GET_EPISODE_INDEX_FROM_SUMMONS, ScriptAny>(); }
	static const ch * GetEpisodeName(u32 episodeIndex) { return NativeInvoke::Invoke<NATIVE_GET_EPISODE_NAME, const ch *>(episodeIndex); }
	static f32 GetFloatStat(eFloatStatistic stat) { return NativeInvoke::Invoke<NATIVE_GET_FLOAT_STAT, f32>(stat); }
	static void GetFrameTime(f32 *time) { NativeInvoke::Invoke<NATIVE_GET_FRAME_TIME, ScriptVoid>(time); }
	static void GetGameTimer(u32 *pTimer) { NativeInvoke::Invoke<NATIVE_GET_GAME_TIMER, ScriptVoid>(pTimer); }
	static u32 GetHashKey(const ch *value) { return NativeInvoke::Invoke<NATIVE_GET_HASH_KEY, ScriptAny>(value); }
	static void GetHUDColour(eHUDType type, u32 *pR, u32 *pG, u32 *pB, ScriptAny *pUnknown) { NativeInvoke::Invoke<NATIVE_GET_HUD_COLOUR, ScriptVoid>(type, pR, pG, pB, pUnknown); }
	static u32 GetIdOfThisThread() { return NativeInvoke::Invoke<NATIVE_GET_ID_OF_THIS_THREAD, u32>(); }
	static b8 GetIsDepositAnimRunning() { return NativeInvoke::Invoke<NATIVE_GET_IS_DEPOSIT_ANIM_RUNNING, b8>(); }
	static b8 GetIsHiDef() { return NativeInvoke::Invoke<NATIVE_GET_IS_HIDEF, b8>(); }
	static b8 GetIsWidescreen() { return NativeInvoke::Invoke<NATIVE_GET_IS_WIDESCREEN, b8>(); }
	static u32 GetLeftPlayerCashToReachLevel(u32 playerRank) { return NativeInvoke::Invoke<NATIVE_GET_LEFT_PLAYER_CASH_TO_REACH_LEVEL, u32>(playerRank); }
	static eMapArea GetMapAreaFromCoords(f32 x, f32 y, f32 z) { return NativeInvoke::Invoke<NATIVE_GET_MAP_AREA_FROM_COORDS, eMapArea>(x, y, z); }
	static b8 GetMissionFlag() { return NativeInvoke::Invoke<NATIVE_GET_MISSION_FLAG, b8>(); }
	static void GetMaxWantedLevel(u32 *pMaxWantedLevel) { NativeInvoke::Invoke<NATIVE_GET_MAX_WANTED_LEVEL, ScriptVoid>(pMaxWantedLevel); }
	static const ch *GetNameOfInfoZone(f32 x, f32 y, f32 z) { return NativeInvoke::Invoke<NATIVE_GET_NAME_OF_INFO_ZONE, const ch *>(x, y, z); }
	static const ch *GetNameOfZone(f32 x, f32 y, f32 z) { return NativeInvoke::Invoke<NATIVE_GET_NAME_OF_ZONE, const ch *>(x, y, z); }
	static u32 GetNumStreamingRequests() { return NativeInvoke::Invoke<NATIVE_GET_NUM_STREAMING_REQUESTS, u32>(); }
	static u32 GetNumberOfInstancesOfStreamedScript(const ch *scriptName) { return NativeInvoke::Invoke<NATIVE_GET_NUMBER_OF_INSTANCES_OF_STREAMED_SCRIPT, u32>(scriptName); }
	static u32 GetNumberOfPlayers() { return NativeInvoke::Invoke<NATIVE_GET_NUMBER_OF_PLAYERS, u32>(); }
	static u32 GetTimeSinceLastArrest() { return NativeInvoke::Invoke<NATIVE_GET_TIME_SINCE_LAST_ARREST, u32>(); }
	static u32 GetTimeSinceLastDeath() { return NativeInvoke::Invoke<NATIVE_GET_TIME_SINCE_LAST_DEATH, u32>(); }
	static b8 HasAchievementBeenPassed(eAchievement achievement) { return NativeInvoke::Invoke<NATIVE_HAS_ACHIEVEMENT_BEEN_PASSED, b8>(achievement); }
	static b8 HasDeathArrestExecuted() { return NativeInvoke::Invoke<NATIVE_HAS_DEATHARREST_EXECUTED, b8>(); }
	static b8 HasResprayHappened() { return NativeInvoke::Invoke<NATIVE_HAS_RESPRAY_HAPPENED, b8>(); }
	static b8 HasScriptLoaded(const ch *scriptName) { return NativeInvoke::Invoke<NATIVE_HAS_SCRIPT_LOADED, b8>(scriptName); }
	static b8 HaveAnimsLoaded(const ch *animName) { return NativeInvoke::Invoke<NATIVE_HAVE_ANIMS_LOADED, b8>(animName); }
	static b8 HaveRequestedPathNodesBeenLoaded(u32 requestId) { return NativeInvoke::Invoke<NATIVE_HAVE_REQUESTED_PATH_NODES_BEEN_LOADED, b8>(requestId); }
	static void HideHelpTextThisFrame() { NativeInvoke::Invoke<NATIVE_HIDE_HELP_TEXT_THIS_FRAME, ScriptVoid>(); }
	static void HideHUDAndRadarThisFrame() { NativeInvoke::Invoke<NATIVE_HIDE_HUD_AND_RADAR_THIS_FRAME, ScriptVoid>(); }
	static b8 IsAussieVersion() { return NativeInvoke::Invoke<NATIVE_IS_AUSSIE_VERSION, b8>(); }
	static b8 IsAutoAimingOn() { return NativeInvoke::Invoke<NATIVE_IS_AUTO_AIMING_ON, b8>(); }
	static b8 IsAutoSaveInProgress() { return NativeInvoke::Invoke<NATIVE_IS_AUTO_SAVE_IN_PROGRESS, b8>(); }
	static b8 IsInteriorScene() { return NativeInvoke::Invoke<NATIVE_IS_INTERIOR_SCENE, b8>(); }
	static b8 IsFrontEndFading() { return NativeInvoke::Invoke<NATIVE_IS_FRONTEND_FADING, b8>(); }
	static b8 IsGameInControlOfMusic() { return NativeInvoke::Invoke<NATIVE_IS_GAME_IN_CONTROL_OF_MUSIC, b8>(); }
	static b8 IsGermanVersion() { return NativeInvoke::Invoke<NATIVE_IS_GERMAN_VERSION, b8>(); }
	static b8 IsHelpMessageBeingDisplayed() { return NativeInvoke::Invoke<NATIVE_IS_HELP_MESSAGE_BEING_DISPLAYED, b8>(); }
	static b8 IsHintRunning() { return NativeInvoke::Invoke<NATIVE_IS_HINT_RUNNING, b8>(); }
	static b8 IsHUDPreferenceSwitchedOn() { return NativeInvoke::Invoke<NATIVE_IS_HUD_PREFERENCE_SWITCHED_ON, b8>(); }
	static b8 IsHUDReticuleComplex() { return NativeInvoke::Invoke<NATIVE_IS_HUD_RETICULE_COMPLEX, b8>(); }
	static b8 IsInSpectatorMode() { return NativeInvoke::Invoke<NATIVE_IS_IN_SPECTATOR_MODE, b8>(); }
	static b8 IsLookInverted() { return NativeInvoke::Invoke<NATIVE_IS_LOOK_INVERTED, b8>(); }
	static b8 IsMemoryCardInUse() { return NativeInvoke::Invoke<NATIVE_IS_MEMORY_CARD_IN_USE, b8>(); }
	static b8 IsMessageBeingDisplayed() { return NativeInvoke::Invoke<NATIVE_IS_MESSAGE_BEING_DISPLAYED, b8>(); }
	static b8 IsMinigameInProgress() { return NativeInvoke::Invoke<NATIVE_IS_MINIGAME_IN_PROGRESS, b8>(); }
	static b8 IsMissionCompletePlaying() { return NativeInvoke::Invoke<NATIVE_IS_MISSION_COMPLETE_PLAYING, b8>(); }
	static b8 IsMobilePhoneCallOngoing() { return NativeInvoke::Invoke<NATIVE_IS_MOBILE_PHONE_CALL_ONGOING, b8>(); }
	static b8 IsMobilePhoneRadioActive() { return NativeInvoke::Invoke<NATIVE_IS_MOBILE_PHONE_RADIO_ACTIVE, b8>(); }
	static b8 IsPauseMenuActive() { return NativeInvoke::Invoke<NATIVE_IS_PAUSE_MENU_ACTIVE, b8>(); }
	static b8 IsPayNSprayActive() { return NativeInvoke::Invoke<NATIVE_IS_PAY_N_SPRAY_ACTIVE, b8>(); }
	static b8 IsPlayerBeingArrested() { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_BEING_ARRESTED, b8>(); }
	static b8 IsPlayerOnline() { return NativeInvoke::Invoke<NATIVE_IS_PLAYER_ONLINE, b8>(); }
	static b8 IsScreenFadedIn() { return NativeInvoke::Invoke<NATIVE_IS_SCREEN_FADED_IN, b8>(); }
	static b8 IsScreenFadedOut() { return NativeInvoke::Invoke<NATIVE_IS_SCREEN_FADED_OUT, b8>(); }
	static b8 IsScreenFading() { return NativeInvoke::Invoke<NATIVE_IS_SCREEN_FADING, b8>(); }
	static b8 IsScreenFadingIn() { return NativeInvoke::Invoke<NATIVE_IS_SCREEN_FADING_IN, b8>(); }
	static b8 IsScreenFadingOut() { return NativeInvoke::Invoke<NATIVE_IS_SCREEN_FADING_OUT, b8>(); }
	static b8 IsSniperInverted() { return NativeInvoke::Invoke<NATIVE_IS_SNIPER_INVERTED, b8>(); }
	static b8 IsSystemUIShowing() { return NativeInvoke::Invoke<NATIVE_IS_SYSTEM_UI_SHOWING, b8>(); }
	static b8 IsThisAMinigameScript() { return NativeInvoke::Invoke<NATIVE_IS_THIS_A_MINIGAME_SCRIPT, b8>(); }
	static b8 IsThreadActive(u32 threadId) { return NativeInvoke::Invoke<NATIVE_IS_THREAD_ACTIVE, b8>(threadId); }
	static u32 LoadAllPathNodes(bool value) { return NativeInvoke::Invoke<NATIVE_LOAD_ALL_PATH_NODES, ScriptAny>(value); }
	static void MarkScriptAsNoLongerNeeded(const ch *scriptName) { NativeInvoke::Invoke<NATIVE_MARK_SCRIPT_AS_NO_LONGER_NEEDED, ScriptVoid>(scriptName); }
	static void PauseGame() { NativeInvoke::Invoke<NATIVE_PAUSE_GAME, ScriptVoid>(); }
	static void PlayMovie() { NativeInvoke::Invoke<NATIVE_PLAY_MOVIE, ScriptVoid>(); }
	static void PopulateNow() { NativeInvoke::Invoke<NATIVE_POPULATE_NOW, ScriptVoid>(); }
	static void RequestAnims(const ch *animName) { NativeInvoke::Invoke<NATIVE_REQUEST_ANIMS, ScriptVoid>(animName); }
	static void ReleaseMovie() { NativeInvoke::Invoke<NATIVE_RELEASE_MOVIE, ScriptVoid>(); }
	static void ReleaseWeather() { NativeInvoke::Invoke<NATIVE_RELEASE_WEATHER, ScriptVoid>(); }
	static void RemoveAnims(const ch *animName) { NativeInvoke::Invoke<NATIVE_REMOVE_ANIMS, ScriptVoid>(animName); }
	static void RemoveCoverPoint(CoverPoint coverPoint) { NativeInvoke::Invoke<NATIVE_REMOVE_COVER_POINT, ScriptVoid>(coverPoint); }
	static void RemoveIpl(const ch *iplName) { NativeInvoke::Invoke<NATIVE_REMOVE_IPL, ScriptVoid>(iplName); }
	static void RequestIpl(const ch *iplName) { NativeInvoke::Invoke<NATIVE_REQUEST_IPL, ScriptVoid>(iplName); }
	static void SetCameraControlsDisabledWithPlayerControls(bool value) { NativeInvoke::Invoke<NATIVE_SET_CAMERA_CONTROLS_DISABLED_WITH_PLAYER_CONTROLS, ScriptVoid>(value); }
	static void SetMissionFlag(b8 isMission) { NativeInvoke::Invoke<NATIVE_SET_MISSION_FLAG, ScriptVoid>(isMission); }
	static void SetFloatStat(eFloatStatistic stat, f32 value) { NativeInvoke::Invoke<NATIVE_SET_FLOAT_STAT, ScriptVoid>(stat, value); }
	static void SetRadarZoom(f32 zoom) { NativeInvoke::Invoke<NATIVE_SET_RADAR_ZOOM, ScriptVoid>(zoom); }
	static void SetTimerA(u32 value) { NativeInvoke::Invoke<NATIVE_SETTIMERA, ScriptVoid>(value); }
	static void SetTimerB(u32 value) { NativeInvoke::Invoke<NATIVE_SETTIMERB, ScriptVoid>(value); }
	static void SetWantedMultiplier(f32 multiplier) { NativeInvoke::Invoke<NATIVE_SET_WANTED_MULTIPLIER, ScriptVoid>(multiplier); }
	//static void TerminateThisScript() { NativeInvoke::Invoke<NATIVE_TERMINATE_THIS_SCRIPT, ScriptVoid>(); }   // Don't use this from a NativeThread (NativeFiberThread overrides this function)
	static u32 TimerA() { return NativeInvoke::Invoke<NATIVE_TIMERA, u32>(); }
	static u32 TimerB() { return NativeInvoke::Invoke<NATIVE_TIMERB, u32>(); }
	static u32 TimeStep() { return NativeInvoke::Invoke<NATIVE_TIMESTEP, u32>(); }
	static void UnlockLazlowStation() { NativeInvoke::Invoke<NATIVE_UNLOCK_LAZLOW_STATION, ScriptVoid>(); }
	static void UnregisterScriptWithAudio() { NativeInvoke::Invoke<NATIVE_UNREGISTER_SCRIPT_WITH_AUDIO, ScriptVoid>(); }
	static void UnPauseGame() { NativeInvoke::Invoke<NATIVE_UNPAUSE_GAME, ScriptVoid>(); }
	//static void Wait(u32 timeInMs) { NativeInvoke::Invoke<NATIVE_WAIT, ScriptVoid>(timeInMs); }   // Don't use this from a NativeThread (NativeFiberThread overrides this function)

	// General
	static f32 AbsF(f32 value) { return NativeInvoke::Invoke<NATIVE_ABSF, f32>(value); }        // Prefer using function from <math.h> instead for performance
	static f32 AbsI(f32 value) { return NativeInvoke::Invoke<NATIVE_ABSI, f32>(value); }        // Prefer using function from <math.h> instead for performance
	static f32 ASin(f32 value) { return NativeInvoke::Invoke<NATIVE_ASIN, f32>(value); }        // Prefer using function from <math.h> instead for performance
	static f32 ATan(f32 value) { return NativeInvoke::Invoke<NATIVE_ATAN, f32>(value); }        // Prefer using function from <math.h> instead for performance
	static i32 Ceil(f32 value) { return NativeInvoke::Invoke<NATIVE_CEIL, i32>(value); }        // Prefer using function from <math.h> instead for performance
	static f32 Cos(f32 value) { return NativeInvoke::Invoke<NATIVE_COS, f32>(value); }          // Prefer using function from <math.h> instead for performance
	static f32 ToFloat(u32 value) { return NativeInvoke::Invoke<NATIVE_TO_FLOAT, f32>(value); } // Prefer casting instead for performance
	static u32 Floor(f32 value) { return NativeInvoke::Invoke<NATIVE_FLOOR, u32>(value); }      // Prefer using function from <math.h> instead for performance
	static void GenerateRandomFloatInRange(f32 min, f32 max, f32 *pValue) { NativeInvoke::Invoke<NATIVE_GENERATE_RANDOM_FLOAT_IN_RANGE, ScriptVoid>(min, max, pValue); }
	static void GenerateRandomIntInRange(u32 min, u32 max, u32 *pValue) { NativeInvoke::Invoke<NATIVE_GENERATE_RANDOM_INT_IN_RANGE, ScriptVoid>(min, max, pValue); }
	static void GetAngleBetween2DVectors(f32 x1, f32 y1, f32 x2, f32 y2, f32 *pResult) { NativeInvoke::Invoke<NATIVE_GET_ANGLE_BETWEEN_2D_VECTORS, ScriptVoid>(x1, y1, x2, y2, pResult); }
	static void GetDistanceBetweenCoords2D(f32 x1, f32 y1, f32 x2, f32 y2, f32 *pDist) { NativeInvoke::Invoke<NATIVE_GET_DISTANCE_BETWEEN_COORDS_2D, ScriptVoid>(x1, y1, x2, y2, pDist); }
	static void GetDistanceBetweenCoords3D(f32 x1, f32 y1, f32 z1, f32 x2, f32 y2, f32 z2, f32 *pDist) { NativeInvoke::Invoke<NATIVE_GET_DISTANCE_BETWEEN_COORDS_3D, ScriptVoid>(x1, y1, z1, x2, y2, z2, pDist); }
	static const ch *GetFirstNCharactersOfLiteralString(const ch *literalString, u32 chars) { return NativeInvoke::Invoke<NATIVE_GET_FIRST_N_CHARACTERS_OF_LITERAL_STRING, const ch *>(literalString, chars); }
	static void GetHeadingFromVector2D(f32 x, f32 y, f32 *pHeading) { NativeInvoke::Invoke<NATIVE_GET_HEADING_FROM_VECTOR_2D, ScriptVoid>(x, y, pHeading); }
	static u32 GetLengthOfLiteralString(const ch *literalString) { return NativeInvoke::Invoke<NATIVE_GET_LENGTH_OF_LITERAL_STRING, u32>(literalString); }
	//static b8 IsStringNull(const ch *str) { return NativeInvoke::Invoke<NATIVE_IS_STRING_NULL, b8>(str); }
	static f32 Sin(f32 value) { return NativeInvoke::Invoke<NATIVE_SIN, f32>(value); }          // Prefer using function from <math.h> instead for performance
	static f32 Sqrt(f32 value) { return NativeInvoke::Invoke<NATIVE_SQRT, f32>(value); }        // Prefer using function from <math.h> instead for performance
	static f32 Tan(f32 value) { return NativeInvoke::Invoke<NATIVE_TAN, f32>(value); }          // Prefer using function from <math.h> instead for performance

}

#endif