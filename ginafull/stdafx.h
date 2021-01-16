// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <winwlx.h>
#include <ntsecapi.h>
#include <limits.h>
#define STRSAFE_NO_CB_FUNCTIONS
#include <strsafe.h>

#include "Log.h"

// use these functions to get this DLL's module handle as an HMODULE or HINSTANCE
extern HMODULE   GetMyModuleHandle();
extern HINSTANCE GetMyInstance();

#define LOGFILE_PATH       L"c:\\ginalog.txt"
#define LOGON_PROCESS_NAME "GINA Sample"
#define MAX_SIZE_USER_NAME      21
#define MAX_SIZE_NB_DOMAIN_NAME 17

// the following three lines define __WFILE__, a Unicode version of __FILE__
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
