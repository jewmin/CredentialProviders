#pragma once

#include "Commond.h"

namespace Utils {

namespace Protocol {

class LoginResponse : public Commond {
public:
	LoginResponse()
		: Commond(RESPONSE_LOGIN)
		, Result(false) {
		wmemset(UserName, 0, sizeof(UserName) / sizeof(wchar_t));
		wmemset(Password, 0, sizeof(Password) / sizeof(wchar_t));
	}

	virtual ~LoginResponse() {
	}

public:
	bool Result;
	wchar_t UserName[128];
	wchar_t Password[128];
};

}

}
