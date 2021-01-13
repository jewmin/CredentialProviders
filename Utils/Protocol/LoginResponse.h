#pragma once

#include "Commond.h"

namespace Utils {

namespace Protocol {

struct LoginResponse {
	static const WORD Unknown = 0;
	static const WORD AuthFailed = 1;
	static const WORD AuthSuccess = 2;
	static const WORD UserCancel = 3;
	static const WORD Timeout = 4;
	static const WORD ConnectFailed = 5;

	WORD Result;
	wchar_t UserName[128];
	wchar_t Password[128];

	LoginResponse()
		: Result(Unknown) {
		wmemset(UserName, 0, sizeof(UserName) / sizeof(wchar_t));
		wmemset(Password, 0, sizeof(Password) / sizeof(wchar_t));
	}
};

}

}
