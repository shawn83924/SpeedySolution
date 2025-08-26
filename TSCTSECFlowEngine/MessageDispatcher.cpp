#include "MessageDispatcher.h"
//---------------------------------------------------------------------------
MessageDispatcher::MessageDispatcher( const UFC::AnsiString& FileName, BOOL FirstTime, BOOL BenchMark,BOOL Test )
:FMessageObject( "TSCTSECFlowEngine", "1.0", "XXXXX" )
,FirstTimeRun( FirstTime )
,FBenchMark( BenchMark)
{
    UFC::UiniFile   Config( FileName );
    int             SectionCount = Config.SectionCount();
    SocketSend*   pConnection;

    for( int i = 0; i<SectionCount; i++ )
    {
        UFC::Section* pSection = Config.GetSection(i);
        Setting*      pSet = new Setting( pSection );
        if( FConnections.GetObjectByKey(pSet->WebID) == NULL )
        {
            UFC::PtrQueue<UFC::AnsiString>*     JobQueue = new UFC::PtrQueue<UFC::AnsiString>();
            pConnection = new SocketSend( pSet, FirstTime, &FMessageObject, JobQueue, Test );
            pConnection->Run();
            FConnections.Add( pSet->WebID, JobQueue );
        }
        else
        {
            pConnection = new SocketSend( pSet, FirstTime, &FMessageObject, FConnections.GetObjectByKey(pSet->WebID), Test);
            pConnection->Run();
        }
    }
    FMessageObject.AddListener( "ORDER", UFC::Hostname, this );
}
//---------------------------------------------------------------------------
void MessageDispatcher::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data )
{
    MString     KeyString;
    Int32       Market;

    //SocketSend* Connection;
    UFC::PtrQueue<UFC::AnsiString>*   FQueue;
    ///< Market = 2 means its an Stock order
    if( Data->get( "EDS", KeyString) == TRUE && Data->get("SYS",Market) == TRUE && Market == 2 )
    {
        UFC::BufferedLog::DebugPrintf(" KeyString = %s", KeyString.c_str());
        ///< Dispatch the event according to the key strring.
        if( ( FQueue = FConnections.GetObjectByKey( KeyString ))!= NULL )
        {
          MString OrderString;

          if( Data->get("ORDER",OrderString) == TRUE )
          {
              bool Mark = FQueue->Inqueue( new UFC::AnsiString(OrderString) );
              UFC::BufferedLog::DebugPrintf(" OnMigoMessage:[%s]", OrderString.c_str());
              EndMessageNo++;
          }
        }
    }
}
//------------------------------------------------------------------------------
void MessageDispatcher::Run( void )
{
    FMessageObject.Start();

    if( FBenchMark )
    {
        Int32 BeginTime = UFC::GetTickCountMS();
        Int32 UseMS;

        sleep(1);
        while( true )
        {
            Int32 EndTime = UFC::GetTickCountMS();
            UseMS =  EndTime - BeginTime;
            UFC::BufferedLog::DebugPrintf( " Message In total = [%d]", EndMessageNo);
            UFC::BufferedLog::Printf( " Message In: %d Message/sec", (EndMessageNo - BeginMessageNo)*1000/UseMS );
            BeginMessageNo = EndMessageNo;
            BeginTime = EndTime;
            sleep(1);
        }
    }

    while( TRUE )
    {
        sleep(1);
    }
}
//------------------------------------------------------------------------------
