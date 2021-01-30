//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//

#ifndef WIN32_NO_STATUS
#include <ntstatus.h>
#define WIN32_NO_STATUS
#endif
#include "Header.h"
#include <unknwn.h>
#include "CSampleCredential.h"
#include "guid.h"
#include "Utils.h"
#include "WindowsHelper.h"

// CSampleCredential ////////////////////////////////////////////////////////

CSampleCredential::CSampleCredential():
    _cRef(1),
    _pCredProvCredentialEvents(NULL)
{
	Utils::Output(L"CSampleCredential::CSampleCredential");
    DllAddRef();

    ZeroMemory(_rgCredProvFieldDescriptors, sizeof(_rgCredProvFieldDescriptors));
    ZeroMemory(_rgFieldStatePairs, sizeof(_rgFieldStatePairs));
    ZeroMemory(_rgFieldStrings, sizeof(_rgFieldStrings));
    _bChecked = FALSE;
    _dwComboIndex = 0;
}

CSampleCredential::~CSampleCredential()
{
	Utils::Output(L"CSampleCredential::~CSampleCredential");
    if (_rgFieldStrings[SFI_PASSWORD])
    {
        size_t lenPassword = lstrlen(_rgFieldStrings[SFI_PASSWORD]);
        SecureZeroMemory(_rgFieldStrings[SFI_PASSWORD], lenPassword * sizeof(*_rgFieldStrings[SFI_PASSWORD]));
    }
    for (int i = 0; i < ARRAYSIZE(_rgFieldStrings); i++)
    {
        CoTaskMemFree(_rgFieldStrings[i]);
        CoTaskMemFree(_rgCredProvFieldDescriptors[i].pszLabel);
    }

    DllRelease();
}

// 凭证初始化处理
// Initializes one credential with the field information passed in.
// Set the value of the SFI_LARGE_TEXT field to pwzUsername.
HRESULT CSampleCredential::Initialize(
    __in CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
    __in const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR* rgcpfd,
    __in const FIELD_STATE_PAIR* rgfsp
    )
{
	Utils::Output(Utils::StringFormat(L"CSampleCredential::Initialize cpus: %s", s_CPUS_Strings[cpus]));
    HRESULT hr = S_OK;

    _cpus = cpus;

    // Copy the field descriptors for each field. This is useful if you want to vary the field
    // descriptors based on what Usage scenario the credential was created for.
    for (DWORD i = 0; SUCCEEDED(hr) && i < ARRAYSIZE(_rgCredProvFieldDescriptors); i++)
    {
        _rgFieldStatePairs[i] = rgfsp[i];
        hr = FieldDescriptorCopy(rgcpfd[i], &_rgCredProvFieldDescriptors[i]);
    }

    // Initialize the String value of all the fields. 
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"终端名", &_rgFieldStrings[SFI_LARGE_TEXT]);
    }
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"", &_rgFieldStrings[SFI_SMALL_TEXT]);
    }
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"", &_rgFieldStrings[SFI_EDIT_TEXT]);
    }
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"", &_rgFieldStrings[SFI_PASSWORD]);
    }
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"Submit", &_rgFieldStrings[SFI_SUBMIT_BUTTON]);
    }
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"Checkbox", &_rgFieldStrings[SFI_CHECKBOX]);
    }
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"Combobox", &_rgFieldStrings[SFI_COMBOBOX]);
    }
    if (SUCCEEDED(hr))
    {
        hr = SHStrDupW(L"Command Link", &_rgFieldStrings[SFI_COMMAND_LINK]);
    }
	if (SUCCEEDED(hr))
	{
		hr = SHStrDupW(L"", &_rgFieldStrings[SFI_USERNAME]);
	}
	if (SUCCEEDED(hr))
	{
		hr = SHStrDupW(L"", &_rgFieldStrings[SFI_FAILURE_TEXT]);
	}

    return S_OK;
}

