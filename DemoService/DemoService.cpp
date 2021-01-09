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

	int status;
	uv_pipe_t * server = CreatePipe(&event_loop_);
	if ((status = uv_pipe_bind(server, PIPENAME))) {
		Utils::Output(Utils::StringFormat(L"绑定命名管道[%s] - Error: %s %s", Utils::AToW(PIPENAME), Utils::AToW(uv_err_name(status)), Utils::AToW(uv_strerror(status))));
		ClosePipe(server);
		return;
	}
	if ((status = uv_listen(reinterpret_cast<uv_stream_t *>(server), 128, CDemoService::UvConnectionCb))) {
		Utils::Output(Utils::StringFormat(L"监听命名管道[%s] - Error: %s %s", Utils::AToW(PIPENAME), Utils::AToW(uv_err_name(status)), Utils::AToW(uv_strerror(status))));
		ClosePipe(server);
	}
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

uv_pipe_t * CDemoService::CreatePipe(uv_loop_t * loop) {
	uv_pipe_t * handle = static_cast<uv_pipe_t *>(malloc(sizeof(uv_pipe_t)));
	uv_pipe_init(loop, handle, 0);
	handle->data = NULL;
	return handle;
}

void CDemoService::ClosePipe(uv_pipe_t * handle) {
	uv_close(reinterpret_cast<uv_handle_t *>(handle), CDemoService::UvCloseCb);
}

void CDemoService::UvWalkCb(uv_handle_t* handle, void* arg) {
	if (!uv_is_closing(handle)) {
		if (handle->type == UV_NAMED_PIPE) {
			ClosePipe(reinterpret_cast<uv_pipe_t *>(handle));
		} else {
			uv_close(handle, NULL);
		}
	}
}

void CDemoService::UvCloseCb(uv_handle_t* handle) {
	if (handle->data) {
		free(handle->data);
	}
	free(handle);
}

void CDemoService::UvConnectionCb(uv_stream_t* server, int status) {
	if (status == -1) {
		Utils::Output(Utils::StringFormat(L"监听命名管道[%s]回调 - Error: %s %s", Utils::AToW(PIPENAME), Utils::AToW(uv_err_name(status)), Utils::AToW(uv_strerror(status))));
		return;
	}

	uv_pipe_t * client = CreatePipe(server->loop);
	if ((status = uv_accept(server, reinterpret_cast<uv_stream_t *>(client)))) {
		Utils::Output(Utils::StringFormat(L"连接命名管道[%s] - Error: %s %s", Utils::AToW(PIPENAME), Utils::AToW(uv_err_name(status)), Utils::AToW(uv_strerror(status))));
		ClosePipe(client);
	} else {
		uv_read_start(reinterpret_cast<uv_stream_t *>(client), CDemoService::UvAllocCb, CDemoService::UvReadCb);
	}
}

void CDemoService::UvAllocCb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	buf->base = static_cast<char *>(malloc(suggested_size));
	buf->len = static_cast<ULONG>(suggested_size);
}

void CDemoService::UvReadCb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	if (nread < 0) {
		if (nread == UV_EOF) {
			Utils::Output(Utils::StringFormat(L"有名管道[%s] - 对方断开", Utils::AToW(PIPENAME)));
		} else {
			Utils::Output(Utils::StringFormat(L"有名管道[%s] - 接收报错: %s %s", Utils::AToW(PIPENAME), Utils::AToW(uv_err_name(nread)), Utils::AToW(uv_strerror(nread))));
		}
		ClosePipe(reinterpret_cast<uv_pipe_t *>(stream));
	} else if (nread > 0) {
		
	}

	free(buf->base);
}

}
