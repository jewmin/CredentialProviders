// Gina.cpp
//
// The guts of the Gina implementation are in this class.
//

#include "stdafx.h"
#include "Gina.h"
#include "WinLogonInterface.h"
#include "NoticeDialog.h"
#include "LogonDialog.h"
#include "StatusWindow.h"
#include "SecurityOptionsDialog.h"
#include "SecurityHelper.h"
#include "RegistryHelper.h"

// length of a Windows logon SID of the form S-1-5-5-x-y
#define LOGON_SID_SIZE 20

static DWORD _winLogonVersion;

static wchar_t* _localAllocString(const wchar_t* s) {
    if (!s) return 0;

    const int cb = (1 + lstrlen(s)) * sizeof *s;
    wchar_t* newStr = (wchar_t*)LocalAlloc(LMEM_FIXED, cb);
    if (newStr)
         CopyMemory(newStr, s, cb);
    else LOOM;
    return newStr;
}

static BOOL _isRemoteSession() {
    return 0 != GetSystemMetrics(SM_REMOTESESSION);
}

// STATIC method
BOOL Gina::Negotiate(DWORD dwWinlogonVersion, DWORD* pdwDllVersion) {
    // fail if WinLogon is not at least version 1.3
    if (dwWinlogonVersion < WLX_VERSION_1_3) {
        return FALSE;
    }

    // record which version of WinLogon we're dealing with
    _winLogonVersion = dwWinlogonVersion;

    LDB1(L"WinLogon version is 0x%08X", _winLogonVersion);

    switch (_winLogonVersion) {
        case WLX_VERSION_1_3:
        case WLX_VERSION_1_4:
            // we'll be whatever version WinLogon needs us to be
            // we can support both 1.3 and 1.4
            *pdwDllVersion = _winLogonVersion;
            break;
    }
    return TRUE;
}

// STATIC method
BOOL Gina::Initialize(HANDLE hWlx, PVOID pWinlogonFunctions, Gina** ppNewGina) {

// to interactively debug, be sure _DEBUG is defined, and pass NULL for both hWlx and pWinlogonFunctions
#ifdef _DEBUG
    IWinLogon* pWinLogon = 0;
    if (pWinlogonFunctions)
         pWinLogon = new WinLogonInterface(hWlx, _winLogonVersion, pWinlogonFunctions);
    else pWinLogon = new MockWinLogonInterface();
#else
    IWinLogon* pWinLogon = new WinLogonInterface(hWlx, _winLogonVersion, pWinlogonFunctions);
#endif
    if (!pWinLogon) {
        LOOM;
        return FALSE;
    }

    HANDLE hLsa;
    if (!SecurityHelper::RegisterLogonProcess(LOGON_PROCESS_NAME, &hLsa)) {
        return FALSE;
    }

    *ppNewGina = new Gina(pWinLogon, hLsa);
    if (!*ppNewGina) {
        LOOM;
        return FALSE;
    }
    return TRUE;
}

Gina::Gina(IWinLogon* pWinLogon, HANDLE hLsa)
  : _pWinLogon(pWinLogon), _hLsa(hLsa), _hToken(0), _profilePath(0), _pStatusWindow(0) {

    // tell WinLogon we wish to use Ctrl-Alt-Delete for our SAS
    DWORD oldValue;
    _pWinLogon->wlxSetOption(WLX_OPTION_USE_CTRL_ALT_DEL, TRUE, &oldValue);

    if (_isRemoteSession()) {
        // remote users cannot press Ctrl-Alt-Del, so we simulate it ourselves
        _pWinLogon->wlxSasNotify(WLX_SAS_TYPE_CTRL_ALT_DEL);
    }
}

void Gina::DisplaySASNotice() {
    NoticeDialog dlg(_pWinLogon, IDD_SASNOTICE);
    dlg.Show();
}

