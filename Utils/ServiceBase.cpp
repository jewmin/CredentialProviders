#include "ServiceBase.h"
#include "Utils.h"

namespace Utils {

void WINAPI ServiceMain(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors) {
	CServiceBase * service = new;

	// ServiceInstance->service_status_.dwServiceType = SERVICE_WIN32;
	// ServiceInstance->service_status_.dwCurrentState = SERVICE_START_PENDING;
	// ServiceInstance->service_status_.dwControlsAccepted = ServiceInstance->GetControlsAccepted(SERVICE_START_PENDING);
	// ServiceInstance->service_status_.dwWin32ExitCode = NO_ERROR;
	// ServiceInstance->service_status_.dwServiceSpecificExitCode = NO_ERROR;
	// ServiceInstance->service_status_.dwCheckPoint = 0;
	// ServiceInstance->service_status_.dwWaitHint = 0;

	// ServiceInstance->service_status_handle_ = ::RegisterServiceCtrlHandler();
	// if (ServiceInstance->service_status_handle_)


	// m_ServiceStatus.dwServiceType = SERVICE_WIN32;
	// m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	// //m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;
	// m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	// m_ServiceStatus.dwWin32ExitCode = 0;
	// m_ServiceStatus.dwServiceSpecificExitCode = 0;
	// m_ServiceStatus.dwCheckPoint = 0;
	// m_ServiceStatus.dwWaitHint = 0;

	// m_ServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler); 
	// if (m_ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
	// {
	// 	return;
	// }

	// serviceStatus.dwServiceType = SERVICE_WIN32;
    // serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    // serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
    // serviceStatus.dwWin32ExitCode = NO_ERROR;
    // serviceStatus.dwServiceSpecificExitCode = NO_ERROR;
    // serviceStatus.dwCheckPoint = 0;
    // serviceStatus.dwWaitHint = 0;

    // serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, ServiceControlHandler);

}

CServiceBase::CServiceBase(const std::wstring & service_name)
	: service_name_(service_name)
	, can_stop_(true)
	, can_shutdown_(false)
	, can_pause_and_continue_(false)
	, can_handle_power_event_(false)
	, can_handle_session_change_event_(false) {
	log_.SetLogFileNameFormat(L"Service_%02d%02d%02d.log");
	SetLog(&log_);
}

CServiceBase::~CServiceBase() {
	SetLog(NULL);
}

bool CServiceBase::Run(CServiceBase * service) {
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

DWORD CServiceBase::GetControlsAccepted(DWORD dwCurrentState) {
	DWORD dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_POWEREVENT | SERVICE_ACCEPT_SESSIONCHANGE;
	switch (dwCurrentState) {
	case SERVICE_START_PENDING:
		return UpdateControlsAccepted(dwControlsAccepted);

	case SERVICE_STOPPED:
	case SERVICE_STOP_PENDING:
	case SERVICE_RUNNING:
	case SERVICE_CONTINUE_PENDING:
	case SERVICE_PAUSE_PENDING:
	case SERVICE_PAUSED:
	};
}

DWORD CServiceBase::UpdateControlsAccepted(DWORD dwControlsAccepted) {
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
	return dwControlsAccepted;
}

}
