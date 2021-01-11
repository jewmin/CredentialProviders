#include "ServiceBase.h"
#include "Utils.h"
#include "FileLog.h"

namespace Utils {

CServiceBase * CServiceBase::ServiceInstance = NULL;


bool CServiceBase::InstallService(const std::wstring & service_name, const std::wstring & service_display_name, const std::wstring & service_description, const std::wstring & service_binary_path, DWORD dwStartType) {
	Utils::Output(Utils::StringFormat(L"CServiceBase::InstallService service_name: %s, service_display_name: %s, service_description: %s, service_binary_path: %s", service_name.c_str(), service_display_name.c_str(), service_description.c_str(), service_binary_path.c_str()));
	SC_HANDLE hSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager) {
		Utils::Output(Utils::StringFormat(L"OpenSCManager Error: %s", Utils::GetLastErrorString().c_str()));
		return false;
	}

	SC_HANDLE hService = ::CreateService(hSCManager, service_name.c_str(), service_display_name.c_str(), SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, dwStartType, SERVICE_ERROR_NORMAL, service_binary_path.c_str(), NULL, NULL, NULL, NULL, NULL);
	if (!hService) {
		Utils::Output(Utils::StringFormat(L"CreateService Error: %s", Utils::GetLastErrorString().c_str()));
		::CloseServiceHandle(hSCManager);
		return false;
	}

	SERVICE_DESCRIPTION sd;
	sd.lpDescription = const_cast<LPWSTR>(service_description.c_str());
	if (!::ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sd)) {
		Utils::Output(Utils::StringFormat(L"ChangeServiceConfig2 Error: %s", Utils::GetLastErrorString().c_str()));
		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCManager);
		return false;
	}

	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCManager);
	return true;
}

bool CServiceBase::UnInstallService(const std::wstring & service_name) {
	Utils::Output(Utils::StringFormat(L"CServiceBase::UnInstallService service_name: %s", service_name.c_str()));
	SC_HANDLE hSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager) {
		Utils::Output(Utils::StringFormat(L"OpenSCManager Error: %s", Utils::GetLastErrorString().c_str()));
		return false;
	}

	SC_HANDLE hService = ::OpenService(hSCManager, service_name.c_str(), SERVICE_ALL_ACCESS);
	if (!hService) {
		Utils::Output(Utils::StringFormat(L"OpenService Error: %s", Utils::GetLastErrorString().c_str()));
		::CloseServiceHandle(hSCManager);
		return false;
	}

	SERVICE_STATUS status = { 0 };
	if (!::QueryServiceStatus(hService, &status))
	{
		Utils::Output(Utils::StringFormat(L"QueryServiceStatus Error: %s", Utils::GetLastErrorString().c_str()));
		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCManager);
		return false;
	}

	if (status.dwCurrentState != SERVICE_STOPPED) {
		if (status.dwCurrentState == SERVICE_RUNNING || status.dwCurrentState == SERVICE_PAUSED) {
			if (!::ControlService(hService, SERVICE_CONTROL_STOP, &status)) {
				Utils::Output(Utils::StringFormat(L"ControlService STOP Error: %s", Utils::GetLastErrorString().c_str()));
				::CloseServiceHandle(hService);
				::CloseServiceHandle(hSCManager);
				return false;
			}
		} else {
			Utils::Output(Utils::StringFormat(L"dwCurrentState: %s, NOT RUNNING OR PAUSED", GetServiceStatusString(status.dwCurrentState)));
			return false;
		}
	}

	if (!::DeleteService(hService)) {
		Utils::Output(Utils::StringFormat(L"DeleteService Error: %s", Utils::GetLastErrorString().c_str()));
		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCManager);
		return false;
	}

	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCManager);
	return true;
}

