#pragma once

#include "Common.h"

namespace Utils {

class CCriticalSection {
public:
	class Owner {
	public:
		explicit Owner(CCriticalSection & cs);
		~Owner();

	private:
		Owner(const Owner &);
		Owner & operator=(const Owner &);

	private:
		CCriticalSection & cs_;
	};

	CCriticalSection();
	~CCriticalSection();
	void Enter();
	void Leave();

private:
	CCriticalSection(const CCriticalSection &);
	CCriticalSection & operator=(const CCriticalSection &);

private:
	CRITICAL_SECTION cs_;
};

}
