//
// File:   TEmbadedResource.h
// Author: yuan
//
// Created on August 9, 2007, 3:26 PM
//
//------------------------------------------------------------------------------
#ifndef _TEmbadedResource_H
#define	_TEmbadedResource_H
//------------------------------------------------------------------------------
#include <UFC.h>
//------------------------------------------------------------------------------
#ifndef _WIN32
typedef UFC::PHashedList<UFC::AnsiString, UInt8*> TResourceList;
typedef UInt8*                                    HRSRC;
typedef HRSRC                                     HGLOBAL;
typedef TResourceList*                            HMODULE;
//------------------------------------------------------------------------------
extern "C"
{
//------------------------------------------------------------------------------
HRSRC FindResource
(
    HMODULE hModule,
    const UFC::AnsiString& Name,
    const UFC::AnsiString& Type
);
//------------------------------------------------------------------------------
HGLOBAL LoadResource
(
    HMODULE hModule,
    HRSRC hResInfo
);
//------------------------------------------------------------------------------
DWORD SizeofResource
(
    HMODULE hModule,
    HRSRC hResInfo
);
//------------------------------------------------------------------------------
void* LockResource
(
    HGLOBAL hResData
);
//------------------------------------------------------------------------------
void UnlockResource
(
    HGLOBAL hResData
);
//------------------------------------------------------------------------------
} /* extern "C" */
#endif	/* #ifndef _WIN32 */


//------------------------------------------------------------------------------
class TFormatResource
{
private:
	HGLOBAL          hGLOBAL;
	DWORD            FSize;
	UFCType::UInt8*  FData;
public:
	TFormatResource( HMODULE hMODULE, const UFC::AnsiString& Name, const UFC::AnsiString& Type );
	virtual ~TFormatResource( void );
public:
	DWORD           GetSize( void );
	UFCType::UInt8* GetData( void );
    void            SaveToFile( const UFC::AnsiString& FileName );
};
//------------------------------------------------------------------------------
extern bool GetResourceName( DWORD ResID, UFC::AnsiString& Name );
//------------------------------------------------------------------------------
#endif	/* _TEmbadedResource_H */

