#include "ConfirmObjectsManagerThread.h"
#include "LinkSubSystem.h"
//-----------------------------------------------------------------------------------------------------------------------
//
//  TAIFEX side( TAIFEXConfirmManager )
//
//  Objects Manager for AP code 3,9 Confirm subsystem
//
//-----------------------------------------------------------------------------------------------------------------------
extern BOOL IsOptions;
extern MessageObject*             MessageObj;
extern AnsiString                 COMMAND_SUBJECT;
//-----------------------------------------------------------------------------------------------------------------------
SInt TotalCount( 0 );
SInt TotalProcessingTime( 0 );
SInt MaxProcessingTime( 0 );
//-----------------------------------------------------------------------------------------------------------------------
TAIFEXConfirmManager::TAIFEXConfirmManager( void )
:PThread(NULL, FALSE),
 FOpenSwitch( 0 )
{
	AnsiString LogFileName;

    if( IsOptions == TRUE )
        LogFileName = "log/OPT.x" + UFC::GetDateString()+".3.8.9";
    else
        LogFileName = "log/FUT.x" + UFC::GetDateString()+".3.8.9";
	FLog    = new LogManager( LogFileName );
	FBuffer = new char[ sizeof( RPTLOG_MSG_LAST ) ];
}
//-----------------------------------------------------------------------------------------------------------------------
TAIFEXConfirmManager::~TAIFEXConfirmManager()
{
    for( Int32 i = FConnectionObjects.ItemCount() - 1; i >= 0; i-- )
         delete (TAIFEXConfirmConnectionObject*)FConnectionObjects.GetItem(i);

    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
    printf("------ TAIFEXConfirmManager thread stopped. -----\n");
    delete [] FBuffer;
    if( FLog != NULL )
    {
            delete FLog;
            FLog = NULL;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::CreateConnectionObjects( AnsiString Server, AnsiString ID,AnsiString Pwd,AnsiString DBName, BOOL IsSim )
{
    TAIFEXConfirmConnectionObject* ConfirmConnectionObject;
    int FFCMCount = GetFCMs( '3' );
    
    for( int i = 0; i < FFCMCount ; i++ )
    {
        ConnectionParameter cp( GetLink( i ) , GetPVC( i ), '3', GetBrokerID( i ), GetLinePassword( i ) );

        if( strcmp( GetBrokerID( i ), "F099000" ) == 0 )
        {
            if( IsSim == FALSE )     // Normal Open market, create a TSCD.
            {
                UFC::PLockObject Lock( FThreadCriticalSection );
                
                ConfirmConnectionObject = new TAIFEXConfirmConnectionObject( cp, this, this );
                FConnectionObjects.Add((void*)ConfirmConnectionObject);
                ConfirmConnectionObject->SetLogManager( FLog );
                ConfirmConnectionObject->Active();                
            }
        }
        else
        {
            UFC::PLockObject Lock( FThreadCriticalSection );
            
            ConfirmConnectionObject = new TAIFEXConfirmConnectionObject( cp, this, this );
            FConnectionObjects.Add((void*)ConfirmConnectionObject);
            ConfirmConnectionObject->SetLogManager( FLog );
            ConfirmConnectionObject->Active();
        }
    }
    int FCM8Count = GetFCMs( '8' );

    for( int i = 0; i < FCM8Count ; i++ )
    {
        ConnectionParameter cp( GetLink( i ) , GetPVC( i ), '8', GetBrokerID( i ), GetLinePassword( i ) );
        UFC::PLockObject Lock( FThreadCriticalSection );
        
        ConfirmConnectionObject = new TAIFEXConfirmConnectionObject( cp, this, this );
        FConnectionObjects.Add((void*)ConfirmConnectionObject);
        ConfirmConnectionObject->SetLogManager( FLog );
        ConfirmConnectionObject->Active();
    }

    int FCMCount = GetFCMs( '9' );

    for( int i = 0; i < FCMCount ; i++ )
    {
        ConnectionParameter cp( GetLink( i ) , GetPVC( i ), '9', GetBrokerID( i ), GetLinePassword( i ) );
        UFC::PLockObject Lock( FThreadCriticalSection );
        
        ConfirmConnectionObject = new TAIFEXConfirmConnectionObject( cp, this, this );
        FConnectionObjects.Add((void*)ConfirmConnectionObject);
        ConfirmConnectionObject->SetLogManager( FLog );
        ConfirmConnectionObject->Active();
    }
    UFC::BufferedLog::Printf( " Create %d APCode 3 PVCs.", FFCMCount );
    UFC::BufferedLog::Printf( " Create %d APCode 8 PVCs.", FCM8Count );
    UFC::BufferedLog::Printf( " Create %d APCode 9 PVCs.", FCMCount );
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::OnOpenSwitch( int Value )
{
	FOpenSwitch = Value;
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::OpenCCH( )
{
    RPTREG     rptreg;
    AnsiString ChannelName( "XBIO3_Manager" ); /// Unique channel name.

    rptreg.Channel_type = RPTREG_TYPE_NORMAL;
    strcpy( rptreg.Channel_name, ChannelName.c_str() );
    if( Fcch.cchOpen( &rptreg ) < 0 )
	throw( CCHException( "cchOpen Error!"));
}
//-----------------------------------------------------------------------------------------------------------------------
//  Modify 2004/08/14 By Simon. return seq. number
//-----------------------------------------------------------------------------------------------------------------------
int TAIFEXConfirmManager::AddBrokerToCCH( ConnectionParameter* cp  )
{
    BROKER_PVC_DEF BrokerInfo;
    char           LineNo[ 7 ];
    int            Count = 1;

    BrokerInfo.apcode = cp->GetAPCode();
    BrokerInfo.pvc    = ( short ) cp->GetPVC();
    sprintf( LineNo, "%06d", cp->GetLink() );
    strncpy( BrokerInfo.line, LineNo, 6 );
    strncpy( BrokerInfo.brokerid, (char*)cp->GetBrokerID( ).c_str(), 7 );
    FCCHCriticalSection.Enter();
    Fcch.cchIOCtl( PVCAdd, &BrokerInfo, &Count, NULL );
    FCCHCriticalSection.Leave();
    return BrokerInfo.current_seq;
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::RemoveBrokerFromCCH( ConnectionParameter* cp )
{
    BROKER_PVC_DEF BrokerInfo;
    char           LineNo[ 7 ];
    int            Count = 1;

    BrokerInfo.apcode = cp->GetAPCode();
    BrokerInfo.pvc = ( short ) cp->GetPVC();
    sprintf( LineNo, "%06d", cp->GetLink() );
    strncpy( BrokerInfo.line,LineNo, 6 );
    strncpy( BrokerInfo.brokerid, (char*)cp->GetBrokerID( ).c_str(), 7 );
    FCCHCriticalSection.Enter();
    Fcch.cchIOCtl( PVCRemove, &BrokerInfo, &Count, NULL);
    FCCHCriticalSection.Leave();
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::GetReportNotify( )
{
    int ExtInfo;

    if( Fcch.cchGetSeq( FBuffer, 1, 0, ExtInfo ) < 0 )
        throw( CCHException( "cchGetSeq Error!" ) );

    switch( ExtInfo )
    {
            case CCH_I_NO_MSG:   break;
            case CCH_I_MSG_SEQ:  FCommandCriticalSection.Enter();
                         SendSeqNotify( (RPTLOG_MSG_SEQ*) (void*)FBuffer );
                         FCommandCriticalSection.Leave();
                         break;
            case CCH_I_MSG_LAST: FCommandCriticalSection.Enter();
                         SendLastNotify( (RPTLOG_MSG_LAST*)(void*)FBuffer );
                         FCommandCriticalSection.Leave();
                         break;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::SendSeqNotify( RPTLOG_MSG_SEQ* MsgSeq )
{
    AnsiString FCMNo;
    
    if( MsgSeq->Ap_Code == '3')
        FCMNo = AnsiString( MsgSeq->Broker_ID, 7 );
    else ///< for ap code 8,9
        FCMNo = AnsiString( MsgSeq->Broker_ID, 4 );
    ConnectionObjectBase* Co = GetConnection( FCMNo, MsgSeq->Ap_Code );

    if( Co != NULL )
    {
        //printf("AP:%c Broker:%s Sequence:%d\n",MsgSeq->Ap_Code,FCMNo.c_str(),MsgSeq->Current_Seq);
        Co->OnCommand( CONNECTION_CCH_UPDATE_SEQ, (void*) MsgSeq->Current_Seq );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::SendLastNotify( RPTLOG_MSG_LAST* LastMsg )
{
    AnsiString FCMNo;
    if( LastMsg->Ap_Code == '3')
            FCMNo = AnsiString( LastMsg->Broker_ID, 7 );
    else ///< for ap code 8,9
            FCMNo = AnsiString( LastMsg->Broker_ID, 4 );
    ConnectionObjectBase* Co = GetConnection( FCMNo, LastMsg->Ap_Code );

    if( Co != NULL )
    {
        printf("---------------------------------------------\n");
        printf("Last confirm message Broker = %s\n",FCMNo.c_str() );
        printf("---------------------------------------------\n");
        Co->OnCommand( CONNECTION_CCH_LAST_MSG,(void*) LastMsg);
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::ResetPVC( int Link, int PVC )
{
    ConnectionObjectBase* co = GetConnection( Link, PVC );
    if( co != NULL )
{
        UFC::BufferedLog::Printf( "Unlock Link:%d PVC:%d.", Link, PVC);
        co->OnCommand( COMMAND_UNLOCK_PVC, NULL );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
int TAIFEXConfirmManager::GetPVCState( int Link, int PVC )
{
    int State = PVC_STATUS_UNKNOWNPVC;
    ConnectionObjectBase* co = GetConnection( Link, PVC );
    if( co != NULL )
        co->OnCommand( COMMAND_GET_STATE, (void*) &State );
    return State;
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::StopConnection( int Link, int PVC )
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
ConnectionObjectBase* TAIFEXConfirmManager::GetConnection( int Link, int PVC )
{
    ConnectionParameter* cp;
    ConnectionObjectBase* co;

    FThreadCriticalSection.Enter();
    for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
    {
        co = static_cast<TAIFEXConfirmConnectionObject*>(FConnectionObjects.GetItem( i ));
        cp = co->GetConnectionParameter();
        if(  cp->GetPVC() == PVC && cp->GetLink() == Link )
        {
            FThreadCriticalSection.Leave();
            return co;
        }
    }
    FThreadCriticalSection.Leave();
    return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase* TAIFEXConfirmManager::GetConnection( AnsiString& BrokerID, char APCode )
{
    ConnectionParameter* cp;
    ConnectionObjectBase* co;
    PLockObject Lock( FThreadCriticalSection );

    if( APCode == '9')
    {
        for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
        {
            co = static_cast<TAIFEXConfirmConnectionObject*>(FConnectionObjects.GetItem( i ));
            cp = co->GetConnectionParameter();
            if(  cp->GetAPCode() == '9' && strncmp( BrokerID.c_str(), cp->GetBrokerID().c_str(),4) == 0 )
                return co;
        }
    }
    else if( APCode == '8')
    {
        for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
        {
            co = static_cast<TAIFEXConfirmConnectionObject*>(FConnectionObjects.GetItem( i ));
            cp = co->GetConnectionParameter();
            if(  cp->GetAPCode() == '8' && strncmp( BrokerID.c_str(), cp->GetBrokerID().c_str(),4) == 0 )
                return co;
        }
    }
    else
    {
        for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
        {
            co = static_cast<TAIFEXConfirmConnectionObject*>(FConnectionObjects.GetItem( i ));
            cp = co->GetConnectionParameter();
            if(  cp->GetAPCode() == '3' && strncmp( BrokerID.c_str(), cp->GetBrokerID().c_str(), 7 ) == 0 )
                return co;
        }
    }
    return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::OnTerminate( PThread* TerminateThread  )
{
    FThreadCriticalSection.Enter();
    FConnectionObjects.Remove((void*)TerminateThread);
    FThreadCriticalSection.Leave();
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::UpdatePVCState( ConnectionObjectBase* CObj )
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
void TAIFEXConfirmManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* Data )
{
    ConnectionParameter* cp = CObj->GetConnectionParameter();
    int PVC = cp->GetPVC();
    AnsiString Broker = cp->GetBrokerID();

    switch( Reason )
    {
            case CONNECTION_OBJECT_X25ERROR:
                                              UFC::BufferedLog::Printf(" PVC %d X.25 error.",PVC);
                                              UpdatePVCState( CObj );
                                              break;
            case CONNECTION_OBJECT_UNKNOWN_ERROR:
                                              UFC::BufferedLog::Printf(" PVC %d Unknown error.",PVC);
                                              break;
            case CONNECTION_OBJECT_PVC_READY:
                                              *((int*)Data) = AddBrokerToCCH( cp );
                                              UFC::BufferedLog::Printf(" PVC %d Ready!.",PVC);
                                              UpdatePVCState( CObj );
                                              break;
            case CONNECTION_PVC_LOCKED:
                                              RemoveBrokerFromCCH( cp );
                                              UFC::BufferedLog::Printf(" PVC %d Locked.",PVC);
                                              UpdatePVCState( CObj );
                                              break;
            case CONNECTION_OBJECT_PVC_BROKEN:
                                              RemoveBrokerFromCCH( cp );
                                              UFC::BufferedLog::Printf(" PVC %d broken.",PVC);
                                              UpdatePVCState( CObj );
                                              break;
            case CONNECTION_OBJECT_NORMAL_CLOSE:
                                              RemoveBrokerFromCCH( cp );
                                              UFC::BufferedLog::Printf(" PVC %d Normal close.",PVC);
                                              break;
            case CONNECTION_CONFIRM_FINASHED:
                                              UFC::BufferedLog::Printf(" PVC %d Sent last confirm message.",PVC);
                                              break;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::GetStates( XBIO3States* States )
{
    States->MaxProcessingTime     = MaxProcessingTime;
    States->CurrentReportCount    = TotalCount;
    if( States->CurrentReportCount == 0 )
        States->AverageProcessingTime = 0;
    else
        States->AverageProcessingTime = TotalProcessingTime/States->CurrentReportCount;
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::Execute()
{
    int FlushCount = 0;

    while( IsTerminated() == FALSE )
    {
        try
        {
            if( FOpenSwitch < OS_SHOUTDOWN_SYS  )
                OpenCCH();           // Open cch client.
            while( FOpenSwitch < OS_SHOUTDOWN_SYS && IsTerminated() == FALSE )
            {
                GetReportNotify( );
                FlushCount++;
                fflush( stdout );
                if( FlushCount  >= 10 && FLog != NULL )
                {
                    FlushCount = 0;
                    FLog->Flush();
                }
            }
            break;
        }
        catch( CCHException &e )
        {
            UFC::BufferedLog::Printf(" CCH Exception:%s",e.what());
            UFC::BufferedLog::Printf(" Try to connect to RPTS after 5 seconds...");
            sleep( 5 );
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
    Fcch.cchClose();
}
//-----------------------------------------------------------------------------------------------------------------------


