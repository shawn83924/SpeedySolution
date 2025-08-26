#include "FMessages.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
   
//---------------------------------------------------------------------------
TMessageType MessageHeader::GetFileTransferSubsystemMessage( int FuncCode, int MsgType )
{
    switch( FuncCode ) 
    {
        case 0: switch( MsgType ) 
        {
            case  0: return mtF010; //F010,F090
            case  1: return mtF020; //F020,F100
            default: return mtUnknown;
        }
        case 1: switch( MsgType ) 
        {        		
            case  2: return mtF030; //F030,F110
            case  3: return mtF040; //F040,F120
            case 12: return mtF210; //F210,F230 	
            case 13: return mtF220; //F220,F240
            default: return mtUnknown;
        }
        case 2: switch( MsgType ) 
        {
            case  4: return mtF050; //F050,F130
            case  5: return mtF060; //F060,F140
            default: return mtUnknown;
        }
        case 3: switch( MsgType ) 
        {
            case  6: return mtF070; //F070,F150
            case  7: return mtF080; //F080,F160
            default: return mtUnknown;
        }
        case 4: switch( MsgType )  ///< TSE/OTC have these message??
        {
            case  8: return mtF170; //F170,F190
            case  9: return mtF180; //F180,F200
            default: return mtUnknown;
        }
        default: return mtUnknown;
    }
}
//---------------------------------------------------------------------------
// Class FMessageHeader
// Base class for all FT messages.
//---------------------------------------------------------------------------
FMessageHeader::FMessageHeader( BOOL IsTWSE, const char* SourceID, const char* DestinationID, int MessageLength, const char* Func,  const char* MsgType,  const char* Status )
:MessageHeader( (char*)((IsTWSE)?"20":"92"), Func, MsgType, Status )
{
    char MsgLen[5];
    
    memcpy( FSourceID,       SourceID,      4 );
    memcpy( FDestinationID,  DestinationID, 4 );
    sprintf( MsgLen, "%04d", MessageLength );
    memcpy( FMessageLength,  MsgLen,        4 );
}
//---------------------------------------------------------------------------
FMessageHeader::FMessageHeader( const char* Data )
:MessageHeader( Data )
{
    MemoryStreamReader MemReader( Data + MESSAGE_HEADER_SIZE );
    MemReader.Read( FSourceID,      4 );
    MemReader.Read( FDestinationID, 4 );
    MemReader.Read( FMessageLength, 4 );
}
//---------------------------------------------------------------------------
void  FMessageHeader::SetSourceID( char* ID )
{
    memcpy( FSourceID, ID, 4 );
    UpdateTime();
}
//---------------------------------------------------------------------------
void  FMessageHeader::SetDestinationID( char* ID )
{
    memcpy( FDestinationID, ID, 4 );
    UpdateTime();
}
//---------------------------------------------------------------------------
void  FMessageHeader::SwapID( void )
{
    char Str[4];
    
    memcpy( Str,            FSourceID,      4 );
    memcpy( FSourceID,      FDestinationID, 4 );
    memcpy( FDestinationID, Str,            4 );
    UpdateTime();
}
//---------------------------------------------------------------------------
int FMessageHeader::GetMessageLength()
{
    char Str[5];
    
    strncpynt( Str, FMessageLength, 4 );
    return atoi( Str );
}
//---------------------------------------------------------------------------
void FMessageHeader::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( FSourceID,      4 );
    MemWriter.Write( FDestinationID, 4 );
    MemWriter.Write( FMessageLength, 4 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void FMessageHeader::print( void )
{
    MessageHeader::print();
    SocketPrintf("---------------- File Transfer Header -------------------\n" );
    SocketPrintf("[ Source ID %s ]\n", GetSourceID().c_str() );
    SocketPrintf("[ Destination ID %s ]\n", GetDestinationID().c_str() );
    SocketPrintf("[ Message Length %d ]\n", GetMessageLength( ) );
    SocketPrintf("-------------- File Transfer Header End -----------------\n" );
}
//---------------------------------------------------------------------------
// Class MessageF010
// FT init mesage
//---------------------------------------------------------------------------
MessageF010::MessageF010(  BOOL IsTWSE, char* SourceID,  char* DestinationID,  char* FileCode, int FileSize )
:FMessageHeader( IsTWSE, SourceID, DestinationID, 11, "00", "00" )
{
    char File_Size[9];
    if( SourceID != NULL && DestinationID != NULL && FileCode != NULL ) 
    {
        memcpy( FFileCode, FileCode,  3 );
        sprintf( File_Size, "%08d", FileSize );
        memcpy( FFileSize, File_Size, 8 );
    }
}
//---------------------------------------------------------------------------
MessageF010::MessageF010( const char* Data )
:FMessageHeader( Data )
{
    MemoryStreamReader MemReader( Data + TWSE_FMESSAGE_HEADER_SIZE );
    MemReader.Read( FFileCode, 3 );
    MemReader.Read( FFileSize, 8 );
}
//---------------------------------------------------------------------------
int MessageF010::GetFileSize()
{
    char Str[9];
    
    strncpynt( Str, FFileSize, 8 );
    return atoi( Str );
}
//---------------------------------------------------------------------------
void MessageF010::AsString( char* Buffer )
{
    FMessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + TWSE_FMESSAGE_HEADER_SIZE  );
    MemWriter.Write( FFileCode, 3 );
    MemWriter.Write( FFileSize, 8 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
// Class MessageF020
// FT init reply message
//---------------------------------------------------------------------------
MessageF020::MessageF020(  BOOL IsTWSE, char* SourceID,  char* DestinationID,  char* FileCode, int FileSize )
:FMessageHeader( IsTWSE, SourceID, DestinationID, 11, "00", "01" )
{
    char File_Size[9];
    
    memcpy( FFileCode, FileCode,  3 );    
    sprintf( File_Size, "%08d", FileSize );
    memcpy( FFileSize, File_Size, 8 );
}
//---------------------------------------------------------------------------
MessageF020::MessageF020( const char* Data )
:FMessageHeader( Data )
{
    MemoryStreamReader MemReader( Data + TWSE_FMESSAGE_HEADER_SIZE );
    MemReader.Read( FFileCode, 3 );
    MemReader.Read( FFileSize, 8 );
}
//---------------------------------------------------------------------------
int MessageF020::GetFileSize()
{
    char Str[9];
    
    strncpynt( Str, FFileSize, 8 );
    return atoi( Str );
}
//---------------------------------------------------------------------------
void MessageF020::AsString( char* Buffer )
{
    FMessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + TWSE_FMESSAGE_HEADER_SIZE  );
    MemWriter.Write( FFileCode, 3 );
    MemWriter.Write( FFileSize, 8 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
// Class MessageF030
// Data transfer message.
//---------------------------------------------------------------------------
MessageF030::MessageF030( BOOL IsTWSE, char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode, char FileEOF, char* Data )
:FMessageHeader( IsTWSE, SourceID, DestinationID, MessageLength, "01", "02" )
,FEOF( FileEOF )
{
    memcpy( FFileCode, FileCode, 3 );
    memcpy( FData,     Data,     TWSE_DATA_SIZE_F030 );
}
//---------------------------------------------------------------------------
MessageF030::MessageF030( const char* Data )
:FMessageHeader( Data )
{
    MemoryStreamReader MemReader( Data + TWSE_FMESSAGE_HEADER_SIZE );
    MemReader.Read( FFileCode, 3 );
    MemReader.Read( &FEOF,     1 );
    MemReader.Read( FData,     GetMessageLength() - 4 );
    
}
//---------------------------------------------------------------------------
void MessageF030::AsString( char* Buffer )
{
    FMessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + TWSE_FMESSAGE_HEADER_SIZE  );
    MemWriter.Write( FFileCode, 3 );
    MemWriter.Write( &FEOF,     1 );
    MemWriter.Write( FData,     TWSE_DATA_SIZE_F030 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageF030::print( void )
{
    FMessageHeader::print();
    SocketPrintf("---------------- Message F030 -------------------\n" );
    SocketPrintf("[ FILE CODE %s ]\n", GetFileCode().c_str() );
    SocketPrintf("[ EOF %c ]\n", FEOF );
    SocketPrintf("[ DATA %s ]\n", (char*)GetData() );
    SocketPrintf("-------------- Message F030 End -----------------\n" );
}
//---------------------------------------------------------------------------
// Class MessageF210
// Data transfer message.
// TCP/IP Ext version of F030 ( Larger buffer size )
//---------------------------------------------------------------------------
MessageF210::MessageF210( BOOL IsTWSE, char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode, char FileEOF, char* Data )
:FMessageHeader( IsTWSE, SourceID, DestinationID, MessageLength, "01", "12" )
,FEOF( FileEOF )
{
    memcpy( FFileCode, FileCode, 3 );
    memcpy( FData,     Data,     GetMessageLength() - 4 );
}
//---------------------------------------------------------------------------
MessageF210::MessageF210( const char* Data )
:FMessageHeader( Data )
{
    MemoryStreamReader MemReader( Data + TWSE_FMESSAGE_HEADER_SIZE );
    MemReader.Read( FFileCode, 3 );
    MemReader.Read( &FEOF,     1 );
    MemReader.Read( FData,     GetMessageLength() - 4 );
    
}
//---------------------------------------------------------------------------
void MessageF210::AsString( char* Buffer )
{
    FMessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + TWSE_FMESSAGE_HEADER_SIZE  );
    MemWriter.Write( FFileCode, 3 );
    MemWriter.Write( &FEOF,     1 );
    MemWriter.Write( FData,     GetMessageLength() - 4 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageF210::print( void )
{
    FMessageHeader::print();
    SocketPrintf("---------------- Message F210 -------------------\n" );
    SocketPrintf("[ FILE CODE %s ]\n", GetFileCode().c_str() );
    SocketPrintf("[ EOF %c ]\n", FEOF );
    SocketPrintf("[ DATA %s ]\n", (char*)GetData() );
    SocketPrintf("-------------- Message F210 End -----------------\n" );
}
//---------------------------------------------------------------------------
// Class MessageF040
// Data transfer ack message.
//---------------------------------------------------------------------------
MessageF040::MessageF040( BOOL IsTWSE, char* SourceID,  char* DestinationID,  char* FileCode, char DataEOF )
:FMessageHeader( IsTWSE, SourceID, DestinationID, 4, "01", "03" )
,FEOF( DataEOF )
{
    memcpy( FFileCode, FileCode,  3 );
}
//---------------------------------------------------------------------------
MessageF040::MessageF040( const char* Data )
:FMessageHeader( Data )
{
    MemoryStreamReader MemReader( Data + TWSE_FMESSAGE_HEADER_SIZE );
    MemReader.Read( FFileCode, 3 );
    MemReader.Read( &FEOF,     1 );
}
//---------------------------------------------------------------------------
void MessageF040::AsString( char* Buffer )
{
    FMessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + TWSE_FMESSAGE_HEADER_SIZE  );
    MemWriter.Write( FFileCode, 3 );
    MemWriter.Write( &FEOF,     1 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageF040::print( void )
{
    FMessageHeader::print();
    SocketPrintf("---------------- Message F040 -------------------\n" );
    SocketPrintf("[ FILE CODE %s ]\n", GetFileCode().c_str() );
    SocketPrintf("[ EOF %c ]\n", FEOF );
    SocketPrintf("-------------- Message F040 End -----------------\n" );
}
//---------------------------------------------------------------------------
// Class MessageF220
// Data transfer ack message. TCP/IP Ext version of F040.
//---------------------------------------------------------------------------
MessageF220::MessageF220( BOOL IsTWSE, char* SourceID,  char* DestinationID,  char* FileCode, char DataEOF )
:FMessageHeader( IsTWSE, SourceID, DestinationID, 4, "01", "13" )
,FEOF( DataEOF )
{
    memcpy( FFileCode, FileCode,  3 );
}
//---------------------------------------------------------------------------
MessageF220::MessageF220( const char* Data )
:FMessageHeader( Data )
{
    MemoryStreamReader MemReader( Data + TWSE_FMESSAGE_HEADER_SIZE );
    MemReader.Read( FFileCode, 3 );
    MemReader.Read( &FEOF,     1 );
}
//---------------------------------------------------------------------------
void MessageF220::AsString( char* Buffer )
{
    FMessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + TWSE_FMESSAGE_HEADER_SIZE  );
    MemWriter.Write( FFileCode, 3 );
    MemWriter.Write( &FEOF,     1 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageF220::print( void )
{
    FMessageHeader::print();
    SocketPrintf("---------------- Message F220 -------------------\n" );
    SocketPrintf("[ FILE CODE %s ]\n", GetFileCode().c_str() );
    SocketPrintf("[ EOF %c ]\n", FEOF );
    SocketPrintf("-------------- Message F220 End -----------------\n" );
}
//---------------------------------------------------------------------------
// Class MessageF050
// Single message transfer message.
//---------------------------------------------------------------------------
MessageF050::MessageF050( BOOL IsTWSE, char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode,  char* Request_Message )
:FMessageHeader( IsTWSE, SourceID, DestinationID, MessageLength, "02", "04" )
,FHasS10( FALSE )
{
    memcpy( FFileCode, FileCode, 3 );
    memset( FRequest_Message, 0, TWSE_DATA_SIZE_F050 );
    if( MessageLength > 3 )
        memcpy( FRequest_Message, Request_Message, MessageLength - 3 );
    if( memcmp( FFileCode, "S10", 3 ) == 0 )  ///< Skip "S10"
    {
        FHasS10 = TRUE;
        memcpy( FFileCode, FRequest_Message, 3 );    
    }
}
//---------------------------------------------------------------------------
MessageF050::MessageF050( const char* Data )
:FMessageHeader( Data )
,FHasS10( FALSE )
{
    MemoryStreamReader MemReader( Data + TWSE_FMESSAGE_HEADER_SIZE );
    MemReader.Read( FFileCode,          3 );
    memset( FRequest_Message, 0, TWSE_DATA_SIZE_F050 );
    MemReader.Read( FRequest_Message, GetMessageLength() - 3 );    
    if( memcmp( FFileCode, "S10", 3 ) == 0 )  ///< Skip "S10"
    {
        FHasS10 = TRUE;
        memcpy( FFileCode, FRequest_Message, 3 );
    }
}
//---------------------------------------------------------------------------
void MessageF050::AsString( char* Buffer )
{
    FMessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + TWSE_FMESSAGE_HEADER_SIZE  );
    if( FHasS10 == TRUE )
        MemWriter.Write( "S10", 3 );
    else
        MemWriter.Write( FFileCode, 3 );
    MemWriter.Write( FRequest_Message, TWSE_DATA_SIZE_F050 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageF050::print( void )
{
    FMessageHeader::print();
    SocketPrintf("---------------- Message F050 -------------------\n" );
    SocketPrintf("[ FILE CODE %s ]\n", GetFileCode().c_str() );
    SocketPrintf("[ REQUEST MESSAGE %s ]\n", GetRequest_Message().c_str() );
    SocketPrintf("-------------- Message F050 End -----------------\n" );
}
//---------------------------------------------------------------------------
// Class MessageF060
// Single message transfer ack message.
//---------------------------------------------------------------------------
MessageF060::MessageF060( BOOL IsTWSE, char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode, char* Request_Message )
:FMessageHeader( IsTWSE, SourceID, DestinationID, MessageLength, "02", "05" )
,FHasS10( FALSE )
{
    memcpy( FFileCode,        FileCode,         3 );
    memcpy( FRequest_Message, Request_Message,  TWSE_DATA_SIZE_F060 );
    if( memcmp( FFileCode, "S10", 3 ) == 0 )  ///< Skip "S10"
    {
        FHasS10 = TRUE;
        memcpy( FFileCode, FRequest_Message, 3 );
    }
}
//---------------------------------------------------------------------------
MessageF060::MessageF060( const char* Data )
:FMessageHeader( Data )
,FHasS10( FALSE )
{
    MemoryStreamReader MemReader( Data + TWSE_FMESSAGE_HEADER_SIZE );
    MemReader.Read( FFileCode,        3 );
    memset( FRequest_Message, 0, TWSE_DATA_SIZE_F060 );
    MemReader.Read( FRequest_Message, TWSE_DATA_SIZE_F060 );
    if( memcmp( FFileCode, "S10", 3 ) == 0 )  ///< Skip "S10"
    {
        FHasS10 = TRUE;
        memcpy( FFileCode, FRequest_Message, 3 );    
    }
}
//---------------------------------------------------------------------------
void MessageF060::AsString( char* Buffer )
{
    FMessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + TWSE_FMESSAGE_HEADER_SIZE  );
    if( FHasS10 == TRUE )
        MemWriter.Write( "S10",        3 );
    else
        MemWriter.Write( FFileCode,        3 );
    MemWriter.Write( FRequest_Message, TWSE_DATA_SIZE_F060 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageF060::print( void )
{
    FMessageHeader::print();
    SocketPrintf("---------------- Message F060 -------------------\n" );
    SocketPrintf("[ FILE CODE %s ]\n", GetFileCode().c_str() );
    SocketPrintf("[ REQUEST MESSAGE %s ]\n", GetRequest_Message() );
    SocketPrintf("-------------- Message F060 End -----------------\n" );
}
//---------------------------------------------------------------------------
// Class MessageF070
// File transfer end message.
//---------------------------------------------------------------------------
MessageF070::MessageF070( BOOL IsTWSE, char* SourceID,  char* DestinationID )
:FMessageHeader( IsTWSE, SourceID, DestinationID, 0, "03", "06" )
{
}
//---------------------------------------------------------------------------
// Class MessageF070
// File transfer end ack message.
//---------------------------------------------------------------------------
MessageF080::MessageF080( BOOL IsTWSE, char* SourceID,  char* DestinationID )
:FMessageHeader( IsTWSE, SourceID, DestinationID, 0, "03", "07" )
{
}
//-----------------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