int Gina::LoggedOutSAS(DWORD dwSasType, PLUID pAuthenticationId, PSID pLogonSid, PDWORD pdwOptions,
                       PHANDLE phToken, PWLX_MPR_NOTIFY_INFO pNprNotifyInfo, PVOID* ppWinLogonProfile) {

    ZeroMemory(pNprNotifyInfo, sizeof *pNprNotifyInfo);

    *pdwOptions = 0; // we always let WinLogon load the user profile for us

    wchar_t*       profilePath = 0;
    const wchar_t* domain      = 0;
    const wchar_t* userName    = 0;
    const wchar_t* password    = 0;

    LogonDialog dlg(_pWinLogon);

    if (WLX_SAS_TYPE_CTRL_ALT_DEL == dwSasType) {

        // collect credentials from user
        if (IDOK != dlg.Show()) return WLX_SAS_ACTION_NONE;

        // attempt the login
        DWORD win32Error;
        MSV1_0_INTERACTIVE_PROFILE* pProfile = 0;
        if (!SecurityHelper::CallLsaLogonUser(_hLsa,
                                                dlg.domain, dlg.userName, dlg.password,
                                                Interactive,
                                                pAuthenticationId, phToken,
                                                &pProfile, &win32Error)) {

            // NOTE: a full implementation would deal with expired / must change passwords here
            // by reading the statistics in the profile and giving the user a chance to
            // change her password if it's expired or about to expire

            // message the user to let her know why the logon failed
            wchar_t msg[256];
            Log::LookupErrorMessage(msg, sizeof msg / sizeof *msg, win32Error);
            _pWinLogon->wlxMessageBox(0, msg, L"Logon Message", MB_ICONEXCLAMATION);

            return WLX_SAS_ACTION_NONE;
        }
        if (!SecurityHelper::ExtractProfilePath(&profilePath, pProfile)) {
            return WLX_SAS_ACTION_NONE;
        }
        LsaFreeReturnBuffer(pProfile);

        domain   = dlg.domain;
        userName = dlg.userName;
        password = dlg.password;
    }
    else if (WLX_SAS_TYPE_AUTHENTICATED == dwSasType) {
        // use the information in the other TS session to auto-logon this session
        WLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 credInfo;
        ZeroMemory(&credInfo, sizeof credInfo);
        credInfo.dwType = WLX_CONSOLESWITCHCREDENTIAL_TYPE_V1_0;

        LDB(L"Calling WlxQueryConsoleSwitchCredentials");

        if (_pWinLogon->wlxQueryConsoleSwitchCredentials(&credInfo)) {
            LDB(L"WlxQueryConsoleSwitchCredentials succeeded");
            *phToken    = credInfo.UserToken;
            profilePath = credInfo.ProfilePath;
            
            LDB1(L"Profile Path: [%d]", profilePath ? profilePath : L"<null>");

            if (!SecurityHelper::GetLogonSessionId(credInfo.UserToken, pAuthenticationId)) {
                // this should never fail, just a sanity check
                return WLX_SAS_ACTION_NONE;
            }

            // I believe the profile is already loaded in this case;
            // if you don't specify this option, WinLogon will fail and the error log will
            // complain that the profile failed to be loaded
            *pdwOptions = WLX_LOGON_OPT_NO_PROFILE;
        }
        else {
            LCF(L"WlxQueryConsoleSwitchCredentials failed");
            return WLX_SAS_ACTION_NONE;
        }
    }
    else {
        LDB1(L"WARNING: Unrecognized SAS type: %d", dwSasType);
        return WLX_SAS_ACTION_NONE;
    }

    // if we get this far, the login succeeded, but there are a few minor things that could still fail
    int action = WLX_SAS_ACTION_NONE;
    bool success = false;

    // cache the profilePath in case we need to transfer it to another TS session
    LocalFree(_profilePath);
    _profilePath = profilePath;

    // Assume that WinLogon provides a buffer large enough to hold a logon SID,
    // which is of fixed length. It'd be nice if WinLogon would tell us how big
    // its buffer actually was, but it appears this is assumed.
    if (SecurityHelper::GetLogonSid(*phToken, pLogonSid, LOGON_SID_SIZE)) {

        if (SecurityHelper::AllocWinLogonProfile((WLX_PROFILE_V1_0**)ppWinLogonProfile, profilePath)) {

            if (WLX_SAS_TYPE_AUTHENTICATED == dwSasType) {
                success = true;
                action = WLX_SAS_ACTION_LOGON;
            }
            else {
                // copy login information for network providers
                pNprNotifyInfo->pszUserName = _localAllocString(userName);
                pNprNotifyInfo->pszDomain   = _localAllocString(domain);
                pNprNotifyInfo->pszPassword = _localAllocString(password);

                if (pNprNotifyInfo->pszUserName &&
                    pNprNotifyInfo->pszDomain   &&
                    pNprNotifyInfo->pszPassword) {

                    success = true;
                    action = WLX_SAS_ACTION_LOGON;
                }
            }
        }
    }

    if (success) {
        // GINA caches a copy of the interactive user's token
        _hToken = *phToken;
    }
    else {
        CloseHandle(*phToken);
        *phToken = 0;
    }
	return action;
}

