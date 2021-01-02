#include "WindowsHelper.h"

namespace Utils {

std::wstring GetMachineName() {
	wchar_t computer[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD computer_len = ARRAYSIZE(computer);
	if (GetComputerName(computer, &computer_len)) {
		return computer;
	} else {
		return L".";
	}
}

bool IsUserLocalAdmin(std::wstring username) {
	bool result = false;
	LPUSER_INFO_3 info;
	if (::NetUserGetInfo(NULL, username.c_str(), 3, (LPBYTE *)&info) == NERR_Success) {
		result = (info->usri3_priv == USER_PRIV_ADMIN);
		::NetApiBufferFree(info);
	}
	return result;
}

std::wstring GetSessionUserName(DWORD SessionId) {
	std::wstring result;
	LPWSTR buffer = NULL;
	DWORD size = 0;
	if (::WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, SessionId, WTSUserName, &buffer, &size)) {
		result = buffer;
		::WTSFreeMemory(buffer);
	}
	return result;
}

std::wstring GetSessionDomainName(DWORD SessionId) {
	std::wstring result;
	LPWSTR buffer = NULL;
	DWORD size = 0;
	if (::WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, SessionId, WTSDomainName, &buffer, &size)) {
		result = buffer;
		::WTSFreeMemory(buffer);
	}
	return result;
}

DWORD GetCurrentSessionId() {
	DWORD SessionId = (DWORD)-1;
	::ProcessIdToSessionId(::GetCurrentProcessId(), &SessionId);
	return SessionId;
}

}
