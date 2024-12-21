#pragma once
#include "offsets.h" 
#include "bin/ValhallaCombat.hpp"          
#include "include/lib/robin_hood.h"
#include <cmath>
#include <numbers>
#include  <random>
#include  <iterator>
#define CONSOLELOG(msg) 	RE::ConsoleLog::GetSingleton()->Print(msg);

inline constexpr float PI = std::numbers::pi_v<float>;

namespace REL
{
// Similar to REL::RelocateMemberIfNewer except for the parent. Potentially upstream to CommonlibNG if use is common
template <class T, class This>
[[nodiscard]] inline T& RelocateParentIfNewer(Version v, This* a_self, std::ptrdiff_t older, std::ptrdiff_t newer)
{
	return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(a_self) -
									(REL::Module::get().version().compare(v) == std::strong_ordering::less ? older : newer));
}
}

namespace Utils
{
	namespace Actor
	{
		bool isHumanoid(RE::Actor* a_actor);
		RE::TESObjectWEAP* getWieldingWeapon(RE::Actor* a_actor);
		bool isDualWielding(RE::Actor* a_actor);
		bool isEquippedShield(RE::Actor* a_actor);
		bool isPowerAttacking(RE::Actor* a_actor);

		void damageav(RE::Actor* a, RE::ActorValue av, float val);
		bool tryDamageAv(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_damage);
		void restoreav(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_value);

		void refillActorValue(RE::Actor* a_actor, RE::ActorValue a_actorValue);

		void safeApplySpell(RE::SpellItem* a_spell, RE::Actor* a_actor);
		void safeRemoveSpell(RE::SpellItem* a_spell, RE::Actor* a_actor);
		void safeApplyPerk(RE::BGSPerk* a_perk, RE::Actor* a_actor);
		void safeRemovePerk(RE::BGSPerk* a_perk, RE::Actor* a_actor);
		bool isBackFacing(RE::Actor* actor1, RE::Actor* actor2);

		bool isBashing(RE::Actor* a_actor);
		bool canBlock(RE::Actor* a_actor);

		bool getGraphVariable(bool& r_gv, RE::Actor* a_actor, const RE::BSFixedString& a_variableName);
		bool getGraphVariable(float& r_gv, RE::Actor* a_actor, const RE::BSFixedString& a_variableName);
		bool getGraphVariable(int& r_gv, RE::Actor* a_actor, const RE::BSFixedString& a_variableName);
	}

	namespace AvOwner
	{
		inline RE::Actor* asActor(RE::ActorValueOwner* a_avOwner) {
			return &REL::RelocateParentIfNewer<RE::Actor>(SKSE::RUNTIME_SSE_1_6_629, a_avOwner, 0xB0, 0xB8);
		}
	}

	namespace TESObjectREFR
	{
		double GetHeadingAngle(RE::TESObjectREFR* a_object, RE::TESObjectREFR* a_target);
	}
}


namespace inlineUtils
{
	/*Send the target flying based on causer's location.
	@param magnitude: strength of a push.*/
	inline static void PushActorAway(RE::Actor* causer, RE::Actor* target, float magnitude)
	{
		auto targetPoint = causer->GetNodeByName(causer->GetActorRuntimeData().race->bodyPartData->parts[0]->targetName.c_str());
		RE::NiPoint3 vec = targetPoint->world.translate;
		causer->GetActorRuntimeData().currentProcess->KnockExplosion(target, vec, magnitude);
	}

	inline void SetRotationMatrix(RE::NiMatrix3& a_matrix, float sacb, float cacb, float sb)
	{
		float cb = std::sqrtf(1 - sb * sb);
		float ca = cacb / cb;
		float sa = sacb / cb;
		a_matrix.entry[0][0] = ca;
		a_matrix.entry[0][1] = -sacb;
		a_matrix.entry[0][2] = sa * sb;
		a_matrix.entry[1][0] = sa;
		a_matrix.entry[1][1] = cacb;
		a_matrix.entry[1][2] = -ca * sb;
		a_matrix.entry[2][0] = 0.0;
		a_matrix.entry[2][1] = sb;
		a_matrix.entry[2][2] = cb;
	}

