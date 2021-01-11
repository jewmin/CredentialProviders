#pragma once

#include "Commond.h"

namespace Utils {

namespace Protocol {

class LoginResponse : public Commond {
public:
	LoginResponse() : Commond(RESPONSE_LOGIN) {
		wmemset(UserName, 0, sizeof(UserName) / sizeof(wchar_t));
		wmemset(Password, 0, sizeof(Password) / sizeof(wchar_t));
	}
	virtual ~LoginResponse() {}

public:
	wchar_t UserName[128];
	wchar_t Password[128];
};

}

}