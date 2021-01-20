#include "MockWinlogon.h"

MockWinlogon::MockWinlogon() {
}

VOID MockWinlogon::WlxUseCtrlAltDel() {
}

VOID MockWinlogon::WlxSetContextPointer(PVOID pWlxContext) {
}

VOID MockWinlogon::WlxSasNotify(DWORD dwSasType) {
}

BOOL MockWinlogon::WlxSetTimeout(DWORD Timeout) {
    return FALSE;
}

int MockWinlogon::WlxAssignShellProtection(HANDLE hToken, HANDLE hProcess, HANDLE hThread) {
    return 0;
}

int MockWinlogon::WlxMessageBox(HWND hwndOwner, LPWSTR lpszText, LPWSTR lpszTitle, UINT fuStyle) {
    return ::MessageBox(hwndOwner, lpszText, lpszTitle, fuStyle);
}

int MockWinlogon::WlxDialogBox(HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc) {
    return ::DialogBox(static_cast<HINSTANCE>(hInst), lpszTemplate, hwndOwner, dlgprc);
}

int MockWinlogon::WlxDialogBoxParam(HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam) {
    return ::DialogBoxParam(static_cast<HINSTANCE>(hInst), lpszTemplate, hwndOwner, dlgprc, dwInitParam);
}

int MockWinlogon::WlxDialogBoxIndirect(HANDLE hInst, LPCDLGTEMPLATE hDialogTemplate, HWND hwndOwner, DLGPROC dlgprc) {
    return ::DialogBoxIndirect(static_cast<HINSTANCE>(hInst), hDialogTemplate, hwndOwner, dlgprc);
}

int MockWinlogon::WlxDialogBoxIndirectParam(HANDLE hInst, LPCDLGTEMPLATE hDialogTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam) {
    return ::DialogBoxIndirectParam(static_cast<HINSTANCE>(hInst), hDialogTemplate, hwndOwner, dlgprc, dwInitParam);
}

int MockWinlogon::WlxSwitchDesktopToUser() {
    return 0;
}

int MockWinlogon::WlxSwitchDesktopToWinlogon() {
    return 0;
}

int MockWinlogon::WlxChangePasswordNotify(PWLX_MPR_NOTIFY_INFO pMprInfo, DWORD dwChangeInfo) {
    return 0;
}

BOOL MockWinlogon::WlxGetSourceDesktop(PWLX_DESKTOP * ppDesktop) {
    return FALSE;
}

BOOL MockWinlogon::WlxSetReturnDesktop(PWLX_DESKTOP pDesktop) {
    return FALSE;
}

BOOL MockWinlogon::WlxCreateUserDesktop(HANDLE hToken, DWORD Flags, PWSTR pszDesktopName, PWLX_DESKTOP * ppDesktop) {
    return FALSE;
}

int MockWinlogon::WlxChangePasswordNotifyEx(PWLX_MPR_NOTIFY_INFO pMprInfo, DWORD dwChangeInfo, PWSTR ProviderName, PVOID Reserved) {
    return 0;
}

BOOL MockWinlogon::WlxCloseUserDesktop (PWLX_DESKTOP pDesktop, HANDLE hToken) {
    return FALSE;
}

BOOL MockWinlogon::WlxSetOption(DWORD Option, ULONG_PTR Value, ULONG_PTR * OldValue) {
    return FALSE;
}

BOOL MockWinlogon::WlxGetOption(DWORD Option, ULONG_PTR * Value) {
    return FALSE;
}

VOID MockWinlogon::WlxWin31Migrate() {
}

BOOL MockWinlogon::WlxQueryClientCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V1_0 pCred) {
    return FALSE;
}

BOOL MockWinlogon::WlxQueryInetConnectorCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V1_0 pCred) {
    return FALSE;
}

BOOL MockWinlogon::WlxDisconnect() {
    return FALSE;
}

DWORD MockWinlogon::WlxQueryTerminalServicesData(PWLX_TERMINAL_SERVICES_DATA pTSData, WCHAR * UserName, WCHAR * Domain) {
    return 0;
}

DWORD MockWinlogon::WlxQueryConsoleSwitchCredentials(PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 pCred) {
    return 0;
}

BOOL MockWinlogon::WlxQueryTsLogonCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V2_0 pCred) {
    return FALSE;
}
