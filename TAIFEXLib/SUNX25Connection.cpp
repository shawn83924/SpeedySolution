//-----------------------------------------------------------------------------------------------------------------------
#ifndef _TCP_ONLY
//-----------------------------------------------------------------------------------------------------------------------
#include "X25Connection.h"
//-----------------------------------------------------------------------------------------------------------------------
#ifdef __SOLARIS  ///< For Solaris
//-----------------------------------------------------------------------------------------------------------------------
X25Connection::X25Connection( )
:Connection(),
 FX25FD( -1 )
{
    FDataBuffer = new char[ PVC_MAX_PACKET_SIZE ];
    FCtrlBlock  = new char[ PVC_MAX_CONTROLBUFFER ];
}
//-----------------------------------------------------------------------------------------------------------------------
X25Connection::~X25Connection()
{
    Close();
    delete [] FDataBuffer;
    delete [] FCtrlBlock;
}
//-----------------------------------------------------------------------------------------------------------------------
BOOL X25Connection::ReadyToWrite( long TimeOut, long us )
{
    Int32 BeginTime = UFC::GetTickCountMS();
    Int32 WaitTime  = TimeOut*1000 + us/1000;;
    Int32 TimeNow, UseTime, rtn;  
    
    while( WaitTime > 0  )
    {
        FTimeout.tv_sec  = WaitTime/1000;
        FTimeout.tv_usec = (WaitTime%1000)*1000;    
        FD_ZERO( &FPutfd );
        FD_SET( FX25FD, &FPutfd );
        if( (rtn = select( FX25FD+1, NULL, &FPutfd, NULL, &FTimeout)) == 0 ) ///< Timeout or interrupt by signal
        {
            TimeNow = UFC::GetTickCountMS();
            UseTime = TimeNow - BeginTime;
            BeginTime = TimeNow;
            WaitTime -= UseTime;
            if( WaitTime <= 0 )
                return FALSE; ///< time is up.                
        }
        else if( rtn > 0 )
        {
            if( FD_ISSET( FX25FD, &FPutfd ) )
                return TRUE; ///< OK!
            TimeNow = UFC::GetTickCountMS();
            UseTime = TimeNow - BeginTime;
            BeginTime = TimeNow;
            WaitTime -= UseTime;
            if( WaitTime <= 0 )
                return FALSE; ///< time is up.
        }
        else
        {
            UFC::BufferedLog::Printf(" Link:%d PVC:%d ReadyToWrite select return error code:%d.", FPort,FPVC,rtn );
            return FALSE; ///< Error
        }
    }
    return FALSE;
}
//-----------------------------------------------------------------------------------------------------------------------
BOOL X25Connection::ReadyToRead( long TimeOut, long us )
{
    Int32 BeginTime = UFC::GetTickCountMS();
    Int32 WaitTime  = TimeOut*1000 + us/1000;;
    Int32 TimeNow, UseTime, rtn;  
    
    while( WaitTime > 0  )
    {
        FTimeout.tv_sec  = WaitTime/1000;
        FTimeout.tv_usec = (WaitTime%1000)*1000;
        FD_ZERO( &FGetfd );
        FD_SET( FX25FD, &FGetfd );        
        if( (rtn = select( FX25FD+1, &FGetfd, NULL, NULL, &FTimeout )) == 0 ) ///< Timeout or interrupt by signal
        {
            TimeNow = UFC::GetTickCountMS();
            UseTime = TimeNow - BeginTime;
            BeginTime = TimeNow;
            WaitTime -= UseTime;
            if( WaitTime <= 0 )
                return FALSE; ///< time is up.                
        }
        else if( rtn > 0 )
        {
            if( FD_ISSET( FX25FD, &FGetfd ) )
                return TRUE; ///< OK!
            TimeNow = UFC::GetTickCountMS();
            UseTime = TimeNow - BeginTime;
            BeginTime = TimeNow;
            WaitTime -= UseTime;
            if( WaitTime <= 0 )
                return FALSE; ///< time is up.
        }
        else
        {
            UFC::BufferedLog::Printf(" Link:%d PVC:%d ReadyToRead select return error code:%d.", FPort,FPVC,rtn );
            return FALSE; ///< Error
        }
    }
    return FALSE;
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::WritPVCData( struct strbuf *Control, struct strbuf *Data, long timeout )
{
    int RtnCode;
    
    if( ReadyToWrite( timeout ) == TRUE )
    {
        if( (RtnCode = putmsg( FX25FD, Control ,Data, 0 )) == 0 ) // Call x.25 system call "putmsg"
            return PVC_OK;    // Send okay
        else
            throw( X25Exception( RtnCode, "Send data failed.") ); // send data error.
    }
    else
        throw( X25Exception( PVC_TIMEOUT, "Send data timeout.") );   // time out error
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::ReadPVCData( struct strbuf *Control, struct strbuf *Data, long timeout )
{
    int Flags = 0;
    int rtn;

    if( ReadyToRead( timeout ) == TRUE )
    {
        rtn = getmsg( FX25FD, Control ,Data, &Flags );
        if( rtn == MOREDATA )
        {
            UFC::BufferedLog::Printf(" Link:%d PVC:%d getmsg return:Has more data.", FPort,FPVC );
            throw( X25Exception( rtn, "Has more data." ) );
        }
        else if( rtn == MORECTL )
        {
            UFC::BufferedLog::Printf(" Link:%d PVC:%d getmsg return:Has more control data.", FPort,FPVC );
            throw( X25Exception( rtn, "Has more control data." ) );
        }
        else if( rtn != 0 ) // Call x.25 system call "getmsg"
        {
            UFC::BufferedLog::Printf(" Link:%d PVC:%d getmsg return error code:%d.", FPort,FPVC,rtn );
            throw( X25Exception( rtn, "Receive data failed." ) ); // Receive data error.
        }
        else
            return PVC_OK;    // Received okay
    }
    else
        return PVC_TIMEOUT;  // time out error
}
//-----------------------------------------------------------------------------------------------------------------------
void X25Connection::OpenStrem()
{
    if( FX25FD != -1 )
        close( FX25FD );
    FX25FD = open( "/dev/x25", O_RDWR |O_NDELAY );
    if( FX25FD < 0 )
        throw( X25Exception( FX25FD, "Open X.25 failed." ) );
}
//----------------------------------------------------------------------------------------------------------------------
void X25Connection::SendAttachMessage(  int Line, int PVC   )
{
    struct pvcattf attach;
    struct strbuf  ctlblk;
    int	   Result;

    memset( &attach, 0, sizeof( struct pvcattf ));
    attach.xl_type       = XL_CTL;
    attach.xl_command    = N_PVC_ATTACH;
    attach.lci           = PVC;            // PVC number
    attach.link_id       = Line;           // this is the link ID
    ctlblk.len           = sizeof( struct pvcattf );
    ctlblk.buf           = ( char* )&attach;
    // send attach message to stream
    Result = WritPVCData( &ctlblk, NULL, 5L);
    if( Result == PVC_TIMEOUT )
        throw( X25Exception( Result, "Send attach PVC message time out." ) ); // time out error
}
//----------------------------------------------------------------------------------------------------------------------
BOOL X25Connection::GetAttachRespsonseMessage( void )
{
    struct strbuf ctlblk,datblk;
    struct pvcattf ctlbuf;
    int    RetryCount = 0;

    ctlblk.maxlen = sizeof( struct pvcattf );
    ctlblk.buf    = (char*)&ctlbuf;
    datblk.maxlen = PVC_MAX_PACKET_SIZE;
    datblk.buf    = FDataBuffer;
    while( TRUE )
    {
        if( ReadPVCData( &ctlblk,&datblk,5L) == PVC_TIMEOUT )
            throw( X25Exception( PVC_TIMEOUT, "Get attach PVC message time out." ) ); // time out error
        if( ctlbuf.xl_type == XL_CTL && ctlbuf.xl_command == N_PVC_ATTACH )
        {
            switch( ctlbuf.result_code )
            {
                case PVC_SUCCESS: return TRUE;      // Attach to PVC successful!
                case PVC_BUSY:	  if( RetryCount++ < 10 )
                                      return FALSE; // PVC line is busy. Try again.
                                  else
                                      throw( X25Exception( ctlbuf.result_code, "PVC is busy." ) );
            default:		  throw( X25Exception( ctlbuf.result_code, "Attach to PVC failed." ) );
            }
        }
    };
}
//-----------------------------------------------------------------------------------------------------------------------
void X25Connection::Open( int LineNo, int Pvc, char* )
{
    FPVC = Pvc;
    FPort = LineNo;
    UFC::BufferedLog::Printf( " Open PVC Link:%d PVC:%d",LineNo, Pvc );
    while( TRUE )
    {	///< Open a X.25 stream.
        OpenStrem();
        ///< Send control message to attach PVC.
        SendAttachMessage( LineNo, Pvc );
        ///< get respsonse message for the attach from the streams driver
        if( GetAttachRespsonseMessage() == TRUE ) ///< Will throw X25Exception after retry 10 times.
        {
            UFC::BufferedLog::Printf(" Attach to PVC:%d succeed.",Pvc);
            FIsOpen = TRUE; ///< Set the open flag to TRUE.
            return;         ///< Open PVC succeed.
        }
        UFC::BufferedLog::Printf(" Attach to PVC:%d time out, close it and try again...",Pvc );
        sleep( 5 );
        Close(); ///< Attach to PVC time out, close it an try again....
    };
}
//-----------------------------------------------------------------------------------------------------------------------
void X25Connection::Close( void )
{
    if( FX25FD != -1 )
    {
        for( int i = 0;i < 10; i++ )
        {
            try
            {
                SendDetachMessage();
                GetDetachRespsonseMessage();
                close( FX25FD );
                break;
            }
            catch(...)
            {
                if( i == 9 )
                {
                    close( FX25FD );
                    UFC::BufferedLog::Printf(" Try to close PVC failed." );
                }
            }
        }
        FX25FD = -1;
    }
    FIsOpen = FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
int X25Connection::Send( char* Data, int Length, long TimeoutSec )
{
    struct strbuf ctl;
    struct strbuf dat;
    struct xdataf data;
    int    CurrentLength = Length;
    char*  CurrentData   = Data;
    int    Result;

    while( TRUE )
    {
        if(( Result = CheckPVCForWrite( ) ) != PVC_OK )
             return Result;
        memset( (void*)&data, 0, sizeof( struct xdataf ) );
        data.xl_type    = XL_DAT;
        data.xl_command = N_Data;
        ctl.len         = sizeof( struct xdataf );
        ctl.buf         = (char *)&data;
        dat.buf         = CurrentData;
        if( CurrentLength <= PVC_MAX_PACKET_SIZE )
        {
            dat.len   = CurrentLength;
            data.More = 0;
            return WritPVCData( &ctl,&dat,TimeoutSec );
        }
        else
        {
            dat.len   = PVC_MAX_PACKET_SIZE;
            data.More = 1;
            if( (Result = WritPVCData( &ctl,&dat,TimeoutSec ))!= PVC_OK )
                return Result; // Error occur
            CurrentLength -= PVC_MAX_PACKET_SIZE;
            CurrentData   += PVC_MAX_PACKET_SIZE;
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------
// Return Value
// >0               : Return received data length.
//  0 PVC_OK        : Ok
// -2 PVC_TIMEOUT   : Received data timeout.
// -3 PVC_DISCONNECT: Received PVC disconnected control command.
// -4 PVC_RESET     : Received PVC reset control command.
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::Receive( char* Data,int Length, long TimeoutSec )
{
    struct xdataf* xd = (struct xdataf *)FCtrlBlock;
    struct strbuf  ct,dt;
    int            ReceivedLength = 0;
    int 		   Result;

    do
    {
        memset( FCtrlBlock, 0, PVC_MAX_CONTROLBUFFER );
        ct.buf    = FCtrlBlock;
        ct.maxlen = PVC_MAX_CONTROLBUFFER;
        dt.buf    = Data + ReceivedLength;
        dt.maxlen = Length - ReceivedLength;
        if( (Result = CheckPVCAndRead( &ct, &dt, TimeoutSec )) != PVC_OK )
            return Result;
        if( dt.len > 0 )// -1 means no data received.
            ReceivedLength += dt.len;
        if( ReceivedLength == Length )
            break;
    }while( xd->More != 0 );
    return ReceivedLength; // return received length
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::SendResetConfirm( )
{
    struct strbuf   ctlbuf;
    struct xrscf    rset;

    memset( &rset, 0, sizeof( struct xrscf ) );
    rset.xl_command = N_RC;
    rset.xl_type    = XL_CTL;
    ctlbuf.len      = sizeof( struct xrscf );
    ctlbuf.buf      = (char*)&rset;
    // Send a RESET Confirm packet.
    WritPVCData( &ctlbuf, NULL, 5L);
    return PVC_RESET;
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::SendExpeditedDataAck( void )
{
    struct strbuf   ctlbuf;
    struct xedatacf edatac;

    memset( &edatac, 0, sizeof( struct xedatacf ) );
    edatac.xl_command = N_EAck;
    edatac.xl_type = XL_DAT;
    ctlbuf.len      = sizeof( struct xedatacf );
    ctlbuf.buf      = (char*)&edatac;
    // Send a Expedited Data Acknowledgement.
    return WritPVCData( &ctlbuf, NULL, 5L);
}
//----------------------------------------------------------------------------------------------------------------------
void X25Connection::SendDetachMessage( void )
{
    struct pvcdetf detach;
    struct strbuf  ctlblk;
    int	   Result;

    detach.xl_type     = XL_CTL;
    detach.xl_command  = N_PVC_DETACH;
    detach.reason_code = 0;
    ctlblk.len         = sizeof(struct pvcdetf);
    ctlblk.buf         = (char *)&detach;
    // send detach message to stream
    Result = WritPVCData( &ctlblk, NULL, 5L);
    if( Result == PVC_TIMEOUT )
        throw( X25Exception( PVC_TIMEOUT, "Send detach PVC message time out." ) ); // time out error
}
//----------------------------------------------------------------------------------------------------------------------
void X25Connection::GetDetachRespsonseMessage( void )
{
    struct strbuf ctlblk;
    struct strbuf datblk;
    struct pvcdetf *ind_msg;
    char   ctlbuf[sizeof(struct pvcdetf)],datbuf[128];
    int    getflags;

    // get respsonse message for the setach from the streams driver
    ctlblk.maxlen = sizeof(struct pvcdetf);
    ctlblk.buf    = ctlbuf;
    datblk.maxlen = 128;
    datblk.buf    = datbuf;
    getflags      = 0;
    while( TRUE )
    {
        if( ReadPVCData( &ctlblk,&datblk,5L) == PVC_TIMEOUT )
            throw( X25Exception( PVC_TIMEOUT, "Get detach PVC message time out." ) ); ///< time out error
        ind_msg = (struct pvcdetf *)(void*)ctlbuf;
        if( ind_msg->xl_type == XL_CTL && ind_msg->xl_command==N_PVC_DETACH )
            return;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::CheckPVCAndRead( struct strbuf *Control, struct strbuf *Data, long timeout  )
{
    if( ReadPVCData( Control, Data, timeout ) == PVC_OK )
        return CheckCTLCommand( (S_X25_HDR*) Control->buf );
    return PVC_TIMEOUT;													       ///< time out
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::CheckPVCForWrite( void )
{
    struct strbuf  ct;
    int            flag = 0;
    int            Rtn;

    memset( FCtrlBlock, 0, PVC_MAX_CONTROLBUFFER );
    ct.buf    = FCtrlBlock;
    ct.maxlen = PVC_MAX_CONTROLBUFFER;
    if( ReadyToRead( 0L,1000L ) == FALSE)              ///< Any data in buffer? Check 1 ms
        return PVC_OK;                 			///< Time out, no control packet
    if( (Rtn = getmsg( FX25FD, &ct, 0,&flag )) != 0 )
            throw( X25Exception( Rtn, "Send data failed.") );
    return CheckCTLCommand( (S_X25_HDR*) FCtrlBlock );
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::CheckCTLCommand( S_X25_HDR*  X25Result )
{
    if((X25Result->xl_type == XL_CTL)&&(X25Result->xl_command==N_DI))       ///< Is a DISCONNECT Indicate.
        return PVC_DISCONNECT; 										        ///< Disconnect
    if((X25Result->xl_type == XL_CTL)&&(X25Result->xl_command==N_RI))       ///< Is a RESET Indicate.
    { 
        UFC::BufferedLog::Printf(" Link:%d PVC:%d Receive a RESET Indicate.", FPort,FPVC );           
        return SendResetConfirm();                                          ///< Send reset confirm.
    }
    if((X25Result->xl_type == XL_CTL)&&( X25Result->xl_command==N_Abort))   ///< Disconnect, but there is not enough resource
    { 
        UFC::BufferedLog::Printf(" Link:%d PVC:%d Receive N_Abort.", FPort,FPVC );           
        throw( X25Exception( X25Result->xl_command,"N_Abort received." ));  ///< expected data packet,irrecoverable.
    }
    if((X25Result->xl_type != XL_DAT)||(X25Result->xl_command != N_Data))
    { 
        UFC::BufferedLog::Printf(" Link:%d PVC:%d Receive Expedited Packet.", FPort,FPVC );           
        throw( X25Exception( X25Result->xl_type,"Expected data packet." ));
    }
    if((X25Result->xl_type==XL_DAT)&&(X25Result->xl_command==N_EData)) 	    ///< PVC Expedited Data
    {
        UFC::BufferedLog::Printf(" Link:%d PVC:%d Receive Expedited Data.", FPort,FPVC );        
        throw( X25Exception( SendExpeditedDataAck(), "Receive Expedited Data.") );
    }
    if((X25Result->xl_type==XL_CTL)&&(X25Result->xl_command==N_PVC_DETACH))  ///< PVC DETACH Indicate
    {
     	if( FX25FD != -1 )
     	{
            close(FX25FD);
            FX25FD = -1;
    	}
        UFC::BufferedLog::Printf(" Link:%d PVC:%d Receive a DETACH Indicate.", FPort,FPVC );                   
    	throw( X25Exception( ((struct pvcdetf *)(void*)FCtrlBlock)->reason_code, "Receive DETACH Indicate.") );
    }
    return PVC_OK;
}
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
