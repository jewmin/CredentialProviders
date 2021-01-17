#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <WinWlx.h>

class IWinlogon {
public:
    // WLX_DISPATCH_VERSION_1_1
    virtual VOID WlxUseCtrlAltDel() = 0;
    virtual VOID WlxSetContextPointer(PVOID pWlxContext) = 0;
    virtual VOID WlxSasNotify(DWORD dwSasType) = 0;
    virtual BOOL WlxSetTimeout(DWORD Timeout) = 0;
    virtual int WlxAssignShellProtection(HANDLE hToken, HANDLE hProcess, HANDLE hThread) = 0;
    virtual int WlxMessageBox(HWND hwndOwner, LPWSTR lpszText, LPWSTR lpszTitle, UINT fuStyle) = 0;
    virtual int WlxDialogBox(HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc) = 0;
    virtual int WlxDialogBoxParam(HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam) = 0;
    virtual int WlxDialogBoxIndirect(HANDLE hInst, LPCDLGTEMPLATE hDialogTemplate, HWND hwndOwner, DLGPROC dlgprc) = 0;
    virtual int WlxDialogBoxIndirectParam(HANDLE hInst, LPCDLGTEMPLATE hDialogTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam) = 0;
    virtual int WlxSwitchDesktopToUser() = 0;
    virtual int WlxSwitchDesktopToWinlogon() = 0;
    virtual int WlxChangePasswordNotify(PWLX_MPR_NOTIFY_INFO pMprInfo, DWORD dwChangeInfo) = 0;
    virtual BOOL WlxGetSourceDesktop(PWLX_DESKTOP * ppDesktop) = 0;
    virtual BOOL WlxSetReturnDesktop(PWLX_DESKTOP pDesktop) = 0;
    virtual BOOL WlxCreateUserDesktop(HANDLE hToken, DWORD Flags, PWSTR pszDesktopName, PWLX_DESKTOP * ppDesktop) = 0;
    // WLX_DISPATCH_VERSION_1_2
    virtual int WlxChangePasswordNotifyEx(PWLX_MPR_NOTIFY_INFO pMprInfo, DWORD dwChangeInfo, PWSTR ProviderName, PVOID Reserved) = 0;
    // WLX_DISPATCH_VERSION_1_3
    virtual BOOL WlxCloseUserDesktop (PWLX_DESKTOP pDesktop, HANDLE hToken) = 0;
    virtual BOOL WlxSetOption(DWORD Option, ULONG_PTR Value, ULONG_PTR * OldValue) = 0;
    virtual BOOL WlxGetOption(DWORD Option, ULONG_PTR * Value) = 0;
    virtual VOID WlxWin31Migrate() = 0;
    virtual BOOL WlxQueryClientCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V1_0 pCred) = 0;
    virtual BOOL WlxQueryInetConnectorCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V1_0 pCred) = 0;
    virtual BOOL WlxDisconnect() = 0;
    virtual DWORD WlxQueryTerminalServicesData(PWLX_TERMINAL_SERVICES_DATA pTSData, WCHAR * UserName, WCHAR * Domain) = 0;
    // WLX_DISPATCH_VERSION_1_4
    virtual DWORD WlxQueryConsoleSwitchCredentials(PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 pCred) = 0;
    virtual BOOL WlxQueryTsLogonCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V2_0 pCred) = 0;

public:
    static DWORD Version() { return WinlogonVersion; }
    static VOID Version(DWORD version) { WinlogonVersion = version; }

private:
    static DWORD WinlogonVersion;
};
