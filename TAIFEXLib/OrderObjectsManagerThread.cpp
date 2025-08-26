#ident "@(#) $Id: OrderObjectsManager.cpp,v 1.23 2004/05/21 10:34:21 ap27 Exp $"
#include "OrderObjectsManagerThread.h"
#include "LinkSubSystem.h"
//-----------------------------------------------------------------------------------------------------------------------
//
//  TAIFEX side( TAIFEXOrderManeger )
//
//  Objects Manager for AP code 4 Order subsystem
//
//-----------------------------------------------------------------------------------------------------------------------
extern BOOL IsOptions;
extern MessageObject*             MessageObj;
extern AnsiString                 COMMAND_SUBJECT;
//-----------------------------------------------------------------------------------------------------------------------
TAIFEXOrderManager::TAIFEXOrderManager( void )
:PThread( NULL, FALSE ),
 FGroupCount( 0 ),
 FOpenSwitch( 0 ),
 FOrderCheck( NULL )
{
	AnsiString LogFileName;

    if( IsOptions == TRUE )
        LogFileName = "log/OPT.x" + UFC::GetDateString()+".4";
    else
        LogFileName = "log/FUT.x" + UFC::GetDateString()+".4";
    FLog               = new LogManager( LogFileName );
    FTransactionObject = new OrderTransactionObject( );
}
//-----------------------------------------------------------------------------------------------------------------------
TAIFEXOrderManager::~TAIFEXOrderManager()
{
    for( Int32 i = FConnectionObjects.ItemCount() - 1; i >= 0; i-- )
         delete (TAIFEXOrderConnectionObject*)FConnectionObjects.GetItem(i);

    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        printf("Terminate TAIFEXOrderManager thread.\n");
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
    printf("TAIFEXOrderManager thread stopped.\n");
    if( FTransactionObject != NULL )
        delete FTransactionObject;
    printf("TransactionObject deleted.\n");
    if( FOrderCheck != NULL )
        delete FOrderCheck;
    printf("OrderCheck deleted.\n");
    if( FLog != NULL )
    {
        delete FLog;
        FLog = NULL;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::GetStates( XBIO4States* States )
{
	if( FTransactionObject != NULL )
		FTransactionObject->GetStates( States );
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::OnOpenSwitch( int Value )
{
	if( FOpenSwitch != Value )
	{
		FOpenSwitch = Value;
		if( FTransactionObject != NULL )
			FTransactionObject->OnEvent( OPEN_SWITCH_CHANGED,(void*)FOpenSwitch);
	    if( FOrderCheck != NULL )
			FOrderCheck->SetOpenSwitch( Value );
	}
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::ResetPCM( void )
{
    if( FOrderCheck != NULL )
		FOrderCheck->LoadPCM_Data( );
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::LoadTable( AnsiString DBServerName, AnsiString UserName, AnsiString  Password, AnsiString  DatabaseName, char* DBFileName )
{
    FGroupCount = LoadProduct( (char*)DBServerName.c_str(),(char*)UserName.c_str(),(char*)Password.c_str(),(char*)DatabaseName.c_str() );
    FOrderCheck = new OrderCheck( );
    FOrderCheck->SetDBFilename( DBFileName );
    if( FOrderCheck->LoadDB_Data() != 0 )
    {
        UFC::BufferedLog::Printf( " TAP Client load data from DB error.");
        exit( 1 );
    }
    FOrderCheck->set_bOnlyTestXBIO( TAP_TEST_MODE );
    FTransactionObject->SetOrderCheckObj( FOrderCheck );
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::CreateConnectionObjects( AnsiString Server, AnsiString ID,AnsiString Pwd,AnsiString DBName,BOOL IsSim )
{
    TAIFEXOrderConnectionObject* ConnectionObject;
    int Count = GetFCMs( '4' );    

    printf("%d order connection objects created.\n",Count);
    for( int i = 0; i < Count ; i++ )
    {
        ConnectionParameter cp( GetLink( i ) , GetPVC( i ), '4', GetBrokerID(i), GetLinePassword(i) );
        UFC::PLockObject Lock( FCriticalSection );

        ConnectionObject = new TAIFEXOrderConnectionObject( cp, FTransactionObject, this, this );
        FConnectionObjects.Add((void*)ConnectionObject);
        ConnectionObject->SetLogManager( FLog );
        ConnectionObject->Active();
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::ResetPVC( int Link, int PVC )
{
    ConnectionObjectBase* co = GetConnection( Link, PVC );
    if( co != NULL )
    {
        UFC::BufferedLog::Printf( " Unlock Link:%d PVC:%d.", Link, PVC);
        co->OnCommand( COMMAND_UNLOCK_PVC, NULL );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
int TAIFEXOrderManager::GetPVCState( int Link, int PVC )
{
    int State = PVC_STATUS_UNKNOWNPVC;
    ConnectionObjectBase* co = GetConnection( Link, PVC );
    if( co != NULL )
        co->OnCommand( COMMAND_GET_STATE, (void*) &State );
    return State;
}
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase* TAIFEXOrderManager::GetConnection( int Link, int PVC )
{
    ConnectionParameter* cp;
    ConnectionObjectBase* co;

    FCriticalSection.Enter();
    for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
    {
        co = static_cast<TAIFEXOrderConnectionObject*>(FConnectionObjects.GetItem( i ));
        cp = co->GetConnectionParameter();
        if(  cp->GetPVC() == PVC && cp->GetLink() == Link )
        {
            FCriticalSection.Leave();
            return co;
        }
    }
    FCriticalSection.Leave();
    return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::StopConnection( int Link, int PVC )
{
    ConnectionObjectBase* lCobj = GetConnection( Link, PVC );
    if( lCobj != NULL )
    {
        ConnectionParameter* cp = lCobj->GetConnectionParameter();
        UFC::BufferedLog::Printf( " FCM:%s PVC %d perform backup procedure.(Locked)",cp->GetBrokerID().c_str(),PVC);
        lCobj->OnCommand( COMMAND_LOCK_PVC, NULL );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::OnTerminate( PThread* TerminateThread  )
{
    FCriticalSection.Enter();
    FConnectionObjects.Remove((void*)TerminateThread);
    FCriticalSection.Leave();
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::UpdatePVCState( ConnectionObjectBase* CObj )
{
    ConnectionParameter* cp = CObj->GetConnectionParameter();
    int PVC = cp->GetPVC();
    int LinkNo = cp->GetLink();
    int State;
    MTree Tree;
    
    CObj->OnCommand( COMMAND_GET_STATE, (void*) &State );
    Tree.append( "PVC_NO", PVC );
    Tree.append( "LINK_NO", LinkNo );    
    Tree.append( "PVC_STATE", State );
    MessageObj->Send( PVC_STATE_PUBLISHER, UFC::Hostname, Tree );
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* )
{
	ConnectionParameter* cp = CObj->GetConnectionParameter();
	int PVC = cp->GetPVC();
	AnsiString Broker = cp->GetBrokerID();

	switch( Reason )
	{
		case CONNECTION_OBJECT_X25ERROR:
                                                  UFC::BufferedLog::Printf(" PVC %d X.25 error.", PVC);
                                                  UpdatePVCState( CObj );
                                                  break;
		case CONNECTION_OBJECT_UNKNOWN_ERROR:
                                                  UFC::BufferedLog::Printf(" PVC %d Unknown error.", PVC);
                                                  UpdatePVCState( CObj );
                                                  break;
		case CONNECTION_OBJECT_PVC_READY:
                                                  UFC::BufferedLog::Printf(" PVC %d Ready!.",PVC);
                                                  UpdatePVCState( CObj );
                                                  break;
		case CONNECTION_PVC_LOCKED:
                                                  UFC::BufferedLog::Printf(" PVC %d Locked.",PVC);
                                                  UpdatePVCState( CObj );
                                                  break;
		case CONNECTION_OBJECT_PVC_BROKEN:
                                                  UFC::BufferedLog::Printf(" PVC %d broken.",PVC);
                                                  UpdatePVCState( CObj );
                                                  break;
		case CONNECTION_OBJECT_NORMAL_CLOSE:
                                                  UFC::BufferedLog::Printf(" PVC %d Normal close.", PVC);
                                                  UpdatePVCState( CObj );
                                                  break;
	}

}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::Execute( void )
{
    int FlushCount = 0;
    
    do
    {
        sleep( 1 );
        FlushCount ++;
        fflush( stdout );
        if( FlushCount >= 10 && FLog != NULL )
        {
            FlushCount = 0;
            FLog->Flush();
        }

    }while( FOpenSwitch < OS_SHOUTDOWN_SYS && IsTerminated() == FALSE );

    if( FLog != NULL )
    {
        FTransactionObject->Report( FLog );
    	FLog->Flush();
    }
    UFC::BufferedLog::Printf( " XBIO4 Close market." );
}
//----------------------------------------------------------------------------------------------------------------------

