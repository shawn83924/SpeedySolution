#include "LMessages.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetLinkSubsystemMessage( int FuncCode, int MsgType )
{
	switch( FuncCode )
	{
		case 10: switch( MsgType )
				 {
				 	case 0: return mtL010;
				 	case 1: return mtL020;
				 	default: return mtUnknown;
				 }
		case 20: switch( MsgType )
				 {
				 	case 2: return mtL030;
				 	case 3: return mtL040;
				 	case 4: return mtL050;
				 	case 5: return mtL060;
				 	default: return mtUnknown;
				 }
		case 30: switch( MsgType )
				 {
				 	case 6: return mtL070;
				 	case 7: return mtL080;
				 	default: return mtUnknown;
				 }
		default: return mtUnknown;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//   Begin implement Link Sub-System messages
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageL030::MessageL030( BOOL IsTWSE, int AppNo )
:MessageHeader((char*)((IsTWSE)?"10":"91"),"20","02")
{
	char TmpStr[ 4 ];

	sprintf( TmpStr, "%03d", AppNo );
        memcpy( AppendNo, TmpStr, 3 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageL030::MessageL030( const char* Data )
:MessageHeader( Data )
{
	memcpy( AppendNo, Data + MESSAGE_HEADER_SIZE, 3 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageL030::AsString( char* Buffer )
{
	MessageHeader::AsString( Buffer );
	memcpy( Buffer + MESSAGE_HEADER_SIZE, AppendNo, 3 );
	*( Buffer + MESSAGE_HEADER_SIZE + 3 ) = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString MessageL030::GetAppendNo()
{
	UFC::AnsiString AppNo(AppendNo,3);
	return AppNo;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageL040::MessageL040( BOOL IsTWSE, char* AppendNumber, char* Broker, char AP, char* Password )
:MessageHeader((char*)((IsTWSE)?"10":"91"),"20","03"),APCode( AP )
{
	char KeyValueStr[ 8];
	int  Value = ((atoi( AppendNumber ) * atoi( Password ))/100)%100;/* we need thousand & hundred */

	sprintf( KeyValueStr, "%02d", Value );
	memcpy( AppendNo, AppendNumber,    3 );
	memcpy( BrokerID, Broker,          4 );
	memcpy( KeyValue, KeyValueStr,     2 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageL040::MessageL040( const char* Data )
:MessageHeader( Data )
{
	MemoryStreamReader MemReader( Data + MESSAGE_HEADER_SIZE );
    MemReader.Read( AppendNo, 3 );
	MemReader.Read( BrokerID, 4 );
	MemReader.Read( &APCode,  1 );
	MemReader.Read( KeyValue, 2 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageL040::AsString( char* Buffer )
{
	MessageHeader::AsString( Buffer );
	MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
	MemWriter.Write( AppendNo, 3 );
	MemWriter.Write( BrokerID, 4 );
	MemWriter.Write(&APCode,   1 );
	MemWriter.Write( KeyValue, 2 );
	MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageL040::print( void )
{
	SocketPrintf("----------- Register notice message L030 ---------------\n" );
	SocketPrintf("[ Append Number %s ]\n",UFC::AnsiString( AppendNo,3 ).c_str() );
	SocketPrintf("[ Broker ID %s ]\n",UFC::AnsiString( BrokerID, 4 ).c_str());
	SocketPrintf("[ AP Code %c ]\n",APCode);
	SocketPrintf("[ Key Value %s ]\n",UFC::AnsiString( KeyValue, 2 ).c_str());
	SocketPrintf("----------- Confirm register message L040 --------------\n" );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString MessageL040::GetAppendNo( void )
{
	UFC::AnsiString RtnStr(AppendNo,3);
	return RtnStr;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString MessageL040::GetBrokerID( void )
{
	UFC::AnsiString RtnStr(BrokerID,4);
	return RtnStr;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString MessageL040::GetKeyValue( void )
{
	UFC::AnsiString RtnStr(KeyValue,2);
	return RtnStr;
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// End implement Link Sub-System messages.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


