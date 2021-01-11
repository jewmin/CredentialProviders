#include "IOBuffer.h"

namespace Utils {

CIOBuffer::CIOBuffer(Allocator & allocator, size_t size)
	: allocator_(allocator)
	, ref_(1)
	, size_(size)
	, used_(0) {
	Empty();
}

void * CIOBuffer::operator new(size_t obj_size, size_t buffer_size, int placeholder) {
	UNREFERENCED_PARAMETER(placeholder);
	void * mem = new char[obj_size + buffer_size];
	return mem;
}

void CIOBuffer::operator delete(void * obj, size_t buffer_size, int placeholder) {
	UNREFERENCED_PARAMETER(buffer_size);
	UNREFERENCED_PARAMETER(placeholder);
	delete[] obj;
}

void CIOBuffer::operator delete(void * obj) {
	delete[] obj;
}

void CIOBuffer::SetupRead() {
	if (0 == used_) {
		uv_buf_.base = reinterpret_cast<char *>(buffer_);
		uv_buf_.len = static_cast<ULONG>(size_);
	} else {
		uv_buf_.base = reinterpret_cast<char *>(buffer_) + used_;
		uv_buf_.len = static_cast<ULONG>(size_ - used_);
	}
}

void CIOBuffer::SetupWrite() {
	uv_buf_.base = reinterpret_cast<char *>(buffer_);
	uv_buf_.len = static_cast<ULONG>(used_);

	used_ = 0;
}

void CIOBuffer::AddData(const char * data, size_t len) {
	AddData(reinterpret_cast<const BYTE *>(data), len);
}

void CIOBuffer::AddData(const BYTE * data, size_t len) {
	assert(len <= size_ - used_);
	memcpy(buffer_ + used_, data, len);
	used_ += len;
}

void CIOBuffer::AddData(BYTE data) {
	AddData(&data, 1);
}

CIOBuffer * CIOBuffer::SplitBuffer(size_t remove) {
	CIOBuffer * new_buffer = AllocateNewBuffer();
	new_buffer->AddData(buffer_, remove);
	used_ -= remove;
	memmove(buffer_, buffer_ + remove, used_);
	return new_buffer;
}

CIOBuffer * CIOBuffer::AllocateNewBuffer() const {
	return allocator_.Allocate();
}

void CIOBuffer::ConsumeAndRemove(size_t remove) {
	used_ -= remove;
	memmove(buffer_, buffer_ + remove, used_);
}

void CIOBuffer::AddRef() {
	::InterlockedIncrement(&ref_);
}

void CIOBuffer::Release() {
	if (0 == ::InterlockedDecrement(&ref_)) {
		allocator_.Release(this);
	}
}

CIOBuffer::Allocator::Allocator(size_t buffer_size, size_t max_free_buffers)
	: buffer_size_(buffer_size)
	, max_free_buffers_(max_free_buffers) {
}

CIOBuffer::Allocator::~Allocator() {
	try {
		Flush();
	} catch (...) {
	}
}

void CIOBuffer::Allocator::Flush() {
	Utils::CCriticalSection::Owner lock(cs_);

	while (!active_list_.empty()) {
		OnBufferReleased();
		Destroy(active_list_.front());
		active_list_.pop_front();
	}

	while (!free_list_.empty()) {
		Destroy(free_list_.front());
		free_list_.pop_front();
	}
}

CIOBuffer * CIOBuffer::Allocator::Allocate() {
	Utils::CCriticalSection::Owner lock(cs_);

	CIOBuffer * new_buffer = NULL;

	if (!free_list_.empty()) {
		new_buffer = free_list_.front();
		free_list_.pop_front();
		new_buffer->AddRef();
	} else {
		new_buffer = new(buffer_size_, 0)CIOBuffer(*this, buffer_size_);
		OnBufferCreated();
	}

	active_list_.push_back(new_buffer);
	OnBufferAllocated();
	return new_buffer;
}

void CIOBuffer::Allocator::Release(CIOBuffer * buffer) {
	assert(buffer != NULL);
	Utils::CCriticalSection::Owner lock(cs_);

	OnBufferReleased();
	active_list_.remove(buffer);

	if (0 == max_free_buffers_ || free_list_.size() < max_free_buffers_) {
		buffer->Empty();
		free_list_.push_back(buffer);
	} else {
		Destroy(buffer);
	}
}

void CIOBuffer::Allocator::Destroy(CIOBuffer * buffer) {
	delete buffer;
	OnBufferDestroyed();
}

}
