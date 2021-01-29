#include <WinSock2.h>
#include <Windows.h>
#include <WinWlx.h>
#include "Gina.h"

static const PWSTR s_WST_Strings[] =
{
    L"WLX_SAS_TYPE_TIMEOUT",
    L"WLX_SAS_TYPE_CTRL_ALT_DEL",
    L"WLX_SAS_TYPE_SCRNSVR_TIMEOUT",
    L"WLX_SAS_TYPE_SCRNSVR_ACTIVITY",
    L"WLX_SAS_TYPE_USER_LOGOFF",
    L"WLX_SAS_TYPE_SC_INSERT",
    L"WLX_SAS_TYPE_SC_REMOVE",
    L"WLX_SAS_TYPE_AUTHENTICATED",
    L"WLX_SAS_TYPE_SC_FIRST_READER_ARRIVED",
    L"WLX_SAS_TYPE_SC_LAST_READER_REMOVED",
    L"WLX_SAS_TYPE_SWITCHUSER",
    L"WLX_SAS_TYPE_MAX_MSFT_VALUE",
};

static const PWSTR s_WSA_Strings[] =
{
    L"",
    L"WLX_SAS_ACTION_LOGON",
    L"WLX_SAS_ACTION_NONE",
    L"WLX_SAS_ACTION_LOCK_WKSTA",
    L"WLX_SAS_ACTION_LOGOFF",
    L"WLX_SAS_ACTION_SHUTDOWN",
    L"WLX_SAS_ACTION_PWD_CHANGED",
    L"WLX_SAS_ACTION_TASKLIST",
    L"WLX_SAS_ACTION_UNLOCK_WKSTA",
    L"WLX_SAS_ACTION_FORCE_LOGOFF",
    L"WLX_SAS_ACTION_SHUTDOWN_POWER_OFF",
    L"WLX_SAS_ACTION_SHUTDOWN_REBOOT",
    L"WLX_SAS_ACTION_SHUTDOWN_SLEEP",
    L"WLX_SAS_ACTION_SHUTDOWN_SLEEP2",
    L"WLX_SAS_ACTION_SHUTDOWN_HIBERNATE",
    L"WLX_SAS_ACTION_RECONNECTED",
    L"WLX_SAS_ACTION_DELAYED_FORCE_LOGOFF",
    L"WLX_SAS_ACTION_SWITCH_CONSOLE",
};

// The WlxNegotiate function must be implemented by a replacement GINA DLL.
// This is the first call made by Winlogon to the GINA DLL.
// WlxNegotiate allows the GINA to verify that it supports the installed version of Winlogon.
BOOL
WINAPI
WlxNegotiate(
    DWORD                   dwWinlogonVersion,
    PDWORD                  pdwDllVersion
    )
{
    Utils::Output(Utils::StringFormat(L"WlxNegotiate dwWinlogonVersion: %u", dwWinlogonVersion));
    return Gina::Negotiate(dwWinlogonVersion, pdwDllVersion);
}

// Winlogon calls this function once for each window station present on the computer.
// Currently, the operating system supports one window station per workstation.
BOOL
WINAPI
WlxInitialize(
    LPWSTR                  lpWinsta,
    HANDLE                  hWlx,
    PVOID                   pvReserved,
    PVOID                   pWinlogonFunctions,
    PVOID *                 pWlxContext
    )
{
    Utils::Output(Utils::StringFormat(L"WlxInitialize lpWinsta: %s, hWlx: %p, pWinlogonFunctions: %p", lpWinsta, hWlx, pWinlogonFunctions));
    return Gina::Initialize(hWlx, pWinlogonFunctions, reinterpret_cast<Gina * *>(pWlxContext));
}

// Winlogon calls this function when no user is logged on.
VOID
WINAPI
WlxDisplaySASNotice(
    PVOID                   pWlxContext
    )
{
    Utils::Output(Utils::StringFormat(L"WlxDisplaySASNotice pWlxContext: %p", pWlxContext));
    static_cast<Gina *>(pWlxContext)->DisplaySASNotice();
}

