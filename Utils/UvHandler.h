#pragma once

#include "Header.h"
#include "IOBuffer.h"

namespace Utils {

class UvHandler : private Utils::CIOBuffer::Allocator {
public:
	virtual ~UvHandler();

protected:
	UvHandler();
	void PreWrite(Utils::CIOBuffer * buffer, WORD wCmd);
	void Write(uv_pipe_t * pipe, WORD wCmd, const BYTE * data, size_t len);

	void ReadCompleted(uv_pipe_t * pipe, Utils::CIOBuffer * buffer);
	size_t GetMinimumMessageSize() const;
	size_t GetMessageSize(const Utils::CIOBuffer * buffer) const;
	virtual void ProcessCommand(uv_pipe_t * pipe, const Utils::CIOBuffer * buffer) = 0;

protected:
	int EventPoll(uv_run_mode mode = UV_RUN_DEFAULT);
	void EventStop();
	void OnEventStop();
	void EventInitServer();

	static uv_pipe_t * CreatePipe(uv_loop_t * loop);
	static void ClosePipe(uv_pipe_t * handle);

private:
	static void UvWalkCb(uv_handle_t* handle, void* arg);
	static void UvCloseCb(uv_handle_t* handle);
	static void UvConnectionCb(uv_stream_t* server, int status);
	static void UvAllocCb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static void UvReadCb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void UvWriteCb(uv_write_t* req, int status);

private:
	uv_loop_t event_loop_;
	uv_async_t event_notify_;
};

}
