#include "DemoService.h"
#include "FileLog.h"
#include "Utils.h"

#define SERVER_NAME L"DemoService"

int wmain(int argc, const wchar_t ** argv)
{
	Utils::CFileLog log;
	log.SetLogFileNameFormat(L"ServiceRun_%02d%02d%02d.log");
	Utils::SetLog(&log);

	if (argc == 2) {
		std::wstring opt(argv[1]);
		if (opt == L"--install") {
			if (!Utils::CServiceBase::InstallService(SERVER_NAME, L"Demo Service", L"这是一个测试服务程序", argv[0], SERVICE_AUTO_START)) {
				Utils::Output(L"Install Error!!!");
			}
			else {
				Utils::Output(L"Install Success.");
			}
			return 0;
		}
		else if (opt == L"--uninstall") {
			if (!Utils::CServiceBase::UnInstallService(SERVER_NAME)) {
				Utils::Output(L"UnInstall Error!!!");
			}
			else {
				Utils::Output(L"UnInstall Success.");
			}
			return 0;
		}
	}

	Utils::CServiceBase::Run(new Utils::CDemoService(SERVER_NAME));
	Utils::SetLog(NULL);
	return 0;
}
