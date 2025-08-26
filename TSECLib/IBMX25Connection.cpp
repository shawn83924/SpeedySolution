 
/*-----------------------------------------------------------------------------------------------------------------------*/
#include "X25Connection.h"
//-----------------------------------------------------------------------------------------------------------------------
#ifdef __AIX
//-----------------------------------------------------------------------------------------------------------------------
X25Connection::X25Connection( )
:Connection(),
 FX25FD( -1 )
{
    FDataBuffer = new char[ PVC_MAX_PACKET_SIZE ];
    FCtrlBlock  = new char[ NPI_MAX_CTL ];
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
	    throw( X25Exception( rtn, "Has more data." ) );
	else if( rtn == MORECTL )
	    throw( X25Exception( rtn, "Has more control data." ) );
	else if( rtn != 0 ) // Call x.25 system call "getmsg"
	    throw( X25Exception( rtn, "Receive data failed." ) ); // Receive data error.
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
    FX25FD = open ("/dev/x25pkt", O_RDWR|O_NONBLOCK );
    if( FX25FD < 0 )
        throw( X25Exception( FX25FD, "Open X.25 failed." ) );
    if( ioctl( FX25FD, I_PUSH, "npi") < 0)
        throw( X25Exception( FX25FD, "X.25 Push to NPI failed." ) );
}
//----------------------------------------------------------------------------------------------------------------------
void X25Connection::SendAttachMessage(  int Line, int PVC   )
{
    struct strbuf   ctlblk;         /* control structure passed to putmsg */
    char            *addtl_info;
    char            PVC_No[ 5 ];
    int             Result;
    N_bind_req_t *  BindReq = (N_bind_req_t*)FCtrlBlock;

    sprintf( PVC_No, "%04d", PVC );
    memset( FCtrlBlock, 0, NPI_MAX_CTL);
    BindReq->PRIM_type = (ulong) N_BIND_REQ;
   /* Note that the address length is 6 and NOT four. Two bytes MUST preceed
    * the actual address of 1965 in an N_BIND_REQ. The first byte contains the
    * line number being used with this address, and the second byte is the
    * X.121 address prefix, which must be zero.*/
    BindReq->ADDR_length    = (ulong) 2+4;
    BindReq->ADDR_offset    = (ulong)(sizeof(N_bind_req_t));
    BindReq->CONIND_number  = (ulong) 1;
    BindReq->BIND_flags     = (ulong) PVC_BIND;
    BindReq->PROTOID_length = (ulong) 0;
    BindReq->PROTOID_offset = (ulong) 0;
    /* Now fill in the additional information required with this primitive
     * (address and protocol information) */
    addtl_info = (char *) (FCtrlBlock + sizeof(N_bind_req_t));
    /* The bitwise "&" ensures that the line number is only one byte long */
    *addtl_info++ = (char) Line & 0xff;
    *addtl_info++ = (char) 'P'; /* X.121 format */    
    memcpy( addtl_info, PVC_No, 4 );
    //bcopy( PVC_No, addtl_info, 4 );
    addtl_info += 4;
    /* Build the control structure that is sent to putmsg */
    ctlblk.maxlen = 0;
    ctlblk.len = NPI_MAX_CTL;
    ctlblk.buf = FCtrlBlock;
    Result = WritPVCData( &ctlblk, NULL, 5L);
    if( Result == PVC_TIMEOUT )
	throw( X25Exception( Result, "Send attach PVC message time out." ) ); // time out error
}
//----------------------------------------------------------------------------------------------------------------------
BOOL X25Connection::GetAttachRespsonseMessage( void )
{
    struct strbuf    ack_ctl;        /* control structure passed to getmsg */
    N_bind_ack_t*    BindAck;
    int Flags = 0;

    memset( FCtrlBlock, 0, NPI_MAX_CTL);
    ack_ctl.maxlen  = NPI_MAX_CTL;
    ack_ctl.len     = 0;
    ack_ctl.buf     = FCtrlBlock;
    
    /* Call getmsg() to check for an acknowledgement */   
    if( getmsg( FX25FD, &ack_ctl ,NULL, &Flags ) != 0 )
 	throw( X25Exception( PVC_TIMEOUT, "Get attached ack time out." ) ); // time out error    
    BindAck = (N_bind_ack_t *) ack_ctl.buf;
    /* Verify that we received an N_BIND_ACK*/
    if( BindAck->PRIM_type != N_BIND_ACK )
    {
    	UFC::BufferedLog::Printf(" Not a Binded acknowledgement" );
   	throw( X25Exception( BindAck->PRIM_type, "Bind acknowledgement NOT received." ));
    }
    /* Save the token value to return for use later */
    FToken = BindAck->TOKEN_value;    
    return TRUE;
}
/*-----------------------------------------------------------------------------------------------------------------------*/
void X25Connection::Open( int LineNo, int Pvc, char*, int )
{
    FPVC  = Pvc;
    FPort = LineNo;    
    UFC::BufferedLog::Printf( " Open PVC Line:%d PVC:%d",LineNo, Pvc );
    while( TRUE )
    {   ///< Open a X.25 stream.
        OpenStrem();
        UFC::BufferedLog::Printf(" Open X.25 device OK." );
        ///< Send control message to attach PVC.
        SendAttachMessage( LineNo, Pvc );
        ///< get respsonse message for the attach from the streams driver
        if( GetAttachRespsonseMessage() == TRUE ) ///< Will throw X25Exception after retry 10 times.
        {
            UFC::BufferedLog::Printf(" Attach to PVC:%d succeed.",Pvc);
            FIsOpen = TRUE; ///< Set the open flag to TRUE.
            return;         ///< Open PVC succeed.
        }
        UFC::BufferedLog::Printf(" Attach to PVC:%d time out, close it an try again...",Pvc );
        sleep( 2 );
        Close(); ///< Attach to PVC time out, close it an try again....
    };
}
//-----------------------------------------------------------------------------------------------------------------------
void X25Connection::Close( void )
{
    if( FX25FD != -1 )
    {        
	try
	{
	    SendDetachMessage( );
	    GetDetachRespsonseMessage();
	    close( FX25FD );
	}
	catch( ... )
	{
	    close( FX25FD );
	    UFC::BufferedLog::Printf(" Try to close failed." );
	}
        FX25FD = -1;
    }
    FIsOpen = FALSE;
}
/*-----------------------------------------------------------------------------------------------------------------------*/
int X25Connection::Send( char* Data, int Length, long TimeoutSec )
{
    struct strbuf rec_ctl;	/* control structure for getmsg */
    struct strbuf rec_data;	/* data structure for getmsg */
    int    Result;
    N_data_req_t* DataReq = (N_data_req_t*)FCtrlBlock;

    memset( FCtrlBlock,0, NPI_MAX_CTL);
    DataReq->PRIM_type = (ulong) N_DATA_REQ;
    DataReq->DATA_xfer_flags = (ulong) 0;
    /* Build the control structure */
    rec_ctl.maxlen = 0;
    rec_ctl.len = NPI_MAX_CTL;
    rec_ctl.buf = FCtrlBlock;
    /* Build the data structure */
    rec_data.maxlen = 0;
    rec_data.len = Length;
    rec_data.buf = (char *)Data;
    if( (Result = WritPVCData( &rec_ctl,&rec_data,TimeoutSec ))!= PVC_OK )
	     return Result; // Error occur
    return PVC_OK;
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::Receive( char* Data,int Length, long TimeoutSec )
{
    struct strbuf  ct,dt;
    int            ReceivedLength = 0;
    int 	   Result;
    BOOL           MoreData;

    do
    {
        memset( FCtrlBlock, 0, NPI_MAX_CTL );
        memset( FDataBuffer, 0, PVC_MAX_PACKET_SIZE );
	ct.buf    = FCtrlBlock;
	ct.maxlen = NPI_MAX_CTL;
        ct.len    = 0;        
	dt.buf    = FDataBuffer;
	dt.maxlen = PVC_MAX_PACKET_SIZE;
        dt.len    = 0;
	if( (Result = CheckPVCAndRead( &ct, &dt, MoreData, TimeoutSec )) != PVC_OK )
	     return Result;                
	if( dt.len > 0 ) /// -1 means no data received.
        {
            if( ReceivedLength +dt.len > Length )
                throw( X25Exception( Length, "Packet size larger then buffer size." ) );
            memcpy( Data + ReceivedLength, dt.buf, dt.len );
            ReceivedLength += dt.len;
            if( MoreData == FALSE ) ///< No more data, stop to receive.
               	break;                    
         }
         else
             throw( X25Exception( Result, "Packet size equals 0." ) );
	 if( ReceivedLength == Length )
             break;

    }while( TRUE );
    return ReceivedLength; // return received length
}
/*-----------------------------------------------------------------------------------------------------------------------*/
int X25Connection::SendResetConfirm( )
{
    struct strbuf   ctlbuf;
    N_reset_res_t   rset;

    memset( &rset, 0, sizeof( N_reset_res_t ) );
    rset.PRIM_type  = (ulong)N_RESET_RES;
    ctlbuf.len      = sizeof( N_reset_res_t );
    ctlbuf.buf      = (char*)&rset;
    // Send a RESET Confirm packet.
    WritPVCData( &ctlbuf, NULL, 5L);
    return PVC_RESET;
}
//----------------------------------------------------------------------------------------------------------------------
void X25Connection::SendDetachMessage( void )
{
    int	             Result;
    struct strbuf    req_ctl;        /* control structure for putmsg */
    N_unbind_req_t*  UnBindReq = (N_unbind_req_t*)FCtrlBlock;

    memset( FCtrlBlock, 0, NPI_MAX_CTL);
    UnBindReq->PRIM_type = (ulong) N_UNBIND_REQ;
    req_ctl.maxlen       = 0;
    req_ctl.len          = NPI_MAX_CTL;
    req_ctl.buf          = FCtrlBlock;
    Result = WritPVCData( &req_ctl, NULL, 5L );
    if( Result == PVC_TIMEOUT )
	throw( X25Exception( PVC_TIMEOUT, "Send detach PVC message time out." ) ); // time out error
}
//----------------------------------------------------------------------------------------------------------------------
void X25Connection::GetDetachRespsonseMessage( void )
{
    struct strbuf   ack_ctl;        ///< Control structure for getmsg to get a ack control package.
    N_ok_ack_t*     AckMsg;

    ack_ctl.maxlen = NPI_MAX_CTL;
    ack_ctl.len    = 0;
    ack_ctl.buf    = FCtrlBlock;
    if( ReadPVCData( &ack_ctl, NULL, 5L ) == PVC_TIMEOUT )
  	throw( X25Exception( PVC_TIMEOUT, "Get detach PVC message time out." ) ); ///< time out error
    AckMsg = (N_ok_ack_t *) ack_ctl.buf;
    if( AckMsg->PRIM_type != N_OK_ACK)
	throw( X25Exception( AckMsg->PRIM_type, "NPI_unbind failed" ));
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::CheckPVCAndRead( struct strbuf *Control, struct strbuf *Data, BOOL& MoreData, long timeout  )
{
    if( ReadPVCData( Control, Data, timeout ) == PVC_OK )
	return CheckCTLCommand( Control,MoreData );
    return PVC_TIMEOUT;///< time out
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::CheckPVCForWrite( void )
{
    struct strbuf  Control;
    int            flag = 0;
    int            Rtn;
    BOOL           MoreData;

    memset( FCtrlBlock, 0, PVC_MAX_CONTROLBUFFER );
    Control.maxlen = NPI_MAX_CTL;
    Control.len    = 0;
    Control.buf    = FCtrlBlock;
    if( ReadyToRead( 0L,10L ) == FALSE ) ///< Any data in buffer?
	return PVC_OK;  ///< Time out, no control packet
    if( ( Rtn = getmsg( FX25FD, &Control, 0, &flag ) ) != 0 )
	throw( X25Exception( Rtn, "Send data failed.") );
    return CheckCTLCommand( &Control, MoreData );
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::CheckCTLCommand( struct strbuf* rec_ctl, BOOL& MoreData )
{
    N_data_ind_t*  DataInd = (N_data_ind_t*)rec_ctl->buf;
    
    if( rec_ctl->len != -1  ) ///< Received Control package
    {
        if( DataInd->PRIM_type == N_DATA_IND ) ///< Handle the data indicater N_DATA_IND 
        {
            if( (DataInd->DATA_xfer_flags & N_MORE_DATA_FLAG ) == 0 )
                MoreData = TRUE;
            return PVC_OK;
        }
        else if( DataInd->PRIM_type == N_RESET_IND ) ///< Handle the reset indicater N_RESET_IND
        {
            N_reset_ind_t* RestInd =  (N_reset_ind_t *)rec_ctl->buf;	    
	    
            if( RestInd->RESET_reason == N_NET_LINK_UP )
	    {
		UFC::BufferedLog::Printf(" Receive N_NET_LINK_UP" );
		SendResetConfirm();		
		ReadPVCData( rec_ctl, NULL, 5L );
		N_ok_ack_t* OkAck = (N_ok_ack_t *) rec_ctl->buf;
		if( OkAck->PRIM_type != N_OK_ACK )
                    throw( X25Exception( OkAck->PRIM_type, "RESponse RESET Primitive failed!") );
		return PVC_RESET;
	    }
	    throw( X25Exception( RestInd->RESET_reason, "Receive reset indicater") );
        }
        else
            throw( X25Exception( DataInd->PRIM_type, "Receive unknown indicater.") );
    }
    MoreData = FALSE;    
    return PVC_OK;    
}
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
