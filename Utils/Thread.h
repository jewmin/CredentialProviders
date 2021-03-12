#pragma once

#include "Header.h"

namespace Utils {

class CThread {
public:
	virtual ~CThread();

	void Start();
	void Wait();
	void Terminate();
	bool Terminated() const;
	std::thread::id GetThreadId() const;

	static void Sleep(int milliseconds);
	static std::thread::id CurrentThreadId();

protected:
	CThread();
	virtual void OnRountine() = 0;

private:
	static void ThreadRoutine(CThread * thread);

private:
	CThread(CThread &&) = delete;
	CThread(const CThread &) = delete;
	CThread & operator=(CThread &&) = delete;
	CThread & operator=(const CThread &) = delete;

private:
	std::thread * thread_;
	bool terminated_;
};

inline void CThread::Terminate() {
	terminated_ = true;
}

inline bool CThread::Terminated() const {
	return terminated_;
}

inline std::thread::id CThread::GetThreadId() const {
	if (thread_) {
		return thread_->get_id();
	} else {
		return std::thread::id();
	}
}

}
