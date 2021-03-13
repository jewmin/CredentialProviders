#pragma once

#include "UvHandler.h"
#include "Thread.h"
#include "Protocol/LoginRequest.h"
#include "Protocol/LoginResponse.h"
#include <credentialprovider.h>
#include "CriticalSection.h"

// 管道客户端
class CSampleProvider;
class AuthClient : public Utils::UvHandler, Utils::CThread {
public:
	AuthClient(int timeout);
	virtual ~AuthClient();

	// 初始化
	HRESULT Initialize(CSampleProvider * pProvider);
	// 授权验证
	void Auth(Utils::Protocol::LoginRequest * request);

protected:
	// 协议处理
	virtual void ProcessCommand(uv_pipe_t * pipe, const Utils::CIOBuffer * buffer);
	// 连接成功回调
	virtual void OnConnected(uv_pipe_t * pipe, bool status);
	// 断开连接回调
	virtual void OnDisconnected(uv_pipe_t * pipe);
	// 线程函数
	virtual void OnRountine();
	// 发送授权请求
	void SendAuth();

private:
	static void TimerCb(uv_timer_t* handle);
	static void AsyncCb(uv_async_t* handle);

private:
	int timeout_;
	uv_pipe_t * pipe_;
	uv_timer_t reconnect_timer_;
	uv_async_t queue_notify_;
	CSampleProvider * pProvider_;
	Utils::Protocol::LoginRequest last_request_;
	Utils::CCriticalSection cs_;
	std::vector<Utils::Protocol::LoginRequest> request_queue_;
};
