#pragma once

#include "Header.h"

namespace Utils {

class CServiceBase {
public:
	CServiceBase(const std::wstring & service_name);
	virtual ~CServiceBase();
	static bool Run(CServiceBase * service);
	static VOID WINAPI ServiceMain(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors);
	static DWORD WINAPI ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

	static bool InstallService(const std::wstring & service_name, const std::wstring & service_display_name, const std::wstring & service_description, const std::wstring & service_binary_path, DWORD dwStartType = SERVICE_AUTO_START);
	static bool UnInstallService(const std::wstring & service_name);

protected:
	virtual void OnStart(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors);
	virtual void OnPause();
	virtual void OnContinue();
	virtual void OnStop();
	virtual void OnShutdown();
	virtual void OnCustomCommand(DWORD command);
	virtual void OnPowerEvent(DWORD dwEventType, PPOWERBROADCAST_SETTING pSetting);
	virtual void OnSessionChange(DWORD dwEventType, PWTSSESSION_NOTIFICATION pNotification);

private:
	void Run(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors);
	virtual void RunService() = 0;
	void ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode = NO_ERROR, DWORD dwWaitHint = 0);
	DWORD UpdateControlsAccepted();

protected:
	bool can_stop_;
	bool can_shutdown_;
	bool can_pause_and_continue_;
	bool can_handle_power_event_;
	bool can_handle_session_change_event_;
	std::wstring service_name_;

private:
	SERVICE_STATUS service_status_;
	SERVICE_STATUS_HANDLE service_status_handle_;
	DWORD controls_accepted_;
	static CServiceBase * ServiceInstance;
};

#define Enum2Strings(x) case (x): return L#x;

inline const wchar_t * GetServiceStatusString(DWORD dwCurrentState) {
	switch (dwCurrentState) {
	Enum2Strings(SERVICE_STOPPED);
	Enum2Strings(SERVICE_START_PENDING);
	Enum2Strings(SERVICE_STOP_PENDING);
	Enum2Strings(SERVICE_RUNNING);
	Enum2Strings(SERVICE_CONTINUE_PENDING);
	Enum2Strings(SERVICE_PAUSE_PENDING);
	Enum2Strings(SERVICE_PAUSED);
	default: return L"";
	}
}

inline const wchar_t * GetServiceControlString(DWORD dwControl) {
	switch (dwControl) {
	Enum2Strings(SERVICE_CONTROL_STOP);
	Enum2Strings(SERVICE_CONTROL_PAUSE);
	Enum2Strings(SERVICE_CONTROL_CONTINUE);
	Enum2Strings(SERVICE_CONTROL_INTERROGATE);
	Enum2Strings(SERVICE_CONTROL_SHUTDOWN);
	Enum2Strings(SERVICE_CONTROL_PARAMCHANGE);
	Enum2Strings(SERVICE_CONTROL_NETBINDADD);
	Enum2Strings(SERVICE_CONTROL_NETBINDREMOVE);
	Enum2Strings(SERVICE_CONTROL_NETBINDENABLE);
	Enum2Strings(SERVICE_CONTROL_NETBINDDISABLE);
	Enum2Strings(SERVICE_CONTROL_DEVICEEVENT);
	Enum2Strings(SERVICE_CONTROL_HARDWAREPROFILECHANGE);
	Enum2Strings(SERVICE_CONTROL_POWEREVENT);
	Enum2Strings(SERVICE_CONTROL_SESSIONCHANGE);
	Enum2Strings(SERVICE_CONTROL_PRESHUTDOWN);
	// Enum2Strings(SERVICE_CONTROL_TIMECHANGE);
	// Enum2Strings(SERVICE_CONTROL_TRIGGEREVENT);
	// Enum2Strings(SERVICE_CONTROL_LOWRESOURCES);
	// Enum2Strings(SERVICE_CONTROL_SYSTEMLOWRESOURCES);
	default: return L"";
	}
}

inline const wchar_t * GetPBTString(DWORD dwEventType) {
	switch (dwEventType) {
	Enum2Strings(PBT_APMQUERYSUSPEND);
	Enum2Strings(PBT_APMQUERYSTANDBY);
	Enum2Strings(PBT_APMQUERYSUSPENDFAILED);
	Enum2Strings(PBT_APMQUERYSTANDBYFAILED);
	Enum2Strings(PBT_APMSUSPEND);
	Enum2Strings(PBT_APMSTANDBY);
	Enum2Strings(PBT_APMRESUMECRITICAL);
	Enum2Strings(PBT_APMRESUMESUSPEND);
	Enum2Strings(PBT_APMRESUMESTANDBY);
	Enum2Strings(PBT_APMBATTERYLOW);
	Enum2Strings(PBT_APMPOWERSTATUSCHANGE);
	Enum2Strings(PBT_APMOEMEVENT);
	Enum2Strings(PBT_APMRESUMEAUTOMATIC);
	Enum2Strings(PBT_POWERSETTINGCHANGE);
	default: return L"";
	}
}

inline const wchar_t * GetWTSString(DWORD dwEventType) {
	switch (dwEventType) {
	Enum2Strings(WTS_CONSOLE_CONNECT);
	Enum2Strings(WTS_CONSOLE_DISCONNECT);
	Enum2Strings(WTS_REMOTE_CONNECT);
	Enum2Strings(WTS_REMOTE_DISCONNECT);
	Enum2Strings(WTS_SESSION_LOGON);
	Enum2Strings(WTS_SESSION_LOGOFF);
	Enum2Strings(WTS_SESSION_LOCK);
	Enum2Strings(WTS_SESSION_UNLOCK);
	Enum2Strings(WTS_SESSION_REMOTE_CONTROL);
	// Enum2Strings(WTS_SESSION_CREATE);
	// Enum2Strings(WTS_SESSION_TERMINATE);
	default: return L"";
	}
}

}
