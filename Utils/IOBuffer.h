#pragma once

#include "Header.h"
#include "CriticalSection.h"

namespace Utils {

class CIOBuffer {
public:
	class Allocator;
	friend class Allocator;

	uv_buf_t * GetUvBuf() const;
	size_t GetSize() const;
	size_t GetUsed() const;
	const unsigned char * GetBuffer() const;
	void Empty();

	void SetupRead();
	void SetupWrite();

	void AddData(const char * data, size_t len);
	void AddData(const unsigned char * data, size_t len);
	void AddData(unsigned char data);

	void Use(size_t used);
	CIOBuffer * SplitBuffer(size_t remove);
	CIOBuffer * AllocateNewBuffer() const;
	void ConsumeAndRemove(size_t remove);

	void AddRef();
	void Release();

private:
	static void * operator new(size_t obj_size, size_t buffer_size);
	static void operator delete(void * obj, size_t buffer_size);

private:
	CIOBuffer(Allocator & allocator, size_t size);
	CIOBuffer(const CIOBuffer &);
	CIOBuffer & operator=(const CIOBuffer &);

private:
	uv_buf_t uv_buf_;
	Allocator & allocator_;
	long ref_;
	const size_t size_;
	size_t used_;
	unsigned char buffer_[0];
};

class CIOBuffer::Allocator {
public:
	friend class CIOBuffer;
	typedef std::list<CIOBuffer *> BufferList;

	Allocator(size_t buffer_size, size_t max_free_buffers);
	virtual ~Allocator();

	CIOBuffer * Allocate();

protected:
	void Flush();

private:
	void Release(CIOBuffer * buffer);
	void Destroy(CIOBuffer * buffer);

	virtual void OnBufferCreated() {}
	virtual void OnBufferAllocated() {}
	virtual void OnBufferReleased() {}
	virtual void OnBufferDestroyed() {}

private:
	Allocator(const Allocator &);
	Allocator & operator=(const Allocator &);

private:
	const size_t buffer_size_;
	const size_t max_free_buffers_;
	CCriticalSection cs_;
	BufferList free_list_;
	BufferList active_list_;
};

inline uv_buf_t * CIOBuffer::GetUvBuf() const {
	return const_cast<uv_buf_t *>(&uv_buf_);
}

inline size_t CIOBuffer::GetSize() const {
	return size_;
}

inline size_t CIOBuffer::GetUsed() const {
	return used_;
}

inline const unsigned char * CIOBuffer::GetBuffer() const {
	return &buffer_[0];
}

inline void CIOBuffer::Empty() {
	uv_buf_.base = reinterpret_cast<char *>(buffer_);
	uv_buf_.len = size_;

	used_ = 0;
}

inline void CIOBuffer::Use(size_t used) {
	used_ += used;
}

}
