#include "CriticalSection.h"

namespace Utils {

CCriticalSection::CCriticalSection() {
	::InitializeCriticalSection(&cs_);
}

CCriticalSection::~CCriticalSection() {
	::DeleteCriticalSection(&cs_);
}

void CCriticalSection::Enter() {
	::EnterCriticalSection(&cs_);
}

void CCriticalSection::Leave() {
	::LeaveCriticalSection(&cs_);
}

CCriticalSection::Owner::Owner(CCriticalSection & cs)
	: cs_(cs) {
	cs_.Enter();
}

CCriticalSection::Owner::~Owner() {
	cs_.Leave();
}

}
