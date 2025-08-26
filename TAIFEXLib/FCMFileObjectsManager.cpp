#ident "@(#) $Id: FCMFileObjectsManager.cpp,v 1.4 2004/05/21 10:34:20 ap27 Exp $"
#include "FCMFileObjectsManager.h"
//-----------------------------------------------------------------------------------------------------------------------
//
//  FCM side( FCMObjectsManeger )
//
//  Objects Manager for AP code 1 File subsystem
//
//-----------------------------------------------------------------------------------------------------------------------
FCMFileObjectsManager::FCMFileObjectsManager(  int LinkNumber, char* IP, int Port )
:FLinkNumber( LinkNumber ),
 FIPAddress( IP ),
 FPort( Port )
{
}
//-----------------------------------------------------------------------------------------------------------------------
FCMFileObjectsManager::~FCMFileObjectsManager()
{
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMFileObjectsManager::CreateConnectionObjects( )
{
	
	FFCMCount = GetFCMs( FLinkNumber,'1',(char*)DB_SERVER.c_str(), (char*)DB_USERNAME.c_str(), (char*)DB_PASSWORD.c_str(), (char*)DB_NAME.c_str(), FALSE );
	
	for( int i = 0; i < FFCMCount; i++ )
	{
		ConnectionParameter cp( FLinkNumber , GetPVC(i), '1', GetBrokerID(i), GetLinePassword(i) );
		FConnectionObjects.Add((void*)new FCMFileConnectionObject( cp, this, this, FIPAddress, FPort ));


	}
	
	FCMCount = GetFCMs( FLinkNumber,'2',(char*)DB_SERVER.c_str(), (char*)DB_USERNAME.c_str(), (char*)DB_PASSWORD.c_str(), (char*)DB_NAME.c_str(), FALSE );
	
	for( int i = 0; i < FCMCount; i++ )
	{
		ConnectionParameter cp( FLinkNumber , GetPVC(i), '2', GetBrokerID( i ), GetLinePassword(i) );
		FConnectionObjects.Add((void*)new FCMFileConnectionObject( cp, this, this, FIPAddress, FPort ));
		break;
	}

}
//-----------------------------------------------------------------------------------------------------------------------
void FCMFileObjectsManager::Run()
{
	do
	{
		sleep(1);
		fflush( stdout );
	}
	while( PThread::GetThreadCount() > 0 );
	printf("-------------------------------------------------------------------\n");		
	printf("XBIO1 Normal close. %d threads alive.\n",PThread::GetThreadCount());
	printf("-------------------------------------------------------------------\n");		
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMFileObjectsManager::OnTerminate( PThread*  )
{
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMFileObjectsManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* )
{
	ConnectionParameter* cp = CObj->GetConnectionParameter();
	switch( Reason )
	{
		case CONNECTION_OBJECT_NORMAL_CLOSE:   printf("PVC %d Normal close.\n",cp->GetPVC());break;
		case CONNECTION_OBJECT_X25ERROR:       printf("PVC %d X.25 error.\n",cp->GetPVC());break;
		case CONNECTION_OBJECT_UNKNOWN_ERROR:  printf("PVC %d Unknown error.\n",cp->GetPVC());break;
		case CONNECTION_OBJECT_PVC_READY:      printf("PVC %d Ready!.\n",cp->GetPVC());break;
	}
}
//-----------------------------------------------------------------------------------------------------------------------
