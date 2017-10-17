/**
 * Copyright (c) 2017-2050
 * All rights reserved.
 *
 * @Author:MXWXZ
 * @Date:2017/10/17
 *
 * @Description:The defination file of UI library
 */
#pragma once

// Library export/import define
// DO NOT DEFINE [UILIB_EXPORT] IN YOUR MAIN PROGRAM!
// Please add [UILIB_STATIC] as a preprocessor definition.
#ifdef UILIB_STATIC
#   define DUIMINI_API
#else
#   if defined(UILIB_EXPORT)
#       define DUIMINI_API __declspec(dllexport)
#   else
#       define DUIMINI_API __declspec(dllimport)
#   endif
#endif

#define DUIMINI_VERSION _T("1.0")           // UI library version

#define RESOURCE_KIND _T("ZIP")             // Custom resource type

// Exit code
#define EXITCODE_SUCCESS 0
#define EXITCODE_FILEFAIL 1

typedef unzFile ZFile;
#ifdef _UNICODE
typedef std::wstring tstring;    // NOLINT
#else
typedef std::string tstring;     // NOLINT
#endif  // _UNICODE

#pragma warning(disable: 4251)            // warnings when exporting string
