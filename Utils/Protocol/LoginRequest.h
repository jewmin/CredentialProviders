#pragma once

#include "Commond.h"

namespace Utils {

namespace Protocol {

// 授权请求协议
struct LoginRequest {
	// 会话
	DWORD SessionID;
	// 授权用户名
	wchar_t UserName[MAX_STRING_SIZE];
	// 授权密码
	wchar_t Password[MAX_STRING_SIZE];
	// 请求序列号，返回时带回
	int Sn;

	LoginRequest()
		: SessionID((DWORD)-1), Sn(0) {
		wmemset(UserName, 0, sizeof(UserName) / sizeof(wchar_t));
		wmemset(Password, 0, sizeof(Password) / sizeof(wchar_t));
	}

	LoginRequest(DWORD session_id, const std::wstring & username, const std::wstring & password, int sn = 0)
		: SessionID(session_id), Sn(sn) {
		wcsncpy_s(UserName, username.c_str(), MAX_STRING_SIZE - 1);
		UserName[MAX_STRING_SIZE - 1] = 0;
		wcsncpy_s(Password, password.c_str(), MAX_STRING_SIZE - 1);
		Password[MAX_STRING_SIZE - 1] = 0;
	}

	LoginRequest(const LoginRequest & rhs)
		: SessionID(rhs.SessionID), Sn(rhs.Sn) {
		wmemcpy(UserName, rhs.UserName, sizeof(UserName) / sizeof(wchar_t));
		wmemcpy(Password, rhs.Password, sizeof(Password) / sizeof(wchar_t));
	}
};

}

}
