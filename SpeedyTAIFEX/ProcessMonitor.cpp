#include "Speedy.h"
#include "SpeedyObjectsManager.h"
//------------------------------------------------------------------------------
extern UFC::PEvent            ProcessExistEvent;
//------------------------------------------------------------------------------
ProcessMonitor::ProcessMonitor( BOOL IsOpt, SpeedyObjectsManager* ObjsManager )
:FIsOptions( IsOpt )
,FObjectsManager( ObjsManager)        
{
}
//------------------------------------------------------------------------------
void ProcessMonitor::OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName )
{
    if( Host.AnsiCompare( UFC::Hostname ) != 0 )
    {
	if( AppName.AnsiCompare("SpeedyOPT") == 0 || AppName.AnsiCompare("SpeedyFUT") == 0 )
        {
            UFC::BufferedLog::Printf( " Speedy on %s startup.", Host.c_str()  );
            FObjectsManager->OrderInfo.OnSpeedyStartup( Host );
        }
    }
}
//------------------------------------------------------------------------------
void ProcessMonitor::OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName )
{
    if( Host.AnsiCompare( UFC::Hostname ) != 0 )
    {
    	if( AppName.AnsiCompare("SpeedyOPT") == 0 || AppName.AnsiCompare("SpeedyFUT") == 0 )
        {
            UFC::BufferedLog::Printf( " Speedy on %s stopped.", Host.c_str()  );
            FObjectsManager->OrderInfo.OnSpeedyStop( Host );
        }
    }
}
//------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        if( FIsOptions == TRUE )
            UFC::BufferedLog::Printf( " ***** Process SpeedyOPT already exists *****"  );
        else
            UFC::BufferedLog::Printf( " ***** Process SpeedyFUT already exists *****"  );
        sleep( 2 );
        exit( 0 );
    }
    ProcessExistEvent.SetEvent( );
}
//------------------------------------------------------------------------------
void ProcessMonitor::OnConnected()
{
    UFC::BufferedLog::Printf( " MBus connected,Send execution sequence to other Speedy servers." );    
    FObjectsManager->OrderInfo.SendSequenceToAllSpeedy();
}
//------------------------------------------------------------------------------
void ProcessMonitor::OnDisconnected()
{
    UFC::BufferedLog::Printf( " MBus disconnected,Stop all execution Sync threads." );        
    FObjectsManager->OrderInfo.StopAllSyncThread();
}
//------------------------------------------------------------------------------
