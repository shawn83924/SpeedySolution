//------------------------------------------------------------------------------------------------------------------------
#include "SpeedyRCCommandReceiver.h"
#include "../UFC/PQueue.h"
//------------------------------------------------------------------------------------------------------------------------
SpeedyRCCommandReceiver* SpeedyRCCommandReceiver::FInstance = NULL;
PCriticalSection         SpeedyRCCommandReceiver::FCreateCS;
//------------------------------------------------------------------------------------------------------------------------
SpeedyRCCommandReceiver::SpeedyRCCommandReceiver( )
: FChkRC( NULL )
, FRptRC( NULL )
{
}
//------------------------------------------------------------------------------------------------------------------------
SpeedyRCCommandReceiver* SpeedyRCCommandReceiver::GetInstance( )
{
	PLockObject Lock( FCreateCS );
	if ( FInstance == NULL )
        {
		FInstance = new SpeedyRCCommandReceiver( );
                FInstance->Run();
        }
        return FInstance;
}
//------------------------------------------------------------------------------------------------------------------------
void SpeedyRCCommandReceiver::Run( void )
{
	BufferedLog::Printf( " [SpeedyRCCommandReceiver] thread is start up.");
        
        UFC::PQueue ClearCommand;
        if (ClearCommand.Open(COMMAND_KEY, TRUE))
        {            
           ClearCommand.Clear(); 
           BufferedLog::Printf( " [SpeedyRCCommandReceiver] Key=0x%x MType=%d is clear.", COMMAND_KEY, COMMAND_MTYPE  );
        }
        else
           BufferedLog::Printf( " [SpeedyRCCommandReceiver] Key=0x%x MType=%d open fail.", COMMAND_KEY, COMMAND_MTYPE );
	FQueueSubject = new UQueueSubject( COMMAND_KEY, COMMAND_MTYPE, 0666 );
	FQueueSubject->SetQueueListener( this );
	FQueueSubject->Run();
}
//------------------------------------------------------------------------------------------------------------------------
void SpeedyRCCommandReceiver::OnQueueDataArrived( UQueueStruct *QueueData, int Len )
{
    try
    {
	UFC::BufferedLog::Printf( " [SpeedyRCCommandReceiver] Receive Command <%d|%s> (%d)"
                                , QueueData->mtype
                                , QueueData->mtext
                                , Len );

	UFC::AnsiString CommandString( QueueData->mtext );
	CommandString.TrimRight();

	if ( CommandString == "ReLoadRouting" ) ///< Reload RC_ROUTING
	{
		if ( FChkRC != NULL )
			FChkRC->ReLoadRouting( );
		if ( FRptRC != NULL )
			FRptRC->ReLoadRouting( );                
	}
	else if ( CommandString.SubString( 0, 2 ) == "RC" )
	{
		BOOL IsBreakCheck = FALSE;
                BOOL IsBreakReport = FALSE;
                BOOL Pre_IsBreakCheck = FALSE;
                BOOL Pre_IsBreakReport = FALSE;
		if ( FChkRC != NULL )
                {                
                        FChkRC->GetRCCenterBreakState( IsBreakCheck, IsBreakReport );
                        Pre_IsBreakCheck  = IsBreakCheck ;
                        Pre_IsBreakReport = IsBreakReport;
                }
                if ( FRptRC != NULL )
                {                
                        FRptRC->GetRCCenterBreakState( IsBreakCheck, IsBreakReport );
                        Pre_IsBreakCheck  = IsBreakCheck ;
                        Pre_IsBreakReport = IsBreakReport;
                }
		if ( CommandString == "RCBreakCheck" )///< Break Check of RCCenter
			IsBreakCheck = TRUE;
		else if ( CommandString == "RCJoinCheck" ) ///< Join Check of RCCenter
			IsBreakCheck = FALSE;
		else if ( CommandString == "RCBreakReport" )///< Break Report of RCCenter 
			IsBreakReport = TRUE;
		else if ( CommandString == "RCJoinReport" ) ///< Join Report of RCCenter
			IsBreakReport = FALSE;
		else if ( CommandString == "RCJoinBoth" ) ///< Join Report of RCCenter
		{
			IsBreakCheck = FALSE;
			IsBreakReport = FALSE;
		}
		else if ( CommandString == "RCBreakBoth" ) ///< Join Report of RCCenter
		{
			IsBreakCheck = TRUE;
			IsBreakReport = TRUE;
		}

		if ( FChkRC != NULL )
                {
			FChkRC->SetRCCenterBreakState( IsBreakCheck, IsBreakReport );
                        UFC::BufferedLog::Printf( " [SpeedyRCCommandReceiver_Check_RCRoutingManager] (IsBreakCheck=%d, IsBreakReport=%d) => (%d, %d)", Pre_IsBreakCheck, Pre_IsBreakReport, IsBreakCheck, IsBreakReport );                        
                }
		if ( FRptRC != NULL )
                {
			FRptRC->SetRCCenterBreakState( IsBreakCheck, IsBreakReport );
                        UFC::BufferedLog::Printf( " [SpeedyRCCommandReceiver_Report_RCRoutingManager] (IsBreakCheck=%d, IsBreakReport=%d) => (%d, %d)", Pre_IsBreakCheck, Pre_IsBreakReport, IsBreakCheck, IsBreakReport );
                }
	}       
	else
        {
		UFC::BufferedLog::Printf( " [SpeedyRCCommandReceiver] <%d|%s(%d)> is unknow command", QueueData->mtype, QueueData->mtext, Len );
        }
    }
    catch(UFC::Exception & ex)
    {
        UFC::BufferedLog::Printf( " [SpeedyRCCommandReceiver] Exception:%s", ex.what());
    }
}
//------------------------------------------------------------------------------------------------------------------------

