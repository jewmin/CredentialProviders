#include "FileLog.h"
#include "Utils.h"

namespace Utils {

CFileLog::CFileLog()
	: file_name_format_(L"CredentialProvider_%02d%02d%02d.log")
	, file_(INVALID_HANDLE_VALUE) {
}

CFileLog::~CFileLog() {
	Close();
}

void CFileLog::Close() {
	if (file_ != INVALID_HANDLE_VALUE) {
		::CloseHandle(file_);
		file_ = INVALID_HANDLE_VALUE;
	}
}

void CFileLog::SetLogFileNameFormat(const std::wstring & file_name_format) {
	file_name_format_ = file_name_format;
	Close();
}

void CFileLog::Output(const std::wstring & message) {
	Utils::CCriticalSection::Owner lock(cs_);

	Utils::CTime now_time;
	now_time.Now();
	if (!timestamp_.IsSameDate(now_time)) {
		Close();
		timestamp_ = now_time;
	}

	if (file_ == INVALID_HANDLE_VALUE) {
		static wchar_t log_file_name[MAX_PATH];
		_snwprintf_s(log_file_name, sizeof(log_file_name), file_name_format_.c_str(), timestamp_.Year, timestamp_.Month, timestamp_.Day);

		file_ = ::CreateFile(log_file_name, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file_ != INVALID_HANDLE_VALUE) {
			::SetFilePointer(file_, 0, NULL, FILE_END);
			const wchar_t * header = L"******************** New Log ********************\n";
			DWORD written;
			std::string buf = Utils::WToA(header);
			::WriteFile(file_, buf.c_str(), static_cast<DWORD>(buf.length()), &written, NULL);
		}
	}

	if (file_ != INVALID_HANDLE_VALUE) {
		static wchar_t time[20];
		_snwprintf_s(time, sizeof(time), L"%04d-%02d-%02d %02d:%02d:%02d", now_time.Year, now_time.Month, now_time.Day, now_time.Hour, now_time.Minute, now_time.Second);

		std::wstringstream msg;
		msg << time << L" - " << GetCurrentProcessId() << L" - " << GetCurrentThreadId() << L" - " << message << std::endl;
		DWORD written;
		std::string buf = Utils::WToA(msg.str());
		::WriteFile(file_, buf.c_str(), static_cast<DWORD>(buf.length()), &written, NULL);
	}
}

}
