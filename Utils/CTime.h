#pragma once

#include "Header.h"

namespace Utils {

class CTime {
public:
	CTime();
	CTime(const CTime & rhs);
	CTime(const SYSTEMTIME & rhs);
	CTime & operator=(const CTime & rhs);

	void Now();
	bool IsInited() const;
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
