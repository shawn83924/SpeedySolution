// 
// File:   YuantaExecConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _KGIPTRiskManager_H
#define	_KGIPTRiskManager_H
//------------------------------------------------------------------------------
#include "Sigo.h" 
#include "UFC.h" 
#include "iniFile.h"
//--------------------------------------------------------------------------
class AccountInfo
{
public:    
    UFC::AnsiString                     FTFXAccount;
    UFC::PHashedSet<UFC::AnsiString>    FCanPlaceCurrency;    
public:        
    AccountInfo( const UFC::AnsiString& Acc ):FTFXAccount( Acc ){}
    void AddCurrency( const UFC::AnsiString& curr )      { FCanPlaceCurrency.Add( curr ); }
    void DelCurrency( const UFC::AnsiString& curr )      { FCanPlaceCurrency.Delete( curr ); }
    void ClearCurrency( void )                           { FCanPlaceCurrency.Clear();}
    bool CanPlaceCurrency( const UFC::AnsiString& curr ) { return FCanPlaceCurrency.Exists( curr ); }    
};
//--------------------------------------------------------------------------
class KGIPTRiskManager :  public MessageListener
{
private:    
    UFC::PCriticalSection             FAccountLimitCS;
    MessageObject*                    FMessageObject;
    Int32                             FTimeOutSec;
    Int32                             FCountDown;    
    UFC::PHashedSet<UFC::AnsiString>                                    FBypassPrefixSet;    
    UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<UFC::AnsiString>* >  FPrefixAccMap;
    UFC::PHashMap<UFC::AnsiString, AccountInfo* >                       FAccountMap;
    UFC::PHashMap<UFC::AnsiString, AccountInfo* >                       FDepartmentMap;
    UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<UFC::AnsiString>* >  FCurrencyAcceptAE;
private:
    UFC::AnsiString       FToSpeedySubject;
    UFC::AnsiString       FToSpeedyKey;
    UFC::AnsiString       FNodeName;
    UFC::AnsiString       FFutOrderSubject;
    UFC::AnsiString       FOptOrderSubject;
private:
    void AddAE( const UFC::AnsiString& Currency, const UFC::AnsiString& AE );
    void DeleteAE( const UFC::AnsiString& Currency, const UFC::AnsiString& AE );
    BOOL AcceptAE( const UFC::AnsiString& Currency, const UFC::AnsiString& Account, const UFC::AnsiString& AE, UFC::AnsiString& Reason );
    bool AECanUseAccount( const UFC::AnsiString& AE, const UFC::AnsiString& Account );
    void Check( const UFC::AnsiString& AE, const UFC::AnsiString& Account, const UFC::AnsiString& Symbol, const UFC::AnsiString& Subject, MTree* Data );
    void Pass( const UFC::AnsiString& AE, const UFC::AnsiString& Subject, MTree* Data );
    void GetRiskInfo( MTree* Data );
    bool IsDepartment( const UFC::AnsiString& AE );
private:    
    void LoadSetting( void );    
    bool IsCancelReplacePx( const UFC::AnsiString& Order );
    void GetAE( const UFC::AnsiString& Key, UFC::AnsiString& AE );
    BOOL IsLockedSym( const UFC::AnsiString& Symbol );
    BOOL AccountCanPlaceOrder( const UFC::AnsiString& AE, const UFC::AnsiString& Account,const UFC::AnsiString& Symbol, UFC::AnsiString& Reason );
    void SetAccountInfo( const UFC::AnsiString& AE, const UFC::AnsiString& Curr, BOOL CanUse );    
private: 
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );    
public:
    KGIPTRiskManager( );
    virtual ~KGIPTRiskManager( void );
    void CountDown( );

};
//------------------------------------------------------------------------------
#endif
