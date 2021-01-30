#pragma once

#include "Header.h"

namespace Utils {

// 服务程序基类
class CServiceBase {
public:
	CServiceBase(const std::wstring & service_name);
	virtual ~CServiceBase();
	// 启动服务程序，一般在main函数中调用
	static bool Run(CServiceBase * service);
	// 服务程序入口，类似main函数，在主线程，需要无限循环，保证服务不停止
	static VOID WINAPI ServiceMain(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors);
	// 服务程序消息处理，此是由windows调用，在子线程
	static DWORD WINAPI ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

	// 安装服务
	static bool InstallService(const std::wstring & service_name, const std::wstring & service_display_name, const std::wstring & service_description, const std::wstring & service_binary_path, DWORD dwStartType = SERVICE_AUTO_START);
	// 卸载服务
	static bool UnInstallService(const std::wstring & service_name);

protected:
	// 服务启动时回调
	virtual void OnStart(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors);
	// 服务暂停时回调
	virtual void OnPause();
	// 服务恢复时回调
	virtual void OnContinue();
	// 服务停止时回调
	virtual void OnStop();
	// 系统关闭时回调
	virtual void OnShutdown();
	// 自定义指令回调，一般不需要用到
	virtual void OnCustomCommand(DWORD command);
	// 系统休眠时回调
	virtual void OnPowerEvent(DWORD dwEventType, PPOWERBROADCAST_SETTING pSetting);
	// 会话登录、锁定、注销、关机时回调
	virtual void OnSessionChange(DWORD dwEventType, PWTSSESSION_NOTIFICATION pNotification);

private:
	void Run(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors);
	// 子类必须重写，在这里做无限循环
	virtual void RunService() = 0;
	// 报告给服务管理器，当前服务的状态
	void ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode = NO_ERROR, DWORD dwWaitHint = 0);
	// 设置服务操作权限
	DWORD UpdateControlsAccepted();

protected:
	// 可停止
	bool can_stop_;
	// 可关闭
	bool can_shutdown_;
	// 可暂停和恢复
	bool can_pause_and_continue_;
	// 可接收休眠消息
	bool can_handle_power_event_;
	// 可接收会话变更消息，登录、锁定、注销、关机
	bool can_handle_session_change_event_;
	std::wstring service_name_;

private:
	// 服务状态
	SERVICE_STATUS service_status_;
	SERVICE_STATUS_HANDLE service_status_handle_;
	// 服务操作权限
	DWORD controls_accepted_;
	static CServiceBase * ServiceInstance;
};

#define Enum2Strings(x) case (x): return L#x;

inline const wchar_t * GetServiceStatusString(DWORD dwCurrentState) {
	switch (dwCurrentState) {
	Enum2Strings(SERVICE_STOPPED);
	Enum2Strings(SERVICE_START_PENDING);
	Enum2Strings(SERVICE_STOP_PENDING);
	Enum2Strings(SERVICE_RUNNING);
	Enum2Strings(SERVICE_CONTINUE_PENDING);
	Enum2Strings(SERVICE_PAUSE_PENDING);
	Enum2Strings(SERVICE_PAUSED);
	default: return L"";
	}
}

inline const wchar_t * GetServiceControlString(DWORD dwControl) {
	switch (dwControl) {
	Enum2Strings(SERVICE_CONTROL_STOP);
	Enum2Strings(SERVICE_CONTROL_PAUSE);
	Enum2Strings(SERVICE_CONTROL_CONTINUE);
	Enum2Strings(SERVICE_CONTROL_INTERROGATE);
	Enum2Strings(SERVICE_CONTROL_SHUTDOWN);
	Enum2Strings(SERVICE_CONTROL_PARAMCHANGE);
	Enum2Strings(SERVICE_CONTROL_NETBINDADD);
	Enum2Strings(SERVICE_CONTROL_NETBINDREMOVE);
	Enum2Strings(SERVICE_CONTROL_NETBINDENABLE);
	Enum2Strings(SERVICE_CONTROL_NETBINDDISABLE);
	Enum2Strings(SERVICE_CONTROL_DEVICEEVENT);
	Enum2Strings(SERVICE_CONTROL_HARDWAREPROFILECHANGE);
	Enum2Strings(SERVICE_CONTROL_POWEREVENT);
	Enum2Strings(SERVICE_CONTROL_SESSIONCHANGE);
	Enum2Strings(SERVICE_CONTROL_PRESHUTDOWN);
	// Enum2Strings(SERVICE_CONTROL_TIMECHANGE);
	// Enum2Strings(SERVICE_CONTROL_TRIGGEREVENT);
	// Enum2Strings(SERVICE_CONTROL_LOWRESOURCES);
	// Enum2Strings(SERVICE_CONTROL_SYSTEMLOWRESOURCES);
	default: return L"";
	}
}

inline const wchar_t * GetPBTString(DWORD dwEventType) {
	switch (dwEventType) {
	Enum2Strings(PBT_APMQUERYSUSPEND);
	Enum2Strings(PBT_APMQUERYSTANDBY);
	Enum2Strings(PBT_APMQUERYSUSPENDFAILED);
	Enum2Strings(PBT_APMQUERYSTANDBYFAILED);
	Enum2Strings(PBT_APMSUSPEND);
	Enum2Strings(PBT_APMSTANDBY);
	Enum2Strings(PBT_APMRESUMECRITICAL);
	Enum2Strings(PBT_APMRESUMESUSPEND);
	Enum2Strings(PBT_APMRESUMESTANDBY);
	Enum2Strings(PBT_APMBATTERYLOW);
	Enum2Strings(PBT_APMPOWERSTATUSCHANGE);
	Enum2Strings(PBT_APMOEMEVENT);
	Enum2Strings(PBT_APMRESUMEAUTOMATIC);
	Enum2Strings(PBT_POWERSETTINGCHANGE);
	default: return L"";
	}
}

inline const wchar_t * GetWTSString(DWORD dwEventType) {
	switch (dwEventType) {
	Enum2Strings(WTS_CONSOLE_CONNECT);
	Enum2Strings(WTS_CONSOLE_DISCONNECT);
	Enum2Strings(WTS_REMOTE_CONNECT);
	Enum2Strings(WTS_REMOTE_DISCONNECT);
	Enum2Strings(WTS_SESSION_LOGON);
	Enum2Strings(WTS_SESSION_LOGOFF);
	Enum2Strings(WTS_SESSION_LOCK);
	Enum2Strings(WTS_SESSION_UNLOCK);
	Enum2Strings(WTS_SESSION_REMOTE_CONTROL);
	// Enum2Strings(WTS_SESSION_CREATE);
	// Enum2Strings(WTS_SESSION_TERMINATE);
	default: return L"";
	}
}

}
