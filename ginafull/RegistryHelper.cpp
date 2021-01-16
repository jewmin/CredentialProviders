// RegistryHelper.cpp
//
// Routines that interface with Win32 and LSA Registry APIs
//

#include "stdafx.h"
#include "RegistryHelper.h"

static wchar_t* _dupStringRange(const wchar_t* begin, const wchar_t* end) {
    const int cch = (int)(end - begin);
    wchar_t* newStr = new wchar_t[cch+1];
    if (newStr) {
        CopyMemory(newStr, begin, cch * sizeof *newStr);
        newStr[cch] = L'\0';
    }
    else LOOM;
    return newStr;
}

static bool _parseUserInitRegistryValue(const wchar_t* userinitValue, wchar_t*** programList, int* count) {
    // the Userinit value is a comma-separated list of program files to be executed by Gina
    const wchar_t COMMA = L',';

    // quickly overestimate how many programs are in the list
    const wchar_t* it = userinitValue;
    int stringCountEstimate = 1;
    while (*it) {
        if (COMMA == *it++) ++stringCountEstimate;
    }

    // allocate the list
    *programList = new wchar_t*[stringCountEstimate];
    if (!*programList) {
        LOOM;
        return false;
    }
    ZeroMemory(*programList, stringCountEstimate * sizeof **programList);

    // parse the strings
    it = userinitValue;
    const wchar_t* begin = 0;

    *count = 0;
    for (;;) {
        if (!begin) begin = it;

        if (COMMA == *it || 0 == *it) {
            const wchar_t* end = it;

            if (begin != end) {
                wchar_t* newStr = _dupStringRange(begin, end);
                if (!newStr) return false; // system is melting

                (*programList)[*count] = newStr;
                ++*count;
            }
            if (0 == *it) break;
            begin = 0;
        }
        ++it;
    }
    return true;
}

void RegistryHelper::FreeUserInitProgramList(wchar_t** programList, int count) {
    wchar_t** end = programList + count;
    for (wchar_t** it = programList; it != end; ++it) {
        delete *it;
    }
    delete programList;
}

bool RegistryHelper::ReadUserInitProgramList(wchar_t*** programList, int* count) {
    HKEY hkey;
    LONG status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\WinLogon", 0, KEY_QUERY_VALUE, &hkey);
    if (status) {
        LCF1(L"Failed to open WinLogon registry key: %d", status);
        return false;
    }

    BYTE* userinitValue = 0;

    DWORD cb = 0;
    status = RegQueryValueEx(hkey, L"Userinit", 0, 0, 0, &cb);
    if (0 == status) {
        userinitValue = (BYTE*)LocalAlloc(LMEM_FIXED, cb);
        if (!userinitValue) {
            LOOM;
            return false;
        }

        status = RegQueryValueEx(hkey, L"Userinit", 0, 0, userinitValue, &cb);
    }
    RegCloseKey(hkey);

    if (status) {
        LCF1(L"Failed to read Userinit registry value: %d", status);
        return false;
    }

    bool result = _parseUserInitRegistryValue((wchar_t*)userinitValue, programList, count);

    LocalFree(userinitValue);

    return result;
}
