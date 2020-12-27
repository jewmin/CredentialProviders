#include "Utils.h"

#define CP_GBK 936

namespace Utils {

static ILog * g_log = NULL;

void SetLog(ILog * log) {
	g_log = log;
}

void Output(const std::wstring & message) {
	if (g_log) {
		g_log->Output(message);
	}
}

std::string WToA(const std::wstring & input) {
	return WToA(input.c_str(), input.length());
}

std::string WToA(const wchar_t * input) {
	if (!input) {
		return "";
	}

	return WToA(input, wcslen(input));
}

std::string WToA(const wchar_t * input, const size_t len) {
	std::string output;

	if (len != 0) {
		int output_len = ::WideCharToMultiByte(CP_GBK, 0, input, len, NULL, 0, 0, 0);
		if (output_len > 0) {
			output.resize(output_len);
			::WideCharToMultiByte(CP_GBK, 0, input, len, const_cast<char *>(output.c_str()), output.length(), 0, 0);
		}
	}

	return output;
}

}
