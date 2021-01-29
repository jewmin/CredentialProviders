#include "Gina.h"
#include "MockWinlogon.h"
#include "RealWinlogon.h"
#include "SecurityHelper.h"
#include "UI/NoticeWindow.h"
#include "UI/StatusWindow.h"
#include "UI/SecurityOptionsDialog.h"
#include "UI/LogonDialog.h"
#include "UI/NoticeDialog.h"

// length of a Windows logon SID of the form S-1-5-5-x-y
#define LOGON_SID_SIZE 20

BOOL Gina::Negotiate(DWORD dwWinlogonVersion, PDWORD pdwDllVersion) {
    if (dwWinlogonVersion < WLX_VERSION_1_3) {
        return FALSE;
    }

    *pdwDllVersion = dwWinlogonVersion;

    // 保存winlogon版本
    IWinlogon::Version(dwWinlogonVersion);
    return TRUE;
}

BOOL Gina::Initialize(HANDLE hWlx, PVOID pWinlogonFunctions, Gina * * ppGina) {
    IWinlogon * pWinlogon;
    if (!hWlx || !pWinlogonFunctions) {
        pWinlogon = new MockWinlogon();
    } else {
        pWinlogon = new RealWinlogon(hWlx, pWinlogonFunctions);
    }

    if (!pWinlogon) {
        Utils::Output(L"Gina::Initialize pWinlogon: Out Of Memory");
        return FALSE;
    }

    HANDLE LsaHandle;
    if (!SecurityHelper::RegisterLogonProcess(&LsaHandle)) {
        return FALSE;
    }

    *ppGina = new Gina(pWinlogon, LsaHandle);
    if (!*ppGina) {
        Utils::Output(L"Gina::Initialize *ppGina: Out Of Memory");
        return FALSE;
    }

    return TRUE;
}

Gina::Gina(IWinlogon * pWinlogon, HANDLE LsaHandle)
    : pWinlogon_(pWinlogon)
    , hLsaHandle_(LsaHandle)
    , hUserToken_(NULL)
    , pszProfilePath_(NULL)
    , pGinaWindow_(NULL) {
    // 告诉Winlogon我们使用Ctrl-Alt-Del
    ULONG_PTR OldValue;
    pWinlogon_->WlxSetOption(WLX_OPTION_USE_CTRL_ALT_DEL, TRUE, &OldValue);

    // 远程登录用户输入模拟Ctrl-Alt-Del
    if (0 != ::GetSystemMetrics(SM_REMOTESESSION)) {
        pWinlogon_->WlxSasNotify(WLX_SAS_TYPE_CTRL_ALT_DEL);
    }
}

