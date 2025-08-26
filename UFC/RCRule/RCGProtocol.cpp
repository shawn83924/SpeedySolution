//------------------------------------------------------------------------------------------------------------------------
#include "RCGProtocol.h"
//------------------------------------------------------------------------------------------------------------------------
// class RCGPacket
//------------------------------------------------------------------------------------------------------------------------
RCGPacket::RCGPacket()
: FAction( 0 )
{
}
//------------------------------------------------------------------------------------------------------------------------
RCGPacket::RCGPacket( Int32 Action )
: FAction( Action )
{
}
//------------------------------------------------------------------------------------------------------------------------
RCGPacket::RCGPacket( Int32 Action, const UFC::AnsiString& Body )
: FAction( Action )
, FBody( Body )
{
}
//------------------------------------------------------------------------------------------------------------------------
RCGPacket::RCGPacket( Int32 Action, const char* Body )
: FAction( Action )
, FBody( Body )
{
}
//------------------------------------------------------------------------------------------------------------------------
RCGPacket::~RCGPacket()
{
}
//------------------------------------------------------------------------------------------------------------------------
void RCGPacket::LoadFromSocket( UFC::PClientSocket* Socket )
{
	UInt8 HeaderBuffer[ RCGP_HEADER_SIZE ];
	UInt8 BodyBuffer[ RCGP_BODY_BUFFER_SIZE ];
	Int32 BodyLen;

	UFC::PLockObject Locker( Socket->ReadLock );
	// Receive Header
	Socket->BlockRecv( HeaderBuffer, RCGP_HEADER_SIZE );
	//FBody = (char*)BodyBuffer;
	FAction = UFC::NInt32::ToInt32( HeaderBuffer   );	///< Load "Action Code"
	BodyLen = UFC::NInt32::ToInt32( HeaderBuffer+4 );	///< Load "Body Length"
	if ( BodyLen > RCGP_BODY_BUFFER_SIZE-1 )
		throw( UFC::SocketException( "Receive Header->BodyLength over buffer size(1024)" ) );
	// Receive Body
	if ( BodyLen > 0 )
	{
		Socket->BlockRecv( BodyBuffer, BodyLen );
		BodyBuffer[ BodyLen ] = 0;
		FBody = (char*)BodyBuffer;
	}
	else
		FBody = "";
}
//------------------------------------------------------------------------------------------------------------------------
void RCGPacket::SerializeToSocket( UFC::PClientSocket* Socket )
{
	UFC::MemoryStream HeaderStream( RCGP_HEADER_SIZE );
	Int32 BodyLen = FBody.Length(); ///< User Int32 to get UInt16

	UFC::NInt32 N32;
	N32 = FAction;
	N32.SaveToStream( &HeaderStream );
	N32 = BodyLen;
	N32.SaveToStream( &HeaderStream );
	// Enter Critical session
	UFC::PLockObject Locker( Socket->IOLock );
	// Send Header
	Socket->BlockSend( (char*)HeaderStream.GetBuffer(), RCGP_HEADER_SIZE );
	// Send Body
	if ( BodyLen > 0 )
        Socket->BlockSend( (char*)FBody.c_str(), BodyLen );
}
//------------------------------------------------------------------------------------------------------------------------
RCGPacket* RCGPacket::CreateFromStream( UFC::PClientSocket* Socket ) /* Caller need delete it self */
{
	RCGPacket* NewPacket = new RCGPacket();
	NewPacket->LoadFromSocket( Socket );
	return NewPacket;
}
//------------------------------------------------------------------------------------------------------------------------
