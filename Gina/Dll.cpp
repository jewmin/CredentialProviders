#include <WinSock2.h>
#include <Windows.h>
#include "FileLog.h"
#include "Utils.h"

static HINSTANCE g_hinst = NULL;
static Utils::CFileLog g_log;

HINSTANCE GetInstance() { return g_hinst; }

BOOL WINAPI DllMain(__in HINSTANCE hinstDll, __in DWORD dwReason, __in void *)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_log.SetLogFileNameFormat(L"C:/Gina_%02d%02d%02d.log");
        Utils::SetLog(&g_log);
        DisableThreadLibraryCalls(hinstDll);
        break;

    case DLL_PROCESS_DETACH:
        Utils::SetLog(NULL);
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    
    g_hinst = hinstDll;
    return TRUE;
}
