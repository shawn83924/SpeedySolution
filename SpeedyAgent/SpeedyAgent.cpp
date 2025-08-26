
#include "SpeedyAgent.h"
#include "SpeedyManager.h"
//----------------------------------------------------------------------------------------------------------------------
#include "../Version.h"
#include "../UFC/TLicenseKey.h"
#include <iostream>
#include <sys/stat.h>
//----------------------------------------------------------------------------------------------------------------------
// Change this constant to modify the version of SpeedyAgent
// example: 2001 means Version 2.0.01
//          2138 means Version 2.1.38
//----------------------------------------------------------------------------------------------------------------------
const int SpeedyAgentVer = SPEEDY_AGENT_VER_INT;
//----------------------------------------------------------------------------------------------------------------------
BOOL             START_ALL  = FALSE;
BOOL             CLEAR      = FALSE;
MessageObject*   MessageObj = NULL;
SpeedyManager*   Manager    = NULL;
UFC::AnsiString  CurrentDir;
//--------------------------------------------------------------------------------------------------------------------
BOOL             MonitorOtherProcess = FALSE;
UFC::AnsiString  OtherProcessFilename;
UFC::AnsiString  OtherProcessCommand;
UFC::AnsiString  CurrentClientKey = "SpeedyAgent";
int              MONOTER_OTHER_PROCESS = 0;
ProcessInfo      MoniterOtherProcess[ MAX_PROCESS ];
//--------------------------------------------------------------------------------------------------------------------
int              MONOTER_PROCESS = 0;
ProcessInfo      MoniterProcess[ MAX_PROCESS ];
//--------------------------------------------------------------------------------------------------------------------
UFC::AnsiString  LicenseExpDate;
UFC::AnsiString  VersionBuidDate;
UFC::UDateTime   StartupTime;
//--------------------------------------------------------------------------------------------------------------------
const char*  PROCESS_CFG        = "../cfg/Speedy.cfg";
const char*  OTHER_PROCESS_CFG  = "../cfg/OtherProcess.cfg";
const char*  FIXSPEC_DIR        = "../FIXSPEC";
const char*  FIX_CFG_FILE       = "../cfg/imFIX.cfg";
const char*  ERR_MSG_SHELL      = "./ErrorMessage.sh";
//----------------------------------------------------------------------------------------------------------------------
const int AgentStartNotification = 1;    
const int OtherStartNotification = 2;     
const int AgentStopNotification = 4;    
const int OtherStopNotification = 8;
//----------------------------------------------------------------------------------------------------------------------
bool CheckMask( int mask, int bit ) 
{
    return ( ( mask & bit ) != 0 );
}
//----------------------------------------------------------------------------------------------------------------------
bool IsRunningTime( int i )
{
    Int32 TimeNow = UFC::GetSecondsToday();
    
    if( MoniterProcess[i].IsAuto == TRUE && MoniterProcess[i].IsRemote == FALSE )
    {   ///< Start time < Stop Time, Normal case. (example: 07:00 start --> 16:00 stop)            
        if( MoniterProcess[i].StartSec < MoniterProcess[i].StopSec )
        {
            if( TimeNow > (MoniterProcess[i].StartSec + 30) && TimeNow < (MoniterProcess[i].StopSec - 30)  )
                return true;
        }
        else///< Start time > Stop Time, Over on day case. (example: 22:00 start --> 05:00 stop
        {
            if( TimeNow > (MoniterProcess[i].StartSec + 30) || TimeNow < (MoniterProcess[i].StopSec - 30)  )
                return true;
        }
    }
    return false;    
}
//------------------------------------------------------------------------------
void SendLineNotifyMessage(UFC::LineMsgTypeEnum LineMsgType, UFC::AnsiString& Message, UFC::AnsiString& Script)
{
    if (Script.Length() > 0)
        UFC::SendLineNotifyMessage(LineMsgType, Message, Script);
    else
        UFC::SendLineNotifyMessage(LineMsgType, Message);
}
//------------------------------------------------------------------------------
UFC::PEvent* ProcessExistEventPtr = 0;  //Modified by Zhen Fan 2021/08/30
//----------------------------------------------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        UFC::BufferedLog::Printf( " ***** SpeedyAgent already exists *****"  );
        UFC::SleepMS( 2000 );
        exit( 0 );
    }
    ProcessExistEventPtr->SetEvent( );  //Modified by Zhen Fan 2021/08/30
}
//--------------------------------------------------------------------------------------------------------------------
void ProcessMonitor::OnProcessList( UFC::PStringList& Processs )
{
        UFC::AnsiString        ProcessName;
        UFC::PStringHashedSet  RunningProcesses;
        ///< Build running processes set
        for( int i = 0; i < Processs.ItemCount(); i++ )        
             RunningProcesses.Add( Processs.GetItem( i ) );       
        ///< Check each process
        for( int i = 0; i < MONOTER_PROCESS; i++ )
        {
            if( RunningProcesses.Exists( MoniterProcess[i].ProcessName ) == FALSE )
            {
                if( MoniterProcess[ i ].IsRunning == 1 )
                {
                    UFC::AnsiString ErrMsg;
                    
                    ErrMsg.Printf( "Process:%s stopped at %s", MoniterProcess[i].ProcessName, UFC::Hostname );
                    WriteErrorLog( "SpeedySolution", MoniterProcess[i].ProcessName , ErrMsg );                    
                    if( IsRunningTime( i ) == true )
                    {
                        UFC::BufferedLog::Printf( " %s", ErrMsg.c_str()  );
                        if ( MoniterProcess[ i ].IsStopByAgent == true )
                        {
                            ErrMsg.Printf( "SpeedyAgent_Process[%s] stoped at %s !", MoniterProcess[i].ProcessName, UFC::Hostname  );
                            if ( CheckMask( AgentStopNotification, MoniterProcess[ i ].NotifyType ) == true )
                                    SendLineNotifyMessage( UFC::lmtError, ErrMsg, MoniterProcess[ i ].LineNotifyScript ); //Add by Vicky 2023/06/28
                            MoniterProcess[ i ].IsStopByAgent = false;
                        }
                        else
                        {
                            ErrMsg.Printf( "[%s] crashed at %s !", MoniterProcess[i].ProcessName, UFC::Hostname );
                            if ( CheckMask( OtherStopNotification, MoniterProcess[ i ].NotifyType ) == true )
                                SendLineNotifyMessage( UFC::lmtError, ErrMsg, MoniterProcess[ i ].LineNotifyScript ); //Add by Vicky 2023/06/28
                        }
                        UFC::SendLineMessage( UFC::lmtError, ErrMsg );
                    }
                }                    
                MoniterProcess[ i ].IsRunning = 0;
            }
            else
            {
                if( MoniterProcess[ i ].IsRunning == 0 )
                {
                    UFC::AnsiString InfoMsg;
                    if ( MoniterProcess[ i ].IsStartByAgent == true )
                    {
                        InfoMsg.Printf( "SpeedyAgent_Process[%s] started at %s", MoniterProcess[i].ProcessName, UFC::Hostname );
                        if ( CheckMask( AgentStartNotification, MoniterProcess[ i ].NotifyType ) == true )
                            SendLineNotifyMessage( UFC::lmtOK, InfoMsg, MoniterProcess[ i ].LineNotifyScript ); //Add by Vicky 2023/06/28
                        MoniterProcess[ i ].IsStartByAgent = false;
                    }
                    else 
                    {
                        InfoMsg.Printf( "[%s] started at %s", MoniterProcess[i].ProcessName, UFC::Hostname );
                        if ( CheckMask( OtherStartNotification, MoniterProcess[ i ].NotifyType ) == true )
                            SendLineNotifyMessage( UFC::lmtOK, InfoMsg, MoniterProcess[ i ].LineNotifyScript ); //Add by Vicky 2023/06/28
                    }
                    UFC::BufferedLog::Printf( " %s", InfoMsg.c_str()  );
                }
                MoniterProcess[ i ].IsRunning = 1;
            }
        }
    #ifndef _WIN32 ///< Win32 not support non-MBus process monitoring.
        if( MonitorOtherProcess == TRUE )
        {
            system( OtherProcessCommand );
            UFC::FileStream ProcessFile( OtherProcessFilename, O_RDONLY );
            Int32           DataSize = ProcessFile.GetSize();
            unsigned char*  buffer   = new unsigned char[ DataSize ];
            int             ReadSize = ProcessFile.Read( buffer, DataSize );
            UFC::AnsiString Processes( (char*)buffer, ReadSize );

            delete [] buffer;
            ///< Check each process
            for( int i = 0; i < MONOTER_OTHER_PROCESS; i++ )
            {
                if( Processes.AnsiPos( MoniterOtherProcess[ i ].ProcessName ) != -1 )
                    MoniterOtherProcess[ i ].IsRunning = 1;
                else
                    MoniterOtherProcess[ i ].IsRunning = 0;
            }
        }
    #endif
}
//--------------------------------------------------------------------------------------------------------------------
void SendToSpeedyCenter( const UFC::AnsiString& Msg)
{
    if( MessageObj != NULL )
    {
        MTree Data;

        Data.append( "COMMAND", 9020 );
        Data.append( "MSG", Msg );
        MessageObj->Send( "SPEEDY.COMMAND", UFC::Hostname, Data, FALSE );
    }
    if( UFC::FileExists( ERR_MSG_SHELL ) == TRUE )
    {
        UFC::AnsiString CMD;
    
        CMD.Printf( "%s %s %s", ERR_MSG_SHELL, UFC::Hostname, Msg.c_str() );
        UFC::BufferedLog::Printf(" Command[ %s ]", CMD.c_str() );
        system( CMD.c_str() );        
    }
    
}
//--------------------------------------------------------------------------------------------------------------------
void GetCurrentDir()
{
    CurrentDir = UFC::GetCurrentDir();      
}
//--------------------------------------------------------------------------------------------------------------------
void StartProcess( int i, BOOL Clean )
{
    if( i < MONOTER_PROCESS && i >= 0 )
    {
		MoniterProcess[ i ].IsStartByAgent = true;
        chdir( MoniterProcess[i].Directory );
        Start( MoniterProcess[ i ].ProcessName, MoniterProcess[ i ].BinaryName, MoniterProcess[i].Param, Clean );
        chdir( CurrentDir.c_str() );
    }
    else
        UFC::BufferedLog::Printf(" Start process index %d out of bound.", i );
}
//--------------------------------------------------------------------------------------------------------------------
void StopProcess( int i )
{
    if( i < MONOTER_PROCESS && i >= 0 )
    {
	MoniterProcess[ i ].IsStopByAgent = true;
        chdir( MoniterProcess[i].Directory );
        Stop( MoniterProcess[i].ProcessName );        
        chdir( CurrentDir.c_str() );
    }
    else
        UFC::BufferedLog::Printf(" Stop process index %d out of bound.", i );
}
//--------------------------------------------------------------------------------------------------------------------
void StartOtherProcess( int i )
{
    if( i < MONOTER_OTHER_PROCESS && i >= 0 )
    {
	MoniterProcess[ i ].IsStartByAgent = true;
        chdir( MoniterOtherProcess[i].Directory );
        system( MoniterOtherProcess[ i ].BinaryName  );
        UFC::BufferedLog::Printf(" Start other process[%s]", MoniterOtherProcess[i].BinaryName );
        chdir( CurrentDir.c_str() );
    }
    else
        UFC::BufferedLog::Printf(" Start Other process index %d out of bound.", i );
}
//--------------------------------------------------------------------------------------------------------------------
void StopOtherProcess( int i )
{
    if( i < MONOTER_OTHER_PROCESS && i >= 0 )
    {
	MoniterProcess[ i ].IsStopByAgent = true;
        chdir( MoniterOtherProcess[i].Directory );
        system( MoniterOtherProcess[i].Param);
        UFC::BufferedLog::Printf(" Stop other process[%s]", MoniterOtherProcess[i].Param);
        chdir( CurrentDir.c_str() );
    }
    else
        UFC::BufferedLog::Printf(" Stop Other process index %d out of bound.", i );
}
//----------------------------------------------------------------------------------------------------------------------
void LaunchScript( int ProcessIndex, int Action, BOOL IsMBusApp )
{
    switch( Action )
    {
            case PROCESS_STOP:      if( IsMBusApp == TRUE )
                                        StopProcess( ProcessIndex );
                                    else
                                        StopOtherProcess( ProcessIndex );
                                    break;
            case PROCESS_START:     if( IsMBusApp == TRUE )
                                        StartProcess( ProcessIndex, FALSE );
                                    else
                                        StartOtherProcess( ProcessIndex );
                                    break;
            case PROCESS_RESTART:   if( IsMBusApp == TRUE )
                                    {
                                        StopProcess( ProcessIndex );
                                        UFC::SleepMS( 1500 );
                                        StartProcess( ProcessIndex, FALSE );
                                    }
                                    else
                                    {
                                        StopOtherProcess( ProcessIndex );
                                        UFC::SleepMS( 1500 );
                                        StartOtherProcess( ProcessIndex );
                                    }
                                    break;
            case PROCESS_STARTCLEAR:if( IsMBusApp == TRUE )
                                        StartProcess(ProcessIndex, TRUE );
                                    else
                                        StartOtherProcess( ProcessIndex );        
                                    break;
    }
}
//------------------------------------------------------------------------------
void LaunchScriptAll( int Action, BOOL IsMBusApp  )
{
    for( int i = 0; i < MONOTER_PROCESS; i++ )
         LaunchScript( i, Action , IsMBusApp );
}
//--------------------------------------------------------------------------------------------------------------------
void GetProcesses()
{
    UFC::AnsiString ProcessCFGFile;
        
    ///< Read from Speedy.cfg                               
    ProcessCFGFile.Printf( "%s/%s", CurrentDir.c_str() , PROCESS_CFG ); 
    UFC::UiniFile   Config( ProcessCFGFile );
    UFC::Section*   ProcessSection;
    UFC::AnsiString StartTime,StopTime,ProcessName,Param,AutoStart,WorkingDir,Value;

    MONOTER_PROCESS = Config.SectionCount();
    if( MONOTER_PROCESS > MAX_PROCESS )
        MONOTER_PROCESS = MAX_PROCESS;
    for( int i = 0; i < MONOTER_PROCESS; i++ )
    {
        ProcessSection = Config.GetSection( i );
        strcpy( MoniterProcess[i].ProcessName, ProcessSection->GetSectionName().c_str());
        ProcessSection->GetValue("Binary",ProcessName);
        strcpy( MoniterProcess[i].BinaryName, ProcessName.c_str());
        if( ProcessSection->GetValue("Param",Param))
            strcpy( MoniterProcess[i].Param, Param.c_str());
        if( ProcessSection->GetValue( "Auto",AutoStart ))
            MoniterProcess[i].IsAuto = AutoStart.ToInt();
        else
            MoniterProcess[i].IsAuto = TRUE;
        if( ProcessSection->GetValue( "Remote",Value ) && Value.ToInt() == 1 )
            MoniterProcess[i].IsRemote = TRUE;
        else
            MoniterProcess[i].IsRemote = FALSE;        
        if( ProcessSection->GetValue( "WorkingDir",WorkingDir ))
            strcpy( MoniterProcess[i].Directory, WorkingDir.c_str());
        else
            strcpy( MoniterProcess[i].Directory, CurrentDir.c_str() );        
        ProcessSection->GetValue( "Start",StartTime );
        MoniterProcess[i].StartSec = StartTime.SubString(0,2).ToInt()*3600 + StartTime.SubString(3,2).ToInt()*60;
        ProcessSection->GetValue( "Stop",StopTime );
        MoniterProcess[i].StopSec = StopTime.SubString(0,2).ToInt()*3600 + StopTime.SubString(3,2).ToInt()*60;
        if( ProcessSection->GetValue( "Sleep",Value ))
            MoniterProcess[i].SleepSec = Value.ToInt();
        else
            MoniterProcess[i].SleepSec = 0;
        if ( ProcessSection->GetValue( "AgentStartNotification", Value ) )
        {
            if ( Value.ToInt() == 1 )
                MoniterProcess[i].NotifyType |= AgentStartNotification;
        }
        if ( ProcessSection->GetValue( "OtherStartNotification", Value ) )
        {
            if ( Value.ToInt() == 1 )
                MoniterProcess[i].NotifyType |= OtherStartNotification;
        }
        if ( ProcessSection->GetValue( "AgentStopNotification", Value ) )
        {
            if ( Value.ToInt() == 1 )
                MoniterProcess[i].NotifyType |= AgentStopNotification;
        }
        if ( ProcessSection->GetValue( "OtherStopNotification", Value ) )
        {
            if ( Value.ToInt() == 1 )
                MoniterProcess[i].NotifyType |= OtherStopNotification;
        }
        if ( ProcessSection->GetValue( "LineNotifyScript", Value ) )
        {
            if( Value.Length() > 0 )
                MoniterProcess[i].LineNotifyScript = Value;
        }
        ///< Watch dog features.        
        if( ProcessSection->GetValue( "WatchDog",Value ))
            MoniterProcess[i].EnableWatchDog = Value.ToInt();
        else
            MoniterProcess[i].EnableWatchDog = FALSE;
        if( ProcessSection->GetValue( "WatchDogSeconds",Value ))
            MoniterProcess[i].DogWatchSec = Value.ToInt();
        else
            MoniterProcess[i].DogWatchSec = 10;
        MoniterProcess[i].RestartTimes = 0;
        MoniterProcess[i].LastRestartSec = 0;
        if( ProcessSection->GetValue( "WatchDogTimes",Value ))
            MoniterProcess[i].DogStartTimes = Value.ToInt();
        else
            MoniterProcess[i].DogStartTimes = 3;
    }        
}
//--------------------------------------------------------------------------------------------------------------------
void GetOtherProcesses()
{
#ifndef _WIN32
    try
    {
        UFC::AnsiString ProcessCFGFile;
        
        ProcessCFGFile.Printf( "%s/%s", CurrentDir.c_str() , OTHER_PROCESS_CFG );                                
        UFC::UiniFile   Config( ProcessCFGFile );
        UFC::Section*   ProcessSection;
        UFC::AnsiString StartTime,StopTime,StartScript,StopScript,AutoStart,WorkingDir,Value;

        MONOTER_OTHER_PROCESS = Config.SectionCount();
        if( MONOTER_OTHER_PROCESS > MAX_PROCESS )
            MONOTER_OTHER_PROCESS = MAX_PROCESS;
        for( int i = 0; i < MONOTER_OTHER_PROCESS; i++ )
        {
            ProcessSection = Config.GetSection( i );
            strcpy( MoniterOtherProcess[i].ProcessName, ProcessSection->GetSectionName().c_str());
            if( ProcessSection->GetValue("StartScript",StartScript) )
                strcpy( MoniterOtherProcess[i].BinaryName, StartScript.c_str());
            if( ProcessSection->GetValue("StopScript",StopScript))
                strcpy( MoniterOtherProcess[i].Param, StopScript.c_str());
            if( ProcessSection->GetValue( "Auto",AutoStart ))
                MoniterOtherProcess[i].IsAuto = AutoStart.ToInt();
            else
                MoniterOtherProcess[i].IsAuto = TRUE;        
            if( ProcessSection->GetValue( "WorkingDir",WorkingDir ))
                strcpy( MoniterOtherProcess[i].Directory, WorkingDir.c_str());
            else
                strcpy( MoniterOtherProcess[i].Directory, CurrentDir.c_str() );        
            if( ProcessSection->GetValue( "Start",StartTime ) )
                MoniterOtherProcess[i].StartSec = StartTime.SubString(0,2).ToInt()*3600 + StartTime.SubString(3,2).ToInt()*60;
            if( ProcessSection->GetValue( "Stop",StopTime ) )
                MoniterOtherProcess[i].StopSec = StopTime.SubString(0,2).ToInt()*3600 + StopTime.SubString(3,2).ToInt()*60;
        }
        OtherProcessCommand.Printf( "ps -eo args > %s/process.list" ,CurrentDir.c_str() );
        OtherProcessFilename.Printf( "%s/process.list", CurrentDir.c_str() );
        MonitorOtherProcess = TRUE;
    }
    catch(...)
    {        
        UFC::BufferedLog::Printf(" Open file %s failed.", OTHER_PROCESS_CFG );
        UFC::BufferedLog::Printf(" Do not need monitoring other processes." );
        MonitorOtherProcess = FALSE;
    }
#endif
}
//--------------------------------------------------------------------------------------------------------------------
int GetThreshold( int Val, int Max, int Min )
{
    if( Val > Max )
        return Max;
    else if( Val < Min )
        return Min;
    else
        return Val;
}
//--------------------------------------------------------------------------------------------------------------------
void CheckProcessStart( int PIndex, BOOL ForceStart, int TimeNow )
{
    if( MoniterProcess[PIndex].IsRunning == 0 )
    {
        int TimeDiff = TimeNow - MoniterProcess[PIndex].StartSec;

        if( ForceStart == TRUE )
        {
            if( CLEAR == TRUE )
                LaunchScript( PIndex, PROCESS_STARTCLEAR, TRUE  );
            else
                LaunchScript( PIndex, PROCESS_START, TRUE  );
        }
        else if( MoniterProcess[PIndex].EnableWatchDog == TRUE && TimeDiff > MoniterProcess[PIndex].DogWatchSec +30 )
        {
            if( MoniterProcess[PIndex].LastRestartSec != 0 )
            {
                int ElapsedSec = TimeNow - MoniterProcess[PIndex].LastRestartSec;
                if( ElapsedSec >= MoniterProcess[PIndex].DogWatchSec )
                {
                    if( MoniterProcess[PIndex].RestartTimes < MoniterProcess[PIndex].DogStartTimes )
                    {
                        UFC::AnsiString Msg;

                        MoniterProcess[PIndex].LastRestartSec = 0;   
                        MoniterProcess[PIndex].RestartTimes++;
                        LaunchScript( PIndex, PROCESS_RESTART, TRUE  );
                        Msg.Printf( " Restart[%s] %d times.", MoniterProcess[PIndex].ProcessName, MoniterProcess[PIndex].RestartTimes );
                        SendToSpeedyCenter( Msg );
                    }
                    else if( MoniterProcess[PIndex].RestartTimes == MoniterProcess[PIndex].DogStartTimes )
                    {   
                        UFC::AnsiString Msg;

                        Msg.Printf( " Restart[%s] up to %d times.", MoniterProcess[PIndex].ProcessName, MoniterProcess[PIndex].RestartTimes );
                        MoniterProcess[PIndex].RestartTimes++;
                        SendToSpeedyCenter( Msg );                    
                    }
                }
                else
                {
                    int LeavesSec = MoniterProcess[PIndex].DogWatchSec - ElapsedSec;

                    UFC::BufferedLog::Printf(" Restart[%s] after %d sec", MoniterProcess[PIndex].ProcessName, LeavesSec );
                }
            }
            else
                MoniterProcess[PIndex].LastRestartSec = TimeNow;
        }
        else if( TimeDiff < 30 && TimeDiff > 0 )        
             LaunchScript( PIndex, PROCESS_STARTCLEAR, TRUE  );
    }
}
//--------------------------------------------------------------------------------------------------------------------
void CheckProcessStop( int PIndex, int TimeNow )
{
    if( MoniterProcess[PIndex].IsRunning == 1 )
    {
        int TimeDiff = TimeNow - MoniterProcess[PIndex].StopSec;

        if( TimeDiff < 30 && TimeDiff > 0 )
            LaunchScript( PIndex, PROCESS_STOP, TRUE );
    }
}
//--------------------------------------------------------------------------------------------------------------------
void CheckOtherProcessStart( int PIndex, int TimeNow )
{
    if( MoniterOtherProcess[ PIndex ].IsRunning == 0 ) ///< Not running.
    {
        int TimeDiff = TimeNow - MoniterOtherProcess[ PIndex ].StartSec;

        if( TimeDiff < 20 && TimeDiff > 0 )
            LaunchScript( PIndex, PROCESS_START, FALSE );
    }
}
//--------------------------------------------------------------------------------------------------------------------
void CheckOtherProcessStop( int PIndex, int TimeNow )
{
    if( MoniterOtherProcess[ PIndex ].IsRunning == 1 )
    {
        int TimeDiff = TimeNow - MoniterOtherProcess[ PIndex ].StopSec;

        if( TimeDiff < 20 && TimeDiff > 0 )
            LaunchScript( PIndex, PROCESS_STOP, FALSE );
    }
}
//--------------------------------------------------------------------------------------------------------------------
void StartStopProcess( BOOL ForceStart, BOOL FirstTime )
{
    Int32 TimeNow = UFC::GetSecondsToday();
    UFC::AnsiString ShellCMD;

    for( int i = 0; i < MONOTER_PROCESS; i++ )
    {
        if( MoniterProcess[i].IsAuto == TRUE && MoniterProcess[i].IsRemote == FALSE )
        {            
            ////< Start time < Stop Time, Normal case. (exp: 06:00 start --> 16:00 stop)            
            if( MoniterProcess[i].StartSec < MoniterProcess[i].StopSec )
            {
                if( TimeNow > MoniterProcess[i].StartSec && TimeNow < MoniterProcess[i].StopSec  )
                {
                    CheckProcessStart( i, ForceStart, TimeNow );///< This process should be running.
                    if( FirstTime == TRUE && MoniterProcess[i].SleepSec > 0)
                    {
                        UFC::SleepMS( MoniterProcess[i].SleepSec * 1000 );
                        UFC::BufferedLog::Printf(" Sleep %d sec.", MoniterProcess[i].SleepSec );
                    }
                }
                else 
                    CheckProcessStop( i, TimeNow );///< This process should be stopped.
            }
            else////< Start time > Stop Time, Over on day case. (exp: 22:00 start --> 08:00 stop
            {
                if( TimeNow > MoniterProcess[i].StartSec || TimeNow < MoniterProcess[i].StopSec  )
                {
                    CheckProcessStart( i, ForceStart, TimeNow );///< This process should be running.
                    if( FirstTime == TRUE && MoniterProcess[i].SleepSec > 0 )
                    {
                        UFC::SleepMS( MoniterProcess[i].SleepSec * 1000 );
                        UFC::BufferedLog::Printf(" Sleep %d sec.", MoniterProcess[i].SleepSec );
                    }
                }
                else ///< This process should be stopped.
                    CheckProcessStop( i, TimeNow );///< This process should be stopped.
            }
        }
     }    
    for( int i = 0; i < MONOTER_OTHER_PROCESS; i++ )
    {
        if( MoniterOtherProcess[i].IsAuto == TRUE  ) ///< SpeedyAgent control the process Start/Stop
        {            
            ////< Start time < Stop Time, Normal case. (exp: 06:00 start --> 16:00 stop)
            if( MoniterOtherProcess[i].StartSec < MoniterOtherProcess[i].StopSec )
            {
                if( TimeNow > MoniterOtherProcess[i].StartSec && TimeNow < MoniterOtherProcess[i].StopSec  )
                    CheckOtherProcessStart( i, TimeNow );///< This process should be running.
                else 
                    CheckOtherProcessStop( i, TimeNow ); ///< This process should be stopped.
            }
            else ////< Start time > Stop Time, Over on day case. (exp: 22:00 start --> 08:00 stop
            {
                if( TimeNow > MoniterOtherProcess[i].StartSec || TimeNow < MoniterOtherProcess[i].StopSec  )
                    CheckOtherProcessStart( i, TimeNow );///< This process should be running.
                else 
                    CheckOtherProcessStop( i, TimeNow ); ///< This process should be stopped.
            }
        }
     }    
}
//----------------------------------------------------------------------------------------------------------------------
BOOL ValidateSetting( char* Line  )
{
    if( strlen( Line ) < 18 )
        return FALSE;
    if( strstr( Line, "#") != NULL )
        return FALSE;
    return TRUE;
}
//----------------------------------------------------------------------------------------------------------------------
void GetFIXSetting( MTree& ReplyData )
{
    try
    {
        ///< Add FIX SPEC files into reply data.
        UFC::PStringList FIXSPECFiles;
        UFC::AnsiString  Name;
        UFC::AnsiString  FIXSPECDir,FIXCFGFile;
        
        FIXSPECDir.Printf( "%s/%s", CurrentDir.c_str() , FIXSPEC_DIR );                        
        FIXCFGFile.Printf( "%s/%s", CurrentDir.c_str() , FIX_CFG_FILE );                                        
        Int32 FetchCount = FileList( FIXSPECDir, ".xml", FIXSPECFiles );

        UFC::BufferedLog::Printf(" %d FIX SPEC. dictionary found.", FetchCount );
        ReplyData.append( "COUNT", FetchCount );
        for( int i = 0; i < FetchCount; i++ )
        {
             Name.Printf( "%d", i );
             ReplyData.append( Name, FIXSPECFiles.GetItem(i) );
        }
        ///< Add FIX Engine config files into reply data.
        UFC::FileStream iniFile( FIXCFGFile, O_RDONLY );
        Int32           FileSize  = iniFile.GetSize();
        unsigned char*  buffer    = new unsigned char[ FileSize ];
        Int32           ReadSize  = iniFile.Read( buffer, FileSize );
        ReplyData.append( "FILE", buffer, ReadSize );
        delete [] buffer;
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" FIX session setting file \"imFIX.cfg\" not found");
    }
}
//----------------------------------------------------------------------------------------------------------------------
void GetProcessSetting( MTree& ReplyData )
{
    try
    {
        UFC::AnsiString ProcessCFGFile;
        
        ProcessCFGFile.Printf( "%s/%s", CurrentDir.c_str(), PROCESS_CFG );                        
        UFC::FileStream iniFile( ProcessCFGFile, O_RDONLY );
        Int32           DataSize = iniFile.GetSize();
        unsigned char*  buffer   = new unsigned char[ DataSize ];
        int             ReadSize = iniFile.Read( buffer, DataSize );
        ReplyData.append( "FILE", buffer, ReadSize );
        delete [] buffer;
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" Process setting file [Speedy.cfg] not found");
    }
