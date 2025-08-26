#include "FileObjectManagerThread.h"
#include "LinkSubSystem.h"
//------------------------------------------------------------------------------
TAIFEXFileObjectManager::TAIFEXFileObjectManager( const AnsiString& SourceID, const AnsiString& DestinationID)
:PThread(NULL, FALSE)
,FFCMCount(0)
,FCMCount(0)
,FOpenSwitch( 0 )
,FSourceID(SourceID)
,FDestinationID(DestinationID)
{
}
//------------------------------------------------------------------------------
TAIFEXFileObjectManager::~TAIFEXFileObjectManager(void)
{
    for( Int32 i = FConnectionObjects.ItemCount() - 1; i >= 0; i-- )
         delete (TAIFEXFileConnectionObject*)FConnectionObjects.GetItem(i);

    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
    printf("------ TAIFEXFileManager thread stopped. -----\n");
}
//------------------------------------------------------------------------------
void TAIFEXFileObjectManager::OnTerminate(PThread* TerminateThread)
{
}
//------------------------------------------------------------------------------
void TAIFEXFileObjectManager::OnConnectionNotify(ConnectionObjectBase* CObj, Int32 Reason, void* Data)
{
	ConnectionParameter* cp = CObj->GetConnectionParameter();
	switch( Reason )
	{
    	case CONNECTION_OBJECT_NORMAL_CLOSE:	printf("PVC %d Normal close.\n",cp->GetPVC());break;
	    case CONNECTION_OBJECT_X25ERROR:		printf("PVC %d X.25 error.\n",cp->GetPVC());break;
    	case CONNECTION_OBJECT_UNKNOWN_ERROR:	printf("PVC %d Unknown error.\n",cp->GetPVC());break;
	    case CONNECTION_OBJECT_PVC_READY:		printf("PVC %d Ready!.\n",cp->GetPVC());break;
        case CONNECTION_RECV_MSG_F050:			PassToInitiaorConnection( Data );break;
	}
}
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase* TAIFEXFileObjectManager::GetConnection( int Link, int PVC )
{
    ConnectionParameter* cp;
    ConnectionObjectBase* co;

    for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
    {
        co = static_cast<TAIFEXFileConnectionObject*>(FConnectionObjects.GetItem( i ));
        cp = co->GetConnectionParameter();
        if(  cp->GetPVC() == PVC && cp->GetLink() == Link )
            return co;
    }
    return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileObjectManager::OnOpenSwitch( int Value )
{
    FOpenSwitch = Value;
}
//-----------------------------------------------------------------------------------------------------------------------
int TAIFEXFileObjectManager::GetPVCState( int Link, int PVC )
{
    int State = PVC_STATUS_UNKNOWNPVC;
    ConnectionObjectBase* co = GetConnection( Link, PVC );
    if( co != NULL )
        co->OnCommand( COMMAND_GET_STATE, (void*) &State );
    return State;
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileObjectManager::ResetPVC( int Link, int PVC )
{
    ConnectionObjectBase* co = GetConnection( Link, PVC );
    if( co != NULL )
    {
        UFC::BufferedLog::Printf( " Unlock Link:%d PVC:%d.",Link,PVC);
        co->OnCommand( COMMAND_UNLOCK_PVC, NULL );
    }
}
//------------------------------------------------------------------------------
void TAIFEXFileObjectManager::CreateConnectionObjects(BOOL IsOptions)
{
    AnsiString FolderName = "";
    if( IsOptions == TRUE )
        FolderName = "SimftOPT";
    else
        FolderName = "SimftFUT";
    mkdir( FolderName.c_str(), 0755 );

    TAIFEXFileConnectionObject* Obj = NULL;

    FFCMCount = GetFCMs( '1' );
    for( int i = 0; i < FFCMCount; i++ )
    {
        ConnectionParameter cp( GetLink(i) , GetPVC(i), '1', GetBrokerID(i), GetLinePassword(i) );
        Obj = new TAIFEXFileConnectionObject( cp, this, this );
        Obj->SetFolderName( FolderName );
        FConnectionObjects.Add( (void*)Obj );
        Obj->Active();
    }
    FCMCount = GetFCMs( '2' );
    for( int i = 0; i < FCMCount; i++ )
    {
        ConnectionParameter cp( GetLink(i) , GetPVC(i), '2', GetBrokerID( i ), GetLinePassword(i) );
        Obj = new TAIFEXFileConnectionObject( cp, this, this );
        Obj->SetFolderName( FolderName );
        FConnectionObjects.Add( (void*)Obj );
        Obj->Active();
    }
}
//------------------------------------------------------------------------------
void TAIFEXFileObjectManager::Execute(void)
{
	do
	{
		sleep(1);
		fflush( stdout );
	}
	while( TRUE );
}
//------------------------------------------------------------------------------
void TAIFEXFileObjectManager::PassToInitiaorConnection(void* Data)
{
	printf( "pass to initiaor connection.\n" );
	fflush( stdout );
	TAIFEXFileConnectionObject* pConnection = (TAIFEXFileConnectionObject*)( FConnectionObjects.GetItem(1) );
	if( pConnection != NULL )
	{
		MessageF050* FMsg = (MessageF050*)Data;
		AnsiString sID = FMsg->GetSourceID();
		AnsiString dID = FMsg->GetDestinationID();
		int MessageLength = FMsg->GetMessageLength();
		AnsiString fCode = FMsg->GetFileCode();
		char Request_Message[989];
		memcpy( Request_Message, FMsg->GetRequest_Message(), 989 );
		MessageF050* F050 = new MessageF050( (char*)sID.c_str(), (char*)dID.c_str(), MessageLength, (char*)fCode.c_str(), Request_Message );
		printf( "StartSequence:%d, EndSequence:%d\n", F050->GetStartSequence(), F050->GetEndSequence() );
		printf( "pConnection->AddNewJob( F050 );\n" );
		fflush( stdout );
		pConnection->AddNewJob( F050 );
	}
}
//------------------------------------------------------------------------------
void TAIFEXFileObjectManager::SendConfirmMessage()
{
    TAIFEXFileConnectionObject* pConnection = (TAIFEXFileConnectionObject*)( FConnectionObjects.GetItem(1) );
    if( pConnection != NULL )
    {
        MessageF170* F170 = new MessageF170( (char*)FSourceID.c_str(), (char*)FDestinationID.c_str() );
        pConnection->AddNewJob( F170 );
    }
}
//------------------------------------------------------------------------------
