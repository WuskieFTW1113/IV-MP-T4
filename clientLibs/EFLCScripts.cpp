#include "EFLCScripts.h"
#include "../ClientManager/ivScripting.h"
#include "easylogging++.h"

bool compatibility = false;
void execution::setCompatibility()
{
	compatibility = true;
}

void EFLCScripts::disableGTAStats()
{
/*	Scripting::SetIntStat(Scripting::STAT_ISLANDS_UNLOCKED, 100);
	Scripting::SetIntStat(Scripting::STAT_ACTIVITIES_WITH_BRUCIE, 0);
	Scripting::SetIntStat(Scripting::STAT_ACTIVITIES_WITH_ROMAN, 0);
	Scripting::SetIntStat(Scripting::STAT_MISSIONS_ATTEMPTED, 0);
	Scripting::SetIntStat(Scripting::STAT_MISSIONS_FAILED, 0);
	Scripting::SetIntStat(Scripting::STAT_MISSIONS_PASSED, 0);

	Scripting::SetIntStat(Scripting::STAT_STUNT_JUMPS_COMPLETED, 50);
	Scripting::SetIntStat(Scripting::STAT_ACTIVITIES_WITH_JACOB, 0);
	Scripting::SetIntStat(Scripting::STAT_ACTIVITIES_WITH_DWAYNE, 0);
	Scripting::SetIntStat(Scripting::STAT_ACTIVITIES_WITH_PACKIE, 0);
	Scripting::SetIntStat(Scripting::STAT_PIGEONS_EXTERMINATED, 200);
	Scripting::SetIntStat(Scripting::STAT_PROGRESS_WITH_DENISE, 0);
	Scripting::SetIntStat(Scripting::STAT_PROGRESS_WITH_MICHELLE, 0);
	Scripting::SetIntStat(Scripting::STAT_PROGRESS_WITH_HELENA, 0);
	Scripting::SetIntStat(Scripting::STAT_PROGRESS_WITH_BARBARA, 0);
	Scripting::SetIntStat(Scripting::STAT_PROGRESS_WITH_KATIE, 0);
	Scripting::SetIntStat(Scripting::STAT_PROGRESS_WITH_MILLIE, 0);
	
	Scripting::SetFloatStat(Scripting::STAT_TOTAL_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_ROMAN_LIKE, 100);
	Scripting::SetFloatStat(Scripting::STAT_ROMAN_RESPECT, 100);
	Scripting::SetFloatStat(Scripting::STAT_ROMAN_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_JACOB_LIKE, 100);
	Scripting::SetFloatStat(Scripting::STAT_JACOB_RESPECT, 100);
	Scripting::SetFloatStat(Scripting::STAT_JACOB_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_FAUSTIN_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_MANNY_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_ELIZABETA_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_DWAYNE_LIKE, 100);
	Scripting::SetFloatStat(Scripting::STAT_DWAYNE_RESPECT, 100);
	Scripting::SetFloatStat(Scripting::STAT_DWAYNE_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_BRUCIE_LIKE, 100);
	Scripting::SetFloatStat(Scripting::STAT_BRUCIE_RESPECT, 100);
	Scripting::SetFloatStat(Scripting::STAT_BRUCIE_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_PLAYBOY_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_ULPC_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_PACKIE_LIKE, 100);
	Scripting::SetFloatStat(Scripting::STAT_PACKIE_RESPECT, 100);
	Scripting::SetFloatStat(Scripting::STAT_PACKIE_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_RAY_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_GERRY_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_BERNIE_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_BELL_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_GAMBETTI_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_JIMMY_MISSION_PROGRESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_CARMEN_ORTIZ_FONDNESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_CARMEN_TRUST, 100);
	Scripting::SetFloatStat(Scripting::STAT_ALEX_CHILTON_FONDNESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_ALEX_TRUST, 100);
	Scripting::SetFloatStat(Scripting::STAT_KIKI_JENKINS_FONDNESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_KIKI_TRUST, 100);
	Scripting::SetFloatStat(Scripting::STAT_MICHELLE_FONDNESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_MICHELLE_TRUST, 100);
	Scripting::SetFloatStat(Scripting::STAT_KATE_FONDNESS, 100);
	Scripting::SetFloatStat(Scripting::STAT_KATE_TRUST, 100);
	Scripting::SetFloatStat(Scripting::STAT_GAME_PROGRESS, 100);*/
}

int sub_4950(int uParam0)
{
   /* int Result;

	NativeInvoke::Invoke<u32>("GET_WEAPONTYPE_MODEL", uParam0, &Result);
    return Result;*/
}

