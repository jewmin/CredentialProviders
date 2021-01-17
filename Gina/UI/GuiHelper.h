#pragma once

#include <WinSock2.h>
#include <Windows.h>

class GuiHelper {
public:
    static void SetWindowLongPointer(HWND hWnd, int nIndex, LONG_PTR dwNewLong);
    static LONG_PTR GetWindowLongPointer(HWND hWnd, int nIndex);
    static void CenterWindow(HWND hWnd);
    static bool SetControlText(HWND hDlg, int nIDDlgItem, LPCWSTR lpString);
    static bool ExtractControlText(HWND hDlg, int nIDDlgItem, LPWSTR * ppString);
};
