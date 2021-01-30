#pragma once

#include "ILog.h"
#include "CriticalSection.h"
#include "CTime.h"

namespace Utils {

// 写文件日志
class CFileLog : public ILog {
public:
	CFileLog();
	~CFileLog();

	// 设置文件路径，如果有多个程序，路径不要一样，不然会造成某个程序没有日志
	void SetLogFileNameFormat(const std::wstring & file_name_format);
	// 写文件
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