// Winlogon calls this function when it receives a secure attention sequence (SAS) event while no user is logged on.
int
WINAPI
WlxLoggedOutSAS(
    PVOID                   pWlxContext,
    DWORD                   dwSasType,
    PLUID                   pAuthenticationId,
    PSID                    pLogonSid,
    PDWORD                  pdwOptions,
    PHANDLE                 phToken,
    PWLX_MPR_NOTIFY_INFO    pNprNotifyInfo,
    PVOID *                 pProfile
    )
{
    Utils::Output(Utils::StringFormat(L"WlxLoggedOutSAS pWlxContext: %p, dwSasType: %s", pWlxContext, s_WST_Strings[dwSasType]));
    int res = static_cast<Gina *>(pWlxContext)->LoggedOutSAS(dwSasType, pAuthenticationId, pLogonSid, pdwOptions, phToken, pNprNotifyInfo, pProfile);
    Utils::Output(Utils::StringFormat(L"WlxLoggedOutSAS pWlxContext: %p, res: %s", pWlxContext, s_WSA_Strings[res]));
    return res;
}

// Activates the user shell program.
BOOL
WINAPI
WlxActivateUserShell(
    PVOID                   pWlxContext,
    PWSTR                   pszDesktopName,
    PWSTR                   pszMprLogonScript,
    PVOID                   pEnvironment
    )
{
    Utils::Output(Utils::StringFormat(L"WlxActivateUserShell pWlxContext: %p, pszDesktopName: %s", pWlxContext, pszDesktopName));
    return static_cast<Gina *>(pWlxContext)->ActivateUserShell(pszDesktopName, pszMprLogonScript, pEnvironment);
}

// Winlogon calls this function when it receives a secure attention sequence (SAS) event while the user is logged on and the workstation is not locked.
int
WINAPI
WlxLoggedOnSAS(
    PVOID                   pWlxContext,
    DWORD                   dwSasType,
    PVOID                   pReserved
    )
{
    Utils::Output(Utils::StringFormat(L"WlxLoggedOnSAS pWlxContext: %p, dwSasType: %s", pWlxContext, s_WST_Strings[dwSasType]));
    int res = static_cast<Gina *>(pWlxContext)->LoggedOnSAS(dwSasType);
    Utils::Output(Utils::StringFormat(L"WlxLoggedOnSAS pWlxContext: %p, res: %s", pWlxContext, s_WSA_Strings[res]));
    return res;
}

// Allows the GINA to display information about the lock, such as who locked the workstation and when it was locked.
VOID
WINAPI
WlxDisplayLockedNotice(
    PVOID                   pWlxContext
    )
{
    Utils::Output(Utils::StringFormat(L"WlxDisplayLockedNotice pWlxContext: %p", pWlxContext));
    static_cast<Gina *>(pWlxContext)->DisplayLockedNotice();
}

// Winlogon calls this function when it receives a secure attention sequence (SAS) and the workstation is locked.
int
WINAPI
WlxWkstaLockedSAS(
    PVOID                   pWlxContext,
    DWORD                   dwSasType
    )
{
    Utils::Output(Utils::StringFormat(L"WlxWkstaLockedSAS pWlxContext: %p, dwSasType: %s", pWlxContext, s_WST_Strings[dwSasType]));
    int res = static_cast<Gina *>(pWlxContext)->WkstaLockedSAS(dwSasType);
    Utils::Output(Utils::StringFormat(L"WlxWkstaLockedSAS pWlxContext: %p, res: %s", pWlxContext, s_WSA_Strings[res]));
    return res;
}

// Winlogon calls this function before attempting to lock the workstation.
BOOL
WINAPI
WlxIsLockOk(
    PVOID                   pWlxContext
    )
{
    Utils::Output(Utils::StringFormat(L"WlxIsLockOk pWlxContext: %p", pWlxContext));
    return static_cast<Gina *>(pWlxContext)->IsLockOk();
}

// Winlogon calls this function when the user initiates a logoff operation.
BOOL
WINAPI
WlxIsLogoffOk(
    PVOID                   pWlxContext
    )
{
    Utils::Output(Utils::StringFormat(L"WlxIsLogoffOk pWlxContext: %p", pWlxContext));
    return static_cast<Gina *>(pWlxContext)->IsLogoffOk();
}

// Winlogon calls this function to notify the GINA of a logoff operation on this workstation, allowing the GINA to perform any logoff operations that may be required.
VOID
WINAPI
WlxLogoff(
    PVOID                   pWlxContext
    )
{
    Utils::Output(Utils::StringFormat(L"WlxLogoff pWlxContext: %p", pWlxContext));
    static_cast<Gina *>(pWlxContext)->Logoff();
}

