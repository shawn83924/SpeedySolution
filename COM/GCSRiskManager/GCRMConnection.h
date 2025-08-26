// 
// File:   YuantaExecConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _SinoPacExecConnection_H
#define	_SinoPacExecConnection_H
//------------------------------------------------------------------------------
#include "Sigo.h" 
#include "UFC.h" 
#include "iniFile.h"
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
//--------------------------------------------------------------------------
class GCRMConnection :  public MessageListener
{
private:    
    int                               FMarginLevel;
    UFC::PCriticalSection             FAccountLimitCS;
    UFC::PHashedSet<UFC::AnsiString>  FTWALockAE;
    UFC::PHashedSet<UFC::AnsiString>  FUSDLockAE;
    UFC::PHashedSet<UFC::AnsiString>  FLockSymbolTable;    
    MessageObject*                    FMessageObject;
private:
    UFC::AnsiString       FToSpeedySubject;
    UFC::AnsiString       FToSpeedyKey;
    UFC::AnsiString       FFutOrderSubject;
    UFC::AnsiString       FOptOrderSubject;
private:        
    void UseMargin( UFC::AnsiString& AE );
private:
    void LoadSetting( void );    
    bool IsReplacePx( const UFC::AnsiString& Order );
    void GetAE( const UFC::AnsiString& Key, UFC::AnsiString& AE );
    void UserDataToAE( const UFC::AnsiString& UserData, UFC::AnsiString& AE );
    BOOL IsLockedSym( const UFC::AnsiString& Symbol );
    BOOL AccountCanPlaceOrder( const UFC::AnsiString& AE, const UFC::AnsiString& Symbol );
    void SetAccountInfo( const UFC::AnsiString& AE, const UFC::AnsiString& Curr, int CanUse );    
private: 
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );    
public:
    GCRMConnection( );
    virtual ~GCRMConnection( void );

};
//------------------------------------------------------------------------------
#endif
