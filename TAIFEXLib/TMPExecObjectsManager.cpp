#include "TMPExecObjectsManager.h"
#include "LinkSubSystem.h"
//-----------------------------------------------------------------------------------------------------------------------
//
//  TAIFEX side( TAIFEXOrderManeger )
//
//  Objects Manager for AP code 8,9 Execution subsystem
//  
//-----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString          IOLogPath;
extern Int32                    AppPrefixIndex;
//-----------------------------------------------------------------------------------------------------------------------
TMPExecManager::TMPExecManager( int Port, const UFC::AnsiString& OCFDate )
:FListenPort( Port ),
 FConnectedCount( 0 )
{
    UFC::AnsiString LogFileName;

    LogFileName.Printf( "%s/t%s.8.%d",IOLogPath.c_str(),OCFDate.c_str(), FListenPort );
    FLog = new LogManager( LogFileName );    
}
//-----------------------------------------------------------------------------------------------------------------------
TMPExecManager::~TMPExecManager()
{
    if( FLog != NULL )		
	delete FLog;
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPExecManager::CreateConnectionObjects( TMPSessions* Sessions )
{
    int Count =  Sessions->GetCount();
    
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex," ---------------------- Create connections ----------------------" );
    for( int i = 0; i < Count ; i++ ) 
    {
        ConnectionParameter cp( FListenPort , Sessions->GetSessionID( i ), '8', Sessions->GetBrokerID(i), Sessions->GetPassword(i)
                                ,Sessions->GetPeerIP( i ), TMP_HEARTBT_INT, Sessions->GetThroughput(i), Sessions->FTPNotify(i) );
        TMPExecConnectionObject* ConnectionObject;
        UFC::PLockObject   Lock( FCriticalSection );
        
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex," Create TMP Session CM:[%s] CMID[%d] SessionID[%d]",cp.GetBrokerID().c_str(), cp.GetBrokerIndex(),cp.GetSessionID());
        ConnectionObject = new TMPExecConnectionObject( cp, this, this );
        FConnectionObjects.Add((void*)ConnectionObject);        
        ConnectionObject->SetLogManager( FLog );
        ConnectionObject->Active();
        UInt32 SessionKey = ( cp.GetBrokerIndex() << 16 | cp.GetSessionID());
        FSessionsTable.Add( SessionKey,ConnectionObject );
        FSessionsArray.Add( ConnectionObject );
    }
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex," --------------------------- Finished ---------------------------" );
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPExecManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* )
{
    ConnectionParameter* cp     = CObj->GetConnectionParameter();    
    UFC::AnsiString      Broker = cp->GetBrokerID();
    int                  SID    = cp->GetPVC();
            
    switch( Reason ) 
    {
        case CONNECTION_OBJECT_X25ERROR:      UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex," CM:%s SID:%d Can not establish the connection.(X.25 Retry %d times)", Broker.c_str(), SID, PVC_BUSY_RETRY_TIMES );
                                              break;
        case CONNECTION_OBJECT_UNKNOWN_ERROR: UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex," CM:%s SID:%d Unknown error.(Locked)", Broker.c_str(), SID );
                                              break;  
        case CONNECTION_PVC_LOCKED:           UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex, " CM:%s SID:%d Locked.", Broker.c_str(), SID );
                                              break;        
        case CONNECTION_OBJECT_PVC_BROKEN:    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex," CM:%s SID:%d broken.", Broker.c_str(), SID );
                                              FConnectedCount--;
                                              break;        
        case CONNECTION_OBJECT_NORMAL_CLOSE:  UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex, " CM:%s SID:%d Normal close.", Broker.c_str(), SID);
                                              break;        
        case CONNECTION_OBJECT_PVC_READY:     UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex, " CM:%s SID:%d Ready!.", Broker.c_str(), SID );
                                              FConnectedCount++;
                                              break;         
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPExecManager::ResetPVC( int BrokerIndex, int SID )
{
    ConnectionObjectBase* co = GetSession( BrokerIndex,SID );
    if( co != NULL )
    {
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex, " Unlock CM[%d] TMP Session[%d]", BrokerIndex, SID );
	co->OnCommand( COMMAND_UNLOCK_PVC, NULL );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
int TMPExecManager::GetPVCState( int BrokerIndex,int SID, Int32& Connected )
{
    int State = PVC_STATUS_UNKNOWN;
    ConnectionObjectBase* co = GetSession( BrokerIndex,SID );

    if( co != NULL )
    {
	co->OnCommand( COMMAND_GET_STATE, (void*) &State );
        if( co->IsReady() )
            Connected = 1; ///<Connected.
        else
            Connected = 0; ///< Disconnect
        if( State == PVC_STATUS_SUBSYS_ERR  ||
            State == PVC_STATUS_PW_ERR  ||
            State ==  PVC_STATUS_FCM_ERR ||
            State ==  PVC_STATUS_AP_ERR  ||
            State > 1000  )
        Connected = 2; ///< Error
        return State;
    }
    else
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex," This TMP89 not own Session[%d] CM[%d]", SID, BrokerIndex );
    Connected = 0;
    return PVC_STATUS_UNKNOWNPVC;
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPExecManager::StopConnection( int BrokerIndex,int SID )
{
    ConnectionObjectBase* lCobj = GetSession(BrokerIndex, SID );
    if( lCobj != NULL )
    {
	ConnectionParameter* cp = lCobj->GetConnectionParameter();
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, AppPrefixIndex," FCM:%s SID:%d perform backup procedure.(Locked)",cp->GetBrokerID().c_str(),SID);
	lCobj->OnCommand( COMMAND_LOCK_PVC, NULL );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPExecManager::OnTerminate( UFC::PThread* TerminateThread  )
{
    UFC::PLockObject Lock( FCriticalSection );
    
    FConnectionObjects.Remove((void*)TerminateThread);
}
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase* TMPExecManager::GetSession(  UInt32 SID  )
{
    UFC::PLockObject Lock( FSessionTableCS );
    return FSessionsTable.GetObjectByKey( SID );    
}        
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase* TMPExecManager::GetSession( UInt32 FCMID, UInt16 SID  )
{
    UInt32 RegKey = (FCMID<< 16 | SID );
            
    UFC::PLockObject Lock( FSessionTableCS );
    return FSessionsTable.GetObjectByKey( RegKey );
}        
//-----------------------------------------------------------------------------------------------------------------------
void TMPExecManager::GetStates( TMP89States*  States )
{
   States->SessionNo    = FConnectedCount;
   States->TotalSession = FConnectionObjects.ItemCount( );
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPExecManager::Run()
{
    int FlushCount = 0;
    do 
    {
        UFC::SleepMS( 1000 );
        FlushCount ++;
        fflush( stdout );
        UFC::BufferedLog::FlushToFile();
        if( FlushCount >= 10 && FLog != NULL ) 
        {
            FlushCount = 0;
            FLog->Flush();
        }        
            
    }while( TRUE );
    FLog->Flush();
}
//----------------------------------------------------------------------------------------------------------------------

