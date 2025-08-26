#ident "@(#) $Id: FCMOrderObjectsManager.cpp,v 1.16 2004/05/25 08:07:55 ap27 Exp $"
#include "FCMOrderObjectsManager.h"
#include "XBIODefine.h"
//-----------------------------------------------------------------------------------------------------------------------
//
//  FCM side( FCMObjectsManeger )
//
//  Objects Manager for AP code 4 Order subsystem
//
//-----------------------------------------------------------------------------------------------------------------------
FCMOrderManager::FCMOrderManager( int LinkNumber, int IssuerType, char* StringData, int IntData )
:FTotalOrder( 0 ),
 FTotalTime( 0 ),
 FLinkNumber( LinkNumber ),
 FIssuerType( IssuerType ),
 FInteger( IntData ),
 FMaxTime( 0 ),
 FString( StringData )
{
	Log = new LogManager( "Benchmark.txt");
}
//-----------------------------------------------------------------------------------------------------------------------
FCMOrderManager::~FCMOrderManager()
{
	delete Log;
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMOrderManager::CreateConnectionObjects( int MaxPVC )
{
	FFCMCount = GetFCMs( FLinkNumber,'4',(char*)DB_SERVER.c_str(), (char*)DB_USERNAME.c_str(), (char*)DB_PASSWORD.c_str(), (char*)DB_NAME.c_str(), FALSE );
	if( FIssuerType == BENCHMARK_ORDERISSUER )
	{
		if( MaxPVC != 0 )
		{
			if( MaxPVC < FFCMCount )
				FFCMCount = MaxPVC;
		}
		for( int i = 0; i < FFCMCount; i++ )
		{
			ConnectionParameter cp( FLinkNumber , GetPVC(i), '4', GetBrokerID(i), GetLinePassword(i) );
			FConnectionObjects.Add((void*)new FCMOrderConnectionObject( cp,this, this, FIssuerType, NULL, FInteger ));
		}
	}
	else if( FIssuerType == XBIO_LOG_ORDERISSUER)
	{
		for( int i = 0; i < FFCMCount; i++ )
		{
			ConnectionParameter cp( FLinkNumber , GetPVC(i), '4', GetBrokerID(i), GetLinePassword(i) );
			AnsiString FileName;

			FileName.Printf("x%s.%d.%d", FString,FLinkNumber,GetPVC(i));
			FConnectionObjects.Add((void*)new FCMOrderConnectionObject( cp,this, this, FIssuerType,(char*)FileName.c_str()));
		}
	}
	else  if( FIssuerType == ORDER_LOG_ORDERISSUER ) // Work with gSimulator....
	{
		for( int i = 0; i < FFCMCount; i++ )
		{
			ConnectionParameter cp( FLinkNumber , GetPVC(i), '4', GetBrokerID(i), GetLinePassword(i) );
			AnsiString FileName;

			FileName.Printf("History.%d.%d.txt", FLinkNumber, GetPVC(i) );
			FConnectionObjects.Add((void*)new FCMOrderConnectionObject( cp,this, this,(char*)FileName.c_str(),FString,FInteger ));
		}
	}
    else if( FIssuerType == MESSAGE_ORDERISSUER )
    {
        for( int i = 0; i < FFCMCount; i++ )
        {
            ConnectionParameter cp( FLinkNumber , GetPVC(i), '4', GetBrokerID(i), GetLinePassword(i) );
            FConnectionObjects.Add((void*)new FCMOrderConnectionObject( cp, this, this ));
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMOrderManager::Run()
{
	do
	{
		sleep( 1 );
		fflush( stdout );
	}
	while( PThread::GetThreadCount() > 0 );
	printf("-------------------------------------------------------------------\n");
	printf("%d PVC in Link %d.\n",FFCMCount,FLinkNumber);
	printf("Use          = %ld ms.\n",FTotalTime);
	printf("Total orders = %d.\n",FTotalOrder);
	printf("Average Time = %ld ms.\n",FTotalTime/FFCMCount );
	printf("-------------------------------------------------------------------\n");
	printf("%f orders per second.(PVC)\n", (float)FTotalOrder/((float)FTotalTime/1000.0));
	printf("%f orders per second.(Link)\n", (float)FTotalOrder/((float)(FTotalTime/FFCMCount)/1000.0));
	printf("-------------------------------------------------------------------\n");
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMOrderManager::OnTerminate( PThread*   )
{
	printf("%d Threads alive.\n",PThread::GetThreadCount());
}
//-----------------------------------------------------------------------------------------------------------------------
void FCMOrderManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* Data )
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
//-----------------------------------------------------------------------------------------------------------------------