// Winlogon calls this function just before shutting down, allowing the GINA to perform any shutdown tasks, such as ejecting a smart card from a reader.
VOID
WINAPI
WlxShutdown(
    PVOID                   pWlxContext,
    DWORD                   ShutdownType
    )
{
    Utils::Output(Utils::StringFormat(L"WlxShutdown pWlxContext: %p, ShutdownType: %u", pWlxContext, ShutdownType));
    static_cast<Gina *>(pWlxContext)->Shutdown(ShutdownType);
}


//
// NEW for 1.1
//

// Winlogon calls this function immediately before a screen saver is activated, allowing the GINA to interact with the screen saver program.
BOOL
WINAPI
WlxScreenSaverNotify(
    PVOID                   pWlxContext,
    BOOL *                  pSecure)
{
    Utils::Output(Utils::StringFormat(L"WlxScreenSaverNotify pWlxContext: %p", pWlxContext));
    return static_cast<Gina *>(pWlxContext)->ScreenSaverNotify(pSecure);
}

// Winlogon calls this function when the system needs an application to be started in the context of the user.
BOOL
WINAPI
WlxStartApplication(
    PVOID                   pWlxContext,
    PWSTR                   pszDesktopName,
    PVOID                   pEnvironment,
    PWSTR                   pszCmdLine
    )
{
    Utils::Output(Utils::StringFormat(L"WlxStartApplication pWlxContext: %p, pszDesktopName: %s", pWlxContext, pszDesktopName));
    return static_cast<Gina *>(pWlxContext)->StartApplication(pszDesktopName, pEnvironment, pszCmdLine);
}

//
// New for 1.3
//

// Winlogon calls this function to collect valid authentication and identification information.
BOOL
WINAPI
WlxNetworkProviderLoad(
    PVOID                   pWlxContext,
    PWLX_MPR_NOTIFY_INFO    pNprNotifyInfo
    )
{
    Utils::Output(Utils::StringFormat(L"WlxNetworkProviderLoad pWlxContext: %p, pNprNotifyInfo: %p", pWlxContext, pNprNotifyInfo));
    return static_cast<Gina *>(pWlxContext)->NetworkProviderLoad(pNprNotifyInfo);
}

// Winlogon calls this function when the GINA DLL should display a message.
BOOL
WINAPI
WlxDisplayStatusMessage(
    PVOID                   pWlxContext,
    HDESK                   hDesktop,
    DWORD                   dwOptions,
    PWSTR                   pTitle,
    PWSTR                   pMessage
    )
{
    Utils::Output(Utils::StringFormat(L"WlxDisplayStatusMessage pWlxContext: %p, pTitle: %s, pMessage: %s", pWlxContext, pTitle, pMessage));
    return static_cast<Gina *>(pWlxContext)->DisplayStatusMessage(hDesktop, dwOptions, pTitle, pMessage);
}

// Winlogon calls this function to get the status message being displayed by the GINA DLL.
BOOL
WINAPI
WlxGetStatusMessage(
    PVOID                   pWlxContext,
    DWORD *                 pdwOptions,
    PWSTR                   pMessage,
    DWORD                   dwBufferSize
    )
{
    Utils::Output(Utils::StringFormat(L"WlxGetStatusMessage pWlxContext: %p, pMessage: %s", pWlxContext, pMessage));
    return static_cast<Gina *>(pWlxContext)->GetStatusMessage(pdwOptions, pMessage, dwBufferSize);
}

// Winlogon calls this function to tell the GINA DLL to stop displaying the status message.
BOOL
WINAPI
WlxRemoveStatusMessage(
    PVOID                   pWlxContext
    )
{
    Utils::Output(Utils::StringFormat(L"WlxRemoveStatusMessage pWlxContext: %p", pWlxContext));
    return static_cast<Gina *>(pWlxContext)->RemoveStatusMessage();
}

//
// New for 1.4
//

