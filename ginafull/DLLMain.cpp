// DLLMain.cpp : Defines the entry point for the GINA DLL.
//
#include "stdafx.h"

// this is the one and only global variable we use, and it's implemented to be read-only
static HANDLE _hModule;

HMODULE   GetMyModuleHandle() { return (HMODULE)_hModule; }
HINSTANCE GetMyInstance()     { return (HINSTANCE)_hModule; }

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  reason, 
                       LPVOID lpReserved
					 )
{
    if (DLL_PROCESS_ATTACH == reason) {
        _hModule = hModule;
        DisableThreadLibraryCalls(GetMyModuleHandle());
    }
    return TRUE;
}

