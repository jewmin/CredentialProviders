#pragma once

#include "Header.h"

namespace Utils {

class SessionProperties {
public:
	SessionProperties() : SessionID(-1), UserName(L""), Password(L"") {}
	SessionProperties(DWORD session_id, const std::wstring & username = L"", const std::wstring & password = L"") : SessionID(session_id), UserName(username), Password(password) {}
	SessionProperties(const SessionProperties & rhs) : SessionID(rhs.SessionID), UserName(rhs.UserName), Password(rhs.Password) {}
	SessionProperties(SessionProperties && rhs) : SessionID(rhs.SessionID), UserName(std::move(rhs.UserName)), Password(std::move(rhs.Password)) {}
	SessionProperties & operator=(const SessionProperties & rhs);
	SessionProperties & operator=(SessionProperties && rhs);
	~SessionProperties() {}

	bool operator==(const SessionProperties & rhs) const;
	bool operator!=(const SessionProperties & rhs) const;
	bool operator<(const SessionProperties & rhs) const;
	bool operator<=(const SessionProperties & rhs) const;
	bool operator>(const SessionProperties & rhs) const;
	bool operator>=(const SessionProperties & rhs) const;

public:
	DWORD SessionID;
	std::wstring UserName;
	std::wstring Password;
};

inline SessionProperties & SessionProperties::operator=(const SessionProperties & rhs) {
	if (this != &rhs) {
		SessionID = rhs.SessionID;
		UserName = rhs.UserName;
		Password = rhs.Password;
	}
	return *this;
}

inline SessionProperties & SessionProperties::operator=(SessionProperties && rhs) {
	if (this != &rhs) {
		SessionID = rhs.SessionID;
		UserName = std::move(rhs.UserName);
		Password = std::move(rhs.Password);
	}
	return *this;
}

inline bool SessionProperties::operator==(const SessionProperties & rhs) const {
	return SessionID == rhs.SessionID;
}

inline bool SessionProperties::operator!=(const SessionProperties & rhs) const {
	return SessionID != rhs.SessionID;
}

inline bool SessionProperties::operator<(const SessionProperties & rhs) const {
	return SessionID < rhs.SessionID;
}

inline bool SessionProperties::operator<=(const SessionProperties & rhs) const {
	return SessionID <= rhs.SessionID;
}

inline bool SessionProperties::operator>(const SessionProperties & rhs) const {
	return SessionID > rhs.SessionID;
}

inline bool SessionProperties::operator>=(const SessionProperties & rhs) const {
	return SessionID >= rhs.SessionID;
}

}
