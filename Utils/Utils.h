#pragma once

#include "ILog.h"

namespace Utils {

// 设置日志
void SetLog(ILog * log);
// 写日志
void Output(const std::wstring & message);
// 格式化字符串
std::wstring StringFormat(const wchar_t * format, ...);
std::wstring StringFormatVa(const wchar_t * format, va_list args);
// 格式化Windows报错
std::wstring GetLastErrorString();
std::wstring GetErrorString(DWORD error_code);

// 宽字节转多字节
std::string WToA(const std::wstring & input);
std::string WToA(const wchar_t * input);
std::string WToA(const wchar_t * input, const size_t len);

// 多字节转宽字节
std::wstring AToW(const std::string & input);
std::wstring AToW(const char * input);
std::wstring AToW(const char * input, const size_t len);

}
