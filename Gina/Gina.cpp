#include "Gina.h"
#include "MockWinlogon.h"
#include "RealWinlogon.h"
#include "SecurityHelper.h"

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

    HANDLE LsaHandle;
    if (!SecurityHelper::RegisterLogonProcess(&LsaHandle)) {
        return FALSE;
    }

    *ppGina = new Gina(pWinlogon, LsaHandle);
    return TRUE;
}

Gina::Gina(IWinlogon * pWinlogon, HANDLE LsaHandle)
    : pWinlogon_(pWinlogon)
    , LsaHandle_(LsaHandle)
    , UserToken_(NULL)
    , ProfilePath_(NULL) {
    // 告诉Winlogon我们使用Ctrl-Alt-Del
    ULONG_PTR OldValue;
    pWinlogon_->WlxSetOption(WLX_OPTION_USE_CTRL_ALT_DEL, TRUE, &OldValue);

    // 远程登录用户输入模拟Ctrl-Alt-Del
    if (0 != GetSystemMetrics(SM_REMOTESESSION)) {
        pWinlogon_->WlxSasNotify(WLX_SAS_TYPE_CTRL_ALT_DEL);
    }
}

int Gina::LoggedOutSAS(DWORD dwSasType, PLUID pAuthenticationId, PSID pLogonSid, PDWORD pdwOptions, PHANDLE phToken, PWLX_MPR_NOTIFY_INFO pNprNotifyInfo, PVOID * pProfile) {
    return 0;
}

int Gina::LoggedOnSAS(DWORD dwSasType) {
    return 0;
}

int Gina::WkstaLockedSAS(DWORD dwSasType) {
    return 0;
}

BOOL Gina::ActivateUserShell(PWSTR pszDesktopName, PWSTR pszMprLogonScript, PVOID pEnvironment) {
    return FALSE;
}

VOID Gina::DisplaySASNotice() {

}

VOID Gina::DisplayLockedNotice() {

}

BOOL Gina::IsLockOk() {
    return FALSE;
}

BOOL Gina::IsLogoffOk() {
    return FALSE;
}

VOID Gina::Logoff() {

}

VOID Gina::Shutdown(DWORD ShutdownType) {

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
    return FALSE;
}

BOOL Gina::GetStatusMessage(DWORD * pdwOptions, PWSTR pMessage, DWORD dwBufferSize) {
    return FALSE;
}

BOOL Gina::RemoveStatusMessage() {
    return FALSE;
}

BOOL Gina::GetConsoleSwitchCredentials(PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 pCredInfo) {
    return FALSE;
}

VOID Gina::ReconnectNotify() {
}

VOID Gina::DisconnectNotify() {
}
