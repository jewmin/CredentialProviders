#pragma once

#include "SessionProperties.h"

namespace Utils {

class IService {
public:
	// 初始化回调
	virtual void OnInited() = 0;
	// 登录回调
	virtual void OnLogon(SessionProperties session) = 0;
	// 注销回调
	virtual void OnLogoff(SessionProperties session) = 0;
	// 锁定回调
	virtual void OnLock(SessionProperties session) = 0;
	// 解锁回调
	virtual void OnUnLock(SessionProperties session) = 0;
	// 关机&重启回调
	virtual void OnShutdown() = 0;
};

}
