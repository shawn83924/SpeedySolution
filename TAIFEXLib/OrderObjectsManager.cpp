#ident "@(#) $Id: OrderObjectsManager.cpp,v 1.23 2004/05/21 10:34:21 ap27 Exp $"
#include "OrderObjectsManager.h"
//-----------------------------------------------------------------------------------------------------------------------
//
//  TAIFEX side( TAIFEXOrderManeger )
//
//  Objects Manager for AP code 4 Order subsystem
//  
//-----------------------------------------------------------------------------------------------------------------------
TAIFEXOrderManager::TAIFEXOrderManager( int LinkNumber, char* TAPIPFile, char* QIFIPFile )
:FLinkNumber( LinkNumber ),
 FGroupCount( 0 ),
 FOpenSwitch( 0 ),
 FOrderCheck( NULL )
{
	AnsiString LogFileName = "x" + GetDateString()+".4."+AnsiString( FLinkNumber );
	
	Log                = new LogManager( LogFileName );  
	FTransactionObject = new OrderTransactionObject( FLinkNumber, TAPIPFile, QIFIPFile );	
}
//-----------------------------------------------------------------------------------------------------------------------
TAIFEXOrderManager::~TAIFEXOrderManager()
{
	if( FTransactionObject != NULL )
		delete FTransactionObject;
	if( FOrderCheck != NULL )
		delete FOrderCheck; 
	if( Log != NULL )		
	{
		delete Log;
		Log = NULL;
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
	FOrderCheck = new OrderCheck( FLinkNumber, (char*)UserName.c_str(),(char*)Password.c_str(),(char*)DBServerName.c_str(), (char*)DatabaseName.c_str());
	FOrderCheck->SetDBFilename( DBFileName );
	if( FOrderCheck->LoadDB_Data() != 0 ) 
	{
		TFXLog.MSG( XBIO_INITIAL_ERROR, "TAP Client load data from DB error.\n");
		exit( 1 );
	}
	FOrderCheck->set_bOnlyTestXBIO( TAP_TEST_MODE );
	FTransactionObject->SetOrderCheckObj( FOrderCheck );
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::CreateConnectionObjects( AnsiString Server, AnsiString ID,AnsiString Pwd,AnsiString DBName,BOOL IsSim )
{
	int Count = GetFCMs( FLinkNumber,'4',(char*)Server.c_str(),(char*)ID.c_str(),(char*)Pwd.c_str(),(char*)DBName.c_str(),IsSim );
		
	for( int i = 0; i < Count ; i++ )
	{
		ConnectionParameter cp( FLinkNumber , GetPVC(i), '4', GetBrokerID(i), GetLinePassword(i) );
		FCriticalSection.Enter();
		FConnectionObjects.Add((void*)new TAIFEXOrderConnectionObject( cp, FTransactionObject, this, this ));
		FCriticalSection.Leave();
	}
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::CreateConnectionObject( AnsiString BrokerID, AnsiString Pwd, int PVC )
{
	ConnectionObjectBase* NewCobj = GetConnection( PVC );
	ConnectionParameter   cp( FLinkNumber , PVC, '4', BrokerID, Pwd );
	
	if( NewCobj == NULL )
	{
		TFXLog.MSG( XBIO_INFO, "FCM:%s Create a new backup connection, PVC:%d ",BrokerID.c_str(),PVC);
		FCriticalSection.Enter();
		FConnectionObjects.Add((void*)new TAIFEXOrderConnectionObject( cp, FTransactionObject, this, this ));
		FCriticalSection.Leave();
	}
	else
	{
		TFXLog.MSG( XBIO_INFO, "PVC:%d Change from FCM:%s to FCM:%s",PVC,NewCobj->GetConnectionParameter()->GetBrokerID().c_str(),BrokerID.c_str());
		NewCobj->UpdateConnectionParameter( cp );
	}
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::ResetPVC( int PVC )
{
	ConnectionObjectBase* co = GetConnection( PVC );
	if( co != NULL )
	{
		TFXLog.MSG( XBIO_INFO, "Unlock PVC %d.\n", PVC);
		co->OnCommand( COMMAND_UNLOCK_PVC, NULL );
	}
}
//-----------------------------------------------------------------------------------------------------------------------
int TAIFEXOrderManager::GetPVCState( int PVC )
{
	int State = PVC_STATUS_UNKNOWNPVC;
	ConnectionObjectBase* co = GetConnection( PVC );
	if( co != NULL )
		co->OnCommand( COMMAND_GET_STATE, (void*) &State );
	return State;
}
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase* TAIFEXOrderManager::GetConnection( int PVC )
{
	ConnectionParameter* cp;
	ConnectionObjectBase* co;

	FCriticalSection.Enter();
	for( register int i = 0; i < FConnectionObjects.ItemCount();i++ )
	{
		co = static_cast<TAIFEXOrderConnectionObject*>(FConnectionObjects.GetItem( i ));
		cp = co->GetConnectionParameter();
		if(  cp->GetPVC() == PVC )
		{
			FCriticalSection.Leave();
		    return co;
		}
	}
	FCriticalSection.Leave();
	return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::StopConnection( int PVC )
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
void TAIFEXOrderManager::OnTerminate( PThread* TerminateThread  )
{
	FCriticalSection.Enter();
	FConnectionObjects.Remove((void*)TerminateThread);
	FCriticalSection.Leave();
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* )
{
	ConnectionParameter* cp = CObj->GetConnectionParameter();
	int PVC = cp->GetPVC();
	AnsiString Broker = cp->GetBrokerID();
		
	switch( Reason )
	{
		case CONNECTION_OBJECT_X25ERROR:      TFXLog.MSG( XBIO_ERROR, "FCM:%s PVC %d Can not establish the connection.(X.25 Retry %d times)",Broker.c_str(),PVC,PVC_BUSY_RETRY_TIMES );	
											  printf("PVC %d X.25 error.\n", PVC);
											  break;
		case CONNECTION_OBJECT_UNKNOWN_ERROR: TFXLog.MSG( XBIO_ERROR, "FCM:%s PVC %d Unknown error.(Locked)\n",Broker.c_str(),PVC);
										      printf("PVC %d Unknown error.\n", PVC);
											  break;
		case CONNECTION_OBJECT_PVC_READY:     TFXLog.MSG( XBIO_INFO,"FCM:%s PVC %d Ready!.\n",Broker.c_str(),PVC);
											  printf("PVC %d Ready!.\n",PVC);
											  break;
		case CONNECTION_PVC_LOCKED:			  TFXLog.MSG( XBIO_WARNING,"FCM:%s PVC %d Locked.\n",Broker.c_str(),PVC);
											  printf("PVC %d Locked.\n",PVC);
											  break;								 
		case CONNECTION_OBJECT_PVC_BROKEN:	  TFXLog.MSG( XBIO_INFO,"FCM:%s PVC %d broken.\n",Broker.c_str(),PVC);
											  printf("PVC %d broken.\n",PVC);
											  break;								  	 																				  
		case CONNECTION_OBJECT_NORMAL_CLOSE:  TFXLog.MSG( XBIO_INFO, "FCM:%s PVC %d Normal close.\n",Broker.c_str(),PVC);
											  printf("PVC %d Normal close.\n", PVC);
											  break;											   											
	}
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderManager::Run()
{
	int FlushCount = 0;
	do
	{
		sleep( 1 );
		FlushCount ++;
		fflush( stdout );
		if( FlushCount >= 10 && Log != NULL )
		{
			FlushCount = 0;
			Log->Flush();
		}
		
	}while( FOpenSwitch < OS_SHOUTDOWN_SYS );
	
	FTransactionObject->Report();
	Log->Flush();
	TFXLog.MSG( XBIO_INFO, "XBIO4 Close market.\n" );
	printf("-------------------------------------------------------------------\n");	
	printf("XBIO Close market OS:%d. %d threads alive.\n",FOpenSwitch, PThread::GetThreadCount());
	printf("-------------------------------------------------------------------\n");	
}
//----------------------------------------------------------------------------------------------------------------------

