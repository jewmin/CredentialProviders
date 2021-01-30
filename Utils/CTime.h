#pragma once

#include "Header.h"

namespace Utils {

// 系统时间类
class CTime {
public:
	CTime();
	CTime(const CTime & rhs);
	CTime(const SYSTEMTIME & rhs);
	CTime & operator=(const CTime & rhs);

	// 当前时间
	void Now();
	bool IsInited() const;
	// 比较日期
	bool IsSameDate(const CTime & rhs) const;
	bool IsSameDate(const SYSTEMTIME & rhs) const;

public:
	WORD Year;
	WORD Month;
	WORD Day;
	WORD Hour;
	WORD Minute;
	WORD Second;
};

}
