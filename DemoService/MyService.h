#pragma once

#include "Header.h"
#include "IService.h"
#include "CriticalSection.h"

namespace DemoService {

class MyService : public Utils::IService {
public:
	typedef std::map<DWORD, Utils::SessionProperties *> SessionPropertiesDict;
	typedef std::map<DWORD, Utils::SessionProperties *>::iterator SessionPropertiesDictIter;
	MyService();
	~MyService();

	// 初始化回调
	virtual void OnInited();
	// 授权验证
	virtual void Auth(Utils::Protocol::LoginRequest * request, Utils::Protocol::LoginResponse * response);
	// 登录回调
	virtual void OnLogon(DWORD SessionID);
	// 注销回调
	virtual void OnLogoff(DWORD SessionID);
	// 锁定回调
	virtual void OnLock(DWORD SessionID);
	// 解锁回调
	virtual void OnUnLock(DWORD SessionID);
	// 关机&重启回调
	virtual void OnShutdown();

private:
	Utils::SessionProperties last_auth_user_;
	SessionPropertiesDict sessions_dict_;
	Utils::CCriticalSection cs_;
};

}
