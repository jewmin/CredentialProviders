#pragma once

#include "ILog.h"

namespace Utils {

void SetLog(ILog * log);
void Output(const std::wstring & message);

std::string WToA(const std::wstring & input);
std::string WToA(const wchar_t * input);
std::string WToA(const wchar_t * input, const size_t len);

}
