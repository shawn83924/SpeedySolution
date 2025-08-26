#include "OrderObjectsManagerThread.h"
#include "LinkSubSystem.h"
//-----------------------------------------------------------------------------------------------------------------------
//
//  TAIFEX side( TAIFEXOrderManeger )
//
//  Objects Manager for AP code 4 Order subsystem
//
//-----------------------------------------------------------------------------------------------------------------------
extern BOOL                    IsTSEC;
extern MessageObject*          MessageObj;
extern TWSE::TSECLines*        PVCLines; 
extern UFC::AnsiString         COMMAND_SUBJECT;
extern UFC::AnsiString         COMMAND_KEY;
extern TradeSession            CurrentSession;
extern int                     MatchInterval;
extern BOOL                    MatchBySetting;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//-----------------------------------------------------------------------------------------------------------------------
TSECOrderManager::TSECOrderManager( int LinkNumber )
:PThread( NULL, FALSE ),
 FLinkNumber( LinkNumber ),
 FGroupCount( 0 ),
 FOpenSwitch( 0 ),
 FTransactionObject( this ),
 FFIXManager( NULL )
{
    AnsiString LogFileName;

    if( IsTSEC == TRUE )
        LogFileName = "log/Link"+AnsiString( FLinkNumber )+"/TSE.x" + UFC::GetDateString()+".4";
    else
        LogFileName = "log/Link"+AnsiString( FLinkNumber )+"/OTC.x" + UFC::GetDateString()+".4";
    OrderLog = new UFC::BufferedLog( LogFileName );
}
//-----------------------------------------------------------------------------------------------------------------------
TSECOrderManager::~TSECOrderManager()
{
    for( Int32 i = FConnectionObjects.ItemCount() - 1; i >= 0; i-- )
         delete (TSECOrderConnectionObject*)FConnectionObjects.GetItem(i);
    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        printf("Terminate TAIFEXOrderManager thread.\n");
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
    printf("TAIFEXOrderManager thread stopped.\n");        
    if( OrderLog != NULL )
    {
        delete OrderLog;
        OrderLog = NULL;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECOrderManager::GetStates( OrderStates* States )
{
    FTransactionObject.GetStates( States );
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECOrderManager::OnOpenSwitch( int Value )
{
    if( FOpenSwitch != Value )
    {
        FOpenSwitch = Value;
        FTransactionObject.SetOpenSwitch( FOpenSwitch );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECOrderManager::CreateConnectionObjects( void )
{   
    TSECOrderConnectionObject* ConnectionObject;
    int Count = 0;
    char SessionAPcode;

    if( PVCLines->SaveFIXConfig() > 0 )
        FFIXManager = new SpeedyFIXManager( PVCLines->GetFIXPort( 0 ) );    
    for( int i = 0; i < PVCLines->GetCount() ; i++ )
    {
        if( PVCLines->GetAPCode( i ) == '0' )
        {
            switch( CurrentSession )
            {
                case tsNormal: SessionAPcode = '0';break;
                case tsOdd:    SessionAPcode = '4';break;
                case tsPost:   SessionAPcode = '9';break;
                default:       SessionAPcode = '0';break;
            }
            ConnectionParameter cp( FLinkNumber ,i, SessionAPcode, 
                                    PVCLines->GetBrokerID(i), 
                                    PVCLines->GetPassword(i),
                                    PVCLines->GetIP( i ),
                                    PVCLines->GetRemotePort(i),
                                    PVCLines->GetPVCID(i),
                                    FALSE, 
                                    PVCLines->GetThroughput( i )    );            

            UFC::PLockObject Lock( FCriticalSection );

            ConnectionObject = new TSECOrderConnectionObject( IsTSEC, cp, &FTransactionObject, this );
            FConnectionObjects.Add((void*)ConnectionObject);
            ConnectionObject->SetLogManager( OrderLog );
            ConnectionObject->Active();
            Count ++;
        }
    }
    if( FFIXManager != NULL )
    {
        UFC::BufferedLog::Printf( " ***** Start running FIX Engine *****" );
        FFIXManager->Run();
        UFC::SleepMS( 1000 ); 
    }
    UFC::BufferedLog::Printf(" %d order connection objects created.",Count);    
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECOrderManager::ResetPVC( const UFC::AnsiString& PVCID )
{
    ConnectionObjectBase* co = GetConnection( PVCID );
    if( co != NULL )
    {
        UFC::BufferedLog::Printf( " Unlock PVC %s.", PVCID.c_str() );
        co->OnCommand( COMMAND_UNLOCK_PVC, NULL );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
int TSECOrderManager::GetPVCState( const UFC::AnsiString& PVCID )
{
    int State = PVC_STATUS_UNKNOWNPVC;
    ConnectionObjectBase* co = GetConnection( PVCID );
    
    if( co != NULL )
        co->OnCommand( COMMAND_GET_STATE, (void*) &State );
    return State;
}
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase* TSECOrderManager::GetConnection( const UFC::AnsiString& PVCID )
{
    ConnectionParameter* cp;
    ConnectionObjectBase* co;
    UFC::PLockObject CSLock( FCriticalSection );
    
    for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
    {
        co = static_cast<TSECOrderConnectionObject*>(FConnectionObjects.GetItem( i ));
        cp = co->GetConnectionParameter();
        if(  cp->GetPVCID() == PVCID )
        {
            FCriticalSection.Leave();
            return co;
    	}
    }
    return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECOrderManager::StopConnection( const UFC::AnsiString& PVCID )
{
    ConnectionObjectBase* lCobj = GetConnection( PVCID );
    if( lCobj != NULL )
    {
        ConnectionParameter* cp = lCobj->GetConnectionParameter();
        UFC::BufferedLog::Printf( " FCM:%s PVC:%s perform backup procedure.(Locked)",cp->GetBrokerID().c_str(),PVCID.c_str() );
        lCobj->OnCommand( COMMAND_LOCK_PVC, NULL );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECOrderManager::OnTerminate( PThread* TerminateThread  )
{
    FCriticalSection.Enter();
    FConnectionObjects.Remove((void*)TerminateThread);
    FCriticalSection.Leave();
}
//-----------------------------------------------------------------------------------------------------------------------
void TSECOrderManager::UpdatePVCState( ConnectionObjectBase* CObj )
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
void TSECOrderManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* )
{
	ConnectionParameter* cp = CObj->GetConnectionParameter();
	AnsiString PVCID = cp->GetPVCID();
	AnsiString Broker = cp->GetBrokerID();

	switch( Reason )
	{
		case CONNECTION_OBJECT_X25ERROR:
                                                  UFC::BufferedLog::Printf(" PVC %s X.25 error.", PVCID.c_str());
                                                  UpdatePVCState( CObj );
                                                  break;
		case CONNECTION_OBJECT_UNKNOWN_ERROR:
                                                  UFC::BufferedLog::Printf(" PVC %s Unknown error.", PVCID.c_str());
                                                  UpdatePVCState( CObj );
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
                                                  UFC::BufferedLog::Printf(" PVC %s Normal close.", PVCID.c_str());
                                                  UpdatePVCState( CObj );
                                                  break;
	}

}
//-----------------------------------------------------------------------------------------------------------------------
void TSECOrderManager::Execute( void )
{
    int FlushCount = 0;
    
    do
    {
        sleep( 1 );        
        if( MatchBySetting == FALSE )  ///< Metch every MatchInterval sec. 
        {
            FlushCount ++;
            fflush( stdout );
            if( FlushCount >= MatchInterval )
            {            
                if( OrderLog != NULL )
                    OrderLog->Flush();
                if( FTransactionObject.Match() > 0 )
                {
                    UFC::BufferedLog::Printf( " Set execution event." );
                    ExecutionsEvent.SetEvent();
                }
                FlushCount = 0;
            }
        }
        else ///< Match each sec.(Matched by Rule )
        {
            if( OrderLog != NULL )
                OrderLog->Flush();
            if( FTransactionObject.Match() > 0 )
            {                
                UFC::BufferedLog::Printf( " Set execution event." );
                ExecutionsEvent.SetEvent();
            }
        }
        
    }while( FOpenSwitch < OS_SHOUTDOWN_SYS && IsTerminated() == FALSE );

    if( OrderLog != NULL )    
        OrderLog->Flush();
    UFC::BufferedLog::Printf( " XBIO4 Close market." );
}
//----------------------------------------------------------------------------------------------------------------------
void TSECOrderManager::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
#ifndef SIMULATOR
    UFC::AnsiString PVCID, MSG;

    if( Data->get( "PROCESS_PVC", PVCID ) && Data->get( "CONFIRM_ORDER", MSG ) )
    {
        ConnectionObjectBase* co = GetConnection( PVCID );
        if( co != NULL )
        {
            try
            {
                TSECOrderConnectionObject* tco = dynamic_cast<TSECOrderConnectionObject*>(co);
                if( tco != NULL )
                {
	                if( tco != NULL )
	                {
	                    MessageHeader* Msg = MessageHeader::CreateMessage( MSG.c_str() );
	                    tco->SendMessage( Msg );
	                	FTransactionObject.SetLastConfirm( PVCID, Msg );
	                }
                }
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
