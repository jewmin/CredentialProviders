#include "Thread.h"

namespace Utils {

CThread::CThread() : thread_(nullptr), terminated_(false) {
}

CThread::~CThread() {
	Wait();
}

void CThread::Start() {
	if (!thread_) {
		thread_ = new std::thread(ThreadRoutine, this);
	}
}

void CThread::Wait() {
	if (thread_) {
		Terminate();
		thread_->join();
		delete thread_;
		thread_ = nullptr;
		terminated_ = false;
	}
}

void CThread::ThreadRoutine(CThread * thread) {
	thread->OnRountine();
}

void CThread::Sleep(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::microseconds(milliseconds));
}

std::thread::id CThread::CurrentThreadId() {
	return std::this_thread::get_id();
}

}