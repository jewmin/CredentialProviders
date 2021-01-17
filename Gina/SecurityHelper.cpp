#include "SecurityHelper.h"
#include "Utils.h"

#define LOGON_PROCESS_NAME "GINA Sample"

//
// UINT_PTR -> USHORT conversion
//
static bool UIntPtrToUShort(UINT_PTR uOperand, USHORT * pusResult) {
    if (uOperand <= USHRT_MAX) {
        *pusResult = static_cast<USHORT>(uOperand);
        return true;
    } else {
        *pusResult = USHRT_MAX;
        return false;
    }
}

// 
// This function packs the string pszSourceString in pszDestinationString
// for use with LSA functions including LsaRegisterLogonProcess.
//
static bool _LsaInitString(PSTRING pszDestinationString, PCSTR pszSourceString) {
    size_t cchLength = lstrlenA(pszSourceString);
    USHORT usLength;
    if (UIntPtrToUShort(cchLength, &usLength)) {
        pszDestinationString->Buffer = const_cast<PCHAR>(pszSourceString);
        pszDestinationString->Length = usLength;
        pszDestinationString->MaximumLength = pszDestinationString->Length + 1;
        return true;
    }
    return false;
}

bool SecurityHelper::RegisterLogonProcess(PHANDLE LsaHandle) {
    *LsaHandle = NULL;
    LSA_STRING LogonProcessName;
    if (!_LsaInitString(&LogonProcessName, LOGON_PROCESS_NAME)) {
        return false;
    }

    LSA_OPERATIONAL_MODE unused;
    NTSTATUS status = LsaRegisterLogonProcess(&LogonProcessName, LsaHandle, &unused);
    if (FAILED(HRESULT_FROM_NT(status))) {
        *LsaHandle = NULL;
        Utils::Output(Utils::StringFormat(L"SecurityHelper::RegisterLogonProcess Error: %u", LsaNtStatusToWinError(status)));
        return false;
    }
    return true;
}
