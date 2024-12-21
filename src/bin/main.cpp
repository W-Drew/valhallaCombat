#include "SimpleIni.h"
#include "include/Hooks.h"
#include "include/data.h"
#include "include/debuffHandler.h"
#include "include/events.h"
#include "include/lib/TrueHUDAPI.h"
#include "include/logger.h"
#include "ValhallaCombat.hpp"
#include "include/settings.h"
#include "include/Utils.h"
#include "include/lib/ValhallaCombatAPI.h"
#include "include/ModAPI.h"

void initTrueHUDAPI() {
	auto val = ValhallaCombat::GetSingleton();
	val->ersh_TrueHUD = reinterpret_cast<TRUEHUD_API::IVTrueHUD3*>(TRUEHUD_API::RequestPluginAPI(TRUEHUD_API::InterfaceVersion::V3));
	if (val->ersh_TrueHUD) {
		logger::info("Obtained TruehudAPI - {0:x}", (uintptr_t)val->ersh_TrueHUD);
		settings::facts::TrueHudAPI_Obtained = true;
	} else {
		logger::info("TrueHUD API not found.");
		settings::facts::TrueHudAPI_Obtained = false;
	}
}

void initPrecisionAPI() {
	auto val = ValhallaCombat::GetSingleton();
	val->ersh_Precision = reinterpret_cast<PRECISION_API::IVPrecision1*>(PRECISION_API::RequestPluginAPI(PRECISION_API::InterfaceVersion::V1));
	if (val->ersh_Precision) {
		logger::info("Obtained PrecisionAPI - {0:x}", (uintptr_t)val->ersh_Precision);
		settings::facts::PrecisionAPI_Obtained = true;
		auto res = val->ersh_Precision->AddPreHitCallback(SKSE::GetPluginHandle(), blockHandler::precisionPrehitCallbackFunc);
		if (res == PRECISION_API::APIResult::OK || res == PRECISION_API::APIResult::AlreadyRegistered) {
			logger::info("Collision prehit callback successfully registered.");
		}
	} else {
		logger::info("Precision API not found.");
		settings::facts::PrecisionAPI_Obtained = false;
	}
}

void onDataLoaded() 
{
	Hooks::install();  //hook install has been postponed for compatibility with other plugins.

	settings::readSettings();
	events::registerAllEventHandlers();
	data::loadData();
	blockHandler::GetSingleton()->init();
	blockHandler::EldenCounterCompatibility::attemptInit(); //todo: fix this jank
}

void onPostLoad() {
	logger::info("Initializing API fetch...");
	initTrueHUDAPI();
	initPrecisionAPI();
	logger::info("...done");
}

void onPostLoadGame() {
	debuffHandler::GetSingleton()->stopDebuff(RE::PlayerCharacter::GetSingleton());
	stunHandler::GetSingleton()->reset();
	settings::updateGameSettings();
}



void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		onDataLoaded();
		break;
	case SKSE::MessagingInterface::kPostLoad:
		onPostLoad();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		onPostLoadGame();
		break;
	}
}

void onSKSEInit()
{
	settings::UpdateHandler::Register();
}

/*SKSEPluginInfo(
	.Version = Plugin::VERSION,
	.Name = Plugin::NAME,
	.Author = "D7ry",
	.StructCompatibility = SKSE::StructCompatibility::Independent,
	.RuntimeCompatibility = SKSE::VersionIndependence::AddressLibrary,
	.MinimumSKSEVersion = REL::Version{2, 0, 0, 6} /* Game version 1.5.39 */
//)

/*#ifndef NDEBUG
	while (!IsDebuggerPresent()) { Sleep(100); }
#endif*/

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{    	
	SetupLog();

	SKSE::Init(a_skse);

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	onSKSEInit();

	return true;
}


extern "C" DLLEXPORT void* SKSEAPI RequestPluginAPI(const VAL_API::InterfaceVersion a_interfaceVersion)
{
	//auto api = Messaging::TrueHUDInterface::GetSingleton();
	auto api = ModAPI::VALInterface::GetSingleton();
	logger::info("ValhallaCombat::RequestPluginAPI called");

	switch (a_interfaceVersion) {
	case VAL_API::InterfaceVersion::V1:
	case VAL_API::InterfaceVersion::V2:
		logger::info("ValhallaCombat::RequestPluginAPI returned the API singleton");
		return static_cast<void*>(api);
	}
	logger::info("ValhallaCombat::RequestPluginAPI requested the wrong interface version");
	return nullptr;
}