	template<typename Iter, typename RandomGenerator>
	Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
		using difference_type = typename std::iterator_traits<Iter>::difference_type;
		std::uniform_int_distribution<difference_type> dis(0, std::distance(start, end) - 1);
		std::advance(start, dis(g));
		return start;
	}

	template<typename Iter>
	Iter select_randomly(Iter start, Iter end) {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return select_randomly(start, end, gen);
	}



	/*Slow down game time for a set period.
	@param a_duration: duration of the slow time.
	@param a_percentage: relative time speed to normal time(1).*/
	inline void slowTime(float a_duration, float a_percentage) {
		int duration_milisec = static_cast<int>(a_duration * 1000);
		RE::BSTimer::GetSingleton()->SetGlobalTimeMultiplier(a_percentage, true);
		/*Reset time here*/
		auto resetSlowTime = [](int a_duration) {
			std::this_thread::sleep_for(std::chrono::milliseconds(a_duration));
			RE::BSTimer::GetSingleton()->SetGlobalTimeMultiplier(1.f, true);
		};
		std::jthread resetThread(resetSlowTime, duration_milisec);
		resetThread.detach();
	}

	/*Calculate the real hit damage based on game difficulty settings, and whether the player is aggressor/victim,
	* assuming the damage is uncalculated raw damage.
	@param damage: raw damage taken from hitdata.
	@param aggressor: aggressor of this damage.
	@param victim: victim of this damage.*/
	inline void offsetRealDamage(float& damage, RE::Actor* aggressor, RE::Actor* victim) {
		if ((aggressor) && (aggressor->IsPlayerRef() || aggressor->IsPlayerTeammate())) {
			switch (RE::PlayerCharacter::GetSingleton()->GetGameStatsData().difficulty) {
			case RE::DIFFICULTY::kNovice: damage *= data::fDiffMultHPByPCVE; break;
			case RE::DIFFICULTY::kApprentice: damage *= data::fDiffMultHPByPCE; break;
			case RE::DIFFICULTY::kAdept: damage *= data::fDiffMultHPByPCN; break;
			case RE::DIFFICULTY::kExpert: damage *= data::fDiffMultHPByPCH; break;
			case RE::DIFFICULTY::kMaster: damage *= data::fDiffMultHPByPCVH; break;
			case RE::DIFFICULTY::kLegendary: damage *= data::fDiffMultHPByPCL; break;
			}
		}
		else if ((victim) && (victim->IsPlayerRef() || victim->IsPlayerTeammate())) {
			switch (RE::PlayerCharacter::GetSingleton()->GetGameStatsData().difficulty) {
			case RE::DIFFICULTY::kNovice: damage *= data::fDiffMultHPToPCVE; break;
			case RE::DIFFICULTY::kApprentice: damage *= data::fDiffMultHPToPCE; break;
			case RE::DIFFICULTY::kAdept: damage *= data::fDiffMultHPToPCN; break;
			case RE::DIFFICULTY::kExpert: damage *= data::fDiffMultHPToPCH; break;
			case RE::DIFFICULTY::kMaster: damage *= data::fDiffMultHPToPCVH; break;
			case RE::DIFFICULTY::kLegendary: damage *= data::fDiffMultHPToPCL; break;
			}
		}
	}

	inline void offsetRealDamageForPc(float& damage) {
		offsetRealDamage(damage, nullptr, RE::PlayerCharacter::GetSingleton());
	}

};

namespace TrueHUDUtils
{

	inline void flashActorValue(RE::Actor* a_actor, RE::ActorValue actorValue) {
		if (!settings::facts::TrueHudAPI_Obtained) {
			return;
		}
		if (a_actor) {
			ValhallaCombat::GetSingleton()->ersh_TrueHUD->FlashActorValue(a_actor->GetHandle(), actorValue, true);
		}	
	}

