#include "PUDPClient.h"
#include "BufferedLog.h"
#ifdef __LINUX
    #include <ifaddrs.h>
#endif        
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
const Int32  PUDPClient::ANY_PORT = 0;
//---------------------------------------------------------------------------
PUDPClient::PUDPClient( void )
:PSocket( SOCK_DGRAM, BLOCKING_SOCKET )
,PThread( NULL, FALSE )
,FAddress( "any" )
,FPort( ANY_PORT )
,FListener( NULL )
,FSendBufferSize( DEFAULT_BUFFER_SIZE )
,FRecvBufferSize( DEFAULT_BUFFER_SIZE )
,FCheckInterval( 10 )
,FMulticastAddress("")
,FIsBind( FALSE )
,FCopyData( FALSE )
,FRecvStream( DEFAULT_BUFFER_SIZE )
{

}
//---------------------------------------------------------------------------
// Address:    "any"
// Port:       ANY_PORT
// ThreadMode: true 
//---------------------------------------------------------------------------
PUDPClient::PUDPClient( const AnsiString& Address, Int32 Port, BOOL ThreadMode, int core )
:PSocket( SOCK_DGRAM, BLOCKING_SOCKET )
,PThread( NULL, FALSE )
,FAddress( Address )
,FPort( Port )
,FListener( NULL )
,FSendBufferSize( DEFAULT_BUFFER_SIZE )
,FRecvBufferSize( DEFAULT_BUFFER_SIZE )
,FCheckInterval( 10 )
,FMulticastAddress("")
,FIsBind( FALSE )
,FCopyData( FALSE )
,FRecvStream( DEFAULT_BUFFER_SIZE )
{
	if( ThreadMode == TRUE )
        {
	    Start(  );
            if( core != -1)
                PThread_setaffinity( core );
        }
}
//---------------------------------------------------------------------------
PUDPClient::~PUDPClient()
{
    Close();
    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
    if( FRecvBuffer != NULL )
        delete [] FRecvBuffer;
}
//---------------------------------------------------------------------------
BOOL PUDPClient::GetInterfaceIP( const AnsiString& Interface, AnsiString& IP )
{
    BOOL Rtn = false;
    IP = "any";
#ifdef __LINUX    
    if( Interface.Length() > 0 )
    {
        struct ifaddrs *ifa, *ifa_tmp;

        if( getifaddrs( &ifa ) != -1) 
        {                    
            ifa_tmp = ifa;
            while( ifa_tmp ) 
            {
                if (!strcmp(ifa_tmp->ifa_name, Interface) && ifa_tmp->ifa_addr && (ifa_tmp->ifa_addr->sa_family == AF_INET)) 
                {
                    UFC::AnsiString IPAddr( inet_ntoa(((struct sockaddr_in *)ifa_tmp->ifa_addr)->sin_addr) );

                    BufferedLog::Printf( " Interface[%s]->[%s]", Interface.c_str(), IPAddr.c_str() );
                    IP = IPAddr;                        
                    Rtn = true;
                    break;
                }
                ifa_tmp = ifa_tmp->ifa_next;
            }
            freeifaddrs(ifa);
        }
    }    
#endif
    return Rtn;
}
//---------------------------------------------------------------------------
void PUDPClient::Open( void )
{
    if( FPort > 0 )
    {
        try
        {            
            OpenSocket();
            SetSendBufferSize( FSendBufferSize );
            SetRecvBufferSize( FRecvBufferSize );
            SetReuse( TRUE );
            SetBroadcast( 1 );
            if( FPort != ANY_PORT )
            {
                Bind( "any", FPort );
                FIsBind = TRUE;
            }
        }
        catch( SocketException& e )
        {
            BufferedLog::Printf( "%s", e.what() );
        }
    }
}
//---------------------------------------------------------------------------
void PUDPClient::OpenForMulticast( void )
{
	if ( FPort > 0 )
	{
		try
		{
			OpenSocket();
			SetSendBufferSize( FSendBufferSize );
			SetRecvBufferSize( FRecvBufferSize );
			SetReuse( TRUE );
		}
		catch( SocketException& e )
		{
			BufferedLog::Printf( "%s", e.what() );
		}
	}
}
//---------------------------------------------------------------------------
void PUDPClient::BindForMulticast( const AnsiString& MulticastAddress )
{
	if ( FPort > 0 )
	{
		try
		{
			Bind( MulticastAddress, FPort );
			FIsBind = TRUE;
		}
		catch( SocketException& e )
		{
			BufferedLog::Printf( "%s", e.what() );
		}
	}
}
//---------------------------------------------------------------------------
void PUDPClient::Close( void )
{
    ///< Ignore exception DropMulticastAddress failed.
    try
    {
        DropMulticastAddress( FMulticastAddress );
    }
    catch( SocketException& e )
    {
        BufferedLog::Printf( " PUDPClient::Close occurs exception, ignore it. <%s>", e.what() );
    }
    CloseSocket();
    FIsBind = FALSE;
}
//------------------------------------------------------------------------------
void PUDPClient::Execute( void )
{
    MemoryStream* DataStream;
        
    while ( IsTerminated() == FALSE )
    {
        if( FIsBind != FALSE )
        {
            try
            {
                if( RecvUDPData( DataStream ) > 0 && FListener != NULL )
                    FListener->OnDataArrived( this, DataStream, inet_ntoa( FRecvAddress.sin_addr) );                                
            }
            catch( SocketException & e )
            {
                BufferedLog::Printf("%s\n", e.what() );
            }
        }
        else
              UFC::SleepMS( 50 );  
    }
}
//---------------------------------------------------------------------------
Int32 PUDPClient::RecvUDPData(  MemoryStream*& OutDataStream )
{
    #ifdef _WIN32
    Int32 len = sizeof( struct sockaddr_in );
    #else
    socklen_t len = sizeof( struct sockaddr_in );
    #endif    

    memset( &FRecvAddress, 0, len );
    FRecvDataSize = (Int32)recvfrom( GetHandle()
                            , (char*)FRecvStream.GetBuffer()
                            , FRecvBufferSize
                            , 0
                            , (sockaddr*)&FRecvAddress
                            , &len );
    if( FRecvDataSize > 0  && FRecvDataSize <= FRecvBufferSize )
    {
        FRecvStream.SetSize( FRecvDataSize );        
        FRecvStream.Seek( (Int64)0, soFromBeginning );
        if( FCopyData == FALSE )
            OutDataStream = &FRecvStream; 
        else
            OutDataStream = new UFC::MemoryStream( FRecvStream );
    }
    else
           BufferedLog::Printf( " ########## recvfrom return[%d] ##########", FRecvDataSize);
    return FRecvDataSize;    
}
//---------------------------------------------------------------------------
Int32 PUDPClient::RecvData( MemoryStream*& OutDataStream )
{
    Int32  Count;
    fd_set FReadSet;
    struct timeval SelectTimeout;
    
    SelectTimeout.tv_sec  = 0;
    SelectTimeout.tv_usec = FCheckInterval*1000; ///< 1 ms
    FD_ZERO( &FReadSet );       ///< Empty the Read set.
    FD_SET( (u_int)FFD, &FReadSet );
    if( (Count = select( FFD + 1, &FReadSet, NULL, NULL, &SelectTimeout)) < 0 )
        throw( SocketException( "ClientSocket select failed.\n" ) );
    else if( Count == 0 )
        return 0;           ///< No data. Select time out.
    #ifdef _WIN32
    else if( FFD == (Int32)FReadSet.fd_array[ 0 ] )
    #else
    else if( FD_ISSET( FFD, &FReadSet ) )
    #endif
    {   ///< Data arrived!
        return RecvUDPData( OutDataStream );
    }
    return 0;
}
//------------------------------------------------------------------------------
Int32 PUDPClient::SetSendBufferSize( Int32 Size )
{
    if( FIsBind == TRUE )
        FSendBufferSize = SetBufferSize( SO_SNDBUF, Size );
    else
        FSendBufferSize = Size;
    return FSendBufferSize;
}
//------------------------------------------------------------------------------
Int32 PUDPClient::SetRecvBufferSize( Int32 Size )
{
    if( Size != FRecvBufferSize )
    {
        FRecvStream.SetSize( Size );
        if( FIsBind == TRUE )            
            FRecvBufferSize = SetBufferSize( SO_RCVBUF, Size );
        else
            FRecvBufferSize = Size;
    }
    return FRecvBufferSize;
}
//------------------------------------------------------------------------------
void PUDPClient::SetMulticastTTL( Int32 TTL )
{
    unsigned char ttl = TTL;
    
    SetOption( IPPROTO_IP, IP_MULTICAST_TTL,(char *)&ttl, sizeof(ttl));
}
//------------------------------------------------------------------------------
void PUDPClient::SetMulticastAddress( const AnsiString& Address, Int32 LoopBack )
{
#ifdef _WIN32
    SetOption( IPPROTO_IP, IP_MULTICAST_LOOPBACK, &LoopBack, sizeof( Int32 ));
#else
    UInt8 lb = LoopBack;
    SetOption( IPPROTO_IP, IP_MULTICAST_LOOP, &lb, sizeof( UInt8 ));
    if( FAddress.Length() != 0 && FAddress != "any" )
    {
        struct in_addr addr;
        addr.s_addr = inet_addr( FAddress.c_str() );
        SetOption( IPPROTO_IP, IP_MULTICAST_IF, &addr, sizeof(addr) );
    }
#endif
    
    DropMulticastAddress( FMulticastAddress );
    //translate address first
    struct ip_mreq  Mreq;
    
    Mreq.imr_multiaddr.s_addr = inet_addr( Address.c_str() );
    if( FAddress.Length() == 0 || FAddress == "any" )
        Mreq.imr_interface.s_addr = htonl( INADDR_ANY );
    else
        Mreq.imr_interface.s_addr = inet_addr( FAddress.c_str() );    
#ifdef _WIN32
    SetOption( IPPROTO_IP, 12, (char*)&Mreq, sizeof( struct ip_mreq ));
#else
    SetOption( IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&Mreq, sizeof( struct ip_mreq ));
#endif
    FMulticastAddress = Address;
}
//------------------------------------------------------------------------------
void PUDPClient::DropMulticastAddress( const AnsiString& Address )
{
    if( Address.Length() > 0 )
    {
        struct ip_mreq  Mreq;
        Mreq.imr_multiaddr.s_addr = inet_addr( Address.c_str() );
        Mreq.imr_interface.s_addr = htonl( INADDR_ANY );
    #ifdef _WIN32
        SetOption( IPPROTO_IP, 13, (char*)&Mreq, sizeof( struct ip_mreq ));
    #else
        SetOption( IPPROTO_IP, IP_DROP_MEMBERSHIP, &Mreq, sizeof( struct ip_mreq ));
    #endif
    }
}
//------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

