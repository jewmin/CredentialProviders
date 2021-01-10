#include "CTime.h"

namespace Utils {

CTime::CTime()
	: Year(0)
	, Month(0)
	, Day(0)
	, Hour(0)
	, Minute(0)
	, Second(0) {
}

CTime::CTime(const CTime & rhs)
	: Year(rhs.Year)
	, Month(rhs.Month)
	, Day(rhs.Day)
	, Hour(rhs.Hour)
	, Minute(rhs.Minute)
	, Second(rhs.Second) {
}

CTime::CTime(const SYSTEMTIME & rhs)
	: Year(rhs.wYear)
	, Month(rhs.wMonth)
	, Day(rhs.wDay)
	, Hour(rhs.wHour)
	, Minute(rhs.wMinute)
	, Second(rhs.wSecond) {
}

CTime & CTime::operator=(const CTime & rhs) {
	if (this != &rhs) {
		Year = rhs.Year;
		Month = rhs.Month;
		Day = rhs.Day;
		Hour = rhs.Hour;
		Minute = rhs.Minute;
		Second = rhs.Second;
	}
	return *this;
}

void CTime::Now() {
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	Year = systime.wYear;
	Month = systime.wMonth;
	Day = systime.wDay;
	Hour = systime.wHour;
	Minute = systime.wMinute;
	Second = systime.wSecond;
}

bool CTime::IsInited() const {
	return Year != 0 || Month != 0 || Day !=  0;
}

bool CTime::IsSameDate(const CTime & rhs) const {
	return Year == rhs.Year && Month == rhs.Month && Day == rhs.Day;
}

bool CTime::IsSameDate(const SYSTEMTIME & rhs) const {
	return Year == rhs.wYear && Month == rhs.wMonth && Day == rhs.wDay;
}

}
