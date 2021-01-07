#pragma once

#include "Header.h"
#include "IService.h"
#include "CriticalSection.h"

namespace Utils {

class MyService : public IService {
public:
	MyService();
	~MyService();

	// 初始化回调
	virtual void OnInited();
	// 登录回调
	virtual void OnLogon(SessionProperties session);
	// 注销回调
	virtual void OnLogoff(SessionProperties session);
	// 锁定回调
	virtual void OnLock(SessionProperties session);
	// 解锁回调
	virtual void OnUnLock(SessionProperties session);
	// 关机&重启回调
	virtual void OnShutdown();

private:
	std::map<DWORD, SessionProperties *> sessions_map_;
	CCriticalSection cs_;
};

}
