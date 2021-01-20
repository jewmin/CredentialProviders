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
// ULONG -> USHORT conversion
//
static bool ULongToUShort(ULONG ulOperand, USHORT * pusResult) {
    if (ulOperand <= USHRT_MAX) {
        *pusResult = static_cast<USHORT>(ulOperand);
        return true;
    } else {
        *pusResult = USHRT_MAX;
        return false;
    }
}

//
// USHORT multiplication
//
static bool UShortMult(USHORT usMultiplicand, USHORT usMultiplier, USHORT * pusResult) {
    ULONG ulResult = static_cast<ULONG>(usMultiplicand) * static_cast<ULONG>(usMultiplier);
    return ULongToUShort(ulResult, pusResult);
}

// 
// This function packs the string pszSourceString in pszDestinationString
// for use with LSA functions including LsaRegisterLogonProcess.
//
static bool LsaInitString(PSTRING pszDestinationString, PCSTR pszSourceString) {
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

//
// This function copies the length of pwz and the pointer pwz into the UNICODE_STRING structure
// Note that this function just makes a copy of the string pointer. It DOES NOT ALLOCATE storage!
//
static bool UnicodeStringInitWithString(PWSTR pwz, UNICODE_STRING * pus) {
    if (pwz) {
        size_t lenString = lstrlen(pwz);
        USHORT usCharCount;
        if (UIntPtrToUShort(lenString, &usCharCount)) {
            USHORT usSize;
            if (UIntPtrToUShort(sizeof(WCHAR), &usSize)) {
                if (UShortMult(usCharCount, usSize, &(pus->Length))) { // Explicitly NOT including NULL terminator
                    pus->MaximumLength = pus->Length + 1;
                    pus->Buffer = pwz;
                    return true;
                }
            }
        }
    }

    return false;
}

static bool MSV1InteractiveLogonInit(PWSTR pwzDomain, PWSTR pwzUsername, PWSTR pwzPassword, PMSV1_0_INTERACTIVE_LOGON pmil) {
    ZeroMemory(pmil, sizeof(*pmil));
    if (UnicodeStringInitWithString(pwzDomain, &pmil->LogonDomainName)) {
        hr = UnicodeStringInitWithString(pwzUsername, &pkil->UserName);
        if (SUCCEEDED(hr))
        {
            hr = UnicodeStringInitWithString(pwzPassword, &pkil->Password);
            if (SUCCEEDED(hr))
            {
                // Set a MessageType based on the usage scenario.
                switch (cpus)
                {
                case CPUS_UNLOCK_WORKSTATION:
                    pkil->MessageType = KerbWorkstationUnlockLogon;
                    hr = S_OK;
                    break;

                case CPUS_LOGON:
                    pkil->MessageType = KerbInteractiveLogon;
                    hr = S_OK;
                    break;

                case CPUS_CREDUI:
                    pkil->MessageType = (KERB_LOGON_SUBMIT_TYPE)0; // MessageType does not apply to CredUI
                    hr = S_OK;
                    break;

                default:
                    hr = E_FAIL;
                    break;
                }

                if (SUCCEEDED(hr))
                {
                    // KERB_INTERACTIVE_UNLOCK_LOGON is just a series of structures.  A
                    // flat copy will properly initialize the output parameter.
                    CopyMemory(pkiul, &kiul, sizeof(*pkiul));
                }
            }
        }
    }

    return hr;
}

PWSTR SecurityHelper::LocalAllocString(PCWSTR lpString) {
    if (!lpString) {
        return NULL;
    }

    size_t cch = lstrlen(lpString);
    size_t cbLen = sizeof(WCHAR) * (cch + 1);
    PWSTR pwszDest = static_cast<PWSTR>(::LocalAlloc(0, cbLen));
    if (pwszDest) {
        CopyMemory(pwszDest, lpString, cbLen);
    } else {
        Utils::Output(L"SecurityHelper::LocalAllocString pwszDest: Out Of Memory");
    }

    return pwszDest;
}

bool SecurityHelper::RegisterLogonProcess(PHANDLE pLsaHandle) {
    *pLsaHandle = NULL;
    LSA_STRING LogonProcessName;
    if (!LsaInitString(&LogonProcessName, LOGON_PROCESS_NAME)) {
        return false;
    }

    LSA_OPERATIONAL_MODE unused;
    NTSTATUS status = LsaRegisterLogonProcess(&LogonProcessName, pLsaHandle, &unused);
    if (FAILED(HRESULT_FROM_NT(status))) {
        *pLsaHandle = NULL;
        Utils::Output(Utils::StringFormat(L"SecurityHelper::RegisterLogonProcess Error: %u", LsaNtStatusToWinError(status)));
        return false;
    }
    return true;
}

bool SecurityHelper::CallLsaLogonUser(HANDLE hLsaHandle, PCWSTR szDomain, PCWSTR szUserName, PCWSTR szPassword, SECURITY_LOGON_TYPE logonType, PLUID pLogonId, PHANDLE pToken, PMSV1_0_INTERACTIVE_PROFILE * ppProfile, DWORD * pdwWin32Error) {
    bool result = false;
    DWORD dwWin32Error = 0;
    *pToken = NULL;
    LUID ignoredLogonId;

    if (ppProfile) {
        *ppProfile = NULL;
    }

    if (!pLogonId) {
        pLogonId = &ignoredLogonId;
    }

    LSA_STRING LogonProcessName;
    TOKEN_SOURCE SourceContext = {'s', 'a', 'm', 'p', 'l', 'e'};
    PMSV1_0_INTERACTIVE_PROFILE pProfile = NULL;
    ULONG cbProfile = 0;
    // QUOTA_LIMITS ;
    // NTSTATUS



}

bool SecurityHelper::ImpersonateAndGetUserName(HANDLE hUserToken, PWSTR pszUserName, int cch) {
    bool result = false;
    if (::ImpersonateLoggedOnUser(hUserToken)) {
        DWORD cchMax = cch;
        if (::GetUserName(pszUserName, &cchMax)) {
            result = true;
        } else {
            Utils::Output(Utils::StringFormat(L"SecurityHelper::ImpersonateAndGetUserName GetUserName Error: %s", Utils::GetLastErrorString().c_str()));
        }
        ::RevertToSelf();
    } else {
        Utils::Output(Utils::StringFormat(L"SecurityHelper::ImpersonateAndGetUserName ImpersonateLoggedOnUser Error: %s", Utils::GetLastErrorString().c_str()));
    }
    return result;
}
