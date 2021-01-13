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

	LoginRequest(DWORD session_id, const std::wstring & username, const std::wstring & password)
		: SessionID(session_id) {
		wcsncpy_s(UserName, username.c_str(), 127);
		UserName[127] = 0;
		wcsncpy_s(Password, password.c_str(), 127);
		Password[127] = 0;
	}

	LoginRequest(const LoginRequest & rhs)
		: SessionID(rhs.SessionID) {
		wmemcpy(UserName, rhs.UserName, sizeof(UserName) / sizeof(wchar_t));
		wmemcpy(Password, rhs.Password, sizeof(Password) / sizeof(wchar_t));
	}
};

}

}
