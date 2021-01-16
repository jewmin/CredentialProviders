#pragma once

#include "IWinlogon.h"

class RealWinlogon : public IWinlogon {
public:
    // WLX_DISPATCH_VERSION_1_1
    virtual VOID WlxUseCtrlAltDel();
    virtual VOID WlxSetContextPointer(PVOID pWlxContext);
    virtual VOID WlxSasNotify(DWORD dwSasType);
    virtual BOOL WlxSetTimeout(DWORD Timeout);
    virtual int WlxAssignShellProtection(HANDLE hToken, HANDLE hProcess, HANDLE hThread);
    virtual int WlxMessageBox(HWND hwndOwner, LPWSTR lpszText, LPWSTR lpszTitle, UINT fuStyle);
    virtual int WlxDialogBox(HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc);
    virtual int WlxDialogBoxParam(HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam);
    virtual int WlxDialogBoxIndirect(HANDLE hInst, LPCDLGTEMPLATE hDialogTemplate, HWND hwndOwner, DLGPROC dlgprc);
    virtual int WlxDialogBoxIndirectParam(HANDLE hInst, LPCDLGTEMPLATE hDialogTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam);
    virtual int WlxSwitchDesktopToUser();
    virtual int WlxSwitchDesktopToWinlogon();
    virtual int WlxChangePasswordNotify(PWLX_MPR_NOTIFY_INFO pMprInfo, DWORD dwChangeInfo);
    virtual BOOL WlxGetSourceDesktop(PWLX_DESKTOP * ppDesktop);
    virtual BOOL WlxSetReturnDesktop(PWLX_DESKTOP pDesktop);
    virtual BOOL WlxCreateUserDesktop(HANDLE hToken, DWORD Flags, PWSTR pszDesktopName, PWLX_DESKTOP * ppDesktop);
    // WLX_DISPATCH_VERSION_1_2
    virtual int WlxChangePasswordNotifyEx(PWLX_MPR_NOTIFY_INFO pMprInfo, DWORD dwChangeInfo, PWSTR ProviderName, PVOID Reserved);
    // WLX_DISPATCH_VERSION_1_3
    virtual BOOL WlxCloseUserDesktop (PWLX_DESKTOP pDesktop, HANDLE hToken);
    virtual BOOL WlxSetOption(DWORD Option, ULONG_PTR Value, ULONG_PTR * OldValue);
    virtual BOOL WlxGetOption(DWORD Option, ULONG_PTR * Value);
    virtual VOID WlxWin31Migrate();
    virtual BOOL WlxQueryClientCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V1_0 pCred);
    virtual BOOL WlxQueryInetConnectorCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V1_0 pCred);
    virtual BOOL WlxDisconnect();
    virtual DWORD WlxQueryTerminalServicesData(PWLX_TERMINAL_SERVICES_DATA pTSData, WCHAR * UserName, WCHAR * Domain);
    // WLX_DISPATCH_VERSION_1_4
    virtual DWORD WlxQueryConsoleSwitchCredentials(PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 pCred);
    virtual BOOL WlxQueryTsLogonCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V2_0 pCred);

public:
    RealWinlogon(HANDLE hWlx, PVOID pWinlogonFunctions);

private:
    HANDLE hWlx_;
    PVOID pWinlogonFunctions_;
};
