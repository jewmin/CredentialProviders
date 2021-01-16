// RegistryHelper.h
//
// Routines that simplify access to some WinLogon registry keys
//

#pragma once

class RegistryHelper {
public:
    static bool ReadUserInitProgramList(wchar_t*** programList, int* count);
    static void FreeUserInitProgramList(wchar_t** programList, int count);

private:
    RegistryHelper() {} // not meant to be instantiated
};
