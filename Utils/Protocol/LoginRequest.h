#pragma once

#include "Commond.h"

namespace Utils {

namespace Protocol {

struct LoginRequest {
	DWORD SessionID;
	wchar_t UserName[128];
	wchar_t Password[128];

	LoginRequest()
		: SessionID((DWORD)-1) {
		wmemset(UserName, 0, sizeof(UserName) / sizeof(wchar_t));
		wmemset(Password, 0, sizeof(Password) / sizeof(wchar_t));
	}
};

}

}
