// 
// File:   Sysque.h
// Author: yuan
//
// Created on September 6, 2007, 1:11 PM
//
//------------------------------------------------------------------------------
#ifndef _Sysque_H
#define	_Sysque_H
//------------------------------------------------------------------------------
#include "quefun4eb.h"
#include "sqerrno.h"
#include "SQMessage.h"
//------------------------------------------------------------------------------
class ISysqueEventListener
{
public:
    virtual ~ISysqueEventListener( void ) {}
public:
    virtual void OnData( char* Data, int Size ) = 0;
};
//------------------------------------------------------------------------------
class TSysque
{
private:
    Handle                FHandle;
    UFC::AnsiString       FQueIP;
    int                   FQuePort;
    UFC::AnsiString       FQueName;
    ISysqueEventListener* FEventListener;
    
public:
    TSysque( const UFC::AnsiString& IP, int Port, const UFC::AnsiString& Name );
    virtual ~TSysque( void );

private:
    void OnError( int ErrorCode );

public:
    BOOL Create( unsigned int max_msg_number, unsigned int max_msg_size );
    BOOL Open( void );
    void Close( void );
    int  Read( char* Buffer, int Size );
    int  Write( char* Data, int Size );
};
//------------------------------------------------------------------------------
#endif	/* _Sysque_H */

