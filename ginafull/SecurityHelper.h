// SecurityHelper.h
//
// Routines that interface with Win32 and LSA security APIs
//

#pragma once

class SecurityHelper {
public:
    static bool RegisterLogonProcess(const char* logonProcessName, HANDLE* phLsa);
    static bool CallLsaLogonUser(HANDLE hLsa,
                                const wchar_t* domain,
                                const wchar_t* user,
                                const wchar_t* pass,
                                SECURITY_LOGON_TYPE logonType,
                                LUID* pLogonSessionId,
                                HANDLE* phToken,
                                MSV1_0_INTERACTIVE_PROFILE** ppProfile,
                                DWORD* pWin32Error);
    static bool GetLogonSid(HANDLE htok, void* psid, DWORD cbMax);
    static bool GetLogonSessionId(HANDLE htok, LUID* pluid);
    static bool ExtractProfilePath(wchar_t** ppProfilePath, MSV1_0_INTERACTIVE_PROFILE* pProfile);
    static bool AllocWinLogonProfile(WLX_PROFILE_V1_0** ppWinLogonProfile, const wchar_t* profilePath);
    static bool CreateProcessAsUserOnDesktop(HANDLE hToken, wchar_t* programImage, wchar_t* desktop, void* env);
    static bool ImpersonateAndGetUserName(HANDLE hToken, wchar_t* name, int cch);
    static bool IsSameUser(HANDLE hToken1, HANDLE hToken2, bool* pbIsSameUser);
    static bool IsAdmin(HANDLE hToken);
private:
    SecurityHelper() {} // not meant to be instantiated
};
