#include "RMessages.h"
//#include "AnsiString.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetConfirmSubsystemMessage( int FuncCode, int MsgType )
{
    switch( FuncCode )
    {
        case 0:  switch( MsgType )
                 {
                    case  0: return mtR1;
                    case  1: return mtR2;
                    case  4: return mtR4;
                    case  5: return mtR5;
                    default: return mtUnknown;
                 }
        case 10: if( MsgType == 0 )
                     return mtR3;
                 return mtUnknown;
        case 20: if( MsgType == 0 )
                     return mtR6;
                 return mtUnknown;
        default: return mtUnknown;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR1::MessageR1( const char* Data )
:MessageHeader( Data )
{
    MemoryStreamReader MemReader( Data + MESSAGE_HEADER_SIZE );
    MemReader.Read( BrokerID, 4 );
    MemReader.Read( StartSeq, 6 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR1::MessageR1( BOOL IsTWSE, char* Broker, int SeqNo )
:MessageHeader( (char*)((IsTWSE)?"50":"95"), "00", "00" )
{
    char SwqNoStr[9];
    sprintf( SwqNoStr, "%06d", SeqNo );
    memcpy( BrokerID, Broker,   4 );
    memcpy( StartSeq, SwqNoStr, 6 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageR1::GetSeq()
{
    UFC::AnsiString SeqStr( StartSeq, 6 );
    return SeqStr.ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR1::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( BrokerID, 4 );
    MemWriter.Write( StartSeq, 6 );
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR1::print( void )
{
	char     TmpStr[ 10 ];
	SocketPrintf("---------------- Message C010 -------------------\n" );
	strncpynt( TmpStr, BrokerID, 4 );
	SocketPrintf("[ Broker ID %s ]\n",TmpStr);
	strncpynt( TmpStr, StartSeq, 6 );
	SocketPrintf("[ Start sequence %s ]\n",TmpStr);
	SocketPrintf("-------------- Message C010 End -----------------\n" );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR2::MessageR2( const char* Data )
:MessageHeader( Data )
{
	MemoryStreamReader MemReader( Data + MESSAGE_HEADER_SIZE );
	MemReader.Read( BrokerID, 4 );
	MemReader.Read( StartSeq, 6 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR2::MessageR2( BOOL IsTWSE, char* Broker, int SeqNo )
:MessageHeader((char*)((IsTWSE)?"50":"95"),"00","01")
{
	char SwqNoStr[9];

        sprintf( SwqNoStr, "%06d", SeqNo );
	memcpy( BrokerID, Broker,   4 );
	memcpy( StartSeq, SwqNoStr, 6 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageR2::GetSeq()
{
	UFC::AnsiString SeqStr( StartSeq, 6 );
	return SeqStr.ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR2::AsString( char* Buffer )
{
	MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
	MemWriter.Write( BrokerID, 4 );
	MemWriter.Write( StartSeq, 6 );
	MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR2::print( void )
{
	char     TmpStr[ 10 ];
	SocketPrintf("---------------- Message C020 -------------------\n" );
	strncpynt( TmpStr, BrokerID, 4 );
	SocketPrintf("[ Broker ID %s ]\n",TmpStr);
	strncpynt( TmpStr, StartSeq, 6 );
	SocketPrintf("[ Start sequence %s ]\n",TmpStr);
	SocketPrintf("-------------- Message C020 End -----------------\n" );
}
//----------------------------------------------------------------------------------------------------------------------
// Confirm report message C030
//----------------------------------------------------------------------------------------------------------------------
MessageR3::MessageR3( BOOL IsTWSE )
:MessageHeader( (char*)((IsTWSE)?"50":"95"), "10", "00" ),
 FMsgCount( 0 )
{
    memset( FMessages, 0, sizeof( R3Body*) * R3_MSG_COUNT );
}
//----------------------------------------------------------------------------------------------------------------------
MessageR3::MessageR3( const char* Data )
:MessageHeader( Data )
{
    const char* Buffer = Data + MESSAGE_HEADER_SIZE;
    int   DataOffset;

    memset( FMessages, 0, sizeof( R3Body*)*R3_MSG_COUNT );    // Set message pointers to NULL.
    memcpy( FBodyLength, Buffer, 4 );                
    Buffer += 4;
    FMsgCount = UFC::AnsiString( Buffer, 2 ).ToInt();    // Get the message count from given string.
    Buffer += 2;
    for( int i = 0; i < FMsgCount; i++ ) // Create attached messages.
    {
      	FMessages[ i ] = R3Body::CreateFromString( Buffer, DataOffset );
	Buffer += DataOffset;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageR3::~MessageR3( )
{
    for( int i = 0; i < FMsgCount; i++ )
         delete FMessages[ i ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL MessageR3::AddMessage( R3Body* NewMessage )
{
    if( FMsgCount > R3_MSG_COUNT-1 )
        return FALSE;
    else if( FMsgCount == R3_MSG_COUNT-1 )
    {
        FMessages[ R3_MSG_COUNT-1 ] = NewMessage;
        FMsgCount++;
        return FALSE;
    }
    else
    {
	FMessages[ FMsgCount ] = NewMessage;
      	FMsgCount++;
	return TRUE;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR3::AsString( char* Buffer )
{
    char* StrBuffer = Buffer;
    int   Offset;
    int   BODY_LEN = 62;
    
    if( MessageHeader::FNewVersion == TRUE )
        BODY_LEN = 66;
    MessageHeader::AsString( StrBuffer );
    StrBuffer = Buffer + strlen( Buffer );
    sprintf( StrBuffer, "%04d", FMsgCount * BODY_LEN );
    StrBuffer += 4;
    sprintf( StrBuffer, "%02d", FMsgCount );
    StrBuffer += 2;
    for( int i = 0; i < FMsgCount; i++ )
    {
    	Offset = FMessages[ i ]->AsString( StrBuffer );
	StrBuffer += Offset;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageR3::print( void )
{
     char Buffer[ 256 ];
     SocketPrintf("---------------- Confirm Report Message R3 -----------------\n" );
     SocketPrintf("[ Body Length %s ]\n",UFC::AnsiString( FBodyLength, 4).c_str());
     SocketPrintf("[ Message Count %02d ]\n", FMsgCount );
     for( int i = 0; i < FMsgCount; i++ )
     {
          FMessages[ i ]->AsString( Buffer );
          printf( "Report %d:%s\n",i+1, Buffer );
     }
     SocketPrintf("-------------------------- End of C030 ------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Confirm report message C030 body class.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
R3Body* R3Body::CreateFromString( const char* Data, int &Offset )
{
    return new R3Body( Data, Offset );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Single order report message body.
//----------------------------------------------------------------------------------------------------------------------
R3Body::R3Body( const char* Data, int& Offset )
{
    MemoryStreamReader MemReader( Data );
    MemReader.Read( FStockID, 6 );
    MemReader.Read( FQuntity, 8 );
    if( MessageHeader::FNewVersion == TRUE )
    {
        MemReader.Read( FPrice, 9 );
        MemReader.Read( FMatchTime, 9 );
        FLastPx = static_cast<double>( UFC::AnsiString( FPrice, 9 ).ToInt() ) / 10000.0;
    }
    else
    {
        MemReader.Read( FPrice,   6 );
        MemReader.Read( FMatchTime, 8 );
        FLastPx = static_cast<double>( UFC::AnsiString( FPrice, 6 ).ToInt() ) / 100.0;
    }
    MemReader.Read( &FEXCD,1 );
    MemReader.Read( &FBuySellCode, 1 );
    MemReader.Read( FOrderNo,   5 );
    MemReader.Read( FInvestorACNo,  7 );
    MemReader.Read( &FOrderType, 1 );
    MemReader.Read( FSeq, 6 );
    MemReader.Read( FBroker, 4 );
    MemReader.Read( FRecNo, 8 );
    MemReader.Read( &Fmarks, 1 );
    Offset = MemReader.GetPosition();    
    FQty = UFC::AnsiString( FQuntity, 8 ).ToInt();
}
//----------------------------------------------------------------------------------------------------------------------
int R3Body::AsString( char* Buffer )
{
    MemoryStreamWriter MemWriter( Buffer );
    MemWriter.Write( FStockID, 6 );
    MemWriter.Write( FQuntity, 8 );
    if( MessageHeader::FNewVersion == TRUE )
    {
        MemWriter.Write( FPrice,   9 );
        MemWriter.Write( FMatchTime, 9 );
    }
    else
    {
        MemWriter.Write( FPrice,   6 );
        MemWriter.Write( FMatchTime, 8 );
    }
    MemWriter.Write( &FEXCD,1 );
    MemWriter.Write( &FBuySellCode, 1 );
    MemWriter.Write( FOrderNo,   5 );
    MemWriter.Write( FInvestorACNo,  7 );
    MemWriter.Write( &FOrderType, 1 );
    MemWriter.Write( FSeq, 6 );
    MemWriter.Write( FBroker, 4 );
    MemWriter.Write( FRecNo, 8 );
    MemWriter.Write( &Fmarks, 1 );
    MemWriter.AppendZero();
    return MemWriter.GetPosition() - 1;
}
//----------------------------------------------------------------------------------------------------------------------
void R3Body::print( void )
{
    SocketPrintf("[ Stock ID %s ]\n"    , UFC::AnsiString( FStockID, 6 ).c_str());
    SocketPrintf("[ Quntity %s ]\n"     , UFC::AnsiString( FQuntity, 8 ).c_str());
    if( MessageHeader::FNewVersion == TRUE )
    {
        SocketPrintf("[ Price %s ]\n"       , UFC::AnsiString( FPrice, 9 ).c_str());
        SocketPrintf("[ MatchTime %s ]\n"   , UFC::AnsiString( FMatchTime, 9 ).c_str());
    }
    else
    {
        SocketPrintf("[ Price %s ]\n"       , UFC::AnsiString( FPrice, 6 ).c_str());
        SocketPrintf("[ MatchTime %s ]\n"   , UFC::AnsiString( FMatchTime, 8 ).c_str());
    }
    SocketPrintf("[ Execute Code %c ]\n", FEXCD );
    SocketPrintf("[ BuySellCode %c ]\n" , FBuySellCode );
    SocketPrintf("[ OrderNo %s ]\n"     , UFC::AnsiString( FOrderNo,5 ).c_str());
    SocketPrintf("[ InvestorACNo %s ]\n", UFC::AnsiString( FInvestorACNo, 7 ).c_str());
    SocketPrintf("[ OrderType %c ]\n", FOrderType );
    SocketPrintf("[ SeqNo %s ]\n"       , UFC::AnsiString( FSeq, 6 ).c_str());
    SocketPrintf("[ Broker ID %s ]\n"   , UFC::AnsiString( FBroker, 4 ).c_str());
    SocketPrintf("[ Record No %s ]\n"   , UFC::AnsiString( FRecNo,    8 ).c_str());
    SocketPrintf("[ Marks %c ]\n"       , Fmarks );
}
//----------------------------------------------------------------------------------------------------------------------
void R3Body::SetPrice( double Px )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( FPrice, 10, "%09d", (int)((Px +0.00001) * 10000.0) );
    else
        snprintf( FPrice, 10, "%06d", (int)((Px +0.001) * 100.0) );    
    FLastPx = Px;
}
//----------------------------------------------------------------------------------------------------------------------
void R3Body::SetQty( int Qty )
{
    snprintf( FQuntity, 9, "%08d", Qty );    
    FQty = Qty;
}
//----------------------------------------------------------------------------------------------------------------------
void R3Body::SetTime( int H, int M, int S, int mmm )
{
    snprintf( FMatchTime, 10, "%02d%02d%02d%03d", (UInt16)H%100,(UInt16)M%100,(UInt16)S%100, (UInt16)mmm%1000 );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int R3Body::ProcessingTime( void ) 
{ 
    if( MessageHeader::FNewVersion == TRUE )        
        return UFC::AnsiString( FMatchTime, 9 ).ToInt(); 
    else
        return UFC::AnsiString( FMatchTime, 8 ).ToInt(); 
}
//----------------------------------------------------------------------------------------------------------------------
double R3Body::GetPrice( void )
{
    return FLastPx;
}
//----------------------------------------------------------------------------------------------------------------------
int R3Body::GetQty( void )
{
    return FQty;
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
