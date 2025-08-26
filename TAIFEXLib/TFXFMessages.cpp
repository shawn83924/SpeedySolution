#include "TFXFMessages.h"
//-----------------------------------------------------------------------------------------
namespace TFX
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
					default: return mtUnknown;
				}
		case 2: switch( MsgType )
				{
					case  4:
					return mtF050; //F050,F130
					case  5: return mtF060; //F060,F140
					default: return mtUnknown;
				}
		case 3: switch( MsgType )
				{
					case  6: return mtF070; //F070,F150
					case  7: return mtF080; //F080,F160
					default: return mtUnknown;
				}
		case 4: switch( MsgType )
				{
					case  0: return mtF170; //F170
					case  1: return mtF180; //F180
					default: return mtUnknown;
				}
		default: return mtUnknown;
	}
}
//---------------------------------------------------------------------------
FMessageHeader::FMessageHeader( const char* SourceID, const char* DestinationID, int MessageLength, const char* SubSys, const char* Func,  const char* MsgType,  const char* Status )
:MessageHeader( SubSys, Func, MsgType, Status )
{
	char MsgLen[8];

	memcpy( FSourceID,       SourceID,      7 );
	memcpy( FDestinationID,  DestinationID, 7 );
	sprintf( MsgLen, "%04d", MessageLength );
	memcpy( FMessageLength,  MsgLen,        4 );
}
//---------------------------------------------------------------------------
FMessageHeader::FMessageHeader( char* Data )
:MessageHeader( Data )
{
    MemoryStreamReader MemReader( Data + MESSAGE_HEADER_SIZE );
    MemReader.Read( FSourceID,      7 );
    MemReader.Read( FDestinationID, 7 );
    MemReader.Read( FMessageLength, 4 );
}
//---------------------------------------------------------------------------
void  FMessageHeader::SetSourceID( char* ID )
{
	memcpy( FSourceID, ID, 7 );
	UpdateTime();
}
//---------------------------------------------------------------------------
void  FMessageHeader::SetDestinationID( char* ID )
{
	memcpy( FDestinationID, ID, 7 );
	UpdateTime();
}
//---------------------------------------------------------------------------
void  FMessageHeader::SwapID( void )
{
	char Str[7];

	memcpy( Str,            FSourceID,      7 );
	memcpy( FSourceID,      FDestinationID, 7 );
	memcpy( FDestinationID, Str,            7 );
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
    MemWriter.Write( FSourceID,      7 );
    MemWriter.Write( FDestinationID, 7 );
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
int FMessageHeader::GetDefMsgLen( char* FileCode )
{
	if( strcmp( FileCode, "C09" ) == 0 )
		return  3;
	if( strcmp( FileCode, "C10" ) == 0 )
		return  3;
	if( strcmp( FileCode, "P01" ) == 0 )
		return  3;
	if( strcmp( FileCode, "P02" ) == 0 )
		return  3;
	if( strcmp( FileCode, "T90" ) == 0 )
		return  3;
	if( strcmp( FileCode, "R30" ) == 0 )
		return 26;
	if( strcmp( FileCode, "R40" ) == 0 )
		return 23;
	if( strcmp( FileCode, "R50" ) == 0 )
		return 23;

	return 0;
}
//---------------------------------------------------------------------------
MessageF010::MessageF010(  char* SourceID,  char* DestinationID,  char* FileCode, int FileSize )
:FMessageHeader( SourceID, DestinationID, 11, "20", "00", "00" )
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
MessageF010::MessageF010( char* Data )
:FMessageHeader( Data )
{
	MemoryStreamReader MemReader( Data + FMESSAGE_HEADER_SIZE );
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
	MemoryStreamWriter MemWriter( Buffer + FMESSAGE_HEADER_SIZE  );
	MemWriter.Write( FFileCode, 3 );
	MemWriter.Write( FFileSize, 8 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
/*void MessageF010::print( void )
{
	FMessageHeader::print();
	SocketPrintf("---------------- Message F010 -------------------\n" );
	SocketPrintf("[ FILE CODE %s ]\n",UFC::AnsiString( FFileCode, 3 ).c_str());
	SocketPrintf("[ FILE SIZE %d ]\n",GetFileSize( ));
	SocketPrintf("-------------- Message F010 End -----------------\n" );
}*/
//---------------------------------------------------------------------------
MessageF020::MessageF020(  char* SourceID,  char* DestinationID,  char* FileCode, int FileSize )
:FMessageHeader( SourceID, DestinationID, 11, "20", "00", "01" )
{
	char File_Size[9];

	memcpy( FFileCode, FileCode,  3 );

	sprintf( File_Size, "%08d", FileSize );
	memcpy( FFileSize, File_Size, 8 );
}
//---------------------------------------------------------------------------
MessageF020::MessageF020( char* Data )
:FMessageHeader( Data )
{
	MemoryStreamReader MemReader( Data + FMESSAGE_HEADER_SIZE );
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
	MemoryStreamWriter MemWriter( Buffer + FMESSAGE_HEADER_SIZE  );
	MemWriter.Write( FFileCode, 3 );
	MemWriter.Write( FFileSize, 8 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
/*void MessageF020::print( void )
{
	FMessageHeader::print();
	SocketPrintf("---------------- Message F020 -------------------\n" );
	SocketPrintf("[ FILE CODE %s ]\n",UFC::AnsiString( FFileCode, 3 ).c_str());
	SocketPrintf("[ FILE SIZE %d ]\n",GetFileSize( ));
	SocketPrintf("-------------- Message F020 End -----------------\n" );
}*/
//---------------------------------------------------------------------------
MessageF030::MessageF030(  char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode, char FileEOF, char* Data )
:FMessageHeader( SourceID, DestinationID, MessageLength, "20", "01", "02" )
,FEOF( FileEOF )
{
	memcpy( FFileCode, FileCode,   3 );
	memcpy( FData,     Data,     988 );
}
//---------------------------------------------------------------------------
MessageF030::MessageF030( char* Data )
:FMessageHeader( Data )
{
	MemoryStreamReader MemReader( Data + FMESSAGE_HEADER_SIZE );
	MemReader.Read( FFileCode, 3 );
	MemReader.Read( &FEOF,     1 );
	MemReader.Read( FData,   988 );
}
//---------------------------------------------------------------------------
void MessageF030::AsString( char* Buffer )
{
	FMessageHeader::AsString( Buffer );
	MemoryStreamWriter MemWriter( Buffer + FMESSAGE_HEADER_SIZE  );
	MemWriter.Write( FFileCode, 3 );
	MemWriter.Write( &FEOF,     1 );
	MemWriter.Write( FData,   988 );
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageF030::print( void )
{
	FMessageHeader::print();
	SocketPrintf("---------------- Message F030 -------------------\n" );
	SocketPrintf("[ FILE CODE %s ]\n", GetFileCode().c_str() );
	SocketPrintf("[ EOF %c ]\n", FEOF );
	SocketPrintf("[ DATA %s ]\n", GetData() );
	SocketPrintf("-------------- Message F030 End -----------------\n" );
}
//---------------------------------------------------------------------------
MessageF040::MessageF040(  char* SourceID,  char* DestinationID,  char* FileCode, char DataEOF )
:FMessageHeader( SourceID, DestinationID, 4, "20", "01", "03" )
,FEOF( DataEOF )
{
	memcpy( FFileCode, FileCode,  3 );
}
//---------------------------------------------------------------------------
MessageF040::MessageF040( char* Data )
:FMessageHeader( Data )
{
	MemoryStreamReader MemReader( Data + FMESSAGE_HEADER_SIZE );
	MemReader.Read( FFileCode, 3 );
	MemReader.Read( &FEOF,     1 );
}
//---------------------------------------------------------------------------
void MessageF040::AsString( char* Buffer )
{
	FMessageHeader::AsString( Buffer );
	MemoryStreamWriter MemWriter( Buffer + FMESSAGE_HEADER_SIZE  );
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
MessageF050::MessageF050(  char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode,  char* Request_Message )
:FMessageHeader( SourceID, DestinationID, MessageLength, "20", "02", "04" )
,FReplyType(0x2A)
,FStartSequence(-1)
,FEndSequence(-1)
{
    memcpy( FFileCode,        FileCode,           3 );
    memcpy( FRequest_Message, Request_Message,  989 );
    memset( FRequestID, 0, 7 );
    ReadRequestMessage();
}
//---------------------------------------------------------------------------
MessageF050::MessageF050(  char* SourceID,  char* DestinationID, int MessageLength, char* FileCode, int StartSeq, int EndSeq )
:FMessageHeader( SourceID, DestinationID, MessageLength, "20", "02", "04" )
,FReplyType(0x2A)
,FStartSequence( StartSeq )
,FEndSequence( EndSeq )
{
    char SeqStr[ 17 ];
    memcpy( FFileCode, FileCode,  3 );
    memset( FRequest_Message, 0,  989 );
    memset( FRequestID, 0, 7 );
    memset( FRequest_Message, ' ', MessageLength );
    if( FStartSequence != 0 )
    {
        sprintf( SeqStr, "%08d%08d",FStartSequence,FEndSequence);
        memcpy( FRequest_Message, SeqStr, 16 );
    }
}
//---------------------------------------------------------------------------
MessageF050::MessageF050(  char* SourceID,  char* DestinationID, const UFC::AnsiString& Data )
:FMessageHeader( SourceID, DestinationID, Data.Length(), "20", "02", "04" )
,FReplyType(0x2A)
,FStartSequence(-1)
,FEndSequence(-1)
{
    memcpy( FFileCode, Data.c_str(),  3 );
    memset( FRequest_Message, 0, 989 );
    if( Data.Length() > 3 )
        strcpy( FRequest_Message, Data.c_str() + 3  );
}    
//---------------------------------------------------------------------------
MessageF050::MessageF050( char* Data )
:FMessageHeader( Data )
,FReplyType(0x2A)
,FStartSequence(-1)
,FEndSequence(-1)
{
    MemoryStreamReader MemReader( Data + FMESSAGE_HEADER_SIZE );
    MemReader.Read( FFileCode,          3 );
    memset( FRequest_Message, 0, 989 );
    MemReader.Read( FRequest_Message, GetMessageLength() - 3 );

    memset( FRequestID, 0, 7 );
    ReadRequestMessage();
}
//---------------------------------------------------------------------------
void MessageF050::ReadRequestMessage(void)
{
	MemoryStreamReader MemReader( FRequest_Message );
	char fCode[4];
	memcpy( fCode, FFileCode, 3 );
	fCode[3] = 0;

	BOOL Result = FALSE;
	char str[9];

	if( stricmp(fCode, "R30") == 0 )
	{
		Result = TRUE;
	}
	else if( stricmp(fCode, "R40") == 0 )
	{
		Result = TRUE;
	}
	else if( stricmp(fCode, "R50") == 0 )
	{
		Result = TRUE;
	}

	if( Result == TRUE )
	{
		MemReader.Read( str, 8 );
		str[8] = 0;
		FStartSequence = atoi( str );

		MemReader.Read( str, 8 );
		str[8] = 0;
		FEndSequence = atoi( str );

		MemReader.Read( FRequestID, 7 );
	}
}
//---------------------------------------------------------------------------
void MessageF050::AsString( char* Buffer )
{
    FMessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + FMESSAGE_HEADER_SIZE  );
    MemWriter.Write( FFileCode,          3 );
    MemWriter.Write( FRequest_Message, 989 );
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
MessageF060::MessageF060(  char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode, char* Request_Message )
:FMessageHeader( SourceID, DestinationID, MessageLength, "20", "02", "05" )
{
	memcpy( FFileCode,        FileCode,           3 );
	memcpy( FRequest_Message, Request_Message,  989 );
}
//---------------------------------------------------------------------------
MessageF060::MessageF060( char* Data )
:FMessageHeader( Data )
{
	MemoryStreamReader MemReader( Data + FMESSAGE_HEADER_SIZE );
	MemReader.Read( FFileCode,          3 );
	MemReader.Read( FRequest_Message, 989 );
}
//---------------------------------------------------------------------------
void MessageF060::AsString( char* Buffer )
{
	FMessageHeader::AsString( Buffer );
	MemoryStreamWriter MemWriter( Buffer + FMESSAGE_HEADER_SIZE  );
	MemWriter.Write( FFileCode,          3 );
	MemWriter.Write( FRequest_Message, 989 );
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
MessageF070::MessageF070(  char* SourceID,  char* DestinationID )
:FMessageHeader( SourceID, DestinationID, 0, "20", "03", "06" )
{
}
//---------------------------------------------------------------------------
MessageF080::MessageF080(  char* SourceID,  char* DestinationID )
:FMessageHeader( SourceID, DestinationID, 0, "20", "03", "07" )
{
}
//---------------------------------------------------------------------------
MessageF170::MessageF170(  char* SourceID,  char* DestinationID )
:FMessageHeader( SourceID, DestinationID, 0, "20", "04", "00" )
{
}
//---------------------------------------------------------------------------
MessageF180::MessageF180(  char* SourceID,  char* DestinationID )
:FMessageHeader( SourceID, DestinationID, 0, "20", "04", "01" )
{
}
//-----------------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
