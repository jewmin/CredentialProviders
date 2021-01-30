#pragma once

#include "IWinlogon.h"
#include "Utils.h"
#include "UI/GinaWindow.h"
#include "MyService.h"

class Gina {
public:
    static BOOL Negotiate(DWORD dwWinlogonVersion, PDWORD pdwDllVersion);
    // 初始化
    static BOOL Initialize(HANDLE hWlx, PVOID pWinlogonFunctions, Gina * * ppGina);

public:
    // 未登录
    int LoggedOutSAS(DWORD dwSasType, PLUID pAuthenticationId, PSID pLogonSid, PDWORD pdwOptions, PHANDLE phToken, PWLX_MPR_NOTIFY_INFO pNprNotifyInfo, PVOID * ppWinlogonProfile);
    // 已登录
    int LoggedOnSAS(DWORD dwSasType);
    // 锁定
    int WkstaLockedSAS(DWORD dwSasType);

    BOOL ActivateUserShell(PWSTR pszDesktopName, PWSTR pszMprLogonScript, PVOID pEnvironment);

    // 未登录时显示ctrl+del+alt对话框
    VOID DisplaySASNotice();
    // 锁定时显示ctrl+del+alt对话框
    VOID DisplayLockedNotice();

    BOOL IsLockOk();
    BOOL IsLogoffOk();

    // 注销
    VOID Logoff();
    // 关机
    VOID Shutdown(DWORD ShutdownType);

    BOOL ScreenSaverNotify(BOOL * pSecure);
    BOOL StartApplication(PWSTR pszDesktopName, PVOID pEnvironment, PWSTR pszCmdLine);

    BOOL NetworkProviderLoad(PWLX_MPR_NOTIFY_INFO pNprNotifyInfo);

    // 显示windows状态消息
    BOOL DisplayStatusMessage(HDESK hDesktop, DWORD dwOptions, PWSTR pTitle, PWSTR pMessage);
    BOOL GetStatusMessage(DWORD * pdwOptions, PWSTR pMessage, DWORD dwBufferSize);
    BOOL RemoveStatusMessage();

    BOOL GetConsoleSwitchCredentials(PWLX_CONSOLESWITCH_CREDENTIALS_INFO_V1_0 pCredInfo);
    VOID ReconnectNotify();
    VOID DisconnectNotify();

private:
    Gina(IWinlogon * pWinlogon, HANDLE LsaHandle);
    void Auth(Utils::Protocol::LoginRequest * request, Utils::Protocol::LoginResponse * response);

private:
    IWinlogon *             pWinlogon_;
    HANDLE                  hLsaHandle_;            // Pointer that receives a handle used in future authentication function calls.
    HANDLE                  hUserToken_;            // Handle of the users token.
    PWSTR                   pszProfilePath_;        // UNICODE_STRING specifying the path to the user's roaming profile if the user has a roaming profile. For example: \SomeServer\SomeShare\MyUserName
    GinaWindow *            pGinaWindow_;           // 显示状态消息窗口
    Utils::IService *       pService_;              // 业务逻辑接口
};