// Winlogon calls this function to read the currently logged on user's credentials to transparently transfer them to a target session.
BOOL
WINAPI
WlxGetConsoleSwitchCredentials(
    PVOID                   pWlxContext,
    PVOID                   pCredInfo
    )
{
    Utils::Output(Utils::StringFormat(L"WlxGetConsoleSwitchCredentials pWlxContext: %p, pCredInfo: %p", pWlxContext, pCredInfo));
    return static_cast<Gina *>(pWlxContext)->GetConsoleSwitchCredentials(static_cast<PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0>(pCredInfo));
}

// Winlogon calls this function when a Terminal Services network session is reconnected.
VOID
WINAPI
WlxReconnectNotify(
    PVOID                   pWlxContext
    )
{
    Utils::Output(Utils::StringFormat(L"WlxReconnectNotify pWlxContext: %p", pWlxContext));
    static_cast<Gina *>(pWlxContext)->ReconnectNotify();
}

// Winlogon calls this function when a Terminal Services network session is disconnected.
VOID
WINAPI
WlxDisconnectNotify(
    PVOID                   pWlxContext
    )
{
    Utils::Output(Utils::StringFormat(L"WlxDisconnectNotify pWlxContext: %p", pWlxContext));
    static_cast<Gina *>(pWlxContext)->DisconnectNotify();
}

// 调试
VOID
WINAPI
DebugGINA()
{
    Utils::Output(L"DebugGINA");
    DWORD fakeDllVersion = 0;
    if (WlxNegotiate(WLX_CURRENT_VERSION, &fakeDllVersion)) {
        void * pWlxContext;
        if (WlxInitialize(NULL, NULL, NULL, NULL, &pWlxContext)) {
            //打开desktop
            HDESK hdesk = ::OpenDesktop(L"default", 0, FALSE, DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW | DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL | DESKTOP_JOURNALPLAYBACK | DESKTOP_JOURNALRECORD | DESKTOP_READOBJECTS | DESKTOP_SWITCHDESKTOP | DESKTOP_WRITEOBJECTS);
            if (hdesk) {
                ::SetThreadDesktop(hdesk);
            }
            Gina * gina = static_cast<Gina *>(pWlxContext);
            gina->DisplayStatusMessage(hdesk, 0, L"", L"Windows 正在启动...");
            ::Sleep(2000);
            gina->DisplayStatusMessage(hdesk, 0, L"", L"正在应用计算机设置...");
            ::Sleep(2000);
            gina->RemoveStatusMessage();
            gina->RemoveStatusMessage();
            gina->DisplaySASNotice();
            ::Sleep(2000);
            gina->RemoveStatusMessage();
            LUID authenticationId;
            SID logonSid;
            DWORD dwOptions;
            HANDLE hToken;
            WLX_MPR_NOTIFY_INFO nprNotifyInfo;
            WLX_PROFILE_V1_0 * pWinlogonProfile = NULL;
            int res = gina->LoggedOutSAS(WLX_SAS_TYPE_CTRL_ALT_DEL, &authenticationId, &logonSid, &dwOptions, &hToken, &nprNotifyInfo, (PVOID *)&pWinlogonProfile);
            if (res == WLX_SAS_ACTION_LOGON) {
                res = gina->LoggedOnSAS(WLX_SAS_TYPE_CTRL_ALT_DEL);
                if (res == WLX_SAS_ACTION_LOCK_WKSTA) {
                    gina->WkstaLockedSAS(WLX_SAS_TYPE_CTRL_ALT_DEL);
                } else if (res == WLX_SAS_ACTION_LOGOFF) {
                    gina->Logoff();
                    gina->DisplaySASNotice();
                    ::Sleep(2000);
                } else if (res == WLX_SAS_ACTION_SHUTDOWN) {
                    gina->Shutdown(0);
                    gina->DisplaySASNotice();
                    ::Sleep(2000);
                }
            }
            if (nprNotifyInfo.pszDomain) {
                ::LocalFree(nprNotifyInfo.pszDomain);
            }
            if (nprNotifyInfo.pszPassword) {
                ::LocalFree(nprNotifyInfo.pszPassword);
            }
            if (nprNotifyInfo.pszUserName) {
                ::LocalFree(nprNotifyInfo.pszUserName);
            }
            if (nprNotifyInfo.pszOldPassword) {
                ::LocalFree(nprNotifyInfo.pszOldPassword);
            }
            if (pWinlogonProfile) {
                ::LocalFree(pWinlogonProfile);
            }
        }
    }
}
