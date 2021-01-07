#include "DemoService.h"
#include "MyService.h"
#include "SessionProperties.h"
#include "WindowsHelper.h"

namespace Utils {

CDemoService::CDemoService(const std::wstring & service_name)
	: CServiceBase(service_name), service_(new MyService()) {
	can_shutdown_ = true;
	can_handle_session_change_event_ = true;
	Utils::Output(Utils::StringFormat(L"<libuv> %s", Utils::AToW(uv_version_string()).c_str()));
	uv_loop_init(&event_loop_);
	event_loop_.data = this;
}

CDemoService::~CDemoService() {
	uv_loop_close(&event_loop_);
	if (service_) {
		delete service_;
	}
}

void CDemoService::RunService() {
	uv_run(&event_loop_, UV_RUN_DEFAULT);
	uv_walk(&event_loop_, CDemoService::UvWalkCb, NULL);
	while (uv_run(&event_loop_, UV_RUN_NOWAIT) > 0) {
		uv_run(&event_loop_, UV_RUN_ONCE);
	}
}

void CDemoService::OnStart(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors) {
	Utils::Output(L"Starting Service");
	if (service_) {
		service_->OnInited();
	}
	uv_async_init(&event_loop_, &event_notify_, NULL);
}

void CDemoService::OnStop() {
	Utils::Output(L"Stopping Service");
	uv_stop(&event_loop_);
	uv_async_send(&event_notify_);
}

void CDemoService::OnShutdown() {
	OnStop();
	if (service_) {
		service_->OnShutdown();
	}
}

void CDemoService::OnSessionChange(DWORD dwEventType, PWTSSESSION_NOTIFICATION pNotification) {
	switch (dwEventType) {
	case WTS_SESSION_LOGON:
		if (service_) {
			service_->OnLogon(SessionProperties(pNotification->dwSessionId, Utils::GetSessionUserName(pNotification->dwSessionId)));
		}
		break;

	case WTS_SESSION_LOGOFF:
		if (service_) {
			service_->OnLogoff(SessionProperties(pNotification->dwSessionId));
		}
		break;

	case WTS_SESSION_LOCK:
		if (service_) {
			service_->OnLock(SessionProperties(pNotification->dwSessionId));
		}
		break;

	case WTS_SESSION_UNLOCK:
		if (service_) {
			service_->OnUnLock(SessionProperties(pNotification->dwSessionId));
		}
		break;
	}
}

void CDemoService::UvWalkCb(uv_handle_t* handle, void* arg) {
	if (!uv_is_closing(handle)) {
		uv_close(handle, NULL);
	}
}

}
