#pragma once

#include "UvHandler.h"
#include "Protocol/LoginRequest.h"
#include "Protocol/LoginResponse.h"
#include <credentialprovider.h>

class AuthClient : public Utils::UvHandler {
public:
	AuthClient(DWORD timeout, IQueryContinueWithStatus* pqcws);
	virtual ~AuthClient();

	void Auth(Utils::Protocol::LoginRequest * request, Utils::Protocol::LoginResponse * response);

protected:
	virtual void ProcessCommand(uv_pipe_t * pipe, const Utils::CIOBuffer * buffer);
	virtual void OnConnected(uv_pipe_t * pipe, bool status);

private:
	DWORD timeout_;
	IQueryContinueWithStatus * pqcws_;
	Utils::Protocol::LoginRequest request_;
	Utils::Protocol::LoginResponse response_;
};
