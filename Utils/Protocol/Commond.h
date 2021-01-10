#pragma once

#include "Header.h"

#define REQUEST_LOGIN	1001
#define RESPONSE_LOGIN	2001

namespace Utils {

namespace Protocol {

class Commond {
public:
	Commond(WORD cmd)
		: cmd_(cmd) {
	}

	virtual ~Commond() {
	}

	WORD GetCmd() const;

private:
	WORD cmd_;
};

inline WORD Commond::GetCmd() const {
	return cmd_;
}

}

}
