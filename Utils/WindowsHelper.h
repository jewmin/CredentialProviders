#pragma once

#include "Header.h"

namespace Utils {

std::wstring GetMachineName();
bool IsUserLocalAdmin(std::wstring username);
std::wstring GetSessionUserName(DWORD SessionId);
std::wstring GetSessionDomainName(DWORD SessionId);
DWORD GetCurrentSessionId();

}
