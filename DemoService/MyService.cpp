#include "MyService.h"
#include "Utils.h"

namespace DemoService {

MyService::MyService() {
	Utils::Output(L"MyService::MyService");
}

MyService::~MyService() {
	Utils::Output(L"MyService::~MyService");
	for (SessionPropertiesDictIter it = sessions_dict_.begin(); it != sessions_dict_.end(); it++) {
		delete it->second;
	}
}

void MyService::OnInited() {
	Utils::Output(L"初始化");
}

void MyService::Auth(Utils::Protocol::LoginRequest * request, Utils::Protocol::LoginResponse * response) {
	Utils::CCriticalSection::Owner lock(cs_);

	wcscpy_s(response->UserName, request->UserName);
	wcscpy_s(response->Password, request->Password);
	static WORD testResult = Utils::Protocol::LoginResponse::Unknown;
	response->Result = testResult++ % 6;

	Utils::Output(Utils::StringFormat(L"会话[%u]授权成功", request->SessionID));
	if (last_auth_user_.UserName.empty()) {
		Utils::Output(Utils::StringFormat(L"新用户[%s]", request->UserName));
	} else if (last_auth_user_.UserName != request->UserName) {
		Utils::Output(Utils::StringFormat(L"切换用户[%s] -> [%s]", last_auth_user_.UserName.c_str(), request->UserName));
	}
	last_auth_user_.SessionID = request->SessionID;
	last_auth_user_.UserName = request->UserName;
	last_auth_user_.Password = request->Password;
	last_auth_user_.LoginTime.Now();
}

void MyService::OnLogon(DWORD SessionID) {
	Utils::CCriticalSection::Owner lock(cs_);
	Utils::Output(Utils::StringFormat(L"会话[%u]登录", SessionID));
	SessionPropertiesDictIter it = sessions_dict_.find(SessionID);
	if (it != sessions_dict_.end()) {
		Utils::Output(Utils::StringFormat(L"更新用户信息[%s] -> [%s]", it->second->ToString().c_str(), last_auth_user_.ToString().c_str()));
		delete it->second;
		it->second = new Utils::SessionProperties(last_auth_user_);
	} else {
		sessions_dict_.insert({SessionID, new Utils::SessionProperties(last_auth_user_)});
	}
}

void MyService::OnLogoff(DWORD SessionID) {
	Utils::CCriticalSection::Owner lock(cs_);
	Utils::Output(Utils::StringFormat(L"会话[%u]注销", SessionID));
	SessionPropertiesDictIter it = sessions_dict_.find(SessionID);
	if (it != sessions_dict_.end()) {
		Utils::Output(Utils::StringFormat(L"删除用户信息[%s]", it->second->ToString().c_str()));
		delete it->second;
		sessions_dict_.erase(it);
	}
}

void MyService::OnLock(DWORD SessionID) {
	Utils::CCriticalSection::Owner lock(cs_);
	Utils::Output(Utils::StringFormat(L"会话[%u]锁定", SessionID));
}

void MyService::OnUnLock(DWORD SessionID) {
	Utils::CCriticalSection::Owner lock(cs_);
	Utils::Output(Utils::StringFormat(L"会话[%u]解锁", SessionID));
	SessionPropertiesDictIter it = sessions_dict_.find(SessionID);
	if (it != sessions_dict_.end()) {
		if (!it->second->LoginTime.IsInited()) {
			Utils::Output(Utils::StringFormat(L"更新用户信息[%s] -> [%s]", it->second->ToString().c_str(), last_auth_user_.ToString().c_str()));
			delete it->second;
			it->second = new Utils::SessionProperties(last_auth_user_);
		}
	}
}

void MyService::OnShutdown() {
	Utils::Output(L"关机");
}

}