	inline void greyoutAvMeter(RE::Actor* a_actor, RE::ActorValue actorValue) {
		if (!settings::facts::TrueHudAPI_Obtained) {
			return;
		}
		auto ersh = ValhallaCombat::GetSingleton()->ersh_TrueHUD;
		ersh->OverrideBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
		ersh->OverrideBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::BarColor, 0x7d7e7d);
		ersh->OverrideBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::PhantomColor, 0xb30d10);
	}

	inline void revertAvMeter(RE::Actor* a_actor, RE::ActorValue actorValue) {
		if (!settings::facts::TrueHudAPI_Obtained) {
			return;
		}
		auto ersh = ValhallaCombat::GetSingleton()->ersh_TrueHUD;
		ersh->RevertBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::FlashColor);
		ersh->RevertBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::BarColor);
		ersh->RevertBarColor(a_actor->GetHandle(), actorValue, TRUEHUD_API::BarColorType::PhantomColor);
	}

	inline void greyOutSpecialMeter(RE::Actor* a_actor) {
		if (!settings::facts::TrueHudAPI_Obtained || !settings::facts::TrueHudAPI_HasSpecialBarControl) {
			return;
		}
		auto ersh = ValhallaCombat::GetSingleton()->ersh_TrueHUD;
		ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
		ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::BarColor, 0x7d7e7d);
		ersh->OverrideSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::PhantomColor, 0xb30d10);
		ersh->OverrideBarColor(a_actor->GetHandle(), RE::ActorValue::kHealth, TRUEHUD_API::BarColorType::FlashColor, 0xd72a2a);
	}

	inline void revertSpecialMeter(RE::Actor* a_actor) {
		if (!settings::facts::TrueHudAPI_Obtained || !settings::facts::TrueHudAPI_HasSpecialBarControl) {
			return;
		}
		auto ersh = ValhallaCombat::GetSingleton()->ersh_TrueHUD;
		ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::FlashColor);
		ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::BarColor);
		ersh->RevertSpecialBarColor(a_actor->GetHandle(), TRUEHUD_API::BarColorType::PhantomColor);
		ersh->RevertBarColor(a_actor->GetHandle(), RE::ActorValue::kHealth, TRUEHUD_API::BarColorType::FlashColor);
	}

};

class ValhallaUtils
{
public:
	class Papyrus
	{
	public:
		static void AddPackageOverride(RE::Actor* a_actor, RE::TESPackage* a_package, int priority, int flags)
		{
			auto callback = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>();
			auto args = RE::MakeFunctionArguments(std::move(a_actor), std::move(a_package), std::move(priority), std::move(flags));
			RE::BSScript::Internal::VirtualMachine::GetSingleton()->DispatchStaticCall("ActorUtil", "AddPackageOverride", args, callback);
		}

		static void RemovePackageOverride(RE::Actor* a_actor, RE::TESPackage* a_package)
		{
			auto callback = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>();
			auto args = RE::MakeFunctionArguments(std::move(a_actor), std::move(a_package));
			RE::BSScript::Internal::VirtualMachine::GetSingleton()->DispatchStaticCall("ActorUtil", "RemovePackageOverride", args, callback);
		}
	};


	/*Play sound with formid at a certain actor's position.
	@param a: actor on which to play sonud.
	@param formid: formid of the sound descriptor.*/
	static void playSound(RE::Actor* a, RE::BGSSoundDescriptorForm* a_descriptor, float a_volumeOverride = 1)
	{
		RE::BSSoundHandle handle;
		if (!RE::BSAudioManager::GetSingleton()->BuildSoundDataFromDescriptor(handle, a_descriptor, 0x10))
		{
			logger::error("Utils::playSound::BuildSoundDataFromDescriptor failed for FormId {}", a_descriptor->GetFormID());
			return;
		}
		if (!handle.SetPosition(a->data.location))
		{
			logger::error("Utils::playSound::SetPosition failed for actor {}", a->GetName());
			return;
		}
			
		handle.SetVolume(a_volumeOverride);
		handle.SetObjectToFollow(a->Get3D());
		handle.Play();
	}

	static void playSound(RE::Actor* a, std::vector<RE::BGSSoundDescriptorForm*> sounds)
	{
		playSound(a, *inlineUtils::select_randomly(sounds.begin(), sounds.end()));
	}

	/*
	@return actor a and actor b's absolute distance, if the radius is bigger than distance.
	@return -1, if the distance exceeds radius.*/
	static float getInRange(RE::Actor* a, RE::Actor* b, float maxRange) {
		float dist = a->GetPosition().GetDistance(b->GetPosition());
		if (dist <= maxRange) {
			return dist;
		}
		else {
			return -1;
		}
	}

