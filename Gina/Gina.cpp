#include "Gina.h"
#include "MockWinlogon.h"
#include "RealWinlogon.h"
#include "SecurityHelper.h"
#include "UI/NoticeDialog.h"
#include "UI/SecurityOptionsDialog.h"
#include "UI/LogonDialog.h"

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
    , pStatusDialog_(NULL) {
    // 告诉Winlogon我们使用Ctrl-Alt-Del
    ULONG_PTR OldValue;
    pWinlogon_->WlxSetOption(WLX_OPTION_USE_CTRL_ALT_DEL, TRUE, &OldValue);

    // 远程登录用户输入模拟Ctrl-Alt-Del
    if (0 != GetSystemMetrics(SM_REMOTESESSION)) {
        pWinlogon_->WlxSasNotify(WLX_SAS_TYPE_CTRL_ALT_DEL);
    }
}

int Gina::LoggedOutSAS(DWORD dwSasType, PLUID pAuthenticationId, PSID pLogonSid, PDWORD pdwOptions, PHANDLE phToken, PWLX_MPR_NOTIFY_INFO pNprNotifyInfo, PVOID * pProfile) {
    *pdwOptions = 0; // 由Winlogon加载用户配置
    ZeroMemory(pNprNotifyInfo, sizeof(*pNprNotifyInfo));

    if (WLX_SAS_TYPE_CTRL_ALT_DEL == dwSasType) {
        // 登录对话框，获取域名、用户名、密码
        LogonDialog dlg(pWinlogon_);
        if (IDOK != dlg.Show()) {
            return WLX_SAS_ACTION_NONE;
        }

        MSV1_0_INTERACTIVE_PROFILE * pProfile = NULL;
        
    }

    return 0;
}

int Gina::LoggedOnSAS(DWORD dwSasType) {
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

int Gina::WkstaLockedSAS(DWORD dwSasType) {
    // TODO
    return 0;
}

BOOL Gina::ActivateUserShell(PWSTR pszDesktopName, PWSTR pszMprLogonScript, PVOID pEnvironment) {
    // TODO
    return FALSE;
}

VOID Gina::DisplaySASNotice() {
    NoticeDialog dlg(pWinlogon_, IDD_SASNOTICE);
    dlg.Show();
}

VOID Gina::DisplayLockedNotice() {
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
    pStatusDialog_ = new StatusDialog(hDesktop, pTitle, pMessage);
    if (!pStatusDialog_) {
        Utils::Output(L"Gina::DisplayStatusMessage pStatusDialog_: Out Of Memory");
        return FALSE;
    }
    return TRUE;
}

BOOL Gina::GetStatusMessage(DWORD * pdwOptions, PWSTR pMessage, DWORD dwBufferSize) {
    return FALSE;
}

BOOL Gina::RemoveStatusMessage() {
    if (pStatusDialog_) {
        delete pStatusDialog_;
        pStatusDialog_ = NULL;
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
