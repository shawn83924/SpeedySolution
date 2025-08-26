#include "Messages.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL MessageHeader::FUseExtFT = FALSE;
BOOL MessageHeader::FNewVersion = FALSE;
BOOL MessageHeader::FNewExVersion = FALSE;
TWSEOrderObjectPool*  MessageHeader::T010PoolPtr = NULL;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline void memcpy_zero( char* Dest, const char* Src, int len )
{
    memcpy( Dest,  Src, len );    
    Dest[ len ] = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader::MessageHeader( const char* SubSys, const char* Func, const char* MsgType, const char* Status )
{
    memcpy_zero( FSystemName,  SubSys, 2 );
    memcpy_zero( FFunctionCode,Func,   2 );
    memcpy_zero( FMessageType, MsgType,2 );
    memcpy_zero( FStatusCode,  Status, 2 );
    UFC::GetHHMMSS_Null( FMessageTime );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader::MessageHeader( void )
{
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader::MessageHeader( const char* Data )
{
    LoadHeader( Data );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::LoadHeader( const char* Data )
{
    MemoryStreamReader MemReader( Data );
    MemReader.ReadString( FSystemName,   2 );
    MemReader.ReadString( FFunctionCode, 2 );
    MemReader.ReadString( FMessageType,  2 );
    MemReader.ReadString( FMessageTime,  6 );
    MemReader.ReadString( FStatusCode,   2 );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::AsString( char* Buffer )
{
    MemoryStreamWriter MemWriter( Buffer  );
    MemWriter.Write( FSystemName,   2 );
    MemWriter.Write( FFunctionCode, 2 );
    MemWriter.Write( FMessageType,  2 );
    MemWriter.Write( FMessageTime,  6 );
    MemWriter.Write( FStatusCode,   2 );
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::UpdateTime()
{
    UFC::GetHHMMSS_Null( FMessageTime );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageHeader::GetSubSystem()
{
    return atoi( FSystemName );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageHeader::GetFunctionCode()
{
    return atoi( FFunctionCode );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageHeader::GetMessageType()
{
    return atoi( FMessageType );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageHeader::GetStatusCode()
{
    return atoi( FStatusCode );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::SetFunctionCode( int Code )
{
    snprintf( FFunctionCode, 3, "%02d",Code );
    UpdateTime();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::SetMessageType( int Type )
{
    snprintf( FMessageType, 3, "%02d",Type );
    UpdateTime();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::SetStatusCode( int Code )
{
    snprintf( FStatusCode, 3, "%02d", Code );
    UpdateTime();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  MessageHeader::SetStatusCodeStr( char* Code )
{    
    memcpy_zero( FStatusCode,  Code, 2 );
    UpdateTime();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::print( void )
{
    SocketPrintf("---------------- Message Header -------------------\n" );
    SocketPrintf("[ Sub System %s ]\n", FSystemName );
    SocketPrintf("[ Function Code %s ]\n", FFunctionCode );
    SocketPrintf("[ Message Type %s ]\n", FMessageType );
    SocketPrintf("[ Message Time %s ]\n", FMessageTime );
    SocketPrintf("[ Status Code %s ]\n", FStatusCode );
    SocketPrintf("-------------- Message Header End -----------------\n" );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::MessageType( void )
{
    int SubSystem = GetSubSystem();
    int FuncCode  = GetFunctionCode();
    int MsgType   = GetMessageType();

    switch( SubSystem ) 
    {
        case 91:
        case 10: return GetLinkSubsystemMessage( FuncCode, MsgType );
        case 92:
        case 20: return GetFileTransferSubsystemMessage( FuncCode, MsgType );
        case 93:
        case 30: return GetTransactionSubsystemMessage( FuncCode, MsgType );
        case 98:
        case 32: return GetPostTransactionSubsystemMessage( FuncCode, MsgType );
        case 94:
        case 40: return GetOddTransactionSubsystemMessage( FuncCode, MsgType );
        case 83:
        case 33: return GetOddExTransactionSubsystemMessage( FuncCode, MsgType );
        case 70: return GetAuctionTransactionSubsystemMessage( FuncCode, MsgType );
        case 96: return GetNegotiatePxTransactionSubsystemMessage( FuncCode, MsgType );
        case 90:
        case 31: return GetLendTransactionSubsystemMessage( FuncCode, MsgType );
        case 97:
        case 41: return GetTenderTransactionSubsystemMessage( FuncCode, MsgType );
        case 95:
        case 50: return GetConfirmSubsystemMessage( FuncCode, MsgType );
        case 42:
        case 89: return GetTenderExTransactionSubsystemMessage( FuncCode, MsgType );        
        default: return mtUnknown;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TSubSys MessageHeader::GetTradingSession( int SubSystem )
{
    switch( SubSystem ) 
    {
        case 93:
        case 30: return lsNormal;
        case 98:
        case 32: return lsPost;
        case 94:
        case 40: return lsOdd;
        case 83:
        case 33: return lsOddEx;
        case 70: return lsAuction;
        case 96: return lsNegotiatePx;
        case 90:
        case 31: return lsLend;
        case 97:
        case 41: return lsTender;
        case 42:
        case 89: return lsTenderEx;        
        default: return lsUnknown;
    }    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL MessageHeader::IsMessage( TMessageType mt )
{
    if( mt == MessageType() )
       return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL MessageHeader::IsMessage(  char* Data, TMessageType Msg )
{
    TMessageType mt = GetMessageType( Data );
    if( mt == Msg )
        return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetMessageType( const char* Data )
{
    MessageHeader Header( Data );
    return Header.MessageType();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* MessageHeader::CreateMessage( const char* Data, int Length )
{
    if( Length >= MESSAGE_HEADER_SIZE )
    {
        TMessageType mt = GetMessageType( Data );
        if( GetMessageLength( mt ) <= Length )
            return CreateInstance( mt, Data );
    }
    return NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* MessageHeader::CreateMessage( const char* Data )
{
    TMessageType mt = GetMessageType( Data );
    return CreateInstance( mt, Data );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::Initialize( void )
{
    if( T010PoolPtr == NULL )
        T010PoolPtr = new TWSEOrderObjectPool( );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageT010* MessageHeader::FastT010( const char* Data )
{
    MessageT010* T010;
	
    if( T010PoolPtr == NULL )
	T010PoolPtr = new TWSEOrderObjectPool( );
    T010 = T010PoolPtr->GetInstance();
    T010->Load( Data );
    return T010;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* MessageHeader::CreateInstance( TMessageType Type, const char* Data )
{
    try
    {
      
      switch( Type )
      {
          // Link Sub-System messages
          case mtL010: return new MessageL010( Data );
          case mtL020: return new MessageL020( Data );
          case mtL030: return new MessageL030( Data );
          case mtL040: return new MessageL040( Data );
          case mtL050: return new MessageL050( Data );
          case mtL060: return new MessageL060( Data );
          case mtL070: return new MessageL070( Data );
          case mtL080: return new MessageL080( Data );
          // Transaction Sub-System messages
          case mtT010: return FastT010( Data );
                       //return new MessageT010( Data );
          case mtT020: return new MessageT020( Data );
          case mtT030: return new MessageT030( Data );
          case mtT040: return new MessageT040( Data );
          case mtT050: return new MessageT050( Data );
          case mtT060: return new MessageT060( Data );
          // Odd Transaction Sub-System messages
          case mtO010: return new MessageO010( Data );
          case mtO020: return new MessageO020( Data );
          case mtO030: return new MessageO030( Data );
          case mtO040: return new MessageO040( Data );
          case mtO060: return new MessageO060( Data );
          case mtO070: return new MessageO070( Data );
          case mtO080: return new MessageO080( Data );
          case mtO090: return new MessageO090( Data );
          // Post Transaction Sub-System messages
          case mtP010: return new MessageP010( Data );
          case mtP020: return new MessageP020( Data );
          case mtP030: return new MessageP030( Data );
          case mtP040: return new MessageP040( Data );
          case mtP050: return new MessageP050( Data );
          case mtP060: return new MessageP060( Data );
          case mtP070: return new MessageP070( Data );
          case mtP080: return new MessageP080( Data );
          // Auction Transaction Sub-System messages
          case mtA010: return new MessageA010( Data );
          case mtA020: return new MessageA020( Data );
          case mtA030: return new MessageA030( Data );
          case mtA040: return new MessageA040( Data );
          case mtA050: return new MessageA050( Data );
          case mtA060: return new MessageA060( Data );
          // Tender Transaction Sub-System messages
          case mtE010: return new MessageE010( Data );
          case mtE020: return new MessageE020( Data );
          case mtE030: return new MessageE030( Data );
          case mtE040: return new MessageE040( Data );
          case mtE050: return new MessageE050( Data );
          case mtE060: return new MessageE060( Data );
          // Tender Ex Transaction Sub-System messages
          case mtEx010: return new MessageEx010( Data );
          case mtEx020: return new MessageEx020( Data );
          case mtEx030: return new MessageEx030( Data );
          case mtEx040: return new MessageEx040( Data );
          case mtEx050: return new MessageEx050( Data );
          case mtEx060: return new MessageEx060( Data );          
          // Lend Transaction Sub-System messages
          case mtV010: return new MessageV010( Data );
          case mtV020: return new MessageV020( Data );
          case mtV030: return new MessageV030( Data );
          case mtV040: return new MessageV040( Data );
          case mtV050: return new MessageV050( Data );
          case mtV060: return new MessageV060( Data );
          // Confirm Sub-System messages
          case mtR1: return new MessageR1( Data );
          case mtR2: return new MessageR2( Data );
          case mtR3: return new MessageR3( Data );
          case mtR4: return new MessageR4( Data );
          case mtR5: return new MessageR5( Data );
          case mtR6: return new MessageR6( Data );
          // File Sub-System messages
          case mtF010: return new MessageF010( Data );
          case mtF020: return new MessageF020( Data );
          case mtF030: return new MessageF030( Data );
          case mtF040: return new MessageF040( Data );
          case mtF050: return new MessageF050( Data );
          case mtF060: return new MessageF060( Data );
          case mtF070: return new MessageF070( Data );
          case mtF080: return new MessageF080( Data );
          case mtF170: return new MessageF170( Data );
          case mtF180: return new MessageF180( Data );
          case mtF210: return new MessageF210( Data );
          case mtF220: return new MessageF220( Data );
          // Negotiate Price Sub-System messages
          case mtS010: return new MessageS010( Data );
          case mtS020: return new MessageS020( Data );
          case mtS030: return new MessageS030( Data );
          case mtS040: return new MessageS040( Data );
          case mtS050: return new MessageS050( Data );
          case mtS060: return new MessageS060( Data );
          case mtS070: return new MessageS070( Data );
          case mtS080: return new MessageS080( Data );
          case mtS090: return new MessageS090( Data );
          case mtS100: return new MessageS100( Data );
          case mtS110: return new MessageS110( Data );
          case mtS120: return new MessageS120( Data );
          case mtS130: return new MessageS130( Data );
          case mtS140: return new MessageS140( Data );
          case mtS150: return new MessageS150( Data );
          // Odd Transaction Sub-System messages
          case mtO110: return new MessageO110( Data );
          case mtO120: return new MessageO120( Data );
          case mtO130: return new MessageO130( Data );
          case mtO140: return new MessageO140( Data );
          case mtO150: return new MessageO150( Data );
          case mtO160: return new MessageO160( Data );
          default:	 return NULL;
      }
    }
    catch( ... )
    {
       SocketPrintf( "Bed data:\"%s\"\n",Data );
       return NULL;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageHeader::GetMessageLength( TMessageType Type )
{
    switch( Type ) 
    {
        // Link Sub-System messages
        case mtL010: return 14;
        case mtL020: return 14;
        case mtL030: return 17;
        case mtL040: return 24;
        case mtL050: return 14;
        case mtL060: return 14;
        case mtL070: return 14;
        case mtL080: return 14;
        // Transaction Sub-System messages
        case mtT010: if( MessageHeader::FNewVersion == TRUE )
                         return 59;    
                     return 51;
        case mtT020: if( MessageHeader::FNewVersion == TRUE )
                         return 88;   
                     return 69;
        case mtT030: return 14;
        case mtT040: return 14;
        case mtT050: return 14;
        case mtT060: return 14;
        //Odd Transaction Sub-System messages
        case mtO010: return 58;
        case mtO020: return 86;
        case mtO030: return 14;
        case mtO040: return 23;
        case mtO060: return 51;
        case mtO070: return 14;
        case mtO080: return 14;
        case mtO090: return 14;
        // Post Transaction Sub-System messages
        case mtP010: return 51;
        case mtP020: return 69;
        case mtP030: return 14;
        case mtP040: return 14;
        case mtP050: return 14;
        case mtP060: return 14;
        case mtP070: return 20;
        case mtP080: return 64;
        // Auction Transaction Sub-System messages
        case mtA010: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 59;
                     return 56;
        case mtA020: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 117;
                     return 104;
        case mtA030: return 14;
        case mtA040: return 14;
        case mtA050: return 14;
        case mtA060: return 14;
        // Tender Transaction Sub-System messages
        case mtE010: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 61;
                     return 58;
        case mtE020: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 119;    
                     return 106;
        case mtE030: return 14;
        case mtE040: return 14;
        case mtE050: return 14;
        case mtE060: return 14;
        // Tender Ex Transaction Sub-System messages
        case mtEx010: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 61;    
                      return 58;
        case mtEx020: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 119;    
                      return 106;
        case mtEx030: return 14;
        case mtEx040: return 14;
        case mtEx050: return 14;
        case mtEx060: return 14;        
        // Lend Transaction Sub-System messages
        case mtV010: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 53;
                     return 52;
        case mtV020: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 81; 
                     return 76;
        case mtV030: return 14;
        case mtV040: return 14;
        case mtV050: return 14;
        case mtV060: return 14;
        // Confirm Sub-System messages
        case mtR1: return 24;
        case mtR2: return 24;
        case mtR3: if( MessageHeader::FNewVersion == TRUE )
                       return 86;  ///< 20+66, 20+132 .....    
                   return 82; ///< 20+62, 20+124, 20+186 or 20+248
        case mtR4: return 14;
        case mtR5: return 14;
        case mtR6: return 20;
        // File Sub-System messages
        case mtF010: return 26;
        case mtF020: return 26;
        case mtF030: return 26;
        case mtF040: return 26;
        case mtF050: return 26;
        case mtF060: return 26;
        case mtF070: return 26;
        case mtF080: return 26;
        case mtF170: return 14;
        case mtF180: return 14;
        case mtF210: return 26;
        case mtF220: return 26;
        // Negotiate Price Sub-System messages
        case mtS010: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 45;
                     return 40;
        case mtS020: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 45;
                     return 40;
        case mtS030: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 67;
                     return 55 + 7;
        case mtS040: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 81;
                     return 67 + 7;
        case mtS050: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 55;
                     return 43 + 7;
        case mtS060: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 77;
                     return 61 + 7;
        case mtS070: return 32 + 7;
        case mtS080: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 82;
                     return 66 + 7;
        case mtS090: return 20;
        case mtS100: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 89; ///< 14 + 2+ 73*10
                     return 74; ///< 14 + 2+ 58*10
        case mtS110: return 21;
        case mtS120: if( MessageHeader::FNewExVersion == TRUE ) 
                         return 50;
                     return 45;
        case mtS130: return 14;
        case mtS140: return 14;
        case mtS150: return 14; 
        // Odd Transaction Sub-System messages
        case mtO110: return 59;   
        case mtO120: return 88;   
        case mtO130: return 14;
        case mtO140: return 14;
        case mtO150: return 14;
        case mtO160: return 14;
        default: return 9999999;
    }
}
//-----------------------------------------------------------------------------------------
void  MessageHeader::SetNewVersion( BOOL IsNew, BOOL IsNewEx )
{
    FNewVersion = IsNew;
    FNewExVersion = IsNewEx;
}
//-----------------------------------------------------------------------------------------
BOOL MessageHeader::IsNewVersion( void )
{
    return FNewVersion;
}
//-----------------------------------------------------------------------------------------
BOOL MessageHeader::IsNewExVersion( void )
{
    return FNewExVersion;
}
//--------------------------------------------------------------------------
TWSEOrderObjectPool::TWSEOrderObjectPool( size_t PoolSize, int NewObjectPerSec  )
:UFC::PThread( NULL, TRUE )
,FPoolSize( PoolSize ) ///<  4096
,FNewPer10ms( NewObjectPerSec / 100 ) ///<  1024/100 = 10 objects 
{         
    FPoolSizeLow = (int)((double)FPoolSize*0.5 ); /// 2048
    for( register size_t i = 0; i < FPoolSize; i++ )
         FQ.push( new MessageT010() );
    Start();
}   
//--------------------------------------------------------------------------
MessageT010* TWSEOrderObjectPool::GetInstance( void )
{
    MessageT010* Item;        
    
    if( FCS.TryEnter( ) == true ) ///< Try to enter CriticalSection. 
    {            
        if( FQ.empty() == false ) ///< T010 exists ?
        {
            Item = FQ.front(); ///< Get the first item.
            FQ.pop();          ///< Remove the first item.
            FCS.Leave();        
        }
        else ///< No! call new to create a MessageT010
            Item = new MessageT010();             
    }
    else ///< Failed! call new to create a MessageT010 
        Item = new MessageT010();             
    return Item;
}    
//--------------------------------------------------------------------------
void TWSEOrderObjectPool::Execute( void )     
{   
    while( TRUE )
    {
        ///< Check objects in pool every 50 ms.
        UFC::SleepMS( 10 );            
        ///< Need create New objects ?
        if( FQ.size() < FPoolSizeLow )           
        {
            UFC::PLockObject Lock( FCS );                
            
            for( register int i = 0; i < FNewPer10ms; i++ )
                 FQ.push( new MessageT010() );                            
        }            
    }
}    	
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
