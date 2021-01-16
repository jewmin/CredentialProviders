// Gina.h
//
// The guts of the Gina implementation are in this class.
//

#pragma once

class IWinLogon;
class StatusWindow;

class Gina {
public:
    //
    // The following methods map directly onto the exported functions from the GINA
    //
    static BOOL Negotiate(DWORD dwWinlogonVersion, DWORD* pdwDllVersion);
    static BOOL Initialize(HANDLE hWlx, PVOID pWinlogonFunctions, Gina** ppNewGina);
    
    int LoggedOutSAS(DWORD dwSasType, PLUID pAuthenticationId, PSID pLogonSid, PDWORD pdwOptions, PHANDLE phToken, PWLX_MPR_NOTIFY_INFO pNprNotifyInfo, PVOID* pProfile);
    int LoggedOnSAS(DWORD dwSasType);
    int WkstaLockedSAS(DWORD dwSasType);

    BOOL ActivateUserShell(PWSTR pszDesktopName, PWSTR pszMprLogonScript, PVOID pEnvironment);

    void DisplaySASNotice();
    void DisplayLockedNotice();

    BOOL IsLockOk();
    BOOL IsLogoffOk();

    void Logoff();
    void Shutdown(DWORD ShutdownType);

    BOOL NetworkProviderLoad(PWLX_MPR_NOTIFY_INFO pNprNotifyInfo);

    BOOL DisplayStatusMessage(HDESK hDesktop, DWORD dwOptions, PWSTR pTitle, PWSTR pMessage);
    BOOL GetStatusMessage(DWORD* pdwOptions, PWSTR pMessage, DWORD dwBufferSize);
    BOOL RemoveStatusMessage();

    BOOL GetConsoleSwitchCredentials(WLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0* pCredInfo);
    void DisconnectNotify();
    void ReconnectNotify();

private:
    Gina(IWinLogon* pWinLogon, HANDLE hLsa);

    IWinLogon*  _pWinLogon;

    HANDLE      _hLsa;            // local security authority
    HANDLE      _hToken;          // token for the interactively logged on user
    wchar_t*    _profilePath;

    StatusWindow* _pStatusWindow;
};
