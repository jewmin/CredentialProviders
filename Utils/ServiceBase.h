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
	bool Run(DWORD argc, LPWSTR * argv);
	virtual bool OnContinue();
	virtual bool OnPause();
	virtual bool OnPowerEvent(PPOWERBROADCAST_SETTING setting);
	virtual bool OnSessionChange(PWTSSESSION_NOTIFICATION notification);
	virtual bool OnShutdown();
	virtual bool OnStart(DWORD argc, LPWSTR * argv);
	virtual bool OnStop();

private:
	DWORD GetControlsAccepted(DWORD dwCurrentState);
	DWORD UpdateControlsAccepted(DWORD dwControlsAccepted);

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
	CFileLog log_;
	static CServiceBase * ServiceInstance;
};

}
