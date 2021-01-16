// Exports.cpp
//
// All functions exported from our custom GINA can be found here,
// although all most of them do is delegate to the Gina class.
//

#include "stdafx.h"

#include "Gina.h"

BOOL
WINAPI
WlxNegotiate(
    DWORD                   dwWinlogonVersion,
    PDWORD                  pdwDllVersion
    )
{
    LDB1(L"-->WlxNegotiate(wlv = 0x%08X", dwWinlogonVersion);
    BOOL result = Gina::Negotiate(dwWinlogonVersion, pdwDllVersion);
    LDB(L"<--WlxNegotiate");
    return result;
}

BOOL
WINAPI
WlxInitialize(
    LPWSTR                  /* lpWinsta */,
    HANDLE                  hWlx,
    PVOID                   /* pvReserved */,
    PVOID                   pWinlogonFunctions,
    PVOID *                 pWlxContext
    )
{
    LDB(L"-->WlxInitialize");
    BOOL result = Gina::Initialize(hWlx, pWinlogonFunctions, (Gina**)pWlxContext);
    LDB(L"<--WlxInitialize");
    return result;
}

VOID
WINAPI
WlxDisplaySASNotice(
    PVOID                   pWlxContext
    )
{
    LDB(L"-->WlxDisplaySASNotice");
    ((Gina*)pWlxContext)->DisplaySASNotice();
    LDB(L"<--WlxDisplaySASNotice");
}


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
    LDB(L"-->WlxLoggedOutSAS");
    int result = ((Gina*)pWlxContext)->LoggedOutSAS(dwSasType, pAuthenticationId, pLogonSid, pdwOptions, phToken, pNprNotifyInfo, pProfile);
    const wchar_t* msg = L"";
    switch (result) {
        case WLX_SAS_ACTION_NONE:  msg = L"WLX_SAS_ACTION_NONE"; break;
        case WLX_SAS_ACTION_LOGON: msg = L"WLX_SAS_ACTION_LOGON"; break;
    }
    LDB1(L"<--WlxLoggedOutSAS (%s)", msg);
    return result;
}

BOOL
WINAPI
WlxActivateUserShell(
    PVOID                   pWlxContext,
    PWSTR                   pszDesktopName,
    PWSTR                   pszMprLogonScript,
    PVOID                   pEnvironment
    )
{
    LDB(L"-->WlxActivateUserShell");
    BOOL result = ((Gina*)pWlxContext)->ActivateUserShell(pszDesktopName, pszMprLogonScript, pEnvironment);
    LDB(L"<--WlxActivateUserShell");
    return result;
}

int
WINAPI
WlxLoggedOnSAS(
    PVOID                   pWlxContext,
    DWORD                   dwSasType,
    PVOID                   /* pReserved */
    )
{
    LDB(L"-->WlxLoggedOnSAS");
    int result = ((Gina*)pWlxContext)->LoggedOnSAS(dwSasType);
    LDB(L"<--WlxLoggedOnSAS");
    return result;
}

VOID
WINAPI
WlxDisplayLockedNotice(
    PVOID                   pWlxContext
    )
{
    LDB(L"-->WlxDisplayLockedNotice");
    ((Gina*)pWlxContext)->DisplayLockedNotice();
    LDB(L"<--WlxDisplayLockedNotice");
}

int
WINAPI
WlxWkstaLockedSAS(
    PVOID                   pWlxContext,
    DWORD                   dwSasType
    )
{
    LDB(L"-->WlxWkstaLockedSAS");
    int result = ((Gina*)pWlxContext)->WkstaLockedSAS(dwSasType);
    LDB(L"<--WlxWkstaLockedSAS");
    return result;
}

BOOL
WINAPI
WlxIsLockOk(
    PVOID                   pWlxContext
    )
{
    LDB(L"-->WlxIsLockOk");
    BOOL result = ((Gina*)pWlxContext)->IsLockOk();
    LDB(L"<--WlxIsLockOk");
    return result;
}

