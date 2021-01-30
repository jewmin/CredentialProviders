#pragma once

#include "Header.h"

namespace Utils {

// 日志基类
class ILog {
public:
	virtual void Output(const std::wstring & message) = 0;
};

}