BOOL Gina::ActivateUserShell(PWSTR pszDesktopName, PWSTR /* pszMprLogonScript */ , PVOID pEnvironment) {

    int programCount;
    wchar_t** programList;
	int i;
    if (RegistryHelper::ReadUserInitProgramList(&programList, &programCount)) {
        for (i = 0; i < programCount; ++i) {
            if (!SecurityHelper::CreateProcessAsUserOnDesktop(_hToken, programList[i], pszDesktopName, pEnvironment)) {
                break;
            }
        }
        RegistryHelper::FreeUserInitProgramList(programList, programCount);

        // GINA is required to release the environment block
        VirtualFree(pEnvironment, 0, MEM_RELEASE);

        return i == programCount; // if we launched all the Userinit programs, we succeeded
    }
    return FALSE;
}

int Gina::LoggedOnSAS(DWORD dwSasType) {

    if (WLX_SAS_TYPE_CTRL_ALT_DEL != dwSasType) return WLX_SAS_ACTION_NONE;

    for (;;) {
        SecurityOptionsDialog dlg(_pWinLogon, _hToken);
        switch (dlg.Show()) {
            case SecurityOptionsDialog::Lock:       return WLX_SAS_ACTION_LOCK_WKSTA;
            case SecurityOptionsDialog::Logoff:     return WLX_SAS_ACTION_LOGOFF;
            case SecurityOptionsDialog::Shutdown:   return WLX_SAS_ACTION_SHUTDOWN;
            case SecurityOptionsDialog::TaskMan:    return WLX_SAS_ACTION_TASKLIST;
            case WLX_DLG_SAS:                       continue; // show dialog again
            default:                                return WLX_SAS_ACTION_NONE;
        }
    }
}

void Gina::DisplayLockedNotice() {
    NoticeDialog dlg(_pWinLogon, IDD_LOCKEDNOTICE);
    dlg.Show();
}

int Gina::WkstaLockedSAS(DWORD dwSasType) {

    // we only support Control-Alt-Delete secure attention sequence (SAS)
    if (WLX_SAS_TYPE_CTRL_ALT_DEL != dwSasType) return WLX_SAS_ACTION_NONE;

    int result = WLX_SAS_ACTION_NONE;

    if (WLX_SAS_TYPE_CTRL_ALT_DEL == dwSasType) {

        // an optimization you might want to make here would be to cache
        // the user name and domain so the user doesn't have to retype it here
        LogonDialog dlg(_pWinLogon);

        // collect credentials from user
        if (IDOK == dlg.Show()) {

            // attempt the login
            DWORD win32Error;
            HANDLE hUnlockToken;
            if (SecurityHelper::CallLsaLogonUser(_hLsa, dlg.domain, dlg.userName, dlg.password, Unlock, 0, &hUnlockToken, 0, &win32Error)) {
                bool isSameUser;
                if (SecurityHelper::IsSameUser(hUnlockToken, _hToken, &isSameUser) && isSameUser) {
                    // user has returned to unlock her workstation
                    result = WLX_SAS_ACTION_UNLOCK_WKSTA;
                }
                else if (SecurityHelper::IsAdmin(hUnlockToken)) {
                    // admin trying to forcefully unlock the workstation
                    if (IDYES == _pWinLogon->wlxMessageBox(0, L"Forcefully log off the current user? He/she may lose work!", L"Log Off Windows", MB_ICONHAND | MB_YESNO | MB_DEFBUTTON2)) {
                        result = WLX_SAS_ACTION_FORCE_LOGOFF;
                    }
                }
                else {
                    // some non-admin is trying to break into this workstation
                    // tell them to get lost
                    _pWinLogon->wlxMessageBox(0, L"You do not have permission to force this user to log off.", L"Security", MB_ICONSTOP);
                }
                CloseHandle(hUnlockToken);
            }
            else {
                // message the user to let her know why the logon failed
                wchar_t msg[256];
                Log::LookupErrorMessage(msg, sizeof msg / sizeof *msg, win32Error);
                _pWinLogon->wlxMessageBox(0, msg, L"Logon Message", MB_ICONEXCLAMATION);
            }
        }        
    }

    return result;
}

