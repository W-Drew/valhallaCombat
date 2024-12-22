#pragma once
namespace RE
{

	enum DIFFICULTY
	{
		kNovice = 0,
		kApprentice = 1,
		kAdept = 2,
		kExpert = 3,
		kMaster = 4,
		kLegendary = 5
	};

	namespace Offset
	{
		inline void destroyProjectile(RE::Projectile* a_projectile)
		{
			using func_t = decltype(&destroyProjectile);
			REL::Relocation<func_t> func { RELOCATION_ID(42930, 44110) };
			func(a_projectile);
		}

		inline RE::TESObjectREFR* getEquippedShield(RE::Actor* a_actor)
		{
			using func_t = decltype(&getEquippedShield);
			REL::Relocation<func_t> func { RELOCATION_ID(37624, 38577) };
			return func(a_actor);
		}
	}
}



