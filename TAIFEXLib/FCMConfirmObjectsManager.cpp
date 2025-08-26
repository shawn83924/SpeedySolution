#ident "@(#) $Id: FCMConfirmObjectsManager.cpp,v 1.17 2004/05/21 10:34:20 ap27 Exp $"
#include "FCMConfirmObjectsManager.h"
#include "XBIODefine.h"
//-----------------------------------------------------------------------------------------------------------------------
//
//  FCM side( FCMObjectsManeger )
//
//  Objects Manager for AP code 3 Confirm subsystem
//
//-----------------------------------------------------------------------------------------------------------------------
FCMConfirmManager::FCMConfirmManager(  int LinkNumber  )
:FLinkNumber( LinkNumber )
{
	AnsiString LogFileName = "Confirm" + GetDateString()+".3."+AnsiString( LinkNumber );
	Log  = new LogManager( LogFileName );
}
//-----------------------------------------------------------------------------------------------------------------------
FCMConfirmManager::~FCMConfirmManager()
{
	if( Log != NULL )
	{
		delete Log;
		Log = NULL;
	}
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMConfirmManager::CreateConnectionObjects( void )
{
	FFCMCount = GetFCMs( FLinkNumber,'3', (char*)DB_SERVER.c_str(), (char*)DB_USERNAME.c_str(), (char*)DB_PASSWORD.c_str(), (char*)DB_NAME.c_str(), FALSE );

	for( int i = 0; i < FFCMCount; i++ )
	{
		ConnectionParameter cp( FLinkNumber , GetPVC(i), '3', GetBrokerID(i), GetLinePassword(i) );
		FConnectionObjects.Add((void*)new FCMConfirmConnectionObject( cp, this, this,'3' ));
	}

	FCMCount = GetFCMs( FLinkNumber,'9', (char*)DB_SERVER.c_str(), (char*)DB_USERNAME.c_str(), (char*)DB_PASSWORD.c_str(),(char*)DB_NAME.c_str(), FALSE );

	for( int i = 0; i < FCMCount; i++ )
	{
		ConnectionParameter cp( FLinkNumber , GetPVC(i), '9', GetBrokerID( i ), GetLinePassword(i) );
		FConnectionObjects.Add((void*)new FCMConfirmConnectionObject( cp, this, this,'9'));
	}

	FOrderCMCount  = GetFCMs( FLinkNumber,'8', (char*)DB_SERVER.c_str(), (char*)DB_USERNAME.c_str(), (char*)DB_PASSWORD.c_str(),(char*)DB_NAME.c_str(), FALSE );

	for( int i = 0; i < FCMCount; i++ )
	{
		ConnectionParameter cp( FLinkNumber , GetPVC(i), '8', GetBrokerID( i ), GetLinePassword(i) );
		FConnectionObjects.Add((void*)new FCMConfirmConnectionObject( cp, this, this,'8'));
	}
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMConfirmManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* )
{
	ConnectionParameter* cp = CObj->GetConnectionParameter();
	switch( Reason )
	{
		case CONNECTION_OBJECT_NORMAL_CLOSE:  printf("PVC %d Normal close.\n",cp->GetPVC());break;
		case CONNECTION_OBJECT_X25ERROR:      printf("PVC %d X.25 error.\n",cp->GetPVC());break;
		case CONNECTION_OBJECT_UNKNOWN_ERROR: printf("PVC %d Unknown error.\n",cp->GetPVC());break;
		case CONNECTION_OBJECT_PVC_READY:     printf("PVC %d Ready!.\n",cp->GetPVC());break;
	}
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMConfirmManager::Run()
{
	do
	{
		sleep(1);
		fflush( stdout );
	}
	while( PThread::GetThreadCount() > 0 );
	printf("-------------------------------------------------------------------\n");
	printf("XBIO3 Normal close. %d threads alive.\n",PThread::GetThreadCount());
	printf("-------------------------------------------------------------------\n");
}
//-----------------------------------------------------------------------------------------------------------------------
