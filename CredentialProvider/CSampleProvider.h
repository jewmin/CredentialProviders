//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include <credentialprovider.h>
#include <windows.h>
#include <strsafe.h>

#include "AuthClient.h"
#include "CSampleCredential.h"
#include "helpers.h"
#include "CriticalSection.h"

class CSampleProvider : public ICredentialProvider, public ICredentialProviderSetUserArray
{
  public:
    // IUnknown
    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return ++_cRef;
    }
    
    IFACEMETHODIMP_(ULONG) Release()
    {
        LONG cRef = --_cRef;
        if (!cRef)
        {
            delete this;
        }
        return cRef;
    }

    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void** ppv)
    {
        static const QITAB qit[] =
        {
            QITABENT(CSampleProvider, ICredentialProvider), // IID_ICredentialProvider
			QITABENT(CSampleProvider, ICredentialProviderSetUserArray), // IID_ICredentialProviderSetUserArray
            {0},
        };
        return QISearch(this, qit, riid, ppv);
    }

  public:
    IFACEMETHODIMP SetUsageScenario(__in CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, __in DWORD dwFlags);
    IFACEMETHODIMP SetSerialization(__in const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcs);

    IFACEMETHODIMP Advise(__in ICredentialProviderEvents* pcpe, __in UINT_PTR upAdviseContext);
    IFACEMETHODIMP UnAdvise();

    IFACEMETHODIMP GetFieldDescriptorCount(__out DWORD* pdwCount);
    IFACEMETHODIMP GetFieldDescriptorAt(__in DWORD dwIndex,  __deref_out CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR** ppcpfd);

    IFACEMETHODIMP GetCredentialCount(__out DWORD* pdwCount,
                                      __out_range(<,*pdwCount) DWORD* pdwDefault,
                                      __out BOOL* pbAutoLogonWithDefault);
    IFACEMETHODIMP GetCredentialAt(__in DWORD dwIndex, 
                                   __deref_out ICredentialProviderCredential** ppcpc);

	IFACEMETHODIMP SetUserArray(__in ICredentialProviderUserArray *users);

    friend HRESULT CSample_CreateInstance(__in REFIID riid, __deref_out void** ppv);

  public:
    void Request(Utils::Protocol::LoginRequest * request);
    void OnResponse(bool bPassLogin, Utils::Protocol::LoginResponse * response);

  protected:
    CSampleProvider();
    __override ~CSampleProvider();
    
  private:
	HRESULT _EnumerateCredentials();
    
private:
    LONG                                    _cRef;              // Used for reference counting.
    CSampleCredential                       *_pCredential;      // Our credential.
    CREDENTIAL_PROVIDER_USAGE_SCENARIO      _cpus;
	ICredentialProviderUserArray			*_pCredProviderUserArray;
    ICredentialProviderEvents               *_pcpe;             // Used to tell our owner to re-enumerate credentials.
    UINT_PTR                                _upAdviseContext;   // Used to tell our owner who we are when asking to re-enumerate credentials.

public:
    bool                                    _bPassLogin;        // 使用密码登录
    AuthClient                              *_pAuthClient;      // 授权客户端
    int                                     _CurrentSn;         // 当前请求序号
    Utils::Protocol::LoginResponse          _lastLoginResponse; // 最后登录响应
    WORD                                    _lastLoginResult;   // 最后登录响应结果
    Utils::CCriticalSection                 _cs;                // 互斥锁
};
