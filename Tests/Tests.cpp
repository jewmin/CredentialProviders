// Tests.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <credentialprovider.h>
#include <wincred.h>
#include "Utils.h"
#include "FileLog.h"
#include "WindowsHelper.h"
#include "ServiceBase.h"

#define enum2string(val) L#val

class DemoService : public Utils::CServiceBase {
public:
	DemoService() : CServiceBase(L"DemoService"), shutdown_(false) {
		can_shutdown_ = true;
		can_pause_and_continue_ = true;
		can_handle_power_event_ = true;
		can_handle_session_change_event_ = true;
	}
	virtual ~DemoService() {

	}
	virtual void RunService() {
		while (!shutdown_) {
			Sleep(16);
		}
	}
	virtual void OnStop() {
		shutdown_ = true;
	}
private:
	bool shutdown_;
};

int main(int argc, wchar_t ** argv)
{
	// BOOL save = false;
	// DWORD authPackage = 0;
	// LPVOID authBuffer;
	// ULONG authBufferSize = 0;
	// CREDUI_INFO credUiInfo;

	// credUiInfo.pszCaptionText = TEXT("My caption");
	// credUiInfo.pszMessageText = TEXT("My message");
	// credUiInfo.cbSize = sizeof(credUiInfo);
	// credUiInfo.hbmBanner = NULL;
	// credUiInfo.hwndParent = NULL;

	// CredUIPromptForWindowsCredentials(&credUiInfo, 0, &authPackage, NULL, 0, &authBuffer, &authBufferSize, &save, 0);

	Utils::CFileLog log;
	Utils::SetLog(&log);
	Utils::Output(L"test测试123");
	log.SetLogFileNameFormat(L"%d%d%d.log");
	Utils::Output(L"sksksjdf测试枯叶sdfk夺");
	Utils::WToA(NULL);
	Utils::StringFormat(L"测试abc %s %d", L"瞧瞧", 1000);
	// Utils::Output(enum2string(CPUS_LOGON));
	// CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus = CPUS_UNLOCK_WORKSTATION;
	// Utils::Output(enum2string(CREDENTIAL_PROVIDER_USAGE_SCENARIO(cpus)));
	PWTS_SESSION_INFOW infos;
	DWORD count;
	if (::WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &infos, &count)) {
		for (DWORD i = 0; i < count; i++) {
			Utils::Output(Utils::StringFormat(L"%s, %d, %d", infos[i].pWinStationName, infos[i].SessionId, infos[i].State));
			Utils::Output(Utils::StringFormat(L"domain: %s, user: %s", Utils::GetSessionDomainName(infos[i].SessionId).c_str(), Utils::GetSessionUserName(infos[i].SessionId).c_str()));
		}
		::WTSFreeMemory(infos);
	}

	Utils::CServiceBase::Run(new DemoService());

	return 0;
}
