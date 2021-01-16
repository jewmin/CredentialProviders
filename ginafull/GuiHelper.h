// GuiHelper.h
//
// Common utilities for window management.
//

#pragma once

class GuiHelper {
public:
    // Avoids some spurious warnings that the /Wp64 compiler switch generates.
    // See http://pluralsight.com/blogs/keith/archive/2005/01/13/4940.aspx
    // for a discussion of the issue.
    static void     SetWindowLongPointer(HWND hwnd, int index, LONG_PTR dwNewLong);
    static LONG_PTR GetWindowLongPointer(HWND hwnd, int index);

    // simple and obvious helper functions
    static void CenterWindow(HWND hwnd);
    static bool SetControlText(HWND hDlg, int id, const wchar_t* pText);
    static bool ExtractControlText(HWND hDlg, int id, wchar_t** ppText);

private:
    GuiHelper() {} // not meant to be instantiated
};