void EFLCScripts::turnOffGTAScripts()
{
	if(execution::getPatch() > 0x1080 && compatibility)
	{
		Scripting::TerminateAllScriptsWithThisName("parachutelauncher");
		Scripting::TerminateAllScriptsWithThisName("parachute_ai");
		Scripting::TerminateAllScriptsWithThisName("parachute_player");
	}
	
	//Scripting::TerminateAllScriptsWithThisName("ability_backup");
	//Scripting::TerminateAllScriptsWithThisName("ability_bomb");
	//Scripting::TerminateAllScriptsWithThisName("ability_gun_car");
	//Scripting::TerminateAllScriptsWithThisName("ability_vehicle");
	//Scripting::TerminateAllScriptsWithThisName("advanced_car_actions");
	Scripting::TerminateAllScriptsWithThisName("air_hockey");
	Scripting::TerminateAllScriptsWithThisName("air_hockey_trigger");
	Scripting::TerminateAllScriptsWithThisName("ambairpotarea");
	Scripting::TerminateAllScriptsWithThisName("ambatmq");
	Scripting::TerminateAllScriptsWithThisName("ambbar");
	Scripting::TerminateAllScriptsWithThisName("ambbarrier");
	Scripting::TerminateAllScriptsWithThisName("ambbeggar");
	Scripting::TerminateAllScriptsWithThisName("ambblkhawk");
	Scripting::TerminateAllScriptsWithThisName("ambbouncer");
	Scripting::TerminateAllScriptsWithThisName("ambbridgepatrol");
	Scripting::TerminateAllScriptsWithThisName("ambbusker");
	Scripting::TerminateAllScriptsWithThisName("ambcabaret");
	Scripting::TerminateAllScriptsWithThisName("ambcargoholddoors");
	//Scripting::TerminateAllScriptsWithThisName("ambchurchdoors");
	Scripting::TerminateAllScriptsWithThisName("ambclublights");
	Scripting::TerminateAllScriptsWithThisName("ambcomedyclub");
	Scripting::TerminateAllScriptsWithThisName("ambcontrolloader");
	Scripting::TerminateAllScriptsWithThisName("ambcontrolmain");
	Scripting::TerminateAllScriptsWithThisName("ambdealer");
	Scripting::TerminateAllScriptsWithThisName("ambdebug");
	Scripting::TerminateAllScriptsWithThisName("ambdrinking");
	Scripting::TerminateAllScriptsWithThisName("ambfloater");
	Scripting::TerminateAllScriptsWithThisName("ambfoodeating");
	Scripting::TerminateAllScriptsWithThisName("ambfunfair");
	Scripting::TerminateAllScriptsWithThisName("ambgerry3doorlock");
	Scripting::TerminateAllScriptsWithThisName("ambhelicopter");
	Scripting::TerminateAllScriptsWithThisName("ambhomelandcordon");
	Scripting::TerminateAllScriptsWithThisName("ambhomelandfed");
	Scripting::TerminateAllScriptsWithThisName("ambhomelandsirens");
	Scripting::TerminateAllScriptsWithThisName("ambhotel");
	Scripting::TerminateAllScriptsWithThisName("ambinternetcafe");
	Scripting::TerminateAllScriptsWithThisName("ambjerseydocksgates");
	Scripting::TerminateAllScriptsWithThisName("ambjimslocks");
	Scripting::TerminateAllScriptsWithThisName("ambliftdoors");
	Scripting::TerminateAllScriptsWithThisName("ambnightclubbm");
	Scripting::TerminateAllScriptsWithThisName("ambnightclubbm2");
	Scripting::TerminateAllScriptsWithThisName("ambnightclubbm_min");
	Scripting::TerminateAllScriptsWithThisName("ambnightclubext");
	Scripting::TerminateAllScriptsWithThisName("ambnightclubhc");
	Scripting::TerminateAllScriptsWithThisName("ambnightclubhc2");
	Scripting::TerminateAllScriptsWithThisName("ambnightclubm9");
	Scripting::TerminateAllScriptsWithThisName("ambnightclubm92");
	Scripting::TerminateAllScriptsWithThisName("ambpimpnpros");
	Scripting::TerminateAllScriptsWithThisName("ambpoledancer");
	Scripting::TerminateAllScriptsWithThisName("ambpolrdblk");
	Scripting::TerminateAllScriptsWithThisName("ambpreacher");
	Scripting::TerminateAllScriptsWithThisName("ambprisonwanted");
	Scripting::TerminateAllScriptsWithThisName("ambsavebed");
	Scripting::TerminateAllScriptsWithThisName("ambshowroom");
	Scripting::TerminateAllScriptsWithThisName("ambstripclub");
	Scripting::TerminateAllScriptsWithThisName("ambtaxdpt");
	Scripting::TerminateAllScriptsWithThisName("ambtaxihail");
	Scripting::TerminateAllScriptsWithThisName("ambtoiletdoors");
	Scripting::TerminateAllScriptsWithThisName("ambtunnelcops");
	Scripting::TerminateAllScriptsWithThisName("ambtv");
	Scripting::TerminateAllScriptsWithThisName("ambunarea");
	Scripting::TerminateAllScriptsWithThisName("ambwardrobe");
	Scripting::TerminateAllScriptsWithThisName("ambwindowlift");
	Scripting::TerminateAllScriptsWithThisName("arnaud1");
	Scripting::TerminateAllScriptsWithThisName("arnaud2");
	Scripting::TerminateAllScriptsWithThisName("arrestescapehelp");
	Scripting::TerminateAllScriptsWithThisName("atmobj");
	Scripting::TerminateAllScriptsWithThisName("badman_1");
	Scripting::TerminateAllScriptsWithThisName("basejumping");
	Scripting::TerminateAllScriptsWithThisName("basejumplauncher");
	Scripting::TerminateAllScriptsWithThisName("basejumpscreen");
	Scripting::TerminateAllScriptsWithThisName("bell2");
	Scripting::TerminateAllScriptsWithThisName("bell3m");
	Scripting::TerminateAllScriptsWithThisName("bell3p");
	Scripting::TerminateAllScriptsWithThisName("bell5");
	Scripting::TerminateAllScriptsWithThisName("bell6");
	Scripting::TerminateAllScriptsWithThisName("bell7");
	Scripting::TerminateAllScriptsWithThisName("bernie1");
	Scripting::TerminateAllScriptsWithThisName("bernie2");
	Scripting::TerminateAllScriptsWithThisName("bernie3");
	Scripting::TerminateAllScriptsWithThisName("berniecar");
	Scripting::TerminateAllScriptsWithThisName("binco_brook_s");
	Scripting::TerminateAllScriptsWithThisName("boating");
	Scripting::TerminateAllScriptsWithThisName("bootycall");
	Scripting::TerminateAllScriptsWithThisName("bootycallcontroll");
	Scripting::TerminateAllScriptsWithThisName("bowling_lane");
	Scripting::TerminateAllScriptsWithThisName("bowl_trigger");
	Scripting::TerminateAllScriptsWithThisName("brian_1");
	Scripting::TerminateAllScriptsWithThisName("brian_2");
	Scripting::TerminateAllScriptsWithThisName("brian_3");
	Scripting::TerminateAllScriptsWithThisName("brother1");
	Scripting::TerminateAllScriptsWithThisName("brother2");
	Scripting::TerminateAllScriptsWithThisName("brother3");
	Scripting::TerminateAllScriptsWithThisName("brucie1");
	Scripting::TerminateAllScriptsWithThisName("brucie2");
	Scripting::TerminateAllScriptsWithThisName("brucie3a");
	Scripting::TerminateAllScriptsWithThisName("brucie3b");
	Scripting::TerminateAllScriptsWithThisName("brucie3_meetguy");
	Scripting::TerminateAllScriptsWithThisName("brucie4");
	Scripting::TerminateAllScriptsWithThisName("brucie5m");
	Scripting::TerminateAllScriptsWithThisName("brucie5p");
	Scripting::TerminateAllScriptsWithThisName("brucieboat");
	Scripting::TerminateAllScriptsWithThisName("bruciecopter");
	Scripting::TerminateAllScriptsWithThisName("bruciedrink");
	Scripting::TerminateAllScriptsWithThisName("brucieeat");
	Scripting::TerminateAllScriptsWithThisName("brucielift");
	Scripting::TerminateAllScriptsWithThisName("brucieshow");
	Scripting::TerminateAllScriptsWithThisName("bruciestrip");
	Scripting::TerminateAllScriptsWithThisName("brucietenpin");
	Scripting::TerminateAllScriptsWithThisName("brucie_heli");
	Scripting::TerminateAllScriptsWithThisName("bulgarin1");
	Scripting::TerminateAllScriptsWithThisName("bulgarin2");
	Scripting::TerminateAllScriptsWithThisName("bulgarin3");
	Scripting::TerminateAllScriptsWithThisName("burgervendor");
	Scripting::TerminateAllScriptsWithThisName("busstop");
	Scripting::TerminateAllScriptsWithThisName("cablecars");
	Scripting::TerminateAllScriptsWithThisName("cardivehelp");
	Scripting::TerminateAllScriptsWithThisName("carfoodrunjack");
	Scripting::TerminateAllScriptsWithThisName("carwash");
	Scripting::TerminateAllScriptsWithThisName("car_theft_texts");
	Scripting::TerminateAllScriptsWithThisName("cherise");
	Scripting::TerminateAllScriptsWithThisName("cia1");
	Scripting::TerminateAllScriptsWithThisName("cia2");
	Scripting::TerminateAllScriptsWithThisName("cia3");
	Scripting::TerminateAllScriptsWithThisName("cia4");
	Scripting::TerminateAllScriptsWithThisName("ciawanted");
	Scripting::TerminateAllScriptsWithThisName("clarence");
	Scripting::TerminateAllScriptsWithThisName("cleanupray5trains");
	Scripting::TerminateAllScriptsWithThisName("clubmanblips");
	Scripting::TerminateAllScriptsWithThisName("clubmanphonecalls");
	Scripting::TerminateAllScriptsWithThisName("clubsecurity");
	Scripting::TerminateAllScriptsWithThisName("computermain");
	Scripting::TerminateAllScriptsWithThisName("computernews");
	Scripting::TerminateAllScriptsWithThisName("computerstreamed");
	Scripting::TerminateAllScriptsWithThisName("copbootsearch");
	Scripting::TerminateAllScriptsWithThisName("daisy1");
	Scripting::TerminateAllScriptsWithThisName("dancing");
	Scripting::TerminateAllScriptsWithThisName("darts");
	Scripting::TerminateAllScriptsWithThisName("darts_launcher");
	Scripting::TerminateAllScriptsWithThisName("darts_trigger");
	Scripting::TerminateAllScriptsWithThisName("date_activity");
	Scripting::TerminateAllScriptsWithThisName("date_food");
	Scripting::TerminateAllScriptsWithThisName("dating_alex");
	Scripting::TerminateAllScriptsWithThisName("dating_carmen");
	Scripting::TerminateAllScriptsWithThisName("dating_kate");
	Scripting::TerminateAllScriptsWithThisName("dating_kiki");
	Scripting::TerminateAllScriptsWithThisName("dating_michelle");
	Scripting::TerminateAllScriptsWithThisName("deatharresthelp");
	Scripting::TerminateAllScriptsWithThisName("derrick1");
	Scripting::TerminateAllScriptsWithThisName("derrick2");
	Scripting::TerminateAllScriptsWithThisName("derrick3");
	Scripting::TerminateAllScriptsWithThisName("dominicanfriend");
	Scripting::TerminateAllScriptsWithThisName("drinking");
	Scripting::TerminateAllScriptsWithThisName("drinkingrect");
	Scripting::TerminateAllScriptsWithThisName("drugwarscontrol");
	Scripting::TerminateAllScriptsWithThisName("drugwarsconvoy");
	Scripting::TerminateAllScriptsWithThisName("drugwarscreen");
	Scripting::TerminateAllScriptsWithThisName("drugwarsdeal");
	Scripting::TerminateAllScriptsWithThisName("drugwarspickup");
	Scripting::TerminateAllScriptsWithThisName("drugwarsshipment");
	Scripting::TerminateAllScriptsWithThisName("drunk");
	Scripting::TerminateAllScriptsWithThisName("drunkcontroller");
	Scripting::TerminateAllScriptsWithThisName("dwayne1");
	Scripting::TerminateAllScriptsWithThisName("dwayne3");
	Scripting::TerminateAllScriptsWithThisName("dwaynecherise");
	Scripting::TerminateAllScriptsWithThisName("dwaynedrink");
	Scripting::TerminateAllScriptsWithThisName("dwayneeat");
	Scripting::TerminateAllScriptsWithThisName("dwayneemail");
	Scripting::TerminateAllScriptsWithThisName("dwayneshow");
	Scripting::TerminateAllScriptsWithThisName("dwaynestrip");
	Scripting::TerminateAllScriptsWithThisName("dwaynetenpin");
	Scripting::TerminateAllScriptsWithThisName("dwayne_backup");
	Scripting::TerminateAllScriptsWithThisName("e2endcredits");
	Scripting::TerminateAllScriptsWithThisName("e2rewards");
	Scripting::TerminateAllScriptsWithThisName("e2stattracker");
	Scripting::TerminateAllScriptsWithThisName("eddie1");
	Scripting::TerminateAllScriptsWithThisName("eddie2");
	Scripting::TerminateAllScriptsWithThisName("elizabeta1");
	Scripting::TerminateAllScriptsWithThisName("elizabeta2");
	Scripting::TerminateAllScriptsWithThisName("elizabeta3");
	Scripting::TerminateAllScriptsWithThisName("elizabeta3vehicle");
	Scripting::TerminateAllScriptsWithThisName("elizabeta4");
	Scripting::TerminateAllScriptsWithThisName("emergencycall");
	Scripting::TerminateAllScriptsWithThisName("empiredown");
	Scripting::TerminateAllScriptsWithThisName("empiretelescope");
	Scripting::TerminateAllScriptsWithThisName("endcreditroll");
	Scripting::TerminateAllScriptsWithThisName("endgamenews");
	Scripting::TerminateAllScriptsWithThisName("faustin1");
	Scripting::TerminateAllScriptsWithThisName("faustin2");
	Scripting::TerminateAllScriptsWithThisName("faustin3");
	Scripting::TerminateAllScriptsWithThisName("faustin4");
	Scripting::TerminateAllScriptsWithThisName("faustin5");
	Scripting::TerminateAllScriptsWithThisName("faustin5_fire");
	Scripting::TerminateAllScriptsWithThisName("faustin6");
	Scripting::TerminateAllScriptsWithThisName("faustin7");
	Scripting::TerminateAllScriptsWithThisName("fightclublauncher");
	Scripting::TerminateAllScriptsWithThisName("fightclubring");
	Scripting::TerminateAllScriptsWithThisName("fightclubscreen");
	Scripting::TerminateAllScriptsWithThisName("finale1a");
	Scripting::TerminateAllScriptsWithThisName("finale1b");
	Scripting::TerminateAllScriptsWithThisName("finale1c");
	Scripting::TerminateAllScriptsWithThisName("finale1d");
	Scripting::TerminateAllScriptsWithThisName("finale2");
	Scripting::TerminateAllScriptsWithThisName("finaleaorb");
	Scripting::TerminateAllScriptsWithThisName("finaleintrocut");
	Scripting::TerminateAllScriptsWithThisName("finalewedtxts");
	Scripting::TerminateAllScriptsWithThisName("foodserver");
	Scripting::TerminateAllScriptsWithThisName("francis1");
	Scripting::TerminateAllScriptsWithThisName("francis2a");
	Scripting::TerminateAllScriptsWithThisName("francis2b");
	Scripting::TerminateAllScriptsWithThisName("francis2_appointment");
	Scripting::TerminateAllScriptsWithThisName("francis3");
	Scripting::TerminateAllScriptsWithThisName("francis4");
	Scripting::TerminateAllScriptsWithThisName("francis5");
	Scripting::TerminateAllScriptsWithThisName("francis6");
	Scripting::TerminateAllScriptsWithThisName("friends1");
	Scripting::TerminateAllScriptsWithThisName("friends2");
	Scripting::TerminateAllScriptsWithThisName("gambetti1");
	Scripting::TerminateAllScriptsWithThisName("gambetti2");
	Scripting::TerminateAllScriptsWithThisName("gambetti3");
	Scripting::TerminateAllScriptsWithThisName("garbage_trucks");
	Scripting::TerminateAllScriptsWithThisName("gerry1");
	Scripting::TerminateAllScriptsWithThisName("gerry1_base");
	Scripting::TerminateAllScriptsWithThisName("gerry2");
	Scripting::TerminateAllScriptsWithThisName("gerry3a");
	Scripting::TerminateAllScriptsWithThisName("gerry3b");
	Scripting::TerminateAllScriptsWithThisName("gerry3c");
	Scripting::TerminateAllScriptsWithThisName("gerry3_phonegirl");
	Scripting::TerminateAllScriptsWithThisName("gerry4");
	Scripting::TerminateAllScriptsWithThisName("gerry5");
	Scripting::TerminateAllScriptsWithThisName("golf");
	Scripting::TerminateAllScriptsWithThisName("golf_launcher");
	Scripting::TerminateAllScriptsWithThisName("go_on_date");
	Scripting::TerminateAllScriptsWithThisName("gracie");
	Scripting::TerminateAllScriptsWithThisName("gunlockup");
	Scripting::TerminateAllScriptsWithThisName("gunlockupct");
	Scripting::TerminateAllScriptsWithThisName("handbrakehelp");
	Scripting::TerminateAllScriptsWithThisName("happytshirt");
	Scripting::TerminateAllScriptsWithThisName("helicopter");
	Scripting::TerminateAllScriptsWithThisName("hospitalpickup");
	Scripting::TerminateAllScriptsWithThisName("hossan_1");
	Scripting::TerminateAllScriptsWithThisName("hotwirehelp");
	Scripting::TerminateAllScriptsWithThisName("ilyena");
	Scripting::TerminateAllScriptsWithThisName("ilyena_calls");
	//Scripting::TerminateAllScriptsWithThisName("independencefm");
	Scripting::TerminateAllScriptsWithThisName("initial");
	Scripting::TerminateAllScriptsWithThisName("internet_dating");
	Scripting::TerminateAllScriptsWithThisName("ivan_1");
	Scripting::TerminateAllScriptsWithThisName("jacob1");
	Scripting::TerminateAllScriptsWithThisName("jacob2");
	Scripting::TerminateAllScriptsWithThisName("jacob3m");
	Scripting::TerminateAllScriptsWithThisName("jacob3p");
	Scripting::TerminateAllScriptsWithThisName("jacobdarts");
	Scripting::TerminateAllScriptsWithThisName("jacobdrink");
	Scripting::TerminateAllScriptsWithThisName("jacobeat");
	Scripting::TerminateAllScriptsWithThisName("jacoblift");
	Scripting::TerminateAllScriptsWithThisName("jacobpool");
	Scripting::TerminateAllScriptsWithThisName("jacobshow");
	Scripting::TerminateAllScriptsWithThisName("jacobstrip");
	Scripting::TerminateAllScriptsWithThisName("jacob_gun_car");
	Scripting::TerminateAllScriptsWithThisName("jeff_1");
	Scripting::TerminateAllScriptsWithThisName("jeff_1_mission");
	Scripting::TerminateAllScriptsWithThisName("jeff_2");
	Scripting::TerminateAllScriptsWithThisName("jeff_3");
	Scripting::TerminateAllScriptsWithThisName("jimmy1");
	Scripting::TerminateAllScriptsWithThisName("jimmy2");
	Scripting::TerminateAllScriptsWithThisName("jimmy3");
	Scripting::TerminateAllScriptsWithThisName("jimmy4");
	Scripting::TerminateAllScriptsWithThisName("jimmy4a");
	Scripting::TerminateAllScriptsWithThisName("jimmysuit");
	Scripting::TerminateAllScriptsWithThisName("laterweapons");
	Scripting::TerminateAllScriptsWithThisName("magvendor");
	//Scripting::TerminateAllScriptsWithThisName("main");
	Scripting::TerminateAllScriptsWithThisName("manhat_heli_tours");
	Scripting::TerminateAllScriptsWithThisName("manny1");
	Scripting::TerminateAllScriptsWithThisName("manny2");
	Scripting::TerminateAllScriptsWithThisName("manny3");
	Scripting::TerminateAllScriptsWithThisName("margot1");
	Scripting::TerminateAllScriptsWithThisName("margot2");
	Scripting::TerminateAllScriptsWithThisName("marnie1");
	Scripting::TerminateAllScriptsWithThisName("marnie2");
	Scripting::TerminateAllScriptsWithThisName("mel");
	Scripting::TerminateAllScriptsWithThisName("michelle1");
	Scripting::TerminateAllScriptsWithThisName("michelle1_queue");
	Scripting::TerminateAllScriptsWithThisName("michelle_introcalls");
	Scripting::TerminateAllScriptsWithThisName("missionresulthelp");
	Scripting::TerminateAllScriptsWithThisName("missionresultstimer");
	Scripting::TerminateAllScriptsWithThisName("missionstattracker");
	Scripting::TerminateAllScriptsWithThisName("modo_manhat_5");
	Scripting::TerminateAllScriptsWithThisName("mostwanted");
	Scripting::TerminateAllScriptsWithThisName("mptutoriallauncher");
	Scripting::TerminateAllScriptsWithThisName("multiplayertutorial");
	Scripting::TerminateAllScriptsWithThisName("multitutorial");
	Scripting::TerminateAllScriptsWithThisName("mum1");
	Scripting::TerminateAllScriptsWithThisName("network_session");
	Scripting::TerminateAllScriptsWithThisName("nutvendor");
	Scripting::TerminateAllScriptsWithThisName("opening_credits");
	Scripting::TerminateAllScriptsWithThisName("organiserhelp");
	Scripting::TerminateAllScriptsWithThisName("packie1");
	Scripting::TerminateAllScriptsWithThisName("packie2");
	Scripting::TerminateAllScriptsWithThisName("packie3");
	Scripting::TerminateAllScriptsWithThisName("packiedarts");
	Scripting::TerminateAllScriptsWithThisName("packiedrink");
	Scripting::TerminateAllScriptsWithThisName("packiepool");
	Scripting::TerminateAllScriptsWithThisName("packieshow");
	Scripting::TerminateAllScriptsWithThisName("packiestrip");
	Scripting::TerminateAllScriptsWithThisName("packiesuit");
	Scripting::TerminateAllScriptsWithThisName("packietenpin");
	Scripting::TerminateAllScriptsWithThisName("packie_bomb");

	Scripting::TerminateAllScriptsWithThisName("pathos1");
	Scripting::TerminateAllScriptsWithThisName("pathos2");
	Scripting::TerminateAllScriptsWithThisName("pdbhangoutmgr");
	Scripting::TerminateAllScriptsWithThisName("pdbhomemgr");
	Scripting::TerminateAllScriptsWithThisName("pdbmostwanted");
	Scripting::TerminateAllScriptsWithThisName("perseus_manhat_8");
	Scripting::TerminateAllScriptsWithThisName("phonepadhelp");
	Scripting::TerminateAllScriptsWithThisName("phoneprofilehelp");
	Scripting::TerminateAllScriptsWithThisName("physicstestbed");
	Scripting::TerminateAllScriptsWithThisName("placeholder");
	Scripting::TerminateAllScriptsWithThisName("playboy2");
	Scripting::TerminateAllScriptsWithThisName("playboy3");
	Scripting::TerminateAllScriptsWithThisName("playboy4");
	Scripting::TerminateAllScriptsWithThisName("policebackup");
	Scripting::TerminateAllScriptsWithThisName("policetest");
	Scripting::TerminateAllScriptsWithThisName("pool_game");
	Scripting::TerminateAllScriptsWithThisName("pool_table");
	Scripting::TerminateAllScriptsWithThisName("puzzle");
	Scripting::TerminateAllScriptsWithThisName("puzzle_launcher");
	Scripting::TerminateAllScriptsWithThisName("pxdfcut");
	Scripting::TerminateAllScriptsWithThisName("racesp");
	Scripting::TerminateAllScriptsWithThisName("racesplauncher");
	Scripting::TerminateAllScriptsWithThisName("racesscreen");
	Scripting::TerminateAllScriptsWithThisName("ray1");
	Scripting::TerminateAllScriptsWithThisName("ray2");
	Scripting::TerminateAllScriptsWithThisName("ray3");
	Scripting::TerminateAllScriptsWithThisName("ray4");
	Scripting::TerminateAllScriptsWithThisName("ray5");
	Scripting::TerminateAllScriptsWithThisName("ray6");
	Scripting::TerminateAllScriptsWithThisName("repeat");
	Scripting::TerminateAllScriptsWithThisName("replayhelp");
	Scripting::TerminateAllScriptsWithThisName("replaytutorial");
	Scripting::TerminateAllScriptsWithThisName("rocco1");
	Scripting::TerminateAllScriptsWithThisName("roman1");
	Scripting::TerminateAllScriptsWithThisName("roman10m");
	Scripting::TerminateAllScriptsWithThisName("roman10p");
	Scripting::TerminateAllScriptsWithThisName("roman11");
	Scripting::TerminateAllScriptsWithThisName("roman12");
	Scripting::TerminateAllScriptsWithThisName("roman13");
	Scripting::TerminateAllScriptsWithThisName("roman14");
	Scripting::TerminateAllScriptsWithThisName("roman2");
	Scripting::TerminateAllScriptsWithThisName("roman3");
	Scripting::TerminateAllScriptsWithThisName("roman3txtmsgrepeat");
	Scripting::TerminateAllScriptsWithThisName("roman4");
	Scripting::TerminateAllScriptsWithThisName("roman5");
	Scripting::TerminateAllScriptsWithThisName("roman6");
	Scripting::TerminateAllScriptsWithThisName("roman7");
	Scripting::TerminateAllScriptsWithThisName("roman8m");
	Scripting::TerminateAllScriptsWithThisName("roman8p");
	Scripting::TerminateAllScriptsWithThisName("roman9");
	Scripting::TerminateAllScriptsWithThisName("romanblips");
	Scripting::TerminateAllScriptsWithThisName("romandarts");
	Scripting::TerminateAllScriptsWithThisName("romandrink");
	Scripting::TerminateAllScriptsWithThisName("romaneat");
	Scripting::TerminateAllScriptsWithThisName("romanmarker");
	Scripting::TerminateAllScriptsWithThisName("romanpool");
	Scripting::TerminateAllScriptsWithThisName("romanshow");
	Scripting::TerminateAllScriptsWithThisName("romanstrip");
	Scripting::TerminateAllScriptsWithThisName("romantenpin");
	Scripting::TerminateAllScriptsWithThisName("roman_bleedoutintro");
	Scripting::TerminateAllScriptsWithThisName("roman_fakeblip");
	Scripting::TerminateAllScriptsWithThisName("roman_taxi");
	Scripting::TerminateAllScriptsWithThisName("sara2");
	Scripting::TerminateAllScriptsWithThisName("sara_1");
	Scripting::TerminateAllScriptsWithThisName("scratchpad");
	Scripting::TerminateAllScriptsWithThisName("sixaxistutlauncher");
	Scripting::TerminateAllScriptsWithThisName("sixaxistutorial");
	Scripting::TerminateAllScriptsWithThisName("spcellphone");
	Scripting::TerminateAllScriptsWithThisName("spcellphonecalling");
	Scripting::TerminateAllScriptsWithThisName("spcellphonedebug");
	Scripting::TerminateAllScriptsWithThisName("spcellphoneendcall");
	Scripting::TerminateAllScriptsWithThisName("spcellphonemain");
	Scripting::TerminateAllScriptsWithThisName("spcellphonenetwork");
	Scripting::TerminateAllScriptsWithThisName("spcellphonetutorial");
	Scripting::TerminateAllScriptsWithThisName("speechcontrol");
	Scripting::TerminateAllScriptsWithThisName("sprunk");
	//Scripting::TerminateAllScriptsWithThisName("startup");
	Scripting::TerminateAllScriptsWithThisName("stattracker");
	Scripting::TerminateAllScriptsWithThisName("stat_bridge");
	Scripting::TerminateAllScriptsWithThisName("storytimer");
	Scripting::TerminateAllScriptsWithThisName("stunt");
	Scripting::TerminateAllScriptsWithThisName("taxi");
	Scripting::TerminateAllScriptsWithThisName("taxi_trigger");
	Scripting::TerminateAllScriptsWithThisName("telescope");
	Scripting::TerminateAllScriptsWithThisName("tenpinbowl");
	Scripting::TerminateAllScriptsWithThisName("text_link_mission");
	Scripting::TerminateAllScriptsWithThisName("till");
	Scripting::TerminateAllScriptsWithThisName("toilet_activity");
	Scripting::TerminateAllScriptsWithThisName("tollbooth");
	Scripting::TerminateAllScriptsWithThisName("tony1");
	Scripting::TerminateAllScriptsWithThisName("tony10");
	Scripting::TerminateAllScriptsWithThisName("tony11");
	Scripting::TerminateAllScriptsWithThisName("tony2");
	Scripting::TerminateAllScriptsWithThisName("tony3");
	Scripting::TerminateAllScriptsWithThisName("tony4a");
	Scripting::TerminateAllScriptsWithThisName("tony4b");
	Scripting::TerminateAllScriptsWithThisName("tony5");
	Scripting::TerminateAllScriptsWithThisName("tony6");
	Scripting::TerminateAllScriptsWithThisName("tony7");
	Scripting::TerminateAllScriptsWithThisName("tony8");
	Scripting::TerminateAllScriptsWithThisName("tony9");
	Scripting::TerminateAllScriptsWithThisName("tonylauncher");
	Scripting::TerminateAllScriptsWithThisName("tonym1");
	Scripting::TerminateAllScriptsWithThisName("tonym2");
	Scripting::TerminateAllScriptsWithThisName("tonym3");
	Scripting::TerminateAllScriptsWithThisName("tonym4");
	Scripting::TerminateAllScriptsWithThisName("tonym5");
	Scripting::TerminateAllScriptsWithThisName("tonym6");
	Scripting::TerminateAllScriptsWithThisName("tonym7");
	Scripting::TerminateAllScriptsWithThisName("tonym8");
	Scripting::TerminateAllScriptsWithThisName("tonypreblogb");
	Scripting::TerminateAllScriptsWithThisName("trainhelper");
	Scripting::TerminateAllScriptsWithThisName("txtmsghelp");
	Scripting::TerminateAllScriptsWithThisName("vendor");
	Scripting::TerminateAllScriptsWithThisName("vigilante");
	Scripting::TerminateAllScriptsWithThisName("vlad1");
	Scripting::TerminateAllScriptsWithThisName("vlad2");
	Scripting::TerminateAllScriptsWithThisName("vlad3");
	Scripting::TerminateAllScriptsWithThisName("vlad4");
	Scripting::TerminateAllScriptsWithThisName("wantedhelp");
	Scripting::TerminateAllScriptsWithThisName("waypointhelp");
	Scripting::TerminateAllScriptsWithThisName("webalexblog");
	Scripting::TerminateAllScriptsWithThisName("webautoeroticar");
	Scripting::TerminateAllScriptsWithThisName("webcelebcomment");
	Scripting::TerminateAllScriptsWithThisName("webcelebinatorblog");
	Scripting::TerminateAllScriptsWithThisName("webcraplist");
	Scripting::TerminateAllScriptsWithThisName("webdating");
	Scripting::TerminateAllScriptsWithThisName("webdatingboys");
	Scripting::TerminateAllScriptsWithThisName("webdatingfullprofile");
	Scripting::TerminateAllScriptsWithThisName("webdatinggirls");
	Scripting::TerminateAllScriptsWithThisName("webeddieblog");
	Scripting::TerminateAllScriptsWithThisName("webemail");
	Scripting::TerminateAllScriptsWithThisName("weberror");
	Scripting::TerminateAllScriptsWithThisName("webeyefind");
	Scripting::TerminateAllScriptsWithThisName("webeyefindsearch");
	Scripting::TerminateAllScriptsWithThisName("weblawyerscareer");
	Scripting::TerminateAllScriptsWithThisName("weblawyerscv");
	Scripting::TerminateAllScriptsWithThisName("weblcpd");
	Scripting::TerminateAllScriptsWithThisName("weblcpdprofile");
	Scripting::TerminateAllScriptsWithThisName("weblibertytree");
	Scripting::TerminateAllScriptsWithThisName("weblibertytreenews");
	Scripting::TerminateAllScriptsWithThisName("webourownreality");
	Scripting::TerminateAllScriptsWithThisName("webpublicliberty");
	Scripting::TerminateAllScriptsWithThisName("webrealestate");
	Scripting::TerminateAllScriptsWithThisName("webringtones");
	Scripting::TerminateAllScriptsWithThisName("webringtonesmono");
	Scripting::TerminateAllScriptsWithThisName("webringtonespoly");
	Scripting::TerminateAllScriptsWithThisName("webringtonespurchase");
	Scripting::TerminateAllScriptsWithThisName("webringtonestheme");
	Scripting::TerminateAllScriptsWithThisName("webweazel");
	Scripting::TerminateAllScriptsWithThisName("webweazelnews");
	Scripting::TerminateAllScriptsWithThisName("webwhatthey");
	Scripting::TerminateAllScriptsWithThisName("window_lift_launcher");
	Scripting::TerminateAllScriptsWithThisName("yusuf1");
	Scripting::TerminateAllScriptsWithThisName("yusuf1cleanup");
	Scripting::TerminateAllScriptsWithThisName("yusuf2");
	Scripting::TerminateAllScriptsWithThisName("yusuf3");
	Scripting::TerminateAllScriptsWithThisName("yusuf4");

	Scripting::SetPedNonCreationArea(-3000, -3000, -3000, 3000, 3000, 3000);
	Scripting::SetPedDensityMultiplier(0);
	Scripting::SetCarDensityMultiplier(0);
	Scripting::DisableCarGenerators(1);
	Scripting::SetNoResprays(1);
	Scripting::SwitchAmbientPlanes(0);
	Scripting::SwitchArrowAboveBlippedPickups(0);
	Scripting::SwitchRandomBoats(0);
	Scripting::SwitchRandomTrains(0);
	Scripting::SwitchGarbageTrucks(0);

	Scripting::SetScenarioPedDensityMultiplier(0, 0);
	Scripting::SetPedDensityMultiplier(0);
	Scripting::SetParkedCarDensityMultiplier(0);
	Scripting::SetRandomCarDensityMultiplier(0);
	Scripting::SetMaxWantedLevel(0);

	Scripting::ClearAreaOfChars(0.0f, 0.0f, 0.0f, 10000.0f);
	Scripting::ClearAreaOfCars(0.0f, 0.0f, 0.0f, 1000.0f);

	Scripting::AllowStuntJumpsToTrigger(false);

	int pTypes[4] = {2, 22, 15, 3};

	for(size_t i = 0; i < 4; ++i)
	{
		//NativeInvoke::Invoke<u32>("REMOVE_ALL_PICKUPS_OF_TYPE", pTypes[i]);
		Scripting::RemoveAllPickupsOfType(pTypes[i]);
	}

	//NativeInvoke::Invoke<u32>("CLEAR_NEWS_SCROLLBAR");

	/*NativeInvoke::Invoke<u32>("ADD_STRING_TO_NEWS_SCROLLBAR", "Many men, wish death upon me"
		" Blood in my eye dog and I cant see"
		" Im trying to be what I'm destined to be"
		" And niggas trying to take my life away");*/

	LINFO << "All set";
}