BOOL Gina::IsLockOk() {
    return TRUE;
}

BOOL Gina::IsLogoffOk() {
    return TRUE;
}

void Gina::Logoff() {
    if (_hToken) {
        CloseHandle(_hToken);
        _hToken = 0;
    }
    LocalFree(_profilePath);
    _profilePath = 0;
}

void Gina::Shutdown(DWORD /* ShutdownType */) {
    if (_hToken) {
        CloseHandle(_hToken);
        _hToken = 0;
    }
    LocalFree(_profilePath);
    _profilePath = 0;

    if (_hLsa) {
        LsaDeregisterLogonProcess(_hLsa);
        _hLsa = 0;
    }

    // according to my sources, you're not guaranteed that this is the last message
    // - you may get status messages after this. I used to delete the Gina object here
    // but after finding this out, I decided to just leak the Gina - there's only one
    // instance per WinLogon process, and WinLogon is shutting down, so this should be fine.
}

BOOL Gina::NetworkProviderLoad(PWLX_MPR_NOTIFY_INFO pNprNotifyInfo) {
    // this entrypoint is obsolete according to my Microsoft contact
    return FALSE;
}

BOOL Gina::DisplayStatusMessage(HDESK hDesktop, DWORD /* dwOptions */, PWSTR pTitle, PWSTR pMessage) {
    delete _pStatusWindow;
    _pStatusWindow = new StatusWindow(hDesktop, pTitle, pMessage);
    if (!_pStatusWindow) {
        LOOM;
        return FALSE;
    }
    return TRUE;
}

BOOL Gina::GetStatusMessage(DWORD* /* pdwOptions */, PWSTR /* pMessage */, DWORD /* dwBufferSize */) {
    return FALSE; // not implemented in this sample - doesn't ever seem to be called anyway, but YMMV
}

BOOL Gina::RemoveStatusMessage() {
    delete _pStatusWindow;
    _pStatusWindow = 0;
    return TRUE;
}

BOOL Gina::GetConsoleSwitchCredentials(WLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0* pCredInfo) {
    if (WLX_CONSOLESWITCHCREDENTIAL_TYPE_V1_0 != pCredInfo->dwType) return FALSE;

    // copy over whatever data you want as we shuffle TS sessions
    ZeroMemory(pCredInfo, sizeof(*pCredInfo));

    pCredInfo->dwType = WLX_CONSOLESWITCHCREDENTIAL_TYPE_V1_0;

    pCredInfo->UserToken = _hToken;
    if (_profilePath) {
        pCredInfo->ProfileLength = sizeof(MSV1_0_INTERACTIVE_PROFILE);
        pCredInfo->ProfilePath = _localAllocString(_profilePath);
        if (!pCredInfo->ProfilePath) return FALSE;
    }

    // note that UserName must be set or things don't work properly
    // this implementation doesn't cache the user name, so we set this
    // to a dummy value.
    pCredInfo->UserName = _localAllocString(L"DummyUserName");

    return TRUE;
}

void Gina::DisconnectNotify() {
    // disconnecting from a TS session - nothing to do for this sample
}

void Gina::ReconnectNotify() {
    // connecting to a TS session - nothing to do for this sample
}
