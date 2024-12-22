#pragma once
#include "debuffHandler.h"
#include "lib/robin_hood.h"
#include "lib/SimpleIni.h"
#pragma warning(disable: 4146)

/*Everything stored in game.*/
class data
{
public:
	static data* GetSingleton()
	{
		static data singleton;
		return  std::addressof(singleton);
	}
	/*Load all game data and store their pointers for later use.*/
	static void loadData();

	const static inline std::string AnimEvent_GuardCounter = "Val_GuardCounter_Trigger";
	const static inline std::string GraphBool_IsGuardCountering = "is_Guard_Countering";

	static inline RE::TESPackage* PKG_CirclingBehavior;

#pragma region blockFX
	static inline RE::TESObjectACTI* BlockFX;
	static inline RE::BGSExplosion* BlockSpark;
	static inline RE::BGSExplosion* BlockSparkFlare;
	static inline RE::BGSExplosion* BlockSparkRing;

#pragma endregion

#pragma region blockPerks
	static inline RE::BGSPerk* Perk_EnablePhysicalParry;
	static inline RE::BGSPerk* Perk_EnablePhysicalPerfectParry;
	
	static inline RE::BGSPerk* Perk_EnableArrowParry;
	static inline RE::BGSPerk* Perk_EnableSpellParry;
	
	static inline RE::BGSPerk* Perk_EnableArrowPerfectParry;
	static inline RE::BGSPerk* Perk_EnableSpellPerfectParry;
	
#pragma endregion
#pragma region Perk
	static inline RE::BGSPerk* debuffPerk;
#pragma endregion
#pragma region ExecutionIdles
	using KM = std::vector<RE::TESIdleForm*>;
	static inline KM KM_Humanoid_H2H = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Humanoid_Dagger = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Humanoid_Sword = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Humanoid_Axe = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Humanoid_Mace = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Humanoid_GreatSword = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Humanoid_2hw = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Humanoid_DW = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Humanoid_1hm_Back = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Humanoid_2hm_Back = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Humanoid_H2H_Back = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Humanoid_2hw_Back = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Falmer_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Falmer_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Falmer_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Undead_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Undead_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Undead_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Spider_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Spider_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Spider_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Gargoyle_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Gargoyle_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Gargoyle_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Giant_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Giant_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Giant_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Bear_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Bear_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Bear_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_SabreCat_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_SabreCat_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_SabreCat_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Wolf_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Wolf_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Wolf_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Troll_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Troll_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Troll_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Hagraven_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Hagraven_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Hagraven_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Spriggan_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Spriggan_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Spriggan_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Boar_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Boar_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Boar_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Riekling_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Riekling_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Riekling_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_AshHopper_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_AshHopper_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_AshHopper_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Centurion_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Centurion_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Centurion_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Ballista_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Ballista_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Ballista_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_ChaurusFlyer_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_ChaurusFlyer_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_ChaurusFlyer_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Lurker_1hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Lurker_2hm= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Lurker_2hw= std::vector<RE::TESIdleForm*>();
	static inline KM KM_Dragon_1hm = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Dragon_2hm = std::vector<RE::TESIdleForm*>();
	static inline KM KM_Dragon_2hw = std::vector<RE::TESIdleForm*>();

#pragma endregion
#pragma region ExecutionRaces
	enum raceCatagory
	{
		Humanoid = 0,
		Undead,
		Falmer,
		Spider,
		Gargoyle,
		Giant,
		Bear,
		SabreCat,
		Wolf,
		Troll,
		Hagraven,
		Spriggan,
		Boar,
		Riekling,
		AshHopper,
		SteamCenturion,
		DwarvenBallista,
		ChaurusFlyer,
		Lurker,
		Dragon
	};
	/*Mapping of all loaded races -> catagory of those races.*/
	//Note: catagorization of execution races has to be done manually.
	static inline robin_hood::unordered_map<RE::TESRace*, raceCatagory> raceMapping;
	/*Look up the actor from race mapping, and return whether the actor belongs to a race catagory.
	@param a_actor: actor whose race will be verified.
	@param a_catagory: race catagory to match.*/
	static bool isRaceType(RE::Actor* a_actor, raceCatagory a_catagory);
#pragma endregion
#pragma region sounds
	static inline RE::BGSSoundDescriptorForm* soundParryShield;
	static inline RE::BGSSoundDescriptorForm* soundParryWeapon;

