// Log.h
//
// Logging of critical failures and more verbose information
//

#pragma once

class Log {
public:
    static void CriticalFailure(const wchar_t* sourceFile, int lineNumber, const wchar_t* formatString, ...);
    static void Debug(const wchar_t* sourceFile, int lineNumber, const wchar_t* formatString, ...);

    static bool FormatLogEntry(wchar_t* msg, int cch, const wchar_t* sourceFile, int lineNumber, const wchar_t* formatString, va_list args);

    static bool LookupErrorMessage(wchar_t* buf, int cch, DWORD err);
private:
    Log() {} // not meant to be instantiated
};

//
// Logging macros
//

#define LOOM         Log::CriticalFailure(__WFILE__, __LINE__, L"Out of memory")
#define LCF(reason)          Log::CriticalFailure(__WFILE__, __LINE__, reason)
#define LCF1(reason, a)      Log::CriticalFailure(__WFILE__, __LINE__, reason, a)
#define LCF2(reason, a, b)   Log::CriticalFailure(__WFILE__, __LINE__, reason, a, b)

// use this to log debugging messages (not present in release builds)
#ifdef _DEBUG
#define LDB(reason)         Log::Debug(__WFILE__, __LINE__, reason);
#define LDB1(reason, a)     Log::Debug(__WFILE__, __LINE__, reason, a);
#define LDB2(reason, a, b)  Log::Debug(__WFILE__, __LINE__, reason, a, b);
#else
#define LDB(reason)         NULL
#define LDB1(reason, a)     NULL
#define LDB2(reason, a, b)  NULL
#endif
