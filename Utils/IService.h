#pragma once

#include "SessionProperties.h"
#include "Protocol/LoginRequest.h"
#include "Protocol/LoginResponse.h"

namespace Utils {

class IService {
public:
	// 初始化回调
	virtual void OnInited() = 0;
	// 授权验证
	virtual Utils::Protocol::LoginResponse Auth(Utils::Protocol::LoginRequest request) = 0;
	// 登录回调
	virtual void OnLogon(DWORD SessionID) = 0;
	// 注销回调
	virtual void OnLogoff(DWORD SessionID) = 0;
	// 锁定回调
	virtual void OnLock(DWORD SessionID) = 0;
	// 解锁回调
	virtual void OnUnLock(DWORD SessionID) = 0;
	// 关机&重启回调
	virtual void OnShutdown() = 0;
};

}
