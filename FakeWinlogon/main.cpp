#include <WinSock2.h>
#include <Windows.h>
#include "GinaService.h"

int wmain(int argc, const wchar_t * * argv) {
    if (argc == 2) {
		std::wstring opt(argv[1]);
		if (opt == L"--install") {
			Utils::CServiceBase::InstallService(L"GinaService", L"Gina Service", L"≤‚ ‘Gina", argv[0], SERVICE_AUTO_START);
		} else if (opt == L"--uninstall") {
			Utils::CServiceBase::UnInstallService(L"GinaService");
		}
		return 0;
	}

	Utils::CServiceBase::Run(new GinaService());
    return 0;
}
