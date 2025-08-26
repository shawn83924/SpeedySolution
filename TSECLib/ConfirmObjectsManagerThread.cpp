#include "ConfirmObjectsManagerThread.h"
#include "LinkSubSystem.h"
//-----------------------------------------------------------------------------------------------------------------------
//
//  TAIFEX side( TAIFEXConfirmManager )
//
//  Objects Manager for AP code 3,9 Confirm subsystem
//
//-----------------------------------------------------------------------------------------------------------------------
extern BOOL IsTSEC;
extern MessageObject*             MessageObj;
extern TWSE::TSECLines*           PVCLines; 
extern AnsiString                 COMMAND_SUBJECT;
extern AnsiString                 COMMAND_KEY;
extern UFC::PStringList           Executions;
extern UFC::PEvent                ExecutionsEvent;
//-----------------------------------------------------------------------------------------------------------------------
SInt TotalCount( 0 );
SInt TotalProcessingTime( 0 );
SInt MaxProcessingTime( 0 );
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//-----------------------------------------------------------------------------------------------------------------------
TSECConfirmManager::TSECConfirmManager( int LinkNumber )
:PThread(NULL, FALSE),
 FLinkNumber( LinkNumber ),
 FOpenSwitch( 0 )
{
    AnsiString LogFileName;

    if( IsTSEC == TRUE )
        LogFileName = "log/Link"+AnsiString( FLinkNumber )+"/TSE.x" + UFC::GetDateString()+".3";
    else
        LogFileName = "log/Link"+AnsiString( FLinkNumber )+"/OTC.x" + UFC::GetDateString()+".3";
    FLog    = new UFC::BufferedLog( LogFileName );
}
//-----------------------------------------------------------------------------------------------------------------------
TSECConfirmManager::~TSECConfirmManager()
{
    for( Int32 i = FConnectionObjects.ItemCount() - 1; i >= 0; i-- )
         delete (TSECConfirmConnectionObject*)FConnectionObjects.GetItem(i);

    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
    UFC::BufferedLog::Printf(" ------ TSECConfirmManager thread stopped. -----");
    if( FLog != NULL )
    {
        delete FLog;
        FLog = NULL;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECConfirmManager::CreateConnectionObjects( void )
{
    TSECConfirmConnectionObject* ConnectionObject;
    int Count = 0;

    for( int i = 0; i < PVCLines->GetCount() ; i++ )
    {
        if( PVCLines->GetAPCode( i ) == '3'  && PVCLines->IsFIX( i ) == FALSE )
        {
            ConnectionParameter cp( FLinkNumber ,i, '3', 
                                    PVCLines->GetBrokerID(i), 
                                    PVCLines->GetPassword(i),
                                    PVCLines->GetIP( i ),
                                    PVCLines->GetRemotePort(i),
                                    PVCLines->GetPVCID(i) );            
            
            UFC::PLockObject Lock( FThreadCriticalSection );
        
            ConnectionObject = new TSECConfirmConnectionObject( cp, this, this );
            FConnectionObjects.Add((void*)ConnectionObject);
            ConnectionObject->SetLogManager( FLog );
            ConnectionObject->Active();
            Count ++;
	}
    }
    UFC::BufferedLog::Printf( " %d confirm connection objects created.",Count);    
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECConfirmManager::OnOpenSwitch( int Value )
{
	FOpenSwitch = Value;
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECConfirmManager::GetReportNotify( )
{
    if( ExecutionsEvent.WaitFor( 10 ) == TRUE )
    {
        UFC::BufferedLog::Printf( " Get report notify, %d execution reports in list.", Executions.ItemCount() );    
        ConnectionObjectBase* co;
        Int64                 CurrentSeq = Executions.ItemCount();
        UFC::PLockObject      LockObj( FThreadCriticalSection );

        for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
        {
            co = static_cast<TSECConfirmConnectionObject*>(FConnectionObjects.GetItem( i ));
            co->OnCommand( CONNECTION_CCH_UPDATE_SEQ, (void*)CurrentSeq );        
        }
    }
    
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECConfirmManager::ResetPVC( const UFC::AnsiString& PVCID )
{
    ConnectionObjectBase* co = GetConnection( PVCID );
    if( co != NULL )
    {
        UFC::BufferedLog::Printf( "Unlock PVC %s.", PVCID.c_str());
        co->OnCommand( COMMAND_UNLOCK_PVC, NULL );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
int TSECConfirmManager::GetPVCState( const UFC::AnsiString& PVCID )
{
    int State = PVC_STATUS_UNKNOWNPVC;
    ConnectionObjectBase* co = GetConnection( PVCID );
    if( co != NULL )
        co->OnCommand( COMMAND_GET_STATE, (void*) &State );
    return State;
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECConfirmManager::StopConnection( const UFC::AnsiString& PVCID )
{
    ConnectionObjectBase* lCobj = GetConnection( PVCID );
    if( lCobj != NULL )
    {
        ConnectionParameter* cp = lCobj->GetConnectionParameter();
        UFC::BufferedLog::Printf( " FCM:%s PVC %s perform backup procedure.(Locked)",cp->GetBrokerID().c_str(),PVCID.c_str() );
        lCobj->OnCommand( COMMAND_LOCK_PVC, NULL );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase* TSECConfirmManager::GetConnection( const UFC::AnsiString& PVCID )
{
    ConnectionParameter* cp;
    ConnectionObjectBase* co;
    UFC::PLockObject LockObj( FThreadCriticalSection );

    for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
    {
        co = static_cast<TSECConfirmConnectionObject*>(FConnectionObjects.GetItem( i ));
        cp = co->GetConnectionParameter();
        if(  cp->GetPVCID() == PVCID )
             return co;
    }
    return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECConfirmManager::OnTerminate( PThread* TerminateThread  )
{
    FThreadCriticalSection.Enter();
    FConnectionObjects.Remove((void*)TerminateThread);
    FThreadCriticalSection.Leave();
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECConfirmManager::UpdatePVCState( ConnectionObjectBase* CObj )
{
    ConnectionParameter* cp = CObj->GetConnectionParameter();
    AnsiString PVCID = cp->GetPVCID();
    int LinkNo = cp->GetLink();
    int State;
    MTree Tree;
    
    CObj->OnCommand( COMMAND_GET_STATE, (void*) &State );
    Tree.append( "PVC_NO", PVCID );
    Tree.append( "LINK_NO", LinkNo );    
    Tree.append( "PVC_STATE", State );
    MessageObj->Send( TWSE_EX_PVC_STATE_PUBLISHER, COMMAND_KEY, Tree );
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECConfirmManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* Data )
{
    ConnectionParameter* cp = CObj->GetConnectionParameter();
    AnsiString PVCID = cp->GetPVCID();
    AnsiString Broker = cp->GetBrokerID();

    switch( Reason )
    {
        case CONNECTION_OBJECT_X25ERROR:
                                          UFC::BufferedLog::Printf(" PVC %s X.25 error.",PVCID.c_str());
                                          UpdatePVCState( CObj );
                                          break;
        case CONNECTION_OBJECT_UNKNOWN_ERROR:
                                          UFC::BufferedLog::Printf(" PVC %s Unknown error.",PVCID.c_str());
                                          break;
        case CONNECTION_OBJECT_PVC_READY:
                                          UFC::BufferedLog::Printf(" PVC %s Ready!.",PVCID.c_str());
                                          UpdatePVCState( CObj );
                                          break;
        case CONNECTION_PVC_LOCKED:
                                          UFC::BufferedLog::Printf(" PVC %s Locked.",PVCID.c_str());
                                          UpdatePVCState( CObj );
                                          break;
        case CONNECTION_OBJECT_PVC_BROKEN:
                                          UFC::BufferedLog::Printf(" PVC %s broken.",PVCID.c_str());
                                          UpdatePVCState( CObj );
                                          break;
        case CONNECTION_OBJECT_NORMAL_CLOSE:
                                          UFC::BufferedLog::Printf(" PVC %s Normal close.",PVCID.c_str());
                                          break;
        case CONNECTION_CONFIRM_FINASHED:
                                          UFC::BufferedLog::Printf(" PVC %s Sent last confirm message.",PVCID.c_str());
                                          break;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECConfirmManager::GetStates( ConfirmStates* States )
{
    States->MaxProcessingTime     = MaxProcessingTime;
    States->CurrentReportCount    = TotalCount;
    if( States->CurrentReportCount == 0 )
        States->AverageProcessingTime = 0;
    else
        States->AverageProcessingTime = TotalProcessingTime/States->CurrentReportCount;
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECConfirmManager::Execute()
{    
    while( IsTerminated() == FALSE )
    {
        try
        {            
            while( FOpenSwitch < OS_SHOUTDOWN_SYS && IsTerminated() == FALSE )
            {
                GetReportNotify( );                
                fflush( stdout );
                if( FLog != NULL )                
                    FLog->Flush();                
            }
            break;
        }
        catch( ... )
        {
                UFC::BufferedLog::Printf( " Unknow Exception." );
                return;
        }
    }
    if( FLog != NULL )
        FLog->Flush();
    UFC::BufferedLog::Printf( " XBIO3 Normal close." );
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECConfirmManager::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
#ifndef SIMULATOR
    UFC::AnsiString PVCID, MSG;

    if( Data->get( "PROCESS_PVC", PVCID ) && Data->get( "FILL_ORDER", MSG ) )
    {
        ConnectionObjectBase* co = GetConnection( PVCID );
        if( co != NULL )
        {
            try
            {
                TSECConfirmConnectionObject* tco = dynamic_cast<TSECConfirmConnectionObject*>(co);
                if( tco != NULL )
                    tco->AddReportMessage( new MessageR3( MSG.c_str() ) );
            }
            catch(...)
            {
            }
        }
    }
#endif
}
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------------