// LogonUI calls this in order to give us a callback in case we need to notify it of anything.
HRESULT CSampleCredential::Advise(
    __in ICredentialProviderCredentialEvents* pcpce
    )
{
	Utils::Output(Utils::StringFormat(L"CSampleCredential::Advise pcpce: %p", pcpce));
    if (_pCredProvCredentialEvents != NULL)
    {
        _pCredProvCredentialEvents->Release();
    }
    _pCredProvCredentialEvents = pcpce;
    _pCredProvCredentialEvents->AddRef();
    return S_OK;
}

// LogonUI calls this to tell us to release the callback.
HRESULT CSampleCredential::UnAdvise()
{
	Utils::Output(L"CSampleCredential::UnAdvise");
    if (_pCredProvCredentialEvents)
    {
        _pCredProvCredentialEvents->Release();
    }
    _pCredProvCredentialEvents = NULL;
    return S_OK;
}

// LogonUI calls this function when our tile is selected (zoomed)
// If you simply want fields to show/hide based on the selected state,
// there's no need to do anything here - you can set that up in the 
// field definitions. But if you want to do something
// more complicated, like change the contents of a field when the tile is
// selected, you would do it here.
HRESULT CSampleCredential::SetSelected(__out BOOL* pbAutoLogon)  
{
    *pbAutoLogon = FALSE;
	Utils::Output(Utils::StringFormat(L"CSampleCredential::SetSelected *pbAutoLogon: %s", *pbAutoLogon ? L"TRUE" : L"FALSE"));
    return S_OK;
}

// Similarly to SetSelected, LogonUI calls this when your tile was selected
// and now no longer is. The most common thing to do here (which we do below)
// is to clear out the password field.
HRESULT CSampleCredential::SetDeselected()
{
	Utils::Output(L"CSampleCredential::SetDeselected");
    HRESULT hr = S_OK;
    if (_rgFieldStrings[SFI_PASSWORD])
    {
        size_t lenPassword = lstrlen(_rgFieldStrings[SFI_PASSWORD]);
        SecureZeroMemory(_rgFieldStrings[SFI_PASSWORD], lenPassword * sizeof(*_rgFieldStrings[SFI_PASSWORD]));

        CoTaskMemFree(_rgFieldStrings[SFI_PASSWORD]);
        hr = SHStrDupW(L"", &_rgFieldStrings[SFI_PASSWORD]);

        if (SUCCEEDED(hr) && _pCredProvCredentialEvents)
        {
            _pCredProvCredentialEvents->SetFieldString(this, SFI_PASSWORD, _rgFieldStrings[SFI_PASSWORD]);
        }
    }

    return hr;
}

// Get info for a particular field of a tile. Called by logonUI to get information 
// to display the tile.
HRESULT CSampleCredential::GetFieldState(
    __in DWORD dwFieldID,
    __out CREDENTIAL_PROVIDER_FIELD_STATE* pcpfs,
    __out CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE* pcpfis
    )
{
    HRESULT hr;
    
    // Validate our parameters.
    if ((dwFieldID < ARRAYSIZE(_rgFieldStatePairs)) && pcpfs && pcpfis)
    {
        *pcpfs = _rgFieldStatePairs[dwFieldID].cpfs;
        *pcpfis = _rgFieldStatePairs[dwFieldID].cpfis;
		Utils::Output(Utils::StringFormat(L"CSampleCredential::GetFieldState dwFieldID: %u, *pcpfs: %s, *pcpfis: %s", dwFieldID, s_CPFS_Strings[*pcpfs], s_CPFIS_Strings[*pcpfis]));
        hr = S_OK;
    }
    else
    {
		Utils::Output(Utils::StringFormat(L"CSampleCredential::GetFieldState dwFieldID: %u", dwFieldID));
        hr = E_INVALIDARG;
    }
    return hr;
}

