// WinLogonInterface.h
//
// This class provides type safe access to WinLogon via its function dispatch table.
//

#pragma once

// this is virtualized so we can mock it for interactive debugging
class IWinLogon {
public:
    virtual int wlxMessageBox(HWND hwndOwner, LPWSTR text, LPWSTR title, UINT style) = 0;
    virtual int wlxSwitchDesktopToWinlogon() = 0;
    virtual int wlxSwitchDesktopToUser() = 0;
    virtual BOOL wlxGetOption(DWORD option, ULONG_PTR* pValue) = 0;
    virtual BOOL wlxSetOption(DWORD option, ULONG_PTR newValue, ULONG_PTR* pOldValue) = 0;
    virtual void wlxSasNotify(DWORD dwSasType) = 0;
    virtual int wlxDialogBoxParam(HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgproc, LPARAM dwInitParam) = 0;

    virtual bool Supports1_4() = 0;

    virtual BOOL wlxQueryConsoleSwitchCredentials(PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 pCred) = 0;
    virtual BOOL wlxQueryTsLogonCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V2_0 pCred) = 0;
};

// here's the real thing that dispatches to WinLogon
class WinLogonInterface : public IWinLogon {
public:
    WinLogonInterface(HANDLE winLogonHandle, DWORD winLogonVersion, void* pDispatchTable)
        : _winLogonHandle(winLogonHandle),
          _winLogonVersion(winLogonVersion),
          _pDispatchTable(pDispatchTable) {
    }

    int wlxMessageBox(HWND hwndOwner, LPWSTR text, LPWSTR title, UINT style) {
        return ((WLX_DISPATCH_VERSION_1_3*)_pDispatchTable)->WlxMessageBox(_winLogonHandle, hwndOwner, text, title, style);
    }

    int wlxSwitchDesktopToWinlogon() {
        return ((WLX_DISPATCH_VERSION_1_3*)_pDispatchTable)->WlxSwitchDesktopToWinlogon(_winLogonHandle);
    }

    int wlxSwitchDesktopToUser() {
        return ((WLX_DISPATCH_VERSION_1_3*)_pDispatchTable)->WlxSwitchDesktopToUser(_winLogonHandle);
    }

    BOOL wlxGetOption(DWORD option, ULONG_PTR* pValue) {
        return ((WLX_DISPATCH_VERSION_1_3*)_pDispatchTable)->WlxGetOption(_winLogonHandle, option, pValue);
    }

    BOOL wlxSetOption(DWORD option, ULONG_PTR newValue, ULONG_PTR* pOldValue) {
        return ((WLX_DISPATCH_VERSION_1_3*)_pDispatchTable)->WlxSetOption(_winLogonHandle, option, newValue, pOldValue);
    }

    void wlxSasNotify(DWORD dwSasType) {
        ((WLX_DISPATCH_VERSION_1_3*)_pDispatchTable)->WlxSasNotify(_winLogonHandle, dwSasType);
    }

    int wlxDialogBoxParam(HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgproc, LPARAM dwInitParam) {
        return ((WLX_DISPATCH_VERSION_1_3*)_pDispatchTable)->WlxDialogBoxParam(_winLogonHandle, hInst, lpszTemplate, hwndOwner, dlgproc, dwInitParam);
    }

    bool Supports1_4() { return _winLogonVersion >= WLX_VERSION_1_4; }

    //////////////////////////////
    // 1.4 only from here on down!
    BOOL wlxQueryConsoleSwitchCredentials(PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 pCred) {
        if (!Supports1_4()) return FALSE;
        return ((WLX_DISPATCH_VERSION_1_4*)_pDispatchTable)->WlxQueryConsoleSwitchCredentials(pCred);
    }

    BOOL wlxQueryTsLogonCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V2_0 pCred) {
        if (!Supports1_4()) return FALSE;
        return ((WLX_DISPATCH_VERSION_1_4*)_pDispatchTable)->WlxQueryTsLogonCredentials(pCred);
    }

private:
    HANDLE _winLogonHandle;
    DWORD  _winLogonVersion;
    void*  _pDispatchTable;
};

#ifdef _DEBUG

class MockWinLogonInterface : public IWinLogon {
public:
    MockWinLogonInterface() {}

    int wlxMessageBox(HWND hwndOwner, LPWSTR text, LPWSTR title, UINT style) {
        return MessageBox(hwndOwner, text, title, style);
    }

    int wlxSwitchDesktopToWinlogon() {
        return 0;
    }

    int wlxSwitchDesktopToUser() {
        return 0;
    }

    BOOL wlxGetOption(DWORD option, ULONG_PTR* pValue) {
        return FALSE;
    }

    BOOL wlxSetOption(DWORD option, ULONG_PTR newValue, ULONG_PTR* pOldValue) {
        return TRUE;
    }

    void wlxSasNotify(DWORD /* dwSasType */) {
        
    }

    int wlxDialogBoxParam(HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgproc, LPARAM dwInitParam) {
        return (int)DialogBoxParam((HINSTANCE)hInst, lpszTemplate, hwndOwner, dlgproc, dwInitParam);
    }

    bool Supports1_4() { return TRUE; }

    BOOL wlxQueryConsoleSwitchCredentials(PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 pCred) {
        return FALSE;
    }

    BOOL wlxQueryTsLogonCredentials(PWLX_CLIENT_CREDENTIALS_INFO_V2_0 pCred) {
        return FALSE;
    }
};

#endif