void EFLCScripts::pulseGTAScripts()
{
	Scripting::SetPedNonCreationArea(-3000, -3000, -3000, 3000, 3000, 3000);
	Scripting::SetPedDensityMultiplier(0);
	Scripting::SetCarDensityMultiplier(0);
	Scripting::DisableCarGenerators(1);
	//Scripting::DisableCarGeneratorsWithHeli(1);
	Scripting::SetNoResprays(1);
	Scripting::SwitchAmbientPlanes(0);
	Scripting::SwitchArrowAboveBlippedPickups(0);
	Scripting::SwitchRandomBoats(0);
	Scripting::SwitchRandomTrains(0);
	Scripting::SwitchGarbageTrucks(0);

	Scripting::SetScenarioPedDensityMultiplier(0, 0);
	//Scripting::SetParkedCarDensityMultiplier(0);
	//NativeInvoke::Invoke<u32>("SET_PARKED_CAR_DENSITY_MULTIPLIER", 0);
	Scripting::SetParkedCarDensityMultiplier(0);
	//Scripting::SetRandomCarDensityMultiplier(0);
	//NativeInvoke::Invoke<u32>("SET_RANDOM_CAR_DENSITY_MULTIPLIER", 0);
	Scripting::SetRandomCarDensityMultiplier(0);
	Scripting::SetMaxWantedLevel(0);

	Scripting::TerminateAllScriptsWithThisName("ambcontrolmain");
	Scripting::TerminateAllScriptsWithThisName("storytimer");
	Scripting::TerminateAllScriptsWithThisName("main");
	Scripting::TerminateAllScriptsWithThisName("initial");

	if(execution::getPatch() == 0x1120 || execution::getPatch() == 0x1130)
	{
		Scripting::TerminateAllScriptsWithThisName("ambNightclubBM");
		Scripting::TerminateAllScriptsWithThisName("ambNightclubBM2");
		Scripting::TerminateAllScriptsWithThisName("ambnightclubbm_min");

		Scripting::TerminateAllScriptsWithThisName("ambNightclubHC");
		Scripting::TerminateAllScriptsWithThisName("ambNightclubHC2");
		Scripting::TerminateAllScriptsWithThisName("ambNightclubM9");
		Scripting::TerminateAllScriptsWithThisName("ambNightclubM92");
	}
	Scripting::TerminateAllScriptsWithThisName("darts_launcher");
	Scripting::TerminateAllScriptsWithThisName("darts_trigger");
	Scripting::TerminateAllScriptsWithThisName("darts");

	Scripting::TerminateAllScriptsWithThisName("pool_game");
	Scripting::TerminateAllScriptsWithThisName("pool_table");
	
}

float spawnPos[4] = {0.0f, 0.0f, 0.0f, 0.0f};

void EFLCScripts::setSpawnPos(float x, float y, float z, float heading)
{
	spawnPos[0] = x;
	spawnPos[1] = y;
	spawnPos[2] = z;
	spawnPos[3] = heading;
}

void EFLCScripts::getSpawnPos(float &x, float &y, float &z, float &heading)
{
	x = spawnPos[0];
	y = spawnPos[1];
	z = spawnPos[2];
	heading = spawnPos[3];
}