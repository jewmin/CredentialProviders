#pragma once

#include "Header.h"

namespace Utils {

namespace Protocol {

#define MAX_STRING_SIZE 128

#pragma pack(1)

// 协议头
typedef struct tagCmdHeader {
	BYTE cCmdBeginFlag;	// 0xBF
	WORD wCmd;			// 协议号
	WORD wCRC;			// [cCmdBeginFlag | cCmdEndFlag] | [wCmd]
	BYTE cCmdEndFlag;	// 0xEF
} CmdHeader;

#pragma pack()

// 协议头处理
class Commond {
public:
	// 授权请求
	static const WORD RequestLogin = 1001;
	// 授权请求响应
	static const WORD ResponseLogin = 2001;
	// 扫码授权请求响应
	static const WORD ResponseQCLogin = 2002;
	static const BYTE CmdBeginFlag = 0xBF;
	static const BYTE CmdEndFlag = 0xEF;
	static const size_t CmdHeaderLen = sizeof(CmdHeader);
	static WORD MakeCrc(WORD wCmd) { return (((CmdBeginFlag << 8) | CmdEndFlag) | wCmd); }
};

}

}
