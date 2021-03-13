#include "UvHandler.h"
#include "Utils.h"
#include "Protocol/Commond.h"
#include "Protocol/LoginRequest.h"
#include "Protocol/LoginResponse.h"

namespace Utils {

UvHandler::UvHandler()
	: Utils::CIOBuffer::Allocator(4096, 0) {
	Utils::Output(Utils::StringFormat(L"<libuv> %s", Utils::AToW(uv_version_string()).c_str()));
	uv_loop_init(&event_loop_);
	event_loop_.data = this;
}

UvHandler::~UvHandler() {
	Utils::Output(L"UvHandler::~UvHandler");
	uv_loop_close(&event_loop_);
}

void UvHandler::PreWrite(Utils::CIOBuffer * buffer, WORD wCmd) {
	Utils::Output(Utils::StringFormat(L"UvHandler::PreWrite buffer: %p, wCmd: %u", buffer, wCmd));
	if (buffer) {
		Utils::Protocol::CmdHeader ch = {0};
		ch.cCmdBeginFlag = Utils::Protocol::Commond::CmdBeginFlag;
		ch.cCmdEndFlag = Utils::Protocol::Commond::CmdEndFlag;
		ch.wCmd = wCmd;
		ch.wCRC = Utils::Protocol::Commond::MakeCrc(wCmd);
		buffer->AddData(reinterpret_cast<const BYTE *>(&ch), Utils::Protocol::Commond::CmdHeaderLen);
	}
}

int UvHandler::Write(uv_pipe_t * pipe, WORD wCmd, const BYTE * data, size_t len) {
	Utils::Output(Utils::StringFormat(L"UvHandler::Write pipe: %p, wCmd: %u, data: %p, len: %zu", pipe, wCmd, data, len));
	if (!pipe || !data || len == 0) {
		return UV_ECONNRESET;
	}

	Utils::CIOBuffer * buffer = Allocate();
	PreWrite(buffer, wCmd);
	buffer->AddData(data, len);
	buffer->SetupWrite();

	uv_write_t * req = static_cast<uv_write_t *>(malloc(sizeof(uv_write_t)));
	req->data = buffer;
	int status = uv_write(req, reinterpret_cast<uv_stream_t *>(pipe), buffer->GetUvBuf(), 1, UvHandler::UvWriteCb);
	if (status < 0) {
		Utils::Output(Utils::StringFormat(L"命名管道[%s] - 发送报错: %s %s", Utils::AToW(PIPENAME).c_str(), Utils::AToW(uv_err_name(status)).c_str(), Utils::AToW(uv_strerror(status)).c_str()));
		free(req);
		buffer->Release();
		ClosePipe(pipe);
		return status;
	}

	return 0;
}

// ##################################### 协议处理 #####################################

void UvHandler::ReadCompleted(uv_pipe_t * pipe, Utils::CIOBuffer * buffer) {
	Utils::Output(Utils::StringFormat(L"UvHandler::ReadCompleted pipe: %p, buffer: %p", pipe, buffer));
	try {
		bool done;
		do {
			done = true;
			const size_t used = buffer->GetUsed();
			if (used >= GetMinimumMessageSize()) {
				const size_t message_size = GetMessageSize(buffer);
				if (0 == message_size) {
					ClosePipe(pipe);
					buffer->Empty();
				} else if (used == message_size) {
					ProcessCommand(pipe, buffer);
					buffer->Empty();
				} else if (used > message_size) {
					Utils::CIOBuffer * message = buffer->SplitBuffer(message_size);
					ProcessCommand(pipe, message);
					message->Release();
					done = false;
				}
			}
		} while (!done);
	} catch (...) {
		ClosePipe(pipe);
	}
}

size_t UvHandler::GetMinimumMessageSize() const {
	Utils::Output(L"UvHandler::GetMinimumMessageSize");
	return Utils::Protocol::Commond::CmdHeaderLen;
}

size_t UvHandler::GetMessageSize(const Utils::CIOBuffer * buffer) const {
	Utils::Output(Utils::StringFormat(L"UvHandler::GetMessageSize buffer: %p", buffer));
	const BYTE * data = buffer->GetBuffer();
	Utils::Protocol::CmdHeader ch = {0};
	memcpy(&ch, data, Utils::Protocol::Commond::CmdHeaderLen);
	if (Utils::Protocol::Commond::CmdBeginFlag == ch.cCmdBeginFlag && Utils::Protocol::Commond::CmdEndFlag == ch.cCmdEndFlag) {
		WORD wCRC = Utils::Protocol::Commond::MakeCrc(ch.wCmd);
		if (wCRC == ch.wCRC) {
			switch (ch.wCmd) {
			case Utils::Protocol::Commond::RequestLogin:
				return Utils::Protocol::Commond::CmdHeaderLen + sizeof(Utils::Protocol::LoginRequest);

			case Utils::Protocol::Commond::ResponseLogin:
			case Utils::Protocol::Commond::ResponseQCLogin:
				return Utils::Protocol::Commond::CmdHeaderLen + sizeof(Utils::Protocol::LoginResponse);
			}
		}
	}

	return 0;
}

void UvHandler::OnConnected(uv_pipe_t * pipe, bool status) {
	UNREFERENCED_PARAMETER(pipe);
	UNREFERENCED_PARAMETER(status);
}

void UvHandler::OnDisconnected(uv_pipe_t * pipe) {
	UNREFERENCED_PARAMETER(pipe);
}

// ####################################################################################

int UvHandler::EventPoll(uv_run_mode mode) {
	return uv_run(&event_loop_, mode);
}

void UvHandler::EventStop() {
	Utils::Output(L"UvHandler::EventStop");
	uv_stop(&event_loop_);
	uv_async_send(&event_notify_);
}

void UvHandler::EventStopped() {
	Utils::Output(L"UvHandler::EventStopped");
	uv_walk(&event_loop_, UvHandler::UvWalkCb, NULL);
	while (uv_run(&event_loop_, UV_RUN_NOWAIT) > 0) {
		uv_run(&event_loop_, UV_RUN_ONCE);
	}
}

void UvHandler::EventInitServer() {
	Utils::Output(L"UvHandler::EventInitServer");
	uv_async_init(&event_loop_, &event_notify_, NULL);
	uv_pipe_t * server = CreatePipe(&event_loop_);
	int status = uv_pipe_bind(server, PIPENAME);
	if (status < 0) {
		Utils::Output(Utils::StringFormat(L"命名管道[%s] - 绑定报错: %s %s", Utils::AToW(PIPENAME).c_str(), Utils::AToW(uv_err_name(status)).c_str(), Utils::AToW(uv_strerror(status)).c_str()));
		ClosePipe(server);
		return;
	}
	status = uv_listen(reinterpret_cast<uv_stream_t *>(server), 128, UvHandler::UvConnectionCb);
	if (status < 0) {
		Utils::Output(Utils::StringFormat(L"命名管道[%s] - 监听报错: %s %s", Utils::AToW(PIPENAME).c_str(), Utils::AToW(uv_err_name(status)).c_str(), Utils::AToW(uv_strerror(status)).c_str()));
		ClosePipe(server);
	}
}

void UvHandler::EventInitClient() {
	Utils::Output(L"UvHandler::EventInitClient");
	uv_async_init(&event_loop_, &event_notify_, NULL);
	StartConnectPipe();
}

void UvHandler::StartConnectPipe() {
	Utils::Output(L"UvHandler::StartConnectPipe");
	uv_pipe_t * client = CreatePipe(&event_loop_);
	uv_connect_t * req = static_cast<uv_connect_t *>(malloc(sizeof(uv_connect_t)));
	uv_pipe_connect(req, client, PIPENAME, UvHandler::UvConnectCb);
}

uv_pipe_t * UvHandler::CreatePipe(uv_loop_t * loop) {
	Utils::Output(Utils::StringFormat(L"UvHandler::CreatePipe loop: %p", loop));
	uv_pipe_t * handle = static_cast<uv_pipe_t *>(malloc(sizeof(uv_pipe_t)));
	uv_pipe_init(loop, handle, 0);
	handle->data = NULL;
	return handle;
}

void UvHandler::ClosePipe(uv_pipe_t * handle) {
	Utils::Output(Utils::StringFormat(L"UvHandler::ClosePipe handle: %p", handle));
	uv_close(reinterpret_cast<uv_handle_t *>(handle), UvHandler::UvCloseCb);
}

// ##################################### 回调处理 #####################################

void UvHandler::UvWalkCb(uv_handle_t* handle, void* arg) {
	Utils::Output(Utils::StringFormat(L"UvHandler::UvWalkCb handle: %p, arg: %p", handle, arg));
	UNREFERENCED_PARAMETER(arg);
	if (!uv_is_closing(handle)) {
		if (handle->type == UV_NAMED_PIPE) {
			ClosePipe(reinterpret_cast<uv_pipe_t *>(handle));
		} else {
			uv_close(handle, NULL);
		}
	}
}

void UvHandler::UvCloseCb(uv_handle_t* handle) {
	Utils::Output(Utils::StringFormat(L"UvHandler::UvCloseCb handle: %p", handle));
	UvHandler * service = static_cast<UvHandler *>(handle->loop->data);
	service->OnDisconnected(reinterpret_cast<uv_pipe_t *>(handle));
	if (handle->data) {
		Utils::CIOBuffer * buffer = static_cast<Utils::CIOBuffer *>(handle->data);
		buffer->Release();
	}
	free(handle);
}

void UvHandler::UvConnectionCb(uv_stream_t* server, int status) {
	Utils::Output(Utils::StringFormat(L"UvHandler::UvConnectionCb server: %p, status: %d", server, status));
	if (status < 0) {
		Utils::Output(Utils::StringFormat(L"命名管道[%s] - 监听报错: %s %s", Utils::AToW(PIPENAME).c_str(), Utils::AToW(uv_err_name(status)).c_str(), Utils::AToW(uv_strerror(status)).c_str()));
		return;
	}

	uv_pipe_t * client = CreatePipe(server->loop);
	status = uv_accept(server, reinterpret_cast<uv_stream_t *>(client));
	if (status < 0) {
		Utils::Output(Utils::StringFormat(L"命名管道[%s] - 连接报错: %s %s", Utils::AToW(PIPENAME).c_str(), Utils::AToW(uv_err_name(status)).c_str(), Utils::AToW(uv_strerror(status)).c_str()));
		UvHandler * service = static_cast<UvHandler *>(client->loop->data);
		service->OnConnected(client, false);
		ClosePipe(client);
	} else {
		UvHandler * service = static_cast<UvHandler *>(client->loop->data);
		client->data = service->Allocate();
		uv_read_start(reinterpret_cast<uv_stream_t *>(client), UvHandler::UvAllocCb, UvHandler::UvReadCb);
		service->OnConnected(client, true);
	}
}

void UvHandler::UvConnectCb(uv_connect_t* req, int status) {
	Utils::Output(Utils::StringFormat(L"UvHandler::UvConnectCb req: %p, status: %d", req, status));
	if (status < 0) {
		Utils::Output(Utils::StringFormat(L"命名管道[%s] - 连接报错: %s %s", Utils::AToW(PIPENAME).c_str(), Utils::AToW(uv_err_name(status)).c_str(), Utils::AToW(uv_strerror(status)).c_str()));
		UvHandler * service = static_cast<UvHandler *>(req->handle->loop->data);
		service->OnConnected(reinterpret_cast<uv_pipe_t *>(req->handle), false);
		ClosePipe(reinterpret_cast<uv_pipe_t *>(req->handle));
	} else {
		UvHandler * service = static_cast<UvHandler *>(req->handle->loop->data);
		req->handle->data = service->Allocate();
		uv_read_start(reinterpret_cast<uv_stream_t *>(req->handle), UvHandler::UvAllocCb, UvHandler::UvReadCb);
		service->OnConnected(reinterpret_cast<uv_pipe_t *>(req->handle), true);
	}
	free(req);
}

void UvHandler::UvAllocCb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	Utils::Output(Utils::StringFormat(L"UvHandler::UvAllocCb handle: %p, suggested_size: %zu, buf: %p", handle, suggested_size, buf));
	UNREFERENCED_PARAMETER(suggested_size);
	Utils::CIOBuffer * buffer = static_cast<Utils::CIOBuffer *>(handle->data);
	buffer->AddRef();
	buffer->SetupRead();
	memcpy(buf, buffer->GetUvBuf(), sizeof(uv_buf_t));
}

