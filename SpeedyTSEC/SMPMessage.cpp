/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "SMPMessage.h"
#include "AnsiString.h"
#ifdef __EXTREME_VERSION     
    #include "Account.h"
#endif
//-----------------------------------------------------------------------------------------
extern BOOL GSMPFixLength;
extern BOOL GAutoSession;
extern BOOL GForceOrderID;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------
UFC::AnsiString       SMPMessage::SMP_BROKERID( "9268" );    
int                   SMPMessage::FSMPSeq = 0;
UFC::PCriticalSection SMPMessage::FSeqLock;
const int             TAG_34_OFFSET   = 24 + 5; //< 8=FIX.4.4^9=231^35=D^34=
//------------------------------------------------------------------------------------------------------------------
//
// Class SMPMessage
//
//------------------------------------------------------------------------------------------------------------------
SMPMessage::SMPMessage( char MsgType )
:FType( MsgType )
,FFIXLength( 0 )
,FSeq( 0 )
{
    FData[ 0 ] = 0x04;
    FData[ 1 ] = MsgType;
    if( MsgType == 'H' )
    {
        if( GSMPFixLength == FALSE )
            FSMPLength = 20;        
        else
            FSMPLength = 384;        
        memcpy( FData + 2, "000", 3 );    
        UFC::GetUTCTimeString( FData + 5 , true );
        memcpy( FData + 17, "30", 2 );    
        memset( FData + 19, ' ', 364 );        
        FData[ FSMPLength -1 ] = 0x03;        
        FData[ FSMPLength ] = 0;    
    }
}
//------------------------------------------------------------------------------------------------------------------
SMPMessage::SMPMessage( const FIX::Message& Msg, int Seq )
:FType( 'H' )
,FFIXLength( 0 )
,FSeq( 0 )
{
    std::string FIXMsg  = Msg.toString();
    std::string MsgType = Msg.getHeader().getField( 35 );///< MsgType
        
    ExecMessage( MsgType[0],  FIXMsg, Seq );
}
//------------------------------------------------------------------------------------------------------------------
SMPMessage::SMPMessage( char MsgType, const std::string& FIXMsg, int Seq )
:FType( MsgType )
,FFIXLength( 0 )
,FSeq( 0 )
{
    ExecMessage( MsgType,  FIXMsg, Seq );
}
//------------------------------------------------------------------------------------------------------------------
void SMPMessage::ExecMessage( char MsgType, const std::string& FIXMsg, int Seq )
{
    bool        IsExec = false;
    std::size_t Tag56Pos = FIXMsg.find( "56=" );
    char        TargetStr[16];
      
    if( Tag56Pos != std::string::npos )      
        FIXMsg.copy( TargetStr,  7, Tag56Pos + 3 );    
    
    memset( FData, ' ', SMP_MSG_MAX );
    FData[ 0 ] = 0x04;                ///< Frame Start.
    if( MsgType == '8' || MsgType == '9' )              ///< Message Type.  
    {
        FData[ 1 ] = MsgType;
        IsExec = true;
    }
    FData[ 2 ] = TargetStr[ 0 ];   ///< Tag56
    FData[ 3 ] = TargetStr[ 5 ];
    FData[ 4 ] = TargetStr[ 6 ];   ///< Session ID    
    if( IsExec == true)
    {
        char SeqStr[16];
        int  FIXLen = FIXMsg.size();
        
        snprintf( SeqStr, 16, "%010u", (unsigned int) Seq );
        memcpy( FData + 5, SeqStr, 10 );                     ///< Report sequence        
        if( GSMPFixLength == FALSE )
        {
            memcpy( FData + 15, FIXMsg.c_str(), FIXLen ); ///<  FIX Message.
            FSMPLength = FIXMsg.size() + 16; 
        }
        else
        {
            if( FIXLen <= 368 )
                memcpy( FData + 15, FIXMsg.c_str(), FIXLen ); ///<  FIX Message.
            else
                memcpy( FData + 15, FIXMsg.c_str(), 368 );
            FSMPLength = 384; ///368+16
        }
    }
    else
    {
        memcpy( FData + 5, FIXMsg.c_str(), FIXMsg.size() ); ///<  FIX Message.        
        if( GSMPFixLength == FALSE )
            FSMPLength = FIXMsg.size() + 6;
        else
            FSMPLength = 384;

    }
    FData[ FSMPLength -1 ] = 0x03; ///< Frame End.       
    if( GSMPFixLength == FALSE )
        FData[ SMP_MSG_MAX - 1 ] = 0;  ///< C String Null Term. length 1024
    else
        FData[ FSMPLength ] = 0;       ///< Null Term. length 384
}
//------------------------------------------------------------------------------------------------------------------
SMPMessage::SMPMessage( char User, int Count, char result )
:FType( 'E' )
,FFIXLength( 0 )
,FSeq( 0 )
,FSMPLength( 18 )
{
    char CountStr[16];      
    
    if( GSMPFixLength == TRUE )
        FSMPLength = 384;
    snprintf( CountStr, 16, "%010u", (unsigned int) Count );
    memset( FData, ' ', FSMPLength );
    FData[ 0 ] = 0x04;                ///< Frame Start.
    FData[ 1 ] = 'E';        
    FData[ 2 ] = '0';
    FData[ 3 ] = '0';
    FData[ 4 ] = '0';        ///< Session ID        
    FData[ 5 ] = User;
    memcpy( FData + 6, CountStr, 10 ); 
    FData[ 16 ] = result;
    FData[ FSMPLength -1 ] = 0x03; ///< Frame End.       
    FData[ FSMPLength ] = 0;       ///< C String Null Term
    
}
//------------------------------------------------------------------------------------------------------------------
SMPMessage::SMPMessage( char User, char result, int MaxSeq )
:FType( 'R' )
,FFIXLength( 0 )
,FSeq( 0 )
,FSMPLength( 16 )
{
    char CountStr[16];      
    
    if( GSMPFixLength == TRUE )
        FSMPLength = 384;    
    snprintf( CountStr, 16, "%08u", (unsigned int) MaxSeq );
    memset( FData, ' ', FSMPLength );
    FData[ 0 ] = 0x04;                ///< Frame Start.
    FData[ 1 ] = 'R';        
    FData[ 2 ] = '0';
    FData[ 3 ] = '0';
    FData[ 4 ] = '0';        ///< Session ID        
    FData[ 5 ] = User;
    FData[ 6 ] = result;
    memcpy( FData + 7, CountStr, 8 );     
    FData[ FSMPLength -1 ] = 0x03; ///< Frame End.       
    FData[ FSMPLength ] = 0;       ///< C String Null Term
    
}
//------------------------------------------------------------------------------------------------------------------
int SMPMessage::GetSequence()
{
    UFC::PLockObject  Lock( SMPMessage::FSeqLock );
    
    FSMPSeq++;
    return FSMPSeq;
}
//------------------------------------------------------------------------------------------------------------------
int SMPMessage::GetFIXSequence()
{
    return UFC::AnsiString::StrToInt32( FData + TAG_34_OFFSET, 8 );
}
//------------------------------------------------------------------------------------------------------------------
bool SMPMessage::FromSocket( UFC::PClientSocket* Socket )
{
    UFC::PLockObject   Lock( Socket->ReadLock );

    Socket->BlockRecv( (UFCType::UInt8*) FData, 5 );    ///< Receive Header        
    
    if( FData[0] == 0x04 ) ///< Check Start char
    {        
        FType = FData[ 1 ];
        memcpy( FSID, &FData[2], 3 );
        FSID[3] = 0;
        switch( FType )
        {
            case 'C': ///< Recover
                      if( GSMPFixLength == FALSE )
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 22 );
                          FSMPLength = 27; 
                      }
                      else
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 379 );
                          FSMPLength = 384; 
                      }
                      FUser = FData[5];
                      BeginSeq = UFC::AnsiString::StrToInt32( FData + 6, 10 );
                      EndSeq   = UFC::AnsiString::StrToInt32( FData + 16, 10 );
                      FFIXLength = 0;
                      FSeq = 0;
                      break;              
            case 'D': ///< FIX New length: 238
                      if( GSMPFixLength == FALSE )
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 239 );
                          FSMPLength = 244;                                              
                      }
                      else
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 379 );
                          FData[ 5 + 238] = 0;
                          FSMPLength = 384;                      
                      }
                      FFIXLength = 238;
                      FSeq = GetSequence();
                      break;  
            case 'G': ///< FIX Replace Length: 254
                      if( GSMPFixLength == FALSE )
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 255 );
                          FSMPLength = 260;                      
                      }
                      else
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 379 );
                          FData[ 5 + 254] = 0;
                          FSMPLength = 384;                      
                      }
                      FFIXLength = 254;
                      FSeq = GetSequence();
                      break;  
            case 'F': ///< FIX Cancel Length: 220   
                      if( GSMPFixLength == FALSE )
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 221 );
                          FSMPLength = 226;                      
                      }
                      else
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 379 );
                          FData[ 5 + 220] = 0;
                          FSMPLength = 384;                      
                      }
                      FFIXLength = 220;
                      FSeq = GetSequence();
                      break;  
            case 'L': if( GSMPFixLength == FALSE )
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 2 );
                          FSMPLength = 7;
                      }
                      else
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 379 );
                          FSMPLength = 384;
                      }
                      FFIXLength = 0;
                      FSeq = 0;
                      break;
            case 'H': if( GSMPFixLength == FALSE )
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 15 );
                          FSMPLength = 20;
                      }
                      else
                      {
                          Socket->BlockRecv( (UFCType::UInt8*)FData + 5, 379 );
                          FSMPLength = 384;
                      }            
                      FFIXLength = 0;
                      FSeq = 0;
                      break;
            default:  UFC::BufferedLog::Printf( " Unknown SMP Message[%s]", FData );              
                      return false; 
        }                
        if( FData[ FSMPLength -1 ] == 0x03 )
        {
            FData[ FSMPLength -1 ] = 0;
            return true;
        }
        else
            UFC::BufferedLog::Printf( " Bad SMP Frame End 0x03 Message[%s]", FData );              
    }
    else
        UFC::BufferedLog::Printf( " Bad SMP Frame Start 0x04 Message[%s]", FData );              
    return false;
}    
//------------------------------------------------------------------------------------------------------------------
//char Account;     No  GetAccountSymbol
//char SubAcc;      Yes
//char Symbol;      No  GetAccountSymbol
//char OrderID;     No  GetAccountSymbol
//char UserID;      No  GetAccountSymbol
//int  Shares;      Yes
//bool IsSellShort; Yes     
//bool NeedsRollback;      
//int  Px;        No 
//------------------------------------------------------------------------------------------------------------------
bool  SMPMessage::GetAccountSymbol( RollBackInfo& Info )
{
    switch( FType )
    {
        case 'D': ///< New length: 238
                  memcpy( Info.Account, FData + 97, 7 );
                  Info.Account[7]=0;
                  Info.UserID = FData[ 108 ];                  
                  memcpy( Info.OrderID, FData + 124, 5 );
                  Info.OrderID[5]=0;
    #ifdef __EXTREME_VERSION                     
                  if( GForceOrderID == TRUE || strcmp( Info.OrderID, "XXXXX") == 0 )
                  {
                      UFC::AnsiString OID;
                      UFC::PLockObject Lock( ActiveExecutive::gStore->FPoolUser->FOIDLock );
                      
                      ActiveExecutive::gStore->FPoolUser->GenerateTSEOrderNumber( OID, false );   
                      memcpy( FData + 124, OID.c_str(), 5 );
                      memcpy( Info.OrderID, OID.c_str(), 5 );
                  }                  
    #endif              
                  memcpy( Info.Symbol, FData + 172, 6 );
                  Info.Symbol[6]=0;                  
                  FFIXLength = 238;
                  break;  
        case 'G': ///< Replace Length: 254
                  memcpy( Info.Account, FData + 97, 7 );
                  Info.Account[7]=0;
                  Info.UserID = FData[ 108 ];
                  memcpy( Info.OrderID, FData + 124, 5 );
                  Info.OrderID[5]=0;
                  memcpy( Info.Symbol, FData + 188, 6 );
                  Info.Symbol[6]=0;
                  break;  
        case 'F': ///< Cancel Length: 220   
                  memcpy( Info.Account, FData + 97, 7 );
                  Info.Account[7]=0;
                  Info.UserID = FData[ 108 ];
                  memcpy( Info.OrderID, FData + 124, 5 );
                  Info.OrderID[5]=0;
                  memcpy( Info.Symbol, FData + 154, 6 );
                  Info.Symbol[6]=0;
                  break;
        default:  UFC::BufferedLog::Printf( " Unknown SMP Message[%s]", FData );              
                  return false; 
    }       
    return true;
}
//------------------------------------------------------------------------------------------------------------------
void SMPMessage::GetSubAccount( char* SubAcc )
{
    memcpy( SubAcc, FData + 109, 4 );
    SubAcc[ 4 ] = 0;
}
//------------------------------------------------------------------------------------------------------------------
void SMPMessage::GetPrice( int& Px, bool& IsMarket )
{        
    if( FData[ 157 ] == '1' ) ///< Is Market Order? OrdType ='1'
    {
        IsMarket = true;
        Px = 0.0;
    }
    else
    {
        char PxStr[10];
        
        memcpy( PxStr, FData + 143, 5 );
        memcpy( PxStr + 5, FData + 149, 4 );          
        IsMarket = false;    
        Px = UFC::AnsiString::StrToInt32( PxStr , 9 );
    }    
}
//------------------------------------------------------------------------------------------------------------------
void SMPMessage::GetQty( int& Qty, bool& IsOdd )
{
    Qty = UFC::AnsiString::StrToInt32( FData + 133 , 6 );
    if( FData[ 93 ] == '2' ||  FData[ 93 ] == 'C' ) 
        IsOdd = true;
    else
        IsOdd = false;
}
//------------------------------------------------------------------------------------------------------------------
bool SMPMessage::IsFixedPx( void )
{
    if( FData[ 93 ] == '7' )
        return true;
    return false;
}
//------------------------------------------------------------------------------------------------------------------
bool SMPMessage::IsRegular( void )
{
    if( FData[ 93 ] == '0' )
        return true;
    return false;
}
//------------------------------------------------------------------------------------------------------------------
bool SMPMessage::IsOdd( void )
{
    if( FData[ 93 ] == '2' )
        return true;
    return false;
}
//------------------------------------------------------------------------------------------------------------------
char SMPMessage::GetTradingSession( void )
{
    return FData[ 93 ];
}
//------------------------------------------------------------------------------------------------------------------
bool SMPMessage::IsOddEx( void )
{
    if( FData[ 93 ] == 'C' )
        return true;
    return false;
}
//------------------------------------------------------------------------------------------------------------------
bool SMPMessage::IsIOC( void )
{
    if( FData[ 162 ] == '3' ) ///< Is IOC Order? 59='3'
        return true;
    return false;
}
//------------------------------------------------------------------------------------------------------------------
void SMPMessage::GetShares( int& Shares, bool& IsSellShort )
{
    int Qty = UFC::AnsiString::StrToInt32( FData + 133 , 6 );
    if( FData[ 93 ] == '2' ||  FData[ 93 ] == 'C' ) 
        Shares = Qty;
    else
        Shares = Qty*1000;
    if( FData[ 218 ] == '5' ||  FData[ 218 ] == '6' ) 
        IsSellShort = true;
    else
        IsSellShort = false;
}
//------------------------------------------------------------------------------------------------------------------
void SMPMessage::GetSubAccountShares( RollBackInfo& Info )
{
    int Qty = UFC::AnsiString::StrToInt32( FData + 133 , 6 );    
    
    memcpy( Info.SubAcc, FData + 109, 4 );
    Info.SubAcc[ 4 ] = 0;
    if( FData[ 93 ] == '2' ||  FData[ 93 ] == 'C' ) 
        Info.Shares = Qty;
    else
        Info.Shares = Qty*1000;
    if( FData[ 218 ] == '5' ||  FData[ 218 ] == '6' ) 
        Info.IsSellShort = true;
    else
        Info.IsSellShort = false;    
    //char Account;     No  GetAccountSymbol
    //char SubAcc;      Yes
    //char Symbol;      No  GetAccountSymbol
    //char OrderID;     No  GetAccountSymbol
    //char UserID;      No  GetAccountSymbol
    //int  Shares;      Yes
    //bool IsSellShort; Yes     
    //bool NeedsRollback;      
    //int  Px;        No 
}
//------------------------------------------------------------------------------------------------------------------
bool SMPMessage::IsSell( void )
{    
    if( FData[ 167 ] == '2'  ) 
        return true;
    return false;
}
//------------------------------------------------------------------------------------------------------------------
bool SMPMessage::IsTWSE( void )
{
    if( FSID[0] == 'T' )
        return true;
    return false;
}
//------------------------------------------------------------------------------------------------------------------
const char* SMPMessage::GetTargetID( void )
{
    if( FSID[0] == 'T')  
        return "XTAI";
    else
        return "ROCO";
}
//------------------------------------------------------------------------------------------------------------------
const char* SMPMessage::GetSenderID( void )
{
    FSsnderID[0] = FSID[0];
    memcpy( FSsnderID + 1, SMP_BROKERID.c_str(), 4 );
    memcpy( FSsnderID + 5, FSID + 1, 2 );
    FSsnderID[ 7 ] = 0;
    return FSsnderID;
}
//------------------------------------------------------------------------------------------------------------------
void SMPMessage::PrintFIX( void )
{
    UFC::AnsiString FIXMsg( GetFIXMessage( ), GetFIXLength() );
    
    UFC::BufferedLog::Printf( " SMP FIX[%s]",  FIXMsg.c_str() );
}
//------------------------------------------------------------------------------------------------------------------
const char* SMPMessage::ResultString( char R )
{
    switch( R )
    {
        case RECOVER_OK:               return "Recover OK!";
        case RECOVER_BEGIN_TOO_HEIGHT: return "Failed! Begin Seq too high";
        case RECOVER_END_TOO_HEIGHT:   return "OK! but end Seq too high";;
//        case RECOVER_FILE_NOT_FOUND:   return "Failed! recover file not found";
        default:                       return "Failed! Unknown result";  
    }
}
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------
