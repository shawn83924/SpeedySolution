#include "TFXSocketConnection.h"
#include "../UFC/NetUtility.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int  SocketConnection::FSocketBufferSize = TMP_MAX_DATA_SIZE;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor
// to construct a SocketConnection instance.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SocketConnection::SocketConnection( )
:Connection()
,FSocket( SOCK_STREAM, BLOCKING_SOCKET )
,FIsExchange( FALSE )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SocketConnection::~SocketConnection( )
{
    Close();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Close socket.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::SocketClose( BOOL ThrowException )
{
    try 
    {
        UFC::PLockObject Lock( FSocketIOLock ); ///< Can not close socket while reading.
                
        FSocket.CloseSocket();
        FIsOpen = FALSE;
    }
    catch( UFC::SocketException &e ) 
    {
        UFC::BufferedLog::Printf( " Disconnect failed:%s", e.what());
        if( ThrowException == TRUE )
            throw( e );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
// Any data in socket read buffer?
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SocketConnection::ReadyToRead( long TimeOut, long us )
{   
#ifndef  _WIN32
    Int32         PollCount;
    struct pollfd PollFD;    
    Int32         BeginTime = UFC::GetTickCountMS();

    Int32         WaitTime  = TimeOut*1000 + us/1000;;
    Int32         UseTime;  
    
    while( WaitTime > 0 ) ///< Still need to wait.
    {
        PollFD.fd     = FSocket.GetHandle( );
        PollFD.events = POLLRDNORM;                        
        if( (PollCount = poll( &PollFD,1, WaitTime )) == 0 ) ///< Time is up or interrupt by signal.
        {
            UseTime = UFC::GetTickCountMS() - BeginTime;
            BeginTime += UseTime;
            WaitTime  -= UseTime;
            if( WaitTime <= 0 )
                return FALSE; ///< time is up.                            
        }
        else if( PollCount > 0 )
        {
            return TRUE; ///< Ready to read.
        }
        else
        {
            if( errno == EINTR )
            {
                UFC::BufferedLog::DebugPrintf("### poll Interrupt by signal (read) ###");
                continue;
            }
            else if( errno == EAGAIN )
                continue;
            throw( X25Exception( errno, " call poll failed.(Read)" ) );
        }
    }
#endif
    return FALSE;    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Functions to implement Connection interface
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Open socket for client side. (Broker side)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::Open( int LocalPort, int /*PVCID*/, char* URL )
{
    UFC::PStringList Strs;
    UFC::AnsiString  IPAddress( URL );
    UInt16           PeerPort;
    
    Strs.SetStrings( URL, ":\n" );
    if( Strs.ItemCount() != 2  )
        throw( X25Exception( -1, "Invalidate port number or URL." ) );
    IPAddress = Strs[0];
    PeerPort  = Strs[1].ToInt(); 
    if( LocalPort <= 0 || IPAddress.Length() == 0 || PeerPort == 0 )
        throw( X25Exception( -1, "Invalidate port number or IP address." ) );
    FSocket.CloseSocket();
    UFC::BufferedLog::Printf(" Open URL[%s:%d] LocalPort[%d]", IPAddress.c_str(),PeerPort, LocalPort );
    while( FIsOpen == FALSE  )
    {
    	if( FSocket.OpenSocket() > 0 ) ///< Open socket fail will throw an exception.
        {
            FSocket.SetReuse( 1 );
            FSocket.SetLinger( TRUE, 0 ); ///< Turn On the Linger,Set timeout to 0 sec.
            FSocket.SetTCPNoDelay( TRUE );///< Turn On the TCP no delay function.
            FSocket.SetBlockMode( FALSE );
            ///< FSocket.Connect will throw SocketException if connection failed.
            FSocket.Connect( IPAddress, PeerPort, 5, LocalPort  );
            UFC::BufferedLog::Printf(" URL[%s:%d] Port[%d] Connected.", IPAddress.c_str(),PeerPort ,LocalPort );
            FSocket.SetBufferSize( SO_RCVBUF, FSocketBufferSize );
            FSocket.SetBufferSize( SO_SNDBUF, FSocketBufferSize );
            FIsOpen = TRUE;            
            FIsExchange = FALSE; ///< Broker side
            return;            
        }
        sleep( 5 );
    };
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Open socket for server side. (Exchange side)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::Open( int FD )
{
    FSocket.Attach( FD );    
    try
    {
        FSocket.SetLinger( TRUE, 1 );///< Turn On the Linger,Set timeout to 1 sec.
        FSocket.SetTCPNoDelay( TRUE ); ///< Turn On the TCP no delay function.
        FSocket.SetBlockMode( FALSE );
        FSocket.SetBufferSize( SO_RCVBUF, FSocketBufferSize );
        FSocket.SetBufferSize( SO_SNDBUF, FSocketBufferSize );                          
        FIsOpen = TRUE;
        FIsExchange = TRUE; ///< Exchange side
    }
    catch( UFC::SocketException& )
    {
        SocketClose( FALSE ); ///< Close socket without throw exception.
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Close socket. (No exception thrown)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::Close( void )
{
    SocketClose( FALSE ); ///< Close socket without throw exception.
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Implement function Send for Connection interface
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SocketConnection::Send( char* Data, int Length, long TimeOutSec )
{
    int Rtn;
    
    if( FIsOpen == FALSE )
        X25Exception( -13, "Send to a broken Socket Pipe." );    
    //UFC::PLockObject Lock( FSocket.IOLock );
    Rtn = FSocket.SendBuffer( Data, Length );    
    return Rtn;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Implement function Receive for Connection interface
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SocketConnection::Receive( char* Data, int Length, long TimeOutSec  )
{    
    if( Length < TMP_HEADER_FULL_SIZE )
        throw( X25Exception( -17, "Call TMP Receive function with buffer size less then TMP header size." ) );

    if( ReadyToRead( TimeOutSec ) == TRUE )
    {        
        ///< Get the Message header.
        FSocket.BlockRecv( (UInt8*)Data, TMP_HEADER_FULL_SIZE ); 
        ///< Get the packet length and the control code.
        Int16   DataLen  = UFC::NInt16:: ToInt16( (UInt8*)Data );
        ///< Check whether amessage size exceed buffer length.
        if( DataLen + 3 > Length ) ///< DataLen + 3 = TMP message size.
        {   ///< CheckSum + msg_length = 3
            UFC::AnsiString ErrMsg;
                    
            ErrMsg.Printf( "TMP message size too large, Buffer size[%d] message size[%d].",Length ,DataLen + 3 );
            throw( X25Exception( -15, (char*)ErrMsg.c_str() ) );
        }        
        ///< Get the Message Body.
        FSocket.BlockRecv( (UInt8*)( Data + TMP_HEADER_FULL_SIZE), DataLen + 3 - TMP_HEADER_FULL_SIZE );
        return DataLen +3;
    }
    else
        return PVC_TIMEOUT;  ///< time out error
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::SetTMPBufferSize( int Size )
{
    if( Size < TMP_MAX_DATA_SIZE ) 
        FSocketBufferSize = TMP_MAX_DATA_SIZE;
    else
        FSocketBufferSize = Size;
}    
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SocketConnection::GetTMPBufferSize( void )
{
    return FSocketBufferSize;
}    
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
