#pragma once

#include "Header.h"

namespace Utils {

// 获取计算机名
std::wstring GetMachineName();
// 判断是否管理员账号
bool IsUserLocalAdmin(std::wstring username);
// 返回指定会话的用户名
std::wstring GetSessionUserName(DWORD SessionId);
// 返回指定会话的域名
std::wstring GetSessionDomainName(DWORD SessionId);
// 获取当前会话
DWORD GetCurrentSessionId();

}
