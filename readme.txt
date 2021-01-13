//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//

Overview
--------
This sample demonstrates how to use the credential provider APIs and interfaces such as ICredentialProvider, ICredentialProviderCredential, and ICredentialProviderFilter.

How to build these samples
--------------------------------
The top-level directory contains a solution file, CredentialProviderSamples.sln, that can be opened in Visual Studio or built using msbuild.exe in the SDK command line environment.

How to run this sample
--------------------------------
Each subfolder (for example, SampleCredentialProvider\ or SampleCredentialFilter\) contains a readme.txt file discussing how to run that sample.

Contents of this sample's subfolders:
-------------------
Helpers: common files for the other subfolders.

SampleAllControlsCredentialProvider: demonstrates the use of all credential provider field types.

SampleCredentialFilter: demonstrates the use of ICredentialProviderFilter.

SampleCredentialProvider: demonstrates the basic use of ICredentialProvider.

SampleCredUICredentialProvider: demonstrates a credential provider that runs in the CredUIPromptForWindowsCredentials dialog.

SampleHardwareEventCredentialProvider: demonstrates how a credential provider can respond to a hardware event such as smart card removal/insertion.

SampleWrapExistingCredentialProvider: demonstrates how a credential provider can "wrap" or contain another credential provider in order to add functionality.

使用说明

1）安装凭证
如果是x86系统，复制Win32的dll到C:\Windows\System32目录
如果是x64系统，复制x64的dll到C:\Windows\System32目录
管理员权限运行Register.reg

2）卸装凭证
管理员权限运行Unregister.reg

3）安装服务
使用管理员权限运行cmd，在cmd中执行指令DemoService.exe --install，然后可在服务管理中找到对应服务Demo Service

4）卸装服务
使用管理员权限运行cmd，在cmd中执行指令DemoService.exe --uninstall
