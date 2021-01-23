#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <WinWlx.h>
#include <NTSecAPI.h>

class SecurityHelper {
public:
    static bool RegisterLogonProcess(HANDLE * phLsa);
    static bool CallLsaLogonUser(HANDLE hLsa, const wchar_t * domain, const wchar_t * user, const wchar_t * pass, SECURITY_LOGON_TYPE logonType, LUID * pLogonSessionId, HANDLE * phToken, MSV1_0_INTERACTIVE_PROFILE * * ppProfile, DWORD * pWin32Error);
    static bool GetLogonSid(HANDLE htok, void * psid, DWORD cbMax);
    static bool GetLogonSessionId(HANDLE htok, LUID * pluid);
    static bool ExtractProfilePath(wchar_t * * ppProfilePath, MSV1_0_INTERACTIVE_PROFILE * pProfile);
    static bool AllocWinLogonProfile(WLX_PROFILE_V1_0 * * ppWinLogonProfile, const wchar_t * profilePath);
    static bool CreateProcessAsUserOnDesktop(HANDLE hToken, wchar_t * programImage, wchar_t * desktop, void * env);
    static bool ImpersonateAndGetUserName(HANDLE hToken, wchar_t * name, int cch);
    static bool IsSameUser(HANDLE hToken1, HANDLE hToken2, bool * pbIsSameUser);
    static bool IsAdmin(HANDLE hToken);
    static wchar_t * LocalAllocString(const wchar_t * s);

    static bool ReadUserInitProgramList(wchar_t * * * programList, int * count);
    static void FreeUserInitProgramList(wchar_t * * programList, int count);
};
