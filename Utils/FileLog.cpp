#include "FileLog.h"
#include "Utils.h"

namespace Utils {

CFileLog::CFileLog()
	: file_name_format_(L"CredentialProvider_%02d%02d%02d.log")
	, timestamp_year_(0)
	, timestamp_month_(0)
	, timestamp_day_(0)
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
	CCriticalSection::Owner lock(cs_);

	SYSTEMTIME systime;
	GetLocalTime(&systime);
	if (systime.wDay != timestamp_day_ || systime.wMonth != timestamp_month_ || systime.wYear != timestamp_year_) {
		Close();
		timestamp_year_ = systime.wYear;
		timestamp_month_ = systime.wMonth;
		timestamp_day_ = systime.wDay;
	}

	if (file_ == INVALID_HANDLE_VALUE) {
		static wchar_t log_file_name[MAX_PATH];
		_snwprintf_s(log_file_name, sizeof(log_file_name), file_name_format_.c_str(), timestamp_year_, timestamp_month_, timestamp_day_);

		file_ = ::CreateFile(log_file_name, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file_ != INVALID_HANDLE_VALUE) {
			::SetFilePointer(file_, 0, NULL, FILE_END);
			const wchar_t * header = L"******************** New Log ********************\n";
			DWORD written;
			std::string buf = WToA(header);
			::WriteFile(file_, buf.c_str(), static_cast<DWORD>(buf.length()), &written, NULL);
		}
	}

	if (file_ != INVALID_HANDLE_VALUE) {
		static wchar_t time[20];
		_snwprintf_s(time, sizeof(time), L"%04d-%02d-%02d %02d:%02d:%02d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);

		std::wstringstream msg;
		msg << time << L" - " << GetCurrentProcessId() << L" - " << message << std::endl;
		DWORD written;
		std::string buf = WToA(msg.str());
		::WriteFile(file_, buf.c_str(), static_cast<DWORD>(buf.length()), &written, NULL);
	}
}

}
