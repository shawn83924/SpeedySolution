//------------------------------------------------------------------------------------------------------------------------
#include "RCQGProtocol.h"
//------------------------------------------------------------------------------------------------------------------------
RCQGPacket::RCQGPacket()
: FAction( 0 )
, FEntryCode( RCQGP_CHECKNUMBER )
, FReserveBytes( 0 )
{
}
//------------------------------------------------------------------------------------------------------------------------
RCQGPacket::RCQGPacket( Int32 Action )
: FAction( Action )
, FEntryCode( RCQGP_CHECKNUMBER )
, FReserveBytes( 0 )
{
}
//------------------------------------------------------------------------------------------------------------------------
RCQGPacket::RCQGPacket( Int32 Action, RCQGNameValueSet& Data )
: FAction( Action )
, FEntryCode( RCQGP_CHECKNUMBER )
, FReserveBytes( 0 )
, FData( Data )
{
}
//------------------------------------------------------------------------------------------------------------------------
RCQGPacket::~RCQGPacket()
{
}
//------------------------------------------------------------------------------------------------------------------------
RCQGPacket* RCQGPacket::CreateFromStream( UFC::PSocket* Socket )
{
	RCQGPacket* NewPacket = new RCQGPacket();
	NewPacket->LoadFromSocket( Socket );
	return NewPacket;
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGPacket::LoadFromSocket( UFC::PSocket* Socket )
{
	UInt8	HeaderData[ RCQGP_HEADER_SIZE ];
	Int32   PacketLen;
	UFC::PLockObject Lock( Socket->ReadLock );

	Socket->BlockRecv( HeaderData, RCQGP_HEADER_SIZE );		///< Received the header.
	FAction       = UFC::NInt32::ToInt32( HeaderData    );	// load "Action Code"
	PacketLen     = UFC::NInt32::ToInt32( HeaderData+4  );	// load "Packet Len"
	FEntryCode    = UFC::NInt32::ToInt32( HeaderData+8  );	// load "Entry Code"
	FReserveBytes = UFC::NInt32::ToInt32( HeaderData+12 );	// load "Reserve Bytes"
	if ( FEntryCode != RCQGP_CHECKNUMBER )
		throw( UFC::SocketException( "Receive bad header(Entry code error)." ));
	if( PacketLen > 0 )
	{
		UFC::MemoryStream BodyData( PacketLen, PacketLen );

		Socket->BlockRecv( (UInt8*)BodyData.GetBuffer(), PacketLen ); ///< Received the body.
		FData.LoadFromStream( &BodyData );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGPacket::SerializeToSocket( UFC::PSocket* Socket )
{
	UFC::MemoryStream PacketData( RCQGP_DEFAULT_BODY_SIZE );
	UFC::NInt32 N32;

	N32 = FAction;					///< Make Header Stream.
	N32.SaveToStream( &PacketData );
	N32.SaveToStream( &PacketData );///< Skip the "Data Size" field.
	N32 = FEntryCode;
	N32.SaveToStream( &PacketData );
	N32 = FReserveBytes;
	N32.SaveToStream( &PacketData );

	FData.SaveToStream( &PacketData );///< Make the body stream.
	PacketData.Seek( 4, UFC::soFromBeginning );
	N32 = PacketData.GetSize() - RCQGP_HEADER_SIZE;
	N32.SaveToStream( &PacketData );

	UFC::PLockObject Lock( Socket->IOLock );
	Socket->BlockSend( (char*)PacketData.GetBuffer(), PacketData.GetSize() ); ///< Send the Packet.
}
//------------------------------------------------------------------------------------------------------------------------
