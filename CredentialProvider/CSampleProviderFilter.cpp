#include "Header.h"
#include <credentialprovider.h>
#include "CSampleProviderFilter.h"
#include "guid.h"
#include "Utils.h"

CSampleProviderFilter::CSampleProviderFilter()
	: _cRef(1)
{
	DllAddRef();
	Utils::Output(L"CSampleProviderFilter::CSampleProviderFilter");
}

CSampleProviderFilter::~CSampleProviderFilter()
{
	Utils::Output(L"CSampleProviderFilter::~CSampleProviderFilter");
	DllRelease();
}

HRESULT CSampleProviderFilter::Filter(
	__in CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
	__in DWORD dwFlags,
	__in GUID* rgclsidProviders,
	__inout BOOL* rgbAllow,
	__in DWORD cProviders
	)
{
	Utils::Output(Utils::StringFormat(L"CSampleProviderFilter::Filter cpus: %s, dwFlags: %u, cProviders: %u", s_CPUS_Strings[cpus], dwFlags, cProviders));
	UNREFERENCED_PARAMETER(dwFlags);
	HRESULT hr;

	static const CREDENTIAL_PROVIDER_DESCRIPTOR s_rgCredProvDescriptors[] =
	{
		{ CLSID_PasswordCredentialProvider, L"PasswordCredentialProvider" },
		{ CLSID_V1PasswordCredentialProvider, L"V1PasswordCredentialProvider" },
		{ CLSID_PINLogonCredentialProvider, L"PINLogonCredentialProvider" },
		{ CLSID_NPCredentialProvider, L"NPCredentialProvider" },
		{ CLSID_SmartcardCredentialProvider, L"SmartcardCredentialProvider" },
		{ CLSID_V1SmartcardCredentialProvider, L"V1SmartcardCredentialProvider" },
		{ CLSID_SmartcardPinProvider, L"SmartcardPinProvider" },
		{ CLSID_SmartcardReaderSelectionProvider, L"SmartcardReaderSelectionProvider" },
		{ CLSID_SmartcardWinRTProvider, L"SmartcardWinRTProvider" },
		{ CLSID_GenericCredentialProvider, L"GenericCredentialProvider" },
		{ CLSID_RASProvider, L"RASProvider" },
		{ CLSID_OnexCredentialProvider, L"OnexCredentialProvider" },
		{ CLSID_OnexPlapSmartcardCredentialProvider, L"OnexPlapSmartcardCredentialProvider" },
		{ CLSID_VaultProvider, L"VaultProvider" },
		{ CLSID_WinBioCredentialProvider, L"WinBioCredentialProvider" },
		{ CLSID_V1WinBioCredentialProvider, L"V1WinBioCredentialProvider" },
		{ CLSID_CSample, L"SampleAllControlsCredentialProvider" },
	};

	switch (cpus) {
	case CPUS_LOGON:
	case CPUS_UNLOCK_WORKSTATION:
		for (DWORD i = 0; i < cProviders; i++) {
			if (IsEqualGUID(rgclsidProviders[i], CLSID_CSample)) {
				rgbAllow[i] = TRUE;
			}
			else {
				rgbAllow[i] = FALSE;
			}

			bool found = false;
			for (DWORD j = 0; j < ARRAYSIZE(s_rgCredProvDescriptors); j++) {
				if (IsEqualGUID(rgclsidProviders[i], s_rgCredProvDescriptors[j].guidType)) {
					Utils::Output(Utils::StringFormat(L"CSampleProviderFilter::Filter %u rgbAllow: %s, rgclsidProviders: %s", i, rgbAllow[i] ? L"TRUE" : L"FALSE", s_rgCredProvDescriptors[j].pszLabel));
					found = true;
					break;
				}
			}
			if (!found) {
				LPOLESTR clsid;
				StringFromCLSID(rgclsidProviders[i], &clsid);
				Utils::Output(Utils::StringFormat(L"CSampleProviderFilter::Filter %u rgbAllow: %s, rgclsidProviders: %s", i, rgbAllow[i] ? L"TRUE" : L"FALSE", clsid));
				CoTaskMemFree(clsid);
			}
		}
		hr = S_OK;
		break;

	case CPUS_CHANGE_PASSWORD:
	case CPUS_CREDUI:
		hr = E_NOTIMPL;
		break;

	default:
		hr = E_INVALIDARG;
		break;
	}

	return hr;
}

HRESULT CSampleProviderFilter::UpdateRemoteCredential(
	__in const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsIn,
	__out CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsOut
	)
{
	Utils::Output(Utils::StringFormat(L"CSampleProviderFilter::UpdateRemoteCredential pcpcsIn: %p", pcpcsIn));
	pcpcsOut->ulAuthenticationPackage = pcpcsIn->ulAuthenticationPackage;
	pcpcsOut->cbSerialization = pcpcsIn->cbSerialization;
	pcpcsOut->rgbSerialization = pcpcsIn->rgbSerialization;
	pcpcsOut->clsidCredentialProvider = CLSID_CSample;
	if (pcpcsOut->cbSerialization > 0) {
		pcpcsOut->rgbSerialization = (BYTE*)CoTaskMemAlloc(pcpcsOut->cbSerialization);
		if (pcpcsOut->rgbSerialization) {
			CopyMemory(pcpcsOut->rgbSerialization, pcpcsIn->rgbSerialization, pcpcsOut->cbSerialization);
			return S_OK;
		}
		else {
			return E_OUTOFMEMORY;
		}
	}
	return E_NOTIMPL;
}

HRESULT CFilter_CreateInstance(__in REFIID riid, __deref_out void** ppv)
{
	HRESULT hr;

	CSampleProviderFilter* pFilter = new CSampleProviderFilter();

	if (pFilter)
	{
		hr = pFilter->QueryInterface(riid, ppv);
		pFilter->Release();
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}

	return hr;
}
