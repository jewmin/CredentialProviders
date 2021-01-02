#include "WindowsHelper.h"

namespace Utils {

std::wstring GetMachineName() {
	WCHAR wsz[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD cch = ARRAYSIZE(wsz);
	if (GetComputerName(wsz, &cch)) {
		return wsz;
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
