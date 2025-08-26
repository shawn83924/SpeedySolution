#ident "@(#) $Id: MCClass.cpp,v 1.12 2004/05/31 02:11:22 ap27 Exp $"
#include "MCClass.h"
//----------------------------------------------------------------------------------------------------------
int	  MCCClient::FMCCHandle     = -1;
int	  MCCClient::FInterval      = 10;
BOOL  MCCClient::FStop          = TRUE;
void* MCCClient::FStatisticData = NULL;
PEvent MCCClient::FStopEvent;
FStatisticCallback MCCClient::FCallback = NULL;
//----------------------------------------------------------------------------------------------------------
MCCClient::MCCClient( int ProgramID, int ProgMinorID, void* Data , int Size, int Interval  )
{
    MCCClient::FInterval        = Interval;  	
  	MCCClient::FStatisticData   = Data;
  	MCCClient::FStop            = FALSE;
  	FMCCHandle = hb_open( 0, ProgramID, ProgMinorID, Size );
}
//----------------------------------------------------------------------------------------------------------
MCCClient::~MCCClient()
{
	Stop();
	FStopEvent.WaitFor( 10 );
}
//----------------------------------------------------------------------------------------------------------
void MCCClient::SetInterval( int Sec ) 
{ 
	if( MCCClient::FMCCHandle != -1 )
	{
		if( Sec != FInterval ) 
			FInterval = Sec; 
	}
}
//----------------------------------------------------------------------------------------------------------
void MCCClient::Stop( void )
{
	MCCClient::FStop = TRUE;
}
//----------------------------------------------------------------------------------------------------------
void MCCClient::Run( void )
{
	if( MCCClient::FMCCHandle != -1 )
	{
		pthread_t             ThreadID;
		pthread_attr_t        ThreadAttrib;
  		struct sched_param    Param;

    	pthread_attr_init( &ThreadAttrib );
    	memset( &Param, 0, sizeof( Param ) );
  		Param.sched_priority = -18;
  	    pthread_attr_setschedparam( &ThreadAttrib, &Param );
    	pthread_create( &ThreadID, &ThreadAttrib, Execute, NULL );
	}
}
//----------------------------------------------------------------------------------------------------------
void* MCCClient::Execute( void* )
{
	while( FStop == FALSE )
	{
		sleep( (long) FInterval );
		if( FCallback != NULL )
		{
			if( FCallback() == TRUE )
			{
				hb_write( MCCClient::FMCCHandle, ( char* )FStatisticData );
			}
			else
			{
				hb_ioctl( MCCClient::FMCCHandle, MCC_KILL_ME, 0 );
				break;	
			}
		}
	}
	hb_close( FMCCHandle );
	FStopEvent.SetEvent();
	return NULL;		
}
//----------------------------------------------------------------------------------------------------------
