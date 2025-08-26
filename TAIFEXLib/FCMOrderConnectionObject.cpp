#ident "@(#) $Id: FCMOrderConnectionObject.cpp,v 1.11 2004/05/21 10:34:20 ap27 Exp $"
#include "FCMOrderConnectionObject.h"
#include "../MessageOrderIssuer.h"
#include "Connection.h"
//----------------------------------------------------------------------------------------------------------------------
//
//  ### FCM side ###
//  Connection object for AP code 4 Order subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
FCMOrderConnectionObject::FCMOrderConnectionObject( ConnectionParameter& info,
                                                    ConnectionListener* CListener,
                                                    ThreadListener* TListener )
:ConnectionObjectBase( info ),
 PThread( TListener, TRUE ),
 FOrderFCM( NULL ),
 FOrderIssuer( NULL ),
 FConnectionListener( CListener ),
 FParams( info )
{
    Start();
}
//----------------------------------------------------------------------------------------------------------------------
FCMOrderConnectionObject::~FCMOrderConnectionObject()
{
    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//----------------------------------------------------------------------------------------------------------------------
void FCMOrderConnectionObject::SendReport( void )
{
	TOrderReport report;
    report.OrderCount = FOrderIssuer->GetOrderCount();
    report.UseMS      = FOrderIssuer->GetUsetime();
    FConnectionListener->OnConnectionNotify( this, CONNECTION_ORDER_REPORT, &report );// Send report.
}
//----------------------------------------------------------------------------------------------------------------------
void FCMOrderConnectionObject::Execute( void )
{
	for( int i = 0; i< PVC_BUSY_RETRY_TIMES; i++ )
	{
		try
		{
			FOrderFCM    = new FCMOrderClient( FParams );
            FOrderIssuer = new MessageOrderIssuer( FOrderFCM, FParams );
			FOrderIssuer->OpenMarket();
            SendReport();
			delete FOrderIssuer;
			FOrderIssuer = NULL;
		  	delete FOrderFCM;
		  	FOrderFCM = NULL;
		  	return;
		}
		catch( FileException& e )
		{
			printf( "%s\n", e.what() );
			return;
		}
		catch( X25Exception& e )
		{
			if( FOrderIssuer != NULL )
			{
				delete FOrderIssuer;
				FOrderIssuer = NULL;
			}
			if( FOrderFCM  != NULL )
			{
				delete FOrderFCM ;
				FOrderFCM = NULL;
			}
			printf("%s. Try to reconnect after 5 sec....\n",e.what() );
			sleep( 5 );
			continue;
		}
		catch(...)
		{
			FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
			return;
		}
	}
	FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR );// Can not establish the X.25 connection
	return;
}
//----------------------------------------------------------------------------------------------------------------------
