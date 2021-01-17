#pragma once

#include "IWinlogon.h"
#include "Utils.h"

class Gina {
public:
    static BOOL Negotiate(DWORD dwWinlogonVersion, PDWORD pdwDllVersion);
    static BOOL Initialize(HANDLE hWlx, PVOID pWinlogonFunctions, Gina * * ppGina);

public:
    int LoggedOutSAS(DWORD dwSasType, PLUID pAuthenticationId, PSID pLogonSid, PDWORD pdwOptions, PHANDLE phToken, PWLX_MPR_NOTIFY_INFO pNprNotifyInfo, PVOID * pProfile);
    int LoggedOnSAS(DWORD dwSasType);
    int WkstaLockedSAS(DWORD dwSasType);

    BOOL ActivateUserShell(PWSTR pszDesktopName, PWSTR pszMprLogonScript, PVOID pEnvironment);

    VOID DisplaySASNotice();
    VOID DisplayLockedNotice();

    BOOL IsLockOk();
    BOOL IsLogoffOk();

    VOID Logoff();
    VOID Shutdown(DWORD ShutdownType);

    BOOL ScreenSaverNotify(BOOL * pSecure);
    BOOL StartApplication(PWSTR pszDesktopName, PVOID pEnvironment, PWSTR pszCmdLine);

    BOOL NetworkProviderLoad(PWLX_MPR_NOTIFY_INFO pNprNotifyInfo);

    BOOL DisplayStatusMessage(HDESK hDesktop, DWORD dwOptions, PWSTR pTitle, PWSTR pMessage);
    BOOL GetStatusMessage(DWORD * pdwOptions, PWSTR pMessage, DWORD dwBufferSize);
    BOOL RemoveStatusMessage();

    BOOL GetConsoleSwitchCredentials(PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 pCredInfo);
    VOID ReconnectNotify();
    VOID DisconnectNotify();

private:
    Gina(IWinlogon * pWinlogon, HANDLE LsaHandle);

private:
    IWinlogon *             pWinlogon_;
    HANDLE                  LsaHandle_;             // Pointer that receives a handle used in future authentication function calls.
    HANDLE                  UserToken_;             // Handle of the users token.
    PWSTR                   ProfilePath_;           // UNICODE_STRING specifying the path to the user's roaming profile if the user has a roaming profile. For example: \SomeServer\SomeShare\MyUserName
};
