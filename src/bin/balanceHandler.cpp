//#include "include/balanceHandler.h"
//#include "include/reactionHandler.h"
//#include "include/offsets.h"
//#include "include/Utils.h"
///*
//inline const float balanceRegenTime = 6;//time it takes for balance to regen, in seconds.
//
//void balanceHandler::update() {
//	//logger::debug("update");
//	/*if (garbageCollectionQueued) {
//		collectGarbage();
//		garbageCollectionQueued = false;
//	}*/
//
//	mtx_balanceBrokenActors.lock();
//	if (balanceBrokenActors.empty()) {//stop updating when there is 0 actor need to regen balance.
//		mtx_balanceBrokenActors.unlock();
//		//logger::debug("no balance broken actors, stop update");
//		ValhallaCombat::GetSingleton()->deactivateUpdate(ValhallaCombat::HANDLER::balanceHandler);
//		return;
//	}
//
//	//logger::debug("non-empty balance map");
//	//regenerate balance for all balance broken actors.
//	
//	auto it = balanceBrokenActors.begin();
//	mtx_actorBalanceMap.lock();
//	while (it != balanceBrokenActors.end()) {
//		if (!actorBalanceMap.contains(*it)) { //edge case: actor's balance broken but no longer tracked on actor balance map.
//			//logger::debug("edge case");
//			it = balanceBrokenActors.erase(it);
//			continue;
//		}
//		//regen a single actor's balance.
//		auto* balanceData = &actorBalanceMap.find(*it)->second;
//		float regenVal = balanceData->first * RE::GetSecondsSinceLastFrame() * 1 / balanceRegenTime;
//		//logger::debug(regenVal);
//		//logger::debug(a_balanceData.second);
//		//logger::debug(a_balanceData.first);
//		if (balanceData->second + regenVal >= balanceData->first) {//this regen exceeds actor's max balance.
//			//logger::debug("{}'s balance has recovered", (*it)->GetName());
//			balanceData->second = balanceData->first;//reset balance.
//			debuffHandler::GetSingleton()->quickStopStaminaDebuff(*it);
//			it = balanceBrokenActors.erase(it);
//			continue;
//		}
//		else {
//			//logger::debug("normal regen");
//			balanceData->second += regenVal;
//		}
//		it++;
//	}
//
//	mtx_actorBalanceMap.unlock();
//	mtx_balanceBrokenActors.unlock();
//
//}
//
//void balanceHandler::queueGarbageCollection() {
//	garbageCollectionQueued = true;
//}
//
//float balanceHandler::calculateMaxBalance(RE::Actor* a_actor) {
//	return a_actor->GetPermanentActorValue(RE::ActorValue::kHealth);
//}
//
//void balanceHandler::trackBalance(RE::Actor* a_actor) {
//	float maxBalance = calculateMaxBalance(a_actor);
//	mtx_actorBalanceMap.lock();
//	actorBalanceMap.emplace(a_actor, std::pair<float, float> {maxBalance, maxBalance});
//	mtx_actorBalanceMap.unlock();
//}
//
//void balanceHandler::untrackBalance(RE::Actor* a_actor) {
//	mtx_actorBalanceMap.lock();
//	actorBalanceMap.erase(a_actor);
//	mtx_actorBalanceMap.unlock();
//}
//
//void balanceHandler::collectGarbage() {
//	logger::info("Cleaning up balance map...");
//	int ct = 0;
//	mtx_actorBalanceMap.lock();
//	auto it_balanceMap = actorBalanceMap.begin();
//	while (it_balanceMap != actorBalanceMap.end()) {
//		auto a_actor = it_balanceMap->first;
//		if (!a_actor || !a_actor->currentProcess || !a_actor->currentProcess->InHighProcess()) {
//			safeErase_BalanceBrokenActors(a_actor);
//			it_balanceMap = actorBalanceMap.erase(it_balanceMap);
//			ct++;
//			continue;
//		}
//		it_balanceMap++;
//	}
//	mtx_actorBalanceMap.unlock();
//	logger::info("...done; cleaned up {} inactive actors.", ct);
//}
//
//void balanceHandler::reset() {
//	logger::info("Reset all balance...");
//	mtx_actorBalanceMap.lock();
//	actorBalanceMap.clear();
//	mtx_actorBalanceMap.unlock();
//	mtx_balanceBrokenActors.lock();
//	balanceBrokenActors.clear();
//	mtx_balanceBrokenActors.unlock();
//	logger::info("..done");
//}
//
//bool balanceHandler::isBalanceBroken(RE::Actor* a_actor) {
//	mtx_balanceBrokenActors.lock();
//	if (balanceBrokenActors.contains(a_actor)) {
//		mtx_balanceBrokenActors.unlock();
//		return true;
//	}
//	else {
//		mtx_balanceBrokenActors.unlock();
//		return false;
//	}
//}
//
//void balanceHandler::damageBalance(DMGSOURCE dmgSource, RE::Actor* a_aggressor, RE::Actor* a_victim, float damage) {
//	//logger::debug("damaging balance: aggressor: {}, victim: {}, damage: {}", aggressor->GetName(), victim->GetName(), damage);
//	mtx_actorBalanceMap.lock();
//	if (!actorBalanceMap.contains(a_victim)) {
//		mtx_actorBalanceMap.unlock();
//		trackBalance(a_victim);
//		damageBalance(dmgSource, a_aggressor, a_victim, damage);
//		return;
//	}
//#define a_balanceData actorBalanceMap.find(a_victim)->second
//	//logger::debug("curr balance: {}", a_balanceData.second);
//	if (a_balanceData.second - damage <= 0) { //balance broken, ouch!
//		a_balanceData.second = 0;
//		mtx_actorBalanceMap.unlock();
//		mtx_balanceBrokenActors.lock();
//		if (!balanceBrokenActors.contains(a_victim)) {//if not balance broken already
//			//logger::debug("{}'s balance has broken", victim->GetName());
//			balanceBrokenActors.insert(a_victim);
//			if (dmgSource == DMGSOURCE::parry) {
//				reactionHandler::triggerStagger(a_aggressor, a_victim, reactionHandler::kLarge);
//			}
//			ValhallaCombat::GetSingleton()->activateUpdate(ValhallaCombat::HANDLER::balanceHandler);
//		}
//		else {//balance already broken, yet broken again, ouch!
//			//logger::debug("{}'s balance double broken", victim->GetName());
//			reactionHandler::triggerContinuousStagger(a_aggressor, a_victim, reactionHandler::kLarge);
//		}
//		mtx_balanceBrokenActors.unlock();
//		
//	}
//	else {
//		//logger::debug("normal balance damage.");
//		a_balanceData.second -= damage;
//		mtx_actorBalanceMap.unlock();
//		mtx_balanceBrokenActors.lock();
//		if (balanceBrokenActors.contains(a_victim)) {
//			if (dmgSource == DMGSOURCE::powerAttack) {
//				reactionHandler::triggerStagger(a_aggressor, a_victim, reactionHandler::reactionType::kKnockBack);
//			}
//			else {
//				reactionHandler::triggerContinuousStagger(a_aggressor, a_victim, reactionHandler::kLarge);
//			}
//		}//if balance broken, trigger stagger.
//		else if ((dmgSource == DMGSOURCE::powerAttack  || dmgSource == DMGSOURCE::bash)
//				&& !debuffHandler::GetSingleton()->isInDebuff(a_aggressor)) //or if is power attack and not in debuff
//		{
//			reactionHandler::triggerContinuousStagger(a_aggressor, a_victim, reactionHandler::kLarge);
//		}
//		mtx_balanceBrokenActors.unlock();
//	}
//}
//
//void balanceHandler::recoverBalance(RE::Actor* a_actor, float recovery) {
//	mtx_actorBalanceMap.lock();
//	if (!actorBalanceMap.contains(a_actor)) {
//		mtx_actorBalanceMap.unlock();
//		return;
//	}
//	float attempedRecovery = actorBalanceMap[a_actor].second + recovery;
//	if (attempedRecovery >= actorBalanceMap[a_actor].first) {//balance fully recovered.
//		actorBalanceMap[a_actor].second = actorBalanceMap[a_actor].first;
//		mtx_actorBalanceMap.unlock();
//		if (isBalanceBroken(a_actor)) {
//			safeErase_BalanceBrokenActors(a_actor);
//			debuffHandler::GetSingleton()->quickStopStaminaDebuff(a_actor);
//		}
//	}
//	else {
//		actorBalanceMap[a_actor].second = attempedRecovery;
//		mtx_actorBalanceMap.unlock();
//	}
//
//
//}
//
//void balanceHandler::processBalanceDamage(DMGSOURCE dmgSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage) {
//	if (!settings::bBalanceToggle) {
//		return;
//	}
//	baseDamage *= 2;
//	if (isBalanceBroken(victim) && dmgSource < DMGSOURCE::bash) {
//		recoverBalance(victim, baseDamage * 1);
//		baseDamage = 0;
//	}
//	else {
//		if (debuffHandler::GetSingleton()->isInDebuff(victim)) {
//			baseDamage *= 1.5;
//		}
//		if (dmgSource == DMGSOURCE::parry) {
//			baseDamage *= 1.5;
//		}
//		if (victim->IsRangedAttacking() || ValhallaUtils::isCasting(victim)) {
//			baseDamage * 2.25;
//		}
//	}
//	damageBalance(dmgSource, aggressor, victim, baseDamage);
//}
//
//void balanceHandler::safeErase_ActorBalanceMap(RE::Actor* a_actor) {
//	mtx_actorBalanceMap.lock();
//	actorBalanceMap.erase(a_actor);
//	mtx_actorBalanceMap.unlock();
//}
//
//void balancehandler::safeerase_balancebrokenactors(re::actor* a_actor) {
//	mtx_balancebrokenactors.lock();
//	balancebrokenactors.erase(a_actor);
//	mtx_balancebrokenactors.unlock();
//}
