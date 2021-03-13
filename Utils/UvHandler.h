#pragma once

#include "Header.h"
#include "IOBuffer.h"

namespace Utils {

// 管道基类，基于libuv实现，参考链接：
// https://zhuanlan.zhihu.com/p/139127919
// http://docs.libuv.org/en/v1.x/
class UvHandler : private Utils::CIOBuffer::Allocator {
public:
	virtual ~UvHandler();

protected:
	UvHandler();
	// 写数据前处理，主要是用于处理协议头
	void PreWrite(Utils::CIOBuffer * buffer, WORD wCmd);
	// 管道异步写数据
	int Write(uv_pipe_t * pipe, WORD wCmd, const BYTE * data, size_t len);
	// 管道连接成功后回调
	virtual void OnConnected(uv_pipe_t * pipe, bool status);
	// 管道断开连接后回调
	virtual void OnDisconnected(uv_pipe_t * pipe);

	// 这部分可以重写，实现协议分割
	void ReadCompleted(uv_pipe_t * pipe, Utils::CIOBuffer * buffer);
	size_t GetMinimumMessageSize() const;
	size_t GetMessageSize(const Utils::CIOBuffer * buffer) const;
	// 协议真正处理，子类必须重写
	virtual void ProcessCommand(uv_pipe_t * pipe, const Utils::CIOBuffer * buffer) = 0;

protected:
	// 事件循环
	int EventPoll(uv_run_mode mode = UV_RUN_DEFAULT);
	// 停止事件循环
	void EventStop();
	// 事件循环停止后，释放资源
	void EventStopped();
	// 初始化事件循环并启动管道服务端监听
	void EventInitServer();
	// 初始化事件循环并启动管道客户端连接
	void EventInitClient();
	// 管道客户端连接
	void StartConnectPipe();

	static uv_pipe_t * CreatePipe(uv_loop_t * loop);
	static void ClosePipe(uv_pipe_t * handle);

private:
	// 遍历所有管道，统一关闭，一般是在析构时释放资源使用
	static void UvWalkCb(uv_handle_t* handle, void* arg);
	// 关闭管道回调
	static void UvCloseCb(uv_handle_t* handle);
	// 服务端管道监听回调，当有客户端管道连接上来时回调
	static void UvConnectionCb(uv_stream_t* server, int status);
	// 客户端管道连接返回回调，status为0则成功，否则失败
	static void UvConnectCb(uv_connect_t* req, int status);
	// 管道读取数据前，分配缓存区回调，在这里分配内存，用于读数据
	static void UvAllocCb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	// 管道读取数据回调，nread表示读到的数据长度，如果<0则表示读取报错，此时会关闭管道
	static void UvReadCb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	// 管道发送数据完成回调，如果status<0则表示发送失败，此时会关闭管道
	static void UvWriteCb(uv_write_t* req, int status);

protected:
	// 事件循环
	uv_loop_t event_loop_;
	// 异步通知、线程安全
	uv_async_t event_notify_;
};

}
