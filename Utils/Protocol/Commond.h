#pragma once

#include "Header.h"

#define REQUEST_LOGIN	1001
#define RESPONSE_LOGIN	2001

namespace Utils {

namespace Protocol {

class Commond {
public:
	Commond(short cmd) : cmd_(cmd) {}
	virtual ~Commond() {}
	short GetCmd() const { return cmd_; }

private:
	short cmd_;
};

}

}