// Sets ppwsz to the string value of the field at the index dwFieldID
HRESULT CSampleCredential::GetStringValue(
    __in DWORD dwFieldID, 
    __deref_out PWSTR* ppwsz
    )
{
    HRESULT hr;

    // Check to make sure dwFieldID is a legitimate index
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) && ppwsz) 
    {
        // Make a copy of the string and return that. The caller
        // is responsible for freeing it.
        hr = SHStrDupW(_rgFieldStrings[dwFieldID], ppwsz);
		if (*ppwsz) {
			Utils::Output(Utils::StringFormat(L"CSampleCredential::GetStringValue dwFieldID: %u, *ppwsz: %s", dwFieldID, *ppwsz));
		}
		else {
			Utils::Output(Utils::StringFormat(L"CSampleCredential::GetStringValue dwFieldID: %u", dwFieldID));
		}
    }
    else
    {
		Utils::Output(Utils::StringFormat(L"CSampleCredential::GetStringValue dwFieldID: %u", dwFieldID));
        hr = E_INVALIDARG;
    }

    return hr;
}

// Get the image to show in the user tile
HRESULT CSampleCredential::GetBitmapValue(
    __in DWORD dwFieldID, 
    __out HBITMAP* phbmp
    )
{
	Utils::Output(Utils::StringFormat(L"CSampleCredential::GetBitmapValue dwFieldID: %u", dwFieldID));
    HRESULT hr;
    if ((SFI_TILEIMAGE == dwFieldID) && phbmp)
    {
        HBITMAP hbmp = LoadBitmap(HINST_THISDLL, MAKEINTRESOURCE(IDB_TILE_IMAGE));
        if (hbmp != NULL)
        {
            hr = S_OK;
            *phbmp = hbmp;
        }
        else
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Sets pdwAdjacentTo to the index of the field the submit button should be 
// adjacent to. We recommend that the submit button is placed next to the last
// field which the user is required to enter information in. Optional fields
// should be below the submit button.
HRESULT CSampleCredential::GetSubmitButtonValue(
    __in DWORD dwFieldID,
    __out DWORD* pdwAdjacentTo
    )
{
	Utils::Output(Utils::StringFormat(L"CSampleCredential::GetSubmitButtonValue dwFieldID: %u", dwFieldID));
    HRESULT hr;

    if (SFI_SUBMIT_BUTTON == dwFieldID && pdwAdjacentTo)
    {
        // pdwAdjacentTo is a pointer to the fieldID you want the submit button to 
        // appear next to.
        *pdwAdjacentTo = SFI_PASSWORD;
        hr = S_OK;
    }
    else
    {
        hr = E_INVALIDARG;
    }
    return hr;
}

// Sets the value of a field which can accept a string as a value.
// This is called on each keystroke when a user types into an edit field
HRESULT CSampleCredential::SetStringValue(
    __in DWORD dwFieldID, 
    __in PCWSTR pwz      
    )
{
	if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) && CPFT_EDIT_TEXT == _rgCredProvFieldDescriptors[dwFieldID].cpft) {
		Utils::Output(Utils::StringFormat(L"CSampleCredential::SetStringValue dwFieldID: %u, pwz: %s", dwFieldID, pwz));
	}
	else {
		Utils::Output(Utils::StringFormat(L"CSampleCredential::SetStringValue dwFieldID: %u", dwFieldID));
	}
    HRESULT hr;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) && 
        (CPFT_EDIT_TEXT == _rgCredProvFieldDescriptors[dwFieldID].cpft || 
        CPFT_PASSWORD_TEXT == _rgCredProvFieldDescriptors[dwFieldID].cpft)) 
    {
        PWSTR* ppwszStored = &_rgFieldStrings[dwFieldID];
        CoTaskMemFree(*ppwszStored);
        hr = SHStrDupW(pwz, ppwszStored);
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Returns whether a checkbox is checked or not as well as its label.
HRESULT CSampleCredential::GetCheckboxValue(
    __in DWORD dwFieldID, 
    __in BOOL* pbChecked,
    __deref_out PWSTR* ppwszLabel
    )
{
    HRESULT hr;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) && 
        (CPFT_CHECKBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        *pbChecked = _bChecked;
        hr = SHStrDupW(_rgFieldStrings[SFI_CHECKBOX], ppwszLabel);
		if (*ppwszLabel) {
			Utils::Output(Utils::StringFormat(L"CSampleCredential::GetCheckboxValue dwFieldID: %u, *pbChecked: %s, *ppwszLabel: %s", dwFieldID, *pbChecked ? L"TRUE" : L"FALSE", *ppwszLabel));
		}
		else {
			Utils::Output(Utils::StringFormat(L"CSampleCredential::GetCheckboxValue dwFieldID: %u, *pbChecked: %s", dwFieldID, *pbChecked ? L"TRUE" : L"FALSE"));
		}
    }
    else
    {
		Utils::Output(Utils::StringFormat(L"CSampleCredential::GetCheckboxValue dwFieldID: %u", dwFieldID));
        hr = E_INVALIDARG;
    }

    return hr;
}