	static inline RE::BGSSoundDescriptorForm* soundParryWeapon_perfect;
	static inline RE::BGSSoundDescriptorForm* soundParryShield_perfect;

	static inline RE::BGSSoundDescriptorForm* soundParryShield_gb;
	static inline RE::BGSSoundDescriptorForm* soundParryWeapon_gb;

	static inline RE::BGSSoundDescriptorForm* soundTackleStart;

	static inline RE::BGSSoundDescriptorForm* soundStunBreak;
	
	
#pragma endregion
#pragma region gameDifficultySettings
	static inline float fDiffMultHPByPCVE;
	static inline float fDiffMultHPByPCE;
	static inline float fDiffMultHPByPCN;
	static inline float fDiffMultHPByPCH;
	static inline float fDiffMultHPByPCVH;
	static inline float fDiffMultHPByPCL;
	static inline float fDiffMultHPToPCVE;
	static inline float fDiffMultHPToPCE;
	static inline float fDiffMultHPToPCN;
	static inline float fDiffMultHPToPCH;
	static inline float fDiffMultHPToPCVH;
	static inline float fDiffMultHPToPCL;
	
	static inline float fCombatHitConeAngle;
#pragma endregion
private:
	/*Directory storing all executable race and the mapping to their killmoves.*/
	static inline const std::string kmRaceDir = "Data/SKSE/Plugins/ValhallaCombat/RaceMapping";
	/*Path of file storing all killmoves.*/
	static inline const char* kmFilePath = "Data\\SKSE\\Plugins\\ValhallaCombat\\Killmoves.ini";

	/*Lookup an idle from a certain esp, and store it in an idleContainer. Errors if the idle does not exist.
	@param DATA: pointer to TESDataHandler's singleton.
	@param form: formID of the idle.
	@param pluginName: name of the plugin storing idle.
	@param idleContainer: Vector to store loaded idle.
	@return Whether the idle exists and therefore can be loaded or not.*/
	static bool lookupIdle(RE::TESDataHandler* DATA, RE::FormID form, std::string pluginName, std::vector<RE::TESIdleForm*>* idleContainer);

	/*Load a simpleIni section storing a set of idles belonging to that section.
	@param DATA: pointer to TESDataHandler's singleton.
	@param idleContainer: Vector to store loaded idles.
	@param ini: ini storing idle sections.
	@param section: a simpleIni's section, in string, to be loaded.*/
	static void loadIdleSection(RE::TESDataHandler* DATA, std::vector<RE::TESIdleForm*>* idleContainer,
		CSimpleIniA& ini, const char* section);

	/*Load all execution animations.*/
	static void loadIdle(RE::TESDataHandler* data);

	/*Read race from the plugin, and map it to an execution type through ExecutionRaceMap.
	@param DATA: pointer to TESDataHandler's singleton.
	@param form: formID of the race.
	@param pluginName: name of the plugin storing the race.
	@param raceCatagory: type of the execution race to be mapped to.
	@return If the race form exists or not.*/
	static bool pairUpRace(RE::TESDataHandler* DATA, RE::FormID form, std::string pluginName, raceCatagory raceType);

	/*Load a simpleIni section storing a set of races belonging to that section's corresponding killmove.
	@param DATA: pointer to TESDataHandler's singleton.
	@param raceType: type of the execution race to be mapped to.
	@param ini: ini whose section to load.
	@param section: a simpleIni's section, in string, to be loaded.*/
	static void loadRaceSection(RE::TESDataHandler* DATA, raceCatagory raceType, CSimpleIniA& ini, const char* section);

	/*Load a simpleIni file storing mapping of executable races.*/
	static void loadExecutableRaceIni(RE::TESDataHandler* DATA, const char* ini_path);

	/*Load all executable races from all ini files by iterating over them.*/
	static void loadExecutableRace(RE::TESDataHandler* data);

	/*Load all sounds from game.*/
	static void loadSound(RE::TESDataHandler* data);

	/*Load all perks from game.*/
	static void loadPerk(RE::TESDataHandler* data);

	/*Load all difficulty multiplier from game.*/
	static void loadDifficultySettings(RE::GameSettingCollection* gameSettings);

	static void loadBlockFX(RE::TESDataHandler* data);


};

namespace inlineUtils
{
	/*Parse a string into a vector of tokens.*/
	std::vector<std::string> tokenize(const std::string& delimiter, const std::string& str);

	/*Turn a formID into a uint32. Useful when reading from ini.*/
	bool ToInt32(std::string str, int& value);
}
