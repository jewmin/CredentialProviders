//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// This file contains some global variables that describe what our
// sample tile looks like.  For example, it defines what fields a tile has 
// and which fields show in which states of LogonUI. This sample illustrates
// the use of each UI field type.

#pragma once
#include <helpers.h>

// 界面上新增控件，在这里添加
// The indexes of each of the fields in our credential provider's tiles. Note that we're
// using each of the nine available field types here.
enum SAMPLE_FIELD_ID 
{
    SFI_TILEIMAGE       = 0,
    SFI_LARGE_TEXT      = 1,
    SFI_SMALL_TEXT      = 2,
    SFI_EDIT_TEXT       = 3,
    SFI_PASSWORD        = 4,
    SFI_SUBMIT_BUTTON   = 5, 
    SFI_CHECKBOX        = 6,
    SFI_COMBOBOX        = 7,
    SFI_COMMAND_LINK    = 8,
	SFI_USERNAME        = 9,
	SFI_FAILURE_TEXT    = 10,
    SFI_NUM_FIELDS      = 11,  // Note: if new fields are added, keep NUM_FIELDS last.  This is used as a count of the number of fields
};

// The first value indicates when the tile is displayed (selected, not selected)
// the second indicates things like whether the field is enabled, whether it has key focus, etc.
struct FIELD_STATE_PAIR
{
    CREDENTIAL_PROVIDER_FIELD_STATE cpfs;
    CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE cpfis;
};

// These two arrays are seperate because a credential provider might
// want to set up a credential with various combinations of field state pairs 
// and field descriptors.

// 这里添加控制显示状态
// The field state value indicates whether the field is displayed
// in the selected tile, the deselected tile, or both.
// The Field interactive state indicates when 
static const FIELD_STATE_PAIR s_rgFieldStatePairs[] = 
{
    { CPFS_DISPLAY_IN_BOTH, CPFIS_NONE },                   // SFI_TILEIMAGE
    { CPFS_DISPLAY_IN_BOTH, CPFIS_NONE },                   // SFI_LARGE_TEXT
    { CPFS_DISPLAY_IN_DESELECTED_TILE, CPFIS_NONE    },     // SFI_SMALL_TEXT   
    { CPFS_DISPLAY_IN_SELECTED_TILE, CPFIS_NONE    },       // SFI_EDIT_TEXT   
    { CPFS_DISPLAY_IN_SELECTED_TILE, CPFIS_FOCUSED },       // SFI_PASSWORD
    { CPFS_DISPLAY_IN_SELECTED_TILE, CPFIS_NONE    },       // SFI_SUBMIT_BUTTON   
    { CPFS_DISPLAY_IN_SELECTED_TILE, CPFIS_NONE  },         // SFI_CHECKBOX   
    { CPFS_DISPLAY_IN_SELECTED_TILE, CPFIS_NONE    },       // SFI_COMBOBOX   
    { CPFS_DISPLAY_IN_SELECTED_TILE, CPFIS_NONE    },       // SFI_COMMAND_LINK   
	{ CPFS_HIDDEN, CPFIS_NONE},                             // SFI_USERNAME
	{ CPFS_HIDDEN, CPFIS_NONE},                             // SFI_FAILURE_TEXT
};

// 这里添加控制类型
// Field descriptors for unlock and logon.
// The first field is the index of the field.
// The second is the type of the field.
// The third is the name of the field, NOT the value which will appear in the field.
static const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR s_rgCredProvFieldDescriptors[] =
{
    { SFI_TILEIMAGE, CPFT_TILE_IMAGE, L"Image" },
    { SFI_LARGE_TEXT, CPFT_LARGE_TEXT, L"LargeText" },
    { SFI_SMALL_TEXT, CPFT_SMALL_TEXT, L"SmallText" },
    { SFI_EDIT_TEXT, CPFT_EDIT_TEXT, L"EditText" },
    { SFI_PASSWORD, CPFT_PASSWORD_TEXT, L"Password" },
    { SFI_SUBMIT_BUTTON, CPFT_SUBMIT_BUTTON, L"Submit" },
    { SFI_CHECKBOX, CPFT_CHECKBOX, L"Checkbox" },
    { SFI_COMBOBOX, CPFT_COMBOBOX, L"Combobox" },
    { SFI_COMMAND_LINK, CPFT_COMMAND_LINK, L"CommandLink" },
	{ SFI_USERNAME, CPFT_EDIT_TEXT, L"UserName" },
	{ SFI_FAILURE_TEXT, CPFT_SMALL_TEXT, L"Failure" },
};

static const PWSTR s_rgComboBoxStrings[] =
{
    L"仪器1",
    L"仪器2",
    L"仪器3",
};

static const PWSTR s_CPUS_Strings[] = 
{
	L"CPUS_INVALID",
	L"CPUS_LOGON",
	L"CPUS_UNLOCK_WORKSTATION",
	L"CPUS_CHANGE_PASSWORD",
	L"CPUS_CREDUI",
	L"CPUS_PLAP",
};

static const PWSTR s_CPFS_Strings[] =
{
	L"CPFS_HIDDEN",
	L"CPFS_DISPLAY_IN_SELECTED_TILE",
	L"CPFS_DISPLAY_IN_DESELECTED_TILE",
	L"CPFS_DISPLAY_IN_BOTH",
};

static const PWSTR s_CPFIS_Strings[] =
{
	L"CPFIS_NONE",
	L"CPFIS_READONLY",
	L"CPFIS_DISABLED",
	L"CPFIS_FOCUSED",
};
