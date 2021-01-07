#pragma once

namespace Utils {

class IService {
public:
	// 初始化回调
	virtual void OnInited() = 0;
	// 登录回调
	virtual void OnLogon() = 0;
	// 注销回调
	virtual void OnLogoff() = 0;
	// 锁定回调
	virtual void OnLock() = 0;
	// 解锁回调
	virtual void OnUnLock() = 0;
	// 关机&重启回调
	virtual void OnShutdown() = 0;
};

}