	static void queueMessageBox(RE::BSFixedString a_message) {
		auto factoryManager = RE::MessageDataFactoryManager::GetSingleton();
		auto uiStrHolder = RE::InterfaceStrings::GetSingleton();
		auto factory = factoryManager->GetCreator<RE::MessageBoxData>(uiStrHolder->messageBoxData);
		auto messageBox = factory->Create();
		messageBox->unk4C = 4;
		messageBox->unk38 = 10;
		messageBox->bodyText = a_message;
		auto gameSettings = RE::GameSettingCollection::GetSingleton();
		auto sOk = gameSettings->GetSetting("sOk");
		messageBox->buttonText.push_back(sOk->GetString());
		messageBox->QueueMessage();
	}
#pragma endregion
	/*Clamp the raw damage to be no more than the aggressor's max raw melee damage output.*/
	static void clampDmg(float& dmg, RE::Actor* aggressor) {
		auto a_weapon = Utils::Actor::getWieldingWeapon(aggressor);
		if (a_weapon) {
			//logger::debug("weapon to clamp damage: {}", a_weapon->GetName());
			dmg = std::min(dmg, (float)a_weapon->GetAttackDamage());
		}
	}

	static bool isCasting(RE::Actor* a_actor) {
		for (int i = 0; i < 3; i++)
		{
			auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource(i));
			if (!caster)
				continue;
			if (caster->state) {
				return true;
			}
		}
		return false;
	}

	/*Set the projectile's cause to a new actor; reset the projectile's collision mask.
	@param a_projectile: projectile to be reset.
	@param a_actor: new owner of the projectile.
	@param a_projectile_collidable: pointer to the projectile collidable to rset its collision mask.*/
	static void resetProjectileOwner(RE::Projectile* a_projectile, RE::Actor* a_actor, RE::hkpCollidable* a_projectile_collidable) {
		a_projectile->SetActorCause(a_actor->GetActorCause());
		a_projectile->GetProjectileRuntimeData().shooter = a_actor->GetHandle();
		uint32_t a_collisionFilterInfo;
		a_actor->GetCollisionFilterInfo(a_collisionFilterInfo);
		a_projectile_collidable->broadPhaseHandle.collisionFilterInfo &= (0x0000FFFF);
		a_projectile_collidable->broadPhaseHandle.collisionFilterInfo |= (a_collisionFilterInfo << 16);
	}

	static inline bool ApproximatelyEqual(float A, float B)
	{
		return ((A - B) < FLT_EPSILON) && ((B - A) < FLT_EPSILON);
	}

	static bool PredictAimProjectile(RE::NiPoint3 a_projectilePos, RE::NiPoint3 a_targetPosition, RE::NiPoint3 a_targetVelocity, float a_gravity, RE::NiPoint3& a_projectileVelocity)
	{
		// http://ringofblades.com/Blades/Code/PredictiveAim.cs

		float projectileSpeedSquared = a_projectileVelocity.SqrLength();
		float projectileSpeed = std::sqrtf(projectileSpeedSquared);

		if (projectileSpeed <= 0.f || a_projectilePos == a_targetPosition) {
			return false;
		}

		float targetSpeedSquared = a_targetVelocity.SqrLength();
		float targetSpeed = std::sqrtf(targetSpeedSquared);
		RE::NiPoint3 targetToProjectile = a_projectilePos - a_targetPosition;
		float distanceSquared = targetToProjectile.SqrLength();
		float distance = std::sqrtf(distanceSquared);
		RE::NiPoint3 direction = targetToProjectile;
		direction.Unitize();
		RE::NiPoint3 targetVelocityDirection = a_targetVelocity;
		targetVelocityDirection.Unitize();

		float cosTheta = (targetSpeedSquared > 0)
			? direction.Dot(targetVelocityDirection)
			: 1.0f;

		bool bValidSolutionFound = true;
		float t;

		if (ApproximatelyEqual(projectileSpeedSquared, targetSpeedSquared)) {
			// We want to avoid div/0 that can result from target and projectile traveling at the same speed
			//We know that cos(theta) of zero or less means there is no solution, since that would mean B goes backwards or leads to div/0 (infinity)
			if (cosTheta > 0) {
				t = 0.5f * distance / (targetSpeed * cosTheta);
			}
			else {
				bValidSolutionFound = false;
				t = 1;
			}
		}
		else {
			float a = projectileSpeedSquared - targetSpeedSquared;
			float b = 2.0f * distance * targetSpeed * cosTheta;
			float c = -distanceSquared;
			float discriminant = b * b - 4.0f * a * c;

			if (discriminant < 0) {
				// NaN
				bValidSolutionFound = false;
				t = 1;
			}
			else {
				// a will never be zero
				float uglyNumber = sqrtf(discriminant);
				float t0 = 0.5f * (-b + uglyNumber) / a;
				float t1 = 0.5f * (-b - uglyNumber) / a;

				// Assign the lowest positive time to t to aim at the earliest hit
				t = std::min(t0, t1);
				if (t < FLT_EPSILON) {
					t = std::max(t0, t1);
				}

				if (t < FLT_EPSILON) {
					// Time can't flow backwards when it comes to aiming.
					// No real solution was found, take a wild shot at the target's future location
					bValidSolutionFound = false;
					t = 1;
				}
			}
		}

		a_projectileVelocity = a_targetVelocity + (-targetToProjectile / t);

		if (!bValidSolutionFound)
		{
			a_projectileVelocity.Unitize();
			a_projectileVelocity *= projectileSpeed;
		}

		if (!ApproximatelyEqual(a_gravity, 0.f))
		{
			float netFallDistance = (a_projectileVelocity * t).z;
			float gravityCompensationSpeed = (netFallDistance + 0.5f * a_gravity * t * t) / t;
			a_projectileVelocity.z = gravityCompensationSpeed;
		}

		return bValidSolutionFound;
	}



	static void ReflectProjectile(RE::Projectile* a_projectile)
	{
		a_projectile->GetProjectileRuntimeData().linearVelocity *= -1.f;

		// rotate model
		auto projectileNode = a_projectile->Get3D2();
		if (projectileNode)
		{
			RE::NiPoint3 direction = a_projectile->GetProjectileRuntimeData().linearVelocity;
			direction.Unitize();

			a_projectile->data.angle.x = asin(direction.z);
			a_projectile->data.angle.z = atan2(direction.x, direction.y);

			if (a_projectile->data.angle.z < 0.0) {
				a_projectile->data.angle.z += PI;
			}

			if (direction.x < 0.0) {
				a_projectile->data.angle.z += PI;
			}

			inlineUtils::SetRotationMatrix(projectileNode->local.rotate, -direction.x, direction.y, direction.z);
		}
	}

	/*Get the body position of this actor.*/
	static void getBodyPos(RE::Actor* a_actor, RE::NiPoint3& pos)
	{
		if (!a_actor->GetActorRuntimeData().race) {
			return;
		}
		RE::BGSBodyPart* bodyPart = a_actor->GetActorRuntimeData().race->bodyPartData->parts[0];
		if (!bodyPart) {
			return;
		}
		auto targetPoint = a_actor->GetNodeByName(bodyPart->targetName.c_str());
		if (!targetPoint) {
			return;
		}

		pos = targetPoint->world.translate;
	}

	/// <summary>
	/// Change the projectile's trajectory, aiming it at the target.
	/// </summary>
	/// <param name="a_projectile">Projectile whose trajectory will be changed.</param>
	/// <param name="a_target">New target to be aimed at.</param>
	static void RetargetProjectile(RE::Projectile* a_projectile, RE::TESObjectREFR* a_target)
	{
		a_projectile->GetProjectileRuntimeData().desiredTarget = a_target;

		auto projectileNode = a_projectile->Get3D2();
		auto targetHandle = a_target->GetHandle();

		RE::NiPoint3 targetPos = a_target->GetPosition();

		if (a_target->GetFormType() == RE::FormType::ActorCharacter) {
			getBodyPos(a_target->As<RE::Actor>(), targetPos);
		}

		RE::NiPoint3 targetVelocity;
		targetHandle.get()->GetLinearVelocity(targetVelocity);

		float projectileGravity = 0.f;
		if (auto ammo = a_projectile->GetProjectileRuntimeData().ammoSource) {
			if (auto bgsProjectile = ammo->data.projectile) {
				projectileGravity = bgsProjectile->data.gravity;
				if (auto bhkWorld = a_projectile->parentCell->GetbhkWorld()) {
					if (auto hkpWorld = bhkWorld->GetWorld1()) {
						auto vec4 = hkpWorld->gravity;
						float quad[4];
						_mm_store_ps(quad, vec4.quad);
						float gravity = -quad[2] * RE::bhkWorld::GetWorldScaleInverse();
						projectileGravity *= gravity;
					}
				}
			}
		}

		PredictAimProjectile(a_projectile->data.location, targetPos, targetVelocity, projectileGravity, a_projectile->GetProjectileRuntimeData().linearVelocity);

		// rotate
		RE::NiPoint3 direction = a_projectile->GetProjectileRuntimeData().linearVelocity;
		direction.Unitize();

		a_projectile->data.angle.x = asin(direction.z);
		a_projectile->data.angle.z = atan2(direction.x, direction.y);

		if (a_projectile->data.angle.z < 0.0) {
			a_projectile->data.angle.z += PI;
		}

		if (direction.x < 0.0) {
a_projectile->data.angle.z += PI;
		}

		inlineUtils::SetRotationMatrix(projectileNode->local.rotate, -direction.x, direction.y, direction.z);
	}
};

