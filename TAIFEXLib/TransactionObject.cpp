 #ident "@(#)$Id: TransactionObject.cpp,v 1.130 2004/07/12 09:26:04 ap25 Exp $"
/**@file TransactionObject.cpp
*  @brief Core Class to process all kinds of orders.
*  @author Simon Chaang
*
*/
#include "TransactionObject.h"
#include "ConnectionObject.h"
//----------------------------------------------------------------------------------------------------------------------
extern MessageObject* MessageObj;
#define SIM_TAIFEX

#ifdef SIM_TAIFEX
  #include "../SimTAIFEX/SimMatch.h"
  int    FQuerySeq = 0;
  extern SimMatch*  MatchObj;
#endif
//----------------------------------------------------------------------------------------------------------------------
// OrderTransactionObject constructor
//----------------------------------------------------------------------------------------------------------------------
OrderTransactionObject::OrderTransactionObject( void )
:PThread( NULL ),
 FOpenSwitch( 0 ),
 FSendMark( 0 ),
 FOrderSender( NULL ),
 FOrderCheck( NULL ),
 FMaxResponseTimePVC( 0 ), FMaxResponseTime( 0 ),
 FOrderCount1( 0 ), FOrderCount2( 0 ), FOrderCount3( 0 ), FOrderCount5( 0 ),
 FQueryCount1( 0 ), FQueryCount2( 0 ), FQueryCount3( 0 ), FQueryCount5( 0 ),
 FOrderTotalTime( 0 ), FQueryTotalTime( 0 ),
 FOrderIntervalCount( 0 ), FQueryIntervalCount( 0 ), FOrderIntervalTime( 0 ),FQueryIntervalTime( 0 ),
 FConnected( FALSE )
{
    FReturnMsg   = new MessageHeaderPtr[ MAX_PVC ];
    FEvents      = new PEvent[ MAX_PVC ];
    FR050        = new MessageR050[ MAX_PVC ];
    FR020        = new MessageR020[ MAX_PVC ];
    FR100        = new MessageR100[ MAX_PVC ];
    FR030        = new MessageR030[ MAX_PVC ];
    FR080        = new MessageR080[ MAX_PVC ];
    FR060Count   = new int[ MAX_PVC ];
    FIndex       = new int[ MAX_PVC ];
    FReceiveTime = new unsigned int[ MAX_PVC ];
    FOrderSender = new OrderSender( );

    memset( FIndex, 0, sizeof( int ) * MAX_PVC );
    memset( FReturnMsg, 0, sizeof( MessageHeader* ) * MAX_PVC );
    memset( FR060Count, 0, sizeof( int ) * MAX_PVC );

    FOrderSender->Set_bOnlyTestXBIO( TAP_TEST_MODE );
    MessageObj->AddListener( OQ_QUERY_SUBSCRIBER,  "all", this ); ///< R020,R100
    MessageObj->AddListener( Q_REQUEST_SUBSCRIBER, "all", this ); ///< R080
    
    
    int FFCMCount = GetFCMs( '4' );
    
    for( int i = 0; i < FFCMCount ; i++ )
        FLUT[ GetLink( i ) *100 +  GetPVC( i )] = i;

}
//----------------------------------------------------------------------------------------------------------------------
// OrderTransactionObject destructor
//----------------------------------------------------------------------------------------------------------------------
OrderTransactionObject::~OrderTransactionObject()
{
    delete FOrderSender;
    delete [] FReceiveTime;
    delete [] FIndex;
    delete [] FR060Count;
    delete [] FR080;
    delete [] FR030;
    delete [] FR100;
    delete [] FR020;
    delete [] FR050;
    delete [] FEvents;
    delete [] FReturnMsg;
}
//----------------------------------------------------------------------------------------------------------------------
//  Set the OrderCheck object referance.
//----------------------------------------------------------------------------------------------------------------------
void OrderTransactionObject::SetOrderCheckObj( OrderCheck* CheckObj )
{
	FOrderCheck = CheckObj;
	Start();
}
//----------------------------------------------------------------------------------------------------------------------
// Migo messages callback function
//----------------------------------------------------------------------------------------------------------------------
void OrderTransactionObject::OnMigoMessage( AnsiString Subject, AnsiString,  MTree* Data )
{
    if( Subject.AnsiCompare( OQ_QUERY_SUBSCRIBER ) == 0 ) // R020,R100
        ReceiveR020R100( Data );
    else if( Subject.AnsiCompare( Q_REQUEST_SUBSCRIBER ) == 0 ) //R080
        ReceiveR080( Data );
    else
        printf( "Unknow replay message from bus.( Not R020,R100,R080 )\n" );
}
//----------------------------------------------------------------------------------------------------------------------
// Received R020,R100 from Migo
//----------------------------------------------------------------------------------------------------------------------
void OrderTransactionObject::ReceiveR020R100( MTree* Data )
{
    char*   Body;
    MNode   Node;
    char    OQCode;
    int     PVC,Status;

    Data->get( "PVC_No",  PVC );
    Data->get( "Ret_Status",  Status );
    if( Status == 0 )
    {
        Data->get( "OQ_Code", Node );
        OQCode = *(( char* )Node.getData( ));
        Data->get( "Body", Node );
        Body   = ( char* )Node.getData( );
        printf( "Get Message: %s\n", Body );
        if( OQCode == 'O')
        {
                FR020[ PVC-1 ].LoadBody( Body );
                FR020[ PVC-1 ].SetFunctionCode( 5 );
                FReturnMsg[ PVC-1 ] = &FR020[ PVC-1 ];
        }
        else
        {
                FR100[ PVC-1 ].LoadBody( Body );
                FR100[ PVC-1 ].SetFunctionCode( 5 );
                FReturnMsg[ PVC-1 ] = &FR100[ PVC-1 ];
        }
    }
    else
    {
        FR030[ PVC - 1 ].SetStatusCode( Status );
        FReturnMsg[ PVC-1 ] = &FR030[ PVC - 1 ];
    }
    FEvents[ PVC-1 ].SetEvent();
}
//----------------------------------------------------------------------------------------------------------------------
// Received R080 from Migo
//----------------------------------------------------------------------------------------------------------------------
void OrderTransactionObject::ReceiveR080( MTree* Data )
{
    char*   Body;
    MNode   Node;
    int     PVC,Status;

    Data->get( "PVC_No", PVC );
    Data->get( "Ret_Status", Status );
    if( Status == 0 )
    {
        Data->get( "Body", Node );
        Body  = (char*)Node.getData();
        FR080[ PVC-1 ].LoadBody( Body );
        FReturnMsg[ PVC-1 ] = &FR080[ PVC-1 ];
    }
    else
    {
        FR030[ PVC-1 ].SetStatusCode( Status );
        FReturnMsg[ PVC-1 ] = &FR030[ PVC - 1 ];
    }
    FEvents[ PVC-1 ].SetEvent();
}
//----------------------------------------------------------------------------------------------------------------------
// Send R070 to Migo Bus.
//----------------------------------------------------------------------------------------------------------------------
void OrderTransactionObject::SendR070( MessageR070* R070, int PVC )
{
     MString      FCM( R070->GetFCM().c_str());
     MString      ProdID( R070->GetCommodityId().c_str() );
     MTree        SendData;

     SendData.append( "FCM_No", FCM );
     SendData.append( "Commodity_ID", ProdID );
     //SendData.append( "Link_No", FLink );
     SendData.append( "PVC_No", PVC );
     MessageObj->Send( Q_REQUEST_PUBLISHER, "all" , SendData );
     printf("\n** Is a quote request message( R070 ).\n");
}
//----------------------------------------------------------------------------------------------------------------------
// Send R010 function code 5 order query to Migo Bus.
//----------------------------------------------------------------------------------------------------------------------
BOOL OrderTransactionObject::SendOrderQuery( MessageR010* R010, int PVC ) // R010 function 5
{
     MString      FCM( R010->GetFCM().c_str());
     MString      OrderNo( R010->GetOrderNo().c_str() );
     MTree        SendData;
     int          Group =  GroupOf( (char*)R010->GetCommodityId().c_str() );
     char         GroupStr[ 8 ];

     if( Group > 0 )
     {
         sprintf( GroupStr, "%d", Group );
         SendData.append( "FCM_No", FCM );
         SendData.append( "Order_No", OrderNo );
         SendData.append( "OQ_Code", "O" );
         //SendData.append( "Link_No", FLink );
         SendData.append( "PVC_No", PVC );
         MessageObj->Send( OQ_QUERY_PUBLISHER, GroupStr ,SendData );
         printf("\n** Is a order quote message( R010 function 5 ).\n");
         return TRUE;
     }
     printf("## R010 Can't find Product:%s FCM:%s OrderNo:%s ##\n",R010->GetCommodityId().c_str(),R010->GetFCM().c_str(),R010->GetOrderNo().c_str());
     return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
// Send R090 function code 5 quote query to Migo Bus.
//----------------------------------------------------------------------------------------------------------------------
BOOL OrderTransactionObject::SendQuoteQuery( MessageR090* R090, int PVC ) // R090 function 5
{
     MString      FCM( R090->GetFCM().c_str());
     MString      OrderNo( R090->GetOrderNo().c_str() );
     MTree        SendData;
     int          Group =  GroupOf( (char*)R090->GetCommodityId().c_str() );
     char         GroupStr[ 8 ];

     if( Group > 0 )
     {
         sprintf( GroupStr, "%d", Group );
         SendData.append( "FCM_No", FCM );
         SendData.append( "Order_No", OrderNo );
         SendData.append( "OQ_Code", "Q");
         //SendData.append( "Link_No", FLink );
         SendData.append( "PVC_No", PVC );
         MessageObj->Send( OQ_QUERY_PUBLISHER, GroupStr ,SendData );
         printf("\n** Is a quote query message( R090 function code 5 ).\n");
         return TRUE;
     }
     printf("## R090 Can't find Product:%s FCM:%s OrderNo:%s ##\n",R090->GetCommodityId().c_str(),R090->GetFCM().c_str(),R090->GetOrderNo().c_str());
     return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
// Function to handle the R010 message
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* OrderTransactionObject::OnR010Arrived( MessageR010* InMsg, int PVCZeroBase, AnsiString& BrokerID )
{
    int              ErrCode;
    char*            RtnMsg;

    if( CheckFCM( InMsg->GetFCM().c_str(), BrokerID.c_str() ) == FALSE )
    {
        UFC::BufferedLog::Printf( " BrokerID Error FCM:%s Err FCM:%s.", BrokerID.c_str(), InMsg->GetFCM().c_str() );
        return ReturnErrorMessage( PVCZeroBase, 12 );
    }
    if( FOpenSwitch < OS_ACCEPT_ORDER )
        return ReturnErrorMessage( PVCZeroBase, 2 );
    if( FOpenSwitch >= OS_REJECT_ORDER )
        return ReturnErrorMessage( PVCZeroBase, 1 );
    FReceiveTime[ PVCZeroBase ] = GetTimeMS();
    if( InMsg->GetFunctionCode() == 5 ) /// Query Order
    {
        FOrderCount5++;
        FQueryIntervalCount++;
     #ifdef SIM_TAIFEX
         int ercode;
         if( (ercode = MatchObj->QueryOrder( InMsg, &FR020[ PVCZeroBase ], FQuerySeq ))!= 0)
             return ReturnErrorMessage( PVCZeroBase, ercode );
         return &FR020[ PVCZeroBase ];
     #else
         if( SendOrderQuery( InMsg, PVCZeroBase+1 ) == FALSE ) // Order query message.( Migo Message )
             return ReturnErrorMessage( PVCZeroBase, 20 );
         return WaitMessage( PVCZeroBase, otQuery, InMsg->OrderNo,"R010", FALSE );
     #endif
    }
    else /// Insert, Reduce, Cancel Order
    {
        FOrderIntervalCount++;
        switch( InMsg->GetFunctionCode() )
        {
                case 1: FOrderCount1++; break;
                case 2: FOrderCount2++; break;
                case 3: FOrderCount3++; break;
        }
        RtnMsg = FOrderCheck->CheckOrder( InMsg, (char*)BrokerID.c_str(), PVCZeroBase+1, ErrCode );
        if( ErrCode != 0 ) ///< Error order
                return ReturnErrorMessage( PVCZeroBase, ErrCode );
        if( FConnected == FALSE )
            return ReturnErrorMessage( PVCZeroBase, 99 );
        if( AddMessage( RtnMsg ) < 0 ) // Order message.
            return ReturnErrorMessage( PVCZeroBase, 99 );
        MessageHeader* ReplyMsg = WaitMessage( PVCZeroBase, otOrder, InMsg->OrderNo,"R010" );
        if( FOrderCount1%9 == 0 )
            UFC::SleepMS( 800 );
        return ReplyMsg;
    }
}
//----------------------------------------------------------------------------------------------------------------------
// Function to handle the R040 message
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* OrderTransactionObject::OnConfirmConnectionHandshake( int PVCZeroBase )
{
	FR050[ PVCZeroBase ].UpdateTime( );
	return &FR050[ PVCZeroBase ];
}
//----------------------------------------------------------------------------------------------------------------------
// Function to handle the R060 message
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* OrderTransactionObject::OnBrokenLineQuery( int PVCZeroBase )
{
	FR060Count[ PVCZeroBase ]++;
	return FOrderSender->GetQif060_RetMsg( PVCZeroBase+1 );
}
//----------------------------------------------------------------------------------------------------------------------
// Function to handle the R070 message
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* OrderTransactionObject::OnR070Arrived( MessageR070* InMsg, int PVCZeroBase, AnsiString& BrokerID )
{
	if( FOpenSwitch < OS_OPEN_MARKET )
		return ReturnErrorMessage( PVCZeroBase, 23 );
 	if( FOpenSwitch >= OS_REJECT_ORDER )
		return ReturnErrorMessage( PVCZeroBase, 1 );
	if( CheckFCM( InMsg->GetFCM().c_str(), BrokerID.c_str() )== FALSE )
		return ReturnErrorMessage( PVCZeroBase, 12 );
    #ifdef SIM_TAIFEX
      int ercode;
      if( (ercode = MatchObj->QueryPrice( InMsg, &FR080[ PVCZeroBase ]))!= 0)
          return ReturnErrorMessage( PVCZeroBase, ercode );
      return &FR080[ PVCZeroBase ];
    #else
	  SendR070( InMsg, PVCZeroBase+1  );
	  return WaitMessage( PVCZeroBase, otOther, NULL, "R070", FALSE );
    #endif
}
//----------------------------------------------------------------------------------------------------------------------
// Function to handle the R090 message
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* OrderTransactionObject::OnR090Arrived( MessageR090* InMsg, int PVCZeroBase, AnsiString& BrokerID )
{
	int              ErrCode;
	char*            RtnMsg;

	if( CheckFCM( InMsg->GetFCM().c_str(), BrokerID.c_str() )== FALSE )
	{
		UFC::BufferedLog::Printf( " BrokerID Error FCM:%s  Err FCM:%s.", BrokerID.c_str(), InMsg->GetFCM().c_str() );
		return ReturnErrorMessage( PVCZeroBase, 12 );
	}
	if( FOpenSwitch < OS_OPEN_MARKET )
		return ReturnErrorMessage( PVCZeroBase, 3 );
	if( FOpenSwitch >= OS_REJECT_ORDER )
		return ReturnErrorMessage( PVCZeroBase, 1 );
	FReceiveTime[ PVCZeroBase ] = GetTimeMS();
	if( InMsg->GetFunctionCode() == 5 )
	{
		FQueryCount5++;
		FQueryIntervalCount++;
        #ifdef SIM_TAIFEX
            int ercode;
            if( (ercode = MatchObj->QueryQuote( InMsg, &FR100[ PVCZeroBase ], FQuerySeq ))!= 0)
                return ReturnErrorMessage( PVCZeroBase, ercode );
            return &FR100[ PVCZeroBase ];
        #else
       		if( SendQuoteQuery( InMsg, PVCZeroBase + 1  ) == FALSE )  // Quote query message.( Migo Message )
		    	return ReturnErrorMessage( PVCZeroBase, 20 );
    		return WaitMessage( PVCZeroBase, otQuery, InMsg->OrderNo,"R090", FALSE );
        #endif
	}
	else
	{
		FOrderIntervalCount++;
		switch( InMsg->GetFunctionCode() )
		{
			case 1: FQueryCount1++; break;
			case 2: FQueryCount2++; break;
			case 3: FQueryCount3++; break;
		}
		RtnMsg = FOrderCheck->CheckOrder( InMsg, (char*)BrokerID.c_str(), PVCZeroBase + 1, ErrCode );
		if( ErrCode != 0 )
		{
			if( ErrCode == 12 )
				UFC::BufferedLog::Printf(" CheckOrder,BrokerID Error FCM:%s  Err FCM:%s", BrokerID.c_str(), InMsg->GetFCM().c_str() );
			return ReturnErrorMessage( PVCZeroBase, ErrCode );
		}
		if( FConnected == FALSE )
		    return ReturnErrorMessage( PVCZeroBase, 99 );
	 	if( AddMessage( RtnMsg ) < 0)
                    return ReturnErrorMessage( PVCZeroBase, 99 );
	 	return WaitMessage( PVCZeroBase, otOrder, InMsg->OrderNo, "R090" );
	}
}
//----------------------------------------------------------------------------------------------------------------------
void OrderTransactionObject::UpdateStates( int PVC0Base, POrderType type )
{
	if( type != otOther )
	{
		long Interval = GetTimeMS() - FReceiveTime[ PVC0Base ];

		if( Interval > FMaxResponseTime )
		{
			FMaxResponseTime    = Interval;
			FMaxResponseTimePVC = PVC0Base + 1;
		}
		if( type == otOrder )
		{
			FOrderIntervalTime += Interval;
	    	FOrderTotalTime    += Interval;
	    }
		else if( type ==  otQuery )
		{
	    	FQueryTotalTime    += Interval;
	    	FQueryIntervalTime += Interval;
	    }
	}
}
//----------------------------------------------------------------------------------------------------------------------
// Wait for the reply message.
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* OrderTransactionObject::WaitMessage( int PVC0Base, POrderType type, char* OrderNo,char* Str, BOOL IsTAP )
{
    char OrderNumber[ 6 ];
    
    if( OrderNo != NULL )
        strncpynt( OrderNumber, OrderNo, 5 );
    FIndex[ PVC0Base ]++;                                                     // Increase the order count by PVC.
    if( IsTAP == TRUE )                                                       // Wait the message from TAP Server.
    {
        if( FEvents[ PVC0Base ].WaitFor( TAIFEX_INTERNAL_TIMEOUT ) == TRUE )  // Wait for the semphore.
        {
            FEvents[ PVC0Base ].ResetEvent();
            if( FReturnMsg[ PVC0Base ] != NULL )                              // If the reply message is not NULL.
            {
                MessageHeader* Msg = FReturnMsg[ PVC0Base ];
                FReturnMsg[ PVC0Base ] = NULL;
                UpdateStates( PVC0Base, type );
                return Msg;													  // Return the reply message.
            }
        }
        if( OrderNo != NULL )
            UFC::BufferedLog::Printf( " PVC %d Message:%s OrderNo:%s Received reply from TAPServer timeout.", PVC0Base + 1, Str, OrderNumber );
        else
            UFC::BufferedLog::Printf( " PVC %d Message:%s Received reply from TAPServer timeout.", PVC0Base + 1, Str );
        FR030[ PVC0Base ].SetStatusCode( 99 );
        return &FR030[ PVC0Base ];	                                          // Timeout! return error code 99.
    }
    else																	  // Wait the message from Migo Bus.
    {
        if( FEvents[ PVC0Base ].WaitFor( TAIFEX_MIGOMSG_TIMEOUT ) == TRUE )   // Wait for the semphore.
        {
            FEvents[ PVC0Base ].ResetEvent();
            if( FReturnMsg[ PVC0Base ] != NULL )                              // If the reply message is not NULL.
            {
                MessageHeader* Msg = FReturnMsg[ PVC0Base ];
                FReturnMsg[ PVC0Base ] = NULL;
                UpdateStates( PVC0Base, type );
                return Msg;													  // Return the reply message.
            }
        }
        if( OrderNo != NULL )
            UFC::BufferedLog::Printf( " PVC %d Message:%s OrderNo:%s Received reply from MigoBus timeout.", PVC0Base + 1, Str, OrderNumber );
        else
            UFC::BufferedLog::Printf( " PVC %d Message:%s Received reply from MigoBus timeout.", PVC0Base + 1, Str );
        FR030[ PVC0Base ].SetStatusCode( 99 );
        return &FR030[ PVC0Base ];	                                          // Timeout! return error code 99.
    }
}
//----------------------------------------------------------------------------------------------------------------------
//
// Implement TransactionObject interface.
//
//----------------------------------------------------------------------------------------------------------------------
//  OrderConnectionObjects will use this function to send messages to OrderTransactionObject.
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* OrderTransactionObject::OnMessage( ConnectionObjectBase *Sender, MessageHeader* InMsg)
{
    int              PVCZeroBase = FLUT[ Sender->GetConnectionParameter()->GetLink()*100 + Sender->GetConnectionParameter()->GetPVC()];
    //int              PVCZeroBase = Sender->GetConnectionParameter()->GetPVC() - 1;
    AnsiString       BrokerID    = Sender->GetConnectionParameter()->GetBrokerID();
    int              MessageType = (int)InMsg->MessageType();

    
    switch( MessageType )
    {
        case mtR010: return OnR010Arrived( static_cast< MessageR010*>(InMsg),PVCZeroBase,BrokerID);
        case mtR040: return OnConfirmConnectionHandshake( PVCZeroBase );
        case mtR060: return OnBrokenLineQuery( PVCZeroBase );
        case mtR070: return OnR070Arrived( static_cast< MessageR070*>(InMsg),PVCZeroBase,BrokerID);// Quote request message.( Migo Message )
        case mtR090: return OnR090Arrived( static_cast< MessageR090*>(InMsg),PVCZeroBase,BrokerID);
        default:     UFC::BufferedLog::Printf( " FCM:%s PVC:%d Received unexpected message type:%d.", BrokerID.c_str(), PVCZeroBase+1, MessageType );
                                 return NULL;
    }
}
//----------------------------------------------------------------------------------------------------------------------
//  Other objects will use this function to send notification to OrderTransactionObject.
//----------------------------------------------------------------------------------------------------------------------
void OrderTransactionObject::OnEvent( int Type, void* Data )
{
	int RtnVal;
	switch( Type )
	{
		case OPEN_SWITCH_CHANGED: FOpenSwitch = (int)Data;
								  RtnVal = FOrderSender->SetOpenSwitch( FOpenSwitch );
								  if( RtnVal != 0 )
								 	  FSendMark = RtnVal;
								  break;
	}
}
//----------------------------------------------------------------------------------------------------------------------
//
// Implement PThread execute virtual function.
//
//----------------------------------------------------------------------------------------------------------------------
void OrderTransactionObject::Execute( void )
{
    InitQIF();
    
    while( TRUE  ) 
    {
        if( FOpenSwitch < OS_REJECT_ORDER )                 ///< If market was opened.
        {
            if( ConnectTAPServer() == TRUE )			    ///< Connect to TAPServer succeed.
            {
                while( FOpenSwitch < OS_REJECT_ORDER )      ///< Is market closed?
                {
                    UFC::SleepMS( TAP_MESSAHE_QUEUE_TIMEOUT );  ///< sleep TAP_MESSAHE_QUEUE_TIMEOUT ms.
                    if( SendMessageToTAPServer() == FALSE   ///< Send orders to TAPS failed.
                        || FConnected == FALSE )            ///< Connect to TAPS failed.
                        break;
                }
            }
            else                                            ///< Connect to TAPServer failed.
            {
                sleep( 2 );
                FOrderSender->CloseTapSrv();		        ///< Close the connection.
            }
        }
        else
            sleep( 2 );
    }
}
//----------------------------------------------------------------------------------------------------------------------
//
//  Functions work with James's objects.
//
//----------------------------------------------------------------------------------------------------------------------
// Init the Qif060 server.
//----------------------------------------------------------------------------------------------------------------------
void OrderTransactionObject::InitQIF( void )
{
    switch( FOrderSender->QueryQ060Srv( ))
    {
        case 0: UFC::BufferedLog::Printf( " QIF060Client, Connect to %s:%d succeed.",FOrderSender->m_pszQif060SrvIP, FOrderSender->m_nQ060SrvPort );
                break;
        case 2: UFC::BufferedLog::Printf( " QIF060Client, Open qif060 server ip cfg file fail.");
                break;
        case 3: UFC::BufferedLog::Printf( " QIF060Client, Reading Qif060 Server IP string from m_pszQ060ServerIP_File file error." );
                break;
        case 4: UFC::BufferedLog::Printf( " QIF060Client, m_pszQif060SrvIP is empty." );
                break;
        case 5: UFC::BufferedLog::Printf( " QIF060Client, Q060Server can not be opened." );
                break;
        case 6: UFC::BufferedLog::Printf( " QIF060Client, query_060server() error." );
                break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
// Connect to the TAP server.
//----------------------------------------------------------------------------------------------------------------------
BOOL OrderTransactionObject::ConnectTAPServer( void )
{
    int RtnVal = FOrderSender->ConnectTapSrv();

    memset( FReturnMsg, sizeof( MessageHeader* ) * MAX_PVC, 0 );
    if( RtnVal == 0 )
    {
        FConnected = TRUE;
        UFC::BufferedLog::Printf( "TAPClient, Connect to %s:%d succeed.",FOrderSender->m_pszTapServerIP, FOrderSender->m_nTapSrvPort );
        return TRUE;
    }
    else
    {
        FConnected = FALSE;
        switch( RtnVal )
        {
                case 1:	 UFC::BufferedLog::Printf( "TAPClient, Open tap_server ip cfg file fail." );
                         break;
                case 2:	 UFC::BufferedLog::Printf( "Reading tap_server IP from m_pszTapServerIP_File file error." );
                         break;
                case 3:	 UFC::BufferedLog::Printf( "Ropen_lan() failed." );
                         break;
        }
        return FALSE;
    }
}
//----------------------------------------------------------------------------------------------------------------------
// Add message to OrderSender object.
//----------------------------------------------------------------------------------------------------------------------
int OrderTransactionObject::AddMessage( char* AddMsg )
{
    int nRet;

    FSendingCriticalSection.Enter();
    nRet = FOrderSender->AddMessage( AddMsg );
    // nRet:  0: Add message OK
    //        1: buffer full, but adding msg OK.
    //       -1: error, pt30_data == NULL.
    //       -2: error, ConnectTapSrv() fail or not call open ConnectTapSrv() yet.
    FSendingCriticalSection.Leave();
    return nRet;
}
//----------------------------------------------------------------------------------------------------------------------
// Send messages to Tap Server.
//----------------------------------------------------------------------------------------------------------------------
BOOL OrderTransactionObject::SendMessageToTAPServer( void )
{
    int		   PVC;
    int            MessageCount;
    MessageHeader* RtnMsg;

    FSendingCriticalSection.Enter();
    if( FOrderSender->GetMessageCount() > 0 )
    {
        MessageCount = FOrderSender->Send();
        if( MessageCount > 0 )
        {
            for( register int i = 0; i < MessageCount; i++  )
            {
                RtnMsg = FOrderSender->GetMessage( i, PVC );
                if( PVC < 1 || PVC > MAX_PVC )
                {
                    UFC::BufferedLog::Printf( "Receive error PVC Number:%d, Index:%d", PVC, i  );
                }
                else
                {
                    FReturnMsg[ PVC-1 ] = RtnMsg;
                    FEvents[ PVC-1 ].SetEvent();
                }
            }
        }
        else if( MessageCount == -1 )
        {
            UFC::BufferedLog::Printf( "Send orders to TAPServer fail." );
            FSendingCriticalSection.Leave();
            FConnected = FALSE;
            return FALSE;
        }
    }
    FSendingCriticalSection.Leave();
    if( FSendMark != 0 )
    {
        switch( FSendMark )
        {
            case 1: UFC::BufferedLog::Printf(" From XBIO4" ); break;
            case 2: UFC::BufferedLog::Printf(" From XBIO4" ); break;
            case 3: UFC::BufferedLog::Printf(" From XBIO4" ); break;
        }
        FSendMark = 0;
        if( FOrderSender->SendDummyMsf() == 2 )
        {
                FConnected = FALSE;
                return FALSE;
        }
    }
    return TRUE;
}
//----------------------------------------------------------------------------------------------------------------------
//
//    Functions about states.
//
//----------------------------------------------------------------------------------------------------------------------
void OrderTransactionObject::GetStates( XBIO4States* States )
{
	States->MaxOrderResponseTimePVC      = FMaxResponseTimePVC;
	States->MaxOrderResponseTime         = FMaxResponseTime;
	if( FOrderIntervalCount > 0 )
        States->OrderAverageResponseTime = FOrderIntervalTime / FOrderIntervalCount;
    else
    	States->OrderAverageResponseTime = 0;
	if( FQueryIntervalTime > 0 )
        States->QueryAverageResponseTime = FQueryIntervalCount / FQueryIntervalTime;
    else
    	States->QueryAverageResponseTime = 0;
	States->R060Times = 0;
	for( register int i = 0; i < MAX_PVC; i++ )
	{
		if( FR060Count[ i ] > States->R060Times )
		{
			States->MaxR060TimesPVC = i + 1;
			States->R060Times       = FR060Count[ i ];
		}
	}
	States->CurrentOrderCount   = FOrderCount1 + FOrderCount2 + FOrderCount3 + FQueryCount1 + FQueryCount2 + FQueryCount3;
	FOrderIntervalCount = 0;
 	FQueryIntervalCount = 0;
 	FOrderIntervalTime  = 0;
 	FQueryIntervalTime  = 0;
}
//----------------------------------------------------------------------------------------------------------------------
void OrderTransactionObject::Report( LogManager* Log )
{
	AnsiString LogStr("--------------------------------- Report ---------------------------------");
	long OrderCount = FOrderCount1 + FOrderCount2 + FOrderCount3 + FQueryCount1 + FQueryCount2 + FQueryCount3;
	long QuoteCount = FOrderCount5 + FQueryCount5;

	Log->WriteString( LogStr );
	LogStr.Printf("Total R010 insert order: %d",FOrderCount1);
	Log->WriteString( LogStr );
	LogStr.Printf("Total R010 reduce order: %d",FOrderCount2);
	Log->WriteString( LogStr );
	LogStr.Printf("Total R010 cancel order: %d",FOrderCount3);
	Log->WriteString( LogStr );
	LogStr.Printf("Total R010 query order: %d",FOrderCount5);
	Log->WriteString( LogStr );
	LogStr.Printf("Total R090 insert order: %d",FQueryCount1);
	Log->WriteString( LogStr );
	LogStr.Printf("Total R090 reduce order: %d",FQueryCount2);
	Log->WriteString( LogStr );
	LogStr.Printf("Total R090 cancel order count: %d",FQueryCount3);
	Log->WriteString( LogStr );
	LogStr.Printf("Total R090 query order count: %d",FQueryCount5);
	Log->WriteString( LogStr );
	if( OrderCount != 0 )
	{
		LogStr.Printf("Order average response time: %5.3f",(float)(FOrderTotalTime/OrderCount )/1000.0);
		Log->WriteString( LogStr );
	}
	if( QuoteCount != 0 )
	{
		LogStr.Printf("Quote average response time: %5.3f",(float)(FQueryTotalTime/QuoteCount)/1000.0);
		Log->WriteString( LogStr );
	}
	LogStr.Printf("------------------------------ End ---------------------------------" );
	Log->WriteString( LogStr );
}
//----------------------------------------------------------------------------------------------------------------------
