#include "include/events.h"
#include "include/stunHandler.h"

EventResult cellLoadEventHandler::ProcessEvent(const RE::TESCellFullyLoadedEvent* a_event, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource) {
	//logger::debug("cell load event");
	return EventResult::kContinue;
}
