#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <WinWlx.h>
#include <NTSecAPI.h>

class SecurityHelper {
public:
    static PWSTR LocalAllocString(PCWSTR lpString);
    static bool RegisterLogonProcess(PHANDLE pLsaHandle);
    static bool CallLsaLogonUser(HANDLE hLsaHandle, PCWSTR szDomain, PCWSTR szUserName, PCWSTR szPassword, SECURITY_LOGON_TYPE logonType, PLUID pLogonId, PHANDLE pToken, PMSV1_0_INTERACTIVE_PROFILE * ppProfile, DWORD * pdwWin32Error);
    // static bool GetLogonSid(HANDLE htok, void* psid, DWORD cbMax);
    // static bool GetLogonSessionId(HANDLE htok, LUID* pluid);
    // static bool ExtractProfilePath(wchar_t** ppProfilePath, MSV1_0_INTERACTIVE_PROFILE* pProfile);
    // static bool AllocWinLogonProfile(WLX_PROFILE_V1_0** ppWinLogonProfile, const wchar_t* profilePath);
    // static bool CreateProcessAsUserOnDesktop(HANDLE hToken, wchar_t* programImage, wchar_t* desktop, void* env);
    static bool ImpersonateAndGetUserName(HANDLE hUserToken, PWSTR pszUserName, int cch);
    // static bool IsSameUser(HANDLE hToken1, HANDLE hToken2, bool* pbIsSameUser);
    // static bool IsAdmin(HANDLE hToken);
};
