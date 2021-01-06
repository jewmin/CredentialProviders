#pragma once

#include <credentialprovider.h>
#include <windows.h>
#include <strsafe.h>

#include "dll.h"
#include "common.h"
#include "helpers.h"

class CSampleProviderFilter : public ICredentialProviderFilter {
	typedef struct _CREDENTIAL_PROVIDER_DESCRIPTOR
	{
		GUID guidType;
		LPWSTR pszLabel;
	} CREDENTIAL_PROVIDER_DESCRIPTOR;

public:
	IFACEMETHODIMP_(ULONG) AddRef()
	{
		return ++_cRef;
	}

	IFACEMETHODIMP_(ULONG) Release()
	{
		LONG cRef = --_cRef;
		if (!cRef) {
			delete this;
		}
		return cRef;
	}

	IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void** ppv)
	{
		static const QITAB qit[] = {
			QITABENT(CSampleProviderFilter, ICredentialProviderFilter), // IID_ICredentialProviderFilter
			{0},
		};
		return QISearch(this, qit, riid, ppv);
	}

public:
	IFACEMETHODIMP Filter(__in CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, __in DWORD dwFlags, __in GUID* rgclsidProviders, __inout BOOL* rgbAllow, __in DWORD cProviders);

	IFACEMETHODIMP UpdateRemoteCredential(__in const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsIn, __out CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsOut);

	friend HRESULT CFilter_CreateInstance(__in REFIID riid, __deref_out void** ppv);

protected:
	CSampleProviderFilter();
	__override ~CSampleProviderFilter();

private:
	LONG				_cRef; // 引用计数
};
