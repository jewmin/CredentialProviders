#include "GinaService.h"
#include "Utils.h"

GinaService::GinaService() : Utils::CServiceBase(L"GinaService"), stop_(false), run_gina_(false) {
	can_pause_and_continue_ = true;
	log_.SetLogFileNameFormat(L"C:/GinaService_%02d%02d%02d.log");
	Utils::SetLog(&log_);
}

GinaService::~GinaService() {
	Utils::SetLog(NULL);
}

void GinaService::OnStop() {
	stop_ = true;
}

void GinaService::OnContinue() {
	run_gina_ = true;
}

void GinaService::RunService() {
	while (!stop_) {
		if (run_gina_) {
			RunGina();
			run_gina_ = false;
		}
		::Sleep(16);
	}
}

void GinaService::RunGina() {
	HINSTANCE hDll = ::LoadLibrary(L"Gina.dll");
	FARPROC DebugGINA = ::GetProcAddress(hDll, "DebugGINA");
	DebugGINA();
}