// Sets whether the specified checkbox is checked or not.
HRESULT CSampleCredential::SetCheckboxValue(
    __in DWORD dwFieldID, 
    __in BOOL bChecked
    )
{
	Utils::Output(Utils::StringFormat(L"CSampleCredential::SetCheckboxValue dwFieldID: %u, bChecked: %s", dwFieldID, bChecked ? L"TRUE" : L"FALSE"));
    HRESULT hr;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) && 
        (CPFT_CHECKBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        _bChecked = bChecked;
        hr = S_OK;
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Returns the number of items to be included in the combobox (pcItems), as well as the 
// currently selected item (pdwSelectedItem).
HRESULT CSampleCredential::GetComboBoxValueCount(
    __in DWORD dwFieldID, 
    __out DWORD* pcItems, 
    __out_range(<,*pcItems) DWORD* pdwSelectedItem
    )
{
    HRESULT hr;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) && 
        (CPFT_COMBOBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        *pcItems = ARRAYSIZE(s_rgComboBoxStrings);
        *pdwSelectedItem = 0;
		Utils::Output(Utils::StringFormat(L"CSampleCredential::GetComboBoxValueCount dwFieldID: %u, *pcItems: %u, *pdwSelectedItem: %u", dwFieldID, *pcItems, *pdwSelectedItem));
        hr = S_OK;
    }
    else
    {
		Utils::Output(Utils::StringFormat(L"CSampleCredential::GetComboBoxValueCount dwFieldID: %u", dwFieldID));
        hr = E_INVALIDARG;
    }

    return S_OK;
}

// Called iteratively to fill the combobox with the string (ppwszItem) at index dwItem.
HRESULT CSampleCredential::GetComboBoxValueAt(
    __in DWORD dwFieldID, 
    __in DWORD dwItem,
    __deref_out PWSTR* ppwszItem
    )
{
    HRESULT hr;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) && 
        (CPFT_COMBOBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        hr = SHStrDupW(s_rgComboBoxStrings[dwItem], ppwszItem);
		if (*ppwszItem) {
			Utils::Output(Utils::StringFormat(L"CSampleCredential::GetComboBoxValueAt dwFieldID: %u, dwItem: %u, *ppwszItem: %s", dwFieldID, dwItem, *ppwszItem));
		}
		else {
			Utils::Output(Utils::StringFormat(L"CSampleCredential::GetComboBoxValueAt dwFieldID: %u, dwItem: %u", dwFieldID, dwItem));
		}
    }
    else
    {
		Utils::Output(Utils::StringFormat(L"CSampleCredential::GetComboBoxValueAt dwFieldID: %u, dwItem: %u", dwFieldID, dwItem));
        hr = E_INVALIDARG;
    }

    return hr;
}

