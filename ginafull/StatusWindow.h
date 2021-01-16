// StatusWindow.h
//
// Simple status window for displaying WinLogon's status messages
//

#pragma once

class StatusWindow {
public:
    StatusWindow(HDESK hdesk, const wchar_t* title, const wchar_t* message);
    ~StatusWindow();
private:
    static INT_PTR CALLBACK _dialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    HDESK _hdesk;
    HWND _hwnd;
};
