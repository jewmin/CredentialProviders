#pragma once

#include "uv.h"
#include "Header.h"
#include "Utils.h"
#include "ServiceBase.h"
#include "IService.h"

namespace Utils {

class CDemoService : public CServiceBase {
public:
	CDemoService(const std::wstring & service_name);
	virtual ~CDemoService();

protected:
	virtual void RunService();
	virtual void OnStart(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors);
	virtual void OnStop();
	virtual void OnShutdown();
	virtual void OnSessionChange(DWORD dwEventType, PWTSSESSION_NOTIFICATION pNotification);

private:
	static uv_pipe_t * CreatePipe(uv_loop_t * loop);
	static void ClosePipe(uv_pipe_t * handle);
	static void UvWalkCb(uv_handle_t* handle, void* arg);
	static void UvCloseCb(uv_handle_t* handle);
	static void UvConnectionCb(uv_stream_t* server, int status);
	static void UvAllocCb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static void UvReadCb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);

private:
	uv_loop_t event_loop_;
	uv_async_t event_notify_;
	IService * service_;
};

}
