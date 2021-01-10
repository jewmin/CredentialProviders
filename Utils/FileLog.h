#pragma once

#include "ILog.h"
#include "CriticalSection.h"
#include "CTime.h"

namespace Utils {

class CFileLog : public ILog {
public:
	CFileLog();
	~CFileLog();

	void SetLogFileNameFormat(const std::wstring & file_name_format);
	virtual void Output(const std::wstring & message);

protected:
	void Close();

private:
	std::wstring file_name_format_;
	CTime timestamp_;

	HANDLE file_;
	CCriticalSection cs_;
};

}