bool CServiceBase::Run(CServiceBase * service) {
	Utils::Output(Utils::StringFormat(L"CServiceBase::Run service: %p", service));
	if (!service) {
		return false;
	}

	ServiceInstance = service;
	SERVICE_TABLE_ENTRY ServiceStartTable[] = {
		{ const_cast<LPWSTR>(ServiceInstance->service_name_.c_str()), CServiceBase::ServiceMain },
		{ NULL, NULL }
	};

	if (!::StartServiceCtrlDispatcher(ServiceStartTable)) {
		return false;
	}

	return true;
}

VOID WINAPI CServiceBase::ServiceMain(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors) {
	Utils::CFileLog log;
	log.SetLogFileNameFormat(L"Service_%02d%02d%02d.log");
	Utils::SetLog(&log);
	Utils::Output(Utils::StringFormat(L"CServiceBase::ServiceMain dwNumServicesArgs: %u, lpServiceArgVectors: %p", dwNumServicesArgs, lpServiceArgVectors));
	assert(NULL != ServiceInstance);
	ServiceInstance->service_status_handle_ = ::RegisterServiceCtrlHandlerEx(ServiceInstance->service_name_.c_str(), CServiceBase::ServiceCtrlHandler, reinterpret_cast<LPVOID>(ServiceInstance));
	if (ServiceInstance->service_status_handle_) {
		ServiceInstance->Run(dwNumServicesArgs, lpServiceArgVectors);
	}
	delete ServiceInstance;
	ServiceInstance = NULL;
	Utils::SetLog(NULL);
}

DWORD WINAPI CServiceBase::ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) {
	Utils::Output(Utils::StringFormat(L"CServiceBase::ServiceCtrlHandler dwControl: %s, dwEventType: %u, lpEventData: %p, lpContext: %p", GetServiceControlString(dwControl), dwEventType, lpEventData, lpContext));
	if (!lpContext) {
		return ERROR_INVALID_PARAMETER;
	}

	CServiceBase * service = reinterpret_cast<CServiceBase *>(lpContext);
	if (!service) {
		return ERROR_INVALID_PARAMETER;
	}

	DWORD dwRet = NO_ERROR;
	switch (dwControl) {
	case SERVICE_CONTROL_STOP:
		service->ReportServiceStatus(SERVICE_STOP_PENDING);
		service->OnStop();
		break;

	case SERVICE_CONTROL_PAUSE:
		service->ReportServiceStatus(SERVICE_PAUSE_PENDING);
		service->OnPause();
		service->ReportServiceStatus(SERVICE_PAUSED);
		break;

	case SERVICE_CONTROL_CONTINUE:
		service->ReportServiceStatus(SERVICE_CONTINUE_PENDING);
		service->OnContinue();
		service->ReportServiceStatus(SERVICE_RUNNING);
		break;

	case SERVICE_CONTROL_INTERROGATE:
		service->ReportServiceStatus(service->service_status_.dwCurrentState, service->service_status_.dwWin32ExitCode);
		break;

	case SERVICE_CONTROL_SHUTDOWN:
		service->OnShutdown();
		service->ReportServiceStatus(service->service_status_.dwCurrentState, service->service_status_.dwWin32ExitCode);
		break;

	case SERVICE_CONTROL_POWEREVENT:
		if (dwEventType == PBT_POWERSETTINGCHANGE) {
			service->OnPowerEvent(PBT_POWERSETTINGCHANGE, reinterpret_cast<PPOWERBROADCAST_SETTING>(lpEventData));
		} else {
			service->OnPowerEvent(dwEventType, NULL);
		}
		service->ReportServiceStatus(service->service_status_.dwCurrentState, service->service_status_.dwWin32ExitCode);
		break;

	case SERVICE_CONTROL_SESSIONCHANGE:
		service->OnSessionChange(dwEventType, reinterpret_cast<PWTSSESSION_NOTIFICATION>(lpEventData));
		service->ReportServiceStatus(service->service_status_.dwCurrentState, service->service_status_.dwWin32ExitCode);
		break;

	default:
		if (dwControl >= 128 && dwControl <= 255) {
			service->OnCustomCommand(dwControl);
			service->ReportServiceStatus(service->service_status_.dwCurrentState, service->service_status_.dwWin32ExitCode);
		} else {
			dwRet = ERROR_CALL_NOT_IMPLEMENTED;
		}
		break;
	}

	return dwRet;
}