// 未登录
int Gina::LoggedOutSAS(DWORD dwSasType, PLUID pAuthenticationId, PSID pLogonSid, PDWORD pdwOptions, PHANDLE phToken, PWLX_MPR_NOTIFY_INFO pNprNotifyInfo, PVOID * ppWinlogonProfile) {
    RemoveStatusMessage();
    *pdwOptions = 0; // 由Winlogon加载用户配置
    ZeroMemory(pNprNotifyInfo, sizeof(*pNprNotifyInfo));

    wchar_t *       profilePath = NULL;
    const wchar_t * domain      = NULL;
    const wchar_t * username    = NULL;
    const wchar_t * password    = NULL;
    LogonDialog dlg(pWinlogon_);

    if (WLX_SAS_TYPE_CTRL_ALT_DEL == dwSasType) {
        // 登录对话框，获取域名、用户名、密码
        if (IDOK != dlg.Show()) {
            return WLX_SAS_ACTION_NONE;
        }

        // attempt the login
        DWORD win32Error;
        MSV1_0_INTERACTIVE_PROFILE * pProfile = NULL;
        if (!SecurityHelper::CallLsaLogonUser(hLsaHandle_, dlg.domain_, dlg.username_, dlg.password_, Interactive, pAuthenticationId, phToken, &pProfile, &win32Error)) {
            // NOTE: a full implementation would deal with expired / must change passwords here
            // by reading the statistics in the profile and giving the user a chance to
            // change her password if it's expired or about to expire

            // message the user to let her know why the logon failed
            std::wstring msg = Utils::GetErrorString(win32Error);
            pWinlogon_->WlxMessageBox(NULL, const_cast<LPWSTR>(msg.c_str()), L"Logon Message", MB_ICONEXCLAMATION);
            return WLX_SAS_ACTION_NONE;
        }

        if (!SecurityHelper::ExtractProfilePath(&profilePath, pProfile)) {
            return WLX_SAS_ACTION_NONE;
        }
        ::LsaFreeReturnBuffer(pProfile);

        domain   = dlg.domain_;
        username = dlg.username_;
        password = dlg.password_;
    } else if (WLX_SAS_TYPE_AUTHENTICATED == dwSasType) {
        // use the information in the other TS session to auto-logon this session
        WLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 credInfo;
        ZeroMemory(&credInfo, sizeof(credInfo));
        credInfo.dwType = WLX_CONSOLESWITCHCREDENTIAL_TYPE_V1_0;

        Utils::Output(L"Gina::LoggedOutSAS - Calling WlxQueryConsoleSwitchCredentials");
        if (pWinlogon_->WlxQueryConsoleSwitchCredentials(&credInfo)) {
            Utils::Output(L"Gina::LoggedOutSAS - WlxQueryConsoleSwitchCredentials Successfully");

            *phToken = credInfo.UserToken;
            profilePath = credInfo.ProfilePath;
            Utils::Output(Utils::StringFormat(L"Gina::LoggedOutSAS - Profile Path: %s", profilePath ? profilePath : L"<null>"));

            if (!SecurityHelper::GetLogonSessionId(credInfo.UserToken, pAuthenticationId)) {
                // this should never fail, just a sanity check
                return WLX_SAS_ACTION_NONE;
            }

            // I believe the profile is already loaded in this case;
            // if you don't specify this option, WinLogon will fail and the error log will
            // complain that the profile failed to be loaded
            *pdwOptions = WLX_LOGON_OPT_NO_PROFILE;
        } else {
            Utils::Output(L"Gina::LoggedOutSAS - WlxQueryConsoleSwitchCredentials Error");
            return WLX_SAS_ACTION_NONE;
        }
    } else {
        Utils::Output(L"Gina::LoggedOutSAS - WARNING: Unrecognized SASType");
        return WLX_SAS_ACTION_NONE;
    }

    // if we get this far, the login succeeded, but there are a few minor things that could still fail
    int action = WLX_SAS_ACTION_NONE;
    bool success = false;

    // cache the profilePath in case we need to transfer it to another TS session
    ::LocalFree(pszProfilePath_);
    pszProfilePath_ = profilePath;

    // Assume that WinLogon provides a buffer large enough to hold a logon SID,
    // which is of fixed length. It'd be nice if WinLogon would tell us how big
    // its buffer actually was, but it appears this is assumed.
    if (SecurityHelper::GetLogonSid(*phToken, pLogonSid, LOGON_SID_SIZE)) {
        if (SecurityHelper::AllocWinLogonProfile((WLX_PROFILE_V1_0 * *)ppWinlogonProfile, profilePath)) {
            if (WLX_SAS_TYPE_AUTHENTICATED == dwSasType) {
                success = true;
                action = WLX_SAS_ACTION_LOGON;
            } else {
                // copy login information for network providers
                pNprNotifyInfo->pszDomain   = SecurityHelper::LocalAllocString(domain);
                pNprNotifyInfo->pszUserName = SecurityHelper::LocalAllocString(username);
                pNprNotifyInfo->pszPassword = SecurityHelper::LocalAllocString(password);

                if (pNprNotifyInfo->pszUserName && pNprNotifyInfo->pszDomain && pNprNotifyInfo->pszPassword) {
                    success = true;
                    action = WLX_SAS_ACTION_LOGON;
                }
            }
        }
    }

    if (success) {
        // GINA caches a copy of the interactive user's token
        hUserToken_ = *phToken;
    } else {
        ::CloseHandle(*phToken);
        *phToken = NULL;
    }

    return action;
}