#ifndef _WIN32
    try
    {
        UFC::AnsiString OtherProcessCFGFile;
        
        OtherProcessCFGFile.Printf( "%s/%s", CurrentDir.c_str() , OTHER_PROCESS_CFG );                            
        UFC::FileStream iniFile( OtherProcessCFGFile, O_RDONLY );
        Int32           DataSize = iniFile.GetSize();
        unsigned char*  buffer   = new unsigned char[ DataSize ];
        int             ReadSize = iniFile.Read( buffer, DataSize );
        ReplyData.append( "OTHER", buffer, ReadSize );
        delete [] buffer;
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" Monitoring processes list [%s] not found",OTHER_PROCESS_CFG );        
        UFC::BufferedLog::Printf(" Do not need monitoring other processes." );
    }
#endif
}
//----------------------------------------------------------------------------------------------------------------------
void GetConfigFile( MTree* Data, MTree& ReplyData )
{
    UFC::AnsiString FileName;

    if( Data->get( "FILENAME", FileName ) == TRUE )
    {
        UFC::AnsiString CFGFile;
        
        CFGFile.Printf( "%s/../cfg/%s", CurrentDir.c_str() , FileName.c_str() );                
        try
        {
            Int32 Compress;
            UFC::FileStream iniFile( CFGFile, O_RDONLY );
            Int32           DataSize = iniFile.GetSize();
            unsigned char*  buffer;
            int             ReadSize;
            
            if( DataSize == 0 )
            {
                buffer   = new unsigned char[ 16 ];
                ReadSize = 16;
                memset( buffer, ' ', 16 );
                UFC::BufferedLog::Printf(" Config file \"%s\" is an empty file.", FileName.c_str());
            }
            else
            {
                buffer   = new unsigned char[ DataSize ];
                ReadSize = iniFile.Read( buffer, DataSize );
            }                               
            if(  Data->get( "ZIP", Compress ) == TRUE &&  Compress == 1 )
                 ReplyData.append( "FILE", buffer, ReadSize, TRUE );
            else
                 ReplyData.append( "FILE", buffer, ReadSize );
            ReplyData.append( "FILENAME", FileName );
            delete [] buffer;
            UFC::BufferedLog::Printf(" Send config file[%s] size[%d] to SpeedyCenter.", FileName.c_str(), DataSize );
        }
        catch(...)
        {
            UFC::BufferedLog::Printf(" Config file \"%s\" not found", FileName.c_str());
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void CancelAll( MTree* Data, BOOL IsFut )
{
    UFC::AnsiString ShellCMD,Account;
    char            Market;
    Int32           IsOffHour = FALSE;
    
    Data->get( "OFFHOUR", IsOffHour );
    if( IsFut == TRUE )
        Market = 'F';
    else            
        Market = 'O';
    if( IsOffHour == FALSE )
    {
        if( Data->get( "Account", Account) == TRUE && Account.Length() == 7 ) ///< Cancel by account
            ShellCMD.Printf("%s/SpeedyCancelAll -M%c -A%s",CurrentDir.c_str(), Market, Account.c_str() );
        else ///< Cancel All
            ShellCMD.Printf("%s/SpeedyCancelAll -M%c",CurrentDir.c_str(), Market );
    }
    else
    {
        if( Data->get( "Account", Account) == TRUE && Account.Length() == 7 ) ///< Cancel by account
            ShellCMD.Printf("%s/SpeedyCancelAll -M%c -A%s -OFFHOUR",CurrentDir.c_str(), Market, Account.c_str() );
        else ///< Cancel All
            ShellCMD.Printf("%s/SpeedyCancelAll -M%c -OFFHOUR",CurrentDir.c_str(), Market );
    }
    UFC::BufferedLog::Printf(" Execute CMD:%s", ShellCMD.c_str() );
    system( ShellCMD.c_str() );
}
//----------------------------------------------------------------------------------------------------------------------
void SaveConfigFile( MTree* Data , MTree& ReplyData )
{
    char* Buffer;
    int   Size;
    UFC::AnsiString FileName;

    if( Data->get( "FILENAME", FileName ) == TRUE  && Data->get( "FILE", Buffer, Size ) )
    {
       UFC::AnsiString CFGFile;       
       
       try
       {
           CFGFile.Printf( "%s/../cfg/%s", CurrentDir.c_str() , FileName.c_str() );                
           UFC::FileStream cfgfile( CFGFile, O_CREAT|O_TRUNC|O_WRONLY );
           cfgfile.Write( Buffer, Size );
           cfgfile.Flush();
           ReplyData.append( "FILENAME", FileName );
           ReplyData.append( "HOST", UFC::Hostname );
           MTree OutData;
           OutData.append( "FILENAME", FileName );
           MessageObj->Send( "FILE.CHANGED", UFC::Hostname, OutData, FALSE );
       }
       catch(...)
       {
            UFC::BufferedLog::Printf(" Write file \"%s\" failed.", CFGFile.c_str());
       }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void GetList( MTree& ReplyData )
{
    int   FetchCount;
    char  Buffer[ MAX_LINE_BUFFER ];
    FILE* FFileHandle;
    UFC::AnsiString Field;
    UFC::AnsiString PVCCfgFileName;        
    
    PVCCfgFileName.Printf( "%s/../cfg/SpeedyPVCFut.cfg", CurrentDir.c_str() );                
    if( (FFileHandle = fopen( PVCCfgFileName.c_str(), "r" )) != NULL )
    {
        FetchCount = 0;
        while( fgets( Buffer, MAX_LINE_BUFFER, FFileHandle ) != NULL )
        {
            if( ValidateSetting( Buffer ))
            {
                FetchCount++;
                Field.Printf( "PVC_FUT%d",FetchCount );
                ReplyData.append( Field, Buffer );
            }
        }
        ReplyData.append( "COUNT_FUT", FetchCount );
        UFC::BufferedLog::Printf( " FUT:%d Lines.",FetchCount );
        fclose( FFileHandle );
    }
    PVCCfgFileName.Printf( "%s/../cfg/SpeedyPVCOpt.cfg", CurrentDir.c_str() );                
    if( (FFileHandle = fopen( PVCCfgFileName.c_str(), "r" )) != NULL )
    {
        FetchCount = 0;
        while( fgets( Buffer, MAX_LINE_BUFFER, FFileHandle ) != NULL )
        {
            if( ValidateSetting( Buffer ))
            {
                FetchCount++;
                Field.Printf( "PVC_OPT%d",FetchCount );
                ReplyData.append( Field, Buffer );
            }
        }
        ReplyData.append( "COUNT_OPT", FetchCount );
        UFC::BufferedLog::Printf( " OPT:%d Lines.",FetchCount );
        fclose( FFileHandle );
    }
    PVCCfgFileName.Printf( "%s/../cfg/SpeedyPVCTSE.cfg", CurrentDir.c_str() );                    
    if( (FFileHandle = fopen( PVCCfgFileName.c_str() , "r" )) != NULL )
    {
        FetchCount = 0;
        while( fgets( Buffer, MAX_LINE_BUFFER, FFileHandle ) != NULL )
        {
            if( ValidateSetting( Buffer ))
            {
                FetchCount++;
                Field.Printf( "PVC_TSE%d",FetchCount );
                ReplyData.append( Field, Buffer );
            }
        }
        ReplyData.append( "COUNT_TSE", FetchCount );
        UFC::BufferedLog::Printf( " TSE:%d Lines.",FetchCount );
        fclose( FFileHandle );
    }
    PVCCfgFileName.Printf( "%s/../cfg/SpeedyPVCOTC.cfg", CurrentDir.c_str() );                    
    if( (FFileHandle = fopen( PVCCfgFileName.c_str() , "r" )) != NULL )
    {
        FetchCount = 0;
        while( fgets( Buffer, MAX_LINE_BUFFER, FFileHandle ) != NULL )
        {
            if( ValidateSetting( Buffer ))
            {
                FetchCount++;
                Field.Printf( "PVC_OTC%d",FetchCount );
                ReplyData.append( Field, Buffer );
            }
        }
        ReplyData.append( "COUNT_OTC", FetchCount );
        UFC::BufferedLog::Printf( " OTC:%d Lines.",FetchCount );
        fclose( FFileHandle );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SaveNewPVCCfg( MTree* Data )
{
    char* Buffer;
    int   Size;
    UFC::AnsiString PVCCfgFileName;        
    
    try
    {
        if( Data->get( "OPT", Buffer, Size ) )
        {
            PVCCfgFileName.Printf( "%s/../cfg/SpeedyPVCOpt.cfg", CurrentDir.c_str() );            
            UFC::FileStream cfgfile( PVCCfgFileName, O_CREAT|O_TRUNC|O_WRONLY );
            cfgfile.Write( Buffer, Size );
        }
        if( Data->get( "FUT", Buffer, Size ) )
        {
            PVCCfgFileName.Printf( "%s/../cfg/SpeedyPVCFut.cfg", CurrentDir.c_str() );            
            UFC::FileStream cfgfile( PVCCfgFileName, O_CREAT|O_TRUNC|O_WRONLY );
            cfgfile.Write( Buffer, Size );
        }
        if( Data->get( "TSE", Buffer, Size ) )
        {
            PVCCfgFileName.Printf( "%s/../cfg/SpeedyPVCTSE.cfg", CurrentDir.c_str() );            
            UFC::FileStream cfgfile( PVCCfgFileName, O_CREAT|O_TRUNC|O_WRONLY );
            cfgfile.Write( Buffer, Size );
        }
        if( Data->get( "OTC", Buffer, Size ) )
        {
            PVCCfgFileName.Printf( "%s/../cfg/SpeedyPVCOTC.cfg", CurrentDir.c_str() );            
            UFC::FileStream cfgfile( PVCCfgFileName, O_CREAT|O_TRUNC|O_WRONLY );
            cfgfile.Write( Buffer, Size );
        }
    }
    catch(...)
    {
         UFC::BufferedLog::Printf(" Write file \"%s\" failed.", PVCCfgFileName.c_str());
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SaveNewFIXSetting( MTree* Data )
{
    char* Buffer;
    int   Size;
    if( Data->get( "FIX", Buffer, Size ) )
    {
        UFC::AnsiString FIXCfgFileName;
        try
        {
            FIXCfgFileName.Printf( "%s/%s", CurrentDir.c_str(), FIX_CFG_FILE );        
            UFC::FileStream cfgfile(  FIXCfgFileName ,O_CREAT|O_TRUNC|O_WRONLY );
            cfgfile.Write( Buffer, Size );
        }
        catch(...)
        {
            UFC::BufferedLog::Printf(" Write file \"%s\" failed.", FIXCfgFileName.c_str());
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SaveProcessSetting( MTree* Data )
{
    char* Buffer;
    int   Size;

    if( Data->get( "FILE", Buffer, Size ) )
    {
       UFC::AnsiString ProcessSettingFileName;
        
       try
       {
            ProcessSettingFileName.Printf( "%s/%s", CurrentDir.c_str(), PROCESS_CFG );
            UFC::FileStream cfgfile( ProcessSettingFileName , O_CREAT|O_TRUNC|O_WRONLY );
            cfgfile.Write( Buffer, Size );
       }
       catch(...)
       {
            UFC::BufferedLog::Printf(" Write file \"%s\" failed.", ProcessSettingFileName.c_str());
       }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void ControlProcess( MTree* Data, int Action )
{
    Int32 Index,MBusApp;
    
    if( Data->get( "INDEX", Index ) )
    {
        if( Data->get( "MBusApp", MBusApp ) )
        {
            if( MBusApp == 1 )
                LaunchScript( Index, Action, TRUE );
            else
            {
                UFC::BufferedLog::Printf(" Control other process");
                LaunchScript( Index, Action, FALSE );
            }
        }
        else
            LaunchScript( Index, Action, TRUE  );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void GetProcessStat( MTree& ReplyData )
 {
    Int32  Process, OtherProcess;
    
    if( MONOTER_PROCESS > 32 ) 
    {
        Process = 0;
        for( int i = 0; i< 32; i++ )
            Process |= (MoniterProcess[i].IsRunning << i );
        ReplyData.append( "PROCESS", Process );
        Process = 0;
        for( int i = 32; i< MONOTER_PROCESS; i++ )
            Process |= (MoniterProcess[i].IsRunning << ( i - 32 ) );
        ReplyData.append( "PROCESS1", Process );
    }
    else 
    {
        Process = 0;
        for( int i = 0; i< MONOTER_PROCESS;i++ )
            Process |= (MoniterProcess[i].IsRunning << i );
        ReplyData.append( "PROCESS", Process );
    }
#ifndef _WIN32
    if( MONOTER_OTHER_PROCESS > 32 ) 
    {
        OtherProcess = 0;
        for( int i = 0; i< 32;i++ )
            OtherProcess |= (MoniterOtherProcess[i].IsRunning << i );
        ReplyData.append( "OTHERS", OtherProcess );
        OtherProcess = 0;
        for( int i = 32; i< MONOTER_OTHER_PROCESS;i++ )
            OtherProcess |= (MoniterOtherProcess[i].IsRunning << (i -32 ) );
        ReplyData.append( "OTHERS1", OtherProcess );
    }
    else
    {
        OtherProcess = 0;
        for( int i = 0; i< MONOTER_OTHER_PROCESS;i++ )
            OtherProcess |= (MoniterOtherProcess[i].IsRunning << i );
        ReplyData.append( "OTHERS", OtherProcess );
    }
#endif
}
//----------------------------------------------------------------------------------------------------------------------    
//  0:Ver:2.0.4 Build Date:Sep 28 2009
//  1:License expired:20091010
//  2:Startup:2009/09/29 08:35:40
//  3:Start with clear flag: Yes
//----------------------------------------------------------------------------------------------------------------------    
void CheckSystemListener::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    MTree           ReplyData;
    UFC::AnsiString Value,ReplyKey;
    UFC::UDateTime  Now;    

    if( Data->get("ReplyKey",ReplyKey) == TRUE  )
    {
        Now.setCurrent();
        ReplyData.append( "Name", "Speedy Agent" );
        ReplyData.append( "COUNT", 6 );
        Value.Printf( "inf,%s", VersionBuidDate.c_str() );
        ReplyData.append( "0", Value );
        Value.Printf( "inf,License expired:%s", LicenseExpDate.c_str() );
        ReplyData.append( "1", Value );
        if( Now.getYear()  != StartupTime.getYear() ||
            Now.getMonth() != StartupTime.getMonth() ||
            Now.getDate()  != StartupTime.getDate() )
            Value.Printf( "war,Startup:%04d/%02d/%02d %02d:%02d:%02d", StartupTime.getYear(), StartupTime.getMonth(),StartupTime.getDate(),
                                                                       StartupTime.getHour(), StartupTime.getMinute(), StartupTime.getSecond() );
        else
            Value.Printf( "inf,Startup:%04d/%02d/%02d %02d:%02d:%02d", StartupTime.getYear(), StartupTime.getMonth(),StartupTime.getDate(),
                                                                       StartupTime.getHour(), StartupTime.getMinute(), StartupTime.getSecond() );
        ReplyData.append( "2", Value );        
        ReplyData.append( "3", "inf,Speedy Manager " );        
        Value.Printf( "inf,Password need change every %d days", Manager->PasswordChangeDays( ) );
        ReplyData.append( "4", Value );        
        Value.Printf( "inf,Change password hint %d days before expired.", Manager->PasswordChangeHintDays( ) );
        ReplyData.append( "5", Value );                
        MessageObj->Send( "CHECK.REPLY", ReplyKey, ReplyData, FALSE );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void PrintActionLog( const UFC::AnsiString& User, const UFC::AnsiString& Message )
{
    UFC::PStringList Items;
    
    Items.SetStrings( User, "@_\n" );
    if( Items.ItemCount() >= 2 )
        UFC::BufferedLog::Printf( " *User[%s] use PC[%s] Action[%s]", Items[0].c_str(), Items[1].c_str(), Message.c_str() );    
    else
        UFC::BufferedLog::Printf( " %s Action[%s]", User.c_str(), Message.c_str() );    
}
//----------------------------------------------------------------------------------------------------------------------
void AgentCommandListener::OnMigoMessage( const UFC::AnsiString& ,const UFC::AnsiString& Key, MTree* Data )
{
     UFC::AnsiString ShellCMD,Host;
     MTree           ReplyData;
     Int32           CMD;

     if( Data->get( "COMMAND", CMD ) )
     {
         if( CMD == SPEEDY_AGENT_ACK )
         {
             if( Data->get( "HOST", Host ) == FALSE || Host == UFC::Hostname )
             {
                 ReplyData.append( "COMMAND", SPEEDY_AGENT_REPLY);
                 ReplyData.append( "HOST", UFC::Hostname  );
                 ReplyData.append( "VER",  SpeedyAgentVer );
                 ReplyData.append( "Date", LicenseExpDate );
                 #ifdef __SOLARIS
                    ReplyData.append( "TYPE", 1 );                 
                 #endif
                 #ifdef __AIX
                    ReplyData.append( "TYPE", 2 );                 
                 #endif
                 #ifdef _WIN32
                    ReplyData.append( "TYPE", 3 );                 
                 #endif
                 #ifdef __LINUX
                    ReplyData.append( "TYPE", 4 );                 
                 #endif
                 MessageObj->Send( "SPEEDY.AGENT", Key, ReplyData, FALSE );
             }
         }
         else if( Data->get( "HOST", Host ) && Host == UFC::Hostname )
         {
             switch( CMD )
             {
                case SPEEDY_MODIFY_PVC:         PrintActionLog( Key, "Modify Session Setting" );
                                                SaveNewPVCCfg( Data );break;
                case SPEEDY_RUN_PROCESS:        PrintActionLog( Key, "Start Process" );
                                                ControlProcess( Data, PROCESS_START );break;
                case SPEEDY_KILL_PROCESS:       CurrentClientKey = Key;
                                                PrintActionLog( Key, "Stop Process" );
                                                ControlProcess( Data, PROCESS_STOP );
                                                break;
                case SPEEDY_RESTART_PROCESS:    PrintActionLog( Key, "Re-Start Process" );
                                                CurrentClientKey = Key;
                                                ControlProcess( Data, PROCESS_RESTART );
                                                break;
                case SPEEDY_RUNCLEAR_PROCESS:   CurrentClientKey = Key;
                                                ControlProcess( Data, PROCESS_STARTCLEAR );
                                                break;
                case SPEEDY_MODIFY_PROCESS:     SaveProcessSetting(Data);break;
                case SPEEDY_CHANGE_FIX_SETTING: SaveNewFIXSetting( Data );break;
                case SPEEDY_MONITOR_PVC:        new ParseLogThread( Data, Key ); break;                
                case SPEEDY_GET_PROCESS:        ReplyData.append( "COMMAND", SPEEDY_REPY_PROCESS );
                                                GetProcessStat( ReplyData );
                                                MessageObj->Send( "SPEEDY.AGENT", Key, ReplyData, FALSE );
                                                break;
                case SPEEDY_GET_LIST:           ReplyData.append( "COMMAND", SPEEDY_REPY_LIST );
                                                GetList( ReplyData );
                                                MessageObj->Send( "SPEEDY.AGENT", Key, ReplyData, FALSE );
                                                break;                
                case SPEEDY_GET_FIX_SETTING:    ReplyData.append( "COMMAND", SPEEDY_REPLY_FIX_SETTING );
                                                GetFIXSetting( ReplyData );
                                                MessageObj->Send( "SPEEDY.AGENT", Key, ReplyData, FALSE );
                                                break;
                case SPEEDY_GET_PROCESS_LIST:   ReplyData.append( "COMMAND", SPEEDY_REPLY_PROCESS_LIST );
                                                GetProcessSetting( ReplyData );
                                                MessageObj->Send( "SPEEDY.AGENT", Key, ReplyData, FALSE );
                                                break;
                case SPEEDY_GET_CONFIG_FILE:    ReplyData.append( "COMMAND", SPEEDY_REPLY_CONFIG_FILE );
                                                GetConfigFile( Data , ReplyData);
                                                MessageObj->Send( "SPEEDY.AGENT", Key, ReplyData, FALSE );
                                                break;
                case SPEEDY_SET_CONFIG_FILE:    PrintActionLog( Key, "Modify config file" );
                                                ReplyData.append( "COMMAND", SPEEDY_CONFIG_FILE_SAVED );
                                                SaveConfigFile( Data , ReplyData );
                                                MessageObj->Send( "SPEEDY.AGENT", Key, ReplyData, FALSE );
                                                break;
                case SPEEDY_CANCEL_FUT_ORDERS:  PrintActionLog( Key, "Cancel all Futures orders" );
                                                CancelAll( Data, TRUE );break;
                case SPEEDY_CANCEL_OPT_ORDERS:  PrintActionLog( Key, "Cancel all Options orders" );
                                                CancelAll( Data, FALSE );break;
             }
         }
     }
}
//------------------------------------------------------------------------------
void WaitForConnected()///< Wait for MBus connected.
{
    for( int i=0;i<30;i++)
    {
        if( MessageObj->IsConnected() == TRUE )
            return;
          UFC::SleepMS( 100 );
    }
    printf( "____________________________________________________\n\n" );
    printf( "  MBus daemon is not running.\n");
    printf( "  Run StartSpeedy or StartSpeedyClear script first.\n");
    printf( "____________________________________________________\n" );
    exit( 0 );
}
//------------------------------------------------------------------------------
void PrintMenu( const char* Message, Int32 CMD )
{
    char inputStr[128];

    MessageObj = new MessageObject( "Speedy Utility", "1.0", "XXXXX" );
    MessageObj->Start();
    WaitForConnected();
    while( TRUE )
    {
        printf( "_______________________________________________\n\n" );
        printf( "%s:\n\n", Message );
        for( int i = 0; i < MONOTER_PROCESS; i++ )
                 printf( "%d, %s\n", i, MoniterProcess[i].ProcessName );
        printf( "a, all programs\n" );
        printf( "x, exit\n" );
        printf( "_______________________________________________\n" );
        scanf("%s",inputStr);

        if( inputStr[0] == 'a' ) ///< Start/Stop/Restart all
        {
            for( int i = 0; i < MONOTER_PROCESS; i++ )
            {
                MTree Data;

                Data.append( "HOST", UFC::Hostname );
                Data.append( "COMMAND", CMD );
                Data.append( "INDEX" , i );
                MessageObj->Send( "SPEEDY.AGENT", UFC::Hostname, Data, FALSE );
            }
            break;
        }
        else if( inputStr[0] == 'x' )
        {
            break;
        }
        else
        {
            int index = atoi( inputStr );
            MTree Data;

            Data.append( "HOST", UFC::Hostname );
            Data.append( "COMMAND", CMD );
            Data.append( "INDEX" , index );
            MessageObj->Send( "SPEEDY.AGENT", UFC::Hostname, Data, FALSE );
        }
    }
    for( int i = 0; i < 10; i++ )
	 UFC::SleepMS( 100 );
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char * argv[] )
{
    for( int i = 1; i < argc; i++ )
    {
        UFC::AnsiString Param( argv[i] );

        if( Param == "-H" || Param == "-h" || Param == "-?" ) ///< Help screen
        {
            printf( "SpeedyAgent Copyright (c) 2003~%d MDBS, Build Date:%s %s\n", UFC::ThisYear(), __DATE__, __TIME__);
            printf( " %d bit mode.\n", (int)sizeof(void*)*8 );                    
            printf( "Usage: SpeedyAgent [options]\n" );
            printf( "   -H         Show this page.\n" );
            printf( "   -R         Run a Speedy process.\n" );
            printf( "   -RC        Run a Speedy process and clear all data.\n" );
            printf( "   -T         Terminate a Speedy process.\n" );
            printf( "   -Clear     Clear all exist data.\n" );
            printf( "   -StartAll  Start all Speedy processes.\n" );
            printf( "   -StopAll   Stop all Speedy processes.\n" );
            exit( 1 );
        }        
        else if( Param.AnsiPos("-Clear") != -1 )
        {
            CLEAR = TRUE;
        }
        else if( Param == "-R" ) ///< Run a single process.
        {
            PrintMenu( "Select the program you want to run", SPEEDY_RUN_PROCESS );
            exit( 0 );
        }
        else if( Param == "-RC" ) ///< Run and Clear a single process.
        {
            PrintMenu( "Select the program you want to clear data and run", SPEEDY_RUNCLEAR_PROCESS );
            exit( 0 );
        }
        else if( Param == "-T" ) ///< Terminate a single process.
        {
            PrintMenu( "Select the program you want to terminate", SPEEDY_KILL_PROCESS );
            exit( 0 );
        }
        else if( Param.AnsiPos("-StartAll") != -1 )
        {
            START_ALL = TRUE;
            UFC::ClearErrorLog();
        }
        else if( Param.AnsiPos("-KillAll") != -1 )
        {
            UFC::AnsiString cmdline,ShellCMD;
            pid_t killPID;
            
            for( int c = 0; c < 5; c++ )
            {
                for( int i = 0; i < MONOTER_PROCESS; i++ ) ///< Stop all other speedy processes.
                {
                    cmdline.Printf("%s %s", MoniterProcess[i].BinaryName, MoniterProcess[i].Param );
                    killPID = GetProcessId( cmdline.c_str() );
                    if( killPID != -1 )
                    {
                        printf( " Stop process [%s] pid:%d\n", cmdline.c_str(), (int)killPID  );
                        ShellCMD.Printf("kill -9 %d", killPID);
                        system( ShellCMD.c_str() );
                        UFC::SleepMS( 50 );
                    }
                }                
                killPID = GetProcessId( "MBus" );
                if( killPID != -1 )
                {
                    printf( " Stop process [MBus] pid:%d\n", (int) killPID );
                    ShellCMD.Printf("kill -9 %d", killPID);
                    system( ShellCMD.c_str() );
                    UFC::SleepMS( 50 );
                }
                UFC::SleepMS( 100 );
            }
            exit( 0 ); 
        }
        else if( Param.AnsiPos("-StopAll") != -1 )
        {
            MessageObj = new MessageObject( "Speedy Utility", "1.0", "XXXXX" );
            MessageObj->SetMonitorListener( new ProcessMonitor() );        
            MessageObj->Start(); ///< Start Migo Message pump.    
            if( ProcessExistEventPtr->WaitFor( 5 ) == TRUE )  //Modified by Zhen Fan 2021/08/30
            {    
                //Stop( "SpeedyManager" );                   ///< Stop SpeedyManager
                UFC::SleepMS( 2000 );
                for( int i = 0; i < MONOTER_PROCESS; i++ ) ///< Stop all other speedy processes.
                {
                    if( MoniterProcess[i].IsAuto == TRUE && MoniterProcess[i].IsRemote == FALSE )
                    {
                        MTree Data;

                        Data.append( "HOST", UFC::Hostname );
                        Data.append( "COMMAND", SPEEDY_KILL_PROCESS );
                        Data.append( "INDEX" , i );
                        MessageObj->Send( "SPEEDY.AGENT", UFC::Hostname, Data, FALSE );
                        printf( " Stop process [%s].\n", MoniterProcess[i].ProcessName );
                        UFC::SleepMS( 50 );
                    }
                }
                printf( "____________________________________________________\n\n" );
                printf( "  All MBus applications stopped.\n");
                printf( "  Try to stop the MBus daemon.\n");
                printf( "____________________________________________________\n" );
                for( int i = 0; i < 50; i++ )
                     UFC::SleepMS( 100 );
                Stop( "MBus", true  );                  ///< Stop MBus daemon.
            }
            else
            {
                printf( "____________________________________________________\n\n" );
                printf( "  MBus daemon is not running.\n");
                printf( "  Run StartSpeedy or StartSpeedyClear script first.\n");
                printf( "____________________________________________________\n" );                
                Stop( "MBus", true );                  ///< Stop MBus daemon.
            }
            for( int i = 0; i < 50; i++ )
                 UFC::SleepMS( 10 );
            exit( 0 );                       ///< Stop self.( SpeedyAgent )
        }               

    }
}
//------------------------------------------------------------------------------
void CheckLicenseKey( void )
{
    if( !(UFC::FileExists("License")) )
    {
	printf( "License file not found.\n" );
	exit(1);
    }    	
    UFC::AnsiString Today, Plat, Key;
    UFC::UiniFile iniFile( "License" );
    iniFile.GetValue( "MBus", "Date", LicenseExpDate );
    iniFile.GetValue( "MBus", "Key", Key );
#ifdef _WIN32
	Plat = "WIN32";
#endif
#ifdef __SOLARIS
	Plat = "SOLARIS";
#endif
#ifdef __AIX
	Plat = "AIX";
#endif
#ifdef __LINUX
	Plat = "LINUX";
#endif
    if( (LicenseExpDate.Length() > 0) && (Key.Length() > 0)  )
    {
	UFC::GetYYYYMMDD( Today );
	if( LicenseExpDate.ToInt() >= Today.ToInt() )
    	{
            UFC::TLicenseKey LicenseKey( Plat, "MBus", UFC::Hostname, LicenseExpDate );
            if( LicenseKey.CompareKey( Key ) )
                return;
            else
                printf( "Bad license key.\n" );
    	}
    	else
            printf( "License expired.\n" );
    }
    else
    	printf( "Incorrect license format.\n" );    
    exit(1);
}
//--------------------------------------------------------------------------------------------------------------------
BOOL CheckConfigModifiaction( void )
{
    static time_t ModifyTime = 0;
    struct stat FileStat;
    
    stat( PROCESS_CFG, &FileStat );    
    if( ModifyTime == 0 ) ///< First time get modify time.   
    {        
        ModifyTime = FileStat.st_mtime;
        return FALSE;
    }
    else
    {
        if( ModifyTime != FileStat.st_mtime )
        {
            ModifyTime = FileStat.st_mtime;
            return TRUE;    
        }
        return FALSE;
    }
}
//--------------------------------------------------------------------------------------------------------------------
BOOL CheckOtherProcessModifiaction( void )
{
    static time_t ModifyTime = 0;
    struct stat FileStat;
    
    if( UFC::FileExists( OTHER_PROCESS_CFG ) == FALSE )
        return FALSE;
    stat( OTHER_PROCESS_CFG , &FileStat );    
    if( ModifyTime == 0 ) ///< First time get modify time.
    {        
        ModifyTime = FileStat.st_mtime;
        return FALSE;
    }
    else
    {
        if( ModifyTime != FileStat.st_mtime )
        {
            ModifyTime = FileStat.st_mtime;
            return TRUE;    
        }
        return FALSE;
    }
}
//--------------------------------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    ProcessExistEventPtr = new UFC::PEvent();  //Modified by Zhen Fan 2021/08/30
    Int32 Count = 0;
        
    CheckLicenseKey();
    GetCurrentDir();           
    GetProcesses();            ///< Load MBus process info from Speedy.cfg
    GetOtherProcesses();       ///< Load other process info from OtherProcess.cfg
    CheckArgs( argc, argv );   ///< Parse command line arguments.
    Start( "MBus","MBus", " ", FALSE );
    UFC::BufferedLog::Printf( " Wait for MBus startup..." );
    StartupTime.setCurrent();
    UFC::SleepMS( 3000 );
    if( UFC::FileExists( "SpeedyAgent.que" ) )
        UFC::RemoveFile( "SpeedyAgent.que" );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( "SpeedyAgent Log", "log", "SpeedyAgent", "log", 10240, TRUE ) );    
    VersionBuidDate.Printf( "Version:%s Build Date:%s %s", SPEEDY_AGENT_VER, __DATE__, __TIME__ );
    UFC::BufferedLog::Printf( "____________________________________________" );
    UFC::BufferedLog::Printf( "                                            " );
    UFC::BufferedLog::Printf( "   SpeedyAgent, Process monitoring daemon   " );
    UFC::BufferedLog::Printf( "   Copyright 2003~%d by MDBS Software Inc.", UFC::ThisYear());
    UFC::BufferedLog::Printf( "   startup at %s.                           ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                            " );
    UFC::BufferedLog::Printf( "   %s ", VersionBuidDate.c_str() );
    UFC::BufferedLog::Printf( "   %d bit mode                          ", sizeof(void*)*8 );        
    UFC::BufferedLog::Printf( "____________________________________________" );       
    Manager    = new SpeedyManager( );    
    MessageObj = new MessageObject( "SpeedyAgent", "1.0", "XXXXX" );
    ///< For SpeedyManager
    MessageObj->AddListener( "ADMIN", "all", Manager );
    ///< For SpeedyAgent
    MessageObj->AddListener( "SPEEDY.AGENT", "all", new AgentCommandListener() );    
    MessageObj->AddListener( "SYSTEM.CHECK", UFC::Hostname, new CheckSystemListener() );
    MessageObj->SetMonitorListener( new ProcessMonitor() );        
    MessageObj->Start(); ///< Start Migo Message pump.    
    ProcessExistEventPtr->WaitFor( 30 );  //Modified by Zhen Fan 2021/08/30
    StartStopProcess( TRUE, TRUE );        
    while( TRUE )
    {
       UFC::SleepMS( 1000 );
       Count++;
       if( Count % 3 == 0 ) ///< Ask proocess status every 3 secs.      
           MessageObj->SendProcessListRequest( ); 
       if( Count % 5 == 0 ) ///< Check process state every 5 sec
       {
           StartStopProcess( FALSE, FALSE );
           if( CheckConfigModifiaction() == TRUE )
           {
               UFC::BufferedLog::Printf( "____________________________________________" );
               UFC::BufferedLog::Printf( "   [%s] Modified, reload it.",PROCESS_CFG );       
               GetProcesses();     ///< Load MBus process info from Speedy.cfg
               UFC::BufferedLog::Printf( "____________________________________________" );
           }
           if( CheckOtherProcessModifiaction() == TRUE )
           {
               UFC::BufferedLog::Printf( "____________________________________________" );
               UFC::BufferedLog::Printf( "   [%s] Modified, reload it.",OTHER_PROCESS_CFG );       
               GetOtherProcesses();///< Load other process info from OtherProcess.cfg
               UFC::BufferedLog::Printf( "____________________________________________" );
           }
           UFC::BufferedLog::FlushToFile();///< Force write the screen log to file.
       }
    }
    return 1;
}
//--------------------------------------------------------------------------------------------------------------------


