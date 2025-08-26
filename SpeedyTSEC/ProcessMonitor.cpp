#include "Speedy.h"
#include "TWSESpeedyObjectsManager.h"
//------------------------------------------------------------------------------
//extern UFC::PEvent ProcessExistEvent;
extern UFC::PEvent* ProcessExistEventPtr;
//------------------------------------------------------------------------------
void ProcessMonitor::OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName )
{
    if( Host.AnsiCompare( UFC::Hostname ) != 0 )
    {
	if( AppName.AnsiCompare("SpeedyTSEC") == 0 || AppName.AnsiCompare("SpeedyOTC") == 0 )
        {
            UFC::BufferedLog::Printf( " %s on %s startup.", AppName.c_str() ,Host.c_str()  );
            FObjectsManager->OrderInfo.OnSpeedyStartup( Host );
        }
    }    
}
//------------------------------------------------------------------------------
void ProcessMonitor::OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName )
{
    if( Host.AnsiCompare( UFC::Hostname ) != 0 )
    {
    	if( AppName.AnsiCompare("SpeedyTSEC") == 0 || AppName.AnsiCompare("SpeedyOTC") == 0 )
        {
            UFC::BufferedLog::Printf( " %s on %s stopped.", AppName.c_str() ,Host.c_str()  );
            FObjectsManager->OrderInfo.OnSpeedyStop( Host );
        }
    }    
}
//------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        if( FIsTWSE == TRUE )
            UFC::BufferedLog::Printf( " ***** Process SpeedyTSEC already exists *****"  );
        else
            UFC::BufferedLog::Printf( " ***** Process SpeedyOTC already exists *****"  );
        sleep( 2 );
        exit( 0 );
    }
    ProcessExistEventPtr->SetEvent( );    
}
//------------------------------------------------------------------------------
void ProcessMonitor::OnConnected()
{
    UFC::BufferedLog::Printf( " MBus connected,Send execution sequence to other Speedy servers." );    
}
//------------------------------------------------------------------------------
void ProcessMonitor::OnDisconnected()
{
    UFC::BufferedLog::Printf( " MBus disconnected,Stop all execution Sync threads." );    
    FObjectsManager->OrderInfo.StopAllSyncThread();
}
//------------------------------------------------------------------------------
