#include "SocketConnection.h"
#include "../UFC/NetUtility.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int  SocketConnection::FSocketBufferSize = TWSE_MAX_DATA_SIZE;    
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SocketConnection::SocketConnection( )
:Connection()
,PThread( NULL, TRUE )
,FReadQueue( 512 )
,FSocket( SOCK_STREAM, BLOCKING_SOCKET )
,FSendHeartbeat( 0 )
,FRecvHeartbeat( 0 )
,FIsExchange( FALSE )
{
    FSendBuffer = new Int8[ TWSE_MAX_DATA_SIZE ]; ///< Alloc data buffer.(TSEC SPEC. 8K)
    Start(); ///< Start the thread.
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SocketConnection::~SocketConnection( )
{
    delete [] FSendBuffer;
    Close();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Close socket.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::SocketClose( BOOL ThrowException )
{
    FRecvHeartbeat = 0;
    FSendHeartbeat = 0;
    if( FIsOpen == TRUE )
    {
        try
        {
            FIsOpen = FALSE;            
            UFC::PLockObject Lock( FSocketIOLock ); ///< Can not close socket while reading.        
            
            FReadQueue.Clear( TRUE );
            FSocket.CloseSocket();
        }
        catch( UFC::SocketException &e )
        {
            UFC::BufferedLog::Printf( " Disconnect failed:%s",e.what());
            if( ThrowException == TRUE )
                throw( e );
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Implement the Execute function for PThread
//
// 1. Read data from Socket
// 2. Add the TMP message to queue if read successfully.
// 3. Count down the heartbeat counter if read failed.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::Execute( void )
{
    while( TRUE )
    {
       if( FIsOpen == TRUE )
       {
           try
           {
               if( ReadData( 1 ) == PVC_TIMEOUT )  ///< Try to receive data from TSEC
               {
                   FSendHeartbeat++;
                   FRecvHeartbeat++;
               }
               if( FIsExchange == FALSE )///< Broker Side
               {
                   if( FSendHeartbeat >= (60-2) ) ///< Time is up! 60 sec
                       SendHeartbeat();        ///< Send the heartbeat.
                   if( FRecvHeartbeat >= (25+5) ) ///< Exchange Timeout!! (TSEC SPEC. 25 sec)
                   {
                       UFC::BufferedLog::Printf( " Recv Heartbeat Timeout" );
                       SocketClose( TRUE );                       
                   }
               }
               else
               {
                   if( FSendHeartbeat >= (25-2) ) ///< Time is up! 20 sec
                       SendHeartbeat();        ///< Send the heartbeat.
                   if( FRecvHeartbeat >= (60+12) ) ///< Broker Timeout!!
                   {
                       UFC::BufferedLog::Printf( " Recv Heartbeat Timeout" );
                       SocketClose( TRUE );                       
                   }
               }
           }
           catch( UFC::Exception &e )
           {
               SocketClose( FALSE );
	           UFC::BufferedLog::Printf( " Socket Exception:%s",e.what());
           }
           catch( ... )
           {
               SocketClose( FALSE );
	           UFC::BufferedLog::Printf( " Unknown exception" );
           }
       }
       else
       	   sleep( 1 );
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Send accept connection ack message.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::SendAcceptAck( void )
{
    UInt8 AcceptAck[ 8 ];

    AcceptAck[0] = 0xFE;  ///< Header code.
    AcceptAck[1] = 0xFE;
    AcceptAck[2] = '1';   ///< "10" means accept connection message
    AcceptAck[3] = '0';
    AcceptAck[4] = 0;     ///< Data length = 0
    AcceptAck[5] = 0;
    AcceptAck[6] = 0xEF;  ///< Trailer
    AcceptAck[7] = 0xEF;    
    UFC::PLockObject Lock( FSocket.IOLock );
    
    FSendHeartbeat = 0;    ///< Reset the send counter.    
    FSocket.SendBuffer( (const Int8*)AcceptAck, 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Send heartbeat message
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::SendHeartbeat( void )
{
    UInt8 Heartbeat[ 8 ];

    Heartbeat[0] = 0xFE;  ///< Header code.
    Heartbeat[1] = 0xFE;
    Heartbeat[2] = '1';   ///< "11" means heartbeat  message
    Heartbeat[3] = '1';
    Heartbeat[4] = 0;     ///< Data length = 0
    Heartbeat[5] = 0;
    Heartbeat[6] = 0xEF;  ///< Trailer
    Heartbeat[7] = 0xEF;    
    UFC::PLockObject Lock( FSocket.IOLock );
    
    FSendHeartbeat = 0;    ///< Reset the send counter.        
    FSocket.SendBuffer( (const Int8*)Heartbeat, 8 );    
}
//-----------------------------------------------------------------------------------------------------------------------
// Any data in socket read buffer?
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SocketConnection::ReadyToRead( long TimeOut, long us )
{   
#ifdef _WIN32
	Int32                 SelectCount;    
	fd_set		 ReadSet;
        struct timeval SelectTime;

	SelectTime.tv_sec  = TimeOut;
        SelectTime.tv_usec = us;
	FD_ZERO( &ReadSet );   ///< Reset the read set.
	FD_SET( FSocket.GetHandle( ), & ReadSet );
        SelectCount = select( FSocket.GetHandle( ), &ReadSet, NULL, NULL, &SelectTime );
	if( SelectCount < 0 )       ///< Call system call "select" failed.
	{
	        Int32 ErrNO = WSAGetLastError();
		UFC::BufferedLog::Printf( "   Call Select failed. Errno: %d.", ErrNO   );
		throw( X25Exception(  ErrNO, " call select failed." ) );
	}
	else if( SelectCount == 0 )  ///< No data comming.	
		return FALSE; ///< time is up.                            
	else
		return TRUE; ///< Ready to read.
#else
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
            return TRUE; ///< Ready to read.
        else
            throw( X25Exception( PollCount, " call poll failed." ) );
    }
    return FALSE;    
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Receive TMP Message and add it to data queue.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SocketConnection::ProcessTMPMessage( SocketData* Data, Int16 DataLength  )
{
    if( DataLength + 2 > MAX_TCP_PACKET )
    {
        delete Data;
        throw( X25Exception( -13, "TCP Message size too large." ) );
    }
    ///< Receive message body and trailer.
    FSocket.BlockRecv( Data->RowData, DataLength + 2 );    
    ///< Check the message trailer.
    if( Data->RowData[ DataLength ] != 0xEF || Data->RowData[ DataLength + 1 ]!= 0xEF )
    {
        delete Data;
        throw( X25Exception( -14, "Invalidate TMP message trailer." ) );
    }
    Data->Length = DataLength;
    FReadQueue.Inqueue( Data ); ///< Add TMP Message to queue.
    return DataLength;   ///< Received okay
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Handle accept connection message.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SocketConnection::ProcessAcceptMessage( SocketData* Data  )
{
    UInt8 TCPTrailer[ 2 ];
    ///< Receive message trailer.    
    FSocket.BlockRecv( TCPTrailer, 2 );
    ///< Validate the trailer.
    if( TCPTrailer[ 0 ] != 0xEF || TCPTrailer[ 1 ]!= 0xEF )
    {
        delete Data;
        throw( X25Exception( -15, "Invalidate Accept message trailer." ) );
    }
    UFC::BufferedLog::Printf( " Recv accept connection message" );
    Data->Length = TCP_ACCEPT;
    FReadQueue.Inqueue( Data ); ///< add accept message to read queue.
    return TCP_ACCEPT;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Handle heartbeat message.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SocketConnection::ProcessHeartbeat( SocketData* Data  )
{
    UInt8 TCPTrailer[ 2 ];

    delete Data;
    ///< Receive message trailer.
    FSocket.BlockRecv( TCPTrailer, 2 ); 
    ///< Validate the trailer.
    if( TCPTrailer[ 0 ] != 0xEF || TCPTrailer[ 1 ]!= 0xEF )
        throw( X25Exception( -16, "Invalidate Heartbeat message trailer." ) );    
    return TCP_HEARTBEAT;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SocketConnection::ReadData( long TimeOutSec  )
{
    UInt8 TCPHeader[ 6 ];    
    UFC::PLockObject Lock( FSocketIOLock );
    
    if( ReadyToRead( TimeOutSec ) == TRUE )
    {        
        ///< Get the Message header.
        FSocket.BlockRecv( TCPHeader, 6 ); 
        ///< Get the packet length and the control code.
        SocketData*     Data        = new SocketData();        
        Int16           DataLen     = UFC::NInt16:: ToInt16( TCPHeader + 4 );
        Int16           ControlCode = UFC::AnsiString( (char*)TCPHeader + 2, 2 ).ToInt();

        if( TCPHeader[ 0 ]!= 0xFE || TCPHeader[ 1 ]!= 0xFE ) ///< Validate the header.
        {
            delete Data;
            throw( X25Exception( ControlCode, "Invalidate message header." ) );
        }
        FRecvHeartbeat = 0;
        switch( ControlCode )
        {
            case 0:  ///< TMP Message
                     return ProcessTMPMessage( Data, DataLen );
            case 10: ///< Accept connection message
                     return ProcessAcceptMessage( Data );
            case 11: ///< Heartbeat
                     return ProcessHeartbeat( Data );
            default: delete Data;
                     throw( X25Exception( ControlCode, "Receive invalid TSEC message." ));
        }
    }
    else
        return PVC_TIMEOUT;  ///< time out error
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Functions to implement Connection interface
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Open socket for client side. (Broker side)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::Open( int Port, int PVCID, char* IP, int LocalPort  )
{
    UFC::AnsiString IPAddress( IP );
    
    if( Port <= 0 || IPAddress.Length() == 0 )
        throw( X25Exception( -1, "Invalidate port number or IP address." ) );
    FReadQueue.Clear( TRUE );
    FSocket.CloseSocket();
    UFC::BufferedLog::Printf( " Open PVC:%02d [localhost:%d]->[%s:%d]",PVCID, LocalPort, IPAddress.c_str(), Port );
    while( FIsOpen == FALSE  )
    {
    	if( FSocket.OpenSocket() > 0 ) ///< Open socket fail will throw an exception.
        {
            FSocket.SetReuse( 1 );
            FSocket.SetLinger( TRUE, 0 );///< Turn On the Linger,Set timeout to 1 sec.                        
            ///< FSocket.Connect will throw SocketException if connection failed.
            FSocket.Connect( IPAddress, Port, 10, LocalPort );
            UFC::BufferedLog::Printf(" [localhost:%d]->[%s:%d] Connected.",LocalPort, IPAddress.c_str(), Port );
            FSocket.SetBufferSize( SO_RCVBUF, FSocketBufferSize );
            FSocket.SetBufferSize( SO_SNDBUF, FSocketBufferSize );
            FIsOpen = TRUE;            
            try
            {
                if( Receive( NULL, 0, 5 ) == TCP_ACCEPT )
                {                    
                    UFC::BufferedLog::Printf(" TCP connection PVC:%d connected.",PVCID );
                    FIsExchange = FALSE; ///< Broker side
                    return;
                }
                else
                {                    
                    SocketClose( FALSE ); ///< Close socket without throw exception.
                    UFC::BufferedLog::Printf(" TCP PVC:%d receive connection accepted message failed.", PVCID );
                }
            }
            catch( UFC::SocketException & )
            {
                SocketClose( FALSE ); ///< Close socket without throw exception.
                UFC::BufferedLog::Printf(" TCP PVC:%d receive connection accepted message failed.", PVCID );
            }
        }
        sleep( 5 );
    };
    FRecvHeartbeat = 0;
    FSendHeartbeat = 0;    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Open socket for server side. (Exchange side)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::Open( int FD )
{
   
    FReadQueue.Clear( TRUE );           
    FSocket.Attach( FD );    
    try
    {
        FSocket.SetBufferSize( SO_RCVBUF, FSocketBufferSize );
        FSocket.SetBufferSize( SO_SNDBUF, FSocketBufferSize );          
        SendAcceptAck();
        FIsExchange = TRUE; ///< Exchange side
        FSocket.SetLinger( TRUE, 1 );///< Turn On the Linger,Set timeout to 1 sec.
        FIsOpen = TRUE;
    }
    catch( UFC::SocketException & )
    {
        SocketClose( FALSE ); ///< Close socket without throw exception.
    }
    FRecvHeartbeat = 0;
    FSendHeartbeat = 0;        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Close socket. (No exception thrown)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::Close( void )
{
    SocketClose( FALSE ); ///< Close socket without throw exception.
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Inplement function Send for Connection interface
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SocketConnection::Send( char* Data, int Length, long TimeOutSec )
{
    if( FIsOpen == FALSE )
	    X25Exception( -13, "Send to a broken Socket Pipe." );    
    FSendBuffer[ 0 ] = (char)0xFE;  ///< Header code.
    FSendBuffer[ 1 ] = (char)0xFE;
    FSendBuffer[ 2 ] = '0';    ///< "00" means TMP message
    FSendBuffer[ 3 ] = '0';
    Int16 Len16 = Length;
    UFC::NInt16::ToInt16( Len16, (UInt8*)FSendBuffer +4 ); ///< Set Data length
    memcpy( FSendBuffer + 6, Data, Length );   ///< Copy the Data
    FSendBuffer[ Length + 6 ] = (char)0xEF;          ///< Trailer
    FSendBuffer[ Length + 7 ] = (char)0xEF;
    
    UFC::PLockObject Lock( FSocket.IOLock );
    
    FSendHeartbeat = 0;    ///< Reset recv heartbeat count
    return FSocket.SendBuffer( FSendBuffer, Length + 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Implement function Receive for Connection interface
//
//  (Note: This function get TMP message from data queue.
//         Not real socket I/O. Don't need to reset recv heartbeat counter. )
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SocketConnection::Receive( char* Data, int Length, long TimeOutSec  )
{
    SocketData* Received;

    for( int i = 0; i < TimeOutSec; i ++ )
    {
        if( ( Received = FReadQueue.Dequeue( 1 ) ) != NULL )
        {
            if( FIsOpen == FALSE )
            {
                UFC::BufferedLog::Printf( " SocketConnection.Receive() Socket broken." );
                throw( X25Exception( -14, "Receive data from a broken Socket Pipe." ));
            }
            if( Received->Length == TCP_ACCEPT ) ///< Is a accept message
            {
                delete Received;
                return TCP_ACCEPT;
            }
            else ///< Is a TMP message
            {
                if( Received->Length > Length ) ///< Check the buffer size is enough.
                {
                    char ErrMsg[256];
                    
                    printf( "TCP data size too large, Buffer size[%d] Data size[%d].",Length,Received->Length );
                    delete Received;
                    throw( X25Exception( -15, ErrMsg ) );
                }
                memcpy( Data, Received->RowData, Received->Length );
                int Rtn = Received->Length;
                delete Received;
                return Rtn;
            }
        }
        else
        {
            if( FIsOpen == FALSE )
            {
                UFC::BufferedLog::Printf( " SocketConnection.Receive() Socket broken." );
                throw( X25Exception( -14, "Receive data from a broken Socket Pipe." ) );
            }
        }
    }    
    return PVC_TIMEOUT;  ///< time out error
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SocketConnection::SetTMPBufferSize( int Size )
{
    if( Size < TWSE_MAX_DATA_SIZE ) 
        FSocketBufferSize = TWSE_MAX_DATA_SIZE;
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
