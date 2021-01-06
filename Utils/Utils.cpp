#include "Utils.h"

#define CP_GBK 936

namespace Utils {

static ILog * g_log = NULL;
static IUserLogin * g_user_login = NULL;

void SetLog(ILog * log) {
	g_log = log;
}

void Output(const std::wstring & message) {
	if (g_log) {
		g_log->Output(message);
	}
}

std::wstring StringFormat(const wchar_t * format, ...) {
	va_list args;
	va_start(args, format);
	std::wstring s = StringFormatVa(format, args);
	va_end(args);
	return s;
}

std::wstring StringFormatVa(const wchar_t * format, va_list args) {
	size_t size = 1024;
	wchar_t * buffer = new wchar_t[size];

	while (true) {
		va_list args_copy;
		args_copy = args;
		int n = _vsnwprintf(buffer, size, format, args_copy);
		va_end(args_copy);

		if (n > -1 && static_cast<size_t>(n) < size) {
			std::wstring s(buffer);
			delete[] buffer;
			return s;
		}

		size = n > -1 ? n + 1 : size << 1;
		delete[] buffer;
		buffer = new wchar_t[size];
	}
}

std::wstring GetLastErrorString() {
	DWORD error_code = ::GetLastError();
	LPVOID lpBuffer = NULL;
	if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&lpBuffer, 0, NULL)) {
		std::wstring result = StringFormat(L"%u %s", error_code, lpBuffer);
		LocalFree(lpBuffer);
		return result;
	}

	return StringFormat(L"%u", error_code);
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
		int output_len = ::WideCharToMultiByte(CP_GBK, 0, input, static_cast<int>(len), NULL, 0, 0, 0);
		if (output_len > 0) {
			output.resize(output_len);
			::WideCharToMultiByte(CP_GBK, 0, input, static_cast<int>(len), const_cast<char *>(output.c_str()), static_cast<int>(output.length()), 0, 0);
		}
	}

	return output;
}

void SetUserLogin(IUserLogin * user_login) {
	g_user_login = user_login;
}

}