CServiceBase::CServiceBase(const std::wstring & service_name)
	: service_name_(service_name)
	, can_stop_(true)
	, can_shutdown_(false)
	, can_pause_and_continue_(false)
	, can_handle_power_event_(false)
	, can_handle_session_change_event_(false)
	, controls_accepted_(0) {
	Utils::Output(Utils::StringFormat(L"CServiceBase::CServiceBase %s", service_name_.c_str()));
	service_status_.dwServiceType = SERVICE_WIN32;
	service_status_.dwServiceSpecificExitCode = NO_ERROR;
}

CServiceBase::~CServiceBase() {
	Utils::Output(L"CServiceBase::~CServiceBase");
}

void CServiceBase::Run(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors) {
	Utils::Output(Utils::StringFormat(L"CServiceBase::Run dwNumServicesArgs: %u", dwNumServicesArgs));
	controls_accepted_ = UpdateControlsAccepted();
	ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 30000);
	OnStart(dwNumServicesArgs, lpServiceArgVectors);
	ReportServiceStatus(SERVICE_RUNNING);
	RunService();
	ReportServiceStatus(SERVICE_STOPPED);
}

void CServiceBase::ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {
	Utils::Output(Utils::StringFormat(L"CServiceBase::ReportServiceStatus dwCurrentState: %s, dwWin32ExitCode: %u, dwWaitHint: %u", GetServiceStatusString(dwCurrentState), dwWin32ExitCode, dwWaitHint));
	service_status_.dwCurrentState = dwCurrentState;
	service_status_.dwWin32ExitCode = dwWin32ExitCode;
	service_status_.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING) {
		service_status_.dwControlsAccepted = 0;
	} else {
		service_status_.dwControlsAccepted = controls_accepted_;
	}

	if (dwCurrentState == SERVICE_RUNNING || dwCurrentState == SERVICE_STOPPED) {
		service_status_.dwCheckPoint = 0;
	} else {
		service_status_.dwCheckPoint += 1;
	}

	::SetServiceStatus(service_status_handle_, &service_status_);
}

DWORD CServiceBase::UpdateControlsAccepted() {
	DWORD dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_POWEREVENT | SERVICE_ACCEPT_SESSIONCHANGE;
	if (!can_stop_) {
		dwControlsAccepted &= ~SERVICE_ACCEPT_STOP;
	}
	if (!can_pause_and_continue_) {
		dwControlsAccepted &= ~SERVICE_ACCEPT_PAUSE_CONTINUE;
	}
	if (!can_shutdown_) {
		dwControlsAccepted &= ~SERVICE_ACCEPT_SHUTDOWN;
	}
	if (!can_handle_power_event_) {
		dwControlsAccepted &= ~SERVICE_ACCEPT_POWEREVENT;
	}
	if (!can_handle_session_change_event_) {
		dwControlsAccepted &= ~SERVICE_ACCEPT_SESSIONCHANGE;
	}
	Utils::Output(Utils::StringFormat(L"CServiceBase::UpdateControlsAccepted dwControlsAccepted: %u", dwControlsAccepted));
	return dwControlsAccepted;
}

void CServiceBase::OnStart(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors) {
	Utils::Output(Utils::StringFormat(L"CServiceBase::OnStart dwNumServicesArgs: %u, lpServiceArgVectors: %p", dwNumServicesArgs, lpServiceArgVectors));
}

void CServiceBase::OnPause() {
	Utils::Output(L"CServiceBase::OnPause");
}

void CServiceBase::OnContinue() {
	Utils::Output(L"CServiceBase::OnContinue");
}

void CServiceBase::OnStop() {
	Utils::Output(L"CServiceBase::OnStop");
}

