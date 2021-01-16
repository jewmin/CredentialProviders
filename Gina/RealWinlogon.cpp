#include "RealWinlogon.h"

RealWinlogon::RealWinlogon(HANDLE hWlx, PVOID pWinlogonFunctions)
    : hWlx_(hWlx)
    , pWinlogonFunctions_(pWinlogonFunctions) {
}

VOID RealWinlogon::WlxUseCtrlAltDel() {
    static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxUseCtrlAltDel(hWlx_);
}

VOID RealWinlogon::WlxSetContextPointer(PVOID pWlxContext) {
    static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxSetContextPointer(hWlx_, pWlxContext);
}

VOID RealWinlogon::WlxSasNotify(DWORD dwSasType) {
    static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxSasNotify(hWlx_, dwSasType);
}

BOOL RealWinlogon::WlxSetTimeout(DWORD Timeout) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxSetTimeout(hWlx_, Timeout);
}

int RealWinlogon::WlxAssignShellProtection(HANDLE hToken, HANDLE hProcess, HANDLE hThread) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxAssignShellProtection(hWlx_, hToken, hProcess, hThread);
}

int RealWinlogon::WlxMessageBox(HWND hwndOwner, LPWSTR lpszText, LPWSTR lpszTitle, UINT fuStyle) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxMessageBox(hWlx_, hwndOwner, lpszText, lpszTitle, fuStyle);
}

int RealWinlogon::WlxDialogBox(HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxDialogBox(hWlx_, hInst, lpszTemplate, hwndOwner, dlgprc);
}

int RealWinlogon::WlxDialogBoxParam(HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxDialogBoxParam(hWlx_, hInst, lpszTemplate, hwndOwner, dlgprc, dwInitParam);
}

int RealWinlogon::WlxDialogBoxIndirect(HANDLE hInst, LPCDLGTEMPLATE hDialogTemplate, HWND hwndOwner, DLGPROC dlgprc) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxDialogBoxIndirect(hWlx_, hInst, hDialogTemplate, hwndOwner, dlgprc);
}

int RealWinlogon::WlxDialogBoxIndirectParam(HANDLE hInst, LPCDLGTEMPLATE hDialogTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxDialogBoxIndirectParam(hWlx_, hInst, hDialogTemplate, hwndOwner, dlgprc, dwInitParam);
}

int RealWinlogon::WlxSwitchDesktopToUser() {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxSwitchDesktopToUser(hWlx_);
}

int RealWinlogon::WlxSwitchDesktopToWinlogon() {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxSwitchDesktopToWinlogon(hWlx_);
}

int RealWinlogon::WlxChangePasswordNotify(PWLX_MPR_NOTIFY_INFO pMprInfo, DWORD dwChangeInfo) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxChangePasswordNotify(hWlx_, pMprInfo, dwChangeInfo);
}

BOOL RealWinlogon::WlxGetSourceDesktop(PWLX_DESKTOP * ppDesktop) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxGetSourceDesktop(hWlx_, ppDesktop);
}

BOOL RealWinlogon::WlxSetReturnDesktop(PWLX_DESKTOP pDesktop) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxSetReturnDesktop(hWlx_, pDesktop);
}

BOOL RealWinlogon::WlxCreateUserDesktop(HANDLE hToken, DWORD Flags, PWSTR pszDesktopName, PWLX_DESKTOP * ppDesktop) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxCreateUserDesktop(hWlx_, hToken, Flags, pszDesktopName, ppDesktop);
}

int RealWinlogon::WlxChangePasswordNotifyEx(PWLX_MPR_NOTIFY_INFO pMprInfo, DWORD dwChangeInfo, PWSTR ProviderName, PVOID Reserved) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxChangePasswordNotifyEx(hWlx_, pMprInfo, dwChangeInfo, ProviderName, Reserved);
}

BOOL RealWinlogon::WlxCloseUserDesktop (PWLX_DESKTOP pDesktop, HANDLE hToken) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxCloseUserDesktop(hWlx_, pDesktop, hToken);
}

BOOL RealWinlogon::WlxSetOption(DWORD Option, ULONG_PTR Value, ULONG_PTR * OldValue) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxSetOption(hWlx_, Option, Value, OldValue);
}

BOOL RealWinlogon::WlxGetOption(DWORD Option, ULONG_PTR * Value) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxGetOption(hWlx_, Option, Value);
}

VOID RealWinlogon::WlxWin31Migrate() {
    static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxWin31Migrate(hWlx_);
}

BOOL RealWinlogon::WlxQueryClientCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V1_0 pCred) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxQueryClientCredentials(pCred);
}

BOOL RealWinlogon::WlxQueryInetConnectorCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V1_0 pCred) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxQueryInetConnectorCredentials(pCred);
}

BOOL RealWinlogon::WlxDisconnect() {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxDisconnect();
}

DWORD RealWinlogon::WlxQueryTerminalServicesData(PWLX_TERMINAL_SERVICES_DATA pTSData, WCHAR * UserName, WCHAR * Domain) {
    return static_cast<PWLX_DISPATCH_VERSION_1_3>(pWinlogonFunctions_)->WlxQueryTerminalServicesData(hWlx_, pTSData, UserName, Domain);
}

DWORD RealWinlogon::WlxQueryConsoleSwitchCredentials(PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 pCred) {
    if (Version() <= WLX_VERSION_1_3) {
        return 0;
    }
    return static_cast<PWLX_DISPATCH_VERSION_1_4>(pWinlogonFunctions_)->WlxQueryConsoleSwitchCredentials(pCred);
}

BOOL RealWinlogon::WlxQueryTsLogonCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V2_0 pCred) {
    if (Version() <= WLX_VERSION_1_3) {
        return FALSE;
    }
    return static_cast<PWLX_DISPATCH_VERSION_1_4>(pWinlogonFunctions_)->WlxQueryTsLogonCredentials(pCred);
}
