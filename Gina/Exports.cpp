#include <Windows.h>
#include <WinWlx.h>
#include "Utils.h"

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

}

// Winlogon calls this function when no user is logged on.
VOID
WINAPI
WlxDisplaySASNotice(
    PVOID                   pWlxContext
    )
{

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

}

// Allows the GINA to display information about the lock, such as who locked the workstation and when it was locked.
VOID
WINAPI
WlxDisplayLockedNotice(
    PVOID                   pWlxContext
    )
{

}

// Winlogon calls this function when it receives a secure attention sequence (SAS) and the workstation is locked.
int
WINAPI
WlxWkstaLockedSAS(
    PVOID                   pWlxContext,
    DWORD                   dwSasType
    )
{

}

// Winlogon calls this function before attempting to lock the workstation.
BOOL
WINAPI
WlxIsLockOk(
    PVOID                   pWlxContext
    )
{

}

// Winlogon calls this function when the user initiates a logoff operation.
BOOL
WINAPI
WlxIsLogoffOk(
    PVOID                   pWlxContext
    )
{

}

// Winlogon calls this function to notify the GINA of a logoff operation on this workstation, allowing the GINA to perform any logoff operations that may be required.
VOID
WINAPI
WlxLogoff(
    PVOID                   pWlxContext
    )
{

}

// Winlogon calls this function just before shutting down, allowing the GINA to perform any shutdown tasks, such as ejecting a smart card from a reader.
VOID
WINAPI
WlxShutdown(
    PVOID                   pWlxContext,
    DWORD                   ShutdownType
    )
{

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

}

// Winlogon calls this function to tell the GINA DLL to stop displaying the status message.
BOOL
WINAPI
WlxRemoveStatusMessage(
    PVOID                   pWlxContext
    )
{

}

//
// New for 1.4
//

// Winlogon calls this function to read the currently logged on user's credentials to transparently transfer them to a target session.
BOOL
WINAPI
WlxGetConsoleSwitchCredentials (
    PVOID                   pWlxContext,
    PVOID                   pCredInfo
    )
{

}

// Winlogon calls this function when a Terminal Services network session is reconnected.
VOID
WINAPI
WlxReconnectNotify (
    PVOID                   pWlxContext
    )
{

}

// Winlogon calls this function when a Terminal Services network session is disconnected.
VOID
WINAPI
WlxDisconnectNotify (
    PVOID                   pWlxContext
    )
{

}
