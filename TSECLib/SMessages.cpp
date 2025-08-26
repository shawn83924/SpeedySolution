#include "SMessages.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetNegotiatePxTransactionSubsystemMessage( int FuncCode, int MsgType )
{   ///< Function code 1 buy, 2 Cancel, 3 Reduce, 4 Query
    switch( MsgType )
    {
        case 1: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 )
                    return mtS010;
	case 2: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 )
                    return mtS020;				
	case 3: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 ||  FuncCode == 5 || FuncCode == 6 || FuncCode == 9 )
                    return mtS030;
	case 4: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 ||  FuncCode == 5 || FuncCode == 6 || FuncCode == 9 )
                    return mtS040;
	case 5: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 ||  FuncCode == 6 || FuncCode == 9 )
                    return mtS050;
	case 6: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 ||  FuncCode == 6 || FuncCode == 9 )
                    return mtS060;
        case 7: if( FuncCode == 4 ||  FuncCode == 5 ||  FuncCode == 6)
                    return mtS070;
        case 8: if( FuncCode == 4 ||  FuncCode == 5 ||  FuncCode == 6)
                    return mtS080;
        case 9: if( FuncCode == 4 ||  FuncCode == 8 )
                    return mtS090;                
        case 10:if( FuncCode == 4 )
                    return mtS100;
        case 11:if( FuncCode == 4 ||  FuncCode == 7 ||  FuncCode == 8 )
                    return mtS110;                                
        case 12:if( FuncCode == 4 )
                    return mtS120;
        case 13:if( FuncCode == 0 )
                    return mtS130;
        case 14:if( FuncCode == 0 )
                    return mtS140;
        case 15:if( FuncCode == 0 )
                    return mtS150;
	default: return mtUnknown;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Base class for all S messages.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageSXXX::MessageSXXX( const char* Func, const char* Type, char* Body, int Length )
:MessageHeader( "96", Func, Type )
, FMsgBody( Body )
, FLength( Length ) 
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------    
MessageSXXX::MessageSXXX( const char* Data, char* Body, int Length  )
:MessageHeader( Data )
, FMsgBody( Body )
, FLength( Length ) 
{
    if( FLength > 0 ) 
        memcpy( FMsgBody, Data + MESSAGE_HEADER_SIZE, FLength );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------    
void MessageSXXX::SetBody( const char* Data, int Length )
{
    FLength = Length;
    memcpy( FMsgBody, Data, FLength );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------    
void MessageSXXX::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( FMsgBody, FLength );    
    MemWriter.AppendZero();        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------    
void MessageSXXX::print( void )
{
    SocketPrintf("[ Body %s]\n",  UFC::AnsiString( FMsgBody, FLength ).c_str() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageS100::MessageS100( const char* Data )
:MessageSXXX( Data, Body)
{
    Int32 Count = UFC::AnsiString( Data+MESSAGE_HEADER_SIZE,2 ).ToInt();
    SetBody( Data + MESSAGE_HEADER_SIZE, Count*58 + 2 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageS120::MessageS120( const char* Data )
:MessageSXXX( Data, Body)
{
    Int32 Count = UFC::AnsiString( Data+MESSAGE_HEADER_SIZE,2 ).ToInt();
    SetBody( Data + MESSAGE_HEADER_SIZE, Count*23 + 8 );
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
