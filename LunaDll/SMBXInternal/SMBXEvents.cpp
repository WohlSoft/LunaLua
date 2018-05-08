#include "SMBXEvents.h"

// GET
SMBXEvent* SMBXEvents::Get(int index) {
    return SMBXEvent::Get(index);
}

// GET BY NAME
SMBXEvent* SMBXEvents::GetByName(std::wstring Name) {	
	SMBXEvent* pEventBase = (SMBXEvent*)GM_EVENTS_PTR;

	for(int i = 0; i < 100; i++) {
		if(pEventBase[i].pName == Name)
			return &(pEventBase[i]);
	}
	return 0;
}	

void SMBXEvents::TriggerEvent(int Index, int UnknownArg) {
}

void SMBXEvents::TriggerEvent(std::wstring str, short forceNoSmoke) {
    VB6StrPtr eventName(str);
    native_triggerEvent(&eventName, &forceNoSmoke);
}