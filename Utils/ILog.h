#pragma once

#include "Header.h"

namespace Utils {

class ILog {
public:
	virtual void Output(const std::wstring & message) = 0;
};

}