BOOL
WINAPI
WlxIsLogoffOk(
    PVOID                   pWlxContext
    )
{
    LDB(L"-->WlxIsLogoffOk");
    BOOL result = ((Gina*)pWlxContext)->IsLogoffOk();
    LDB(L"<--WlxIsLogoffOk");
    return result;
}

VOID
WINAPI
WlxLogoff(
    PVOID                   pWlxContext
    )
{
    LDB(L"-->WlxLogoff");
    ((Gina*)pWlxContext)->Logoff();
    LDB(L"<--WlxLogoff");
}


VOID
WINAPI
WlxShutdown(
    PVOID                   pWlxContext,
    DWORD                   ShutdownType
    )
{
    LDB(L"-->WlxShutdown");
    ((Gina*)pWlxContext)->Shutdown(ShutdownType);
    LDB(L"<--WlxShutdown");
}

//
// New for 1.3
//

BOOL
WINAPI
WlxNetworkProviderLoad(
    PVOID                   pWlxContext,
    PWLX_MPR_NOTIFY_INFO    pNprNotifyInfo
    )
{
    LDB(L"-->WlxNetworkProviderLoad");
    BOOL result = ((Gina*)pWlxContext)->NetworkProviderLoad(pNprNotifyInfo);
    LDB(L"<--WlxNetworkProviderLoad");
    return result;
}

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
    LDB(L"-->WlxDisplayStatusMessage");
    BOOL result = ((Gina*)pWlxContext)->DisplayStatusMessage(hDesktop, dwOptions, pTitle, pMessage);
    LDB(L"<--WlxDisplayStatusMessage");
    return result;
}

BOOL
WINAPI
WlxGetStatusMessage(
    PVOID                   pWlxContext,
    DWORD *                 pdwOptions,
    PWSTR                   pMessage,
    DWORD                   dwBufferSize
    )
{
    LDB(L"-->WlxGetStatusMessage");
    BOOL result = ((Gina*)pWlxContext)->GetStatusMessage(pdwOptions, pMessage, dwBufferSize);
    LDB(L"<--WlxGetStatusMessage");
    return result;
}

BOOL
WINAPI
WlxRemoveStatusMessage(
    PVOID                   pWlxContext
    )
{
    LDB(L"-->WlxRemoveStatusMessage");
    BOOL result = ((Gina*)pWlxContext)->RemoveStatusMessage();
    LDB(L"<--WlxRemoveStatusMessage");
    return result;
}

//
// New for 1.4
//
BOOL
WINAPI
WlxGetConsoleSwitchCredentials (
    PVOID                   pWlxContext,
    PVOID                   pCredInfo
    )
{
    LDB(L"-->WlxGetConsoleSwitchCredentials ");
    BOOL result = ((Gina*)pWlxContext)->GetConsoleSwitchCredentials((WLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0*)pCredInfo);
    LDB(L"<--WlxGetConsoleSwitchCredentials ");
    return result;
}

VOID
WINAPI
WlxReconnectNotify (
    PVOID                   pWlxContext
    )
{
    LDB(L"-->WlxReconnectNotify ");
    ((Gina*)pWlxContext)->ReconnectNotify();
    LDB(L"<--WlxReconnectNotify ");
}

VOID
WINAPI
WlxDisconnectNotify (
    PVOID                   pWlxContext
    )
{
    LDB(L"-->WlxDisconnectNotify ");
    ((Gina*)pWlxContext)->DisconnectNotify();
    LDB(L"<--WlxDisconnectNotify ");
}

// used for interactive debugging outside of WinLogon
void WINAPI DebugGINA() {
#ifdef _DEBUG
    DWORD d;
    BOOL result = WlxNegotiate(WLX_VERSION_1_3, &d);
    if (!result) return;
    void* pWlxContext;
    result = WlxInitialize(0, 0, 0, 0, &pWlxContext);
    if (!result) return;
    WlxDisplaySASNotice(pWlxContext);
#endif
}
