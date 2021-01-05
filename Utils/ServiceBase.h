#pragma once

#include "Header.h"
#include "FileLog.h"

namespace Utils {

class CServiceBase {
public:
	CServiceBase(const std::wstring & service_name);
	virtual ~CServiceBase();
	static bool Run(CServiceBase * service);
	static VOID WINAPI ServiceMain(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors);
	static DWORD WINAPI ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

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
	CFileLog log_;
	static CServiceBase * ServiceInstance;
};

}
