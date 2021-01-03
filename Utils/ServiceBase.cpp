#include "ServiceBase.h"
#include "Utils.h"

namespace Utils {

CServiceBase * CServiceBase::ServiceInstance = NULL;

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

VOID WINAPI CServiceBase::ServiceMain(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors) {
	assert(NULL != ServiceInstance);
	ServiceInstance->service_status_handle_ = ::RegisterServiceCtrlHandlerEx(ServiceInstance->service_name_.c_str(), CServiceBase::ServiceCtrlHandler, reinterpret_cast<LPVOID>(ServiceInstance));
	if (ServiceInstance->service_status_handle_) {
		ServiceInstance->Run(dwNumServicesArgs, lpServiceArgVectors);
	}
	delete ServiceInstance;
}

DWORD WINAPI CServiceBase::ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) {

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

	service_status_.dwServiceType = SERVICE_WIN32;
	service_status_.dwCurrentState = SERVICE_START_PENDING;
	service_status_.dwControlsAccepted = GetControlsAccepted(SERVICE_START_PENDING);
	service_status_.dwWin32ExitCode = NO_ERROR;
	service_status_.dwServiceSpecificExitCode = NO_ERROR;
	service_status_.dwCheckPoint = 0;
	service_status_.dwWaitHint = 0;
}

CServiceBase::~CServiceBase() {
	SetLog(NULL);
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
