/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SMPMessage.h
 * Author: yuan
 *
 * Created on July 31, 2022, 11:39 PM
 */

#ifndef SMPMESSAGE_H
#define SMPMESSAGE_H
//-----------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "Message.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//---------------------------------------------------------------------------
const int SMP_MSG_MAX  = 1024;
const int SMP_MSG_FIX  = 384;
const int SMP_TIME_LENGTH = 16;
const int SMP_CRLF = 1;
//---------------------------------------------------------------------------
const char RECOVER_OK               = '1';
const char RECOVER_BEGIN_TOO_HEIGHT = '2';
const char RECOVER_END_TOO_HEIGHT   = '3';
//---------------------------------------------------------------------------
class RollBackInfo
{
public:    
    char Account[8];
    char SubAcc[8];
    char Symbol[8];    
    char OrderID[8];    
    char UserID;    
    int  Shares;
    bool IsSellShort;      
    bool NeedsRollback;      
    bool IsRegular;
    int  IntPx;
public:    
    RollBackInfo()
    :Shares(0),
     IsSellShort(false),
     NeedsRollback(false),
     IsRegular( true ),
     IntPx( 0 )
    {
    }
};
//---------------------------------------------------------------------------
class SMPMessage
{
private:    
    static int FSMPSeq;    
    static UFC::PCriticalSection FSeqLock;
public:
    static UFC::AnsiString SMP_BROKERID;
    static const char* ResultString( char R );    
public:    
    char FData[ SMP_MSG_MAX ];
    char FType;
    char FSID[ 4 ];
    char FSsnderID[ 8 ];
    int  FFIXLength;
    int  FSeq;
    int  FSMPLength;
public:    
    char FUser;
    int  BeginSeq;
    int  EndSeq;    
    void ExecMessage( char MsgType, const std::string& FIXMsg, int Seq );
private:
    
public:    
    SMPMessage( char MsgType );    
    SMPMessage( char MsgType, const std::string& FIXMsg, int Seq );
    SMPMessage( const FIX::Message& Msg, int Seq );
    ///< MsgType E Recover response
    SMPMessage( char User, int Count, char result );
    ///< MsgType R logon response
    SMPMessage( char User, char result, int MaxSeq );
    bool  FromSocket( UFC::PClientSocket* Socket );
    const char* GetSenderID( void );    
    const char* GetTargetID( void );
    bool  IsTWSE( void );    
    char* GetBody( void )       { return FData + 5; }
    char* GetFIXMessage( void ) { return FData + 5; }
    int   GetFIXLength( void )  { return FFIXLength; }
    int   GetSMPLength( void )  { return FSMPLength; }
    int   GetSMPSequence( void ){ return FSeq; }
    int   GetFIXSequence( void );
    void  PrintFIX( void );    
    int   GetSequence();
public:    
    bool  GetAccountSymbol( RollBackInfo& Info );
    void  GetPrice( int& Px, bool& IsMarket );
    void  GetQty( int& Qty, bool& IsOdd );    
    void  GetShares( int& Shares, bool& IsSellShort );
    void  GetSubAccount( char* SubAcc );
    void  GetSubAccountShares( RollBackInfo& Info );
    bool  IsSell( void );
    bool  IsFixedPx(void);
    bool  IsRegular( void );
    bool  IsOdd( void );
    bool  IsOddEx( void );
    char  GetTradingSession( void );
    bool  IsIOC( void );
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

#endif /* SMPMESSAGE_H */