// 已登录
int Gina::LoggedOnSAS(DWORD dwSasType) {
    RemoveStatusMessage();
    if (WLX_SAS_TYPE_CTRL_ALT_DEL != dwSasType) {
        return WLX_SAS_ACTION_NONE;
    }

    while (true) {
        SecurityOptionsDialog dlg(pWinlogon_, hUserToken_);
        switch (dlg.Show()) {
        case SecurityOptionsDialog::Lock:
            return WLX_SAS_ACTION_LOCK_WKSTA;

        case SecurityOptionsDialog::Logoff:
            return WLX_SAS_ACTION_LOGOFF;

        case SecurityOptionsDialog::Shutdown:
            return WLX_SAS_ACTION_SHUTDOWN;

        case SecurityOptionsDialog::Taskman:
            return WLX_SAS_ACTION_TASKLIST;

        case WLX_DLG_SAS:
            continue;

        default:
            return WLX_SAS_ACTION_NONE;
        }
    }
}

// 锁屏
int Gina::WkstaLockedSAS(DWORD dwSasType) {
    RemoveStatusMessage();
    // we only support Control-Alt-Delete secure attention sequence (SAS)
    if (WLX_SAS_TYPE_CTRL_ALT_DEL != dwSasType) {
        return WLX_SAS_ACTION_NONE;
    }

    int result = WLX_SAS_ACTION_NONE;

    if (WLX_SAS_TYPE_CTRL_ALT_DEL == dwSasType) {
        // an optimization you might want to make here would be to cache
        // the user name and domain so the user doesn't have to retype it here
        LogonDialog dlg(pWinlogon_);

        // collect credentials from user
        if (IDOK == dlg.Show()) {
            // attempt the login
            DWORD win32Error;
            HANDLE hUnlockToken;
            if (SecurityHelper::CallLsaLogonUser(hLsaHandle_, dlg.domain_, dlg.username_, dlg.password_, Unlock, 0, &hUnlockToken, 0, &win32Error)) {
                bool isSameUser;
                if (SecurityHelper::IsSameUser(hUnlockToken, hUserToken_, &isSameUser) && isSameUser) {
                    // user has returned to unlock her workstation
                    result = WLX_SAS_ACTION_UNLOCK_WKSTA;
                } else if (SecurityHelper::IsAdmin(hUnlockToken)) {
                    // admin trying to forcefully unlock the workstation
                    if (IDYES == pWinlogon_->WlxMessageBox(NULL, L"Forcefully log off the current user? He/she may lose work!", L"Log Off Windows", MB_ICONHAND | MB_YESNO | MB_DEFBUTTON2)) {
                        result = WLX_SAS_ACTION_FORCE_LOGOFF;
                    }
                } else {
                    // some non-admin is trying to break into this workstation
                    // tell them to get lost
                    pWinlogon_->WlxMessageBox(NULL, L"You do not have permission to force this user to log off.", L"Security", MB_ICONSTOP);
                }
                ::CloseHandle(hUnlockToken);
            } else {
                // message the user to let her know why the logon failed
                std::wstring msg = Utils::GetErrorString(win32Error);
                pWinlogon_->WlxMessageBox(NULL, const_cast<LPWSTR>(msg.c_str()), L"Logon Message", MB_ICONEXCLAMATION);
            }
        }
    }

    return result;
}

BOOL Gina::ActivateUserShell(PWSTR pszDesktopName, PWSTR pszMprLogonScript, PVOID pEnvironment) {
    int programCount;
    wchar_t * * programList;
	int i;
    if (SecurityHelper::ReadUserInitProgramList(&programList, &programCount)) {
        for (i = 0; i < programCount; ++i) {
            if (!SecurityHelper::CreateProcessAsUserOnDesktop(hUserToken_, programList[i], pszDesktopName, pEnvironment)) {
                break;
            }
        }

        SecurityHelper::FreeUserInitProgramList(programList, programCount);

        // GINA is required to release the environment block
        ::VirtualFree(pEnvironment, 0, MEM_RELEASE);

        return i == programCount; // if we launched all the Userinit programs, we succeeded
    }

    return FALSE;
}

