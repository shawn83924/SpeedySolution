//-----------------------------------------------------------------------------------------------------------------------
#ifndef _TCP_ONLY
//-----------------------------------------------------------------------------------------------------------------------
#include "X25Connection.h"
#include "../UFC/NetUtility.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor
// to construct a SocketConnection instance.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
X25Connection::X25Connection( )
:Connection(),
 FX25FD( -1 )
{    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
X25Connection::~X25Connection( )
{
    Close();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void X25Connection::CreateSocket( void )
{
    FX25FD = socket( AF_X25, SOCK_SEQPACKET, 0);
    if( FX25FD < 0 )
        throw( X25Exception( FX25FD, "Open X.25 socket failed." ) );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void X25Connection::SelectPort( int Port )
{
    struct fsk_port_info port;
    int retval;

    port.adapter = Port;
    port.line = 0;
    retval = ioctl( FX25FD, SIOCX25SADAPTER, &port);
    if (retval == -1)
    {
        UFC::AnsiString ErrMsg;
        ErrMsg.Printf( "Select Adapter[%d] failed.",Port );
        throw( X25Exception( FX25FD, (char*)ErrMsg.c_str() ) );    
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void X25Connection::AttachPVC( int PVC )
{
    struct sockaddr_x25 sa;
    char str[16];

    sa.sx25_family = AF_X25;
    /*
    * To select a PVC channel set the NUA in the connect to "+<channel_number>"
    * for example +1
    */
    sprintf( str, "+%d", PVC );
    strcpy( sa.sx25_addr.x25_addr, str );
    if( (connect( FX25FD, (struct sockaddr *)&sa, sizeof(sa))) == -1)
        throw( X25Exception( PVC_TIMEOUT, "Attach to PVC failed." ) );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void X25Connection::Open( int LineNo, int Pvc, char*)
{
    FPort = LineNo;
    FPVC  = Pvc;
    UFC::BufferedLog::Printf( " Open PVC Link:%d PVC:%d",LineNo, Pvc );    
    CreateSocket( );
    SelectPort( LineNo );
    AttachPVC( Pvc );
    UFC::BufferedLog::Printf( " Connected Link:%d PVC:%d",LineNo, Pvc );        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void X25Connection::Close( void )
{
    close( FX25FD );
    FX25FD = -1;
}
//-----------------------------------------------------------------------------------------------------------------------
// Any data in socket read buffer?
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL X25Connection::ReadyToRead( long TimeOut, long us )
{   
    Int32         PollCount;
    struct pollfd PollFD;    
    Int32         BeginTime = UFC::GetTickCountMS();
    Int32         WaitTime  = TimeOut*1000 + us/1000;;
    Int32         UseTime;  
    
    while( WaitTime > 0 ) ///< Still need to wait.
    {
        PollFD.fd     = FX25FD;
        PollFD.events = POLLIN;                
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
            throw( X25Exception( PollCount, " call poll failed.(Read)" ) );
    }
    return FALSE;    
}
//-----------------------------------------------------------------------------------------------------------------------
// TCP write buffer full?
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL X25Connection::ReadyToWrite( long TimeOut, long us )
{   
    Int32         PollCount;
    struct pollfd PollFD;    
    Int32         BeginTime = UFC::GetTickCountMS();
    Int32         WaitTime  = TimeOut*1000 + us/1000;;
    Int32         UseTime;  
    
    UFC::BufferedLog::Printf( " Enter Ready to write PVC:%d", FPVC );            
    while( WaitTime > 0 ) ///< Still need to wait.
    {
        PollFD.fd     = FX25FD;
        PollFD.events = POLLOUT;                        
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
            throw( X25Exception( PollCount, " call poll failed.(Write)" ) );
    }
    return FALSE;    
}
//------------------------------------------------------------------------------
Int32 X25Connection::SendBuffer( const Int8* SendData, Int32 Size )
{
    while( TRUE )
    {
        Int32 SendSize = send( FX25FD, SendData, Size, 0  );
        if( SendSize < 0 )
        {
            if( errno == EINTR )
            {
                UFC::BufferedLog::DebugPrintf("####### Interrupt ###########");
                continue;
            }
            else if( errno == EPIPE )
                throw( X25Exception( EPIPE, "Send data to a broken Pipe." ) );
            else
                throw( X25Exception( errno, "Send data failed." ));
        }
        return SendSize;
    }
}
//---------------------------------------------------------------------------
Int32 X25Connection::ReceiveBuffer( char* RecvData, Int32 Size )
{
    while( TRUE )
    {
        Int32 RecvSize = recv( FX25FD, (char*)RecvData, Size, 0 );
        if( RecvSize < 0 )
        {
            if( errno == EINTR )
            {
                UFC::BufferedLog::DebugPrintf("####### Interrupt #########");
                continue;
            }
            else if( errno == EPIPE )
                throw( X25Exception( EPIPE, "Receive data from a broken Pipe." )  );
            else
                throw( X25Exception( errno, "Recv data failed." ) );
        }
        else if( RecvSize == 0 )
            throw( X25Exception( 0, "the connection has been gracefully closed") );
        return RecvSize;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int X25Connection::Send( char* Data, int Length, long TimeOutSec )
{
    if( ReadyToWrite( TimeOutSec ) == TRUE )
        return SendBuffer( Data, Length );    
    else
    {
        X25Exception( -16, "Call X.25 Send function timeout." );    
        return PVC_TIMEOUT;  ///< time out error    
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int X25Connection::Receive( char* Data, int Length, long TimeOutSec )
{
    if( ReadyToRead( TimeOutSec ) == TRUE )
        return ReceiveBuffer( Data, Length );
    else
        return PVC_TIMEOUT;  ///< time out error    
}
//-----------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

