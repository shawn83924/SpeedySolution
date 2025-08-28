//////////////////////////////////////////////////////////
// File Name: osfunc.h
//
// Author : Little Chio
// Last Modified : 2002/8/12 for PS2/XBOX port.
//
// Copyright (c) 2002, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////
/**
 * @file osfunc.h
 * A document for the OS-related functions and definitions.
 */

#ifndef __OSFUNC_H__
#define __OSFUNC_H__

#pragma warning (disable:4530 4786)

#include <stdio.h>
#include <string>

//////////////////////////////////////////////////////////
// OS functions for Win32
//////////////////////////////////////////////////////////
#if defined(_WIN32) && !defined(_XBOX)

#include <windows.h>
#include <mmsystem.h>
#include <direct.h>

/**
 * @def TICKS_PER_SEC 
 * The definition of the number of ticks per second in the system.
 */
#define TICKS_PER_SEC      1000.0f

/**
 * @fn unsigned int get_system_tick()
 * The system timer which can retrieve the current number of ticks.
 */
#define get_system_tick()  timeGetTime()

// end of _WIN32

//////////////////////////////////////////////////////////
// OS functions for XBOX
//////////////////////////////////////////////////////////
#elif defined(_XBOX)

#include <xtl.h>
#include <stdio.h>

#define fopen   (FILE *)Xfopen

/**
 * @def TICKS_PER_SEC 
 * The definition of the number of ticks per second in the system.
 */
#define TICKS_PER_SEC      1000.0f

/**
 * @fn unsigned int get_system_tick()
 * The system timer which can retrieve the current number of ticks.
 */
#define get_system_tick()  GetTickCount()

extern "C"
{

char *getcwd(char *buf, unsigned int buf_size);
int chdir(const char *dirname);
void *Xfopen(const char *name, const char *access);

}

// end of _XBOX

//////////////////////////////////////////////////////////
// OS functions for PS2
//////////////////////////////////////////////////////////
#elif SN_TARGET_PS2      

#include <stdio.h>
#include <unistd.h>
#include "skyfs.h"

// Output debug string: use printf for host machine
#define OutputDebugString(x) printf(x)

/**
 * @def TICKS_PER_SEC 
 * The definition of the number of ticks per second in the system.
 */
#define TICKS_PER_SEC   1000.0f

/**
 * @fn unsigned int get_system_tick()
 * The system timer which can retrieve the current number of ticks.
 */
extern "C" unsigned int get_system_tick();

// end of SN_TARGET_PS2
//////////////////////////////////////////////////////////
// OS functions for GNU C
//////////////////////////////////////////////////////////
#elif defined(__GNUC__)

#include <stdio.h>
#include <unistd.h>

/**
 * @def TICKS_PER_SEC 
 * The definition of the number of ticks per second in the system.
 */
#define TICKS_PER_SEC   1000.0f

/**
 * @fn unsigned int get_system_tick()
 * The system timer which can retrieve the current number of ticks.
 */
extern "C" unsigned int get_system_tick();

/**
 * @fn unsigned int GetCurrentThreadId()
 * The system timer which can retrieve the current thread id.
 */
extern "C" unsigned int GetCurrentThreadId();

#endif

//////////////////////////////////////////////////////////
// Directory functions
//////////////////////////////////////////////////////////

/**
 * @fn void extract_path(const std::string &path, std::string &pathname, std::string &filename)
 * Extract pathname and filename from a path.
 * @param path The path is going to be extracted.
 * @param pathname  The result of the path-name.
 * @param filename  The result of the file-name.
 */
void extract_path(const std::string &path, std::string &pathname, std::string &filename);

/**
 * Get the string of a path relative to another path.
 * @param to_path The path where we are changing to
 * @param from_path The path where we are changing from.
 * @return The string of relative path between to_path and from_path.
 */
std::string relative_path(const std::string &to_path, const std::string &from_path);

/**
 * Make relative full pathname from current working directory.
 * @param path The absolute full-pathname.
 * @return The string of relative full-pathname from current working directory.
 */
std::string relative_fullpath(const std::string &fullpath);

/** 
 * @fn void push_cwd()
 * Push current working directory to the stack. 
 */
void push_cwd();

/** 
 * @fn void pop_cwd()
 * Pop directory from stack and set it as current working directory. 
 */
void pop_cwd();

/** 
 * @fn std::string set_cwd(const std::string &path)
 * Set current working directory. 
 * @param path The target path.
 * @return Return the filename part if the path contains the file-name.
 */
std::string set_cwd(const std::string &path);

/**
 * Get current working directory.
 */
std::string get_cwd();

//////////////////////////////////////////////////////////
// Interface for custom file system
//////////////////////////////////////////////////////////
struct IFileSystem
{
	virtual void *Fopen(const char *filename, const char *mode);
	virtual long Ftell(void *stream);
	virtual int Fseek(void *stream, long offset, int origin);
	virtual size_t Fread(void *buffer, size_t size, size_t count, void *stream);
	virtual size_t Fwrite(const void *buffer, size_t size, size_t count, void *stream);
	virtual int Fgetc(void *stream);
	virtual char *Fgets(char *string, int n, void *stream);
	virtual int Fputs(const char *string, void *stream);
	virtual int Feof(void *stream);
	virtual int Fclose(void *stream);
	virtual int Chdir(const char *dirname);
	virtual char *Getcwd(char *buffer, int maxlen);
	virtual bool IsDirMT() const;

	// global IFileSystem instance
	static IFileSystem *GetInstance();
	static void Install(IFileSystem *pNewFS);
	static void Uninstall();
};

//////////////////////////////////////////////////////////
// File function re-definitions for file system
//////////////////////////////////////////////////////////
#ifndef __OSFUNC_CPP__

#undef feof
#define fopen	(FILE *)IFileSystem::GetInstance()->Fopen
#define ftell	IFileSystem::GetInstance()->Ftell
#define fseek	IFileSystem::GetInstance()->Fseek
#define fread	IFileSystem::GetInstance()->Fread
#define fwrite	IFileSystem::GetInstance()->Fwrite
#define fgetc	IFileSystem::GetInstance()->Fgetc
#define fgets	IFileSystem::GetInstance()->Fgets
#define fputs	IFileSystem::GetInstance()->Fputs
#define feof	IFileSystem::GetInstance()->Feof
#define fclose	IFileSystem::GetInstance()->Fclose
#define chdir	IFileSystem::GetInstance()->Chdir
#define getcwd	IFileSystem::GetInstance()->Getcwd

#endif

#endif   // end of __OSFUNC_H__