void CServiceBase::OnShutdown() {
	Utils::Output(L"CServiceBase::OnShutdown");
}

void CServiceBase::OnCustomCommand(DWORD command) {
	Utils::Output(Utils::StringFormat(L"CServiceBase::OnCustomCommand command: %u", command));
}

// 
// Event					Meaning
// 
// PBT_APMPOWERSTATUSCHANGE	Power status has changed.
// 10 (0xA)					检测到计算机电源状态的更改，如从电池电源切换到交流电源。 系统还会在剩余电池电量降至用户指定的阈值以下时或电池电量变化了指定的百分比时广播该事件。
// 
// PBT_APMRESUMEAUTOMATIC	Operation is resuming automatically from a low-power state. This message is sent every time the system resumes.
// 18 (0x12)				计算机已自动唤醒来处理事件。 如果系统在广播 ResumeAutomatic 后检测到任何用户活动，则会广播 ResumeSuspend 事件，使应用程序知道他们可以恢复与用户的完全交互。
// 
// PBT_APMRESUMESUSPEND		Operation is resuming from a low-power state. This message is sent after PBT_APMRESUMEAUTOMATIC if the resume is triggered by user input, such as pressing a key.
// 7 (0x7)					系统在挂起之后已恢复操作。
// 
// PBT_APMSUSPEND			System is suspending operation.
// 4 (0x4)					计算机将要进入挂起状态。 该事件通常在所有应用程序和可安装驱动程序已对上一个 true 状态返回 QuerySuspend 后广播。
// 
// PBT_POWERSETTINGCHANGE	A power setting change event has been received.
// 32787 (0x8013)
// 
void CServiceBase::OnPowerEvent(DWORD dwEventType, PPOWERBROADCAST_SETTING pSetting) {
	Utils::Output(Utils::StringFormat(L"CServiceBase::OnPowerEvent dwEventType: %s, pSetting: %p", GetPBTString(dwEventType), pSetting));
}

// 
// Event						Meaning
// 
// WTS_CONSOLE_CONNECT			The session identified by lParam was connected to the console terminal or RemoteFX session.
// (0x1)						控制台会话已连接。
// 
// WTS_CONSOLE_DISCONNECT		The session identified by lParam was disconnected from the console terminal or RemoteFX session.
// (0x2)						控制台会话已断开连接。
// 
// WTS_REMOTE_CONNECT			The session identified by lParam was connected to the remote terminal.
// (0x3)						远程会话已连接。
// 
// WTS_REMOTE_DISCONNECT		The session identified by lParam was disconnected from the remote terminal.
// (0x4)						远程会话已断开连接。
// 
// WTS_SESSION_LOGON			A user has logged on to the session identified by lParam.
// (0x5)						用户已登录到会话。
// 
// WTS_SESSION_LOGOFF			A user has logged off the session identified by lParam.
// (0x6)						用户已从会话注销。
// 
// WTS_SESSION_LOCK				The session identified by lParam has been locked.
// (0x7)						会话已被锁定。
// 
// WTS_SESSION_UNLOCK			The session identified by lParam has been unlocked.
// (0x8)						会话已解锁。
// 
// WTS_SESSION_REMOTE_CONTROL	The session identified by lParam has changed its remote controlled status. To determine the status, call GetSystemMetrics and check the SM_REMOTECONTROL metric.
// (0x9)						会话的远程控制状态已更改。
// 
// WTS_SESSION_CREATE			Reserved for future use.
// (0xA)
// 
// WTS_SESSION_TERMINATE		Reserved for future use.
// (0xB)
// 
void CServiceBase::OnSessionChange(DWORD dwEventType, PWTSSESSION_NOTIFICATION pNotification) {
	Utils::Output(Utils::StringFormat(L"CServiceBase::OnSessionChange dwEventType: %s, dwSessionId: %u", GetWTSString(dwEventType), pNotification->dwSessionId));
}

}
