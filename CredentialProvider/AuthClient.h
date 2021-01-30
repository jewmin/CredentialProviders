#pragma once

#include "UvHandler.h"
#include "Protocol/LoginRequest.h"
#include "Protocol/LoginResponse.h"
#include <credentialprovider.h>

// 管道客户端
class AuthClient : public Utils::UvHandler {
public:
	AuthClient(DWORD timeout, IQueryContinueWithStatus* pqcws);
	virtual ~AuthClient();

	// 授权验证
	void Auth(Utils::Protocol::LoginRequest * request, Utils::Protocol::LoginResponse * response);

protected:
	// 协议处理
	virtual void ProcessCommand(uv_pipe_t * pipe, const Utils::CIOBuffer * buffer);
	// 连接成功回调
	virtual void OnConnected(uv_pipe_t * pipe, bool status);

private:
	DWORD timeout_;
	IQueryContinueWithStatus * pqcws_;
	Utils::Protocol::LoginRequest request_;
	Utils::Protocol::LoginResponse response_;
};