VOID Gina::DisplaySASNotice() {
    RemoveStatusMessage();
    // pGinaWindow_ = new NoticeWindow(::GetThreadDesktop(::GetCurrentThreadId()), IDD_SASNOTICE);
    // if (!pGinaWindow_) {
    //     Utils::Output(L"Gina::DisplaySASNotice pGinaWindow_: Out Of Memory");
    // }
    NoticeDialog dlg(pWinlogon_, IDD_SASNOTICE);
    dlg.Show();
}

VOID Gina::DisplayLockedNotice() {
    RemoveStatusMessage();
    // pGinaWindow_ = new NoticeWindow(::GetThreadDesktop(::GetCurrentThreadId()), IDD_LOCKEDNOTICE);
    // if (!pGinaWindow_) {
    //     Utils::Output(L"Gina::DisplaySASNotice pGinaWindow_: Out Of Memory");
    // }
    NoticeDialog dlg(pWinlogon_, IDD_LOCKEDNOTICE);
    dlg.Show();
}

BOOL Gina::IsLockOk() {
    return TRUE;
}

BOOL Gina::IsLogoffOk() {
    return TRUE;
}

VOID Gina::Logoff() {
    if (hUserToken_) {
        ::CloseHandle(hUserToken_);
        hUserToken_ = NULL;
    }
    if (pszProfilePath_) {
        ::LocalFree(pszProfilePath_);
        pszProfilePath_ = NULL;
    }
}

VOID Gina::Shutdown(DWORD ShutdownType) {
    Logoff();
    if (hLsaHandle_) {
        ::LsaDeregisterLogonProcess(hLsaHandle_);
        hLsaHandle_ = NULL;
    }
}

BOOL Gina::ScreenSaverNotify(BOOL * pSecure) {
    return FALSE;
}

BOOL Gina::StartApplication(PWSTR pszDesktopName, PVOID pEnvironment, PWSTR pszCmdLine) {
    return FALSE;
}

BOOL Gina::NetworkProviderLoad(PWLX_MPR_NOTIFY_INFO pNprNotifyInfo) {
    return FALSE;
}

BOOL Gina::DisplayStatusMessage(HDESK hDesktop, DWORD dwOptions, PWSTR pTitle, PWSTR pMessage) {
    RemoveStatusMessage();
    pGinaWindow_ = new StatusWindow(hDesktop, pTitle, pMessage);
    if (!pGinaWindow_) {
        Utils::Output(L"Gina::DisplayStatusMessage pGinaWindow_: Out Of Memory");
        return FALSE;
    }
    return TRUE;
}

BOOL Gina::GetStatusMessage(DWORD * pdwOptions, PWSTR pMessage, DWORD dwBufferSize) {
    return FALSE;
}

BOOL Gina::RemoveStatusMessage() {
    if (pGinaWindow_) {
        delete pGinaWindow_;
        pGinaWindow_ = NULL;
    }
    return TRUE;
}

BOOL Gina::GetConsoleSwitchCredentials(PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 pCredInfo) {
    if (WLX_CONSOLESWITCHCREDENTIAL_TYPE_V1_0 != pCredInfo->dwType) {
        return FALSE;
    }

    // 复制凭证信息
    ZeroMemory(pCredInfo, sizeof(*pCredInfo));
    pCredInfo->dwType = WLX_CONSOLESWITCHCREDENTIAL_TYPE_V1_0;
    pCredInfo->UserToken = hUserToken_;
    if (pszProfilePath_) {
        pCredInfo->ProfileLength = sizeof(MSV1_0_INTERACTIVE_PROFILE);
        pCredInfo->ProfilePath = SecurityHelper::LocalAllocString(pszProfilePath_);
        if (!pCredInfo->ProfilePath) {
            return FALSE;
        }
    }

    pCredInfo->UserName = SecurityHelper::LocalAllocString(L"DummyUserName");
    if (!pCredInfo->UserName) {
        return FALSE;
    }

    return TRUE;
}

VOID Gina::ReconnectNotify() {
}

VOID Gina::DisconnectNotify() {
}
