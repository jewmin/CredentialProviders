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
	static void UvWalkCb(uv_handle_t* handle, void* arg);

private:
	uv_loop_t event_loop_;
	uv_async_t event_notify_;
	IService * service_;
};

}
