#ident "@(#) $Id: ConfirmObjectsManager.cpp,v 1.35 2004/08/14 08:03:34 ap25 Exp $"
#include "ConfirmObjectsManager.h"
//-----------------------------------------------------------------------------------------------------------------------
//
//  TAIFEX side( TAIFEXConfirmManager )
//
//  Objects Manager for AP code 3,9 Confirm subsystem
//  
//-----------------------------------------------------------------------------------------------------------------------
SInt TotalCount( 0 );
SInt TotalProcessingTime( 0 );
SInt MaxProcessingTime( 0 );
//-----------------------------------------------------------------------------------------------------------------------
TAIFEXConfirmManager::TAIFEXConfirmManager( int LinkNumber )
:FLinkNumber( LinkNumber ),FOpenSwitch( 0 )
{
	AnsiString LogFileName = "x" + GetDateString()+".3."+AnsiString( FLinkNumber );
	
	Log                = new LogManager( LogFileName );  	
	FBuffer = new char[ sizeof( RPTLOG_MSG_LAST ) ];
}
//-----------------------------------------------------------------------------------------------------------------------
TAIFEXConfirmManager::~TAIFEXConfirmManager()
{
	delete [] FBuffer;
	if( Log != NULL )		
	{
		delete Log;
		Log = NULL;
	}	
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::CreateConnectionObjects( AnsiString Server, AnsiString ID,AnsiString Pwd,AnsiString DBName, BOOL IsSim )
{
	int FFCMCount = GetFCMs( FLinkNumber, '3', (char*)Server.c_str(),(char*)ID.c_str(),(char*)Pwd.c_str(),(char*)DBName.c_str(),IsSim );
	
	for( int i = 0; i < FFCMCount ; i++ )
	{
		ConnectionParameter cp( FLinkNumber , GetPVC( i ), '3', GetBrokerID( i ), GetLinePassword( i ) );
		
		
		if( strcmp( GetBrokerID( i ), "F099000" ) == 0 )
		{
			if( IsSim == FALSE )     // Normal Open market, create a TSCD.
			{
				FThreadCriticalSection.Enter();
			    FConnectionObjects.Add((void*)new TAIFEXConfirmConnectionObject( cp, this, this ));			
			    FThreadCriticalSection.Leave();
			}
		}
		else
		{
			FThreadCriticalSection.Enter();
			FConnectionObjects.Add((void*)new TAIFEXConfirmConnectionObject( cp, this, this ));			
			FThreadCriticalSection.Leave();
		}		
	}
	int FCMCount = GetFCMs( FLinkNumber, '9', (char*)Server.c_str(),(char*)ID.c_str(),(char*)Pwd.c_str(),(char*)DBName.c_str(),IsSim );
	
	for( int i = 0; i < FCMCount ; i++ )
	{
		ConnectionParameter cp( FLinkNumber , GetPVC( i ), '9', GetBrokerID( i ), GetLinePassword( i ) );
		
		FThreadCriticalSection.Enter();
		FConnectionObjects.Add((void*)new TAIFEXConfirmConnectionObject( cp, this, this ));
		FThreadCriticalSection.Leave();
	}
	TFXLog.MSG( XBIO3_INFO, "%d APCode 3 PVCs in this Link.\n", FFCMCount );
	TFXLog.MSG( XBIO3_INFO, "%d APCode 9 PVCs in this Link.\n", FCMCount );
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
	AnsiString ChannelName;
	
	ChannelName.Printf("XBIO3_%d", FLinkNumber );
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
	
	sprintf( LineNo, "%06d", FLinkNumber );
	strncpy( BrokerInfo.brokerid, (char*)cp->GetBrokerID( ).c_str(), 7 );
	BrokerInfo.apcode = cp->GetAPCode();
	strncpy( BrokerInfo.line,LineNo, 6 );
	BrokerInfo.pvc       = ( short ) cp->GetPVC();
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
		
	sprintf( LineNo, "%06d", FLinkNumber );
	strncpy( BrokerInfo.brokerid, (char*)cp->GetBrokerID( ).c_str(), 7 );
	BrokerInfo.apcode = cp->GetAPCode();
	strncpy( BrokerInfo.line,LineNo, 6 );
	BrokerInfo.pvc = ( short ) cp->GetPVC();
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
		
	FCommandCriticalSection.Enter();
	switch( ExtInfo )
	{
		case CCH_I_NO_MSG:   break;
		case CCH_I_MSG_SEQ:  SendSeqNotify( (RPTLOG_MSG_SEQ*) (void*)FBuffer ); 
							 break;
		case CCH_I_MSG_LAST: SendLastNotify( (RPTLOG_MSG_LAST*)(void*)FBuffer );
							 break;
	}
	FCommandCriticalSection.Leave();
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::SendSeqNotify( RPTLOG_MSG_SEQ* MsgSeq )
{
	AnsiString FCMNo;
	if( MsgSeq->Ap_Code == '3')
		FCMNo = AnsiString( MsgSeq->Broker_ID, 7 );
	else	
		FCMNo = AnsiString( MsgSeq->Broker_ID, 4 );
	ConnectionObjectBase* Co = GetConnection( FCMNo, MsgSeq->Ap_Code );
	
	if( Co != NULL )
	{
		printf("---------------------------------------------\n");
		printf("Broker = %s, Sequence = %d\n",FCMNo.c_str(),MsgSeq->Current_Seq);
		printf("---------------------------------------------\n");
		Co->OnCommand( CONNECTION_CCH_UPDATE_SEQ, (void*) MsgSeq->Current_Seq );
	}
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::SendLastNotify( RPTLOG_MSG_LAST* LastMsg )
{
	AnsiString FCMNo;
	if( LastMsg->Ap_Code == '3')
		FCMNo = AnsiString( LastMsg->Broker_ID, 7 );
	else	
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
void TAIFEXConfirmManager::ResetPVC( int PVC )
{
	ConnectionObjectBase* co = GetConnection( PVC );
	if( co != NULL )
	{
		TFXLog.MSG( XBIO3_INFO,"Unlock PVC %d.\n", PVC);
		co->OnCommand( COMMAND_UNLOCK_PVC, NULL );
	}
}
//-----------------------------------------------------------------------------------------------------------------------
int TAIFEXConfirmManager::GetPVCState( int PVC )
{
	int State = PVC_STATUS_UNKNOWNPVC;
	ConnectionObjectBase* co = GetConnection( PVC );
	if( co != NULL )
		co->OnCommand( COMMAND_GET_STATE, (void*) &State );
	return State;
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::CreateConnectionObject( AnsiString BrokerID, AnsiString Pwd, int PVC, char APCode )
{
	ConnectionObjectBase* NewCobj = GetConnection( PVC );
	ConnectionParameter   cp( FLinkNumber , PVC, APCode, BrokerID, Pwd );
	
	if( NewCobj == NULL )
	{
		TFXLog.MSG( XBIO_INFO, "FCM:%s Create a new backup connection, PVC:%d ",BrokerID.c_str(),PVC);
		FThreadCriticalSection.Enter();
		FConnectionObjects.Add((void*)new TAIFEXConfirmConnectionObject( cp, this, this ));
		FThreadCriticalSection.Leave();
	}
	else
	{
		TFXLog.MSG( XBIO_INFO, "PVC:%d Change from FCM:%s to FCM:%s",PVC,NewCobj->GetConnectionParameter()->GetBrokerID().c_str(),BrokerID.c_str());
		NewCobj->UpdateConnectionParameter( cp );
	}
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmManager::StopConnection( int PVC )
{
	ConnectionObjectBase* lCobj = GetConnection( PVC );
	if( lCobj != NULL )
	{
		ConnectionParameter* cp = lCobj->GetConnectionParameter();
		TFXLog.MSG( XBIO_INFO, "FCM:%s PVC %d perform backup procedure.(Locked)",cp->GetBrokerID().c_str(),PVC);
		lCobj->OnCommand( COMMAND_LOCK_PVC, NULL );
	}
}
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase* TAIFEXConfirmManager::GetConnection( int PVC )
{
	ConnectionParameter* cp;
	ConnectionObjectBase* co;

	FThreadCriticalSection.Enter();
	for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
	{
		co = static_cast<TAIFEXConfirmConnectionObject*>(FConnectionObjects.GetItem( i ));
		cp = co->GetConnectionParameter();
		if(  cp->GetPVC() == PVC )
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

	FThreadCriticalSection.Enter();
	if( APCode == '9')
	{
		for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
		{
			co = static_cast<TAIFEXConfirmConnectionObject*>(FConnectionObjects.GetItem( i ));
			cp = co->GetConnectionParameter();
			if(  cp->GetAPCode() == '9' && strncmp( BrokerID.c_str(), cp->GetBrokerID().c_str(),4) == 0 )
			{
				FThreadCriticalSection.Leave();
			    return co;	
			}
		}
	}
	else
	{
		for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
		{
			co = static_cast<TAIFEXConfirmConnectionObject*>(FConnectionObjects.GetItem( i ));
			cp = co->GetConnectionParameter();
			if(  cp->GetAPCode() == '3' && strncmp( BrokerID.c_str(), cp->GetBrokerID().c_str(), 7 ) == 0 )
			{
				FThreadCriticalSection.Leave();
			    return co;	
			}
		}
	}
	FThreadCriticalSection.Leave();
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
void TAIFEXConfirmManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* Data )
{
	ConnectionParameter* cp = CObj->GetConnectionParameter();
	int PVC = cp->GetPVC();
	AnsiString Broker = cp->GetBrokerID();
	
	switch( Reason )
	{
		case CONNECTION_OBJECT_X25ERROR:      TFXLog.MSG( XBIO3_ERROR, "FCM:%s PVC %d Can not establish the connection.(X.25 Retry %d times)",Broker.c_str(),PVC,PVC_BUSY_RETRY_TIMES );	
											  printf("PVC %d X.25 error.\n",PVC);
											  break;
		case CONNECTION_OBJECT_UNKNOWN_ERROR: TFXLog.MSG( XBIO3_ERROR,"FCM:%s PVC %d Unknown error.\n",Broker.c_str(),PVC);
											  printf("PVC %d Unknown error.\n",PVC);
											  break;
		case CONNECTION_OBJECT_PVC_READY:     TFXLog.MSG( XBIO3_INFO,"FCM:%s PVC %d Ready!.\n",Broker.c_str(),PVC);
											  *((int*)Data) = AddBrokerToCCH( cp );											  
											  printf("PVC %d Ready!.\n",PVC);
											  break;
		case CONNECTION_PVC_LOCKED:			  TFXLog.MSG( XBIO3_WARNING,"FCM:%s PVC %d Locked.\n",Broker.c_str(),PVC);
											  RemoveBrokerFromCCH( cp );
											  printf("PVC %d Locked.\n",PVC);
											  break;								  
		case CONNECTION_OBJECT_PVC_BROKEN:	  TFXLog.MSG( XBIO3_INFO,"FCM:%s PVC %d broken.\n",Broker.c_str(),PVC);
											  RemoveBrokerFromCCH( cp );
											  printf("PVC %d broken.\n",PVC);
											  break;								  	 									
		case CONNECTION_OBJECT_NORMAL_CLOSE:  TFXLog.MSG( XBIO3_INFO,"FCM:%s PVC %d Normal close.\n",Broker.c_str(),PVC);
											  RemoveBrokerFromCCH( cp );
											  printf("PVC %d Normal close.\n",PVC);
											  break;
		case CONNECTION_CONFIRM_FINASHED:     TFXLog.MSG( XBIO3_INFO,"FCM:%s PVC %d sent last confirm message.(Lock)\n",Broker.c_str(),PVC);
											  printf("PVC %d Sent last confirm message.\n",PVC);
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
void TAIFEXConfirmManager::Run()
{
	int FlushCount = 0;
	
	while( TRUE )
	{
		try
		{
			if( FOpenSwitch < OS_SHOUTDOWN_SYS  )
				OpenCCH();           // Open cch client.	
			while( FOpenSwitch < OS_SHOUTDOWN_SYS  )
			{
			  	GetReportNotify( );
			  	FlushCount++;
			  	fflush( stdout );
			  	if( FlushCount  >= 10 && Log != NULL )
			  	{
			  		FlushCount = 0;
			  		Log->Flush();
			  	}			  	
			}
			break;
		}
		catch( CCHException &e )
		{
			TFXLog.MSG( XBIO3_ERROR,"CCH Exception:%s\n",e.what());
			printf("CCH Exception:%s\n",e.what());
			printf("Try to connect to RPTS after 5 seconds...\n");
			sleep( 5 );			
		}
		catch( ... )
		{
			TFXLog.MSG( XBIO3_UNKNOWN_ERR, "Unknow Exception.\n" );
			printf("Unknow Exception.\n");
			return;
		}
	}
	TFXLog.MSG( XBIO3_INFO,"XBIO3 Normal close.\n" );
	printf("-------------------------------------------------------------------\n");	
	printf("XBIO3 Normal close. %d threads alive.\n",PThread::GetThreadCount());
	printf("-------------------------------------------------------------------\n");	
	Fcch.cchClose();
}
//-----------------------------------------------------------------------------------------------------------------------


