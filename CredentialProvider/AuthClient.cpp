#include "AuthClient.h"
#include "Utils.h"

AuthClient::AuthClient(DWORD timeout, IQueryContinueWithStatus* pqcws)
	: timeout_(timeout)
	, pqcws_(NULL) {
	Utils::Output(Utils::StringFormat(L"AuthClient::AuthClient timeout: %u, pqcws: %p", timeout, pqcws));
	if (pqcws) {
		pqcws_ = pqcws;
		pqcws_->AddRef();
	}
}

AuthClient::~AuthClient() {
	Utils::Output(L"AuthClient::~AuthClient");
	if (pqcws_) {
		pqcws_->Release();
		pqcws_ = NULL;
	}
}

void AuthClient::Auth(Utils::Protocol::LoginRequest * request, Utils::Protocol::LoginResponse * response) {
	Utils::Output(Utils::StringFormat(L"AuthClient::Auth request: %p, response: %p", request, response));
	if (pqcws_) {
		pqcws_->SetStatusMessage(Utils::StringFormat(L"用户[%s]授权验证中...", request->UserName).c_str());
	}

	memcpy(&request_, request, sizeof(Utils::Protocol::LoginRequest));
	EventInitClient();

	HRESULT hr;
	DWORD count = timeout_ / 16;
	while (count-- > 0 && response_.Result == Utils::Protocol::LoginResponse::Unknown) {
		if (pqcws_) {
			hr = pqcws_->QueryContinue();
			if (!SUCCEEDED(hr)) {
				response_.Result = Utils::Protocol::LoginResponse::UserCancel;
				break;
			}
		}
		EventPoll(UV_RUN_NOWAIT);
		Sleep(16);
	}

	if (response_.Result == Utils::Protocol::LoginResponse::Unknown) {
		response_.Result = Utils::Protocol::LoginResponse::Timeout;
	}
	memcpy(response, &response_, sizeof(Utils::Protocol::LoginResponse));
}

void AuthClient::ProcessCommand(uv_pipe_t * pipe, const Utils::CIOBuffer * buffer) {
	Utils::Output(Utils::StringFormat(L"AuthClient::ProcessCommand pipe: %p, buffer: %p", pipe, buffer));
	UNREFERENCED_PARAMETER(pipe);
	const BYTE * data = buffer->GetBuffer();
	Utils::Protocol::CmdHeader ch = { 0 };
	memcpy(&ch, data, Utils::Protocol::Commond::CmdHeaderLen);
	switch (ch.wCmd) {
	case Utils::Protocol::Commond::ResponseLogin:
		Utils::Protocol::LoginResponse * response = reinterpret_cast<Utils::Protocol::LoginResponse *>(const_cast<BYTE *>(data + Utils::Protocol::Commond::CmdHeaderLen));
		memcpy(&response_, response, sizeof(Utils::Protocol::LoginResponse));
		break;
	}
}

void AuthClient::OnConnected(uv_pipe_t * pipe, bool status) {
	Utils::Output(Utils::StringFormat(L"AuthClient::OnConnected pipe: %p, status: %s", pipe, status ? L"true" : L"false"));
	if (status) {
		Write(pipe, Utils::Protocol::Commond::RequestLogin, reinterpret_cast<BYTE *>(&request_), sizeof(request_));
	} else {
		response_.Result = Utils::Protocol::LoginResponse::ConnectFailed;
	}
}
