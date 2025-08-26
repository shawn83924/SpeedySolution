//-----------------------------------------------------------------------------------------------------------------------
#ifndef _TCP_ONLY
//-----------------------------------------------------------------------------------------------------------------------
#include "X25Connection.h"
//-----------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32  ///< For Windows
//-----------------------------------------------------------------------------------------------------------------------
WinX25Lib::WinX25Lib()
{
	if( x25init( 0 , 0 ) < 0 )
	{
		UFC::BufferedLog::Printf(" Initialize the X.25 library failed." );
		exit( 1 );
	}
	else
	{
		UFC::BufferedLog::Printf(" Initialize the X.25 library OK." );
	}
}
//-----------------------------------------------------------------------------------------------------------------------
WinX25Lib X25Connection::FX25Lib;
//-----------------------------------------------------------------------------------------------------------------------
X25Connection::X25Connection( )
:Connection(),
 FX25FD( -1 )
{
	FSendBuff = x25alloc( PVC_MAX_PACKET_SIZE );
	FRecvBuff = x25alloc( PVC_MAX_PACKET_SIZE );
}
//-----------------------------------------------------------------------------------------------------------------------
X25Connection::~X25Connection( )
{
	if( FRecvBuff != NULL)
		x25free( FRecvBuff );
	if( FSendBuff != NULL)
		x25free( FSendBuff );
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::CheckError( void )
{
	int error = x25error();

	if( error == EX25NOINIT )
	{
		UFC::BufferedLog::Printf(" X.25 library not init, exit thread:%d",GetCurrentThreadId() );
		ExitThread( error );
	}
	return error;
}
//-----------------------------------------------------------------------------------------------------------------------
void X25Connection::Open( int LineNo, int Pvc, char*  )
{
	struct x25doneinfo DoneInfo;

	if( x25pvccall(Pvc,X25NOWAIT,&FX25FD,LineNo,XI_ENDEND,NULL ) < 0)
	{
		int error = CheckError();
		UFC::BufferedLog::Printf(" Attach PVC failed code:%d", error );
		sleep( 5 );
		throw( X25Exception( error, "x25pvccall failed!" ) );
	}
	if( x25done(FX25FD, 50, &DoneInfo) < 0 )
	{
		int error = CheckError();
		UFC::BufferedLog::Printf(" x25pvccall-x25done can't complete code:%d", error );
		throw( X25Exception( error, "x25pvccall->x25done can't complete." ) );
	}
	if( DoneInfo.xi_retcode != 0 )
	{
		int error = CheckError();
		UFC::BufferedLog::Printf(" x25done failed code:%d", DoneInfo.xi_retcode );		
		throw( X25Exception( error, "x25done failed." ) );
	}
	UFC::BufferedLog::Printf(" Attach to PVC:%d succeed.",Pvc);
	FIsOpen = TRUE; ///< Set the open flag to TRUE.
}
//-----------------------------------------------------------------------------------------------------------------------
void X25Connection::Close( void )
{
	struct x25doneinfo DoneInfo;

	if( FX25FD != -1 )
	{
		if( x25xhangup(FX25FD,0,0,NULL,NULL,NULL,NULL,XH_IMM,NULL) < 0)
		{
			int error = CheckError();			
			UFC::BufferedLog::Printf(" x25xhangup failed code:%d.", error );
		}
		x25done( FX25FD, 10, &DoneInfo );
		FX25FD = -1;
	}
	FIsOpen = FALSE;
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::WritPVCData( char*Data, int Len, int info, long timeout )
{
	struct x25doneinfo DoneInfo;

	memcpy( FSendBuff, Data, Len );
	if( x25send(FX25FD, FSendBuff, Len, info, NULL ) < 0 )
	{
		int error = CheckError();
		throw( X25Exception( error, "x25send failed!" ) );
	}
	if( x25done(FX25FD, timeout, &DoneInfo) < 0 )
	{
		int error = CheckError();
		throw( X25Exception( error, "x25send-x25done can't complete." ) );
	}
	if( DoneInfo.xi_retcode == 0 )
		return PVC_OK;    // Send okay
	else
		throw( X25Exception( DoneInfo.xi_retcode, "x25send failed.") );
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::Send( char* Data, int Length, long TimeOutSec )
{
	int    CurrentLength = Length;
	char*  CurrentData   = Data;
	int    Result;

	while( TRUE )
	{
		if( CurrentLength <= PVC_MAX_PACKET_SIZE )
		{
			return WritPVCData( Data, Length, 0, TimeOutSec );
		}
		else
		{
			if( (Result = WritPVCData( CurrentData, PVC_MAX_PACKET_SIZE, XI_MBIT, TimeOutSec ))!= PVC_OK )
				return Result; // Error occur
			CurrentLength -= PVC_MAX_PACKET_SIZE;
			CurrentData   += PVC_MAX_PACKET_SIZE;
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::ReadPVCData( char*Data, int& Len, BOOL& More, long timeout )
{
	struct x25doneinfo DoneInfo;
	int    rtncode,info;
	char   msg[ 256 ];
	DWORD BeginTime, TimeoutMS = timeout*1000;
	
	More = FALSE;
	if( x25recv( FX25FD, FRecvBuff, PVC_MAX_PACKET_SIZE, &info, NULL ) < 0 )
	{
		int error = CheckError();
		throw( X25Exception( error, "x25recv failed!" ) );
	}	
	BeginTime = GetTickCount( ); // Get the begin time.
	while(   x25done( FX25FD, XD_TONOW, &DoneInfo ) < 0  )
	{       // X25done() failed. If the error is ENETPEND, then the request hasn't completed - keep   polling. Otherwise, - exit. 		
		rtncode = x25error();
		 if(  rtncode != ENETPEND )
		 {
			// If x25done() fails with an unhandled  error then, abort the polling process 				
			UFC::BufferedLog::Printf(" x25done failed code:%d",rtncode );
			throw( X25Exception( x25error(), "x25recv-x25done can't complete." ) );
		}
		 UFC::SleepMS( 2 );
		 if( (GetTickCount( ) - BeginTime) > TimeoutMS  ) // Time out.
		 {
			 x25cancel( FX25FD,NULL );
			 x25done( FX25FD, 1, &DoneInfo );
			return PVC_TIMEOUT;
		 }
	}
	switch( DoneInfo.xi_cmd )
	{
		case XC_RECEIVE:   switch( DoneInfo.xi_retcode  )
							{
								case ENETIMSG:  More = TRUE;
								case 0:			if( DoneInfo.xi_len > 0 )
													memcpy( Data, DoneInfo.xi_buf, DoneInfo.xi_len );
												Len = DoneInfo.xi_len;
												return PVC_OK;
								case ENETPEND:  UFC::BufferedLog::Printf(" Receive ENETPEND" );
												 x25cancel( FX25FD,NULL );
												 x25done( FX25FD, 1, &DoneInfo );
												return PVC_TIMEOUT;
								case ENETSRESET:UFC::BufferedLog::Printf(" Receive ENETSRESET" );
												if( (x25cause() == 0x00) &&	(x25diag()  == 0x00) )
												{
													x25resetconfirm( FX25FD, NULL );
													x25done( FX25FD, 1, &DoneInfo );
													UFC::BufferedLog::Printf(" Send Reset Confirm" );
													return PVC_RESET;
												}
												else
													throw( X25Exception( x25error(), "x25recv-x25done FAILED." ) );
								default:        x25errormsg( msg );
												UFC::BufferedLog::Printf(" x25 Err:%s",msg );
												throw( X25Exception( x25error(), "x25recv-x25done FAILED." ) );
							}
		case XC_RESET:        x25resetconfirm( FX25FD, NULL );
							x25done( FX25FD, 1, &DoneInfo );
							UFC::BufferedLog::Printf(" Receive PVC reset" );
							return PVC_RESET;
		default:                          UFC::BufferedLog::Printf(" Receive xi_cmd    :%d", DoneInfo.xi_cmd );
					        	UFC::BufferedLog::Printf("         xi_retcode:%d", DoneInfo.xi_retcode );
						       return PVC_RESET;
	}	
	return PVC_TIMEOUT;
}
//-----------------------------------------------------------------------------------------------------------------------
int X25Connection::Receive( char* Data, int Length, long TimeOutSec )
{
	int  ReceivedLength = 0;
	int  RecvLen,Result;
	BOOL MoreData;

	do
	{
		if( (Result = ReadPVCData( Data+ReceivedLength, RecvLen, MoreData, TimeOutSec )) != PVC_OK )
			return Result;
		if( RecvLen > 0 )
			ReceivedLength += RecvLen;
		if( ReceivedLength == Length )
			break;

	}while( MoreData == TRUE );
	return ReceivedLength; // return received length
}
//-----------------------------------------------------------------------------------------------------------------------
void X25Connection::CloseX25( void )
{
	UFC::BufferedLog::Printf(" Release X.25 library resource" );
	x25exit();
}
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
