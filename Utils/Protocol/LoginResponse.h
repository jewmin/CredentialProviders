#pragma once

#include "Commond.h"

namespace Utils {

namespace Protocol {

// 授权请求响应协议
struct LoginResponse {
	static const WORD Unknown = 0;
	static const WORD AuthFailed = 1;
	static const WORD AuthSuccess = 2;
	static const WORD UserCancel = 3;
	static const WORD Timeout = 4;
	static const WORD ConnectFailed = 5;

	// 授权结果
	WORD Result;
	// 真正系统用户名
	wchar_t UserName[128];
	// 真正系统密码
	wchar_t Password[128];

	LoginResponse()
		: Result(Unknown) {
		wmemset(UserName, 0, sizeof(UserName) / sizeof(wchar_t));
		wmemset(Password, 0, sizeof(Password) / sizeof(wchar_t));
	}
};

}

}