// Called when the user changes the selected item in the combobox.
HRESULT CSampleCredential::SetComboBoxSelectedValue(
    __in DWORD dwFieldID,
    __in DWORD dwSelectedItem
    )
{
	Utils::Output(Utils::StringFormat(L"CSampleCredential::SetComboBoxSelectedValue dwFieldID: %u, dwSelectedItem: %u", dwFieldID, dwSelectedItem));
    HRESULT hr;

    // Validate parameters.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) && 
        (CPFT_COMBOBOX == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        _dwComboIndex = dwSelectedItem;
        hr = S_OK;
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

// Called when the user clicks a command link.
HRESULT CSampleCredential::CommandLinkClicked(__in DWORD dwFieldID)
{
	Utils::Output(Utils::StringFormat(L"CSampleCredential::CommandLinkClicked dwFieldID: %u", dwFieldID));
    HRESULT hr;

    // Validate parameter.
    if (dwFieldID < ARRAYSIZE(_rgCredProvFieldDescriptors) && 
        (CPFT_COMMAND_LINK == _rgCredProvFieldDescriptors[dwFieldID].cpft))
    {
        HWND hwndOwner = NULL;

        if (_pCredProvCredentialEvents)
        {
            _pCredProvCredentialEvents->OnCreatingWindow(&hwndOwner);
        }

        // Pop a messagebox indicating the click.
        ::MessageBox(hwndOwner, L"Command link clicked", L"Click!", 0);
        hr = S_OK;
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

struct AUTH_RESPONSE
{
	WORD      wResult;
	PWSTR     pwzMessage;
};

static const AUTH_RESPONSE s_rgAuthResponse[] =
{
	{ Utils::Protocol::LoginResponse::AuthFailed, L"ERROR: 授权失败" },
	{ Utils::Protocol::LoginResponse::UserCancel, L"ERROR: 用户取消" },
	{ Utils::Protocol::LoginResponse::Timeout, L"ERROR: 授权超时" },
	{ Utils::Protocol::LoginResponse::ConnectFailed, L"ERROR: 连接失败" },
};

// Connect返回后，系统会调用此方法，这里是系统真正的登录验证
// Collect the username and password into a serialized credential for the correct usage scenario 
// (logon/unlock is what's demonstrated in this sample).  LogonUI then passes these credentials 
// back to the system to log on.
HRESULT CSampleCredential::GetSerialization(
    __out CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE* pcpgsr,
    __out CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcs, 
    __deref_out_opt PWSTR* ppwszOptionalStatusText, 
    __in CREDENTIAL_PROVIDER_STATUS_ICON* pcpsiOptionalStatusIcon
    )
{
	Utils::Output(L"CSampleCredential::GetSerialization");
    *ppwszOptionalStatusText = NULL;
    *pcpsiOptionalStatusIcon = CPSI_NONE;

	HRESULT hr;

    // 结合之前授权返回的结果，判断是否授权成功，成功则走登录验证，失败则直接返回，并显示错误信息
	if (_response.Result != Utils::Protocol::LoginResponse::AuthSuccess)
	{
		CoTaskMemFree(_rgFieldStrings[SFI_FAILURE_TEXT]);
		hr = SHStrDupW(L"未知错误，请联系管理员", &_rgFieldStrings[SFI_FAILURE_TEXT]);
		if (SUCCEEDED(hr))
		{
			for (DWORD i = 0; i < ARRAYSIZE(s_rgAuthResponse); i++)
			{
				if (s_rgAuthResponse[i].wResult == _response.Result)
				{
					CoTaskMemFree(_rgFieldStrings[SFI_FAILURE_TEXT]);
					hr = SHStrDupW(s_rgAuthResponse[i].pwzMessage, &_rgFieldStrings[SFI_FAILURE_TEXT]);
					break;
				}
			}
		}

		if (SUCCEEDED(hr) && _pCredProvCredentialEvents)
		{
			_pCredProvCredentialEvents->SetFieldString(this, SFI_FAILURE_TEXT, _rgFieldStrings[SFI_FAILURE_TEXT]);
			_pCredProvCredentialEvents->SetFieldState(this, SFI_FAILURE_TEXT, CPFS_DISPLAY_IN_SELECTED_TILE);
			_pCredProvCredentialEvents->SetFieldString(this, SFI_PASSWORD, L"");
		}

		if (SUCCEEDED(hr))
		{
			if (SUCCEEDED(SHStrDupW(_rgFieldStrings[SFI_FAILURE_TEXT], ppwszOptionalStatusText)))
			{
				*pcpsiOptionalStatusIcon = CPSI_ERROR;
			}
		}
		*pcpgsr = CPGSR_NO_CREDENTIAL_NOT_FINISHED;
		return E_UNEXPECTED;
	}

	if (_pCredProvCredentialEvents)
	{
		_pCredProvCredentialEvents->SetFieldState(this, SFI_FAILURE_TEXT, CPFS_HIDDEN);
	}

    WCHAR wsz[MAX_COMPUTERNAME_LENGTH+1];
    DWORD cch = ARRAYSIZE(wsz);
    if (GetComputerNameW(wsz, &cch))
    {
        PWSTR pwzProtectedPassword;

        hr = ProtectIfNecessaryAndCopyPassword(_rgFieldStrings[SFI_PASSWORD], _cpus, &pwzProtectedPassword);

        if (SUCCEEDED(hr))
        {
            KERB_INTERACTIVE_UNLOCK_LOGON kiul;

            hr = KerbInteractiveUnlockLogonInit(wsz, _rgFieldStrings[SFI_USERNAME], pwzProtectedPassword, _cpus, &kiul);

            if (SUCCEEDED(hr))
            {
                // We use KERB_INTERACTIVE_UNLOCK_LOGON in both unlock and logon scenarios.  It contains a
                // KERB_INTERACTIVE_LOGON to hold the creds plus a LUID that is filled in for us by Winlogon
                // as necessary.
                hr = KerbInteractiveUnlockLogonPack(kiul, &pcpcs->rgbSerialization, &pcpcs->cbSerialization);

                if (SUCCEEDED(hr))
                {
                    ULONG ulAuthPackage;
                    hr = RetrieveNegotiateAuthPackage(&ulAuthPackage);
                    if (SUCCEEDED(hr))
                    {
                        pcpcs->ulAuthenticationPackage = ulAuthPackage;
                        pcpcs->clsidCredentialProvider = CLSID_CSample;
 
                        // At this point the credential has created the serialized credential used for logon
                        // By setting this to CPGSR_RETURN_CREDENTIAL_FINISHED we are letting logonUI know
                        // that we have all the information we need and it should attempt to submit the 
                        // serialized credential.
                        *pcpgsr = CPGSR_RETURN_CREDENTIAL_FINISHED;
                    }
                }
            }

            CoTaskMemFree(pwzProtectedPassword);
        }
    }
    else
    {
        DWORD dwErr = GetLastError();
        hr = HRESULT_FROM_WIN32(dwErr);
    }

    return hr;
}

struct REPORT_RESULT_STATUS_INFO
{
    NTSTATUS ntsStatus;
    NTSTATUS ntsSubstatus;
    PWSTR     pwzMessage;
    CREDENTIAL_PROVIDER_STATUS_ICON cpsi;
};

static const REPORT_RESULT_STATUS_INFO s_rgLogonStatusInfo[] =
{
    { STATUS_LOGON_FAILURE, STATUS_SUCCESS, L"Incorrect password or username.", CPSI_ERROR, },
    { STATUS_ACCOUNT_RESTRICTION, STATUS_ACCOUNT_DISABLED, L"The account is disabled.", CPSI_WARNING },
};

// ReportResult is completely optional.  Its purpose is to allow a credential to customize the string
// and the icon displayed in the case of a logon failure.  For example, we have chosen to 
// customize the error shown in the case of bad username/password and in the case of the account
// being disabled.
HRESULT CSampleCredential::ReportResult(
    __in NTSTATUS ntsStatus, 
    __in NTSTATUS ntsSubstatus,
    __deref_out_opt PWSTR* ppwszOptionalStatusText, 
    __out CREDENTIAL_PROVIDER_STATUS_ICON* pcpsiOptionalStatusIcon
    )
{
	DWORD sessionid = Utils::GetCurrentSessionId();
	Utils::Output(Utils::StringFormat(L"CSampleCredential::ReportResult ntsStatus: %d, ntsSubstatus: %d", ntsStatus, ntsSubstatus));
	Utils::Output(Utils::StringFormat(L"CSampleCredential::ReportResult SessionId: %u, DomainName: %s, UserName: %s, MachineName: %s", sessionid, Utils::GetSessionDomainName(sessionid).c_str(), Utils::GetSessionUserName(sessionid).c_str(), Utils::GetMachineName().c_str()));
    *ppwszOptionalStatusText = NULL;
    *pcpsiOptionalStatusIcon = CPSI_NONE;

    DWORD dwStatusInfo = (DWORD)-1;

    // Look for a match on status and substatus.
    for (DWORD i = 0; i < ARRAYSIZE(s_rgLogonStatusInfo); i++)
    {
        if (s_rgLogonStatusInfo[i].ntsStatus == ntsStatus && s_rgLogonStatusInfo[i].ntsSubstatus == ntsSubstatus)
        {
            dwStatusInfo = i;
            break;
        }
    }

    if ((DWORD)-1 != dwStatusInfo)
    {
        if (SUCCEEDED(SHStrDupW(s_rgLogonStatusInfo[dwStatusInfo].pwzMessage, ppwszOptionalStatusText)))
        {
            *pcpsiOptionalStatusIcon = s_rgLogonStatusInfo[dwStatusInfo].cpsi;
        }
    }

    // If we failed the logon, try to erase the password field.
    if (!SUCCEEDED(HRESULT_FROM_NT(ntsStatus)))
    {
        if (_pCredProvCredentialEvents)
        {
            _pCredProvCredentialEvents->SetFieldString(this, SFI_PASSWORD, L"");
        }
    }

    // Since NULL is a valid value for *ppwszOptionalStatusText and *pcpsiOptionalStatusIcon
    // this function can't fail.
    return S_OK;
}

// 输入登录名和密码后，首先进入到这里，在这里进行授权验证，使用类似同步socket阻塞模式
HRESULT CSampleCredential::Connect(_In_ IQueryContinueWithStatus* pqcws)
{
	Utils::Output(Utils::StringFormat(L"CSampleCredential::Connect pqcws: %p", pqcws));
	AuthClient client(5000, pqcws);
	Utils::Protocol::LoginRequest request(Utils::GetCurrentSessionId(), _rgFieldStrings[SFI_EDIT_TEXT], _rgFieldStrings[SFI_PASSWORD]);
	client.Auth(&request, &_response);

    // 复制真正的系统用户名和密码
	HRESULT hr;
	CoTaskMemFree(_rgFieldStrings[SFI_USERNAME]);
	hr = SHStrDupW(_response.UserName, &_rgFieldStrings[SFI_USERNAME]);
	if (SUCCEEDED(hr))
	{
		CoTaskMemFree(_rgFieldStrings[SFI_PASSWORD]);
		hr = SHStrDupW(_response.Password, &_rgFieldStrings[SFI_PASSWORD]);
	}

	return S_OK;
}

HRESULT CSampleCredential::Disconnect()
{
	Utils::Output(L"CSampleCredential::Disconnect");
	return E_NOTIMPL;
}
