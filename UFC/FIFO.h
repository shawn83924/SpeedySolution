// 
// File:   FIFO.h
// Author: yuan
//
// Created on June 11, 2009, 1:33 PM
//

#ifndef _FIFO_H
#define	_FIFO_H
//------------------------------------------------------------------------------
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "AnsiString.h"
#include "NetUtility.h"
//------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
enum FIFOOpenMode
{
    FIFO_ReadOnly,
    FIFO_NonBlockReadOnly,
    FIFO_WriteOnly,
    FIFO_NonBlockWriteOnly
};
//------------------------------------------------------------------------------
class FIFO
{
private:
    AnsiString FName;
    FIFOOpenMode FOpenMode;
    int PFD;
public:
    FIFO( const UFC::AnsiString& PipeName, FIFOOpenMode OpenMode );
    ~FIFO();
public:
    UFC::AnsiString GetName( void ) { return FName; }
    FIFOOpenMode GetOpenMode( void ) { return FOpenMode; }
    int Read( void* Buffer, int Size );
    int Write( const void* Buffer, int Size );
};
//------------------------------------------------------------------------------
}
#endif	/* _FIFO_H */

