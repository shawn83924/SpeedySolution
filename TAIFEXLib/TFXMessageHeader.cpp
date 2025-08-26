#include "TFXMessages.h"
#include "TMPType.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader::MessageHeader( const char* SubSys, const char* Func, const char* MsgType, const char* Status )
{
    memcpy( FSystemName,  SubSys, 2 );
    memcpy( FFunctionCode,Func,   2 );
    memcpy( FMessageType, MsgType,2 );
    memcpy( FStatusCode,  Status, 2 );
    GetCurrentTimeString( FMessageTime );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader::MessageHeader( char* Data )
{
    MemoryStreamReader MemReader( Data );
    MemReader.Read( FSystemName,   2 );
    MemReader.Read( FFunctionCode, 2 );
    MemReader.Read( FMessageType,  2 );
    MemReader.Read( FMessageTime,  6 );
    MemReader.Read( FStatusCode,   2 );
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
    char  HHMMSS[ 7 ];
    int   Now = UFC::GetHHMMSS();

    sprintf( HHMMSS, "%06d", Now );
    memcpy( FMessageTime, HHMMSS, 6 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageHeader::GetSubSystem()
{
    char Str[ 4 ];
    strncpynt( Str, FSystemName, 2 );
    return atoi( Str );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageHeader::GetFunctionCode()
{
    char Str[ 4 ];
    strncpynt( Str, FFunctionCode,2);
    return atoi( Str );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageHeader::GetMessageType()
{
    char Str[ 4 ];
    strncpynt( Str, FMessageType,2);
    return atoi( Str );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageHeader::GetStatusCode()
{
    char Str[ 4 ];
    strncpynt( Str, FStatusCode, 2);
    return atoi( Str );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::SetFunctionCode( int Code )
{
    char Str[ 4 ];
    sprintf( Str,"%02d",Code );
    memcpy( FFunctionCode, Str, 2 );
    UpdateTime();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::SetMessageType( int Type )
{
    char Str[3];

    sprintf( Str,"%02d",Type );
    memcpy( FMessageType, Str, 2 );
    UpdateTime();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::SetStatusCode( int Code )
{
    char Str[4];

    sprintf( Str, "%02d", Code );
    memcpy( FStatusCode, Str, 2);
    UpdateTime();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  MessageHeader::SetStatusCodeStr( char* Code )
{
    memcpy( FStatusCode, Code, 2);
    UpdateTime();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageHeader::print( void )
{
    SocketPrintf("---------------- Message Header -------------------\n" );
    SocketPrintf("[ Sub System %s ]\n", UFC::AnsiString( FSystemName, 2 ).c_str());
    SocketPrintf("[ Function Code %s ]\n", UFC::AnsiString( FFunctionCode, 2 ).c_str());
    SocketPrintf("[ Message Type %s ]\n", UFC::AnsiString( FMessageType, 2 ).c_str());
    SocketPrintf("[ Message Time %s ]\n", UFC::AnsiString( FMessageTime, 6 ).c_str());
    SocketPrintf("[ Status Code %s ]\n", UFC::AnsiString( FStatusCode, 2 ).c_str());
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
            case 10: return GetLinkSubsystemMessage( FuncCode, MsgType );
            case 20: return GetFileTransferSubsystemMessage( FuncCode, MsgType );
            case 30: 
            case 31: return GetTransactionSubsystemMessage( FuncCode, MsgType );
            case 50: return GetConfirmSubsystemMessage( FuncCode, MsgType );
            default: return mtUnknown;
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
TMessageType MessageHeader::GetMessageType( char* Data )
{
     MessageHeader Header( Data );
     return Header.MessageType();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* MessageHeader::CreateMessage( char* Data, int Length )
{
    if( Length >= MESSAGE_HEADER_SIZE )
    {
        TMessageType mt = GetMessageType( Data );
        if( GetMessageLength( mt ) <= Length )        
            return CreateInstance( mt, Data );
        else
            UFC::BufferedLog::Printf(" Not a valid message,type:%d length:%d (required:%d).",mt,Length,GetMessageLength( mt ) );
    }
    return NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* MessageHeader::CreateMessage( char* Data )
{
    TMessageType mt = GetMessageType( Data );
    return CreateInstance( mt, Data );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* MessageHeader::CreateInstance( TMessageType Type, char* Data )
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
          case mtR010: return new MessageR010( Data );
          case mtR020: return new MessageR020( Data );
          case mtR030: return new MessageR030( Data );
          case mtR040: return new MessageR040( Data );
          case mtR050: return new MessageR050( Data );
          case mtR060: return new MessageR060( Data );
          case mtR070: return new MessageR070( Data );
          case mtR080: return new MessageR080( Data );
          case mtR090: return new MessageR090( Data );
          case mtR100: return new MessageR100( Data );
          // Confirm Sub-System messages
          case mtC010: return new MessageC010( Data );
          case mtC020: return new MessageC020( Data );
          case mtC030: return new MessageC030( Data );
          case mtC040: return new MessageC040( Data );
          case mtC050: return new MessageC050( Data );
          case mtC060: return new MessageC060( Data );
          case mtC011: return new MessageC011( Data );
          case mtC021: return new MessageC021( Data );
          case mtC031: return new MessageC031( Data );
          case mtC032: return new MessageC032( Data );
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
		case mtL040: return 27;
		case mtL050: return 14;
		case mtL060: return 14;
		case mtL070: return 14;
		case mtL080: return 14;
		// Transaction Sub-System messages
		case mtR010: return 75;
        case mtR020: return 99;                             
		case mtR030: return 14;
		case mtR040: return 14;
		case mtR050: return 14;
		case mtR060: return 14;
		case mtR070: return 46; ///< 41 -> 46 add OrderID
		case mtR080: return 62; ///< 57 -> 62 add OrderID
		case mtR090: return 86;
		case mtR100: return 104;
		// Confirm Sub-System messages
		case mtC010: return 29;
		case mtC020: return 29;
		case mtC030: return 32;
		case mtC040: return 14;
		case mtC050: return 14;
		case mtC060: return 22;
		case mtC011: return 26;
		case mtC021: return 26;
        case mtC031: return 30;
		case mtC032: return 30;
        // File Sub-System messages
		case mtF010: return 32;
		case mtF020: return 32;
		case mtF030: return 32;
		case mtF040: return 32;
		case mtF050: return 32;
		case mtF060: return 32;
		case mtF070: return 32;
		case mtF080: return 32;
		case mtF170: return 32;
		case mtF180: return 32;
		default: return 9999999;
	}
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

