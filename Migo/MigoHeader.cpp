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
    register UFCType::Int32 Sum = 0;
    for( register UFCType::Int32 i = 0; i < Size; i++ )
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
    /// Proxy Protocol v2 header 
    /// Uint8[12]  Signature             value[ 0x0D0A0D0A000D0A515549540A]
    /// UInt8      Version and Command   0x21
    /// UIint8     Family and Protocol   0x11 :  IPv4 + TCP  0x21 :  IPv6 + TCP
    /// UInt16     Length
    if( Buffer[ 12 ] == 0x21 ) ///< Check Version and Command
    { 
        if( memcmp( Buffer, ProxyPotocolSign, 12 ) == 0 ) ///< Check Signature
        {
            UFC::BufferedLog::Printf( " Found a valid Proxy Protocol  v2  header " );
            return true;
        }
        else
        {
           UFC::BufferedLog::Printf( " Not a Proxy Protocol  v2  header , 0~11 Signature error" );
           return false;
        }
    }
    else
    {
        UFC::BufferedLog::Printf( " Not a Proxy Protocol  v2  header , byts 12 != 0x21" );
        return false;
    }
}         
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MigoHeader* MigoHeader::CreateFromSocket( UFC::PClientSocket* Socket, UFCType::UInt32& RcvSize )
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
        return ProcessDataMessage( Act, MsgID, SenderID, Size , Socket );
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
MigoHeader* MigoHeader::ProcessDataMessage( UFCType::Int32 Act, UFCType::Int32 MsgID, UFCType::Int32 SenderID, UFCType::Int32 Size , UFC::PClientSocket* Socket )
{
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
        UFC::BufferedLog::Printf( "  Received unsupport Proxy Protocol v2 IPV4 Header" );
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
    UFC::BufferedLog::Printf( "  Proxy Protocol v2 IPV4 Header" );
    UFC::BufferedLog::Printf( "  Source IP[%s:%d]", SrcIP.c_str(), LocalPort );
    UFC::BufferedLog::Printf( "  Destination IP[%s:%d]", DestIP.c_str(), PeerPort );
    
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
	for( register int i = 0; i < Src.FPairList.ItemCount(); i++ )
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
	for( register int i = 0; i < FPairList.ItemCount(); i++ )
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
void DataMessage::SaveToStream( UFC::PStream* Stream )
{
    UFC::MemoryStream WriteStream( MIGO_HEADER_SIZE *5 );
    Int32             Size;
    UFC::NInt32       NetInt;

    MigoHeader::SaveToStream( &WriteStream );                        ///< Write all data to straeam.
    WriteStream.Write( (void*)FStream.GetBuffer(), FStream.GetSize());
    Size = ( WriteStream.GetSize() - MIGO_HEADER_SIZE ); ///< Data size not include Header size.
    WriteStream.Seek( MIGO_SIZE_FIELD_OFFSET, UFC::soFromBeginning ); ///< Seek to size field.
    NetInt = Size;
    NetInt.SaveToStream( &WriteStream );   ///< Save the size to stream.
    NetInt = HeaderChecksum( Size );       ///< Calculate the checksum.
    NetInt.SaveToStream( &WriteStream );   ///< Save the checksum to stream.
    Stream->Write( (char*)WriteStream.GetBuffer(), WriteStream.GetSize() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DataMessage::getData( MTree& Data )
{
    FStream.Seek( FDataPos, SEEK_SET );
    Data.LoadFromStream( &FStream );
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
