#include "DemoService.h"
#include "MyService.h"
#include "SessionProperties.h"
#include "WindowsHelper.h"

namespace DemoService {

CDemoService::CDemoService(const std::wstring & service_name)
	: Utils::CServiceBase(service_name), service_(new MyService()) {
	can_shutdown_ = true;
	can_handle_session_change_event_ = true;
	can_handle_power_event_ = true;
}

CDemoService::~CDemoService() {
	if (service_) {
		delete service_;
	}
}

void CDemoService::RunService() {
	// 无限循环
	EventPoll();
	// 释放资源
	EventStopped();
}

void CDemoService::OnStart(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors) {
	Utils::Output(L"Starting Service");
	if (service_) {
		service_->OnInited();
	}
	// 管道监听
	EventInitServer();
}

void CDemoService::OnStop() {
	Utils::Output(L"Stopping Service");
	// 停止循环
	EventStop();
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
			service_->OnLogon(pNotification->dwSessionId);
		}
		break;

	case WTS_SESSION_LOGOFF:
		if (service_) {
			service_->OnLogoff(pNotification->dwSessionId);
		}
		break;

	case WTS_SESSION_LOCK:
		if (service_) {
			service_->OnLock(pNotification->dwSessionId);
		}
		break;

	case WTS_SESSION_UNLOCK:
		if (service_) {
			service_->OnUnLock(pNotification->dwSessionId);
		}
		break;
	}
}

void CDemoService::ProcessCommand(uv_pipe_t * pipe, const Utils::CIOBuffer * buffer) {
	Utils::Output(Utils::StringFormat(L"CDemoService::ProcessCommand pipe: %p, buffer: %p", pipe, buffer));
	const BYTE * data = buffer->GetBuffer();
	Utils::Protocol::CmdHeader ch = {0};
	memcpy(&ch, data, Utils::Protocol::Commond::CmdHeaderLen);
	switch (ch.wCmd) {
	case Utils::Protocol::Commond::RequestLogin:
		Utils::Protocol::LoginRequest * request = reinterpret_cast<Utils::Protocol::LoginRequest *>(const_cast<BYTE *>(data + Utils::Protocol::Commond::CmdHeaderLen));
		Utils::Protocol::LoginResponse response;
		// 授权验证
		service_->Auth(request, &response);
		// 返回授权结果
		Write(pipe, Utils::Protocol::Commond::ResponseLogin, reinterpret_cast<BYTE *>(&response), sizeof(response));
		break;
	}
}

}
