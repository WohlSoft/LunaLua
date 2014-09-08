#include "SMBXEvents.h"

// GET
SMBXEvent* SMBXEvents::Get(int index) {
	if(GM_EVENTS_PTR == 0 || index < 0 || index > 100) {
		return NULL;
	} else {
		
		SMBXEvent* pEventBase = (SMBXEvent*)GM_EVENTS_PTR;
		return &(pEventBase[index]);
	}	
}

// GET BY NAME
SMBXEvent* SMBXEvents::GetByName(std::wstring Name) {	
	SMBXEvent* pEventBase = (SMBXEvent*)GM_EVENTS_PTR;

	for(int i = 0; i < 100; i++) {
		if(std::wcscmp(pEventBase[i].pName, Name.c_str()) == 0)
			return &(pEventBase[i]);
	}
	return 0;
}	

void SMBXEvents::TriggerEvent(int Index, int UnknownArg) {
}

void SMBXEvents::TriggerEvent(std::wstring str, int UnknownArg) {
	char* dbg = "TRIGGER EVENT DBG";
	typedef int triggerfunc(wchar_t**, int*);			
	wchar_t wbuf[250];
	ZeroMemory(wbuf, 500);
	wchar_t* wbufptr = wbuf+2;

	// Place the length of the string in bytes first
	int strlen = str.length() * 2;
	wbuf[0] = strlen;

	// Copy the string's bytes 4 bytes ahead of the len
	memcpy((void*)&wbuf[2], str.data(), strlen);

	triggerfunc* f = (triggerfunc*)GF_TRIGGER_EVENT;
	int ret = f((&wbufptr), &UnknownArg);
}