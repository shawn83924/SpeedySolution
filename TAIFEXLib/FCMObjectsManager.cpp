#include "FCMObjectsManager.h"
#include "FCMConfirmObjectsManager.h"
#include "XBIODefine.h"
#include "../Migo/Sigo.h"
//-----------------------------------------------------------------------------------------------------------------------
//
//  FCM side( FCMObjectsManeger )
//
//  Objects Manager for AP code 4 Order subsystem
//
//-----------------------------------------------------------------------------------------------------------------------
extern Int32      VALUE_HEARTBEAT;
extern AnsiString COMMAND_SUBJECT;
extern AnsiString COMMAND_KEY;
extern AnsiString ORDER_SUBJECT;
//-----------------------------------------------------------------------------------------------------------------------
extern MessageObject*  MessageObj;
extern BOOL IsOptions;
//-----------------------------------------------------------------------------------------------------------------------
FCMObjectsManager::FCMObjectsManager( int LinkNumber )
:FTotalOrder( 0 ),
 FTotalTime( 0 ),
 FLinkNumber( LinkNumber ),
 FMaxTime( 0 )
{
    if( MessageObj != NULL )
        MessageObj->AddListener( ORDER_SUBJECT, "all" ,this); 		///< Add Open/Switch Migo Message listener.
    FOrderQueue = new UFC::PtrQueue<OrderObj>( 120 );
}
//-----------------------------------------------------------------------------------------------------------------------
FCMObjectsManager::~FCMObjectsManager()
{
    delete FOrderQueue;
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMObjectsManager::CreateConnectionObjects( int MaxPVC )
{
	FFCMOrderPVCCount = GetFCMs( FLinkNumber,'4',(char*)DB_SERVER.c_str(), (char*)DB_USERNAME.c_str(), (char*)DB_PASSWORD.c_str(), (char*)DB_NAME.c_str(), FALSE );
    for( int i = 0; i < FFCMOrderPVCCount; i++ )
    {
         ConnectionParameter cp( FLinkNumber , GetPVC(i), '4', GetBrokerID(i), GetLinePassword(i) );
         FConnectionObjects.Add((void*)new FCMOrderConnectionObject( cp, this, this ));
    }

    FFCMConfirmPVCCount = GetFCMs( FLinkNumber,'3', (char*)DB_SERVER.c_str(), (char*)DB_USERNAME.c_str(), (char*)DB_PASSWORD.c_str(), (char*)DB_NAME.c_str(), FALSE );
    for( int i = 0; i < FFCMConfirmPVCCount; i++ )
    {
        ConnectionParameter cp( FLinkNumber , GetPVC(i), '3', GetBrokerID(i), GetLinePassword(i) );
        FConnectionObjects.Add((void*)new FCMConfirmConnectionObject( cp, this, this,'3' ));
    }

    FCMOrderConfirmPVCCount = GetFCMs( FLinkNumber,'8', (char*)DB_SERVER.c_str(), (char*)DB_USERNAME.c_str(), (char*)DB_PASSWORD.c_str(), (char*)DB_NAME.c_str(), FALSE );
    for( int i = 0; i < FCMOrderConfirmPVCCount; i++ )
    {
        ConnectionParameter cp( FLinkNumber , GetPVC(i), '8', GetBrokerID(i), GetLinePassword(i) );
        FConnectionObjects.Add((void*)new FCMConfirmConnectionObject( cp, this, this,'8' ));
    }

    FCMConfirmPVCCount = GetFCMs( FLinkNumber,'9', (char*)DB_SERVER.c_str(), (char*)DB_USERNAME.c_str(), (char*)DB_PASSWORD.c_str(), (char*)DB_NAME.c_str(), FALSE );
    for( int i = 0; i < FCMConfirmPVCCount; i++ )
    {
        ConnectionParameter cp( FLinkNumber , GetPVC(i), '9', GetBrokerID(i), GetLinePassword(i) );
        FConnectionObjects.Add((void*)new FCMConfirmConnectionObject( cp, this, this,'9' ));
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMObjectsManager::Run()
{
    MTree Data;
    MString CommandFieldName("COMMAND");

    Data.append( CommandFieldName, VALUE_HEARTBEAT );
    while( TRUE )
    {
	sleep( 1 );
	fflush( stdout );
        MessageObj->Send( COMMAND_SUBJECT, COMMAND_KEY, Data );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMObjectsManager::OnTerminate( PThread*   )
{
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMObjectsManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* Data )
{
	ConnectionParameter* cp = CObj->GetConnectionParameter();
	int UseTime;
	switch( Reason )
	{
		case CONNECTION_OBJECT_NORMAL_CLOSE:  printf("PVC %d Normal close.\n",cp->GetPVC());break;
		case CONNECTION_OBJECT_X25ERROR:      printf("PVC %d X.25 error.\n",cp->GetPVC());break;
		case CONNECTION_OBJECT_UNKNOWN_ERROR: printf("PVC %d Unknown error.\n",cp->GetPVC());break;
		case CONNECTION_ORDER_REPORT:         UseTime = ((TOrderReport*)Data)->UseMS;
											  if( UseTime > FMaxTime )
											      FMaxTime = UseTime;
											  FTotalOrder += ((TOrderReport*)Data)->OrderCount;
											  FTotalTime +=	 UseTime;
											  break;
	}
}
//---------------------------------------------------------------------------------------------------------------------
void FCMObjectsManager::OnMigoMessage( AnsiString Subject, AnsiString Key,  MTree* Data )
{
    MNode          mNode;
    MString        OrderFieldName("ORDER");

    if( Data->get( OrderFieldName, mNode ) == TRUE )
        FOrderQueue->Inqueue( new OrderObj( (const char*)Key.c_str(),(const char*) mNode.getData()) );
}
//-----------------------------------------------------------------------------------------------------------------------
