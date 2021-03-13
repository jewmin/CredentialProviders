#include "DemoService.h"
#include "MyService.h"
#include "SessionProperties.h"
#include "WindowsHelper.h"

namespace DemoService {

CDemoService::CDemoService(const std::wstring & service_name)
	: Utils::CServiceBase(service_name), service_(new MyService()), qc_pipe_(NULL) {
	can_shutdown_ = true;
	can_handle_session_change_event_ = true;
	can_handle_power_event_ = true;
	can_pause_and_continue_ = true;
}

CDemoService::~CDemoService() {
	if (service_) {
		delete service_;
	}
}

void CDemoService::RunService() {
	// ����ѭ��
	EventPoll();
	// �ͷ���Դ
	EventStopped();
}

void CDemoService::OnStart(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors) {
	Utils::Output(L"Starting Service");
	if (service_) {
		service_->OnInited();
	}
	// �ܵ�����
	EventInitServer();
}

void CDemoService::OnContinue() {
	// =============================== ���Դ��룬������ɾ����ɨ���¼ =================
	static int times = 0;
	Utils::Protocol::LoginResponse response_qc;
	response_qc.Result = Utils::Protocol::LoginResponse::AuthSuccess;
	wcscpy_s(response_qc.UserName, L"admin");
	if (times++ % 2 == 0) {
		wcscpy_s(response_qc.Password, L"test");
		Utils::Output(L"����ɨ���¼ admin/test");
	} else {
		wcscpy_s(response_qc.Password, L"admin");
		Utils::Output(L"����ɨ���¼ admin/admin");
	}
	Write(qc_pipe_, Utils::Protocol::Commond::ResponseQCLogin, reinterpret_cast<BYTE *>(&response_qc), sizeof(response_qc));
	// ===========================================================================
}

void CDemoService::OnStop() {
	Utils::Output(L"Stopping Service");
	// ֹͣѭ��
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
		// ��Ȩ��֤
		service_->Auth(request, &response);
		// ������Ȩ���
		Write(pipe, Utils::Protocol::Commond::ResponseLogin, reinterpret_cast<BYTE *>(&response), sizeof(response));
		// =============================== ���Դ��룬������ɾ����ɨ���¼ =================
		qc_pipe_ = pipe;
		// ===========================================================================
		break;
	}
}

}