void UvHandler::UvReadCb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	Utils::Output(Utils::StringFormat(L"UvHandler::UvReadCb stream: %p, nread: %zd, buf: %p", stream, nread, buf));
	UNREFERENCED_PARAMETER(buf);
	Utils::CIOBuffer * buffer = static_cast<Utils::CIOBuffer *>(stream->data);

	if (nread < 0) {
		if (nread == UV_EOF) {
			Utils::Output(Utils::StringFormat(L"命名管道[%s] - 对方断开", Utils::AToW(PIPENAME).c_str()));
		} else {
			Utils::Output(Utils::StringFormat(L"命名管道[%s] - 接收报错: %s %s", Utils::AToW(PIPENAME).c_str(), Utils::AToW(uv_err_name(static_cast<int>(nread))).c_str(), Utils::AToW(uv_strerror(static_cast<int>(nread))).c_str()));
		}
		ClosePipe(reinterpret_cast<uv_pipe_t *>(stream));
	} else if (nread > 0) {
		buffer->Use(nread);
		UvHandler * service = static_cast<UvHandler *>(stream->loop->data);
		service->ReadCompleted(reinterpret_cast<uv_pipe_t *>(stream), buffer);
	}

	buffer->Release();
}

void UvHandler::UvWriteCb(uv_write_t* req, int status) {
	Utils::Output(Utils::StringFormat(L"UvHandler::UvWriteCb req: %p, status: %d", req, status));
	Utils::CIOBuffer * buffer = static_cast<Utils::CIOBuffer *>(req->data);

	if (status < 0) {
		Utils::Output(Utils::StringFormat(L"命名管道[%s] - 发送报错: %s %s", Utils::AToW(PIPENAME).c_str(), Utils::AToW(uv_err_name(status)).c_str(), Utils::AToW(uv_strerror(status)).c_str()));
		ClosePipe(reinterpret_cast<uv_pipe_t *>(req->handle));
	}

	free(req);
	buffer->Release();
}

}
