//////////////////////////////////////////////////////////
// File Name: tchars.h
//
// Author : Little Chio
// Created: 2003/6/29
// Last Modified : 2013/4/8
//
// Copyright (c) 2003, 2007, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////

#ifndef __TCHARS_H__
#define __TCHARS_H__

//#pragma warning (disable:4530 4786)


#include <iostream>
#include <sstream>
#include <string>

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__GNUC__)	// If using Microsoft or Borland or GNU C++ compiler, do the following.
	#pragma pack(push, 8)						// To confirm the alignment of the structure members is the same under any case. Otherwise, strange bugs will be resulted under multiple projects of different alognment.
												// (Please place this line after all other #include<>.)
#endif

//======= Conversions between UNICODE and multi-byte string =============

/** convert multi-byte string to wide-character string */
std::wstring _A2W(const std::string &as);
std::string _W2A(const std::wstring &ws);

/** convert wide-character string to multi-byte string */
std::wstring _A2W(const char *as);
std::string _W2A(const wchar_t *ws);

//===== Conversions between UNICODE, UTF-8 and UTF-16 string =======

/** convert UTF8 string to wide-character string */
bool _A2W_UTF8(const std::string &as, std::wstring &ws);

/** convert wide-character string to UTF8 string */
void _W2A_UTF8(const std::wstring &ws, std::string &as);

/** get the next character offset relative to current character in bytes. */
int _UTF8_Inc(const char *as);

/** Check ByteOrderMap signature of UTF-8 and return the length of BOM in bytes. */
int _UTF8_BOM_Inc(const void *data);

/** convert UTF16 string to wide-character string. (length in bytes) */
bool _A2W_UTF16(const void *as, unsigned int length, std::wstring &ws);

/** convert wide-character string to UTF16 string */
unsigned short* _W2A_UTF16(const std::wstring &ws);

/** get the next character offset relative to current character in bytes. */
int _UTF16_Inc(const void *as);

/** Check ByteOrderMap signature of UTF-16 and return the length of BOM in bytes. */
int _UTF16_BOM_Inc(const void *data);

//================= UNICODE  ================
#if defined(_WIN32)
#include <tchar.h>
#endif

#ifdef _UNICODE

typedef std::wstring StdString;
typedef std::wistream InStream;
typedef std::wostream OutStream;
typedef std::wistringstream InStrStream;
typedef std::wostringstream OutStrStream;
#define COut  std::wcout
#define _A2T(x)	_A2W(x).c_str()
#define _T2A(x)	_W2A(x).c_str()

#ifndef UNICODE		// for Win32 API
#define UNICODE
#endif

#ifndef _TCHAR
#define _TCHAR wchar_t
#endif

#ifndef _T
#define _T(x) L##x
#endif

//================= ANSI ================
#else

typedef std::string StdString;
typedef std::istream InStream;
typedef std::ostream OutStream;
typedef std::istringstream InStrStream;
typedef std::ostringstream OutStrStream;
#define COut  std::cout
#define _A2T(x)	(x)
#define _T2A(x)	(x)

#ifndef _TCHAR
#define _TCHAR char
#endif

#ifndef _T
#define _T(x) x
#endif

#endif



#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__GNUC__)	// If using Microsoft or Borland or GNU C++ compiler, do the following.
	#pragma pack(pop)							// Restore member alignment size. (Do not affect outside world)
#endif

#endif