namespace DtryUtils
{
	/*Helper class to batch load forms from plugin records.*/
	class formLoader
	{
	private:
		RE::BSFixedString _pluginName;
		RE::TESDataHandler* _dataHandler;
		int _loadedForms;

	public:
		formLoader(RE::BSFixedString pluginName)
		{
			_pluginName = pluginName;
			_dataHandler = RE::TESDataHandler::GetSingleton();
			if (!_dataHandler) {
				logger::critical("Error: TESDataHandler not found.");
			}
			if (!_dataHandler->LookupModByName(pluginName)) {
				logger::critical("Error: {} not found.", pluginName.c_str());
			}
			logger::info("Loading from plugin {}...", pluginName.c_str());
		}

		~formLoader()
		{
			logger::info("Loaded {} forms from {}", _loadedForms, _pluginName.c_str());
		}

		/*Load a form from the plugin.*/
		template <class formType>
		void load(formType*& formRet, RE::FormID formID)
		{
			formRet = _dataHandler->LookupForm<formType>(formID, _pluginName);
			if (!formRet) {
				logger::critical("Error: null formID or wrong form type when loading {} from {}", formID, _pluginName.c_str());
			}
			_loadedForms++;
		}
	};

	/*Helper class to load from a simple ini file.*/
	class settingsLoader
	{
	private:
		std::shared_ptr<CSimpleIniA> _ini;
		const char* _section;
		int _loadedSettings;
		const char* _settingsFile;
	public:
		settingsLoader(const char* settingsFile)
		{
			_ini = std::make_shared<CSimpleIniA>();
			_ini->LoadFile(settingsFile);
			if (_ini->IsEmpty()) {
				logger::info("Warning: {} is empty.", settingsFile);
			}
			_settingsFile = settingsFile;
		};
		/*Set the active section. Load() will load keys from this section.*/
		void setActiveSection(const char* section)
		{
			_section = section;
		}
		/*Load a boolean value if present.*/
		void load(bool& settingRef, const char* key)
		{
			if (_ini->GetValue(_section, key)) {
				bool val = _ini->GetBoolValue(_section, key);
				settingRef = val;
				_loadedSettings++;
			}
		}
		/*Load a float value if present.*/
		void load(float& settingRef, const char* key)
		{
			if (_ini->GetValue(_section, key)) {
				float val = static_cast<float>(_ini->GetDoubleValue(_section, key));
				settingRef = val;
				_loadedSettings++;
			}
		}
		/*Load an unsigned int value if present.*/
		void load(uint32_t& settingRef, const char* key)
		{
			if (_ini->GetValue(_section, key)) {
				uint32_t val = static_cast<uint32_t>(_ini->GetDoubleValue(_section, key));
				settingRef = val;
				_loadedSettings++;
			}
		}

		/*Load an integer value if present.*/
		void load(int& settingRef, const char* key) {
			if (_ini->GetValue(_section, key)) {
				int val = static_cast<int>(_ini->GetDoubleValue(_section, key));
				settingRef = val;
				_loadedSettings++;
			}
		}

		void log() {
			logger::info("Loaded {} settings from {}", _loadedSettings, _settingsFile);
		}
	};
}

