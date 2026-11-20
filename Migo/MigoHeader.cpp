#include "MigoHeader.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Class  MigoHeader
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString ADMIN_SUBJECT = "MBUS";
const UInt8 ProxyPotocolSign[12]={0x0D,0x0A,0x0D,0x0A,0x00,0x0D,0x0A,0x51,0x55,0x49,0x54,0x0A};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MigoHeader::MigoHeader( UFCType::Int32 Action, UFCType::Int32 MessageID, UFCType::Int32 SenderID )
:FACK( Action ),
 FSenderID( SenderID ),
 FMessageID( MessageID )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MigoHeader::MigoHeader( MigoHeader& Src )
:FACK( Src.FACK ),
 FSenderID( Src.FSenderID ),
 FMessageID( Src.FMessageID )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFCType::Int32 MigoHeader::DataChecksum( UInt8* Data, UFCType::Int32 Size )
{
    UFCType::Int32 Sum = 0;
    for( UFCType::Int32 i = 0; i < Size; i++ )
        Sum += *(Data+i);
    return Sum;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MigoHeader::SaveToStream( UFC::PStream* Stream )
{
    UFC::NInt32 N;

    N = FACK;
    N.SaveToStream( Stream ); ///< Save action code.
    N = FSenderID;
    N.SaveToStream( Stream ); ///< Save Sender ID.
    N = FMessageID;
    N.SaveToStream( Stream ); ///< Save Message ID.
    N = 0;
    N.SaveToStream( Stream ); ///< Save data size( Header only Size = 0)
	N = HeaderChecksum( 0 );
    N.SaveToStream( Stream ); ///< Save the Header Checksum.
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MigoHeader::SerializeToSocket( UFC::PClientSocket* Socket )
{
    UFC::MemoryStream WriteStream( MIGO_HEADER_SIZE );
    UFCType::Int32         DataSize;
    UFC::NInt32              NetInt;

    SaveToStream( &WriteStream );                        ///< Write all data to straeam.
    DataSize = ( WriteStream.GetSize() - MIGO_HEADER_SIZE ); ///< Data size not include Header size.
    WriteStream.Seek( MIGO_SIZE_FIELD_OFFSET, UFC::soFromBeginning ); ///< Seek to size field.
    NetInt = DataSize;
    NetInt.SaveToStream( &WriteStream );   ///< Save the size to stream.
	NetInt = HeaderChecksum( DataSize );       ///< Calculate the checksum.
    NetInt.SaveToStream( &WriteStream );   ///< Save the checksum to stream.

    UFC::PLockObject Lock( Socket->IOLock );
    ///< Write to socket
    Socket->BlockSend( (char*)WriteStream.GetBuffer(), WriteStream.GetSize()  );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
std::string MigoHeader::ToString( void )
{
    UFC::MemoryStream WriteStream( MIGO_HEADER_SIZE );
    UFCType::Int32             Size;
    UFC::NInt32       NetInt;

    SaveToStream( &WriteStream );                        ///< Write all data to straeam.
    Size = ( WriteStream.GetSize() - MIGO_HEADER_SIZE ); ///< Data size not include Header size.
    WriteStream.Seek( MIGO_SIZE_FIELD_OFFSET, UFC::soFromBeginning ); ///< Seek to size field.
    NetInt = Size;
    NetInt.SaveToStream( &WriteStream );   ///< Save the size to stream.
    NetInt = HeaderChecksum( Size );       ///< Calculate the checksum.
    NetInt.SaveToStream( &WriteStream );   ///< Save the checksum to stream.
    return std::string( (char*)WriteStream.GetBuffer(), WriteStream.GetSize() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MigoHeader* MigoHeader::CreateFromStream( UFC::PStream* Stream, UFCType::UInt32& RcvSize )
{
    UInt8              HeaderData[ MIGO_HEADER_SIZE ];

    if( Stream->Read( HeaderData, MIGO_HEADER_SIZE ) != MIGO_HEADER_SIZE )  ///< Receive package header from stream.
        return NULL;
    
    UFCType::Int32 Act      = UFC::NInt32::ToInt32( HeaderData    ); ///< Action code of this message.
    UFCType::Int32 SenderID = UFC::NInt32::ToInt32( HeaderData+4  ); ///< the net ID of the
    UFCType::Int32 MsgID    = UFC::NInt32::ToInt32( HeaderData+8  ); ///< Not use now...
    UFCType::Int32 Size     = UFC::NInt32::ToInt32( HeaderData+12 ); ///< Size onf this package.(Not include header)
    UFCType::Int32 Checksum = UFC::NInt32::ToInt32( HeaderData+16 ); ///< Checksum of this message.
    if( Checksum != ( Act + Size + MsgID ) )
    {
            UFC::BufferedLog::Printf("########### Checksum Error!!! Act:%d Size:%d ID:%d CS:%d ##########", Act, Size, MsgID, Checksum );
            RcvSize = 0;
            return NULL;
    }
    RcvSize = MIGO_HEADER_SIZE + Size;
    if( Size == 0 )
        return ProcessHeaderSizeMessage( Act, MsgID, SenderID );
    else
        return ProcessDataMessage( Act, MsgID, SenderID, Size , Stream );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsProxySignature( UInt8* Buffer )
{
    /// ProxyProtocolv2header
    /// Uint8[12]  Signature             value[0x0D0A0D0A000D0A515549540A]
    /// UInt8      Version and Command   0x21
    /// UIint8     Family and Protocol   0x11: IPv4+TCP  0x21: IPv6+TCP
    /// UInt16     Length
    if( Buffer[ 12 ] == 0x21 ) ///< Check Version and Command
    { 
        if( memcmp( Buffer, ProxyPotocolSign, 12 ) == 0 ) ///< Check Signature
        {
            UFC::BufferedLog::Printf( " Found a valid ProxyProtocol v2 header" );
            return true;
        }
        else
        {
           UFC::BufferedLog::Printf( " Not a ProxyProtocol v2 header, 0~11 Signature error" );
           return false;
        }
    }
    else
    {
        UFC::BufferedLog::Printf( " Not a ProxyProtocol v2 header, byts 12 != 0x21" );
        return false;
    }
}         
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MigoHeader* MigoHeader::CreateFromSocket( UFC::PClientSocket* Socket, UFCType::UInt32& RcvSize )
{
    ///< Preserve the pre-pool ABI symbol; behavior identical to the pooled overload with Pool=NULL.
    return CreateFromSocket( Socket, RcvSize, NULL );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MigoHeader* MigoHeader::CreateFromSocket( UFC::PClientSocket* Socket, UFCType::UInt32& RcvSize, DataMessagePool* Pool )
{
    UInt8              HeaderData[ MIGO_HEADER_SIZE ]; /// 20 bytes
    UFC::PLockObject   Lock( Socket->ReadLock );

    Socket->BlockRecv( HeaderData, MIGO_HEADER_SIZE );      ///< Receive package header from Socket.
    UFCType::Int32 Act      = UFC::NInt32::ToInt32( HeaderData    ); ///< Action code of this message.
    UFCType::Int32 SenderID = UFC::NInt32::ToInt32( HeaderData+4  ); ///< the net ID of the
    UFCType::Int32 MsgID    = UFC::NInt32::ToInt32( HeaderData+8  ); ///< Not use now...
    UFCType::Int32 Size     = UFC::NInt32::ToInt32( HeaderData+12 ); ///< Size onf this package.(Not include header)
    UFCType::Int32 Checksum = UFC::NInt32::ToInt32( HeaderData+16 ); ///< Checksum of this message.
    if( Checksum != ( Act + Size + MsgID ) )
    {
        if( IsProxySignature( HeaderData ) == true )
            return ProcessProxyProtocol( HeaderData , Socket );
        UFC::BufferedLog::Printf("########### Checksum Error!!! Act:%d Size:%d ID:%d CS:%d ##########", Act, Size, MsgID, Checksum );
        RcvSize = 0;
        return NULL;
    }
    RcvSize = MIGO_HEADER_SIZE + Size;
    if( Size == 0 )
        return ProcessHeaderSizeMessage( Act, MsgID, SenderID );
    else
        return ProcessDataMessage( Act, MsgID, SenderID, Size , Socket, Pool );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MigoHeader* MigoHeader::ProcessHeaderSizeMessage( UFCType::Int32 Act, UFCType::Int32 MsgID, UFCType::Int32 SenderID )
{
    MigoHeader* Message;

    switch( Act )
    {
        case ACK_CONNECT_CONFIRM:
        case ACK_DISCONNECT:
        case ACK_HEARTBEAT:
        case ACK_HEARTBEAT_CONFIRM:
        case ACK_SERVER_LIST:
        case ACK_TEST_EXISTS:
        case ACK_NEW_SERVER_UP:     Message = new MigoHeader( Act, MsgID, SenderID );
                                                        break;
        case ACK_SERVER_CONNECT:   Message = new MBusConnectMessage( MsgID, "" ); ///< Old version MBus.Don't known it's hostname.                                
                                                        break;
        default:                                    UFC::BufferedLog::Printf(" ##### Received unknown header Ack:%d Sender:%d #####", Act, SenderID );
                                                        Message = NULL;
                                                        break;
     }
     return Message;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MigoHeader* MigoHeader::ProcessDataMessage( UFCType::Int32 Act, UFCType::Int32 MsgID, UFCType::Int32 SenderID, UFCType::Int32 Size , UFC::PClientSocket* Socket, DataMessagePool* Pool )
{
    ///< Order hot path (ACK_MESSAGE): construct straight from the socket, no
    ///< intermediate stack stream and no payload copy-construct. When a pool is
    ///< supplied, reuse a recycled object (and its buffer) instead of new/delete.
    if( Act == ACK_MESSAGE )
    {
        if( Pool != NULL )
            return Pool->Acquire( MsgID, SenderID, Size, Socket );
        return new DataMessage( MsgID, SenderID, Size, Socket );
    }

    UFC::MemoryStream DataStream( Size, Size );

    Socket->BlockRecv( (UInt8*)DataStream.GetBuffer(),Size );///< Receive package data from Socket.
    return CreateMessageFromStream( Act, MsgID, SenderID, Size , &DataStream );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MigoHeader* MigoHeader::ProcessDataMessage( UFCType::Int32 Act, UFCType::Int32 MsgID, UFCType::Int32 SenderID, UFCType::Int32 Size , UFC::PStream* Stream )
{
    if( Stream->GetSize() - Stream->GetPosition() < Size )
    {
        UFC::BufferedLog::Printf(" ##### Bad MigoHeader stream, Stream size:%d Size in header:%d #####", Stream->GetSize(), Size );
        return NULL;           
    }
    else
    {
         UFC::MemoryStream DataStream( Size, Size );

        if( Stream->Read( (UInt8*)DataStream.GetBuffer(),Size ) != Size )
            return NULL;   
        return CreateMessageFromStream( Act, MsgID, SenderID, Size , &DataStream );
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MigoHeader* MigoHeader::ProcessProxyProtocol( UInt8* Data, UFC::PClientSocket* Socket )
{
    if( Data[13] !=0x11 )
    {
        UFC::BufferedLog::Printf( " Received unsupport ProxyProtocolv2 IPV4 Header" );
        return NULL;
    }
    UInt8  PeerIP[4];
    UInt16 LocalPort,PeerPort;
    UFC::AnsiString SrcIP,DestIP;
    
    Socket->BlockRecv( (UInt8*)&LocalPort, 2 );
    Socket->BlockRecv( PeerIP, 4 );
    Socket->BlockRecv( (UInt8*)&PeerPort, 2 );
    SrcIP.Printf("%d.%d.%d.%d",Data[16],Data[17],Data[18],Data[19] );
    DestIP.Printf("%d.%d.%d.%d",PeerIP[0],PeerIP[1],PeerIP[2],PeerIP[3] );
    UFC::BufferedLog::Printf( " ProxyProtocolv2 IPV4 Header" );
    UFC::BufferedLog::Printf( " Source IP[%s:%d]", SrcIP.c_str(), LocalPort );
    UFC::BufferedLog::Printf( " Destination IP[%s:%d]", DestIP.c_str(), PeerPort );
    
    return new ProxyPotocolMessage( SrcIP, DestIP );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MigoHeader* MigoHeader::CreateMessageFromStream( UFCType::Int32 Act, UFCType::Int32 MsgID, UFCType::Int32 SenderID, UFCType::Int32 Size , UFC::MemoryStream* DataStream )
{
    MigoHeader* Message;

    switch( Act )
    {
        case ACK_ADMIN_MESSAGE:     Message = new AdminMessage( MsgID, SenderID, DataStream ); break;
        case ACK_CONNECT:           Message = new ConnectMessage( MsgID, SenderID, DataStream ); break;
        case ACK_MESSAGE:           Message = new DataMessage( MsgID, SenderID, DataStream ); break;
        case ACK_REGISTER:          Message = new RegistMessage( MsgID, SenderID, DataStream ); break;
        case ACK_DELETE_LISTENER:   Message = new UnregistMessage( MsgID, SenderID, DataStream ); break;
        case ACK_SERVER_CONNECT:    Message = new MBusConnectMessage( MsgID, SenderID, DataStream ); break;                                         
        default:                 UFC::BufferedLog::Printf(" ##### Received unknown data Ack:%d Size:%d Sender:%d #####", Act, Size,SenderID );
                                    Message = NULL;
                                    break;
    }
    return Message;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class MBusConnectMessage
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MBusConnectMessage::MBusConnectMessage( UFCType::Int32 ID, UFCType::Int32 SenderID, UFC::MemoryStream* DataStream )
:MigoHeader( ACK_SERVER_CONNECT, ID, SenderID )
{
    FHostname.LoadFromStream( DataStream );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MBusConnectMessage::MBusConnectMessage( UFCType::Int32 ID, const MString &Hostname )
:MigoHeader( ACK_SERVER_CONNECT, ID, 0 )
,FHostname( Hostname )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MBusConnectMessage::MBusConnectMessage( MBusConnectMessage& Src )
:MigoHeader( Src )   
,FHostname( Src.FHostname )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MBusConnectMessage::SaveToStream( UFC::PStream* Stream )
{
    MigoHeader::SaveToStream( Stream );
    FHostname.SaveToStream( Stream );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class ProxyPotocolMessage
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ProxyPotocolMessage::ProxyPotocolMessage( const MString &LocalIP , const MString &PeerIP )
: MigoHeader( ACK_PROXY_PROTOCOL, 0, 0 )
,FLocalIP( LocalIP )
,FPeerIP( PeerIP )
{
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class ConnectMessage
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ConnectMessage::ConnectMessage( UFCType::Int32 ID, UFCType::Int32 SenderID, UFC::MemoryStream* DataStream )
: MigoHeader( ACK_CONNECT, ID, SenderID )
{
    FAppName.LoadFromStream( DataStream );
    if( FAppName.Length() == 0 )
        FAppName = "MBusApp";
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ConnectMessage::ConnectMessage( UFCType::Int32 ID, const MString &AppName )
: MigoHeader( ACK_CONNECT, ID, 0 ),
  FAppName( AppName )
{
   if( FAppName.Length() == 0 )
       FAppName = "MBusApp";
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ConnectMessage::ConnectMessage( ConnectMessage& Src )
:MigoHeader( Src )
,FAppName( Src.FAppName )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ConnectMessage::SaveToStream( UFC::PStream* Stream )
{
    MigoHeader::SaveToStream( Stream );
    FAppName.SaveToStream( Stream );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class RegistMessage
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
RegistMessage::RegistMessage( UFCType::Int32 ID, UFCType::Int32 SenderID, UFC::MemoryStream* DataStream  )
:MigoHeader( ACK_REGISTER, ID, SenderID )
,FRegAllAll( false )
{
	MString     Subject, Key;
	UFC::NInt32 PairCount;
	UFC::NInt32 ServerFlag;

	ServerFlag.LoadFromStream( DataStream );
	PairCount.LoadFromStream( DataStream );
	for( int i = 0; i < (UFCType::Int32)PairCount; i++ )
	{
		Subject.LoadFromStream( DataStream );
		Key.LoadFromStream( DataStream );
		FPairList.Add( std::make_pair( Subject, Key ) );
		if( Subject == "all" && Key == "all" )
			FRegAllAll = true;
	}
	FFlag = (Int32)ServerFlag;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
RegistMessage::RegistMessage( Int32 ID, const MString &Subject, const MString &Key, Int32 Flag  )
:MigoHeader( ACK_REGISTER, ID, 0 )
,FFlag( Flag )
,FRegAllAll( false )
{
	FPairList.Add( std::make_pair( Subject, Key ) );
	if( Subject == "all" && Key == "all" )
		FRegAllAll = true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
RegistMessage::RegistMessage( RegistMessage& Src )
:MigoHeader( Src )
,FFlag( Src.FFlag )
,FRegAllAll( Src.FRegAllAll )
{
	for( int i = 0; i < Src.FPairList.ItemCount(); i++ )
	{
		SubjectKeyPair Pair = Src.FPairList.GetItem( i );
		FPairList.Add( std::make_pair( Pair.first, Pair.second ) );
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
RegistMessage::RegistMessage( Int32 ID, BOOL Flag )
: MigoHeader( ACK_REGISTER, ID, 0 )
,FFlag( Flag )
,FRegAllAll( false )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void RegistMessage::Add( const MString& Subject, const MString& Key )
{
	if( Subject == "all" && Key == "all" )
		FRegAllAll = true;
	FPairList.Add( std::make_pair( Subject, Key ) );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 RegistMessage::Remove( const MString& Subject, const MString& Key )
{
	for( int i = 0; i < FPairList.ItemCount(); i++ )
	{
		if( FPairList.GetItem( i ).first == Subject &&
			FPairList.GetItem( i ).second == Key )
		{
			if( Subject == "all" && Key == "all" )
				FRegAllAll = false;
			FPairList.Delete( i );
            return i;
        }
    }
    return -1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void RegistMessage::SaveToStream( UFC::PStream* Stream )
{
	UFC::NInt32 PairCount = FPairList.ItemCount();
	UFC::NInt32 ServerFlag  = FFlag;

	MigoHeader::SaveToStream( Stream );
	ServerFlag.SaveToStream( Stream );
	if( FRegAllAll == false )
	{
		PairCount.SaveToStream( Stream );
		for( int i = 0; i < FPairList.ItemCount(); i++ )
		{
			FPairList.GetItem( i ).first.SaveToStream( Stream );
			FPairList.GetItem( i ).second.SaveToStream( Stream );
		}
	}
	else
	{
		UFC::AnsiString AllString( "all" );

		PairCount = 1;
		PairCount.SaveToStream( Stream );
		AllString.SaveToStream( Stream );
		AllString.SaveToStream( Stream );
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  RegistMessage::Clear()
{
	FPairList.Clear();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class UnregistMessage
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UnregistMessage::UnregistMessage( Int32 ID, Int32 SenderID, UFC::MemoryStream* DataStream  )
: MigoHeader( ACK_DELETE_LISTENER, ID, SenderID )
{
    FSubject.LoadFromStream( DataStream );
    FKey.LoadFromStream( DataStream );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UnregistMessage::UnregistMessage( Int32 ID, const MString &Subject, const MString &Key )
: MigoHeader( ACK_DELETE_LISTENER, ID, 0 ),
  FSubject( Subject ),
  FKey( Key )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UnregistMessage::UnregistMessage( UnregistMessage& Src )
:MigoHeader( Src )
,FSubject( Src.FSubject )
,FKey( Src.FKey )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void UnregistMessage::SaveToStream( UFC::PStream* Stream )
{
    MigoHeader::SaveToStream( Stream );
    FSubject.SaveToStream( Stream );
    FKey.SaveToStream( Stream );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Class DataMessage
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor. create object from memory stream.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DataMessage::DataMessage( Int32 ID, Int32 SenderID, UFC::MemoryStream* DataStream  )
:MigoHeader( ACK_MESSAGE, ID ,SenderID ),
 FIPAddress( "" ),
 FStream( *DataStream ),
 FFD( -1 )
{
    FSubject.LoadFromStream( &FStream );
    FKey.LoadFromStream( &FStream );
    FDataPos = FStream.GetPosition();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor. Read the payload directly from the socket into FStream (order hot path).
// Mirrors the stream ctor's parse, but skips the intermediate stack MemoryStream + copy.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DataMessage::DataMessage( Int32 ID, Int32 SenderID, Int32 Size, UFC::PClientSocket* Socket )
:MigoHeader( ACK_MESSAGE, ID, SenderID ),
 FIPAddress( "" ),
 FStream( Size, Size ),
 FFD( -1 )
{
    InitFromSocket( ID, SenderID, Size, Socket );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Empty object for pool preallocation. Populated later by InitFromSocket().
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DataMessage::DataMessage( void )
:MigoHeader( ACK_MESSAGE ),
 FIPAddress( "" ),
 FStream( 64 ),
 FDataPos( 0 ),
 FFD( -1 )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Repopulate this object (fresh or recycled) from the socket, reusing FStream's
// buffer. SetSize grows the buffer only when the new payload is larger; Seek(0)
// resets the read position that getData() may have advanced on a prior use.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DataMessage::InitFromSocket( Int32 ID, Int32 SenderID, Int32 Size, UFC::PClientSocket* Socket )
{
    SetACK( ACK_MESSAGE );
    SetMessageID( ID );
    SetSenderID( SenderID );
    FIPAddress = "";
    FFD        = -1;
    FStream.SetSize( Size );                 ///< ensure capacity >= Size (grows only if needed)
    FStream.Seek( 0, SEEK_SET );             ///< reset read position for LoadFromStream below
    Socket->BlockRecv( (UFCType::UInt8*)FStream.GetBuffer(), Size ); ///< recv straight into our own buffer
    FSubject.LoadFromStream( &FStream );
    FKey.LoadFromStream( &FStream );
    FDataPos = FStream.GetPosition();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor. create object from MTree.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DataMessage::DataMessage( Int32 ID, const MString &Subject, const MString &Key, MTree &Data )
: MigoHeader( ACK_MESSAGE, ID, 0 ),
  FSubject( Subject ),
  FKey( Key ),
  FIPAddress( "" ),
  FStream( 128 ),  
  FFD( -1 )
{
    FSubject.SaveToStream( &FStream );
    FKey.SaveToStream( &FStream );
    FDataPos = FStream.GetPosition();
    Data.SaveToStream( &FStream );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Copy Constructor.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DataMessage::DataMessage( DataMessage& Src )
:MigoHeader( Src )
,FSubject( Src.FSubject )
,FKey( Src.FKey )
,FIPAddress( "" )
,FStream( Src.FStream )
,FDataPos( Src.FDataPos )
,FFD( -1 )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Every message on the forwarding hot path comes through here.
//
//  The original opened a separate MemoryStream, wrote "header + payload" into it, filled in
//  size/checksum, then copied the whole thing into the caller's Stream: the payload was copied twice,
//  plus an extra buffer allocation; and ToString()/SerializeToSocket() then copied it once more
//  into a std::string -- three copies in total.
//
//  Now it writes straight into the caller's Stream. size and checksum are filled in by the caller
//  (ToString() and SerializeToSocket() already do this, and both pass a freshly created empty stream,
//  so the offsets are known).
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DataMessage::SaveToStream( UFC::PStream* Stream )
{
    MigoHeader::SaveToStream( Stream );   ///< 20-byte header (size/checksum left as 0 for now)
    Stream->Write( (void*)FStream.GetBuffer(), FStream.GetSize() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DataMessage::getData( MTree& Data )
{
    FStream.Seek( FDataPos, SEEK_SET );
    Data.LoadFromStream( &FStream );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
///< Wire (per MTree/MNode/AnsiString SaveToStream): NInt32 nodeCount, then each node
///< [NInt32 nameLen][name (nameLen bytes, incl null)][NInt32 size][NInt32 type][value].
///< NInt32 is big-endian (UFC::NInt32::ToInt32). Zero allocation; Order/ID point into
///< FStream's buffer (valid while this DataMessage lives).
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
///< Accept a field of interest only in the exact shape MTree::get accepts it; anything else
///< makes ScanOrderFields return false so the caller falls back to getData.
static inline bool ScanStringField( int Type, const char* Val, int Width, const char*& Out, int& OutLen )
{
    ///< MTree::get(MString&) reads an M_STRING node as a C string (up to the first NUL).
    if( Type != M_STRING || Width < 1 )
        return false;
    const char* Nul = (const char*)memchr( Val, 0, Width );
    if( Nul == NULL )
        return false;
    Out    = Val;
    OutLen = (int)( Nul - Val );
    return true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static inline bool ScanIntField( int Type, const char* Val, int& Out )
{
    ///< MTree::get(int&) only accepts M_INT (4 bytes on the wire).
    if( Type != M_INT )
        return false;
    Out = UFC::NInt32::ToInt32( (UFCType::UInt8*)Val );
    return true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
///< Walks the payload exactly the way MNode::LoadFromStream does, so the fields found here are
///< the ones getData would produce. Anything not modelled (a field of interest with an
///< unexpected type, a negative or truncated length) returns false and the caller falls back
///< to getData -- never a partial or misaligned result.
bool DataMessage::ScanOrderFields( FastOrderFields& o )
{
    o = FastOrderFields();
    const char* base = (const char*)FStream.GetBuffer();
    const char* end  = base + (long)FStream.GetSize();
    const char* p    = base + FDataPos;
    if( end - p < 4 ) return false;
    int count = UFC::NInt32::ToInt32( (UFCType::UInt8*)p ); p += 4;
    if( count < 0 ) return false;
    for( int i = 0; i < count; i++ )
    {
        if( end - p < 4 ) return false;
        int nameLen = UFC::NInt32::ToInt32( (UFCType::UInt8*)p ); p += 4;
        if( nameLen < 0 || end - p < nameLen ) return false;
        const char* name = p; p += nameLen;
        if( end - p < 8 ) return false;
        int size = UFC::NInt32::ToInt32( (UFCType::UInt8*)p ); p += 4;
        int type = UFC::NInt32::ToInt32( (UFCType::UInt8*)p ); p += 4;
        ///< Bytes the value occupies on the wire (mirrors MNode::LoadFromStream): STRING/ROWDATA/
        ///< COMPRESSEDDATA use the size field, DOUBLE/INT64 are 8, every other type is 4 --
        ///< SHORT/LONG/... are written as 4 bytes even though their size field says 2 or 8.
        int width;
        if( type == M_STRING || type == M_ROWDATA || type == M_COMPRESSEDDATA )
            width = size;
        else if( type == M_DOUBLE || type == M_INT64 )
            width = 8;
        else
            width = 4;
        if( width < 0 || end - p < width ) return false;
        const char* val = p; p += width;
        ///< Names are compared including their NUL terminator (nameLen counts it).
        if     ( nameLen == 6 && memcmp( name, "ORDER",    6 ) == 0 ) { if( !ScanStringField( type, val, width, o.Order, o.OrderLen ) ) return false; o.HasOrder = true; }
        else if( nameLen == 3 && memcmp( name, "ID",       3 ) == 0 ) { if( !ScanStringField( type, val, width, o.ID, o.IDLen ) ) return false;       o.HasID = true; }
        else if( nameLen == 4 && memcmp( name, "NID",      4 ) == 0 ) { if( !ScanIntField( type, val, o.NID ) ) return false;                         o.HasNID = true; }
        else if( nameLen == 6 && memcmp( name, "GROUP",    6 ) == 0 ) { if( !ScanIntField( type, val, o.Group ) ) return false;                       o.HasGroup = true; }
        else if( nameLen == 8 && memcmp( name, "OFFHOUR",  8 ) == 0 ) { int v; if( !ScanIntField( type, val, v ) ) return false; o.OffHour = ( v != 0 ); o.HasOffHour = true; }
        else if( nameLen == 9 && memcmp( name, "STRATEGY", 9 ) == 0 ) { o.HasStrategy = true; }   ///< touch/stop order marker (any type, like Exists())
        else if( nameLen == 7 && memcmp( name, "PeerIP",   7 ) == 0 ) { if( !ScanStringField( type, val, width, o.PeerIP, o.PeerIPLen ) ) return false; o.HasPeerIP = true; }
    }
    return o.HasOrder;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class DataMessagePool
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DataMessagePool::DataMessagePool( UFCType::Int32 Prealloc )
:FFree( ( Prealloc * 2 > 1024 ) ? Prealloc * 2 : 1024 )   ///< queue capacity; excess on Release is freed
{
    for( UFCType::Int32 i = 0; i < Prealloc; i++ )
        FFree.InqueueNoWait( new DataMessage() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DataMessagePool::~DataMessagePool()
{
    DataMessage* Msg;
    while( ( Msg = FFree.DequeueNoWait() ) != NULL )
        delete Msg;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DataMessage* DataMessagePool::Acquire( UFCType::Int32 ID, UFCType::Int32 SenderID, UFCType::Int32 Size, UFC::PClientSocket* Socket )
{
    DataMessage* Msg = FFree.DequeueNoWait();

    if( Msg == NULL )
        Msg = new DataMessage();   ///< pool empty: grow lazily
    try
    {
        Msg->InitFromSocket( ID, SenderID, Size, Socket );
    }
    catch( ... )
    {
        Release( Msg );   ///< object memory is still valid; recycle it, then propagate
        throw;
    }
    return Msg;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DataMessagePool::Release( DataMessage* Msg )
{
    if( Msg == NULL )
        return;
    if( FFree.InqueueNoWait( Msg ) == FALSE )   ///< pool full: free rather than grow unbounded
        delete Msg;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// *************** Header ***************
// Act       Int32   Action code of this message.
// SenderID  Int32   The net ID of the sender
// MsgID     Int32   Not use now...
// Size      Int32   Size onf this package.(Not include header)
// Checksum  Int32   Checksum of this message.(Act + Size + Message ID)
// **************** Body ****************
// Subject   String  Size(NInt32) + String body
// Key       String  Size(NInt32) + String body
// NodeCount Int32   Number of nodes count in MTree.
//
//     Name  String  Name of this node.
//     Size  Int32   Size of the data.( Int32:4,float:4,String:strlen(str) )
//     Type  Int32   Data type of this node.(enum MDataType 0~18)
//     Data  binary  Row data.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DataMessage::MessageToStream( Int32 ID, const MString &Subject, const MString &Key, MTree &Data, UFC::MemoryStream* WriteStream )
{
	Int32             Size;
	UFC::NInt32       NetInt;

	///< Write MIGO_HEADER_SIZE bytes to Skip the message header.
	WriteStream->Write( MIGO_HEADER_SIZE );
	///< Save message body.
	Subject.SaveToStream( WriteStream );///< Save Subject to straeam.
	Key.SaveToStream( WriteStream );    ///< Save Key to straeam.
	Data.SaveToStream( WriteStream );   ///< Write MTree to straeam.
	///< To fill all fields in Header
	///< Calculate , update the checksum and the size.
	WriteStream->Seek( 0, UFC::soFromBeginning ); ///< Seek to the begining.
	NetInt = ACK_MESSAGE;
	NetInt.SaveToStream( WriteStream );          ///< Save action code.
	NetInt = 0;
	NetInt.SaveToStream( WriteStream );          ///< Save Sender ID.
	NetInt = ID;
	NetInt.SaveToStream( WriteStream );          ///< Save Message ID.
	Size = ( WriteStream->GetSize() - MIGO_HEADER_SIZE ); ///< Data size not include Header size.
	NetInt = Size;
	NetInt.SaveToStream( WriteStream );          ///< Save the size to stream.
	NetInt = MigoHeader::HeaderChecksum( ACK_MESSAGE, Size, ID ); ///< Calculate the checksum.
	NetInt.SaveToStream( WriteStream );          ///< Save the checksum to stream.
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DataMessage::MessageToSocket( Int32 ID, const MString &Subject, const MString &Key, MTree &Data, UFC::PClientSocket* Socket )
{
	UFC::MemoryStream WriteStream( MIGO_HEADER_SIZE * 4 );

	MessageToStream( ID, Subject, Key, Data, &WriteStream  );
	///< Check Data size.
	/*if( WriteStream.GetSize( ) > MBUS_MSG_MAX )
	{
		UFC::AnsiString ErrMsg;
		ErrMsg.Printf( "MTree size too large.(Max:%d This:%d)", MBUS_MSG_MAX, WriteStream.GetSize( ) );
		throw( UFC::Exception( ErrMsg.c_str() ) );
	}*/
	///< Lock the IO critical section and send the data
	UFC::PLockObject Lock( Socket->IOLock );

	Socket->BlockSend( (char*)WriteStream.GetBuffer(), WriteStream.GetSize() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Class AdminMessage
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AdminMessage::AdminMessage( Int32 ID, Int32 SenderID, UFC::MemoryStream* DataStream  )
:MigoHeader( ACK_ADMIN_MESSAGE, ID ,SenderID ),
 FStream( *DataStream )
{
    FKey.LoadFromStream( &FStream );
    FDataPos = FStream.GetPosition();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AdminMessage::AdminMessage( Int32 ID, const MString &Key, MTree &Data )
: MigoHeader( ACK_ADMIN_MESSAGE, ID, 0 ),
  FKey( Key ),
  FStream( 128 )
{
    FKey.SaveToStream( &FStream );
    FDataPos = FStream.GetPosition();
    Data.SaveToStream( &FStream );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AdminMessage::AdminMessage( AdminMessage& Src )
:MigoHeader( Src )
,FKey( Src.FKey ) 
,FStream( Src.FStream )
,FDataPos( Src.FDataPos )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AdminMessage::SaveToStream( UFC::PStream* Stream )
{
    MigoHeader::SaveToStream( Stream );
    Stream->Write( (void*)FStream.GetBuffer(), FStream.GetSize());
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AdminMessage::getData( MTree& Data )
{
    FStream.Seek( (Int64)FDataPos, SEEK_SET );
    Data.LoadFromStream( &FStream );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
