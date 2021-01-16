// Log.cpp
//
// Logging of critical failures and more verbose information
//

#include "stdafx.h"
#include "Log.h"

static void _writeLog(const wchar_t* msg) {
    HANDLE h = CreateFile(LOGFILE_PATH, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
    if (INVALID_HANDLE_VALUE != h) {
        if (INVALID_SET_FILE_POINTER != SetFilePointer(h, 0, 0, FILE_END)) {
            DWORD cb = lstrlen(msg) * sizeof *msg;
            WriteFile(h, msg, cb, &cb, 0);
        }
        CloseHandle(h);
    }
}

static void _simpleLog(const wchar_t* sourceFile, int lineNumber, const wchar_t* formatString, va_list args) {
    wchar_t msg[512];
    if (Log::FormatLogEntry(msg, sizeof msg / sizeof *msg, sourceFile, lineNumber, formatString, args)) {
        _writeLog(msg);
    }
}

void Log::CriticalFailure(const wchar_t* sourceFile, int lineNumber, const wchar_t* formatString, ...) {
    va_list args; va_start(args, formatString);
    _simpleLog(sourceFile, lineNumber, formatString, args);
}

void Log::Debug(const wchar_t* sourceFile, int lineNumber, const wchar_t* formatString, ...) {
    va_list args; va_start(args, formatString);
    _simpleLog(sourceFile, lineNumber, formatString, args);
}

static const wchar_t* _seekToFileName(const wchar_t* sourceFile) {
    if (!*sourceFile) return sourceFile; // empty string

    const wchar_t* begin = sourceFile;
	const wchar_t* it;
    for (it = begin; *it; ++it) {
        // just find the end of the string
    }

    // iterator points to null terminator now - search back for first backslash
    while (it != begin) {
        if (L'\\' == *it) {
            ++it;
            break;
        }
        --it;
    }
    return it;
}


bool Log::FormatLogEntry(wchar_t* msg, int cch, const wchar_t* sourceFile, int lineNumber, const wchar_t* formatString, va_list args) {
    if (0 == cch) return false;
    msg[0] = L'\0';

    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime);

    DWORD tsSessionId;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &tsSessionId)) tsSessionId = 0;

    const wchar_t* tssDesc = GetSystemMetrics(SM_REMOTESESSION) ? L"TSS(R)" : L"TSS(L)";

    HRESULT hr = StringCchPrintf(msg, cch, L"%s %d: %02d:%02d:%02d:%04d %s(%d): ",
        tssDesc,
        tsSessionId,
        systemTime.wHour,
        systemTime.wMinute,
        systemTime.wSecond,
        systemTime.wMilliseconds,
        _seekToFileName(sourceFile),
        lineNumber);
    if (FAILED(hr)) return false;

    wchar_t suffix[256];
    hr = StringCchVPrintf(suffix, sizeof suffix / sizeof *suffix, formatString, args);
    if (FAILED(hr)) return false;

    hr = StringCchCat(msg, cch, suffix);
    if (FAILED(hr)) return false;

    hr = StringCchCat(msg, cch, L"\r\n");
    if (FAILED(hr)) return false;

    return true;
}

bool Log::LookupErrorMessage(wchar_t* buf, int cch, DWORD err) {
    if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, err, 0, buf, cch, 0)) {
        return true;
    }
    else {
        LDB1(L"FormatMessage failed: %d", GetLastError());
        StringCchPrintf(buf, cch, (err < 15000) ? L"Error number: %d" :
                                                  L"Error number: 0x%08X", err);
        return false;
    }
}
