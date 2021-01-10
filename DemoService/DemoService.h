#pragma once

#include "Header.h"
#include "Utils.h"
#include "ServiceBase.h"
#include "IService.h"
#include "IOBuffer.h"
#include "UvHandler.h"

namespace DemoService {

class CDemoService : public Utils::CServiceBase, public Utils::UvHandler {
public:
	CDemoService(const std::wstring & service_name);
	virtual ~CDemoService();

protected:
	virtual void RunService();
	virtual void OnStart(DWORD dwNumServicesArgs, LPWSTR * lpServiceArgVectors);
	virtual void OnStop();
	virtual void OnShutdown();
	virtual void OnSessionChange(DWORD dwEventType, PWTSSESSION_NOTIFICATION pNotification);

	virtual void ProcessCommand(uv_pipe_t * pipe, const Utils::CIOBuffer * buffer);

private:
	Utils::IService * service_;
};

}
