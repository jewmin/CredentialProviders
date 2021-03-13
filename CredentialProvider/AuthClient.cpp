#include "AuthClient.h"
#include "Utils.h"
#include "CSampleProvider.h"

AuthClient::AuthClient(int timeout)
	: timeout_(timeout)
	, pipe_(NULL)
	, pProvider_(NULL) {
	Utils::Output(Utils::StringFormat(L"AuthClient::AuthClient timeout: %d", timeout));
	uv_timer_init(&event_loop_, &reconnect_timer_);
	reconnect_timer_.data = this;
	uv_async_init(&event_loop_, &queue_notify_, AuthClient::AsyncCb);
	queue_notify_.data = this;
}

AuthClient::~AuthClient() {
	Utils::Output(L"AuthClient::~AuthClient");
	// We'll also make sure to release any reference we have to the provider.
	if (pProvider_ != NULL) {
		pProvider_->Release();
		pProvider_ = NULL;
	}
	EventStop();
}

HRESULT AuthClient::Initialize(CSampleProvider * pProvider) {
	Utils::Output(Utils::StringFormat(L"AuthClient::Initialize pProvider: %p", pProvider));
	HRESULT hr = S_OK;

	// Be sure to add a release any existing provider we might have, then add a reference
	// to the provider we're working with now.
	if (pProvider_ != NULL) {
		pProvider_->Release();
	}
	pProvider_ = pProvider;
	pProvider_->AddRef();

	Start();

	return hr;
}

void AuthClient::Auth(Utils::Protocol::LoginRequest * request) {
	Utils::Output(Utils::StringFormat(L"AuthClient::Auth request: %p", request));
	Utils::CCriticalSection::Owner lock(cs_);
	request_queue_.push_back(*request);
	uv_async_send(&queue_notify_);
}

void AuthClient::ProcessCommand(uv_pipe_t * pipe, const Utils::CIOBuffer * buffer) {
	Utils::Output(Utils::StringFormat(L"AuthClient::ProcessCommand pipe: %p, buffer: %p", pipe, buffer));
	UNREFERENCED_PARAMETER(pipe);
	const BYTE * data = buffer->GetBuffer();
	Utils::Protocol::CmdHeader ch = { 0 };
	memcpy(&ch, data, Utils::Protocol::Commond::CmdHeaderLen);
	switch (ch.wCmd) {
	// 密码登录
	case Utils::Protocol::Commond::ResponseLogin:
		{
			Utils::Protocol::LoginResponse * response = reinterpret_cast<Utils::Protocol::LoginResponse *>(const_cast<BYTE *>(data + Utils::Protocol::Commond::CmdHeaderLen));
			// 序号必须一样
			if (response->Sn == last_request_.Sn) {
				pProvider_->OnResponse(true, response);
			}
		}
		break;

	// 扫码登录
	case Utils::Protocol::Commond::ResponseQCLogin:
		{
			Utils::Protocol::LoginResponse * response = reinterpret_cast<Utils::Protocol::LoginResponse *>(const_cast<BYTE *>(data + Utils::Protocol::Commond::CmdHeaderLen));
			pProvider_->OnResponse(false, response);
		}
		break;
	}
}

void AuthClient::OnConnected(uv_pipe_t * pipe, bool status) {
	Utils::Output(Utils::StringFormat(L"AuthClient::OnConnected pipe: %p, status: %s", pipe, status ? L"true" : L"false"));
	if (status) {
		pipe_ = pipe;
	} else {
		pipe_ = NULL;
		uv_timer_start(&reconnect_timer_, AuthClient::TimerCb, timeout_, 0);  // 断线重连
	}
}

void AuthClient::OnDisconnected(uv_pipe_t * pipe) {
	Utils::Output(Utils::StringFormat(L"AuthClient::OnDisconnected pipe: %p", pipe));
	pipe_ = NULL;
	uv_timer_start(&reconnect_timer_, AuthClient::TimerCb, timeout_, 0);  // 断线重连
}

void AuthClient::OnRountine() {
	Utils::Output(L"AuthClient::OnRountine");
	EventInitClient();
	EventPoll();
	EventStopped();
}

void AuthClient::SendAuth() {
	// 只发送最后一个请求
	Utils::CCriticalSection::Owner lock(cs_);
	if (request_queue_.size() > 0) {
		Utils::Protocol::LoginRequest & request = request_queue_.back();
		memcpy(&last_request_, &request, sizeof(Utils::Protocol::LoginRequest));
		int status = Write(pipe_, Utils::Protocol::Commond::RequestLogin, reinterpret_cast<BYTE *>(&last_request_), sizeof(last_request_));
		if (status < 0) {
			Utils::Protocol::LoginResponse response;
			response.Result = Utils::Protocol::LoginResponse::AuthFailed;
			pProvider_->OnResponse(true, &response);
		}
		request_queue_.clear();
	}
}

void AuthClient::TimerCb(uv_timer_t* handle) {
	Utils::Output(Utils::StringFormat(L"AuthClient::TimerCb handle: %p", handle));
	AuthClient * client = static_cast<AuthClient *>(handle->data);
	client->StartConnectPipe();
}

void AuthClient::AsyncCb(uv_async_t* handle) {
	Utils::Output(Utils::StringFormat(L"AuthClient::AsyncCb handle: %p", handle));
	AuthClient * client = static_cast<AuthClient *>(handle->data);
	client->SendAuth();
}
