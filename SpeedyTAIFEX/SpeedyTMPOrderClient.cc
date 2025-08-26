
#include "OrderInfoStore.h"
#include "SpeedyOrderClient.h"
#include "SpeedyObjectsManager.h"
#include "../TAIFEXLib/LinkSubSystem.h"
#include "../TAIFEXLib/Connection.h"
#include "../TAIFEXLib/ConnectionObject.h"
#include "../TAIFEXLib/TMPMessage.h"
#include "../TAIFEXLib/MultilegSymbol.h"
#include "../Migo/Sigo.h"
#include "../UFC/Profiler.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern MessageObject*         MessageObj;
extern BOOL                   RecoverMode;
extern BOOL                   UseIndexsymbol;
extern double                 ThroughputRatio;
//----------------------------------------------------------------------------------------------------------------------
ThroughputCum::ThroughputCum( Int32 Sec )
:FPos( 0 )
,FCount( Sec )
{
    FThroughputs = new Int32[ Sec ];
    memset( FThroughputs, 0, sizeof(Int32)*FCount );
}
//----------------------------------------------------------------------------------------------------------------------
ThroughputCum::~ThroughputCum( void )
{
    delete [] FThroughputs;
}
//----------------------------------------------------------------------------------------------------------------------
void  ThroughputCum::PutThroughput( Int32 Orders )
{
    *( FThroughputs + FPos ) = Orders;
    FPos++;
    if( FPos == FCount )
        FPos = 0;
}
//----------------------------------------------------------------------------------------------------------------------
Int32 ThroughputCum::GetCumThroughput( void )
{
    Int32 Total = 0;
    for( register Int32 i = 0; i < FCount; i++ )
         Total += *( FThroughputs + i );
    return Total;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constractor
// To create a TMP order client.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SpeedyTMPOrderClient::SpeedyTMPOrderClient( SpeedyObjectsManager*  ObjManager,
                                            ConnectionParameter &Param,
                                            ConnectionListener* Listener,
                                            ConnectionObjectBase* Owner,
                                            BOOL BinaryMsg ) // TMP OK.
:UFC::PThread( NULL, TRUE ),
 FObjectManager( ObjManager ),
 FBrokerID(Param.GetBrokerID()),
 FLinePassword(Param.GetPassword()),
 FURL( Param.GetURL()),
 FLine( Param.GetLink() ),
 FLineBrokerIndex( Param.GetBrokerIndex()),
 FSessionID( Param.GetPVC() ),
 FSendHeartbeat( 0 ),
 FRecvHeartbeat( 0 ),
 FIsConnected( FALSE ),
 FIsBinary( BinaryMsg ),
 FConnectionListener( Listener ),
 FOwner( Owner ),
 FOutCount( 0 ),
 FFirstOrderTick( 0 ),
 FOrderPerSec( 5 ),
 FSlowDownFlag( 0 ),
 FOrderSleepNS( 1000000 ), ///< 1 ms
 FPrevSeq( 0 ),
 FThroughputCum( 3 )
{
    FTMPOrderLine = new LinkSubSystemFCM( FObjectManager->IsOptions(), FLine, FSessionID, FLinePassword, FBrokerID,'4', (char*)FURL.c_str() );
    FDataStr      = new char[ 512 ];
    FTMPOrderLine->SetLogManager( FObjectManager->GetOrderLog() );
    FTMPOrderLine->SetListener( FOwner );
    if( RecoverMode == FALSE )
        FTMPOrderLine->SessionEOD();
    FSID.Printf("%03d", Param.GetPVC() );
    FPBNO.Printf( "%s%03d", FBrokerID.SubString(0,4).c_str(), Param.GetPVC() );
    if( FObjectManager->IsOptions() == TRUE )
    {
        FPerforanceGroup = 2;
        FSyncSubject     = "SESSION.SEQ.OPT";
    }
    else
    {
        FPerforanceGroup = 1;
        FSyncSubject     = "SESSION.SEQ.FUT";
    }
    FSyncKey.Printf( "%s%03d", FBrokerID.c_str(), Param.GetPVC() );
    if( MessageObj != NULL )
        MessageObj->AddListener( FSyncSubject, FSyncKey, this );
    Start( );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SpeedyTMPOrderClient::~SpeedyTMPOrderClient( void )// TMP OK.
{
    Close();
    delete [] FDataStr;
    delete FTMPOrderLine;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data )
{
    Int32 SyncSeq;
    if( Data->get( "SEQ", SyncSeq ) == TRUE )
    {
        if( IsReady( ) == FALSE  ) // TMP not connected.(Backup line)
            FTMPOrderLine->SessionEOD( SyncSeq );
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::Open( void )// TMP OK.
{
    FTMPOrderLine->Open();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::Close( void )// TMP OK.
{
    if( FIsConnected == TRUE )
    {
        FIsConnected = FALSE;
        FSendHeartbeat = 0;
        FRecvHeartbeat = 0;        
        UFC::BufferedLog::Printf( " Close this session" );
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_BROKEN );
    }
    FTMPOrderLine->Close();    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// OrderListener interface function
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Call this function when open market.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::OnOpenMarket( void ) // TMP OK.
{
    FOrderCount = 0;
    if( FIsConnected == TRUE )
    {
        FIsConnected = FALSE;
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_BROKEN );
    }
    if( FTMPOrderLine->OnlineHandshake( ) == TRUE )
    {
        FIsConnected   = TRUE;
        FSendHeartbeat = 0;
        FRecvHeartbeat = 0;
        FOrderPerSec   = (Int32)(FTMPOrderLine->GetThroughput() * ThroughputRatio );
        FOrderSleepNS  = 1000000000 / FOrderPerSec;
        UFC::BufferedLog::Printf( " ------------------ Connected ---------------------------" );
        UFC::BufferedLog::Printf( " BrokerID[%s] SessionID[%d]",FBrokerID.c_str(),FSessionID );
        UFC::BufferedLog::Printf( " TAIFEX Throughput[%d]", FTMPOrderLine->GetThroughput() );
        UFC::BufferedLog::Printf( " Limit  Throughput[%d]", FOrderPerSec );
        UFC::BufferedLog::Printf( " Throughput control ratio[%3.2f]", ThroughputRatio );
        switch( FTMPOrderLine->GetSessionBandwidth() )
        {
            case lb128K: UFC::BufferedLog::Printf( " Connection established on a 128K line."); break;
            case lb256K:
            case lb512K: UFC::BufferedLog::Printf( " Connection established on a 512k line."); break;
            case lbE1:   UFC::BufferedLog::Printf( " Connection established on a 2048k line."); break;

        }
        UFC::BufferedLog::Printf( " --------------------------------------------------------" );
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_READY, NULL );
        SendR11( );
        QueryUnknownOrders();
    }
    else
        throw X25Exception(-9,"Handshake exception");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::Handshake( BOOL ReceiveL010 ) // TMP OK.
{
    if( FIsConnected == TRUE )
    {
        FIsConnected = FALSE;
        UFC::BufferedLog::Printf( " Handshake" );
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_BROKEN );
    }
    if( FTMPOrderLine->OnlineHandshake( ReceiveL010 ) == TRUE )
    {
        FIsConnected = TRUE;
        FSendHeartbeat = 0;
        FRecvHeartbeat = 0;
        FOrderPerSec   = FTMPOrderLine->GetThroughput();
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_READY, NULL );
        SendR11( );
        QueryUnknownOrders();
    }
    else
        throw X25Exception(-9,"Handshake exception");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyTMPOrderClient::IsReady( void ) // TMP OK.
{
    return FTMPOrderLine->IsReady();    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UInt8 SpeedyTMPOrderClient::GetHeartBeatInterval( void ) 
{ 
    return FTMPOrderLine->GetHeartBeatInterval(); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Call this function to send order to TAIFEX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyTMPOrderClient::OnOrder( MessageHeader* OrderMessage, Int64 NID ) // TMP OK.
{
    TMessageType Type = OrderMessage->MessageType( );

    ///< CONVERT from X.25 R010 to TMP R01 and then send.
    switch( Type )
    {
        case mtR010: FOrderCount++;
                     SendR01( static_cast<MessageR010*>(OrderMessage), NID ); // R010 New Order message.
                     ControlTroughput();
                     return TRUE;
        case mtR070: FOrderCount++;
                     SendR07( static_cast<MessageR070*>(OrderMessage), NID ); // R070 Quote Request message.
                     ControlTroughput();
                     return TRUE;
        case mtR090: FOrderCount++;
                     SendR09( static_cast<MessageR090*>(OrderMessage), NID ); // R090 Quote message.
                     ControlTroughput();
                     return TRUE;
        default:     UFC::BufferedLog::Printf( " Unsupport Message Type %d", Type );
                     return FALSE;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyTMPOrderClient::OnOrder( TMP::TMPMessage* Order, Int64 NID )
{
    TMP::TMPMessageType Type = Order->GetMessageType( );

    switch( Type )
    {
        case TMP::tmtR01: // New Order message.
        case TMP::tmtR07: // Quote Request message.
        case TMP::tmtR09: // Quote message.
                          FOrderCount++;
                          FSendHeartbeat = 0;
                          FTMPOrderLine->SendTMPMessage( Order, TMP_TIMEOUT, TRUE );
                          ControlTroughput();
                          return TRUE;
        case TMP::tmtR11:
        case TMP::tmtR13: FSendHeartbeat = 0;
                          return  FTMPOrderLine->SendTMPMessage( Order, TMP_TIMEOUT, TRUE );
        default:          UFC::BufferedLog::Printf( " Unsupport Message Type %d", Type );
                          return FALSE;
    }
}
//------------------------------------------------------------------------------
void  SpeedyTMPOrderClient::ControlTroughput( void )
{
    if( FSlowDownFlag > 0 )
    {
        UFC::SleepMS( FSlowDownFlag%1000 );
        UFC::BufferedLog::DebugPrintf( " *** FCM[%s] SID[%d] Flow control, sleep[%d]ms ***", FBrokerID.c_str(),FSessionID, FSlowDownFlag );
        FSlowDownFlag = 0;
    }

    Int32 Now = UFC::GetTickCountMS();

    FOutCount++;
    if( FFirstOrderTick == 0  ) ///< First order
    {
        FFirstOrderTick = Now;
        return;
    }
    Int32 TimeDiff = Now - FFirstOrderTick;

    if( TimeDiff >= 1000 ) ///< more then 1 sec
    {
        FThroughputCum.PutThroughput( FOutCount );
        FFirstOrderTick = Now;///< Reset timer.
        FOutCount       = 0;  ///< Reset counter.
    }
    else
    {
        if( FOutCount >= FOrderPerSec )
        {
            UFC::BufferedLog::DebugPrintf( " *** FCM[%s] SID[%d] Reach throughput [%d] sleep [%d]ms ***", FBrokerID.c_str(),FSessionID, FOutCount, 1000 - TimeDiff );            
            UFC::SleepMS( (1000 - TimeDiff)%1000 );
        }
        else if( FThroughputCum.GetCumThroughput() >= FOrderPerSec*3 )
        {
            UFC::BufferedLog::DebugPrintf( " *** FCM[%s] SID[%d] Cum throughput reach [%d] sleep [%d]ms ***", FBrokerID.c_str(),FSessionID, FOrderPerSec*3,1000 - TimeDiff );
            UFC::SleepMS( (1000 - TimeDiff)%1000 );
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::AddToSentList( TMP::TMPMessage& Order )
{
    char ExecType;
    UFC::AnsiString OID,OrderKey;

    Order[ "ExecType" ].ToBinary( (UInt8*)&ExecType );
    OID = Order[ "order_no" ].ToString();
    if(  ExecType == '0'||  ExecType == '4' || ExecType == '5' ) ///< New order,Cancel or replace order
    {
        UFC::PLockObject Lock( FMsgCS ); ///< Enter Critical Section

        OrderKey.Printf( "%s_%c", OID.c_str(), ExecType );
        FMessages.Add( OrderKey, new TMP::TMPMessage( Order ));
    }    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::RemoveFromSentList( TMP::TMPMessage& Exec ) ///< R02 or R03
{
    char ExecType;
    UFC::AnsiString OID,OrderKey;

    Exec[ "ExecType" ].ToBinary( (UInt8*)&ExecType );
    OID = Exec[ "order_no" ].ToString();
    if(  ExecType == '0'|| ExecType == '4' || ExecType == '5' || ExecType == '6' ) ///< New order or Cancel order or New+Fill
    {
        UFC::PLockObject Lock( FMsgCS ); ///< Enter Critical Section
        TMP::TMPMessage* DelMsg;

        if( ExecType == '6' )
            ExecType = '0';
        OrderKey.Printf("%s_%c",OID.c_str(), ExecType );
        if( (DelMsg = FMessages.GetObjectByKey( OrderKey ))!= NULL )
        {
            FMessages.DeleteByKey( OrderKey );
            delete DelMsg;
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::RemoveFromSentList( TMP::TMPMessage& Exec, UFC::AnsiString& Symbol ) ///< R02 or R03
{
    char ExecType;
    UFC::AnsiString OID,OrderKey;

    Exec[ "ExecType" ].ToBinary( (UInt8*)&ExecType );
    OID = Exec[ "order_no" ].ToString();
    if(  ExecType == '0'|| ExecType == '4' || ExecType == '5' || ExecType == '6' ) ///< New order or Cancel order or New+Fill
    {
        UFC::PLockObject Lock( FMsgCS ); ///< Enter Critical Section
        TMP::TMPMessage* DelMsg;

        if( ExecType == '6' )
            ExecType = '0';
        OrderKey.Printf("%s_%c",OID.c_str(), ExecType );
        if( (DelMsg = FMessages.GetObjectByKey( OrderKey ))!= NULL )
        {
            if( DelMsg->GetMessageType() == TMP::tmtR01 || DelMsg->GetMessageType() == TMP::tmtR09 )
                Symbol = DelMsg->GetField( "Symbol" )->ToString();
            FMessages.DeleteByKey( OrderKey );
            delete DelMsg;
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::QueryUnknownOrders( void ) ///< R02 or R03
{
    UFC::PLockObject Lock( FMsgCS ); ///< Enter Critical Section
    TMP::TMPMessage* QueryOrder = FMessages.First();

    UFC::BufferedLog::Printf( " ---------------- Query Order Ststus --------------------" );
    while( QueryOrder != NULL )
    {
        QueryOrder->SetField( "ExecType", (Int32)'I' ); ///< Query order status
        FTMPOrderLine->SendTMPMessage( QueryOrder, TMP_TIMEOUT, FALSE );
        UFC::SleepNS( FOrderSleepNS );
        delete QueryOrder;
        QueryOrder = FMessages.Next();
    }    
    UFC::BufferedLog::Printf( " Query[%d] unknown status orders.",FMessages.ItemCount() );
    UFC::BufferedLog::Printf( " --------------------------------------------------------" );
    FMessages.Clear();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyTMPOrderClient::SendR11( void )
{
    TMP::TMPMessage R11( TMP::tmtR11 );
    
    R11["status_code"] = 0;
    R11["TradeReqID"] = 10;
    R11["flow_group_no"] = 1; 
    R11["SubscriptionRequestType"] = 1;///< Subscribe
    return FTMPOrderLine->SendTMPMessage( &R11, TMP_TIMEOUT, TRUE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyTMPOrderClient::SendR01( MessageR010* R010, Int64 NID )
{
    TMP::TMPMessage R01( TMP::tmtR01 );
    TMP::TMPSymbol  Symbol( UFC::AnsiString( R010->CommodityId, 20 ) );
    UFC::AnsiString OID( R010->OrderNo,5 );
    UFC::AnsiString Px( R010->OrderPrice,9);
    UFC::AnsiString Qty( R010->OrderQty,4);
    TMP::TMPInt64* UsderDefInt64 = static_cast<TMP::TMPInt64*>(R01.GetField("user_define"));

    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " OrderID[%s] Px[%s] Qty[%s]", OID.c_str(), Px.c_str(), Qty.c_str());
    switch( R010->GetFunctionCode())
    {
        case 1:R01["ExecType"] = "0";break;
        case 2:R01["ExecType"] = "5";break;
        case 3:R01["ExecType"] = "4";break;
        case 5:R01["ExecType"] = "I";break;
        default: UFC::BufferedLog::Printf( " Unsupport Function code %d", R010->GetFunctionCode() );
                 return FALSE;
    }       
    R01["cm_id"]         = TMP::TMPMessage::GetCMID( UFC::AnsiString( R010->ClearMemberId,4) );
    R01["fcm_id"]        = TMP::TMPMessage::GetBrokerID( UFC::AnsiString( R010->FcmNo,7) );
    R01["order_no"]      = OID;    
    *UsderDefInt64      = NID;
    R01["Price"]         = Px.ToInt();
    R01["qty"]           = Qty.ToInt();
    R01["investor_acno"] = UFC::AnsiString( R010->InvestorAcno, 7 ).ToInt();    
    R01["investor_flag"] = R010->InvestorFlag;    
    ///< Set Side
    if( R010->BuySellCode == 'B' )
    {
        R01["Side"] = "1";
        if( UseIndexsymbol == TRUE )
            Symbol.ToBinaryMode( '1' );
        R01["ord_id"] = 10000000 + UFC::AnsiString( R010->InvestorAcno, 7 ).ToInt();
    }
    else
    {
        R01["Side"] = "2";
        if( UseIndexsymbol == TRUE )
            Symbol.ToBinaryMode( '2' );
        R01["ord_id"] = 20000000 + UFC::AnsiString( R010->InvestorAcno, 7 ).ToInt();
    }
    R01["Symbol"]        = Symbol;
    ///< Set OrderType
    if( R010->OrderType == 'L' )        
        R01["OrdType"] = 2;            
    else
        R01["OrdType"] = 1;            
    ///< Set TimeInForce
    if( R010->OrderCondition == 'F' )        
        R01["TimeInForce"] = 4;                
    else if( R010->OrderCondition == 'I' )
        R01["TimeInForce"] = 3;                
    else
        R01["TimeInForce"] = 0;              
    ///< Set PositionEffect
    if( R010->OpenOffsetFlag == '0' )
        R01["PositionEffect"] = "O";
    else if( R010->OpenOffsetFlag == '1' )
        R01["PositionEffect"] = "C";      
    else if( R010->OpenOffsetFlag == '2' || R010->OpenOffsetFlag == '3' )
        R01["PositionEffect"] = "D";          
    else if( R010->OpenOffsetFlag == '9' )
        R01["PositionEffect"] = "9";         
    FSendHeartbeat = 0;
    AddToSentList( R01 );
    return FTMPOrderLine->SendTMPMessage( &R01, TMP_TIMEOUT, TRUE, TRUE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyTMPOrderClient::SendR07( MessageR070* R070, Int64 NID )
{
    TMP::TMPMessage R07( TMP::tmtR07 );
        
    R07["fcm_id"] = TMP::TMPMessage::GetBrokerID( UFC::AnsiString( R070->FcmNo,7) );
    R07["Symbol"] = TMP::TMPSymbol( UFC::AnsiString( R070->CommodityId, 20 ) );        
    FSendHeartbeat = 0;        
    return FTMPOrderLine->SendTMPMessage( &R07, TMP_TIMEOUT, TRUE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyTMPOrderClient::SendR09( MessageR090* R090, Int64 NID )
{
    TMP::TMPMessage R09( TMP::tmtR09 );
    TMP::TMPSymbol  Symbol( UFC::AnsiString( R090->ProdId, 20 ) );

    switch( R090->GetFunctionCode() )
    {
        case 1:R09["ExecType"] = "0";break;
        case 2:R09["ExecType"] = "5";break;
        case 3:R09["ExecType"] = "4";break;
        case 5:R09["ExecType"] = "I";break;
        default: UFC::BufferedLog::Printf( " Unsupport Function code %d", R090->GetFunctionCode() );
                 return FALSE;
    }   
    R09["cm_id"]         = TMP::TMPMessage::GetCMID( UFC::AnsiString( R090->Member,4) );
    R09["fcm_id"]        = TMP::TMPMessage::GetBrokerID( UFC::AnsiString( R090->FcmNo,7) );
    R09["order_no"]      = UFC::AnsiString( R090->OrderNo,5 );
    R09["ord_id"]        = UFC::AnsiString( R090->IvacNo, 7 ).ToInt();
    R09["user_define"]   = NID;
    if( UseIndexsymbol == TRUE )
        Symbol.ToBinaryMode( );
    R09["Symbol"]        = Symbol;
    R09["BidPx"]         = UFC::AnsiString( R090->BuyPrice, 9 ).ToInt();
    R09["OfferPx"]       = UFC::AnsiString( R090->SellPrice, 9 ).ToInt();
    R09["BidSize"]       = UFC::AnsiString( R090->BuyQty, 4 ).ToInt();    
    R09["OfferSize"]     = UFC::AnsiString( R090->SellQty, 4 ).ToInt();    
    R09["investor_acno"] = UFC::AnsiString( R090->IvacNo, 7 ).ToInt();    
    R09["investor_flag"] = R090->IvacNoFlag;    
    if( R090->OrderCond == 'Q' )
        R09["TimeInForce"] = 8;                
    else
        R09["TimeInForce"] = 0;                  
    R09["PositionEffect"] = "9";///< Market maker.          
    FSendHeartbeat = 0;
    AddToSentList( R09 );
    FObjectManager->AddQuote( R09 );
    return FTMPOrderLine->SendTMPMessage( &R09, TMP_TIMEOUT, TRUE, TRUE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 SpeedyTMPOrderClient::GetStatus( void )// TMP OK.
{
    return FTMPOrderLine->GetStatus();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::SetStatus( Int32 Status)// TMP OK.
{
    FTMPOrderLine->SetStatus( Status );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::StopHandshake( void )// TMP OK.
{
    FTMPOrderLine->StopHandshake();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::Listen( BOOL IsListen )// TMP OK.
{
    if( FTMPOrderLine != NULL )
        FTMPOrderLine->Listen( IsListen );
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::SendSessionSequence( void )
{
    Int32 SeqNow = FTMPOrderLine->GetInboundSequence();

    if( SeqNow - FPrevSeq > 10 )
    {
        MTree Data;

        FPrevSeq = SeqNow;
        Data.append( "SEQ", SeqNow );
        MessageObj->Send( FSyncSubject, FSyncKey, Data, FALSE  );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::Execute( void )
{
    TMP::TMPMessage* InMsg;
    while( TRUE )
    {
       if( FIsConnected == TRUE )
       {
           try
           {
               if( (InMsg = FTMPOrderLine->ReceiveTMPMessage( (long)1, FALSE )) == NULL)  ///< Try to receive data from TAIFEX
               {
                   FSendHeartbeat++;
                   FRecvHeartbeat++;
               }
               else
               {
                   FRecvHeartbeat = 0;
                   TMP::TMPMessageType MsgType = InMsg->GetMessageType( );                   
                   
                   if( MsgType == TMP::tmtL10 )
                   {                       
                       FIsConnected = FALSE;
                       UFC::BufferedLog::Printf( " Recv L10, Try to reconnect." );
                       FOwner->OnCommand(EVENT_TMP_RECONNECT,NULL);
                   }
                   else if( MsgType == TMP::tmtR04 )
                   {
                       TMP::TMPMessage  R05(TMP::tmtR05 ); // R050 Confirm Connection message                       
                       FTMPOrderLine->SendTMPMessage( &R05, TMP_TIMEOUT, TRUE  );// Send the confirm connection message    
                       FSendHeartbeat = 0;
                   }
                   else if( MsgType == TMP::tmtR05 )
                   {
                       UFC::BufferedLog::Printf( " Recv Heartbeat reply." );
                   }
                   else
                       OnTMPMessage( InMsg );                    
                   delete InMsg;
                   SendSessionSequence();
               }
               if( FSendHeartbeat == FTMPOrderLine->GetHeartBeatInterval() ||
                   FRecvHeartbeat == FTMPOrderLine->GetHeartBeatInterval() ) ///< Time is up!
               {
                    TMP::TMPMessage  R04(TMP::tmtR04 ); // R040 Confirm Connection message                    
                    FTMPOrderLine->SendTMPMessage( &R04, TMP_TIMEOUT, TRUE  );// Send the confirm connection message    
                    FSendHeartbeat = 0;
               }                   
               if(  FSendHeartbeat > FTMPOrderLine->GetHeartBeatInterval() + 5 ||
                    FRecvHeartbeat > FTMPOrderLine->GetHeartBeatInterval() + 5  ) ///< Broker Timeout!!
               {
                   FIsConnected = FALSE;
                   UFC::BufferedLog::Printf( " Heartbeat Timeout" );
                   FOwner->OnCommand(EVENT_TMP_RECONNECT,NULL);
               }               
           }
           catch( UFC::Exception &e )
           {
               FIsConnected = FALSE;
               UFC::BufferedLog::Printf( " Socket Exception:%s",e.what());
               FOwner->OnCommand(EVENT_TMP_RECONNECT,NULL);	       
           }
           catch( ... )
           {
               FIsConnected = FALSE;
               UFC::BufferedLog::Printf( " Unknown exception" );
               FOwner->OnCommand(EVENT_TMP_RECONNECT,NULL);	       
           }
       }
       else
       	   sleep( 1 );
    }    
}    
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::OnTMPMessage( TMP::TMPMessage* Msg )
{
    if( FIsBinary == TRUE )
    {
        ///< Publish Binary format MBus message.
        return;
    }    
    switch (Msg->GetMessageType())
    {
        case TMP::tmtR02 : ReplyR02(Msg);
                           break;
        case TMP::tmtR03 : ReplyR03(Msg);
                           break;
        case TMP::tmtR12 : break; ///< Trading status
        case TMP::tmtR14 : FTPReply(Msg);
                           break; ///< News or FTP file ready notify.
        default:           UFC::BufferedLog::Printf(" Unhandle TMP message Type:%d.", Msg->GetMessageType());
                           break;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::FTPReply( TMP::TMPMessage* R14 )
{
        MTree Data;
        Int32 ReqID     = R14->GetField( "fcm_req_id" )->ToInteger();
        Int32 ErrorCode = R14->GetField( "status_code" )->ToInteger();
        Int32 Systype   = R14->GetField( "system_type" )->ToInteger();
        UFC::AnsiString Body = R14->GetField( "data" )->ToString();

        Data.append( "Src", "TMP" );
        Data.append( "FileCode", Body.SubString( 0, 3 ) );
        Data.append( "ErrorCode", ErrorCode );
        Data.append( "ReqID", ReqID );
        MessageObj->Send( FObjectManager->GetFTPSubject(), UFC::Hostname, Data, FALSE );
        if( ErrorCode == 0  )///< File Ready, Ask Speedy FTP to download file.
        {
            MTree FTReq;
            
            if( FObjectManager->IsOptions() == TRUE  )
                FTReq.append( "Market", "OPT" );
            else
                FTReq.append( "Market", "FUT" );
            UFC::AnsiString ReqIDStr(ReqID);
            FTReq.append( "ReqID",ReqIDStr );
            FTReq.append( "Session", Systype%10 );
            ///< Format: FileCode.SystemType(10,20)
            ///< P06,P08,P02,P05,P03,I12,I13,T90,T91,T92,T93,T94,T95,P11,I15,I17
            if( Body.Length() == 3 ) ///< File code only
            {
                FTReq.append( "FileCode", Body );
             ///< Format: FileCode.SystemType(10,20).yyyymmddhhmmssxxx
            }///< C01,C10,C11,C20,S08,I16,SIN,SP1,SP2
            else if( Body.Length() == 20 ) ///< File code [3] + TimeStamp [17 ], in public folder.
            {
                UFC::AnsiString FileCode( Body.c_str(), 3 );
                UFC::AnsiString TimeStamp( Body.c_str()+3, 17 );

                FTReq.append( "FileCode", FileCode );
                FTReq.append( "FileExt", TimeStamp );
            } ///< B30 only
            else if( Body.Length() == 50 ) ///< B30, in broker folder
            {
                UFC::AnsiString FileExt;
                UFC::AnsiString SID( Body.c_str() + 8, 5 );
                UFC::AnsiString FCMID( Body.c_str()+ 3, 5 );
                UFC::AnsiString TimeStamp( Body.c_str() + 33, 17 );

                FileExt.Printf( "%s.%s.%s",SID.c_str(),FCMID.c_str(),TimeStamp.c_str());
                FTReq.append( "FileCode", "B30" );
                FTReq.append( "FileExt", FileExt );
            } ///< B50 only
            else if( Body.Length() == 46 ) ///< B40, Where??
            {
                UFC::AnsiString FileExt;
                UFC::AnsiString TargetID( Body.c_str() + 8, 1 );
                UFC::AnsiString CMID( Body.c_str()+ 3, 5 );
                UFC::AnsiString TimeStamp( Body.c_str() + 29, 17 );

                FileExt.Printf( "%s.%s.%s",CMID.c_str(),TargetID.c_str(),TimeStamp.c_str());
                FTReq.append( "FileCode", "B30" );
                FTReq.append( "FileExt", FileExt );
            }
            ///< Not support
            ///< P07.10.FXXX, P12.10.FXXX, S21.20.FXXXXXX, F03.10.12345, F05.10.12345.4
            ///<                                                 .fcm_id       .fcm_id.targ
            ///< Unknown F01, F04
            MessageObj->Send( "TCP.FT.REQUEST", UFC::Hostname, FTReq, FALSE );
        }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::ReplyR02( TMP::TMPMessage* R02 )
{
    UInt32 ReportSeq = R02->GetField( "rpt_seq" )->ToInteger();

    if( ReportSeq != 0 && IsReady( ) == TRUE  ) // Session connected.(B
        FTMPOrderLine->SessionEOD( ReportSeq );
    switch( R02->GetField( "ExecType" )->ToInteger() )
    {
        case '0': ReplyConfirm( *R02, 1 , TRUE ); ///< With Sequence
                  RemoveFromSentList( *R02 );
                  break;//< New Order
        case '4': if( GetPositionEffect( *R02 ) == '9' )//< quote
                      QuoteCancelReplaceExecution( *R02, TRUE );
                  else
                      CancelReplaceExecution( *R02, TRUE );
                  RemoveFromSentList( *R02 );
                  break;//< Canceled
        case '5': if( GetPositionEffect( *R02 ) == '9' ) //< quote
                      QuoteCancelReplaceExecution( *R02, FALSE );
                  else
                      CancelReplaceExecution( *R02, FALSE );
                  RemoveFromSentList( *R02 );
                  break;//< Replaced
        case '6': ReplyConfirm( *R02, 1, FALSE );
                  UFC::SleepMS( 0 );
                  FillExecution( *R02 );
                  if( IsIOCPartialFill( *R02 ) == TRUE )
                      FillExecution( *R02 , FALSE );
                  RemoveFromSentList( *R02 );
                  break;///< New order and filled.
        case 'F': FillExecution( *R02 );  //< Fill
                  if( IsIOCPartialFill( *R02 ) == TRUE )
                      FillExecution( *R02 , FALSE );
                  break;
        case 'I': ReplyConfirm( *R02, 5, TRUE ); break;//< Order Status
        default:  UFC::BufferedLog::Printf( " Unhandle R02 function code:%d.", R02->GetField( "ExecType" )->ToInteger() );
                  break;
    }    
}    
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::ReplyR03( TMP::TMPMessage* R03 )
{
    UInt32 ReportSeq = R03->GetField( "rpt_seq" )->ToInteger();
    UFC::AnsiString Symbol("");

    if( ReportSeq != 0 && IsReady( ) == TRUE  ) // Session connected.(B
        FTMPOrderLine->SessionEOD( ReportSeq );
    switch( R03->GetField( "ExecType" )->ToInteger() )
    {
        case '0': RemoveFromSentList( *R03 , Symbol );
                  ReplyReject( *R03, 1, Symbol );
                  break;//< Reject New Order
        case '5': RemoveFromSentList( *R03 , Symbol );
                  ReplyReject( *R03, 2, Symbol );
                  break;//< Reject Replaced
        case '4': RemoveFromSentList( *R03 , Symbol );
                  ReplyReject( *R03, 3, Symbol );
                  break;//< Reject Canceled
        case 'I': ReplyReject( *R03, 5, Symbol );
                  break;//< Reject Order Status
        default:  UFC::BufferedLog::Printf( " Unhandle R03 function code:%d.", R03->GetField( "ExecType" )->ToInteger() );
                  break;
    }
}    
//---------------------------------------------------------------------------------------------------------------------
char SpeedyTMPOrderClient::GetBSCode( TMP::TMPMessage& R02 )
{
    if( R02["Side"].ToString()[0] == '1' )       
        return 'B';
    else
        return 'S';        
}
//---------------------------------------------------------------------------------------------------------------------
char SpeedyTMPOrderClient::GetOrderType( TMP::TMPMessage& R02 )
{    
    if( R02["OrdType"].ToInteger() == 2 )
        return 'L';       
    else
        return 'M';                  
}
//---------------------------------------------------------------------------------------------------------------------
char SpeedyTMPOrderClient::GetPositionEffect( TMP::TMPMessage& R02 )
{
    switch( R02["PositionEffect"].ToInteger() )
    {
        case 'C': return '1'; ///< Close
        case 'D': return '2'; ///< Day trade
        case '9': return '9'; ///< Market maker
        default:  return '0'; ///< Open
    }        
}
//---------------------------------------------------------------------------------------------------------------------
UInt8 SpeedyTMPOrderClient::HandleSlowdownWarning(  UInt8 Status )
{
    switch( Status )
    {
        case 246: UFC::BufferedLog::DebugPrintf( " *** FCM[%s] SID[%d] Execution delay more than 500ms ***", FBrokerID.c_str(),FSessionID);
                  FSlowDownFlag = 100;
                  return 0;
        case 247: UFC::BufferedLog::DebugPrintf( " *** FCM[%s] SID[%d] Execution delay more than 1000ms ***", FBrokerID.c_str(),FSessionID);
                  FSlowDownFlag = 50;
                  return 0;
        case 248: if( FTMPOrderLine->IsReady() )
                      UFC::BufferedLog::DebugPrintf( " *** FCM[%s] SID[%d] Throughput exceed 80 Percent ***", FBrokerID.c_str(),FSessionID);
                  FSlowDownFlag = 200;
                  return 0;
        case 249: if( FTMPOrderLine->IsReady() )
                      UFC::BufferedLog::DebugPrintf( " *** FCM[%s] SID[%d] Throughput exceed 90 Percent ***", FBrokerID.c_str(),FSessionID);                  
                  FSlowDownFlag = 100;
                  return 0;
        default:  if( Status > 99 )
                      return 98; ///< Other TMP error.
                  return Status;
    }
}
//---------------------------------------------------------------------------------------------------------------------
// Translate TMP R02 to X.25 confarmation execution.
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::ReplyConfirm( TMP::TMPMessage& R02, Int32 Func, BOOL WithSeq )
{
    if( GetPositionEffect( R02 ) == '9' )
        ReplyQuoteConfirm( R02, Func, WithSeq );
    else
        ReplyOrderConfirm( R02, Func, WithSeq  );
}
//---------------------------------------------------------------------------------------------------------------------
// Translate TMP R03 to X.25 R02,R100 confarmation execution with error code.
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::ReplyReject( TMP::TMPMessage& R03, Int32 Func, const UFC::AnsiString& Symbol )
{
    if( FObjectManager->IsQuote( R03 ) == TRUE )
        ReplyQuoteReject( R03, Func, Symbol );
    else
        ReplyOrderReject( R03, Func, Symbol );
}
//---------------------------------------------------------------------------------------------------------------------
// Translate TMP R02 to X.25 R020 confarmation execution.
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::ReplyOrderConfirm( TMP::TMPMessage& R02, Int32 Func, BOOL WithSeq )
{
    UFC::AnsiString BrokerID = TMP::TMPMessage::GetBrokerID( R02["fcm_id"].ToInteger() );
    UFC::AnsiString CMID     = TMP::TMPMessage::GetBrokerID( R02["cm_id"].ToInteger() );
    UFC::AnsiString OID      = R02["order_no"].ToString();    
    UFC::AnsiString Account,Price,Qty,BeforQty,AfterQty,RecvTime,Seq,Key,ExecKey,TFXTime,symbol,OrgOID;
    UFC::AnsiString IB( BrokerID.c_str() + 4, 3 );
    UInt32          ReportSeq = R02["rpt_seq"].ToInteger();
    Int32           ThisNID   = R02["user_define"].ToInteger();
    Int32           OrigNID;
    Int32           StatusCode = R02["status_code"].ToInteger();
    Int64           PxSubTotal;
    char            Ro20Str[ 256 ];    
    UInt8           SymType;
    MessageR020     R020;
    TMP::TMPSymbol* SymbolPtr = (TMP::TMPSymbol*)R02.GetField("Symbol");

    if( WithSeq == FALSE )
        ReportSeq = 0;
    SymbolPtr->GetStringSymbol( SymType, symbol);
    symbol.PadThis( 20, ' ' );
    CMID.PadThis( 4, ' ' );
    OID.PadThis( 5, ' ' );
    R020.SetFunctionCode( Func );
    R020.UpdateTime( );
    R020.SetStatusCode( HandleSlowdownWarning( StatusCode ) );
    Account.Printf("%07d", R02["investor_acno"].ToInteger() );    
    Price.Printf("%09d", R02["Price"].ToInteger() );
    Qty.Printf("%04d", R02["qty"].ToInteger() );
    R02["px_subtotal"].ToBinary((UInt8*)& PxSubTotal);
    if( Func == 1 ) ///< New order confirmation force "BeforeQty" equals 0.
    {
        BeforQty.Printf("%04d", 0 );
        AfterQty.Printf("%04d", R02["before_qty"].ToInteger() );
    }
    else
    {
        BeforQty.Printf("%04d", R02["before_qty"].ToInteger() );
        AfterQty.Printf("%04d", R02["LeavesQty"].ToInteger() );
    }
    RecvTime.Printf("%08d", R02["org_trans_time"].ToInteger()/10 );
    TFXTime.Printf("%09d", R02["TransactTime"].ToInteger() );
    Seq.Printf("%08d", ReportSeq );
    ///< Begin compose old TMP message.
    memcpy( R020.FcmNo, BrokerID.c_str(), 7 );
    memcpy( R020.OrderNo, OID.c_str(), 5 );
    memcpy( R020.InvestorAcno, Account.c_str(), 7 );
    R020.InvestorFlag = R02["investor_flag"].ToString()[0];
    memcpy( R020.CommodityId, symbol.c_str(), 20 );    
    R020.BuySellCode = GetBSCode( R02 );
    R020.OrderType = GetOrderType( R02 );       
    memcpy( R020.OrderPrice, Price.c_str(), 9 );    
    memcpy( R020.OrderQty, Qty.c_str(), 4 );       
    switch( R02["TimeInForce"].ToInteger() )
    {
        case 4: R020.OrderCondition = 'F';break;
        case 3: R020.OrderCondition = 'I';break;
        default:R020.OrderCondition = 'R';break;
    }
    R020.OpenOffsetFlag = GetPositionEffect( R02 );    
    memcpy( R020.ClearMemberId, CMID.c_str(), 4 );
    memcpy( R020.BeforeQty, BeforQty.c_str(), 4 );
    memcpy( R020.AfterQty, AfterQty.c_str(), 4 );
    memcpy( R020.ReceiveTime, RecvTime.c_str(), 8 );
    memcpy( R020.SequenceNo, Seq.c_str(), 8 );       
    ///< Finished.      
    R020.AsString( Ro20Str );    
    if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/ThisNID, /*Out*/OrigNID,/*Out*/OrgOID,/*Out*/IB,/*Out*/Key ) == TRUE )
    {
        if( OrgOID == OID )
        {
            ExecKey.Printf( "OC_%s:%s_%d_%03d:%d", OID.c_str(), IB.c_str(), Func, FSessionID, ReportSeq );
            RecvTime.Printf("%09d", R02["org_trans_time"].ToInteger() );
            if( Func != 5 ) ///< Function = 5, Order status query.
            {
                if( FObjectManager->OrderInfo.IsExecExist( ExecKey ) == FALSE )///< Execution not exists.
                {
                    UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
                    SendConfirmMessage( ReportSeq, &R020, ThisNID, Key, TFXTime, TRUE, StatusCode,R02["uniq_id"].ToInteger(),R02["CumQty"].ToInteger(),(double)PxSubTotal );
                }
            }
            else  ///< Reply Order status request.
            {
                UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
                SendConfirmMessage( 0, &R020, ThisNID, Key, TFXTime, FALSE, StatusCode,R02["uniq_id"].ToInteger(),R02["CumQty"].ToInteger(),(double)PxSubTotal );
            }
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Reply R020[%s]", Ro20Str );
        }
        else
        {
            UFC::BufferedLog::Printf( " *** FATAL ERROR!! miss match OrderID***" );
            UFC::BufferedLog::Printf( " Original OID[%s] NID[%d]",OrgOID.c_str(), OrigNID  );
            UFC::BufferedLog::Printf( " Current execution OID[%s] NID[%d]",OID.c_str(), ThisNID  );
        }
    }
    else    
        UFC::BufferedLog::DebugPrintf( UFC::dlWarning, " Reply Broker[%s] R020[%s] OID not found", BrokerID.c_str(), Ro20Str );
 }
//---------------------------------------------------------------------------------------------------------------------
// Translate TMP R02 to X.25 R090 confarmation execution.
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::ReplyQuoteConfirm( TMP::TMPMessage& R02, Int32 Func, BOOL WithSeq )
{
    UFC::AnsiString BrokerID = TMP::TMPMessage::GetBrokerID( R02["fcm_id"].ToInteger() );
    UFC::AnsiString CMID     = TMP::TMPMessage::GetBrokerID( R02["cm_id"].ToInteger() );
    UFC::AnsiString OID      = R02["order_no"].ToString();
    UFC::AnsiString Account,Price,Qty,BeforQty,AfterQty,RecvTime,Seq,Key,ExecKey,TFXTime,symbol,OrgOID;
    UFC::AnsiString IB( BrokerID.c_str() + 4, 3 );
    Int32           ThisNID   = R02["user_define"].ToInteger();
    UInt32          ReportSeq = R02["rpt_seq"].ToInteger();
    Int32           OrigNID;
    Int32           StatusCode = R02["status_code"].ToInteger();
    char            R100Str[ 256 ];
    UInt8           SymType;
    Int64           PxSubTotal;
    MessageR100     R100;
    TMP::TMPSymbol* SymbolPtr = (TMP::TMPSymbol*)R02.GetField("Symbol");

    if( WithSeq == FALSE )
        ReportSeq = 0;
    SymbolPtr->GetStringSymbol( SymType, symbol);
    symbol.PadThis( 20, ' ' );
    CMID.PadThis( 4, ' ' );
    OID.PadThis( 5, ' ' );
    R100.SetFunctionCode( Func );
    R100.UpdateTime( );
    R100.SetStatusCode( HandleSlowdownWarning( StatusCode ) );
    Account.Printf("%07d", R02["investor_acno"].ToInteger() );
    Price.Printf("%09d", R02["Price"].ToInteger() );
    Qty.Printf("%04d", R02["qty"].ToInteger() );
    R02["px_subtotal"].ToBinary((UInt8*)& PxSubTotal);
    if( Func == 1 ) ///< New order confirmation force "BeforeQty" equals 0.
    {
        BeforQty.Printf("%04d", 0 );
        AfterQty.Printf("%04d", R02["before_qty"].ToInteger() );
    }
    else
    {
        BeforQty.Printf("%04d", R02["before_qty"].ToInteger() );
        AfterQty.Printf("%04d", R02["LeavesQty"].ToInteger() );
    }
    RecvTime.Printf("%08d", R02["org_trans_time"].ToInteger()/10 );
    TFXTime.Printf("%09d", R02["TransactTime"].ToInteger() );
    Seq.Printf("%08d", ReportSeq );
    ///< Begin compose old TMP message.
    memcpy( R100.FcmNo, BrokerID.c_str(), 7 );
    memcpy( R100.OrderNo, OID.c_str(), 5 );
    memcpy( R100.InvestorAcno, Account.c_str(), 7 );
    R100.InvestorFlag = R02["investor_flag"].ToString()[0];
    memcpy( R100.CommodityId, symbol.c_str(), 20 );
    if( GetBSCode( R02 ) == 'B' )
    {
        memcpy( R100.BuyPrice, Price.c_str(), 9 );  
        memcpy( R100.BuyQty, Qty.c_str(), 4 );      
        memcpy( R100.SellPrice, "000000000", 9 );  
        memcpy( R100.SellQty, "0000", 4 );     
    }
    else
    {
        memcpy( R100.BuyPrice, "000000000", 9 );
        memcpy( R100.BuyQty, "0000", 4 );       
        memcpy( R100.SellPrice, Price.c_str(), 9 );
        memcpy( R100.SellQty, Qty.c_str(), 4 );    
    }
    if( R02["TimeInForce"].ToInteger() == 8 )
        R100.OrderCondition = 'Q';
    else
        R100.OrderCondition = 'R';
    R100.OpenOffsetFlag = '9';
    memcpy( R100.ClearMemberId, CMID.c_str(), 4 );
    memcpy( R100.BeforeQty, BeforQty.c_str(), 4 );
    memcpy( R100.AfterQty, AfterQty.c_str(), 4 );
    memcpy( R100.ReceiveTime, RecvTime.c_str(), 8 );
    memcpy( R100.SequenceNo, Seq.c_str(), 8 );
    ///< Finished.
    R100.AsString( R100Str );
    if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/ThisNID, /*Out*/OrigNID,/*Out*/OrgOID,/*Out*/IB,/*Out*/Key ) == TRUE )
    {
        if( OrgOID == OID )
        {
            ExecKey.Printf( "QC_%s:%s_%d_%c_%03d:%d", OID.c_str(), IB.c_str(), Func, GetBSCode( R02 ), FSessionID, ReportSeq );
            RecvTime.Printf("%09d", R02["org_trans_time"].ToInteger() );
            if( Func != 5 ) ///< Function = 5, Order status query.
            {
                if( FObjectManager->OrderInfo.IsExecExist( ExecKey ) == FALSE )///< Execution not exists.
                {
                    UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
                    SendConfirmMessage( ReportSeq, &R100, ThisNID, Key, TFXTime, TRUE, StatusCode,R02["uniq_id"].ToInteger(),R02["CumQty"].ToInteger(),(double)PxSubTotal );
                }
            }
            else  ///< Reply Order status request.(No Seq, need not log)
            {
                UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
                SendConfirmMessage( 0, &R100, ThisNID, Key, TFXTime, FALSE, StatusCode,R02["uniq_id"].ToInteger(),R02["CumQty"].ToInteger(),(double)PxSubTotal );
            }
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Reply R100[%s]", R100Str );
        }
        else
        {
            UFC::BufferedLog::Printf( " *** FATAL ERROR!! miss match OrderID***" );
            UFC::BufferedLog::Printf( " Original OID[%s] NID[%d]",OrgOID.c_str(), OrigNID  );
            UFC::BufferedLog::Printf( " Current execution OID[%s] NID[%d]",OID.c_str(), ThisNID  );
        }
    }
    else
        UFC::BufferedLog::DebugPrintf( UFC::dlWarning, " Reply Broker[%s] R100[%s] OID not found", BrokerID.c_str(), R100Str );
 }
//---------------------------------------------------------------------------------------------------------------------
// Translate TMP R03 to X.25 R02 confarmation execution with error code.
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::ReplyOrderReject( TMP::TMPMessage& R03, Int32 Func, const UFC::AnsiString& Sym )
{
    UFC::AnsiString BrokerID = TMP::TMPMessage::GetBrokerID( R03["fcm_id"].ToInteger() );
    UFC::AnsiString CMID     = BrokerID.SubString( 0, 4 );
    UFC::AnsiString OID      = R03["order_no"].ToString();
    UFC::AnsiString Symbol( Sym );
    UFC::AnsiString RecvTime,Seq,Key,CKey,TFXTime,OrgOID,Account;
    UFC::AnsiString IB( BrokerID.c_str() + 4, 3 );
    UInt32          ReportSeq  = R03["rpt_seq"].ToInteger();
    Int32           ThisNID    = R03["user_define"].ToInteger();
    Int32           StatusCode = R03["status_code"].ToInteger();
    Int32           OrigNID;
    MessageR020     R020;

    Symbol.PadThis( 20, ' ' );
    Account.Printf("%07d", R03["ord_id"].ToInteger() %10000000 );
    R020.UpdateTime( );
    R020.SetFunctionCode( Func );
    R020.SetStatusCode( HandleSlowdownWarning( StatusCode ));
    RecvTime.Printf("%08d", R03.GetMessageTime().ToInteger()/10 );
    TFXTime.Printf("%09d", R03.GetMessageTime().ToInteger() );
    Seq.Printf("%08d", ReportSeq );
    ///< Begin compose old TMP message.    
    memcpy( R020.FcmNo, BrokerID.c_str(), 7 );
    memcpy( R020.OrderNo, OID.c_str(), 5 );
    memcpy( R020.InvestorAcno, Account.c_str(), 7 );
    R020.InvestorFlag = ' ';
    memcpy( R020.CommodityId, Symbol.c_str(), 20 );
    if( R03["ord_id"].ToInteger()/10000000 == 1)
        R020.BuySellCode = 'B';
    else
        R020.BuySellCode = 'S';
    R020.OrderType = 'L';       
    memset( R020.OrderPrice, '0', 9 );    
    memset( R020.OrderQty, '0', 4 );       
    R020.OrderCondition = 'R';
    R020.OpenOffsetFlag = '0';
    memcpy( R020.ClearMemberId, CMID.c_str(), 4 );
    memset( R020.BeforeQty, '0', 4 );
    memset( R020.AfterQty, '0', 4 );
    memcpy( R020.ReceiveTime, RecvTime.c_str(), 8 );
    memcpy( R020.SequenceNo, Seq.c_str(), 8 );
    ///< Finished.
    RecvTime.Printf("%09d", R03.GetMessageTime().ToInteger() );
    if( Func == 1 || Func == 5 ) ///< New Order
    {
        if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/ThisNID, /*Out*/OrigNID,/*Out*/OrgOID,/*Out*/IB,/*Out*/Key ) == TRUE )
        {
            UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
            SendConfirmMessage( ReportSeq, &R020, ThisNID, Key, TFXTime, TRUE, StatusCode,0,0,0 );
        }
    }
    else if( Func == 2 ) ///< Replace Order
    {
        if( FObjectManager->OrderInfo.PopReplaceNID( /*In*/ThisNID, /*In*/OID, /*In*/IB, Key, CKey ) == TRUE )
        {
            UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
            SendConfirmMessage( ReportSeq, &R020, ThisNID, Key, CKey, TFXTime, TRUE, StatusCode,0,0,0 );
        }
    }
    else if( Func == 3 ) ///< Cancel Order
    {
        if( FObjectManager->OrderInfo.PopCancelNID( /*In*/ThisNID, /*In*/OID, /*In*/IB, Key, CKey ) == TRUE )
        {            
            UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
            SendConfirmMessage( ReportSeq, &R020, ThisNID, Key, CKey, TFXTime, TRUE, StatusCode,0,0,0 );
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------
// Translate TMP R03 to X.25 R02 confarmation execution with error code.
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::ReplyQuoteReject( TMP::TMPMessage& R03, Int32 Func, const UFC::AnsiString& Sym )
{
    UFC::AnsiString BrokerID = TMP::TMPMessage::GetBrokerID( R03["fcm_id"].ToInteger() );
    UFC::AnsiString CMID     = BrokerID.SubString( 0, 4 );
    UFC::AnsiString OID      = R03["order_no"].ToString();
    UFC::AnsiString Symbol( Sym );
    UFC::AnsiString RecvTime,Seq,Key,CKey,TFXTime,OrgOID,Account;
    UFC::AnsiString IB( BrokerID.c_str() + 4, 3 );
    UInt32          ReportSeq = R03["rpt_seq"].ToInteger();
    Int32           ThisNID   = R03["user_define"].ToInteger();
    Int32           StatusCode = R03["status_code"].ToInteger();
    Int32           OrigNID;
    MessageR100     R100;

    Account.Printf("%07d", R03["ord_id"].ToInteger()%10000000 );
    Symbol.PadThis( 20, ' ' );
    CMID.PadThis( 4, ' ' );
    OID.PadThis( 5, ' ' );
    R100.SetFunctionCode( Func );
    R100.UpdateTime( );
    R100.SetStatusCode( HandleSlowdownWarning( StatusCode ) );
    RecvTime.Printf("%08d", R03.GetMessageTime().ToInteger()/10 );
    TFXTime.Printf("%09d", R03.GetMessageTime().ToInteger() );
    Seq.Printf("%08d", ReportSeq );
    ///< Begin compose old TMP message.
    memcpy( R100.FcmNo, BrokerID.c_str(), 7 );
    memcpy( R100.OrderNo, OID.c_str(), 5 );
    memcpy( R100.InvestorAcno, Account.c_str(), 7 );
    R100.InvestorFlag = ' ';
    memcpy( R100.CommodityId, Symbol.c_str(), 20 );
    memcpy( R100.BuyPrice, "000000000", 9 );
    memcpy( R100.BuyQty, "0000", 4 );
    memcpy( R100.SellPrice, "000000000", 9 );
    memcpy( R100.SellQty, "0000", 4 );
    R100.OrderCondition = '0';
    R100.OpenOffsetFlag = '9';
    memcpy( R100.ClearMemberId, CMID.c_str(), 4 );
    memcpy( R100.BeforeQty, "0000", 4 );
    memcpy( R100.AfterQty, "0000", 4 );
    memcpy( R100.ReceiveTime, RecvTime.c_str(), 8 );
    memcpy( R100.SequenceNo, Seq.c_str(), 8 );
    ///< Finished.
    RecvTime.Printf("%09d", R03.GetMessageTime().ToInteger() );
    if( Func == 1 || Func == 5 ) ///< New Quote
    {
        if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/ThisNID, /*Out*/OrigNID,/*Out*/OrgOID,/*Out*/IB,/*Out*/Key ) == TRUE )
        {
            UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
            SendConfirmMessage( ReportSeq, &R100, ThisNID, Key, TFXTime, TRUE, StatusCode,0,0,0 );
        }
    }
    else if( Func == 2 ) ///< Replace Order
    {
        if( FObjectManager->OrderInfo.PopReplaceNID( /*In*/ThisNID, /*In*/OID, /*In*/IB, Key, CKey ) == TRUE )
        {
            UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
            SendConfirmMessage( ReportSeq, &R100, ThisNID, Key, CKey, TFXTime, TRUE, StatusCode,0,0,0 );
        }
    }
    else if( Func == 3 ) ///< Cancel Order
    {
        if( FObjectManager->OrderInfo.PopCancelNID( /*In*/ThisNID, /*In*/OID, /*In*/IB, Key, CKey ) == TRUE )
        {
            UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
            SendConfirmMessage( ReportSeq, &R100, ThisNID, Key, CKey, TFXTime, TRUE, StatusCode,0,0,0 );
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::QuoteCancelReplaceExecution( TMP::TMPMessage& R02, BOOL IsCancel )
{
    UFC::AnsiString BrokerID = TMP::TMPMessage::GetBrokerID( R02["fcm_id"].ToInteger() );
    UFC::AnsiString CMID     = TMP::TMPMessage::GetBrokerID( R02["cm_id"].ToInteger() );
    UFC::AnsiString OID      = R02["order_no"].ToString();    
    UFC::AnsiString Account,BeforQty,AfterQty,RecvTime,Seq,Key,CKey,ExecKey,TFXTime,Status,symbol;
    UFC::AnsiString IB( BrokerID.c_str() + 4, 3 );
    UInt32          ReportSeq = R02["rpt_seq"].ToInteger();
    Int32           ThisNID   = R02["user_define"].ToInteger();
    UInt8           SymType;
    Int64           PxSubTotal;
    C030QuoteReduceCancelBody C030;
    TMP::TMPSymbol* SymbolPtr = (TMP::TMPSymbol*)R02.GetField("Symbol");

    SymbolPtr->GetStringSymbol( SymType, symbol);
    symbol.PadThis( 20, ' ' );
    CMID.PadThis( 4, ' ' );
    OID.PadThis( 5, ' ' );
    Status.Printf("%02d", HandleSlowdownWarning( R02["status_code"].ToInteger() ) );
    Account.Printf("%07d", R02["investor_acno"].ToInteger() );    
    BeforQty.Printf("%04d", R02["before_qty"].ToInteger() );
    AfterQty.Printf("%04d", R02["LeavesQty"].ToInteger() );
    RecvTime.Printf("%08d", R02["org_trans_time"].ToInteger()/10 );
    TFXTime.Printf("%09d", R02["TransactTime"].ToInteger() );
    Seq.Printf("%08d", ReportSeq );
    R02["px_subtotal"].ToBinary((UInt8*)& PxSubTotal);
    ///< Begin compose old TMP message.
    memcpy( C030.FStatusCode, Status.c_str(), 2 );
    memcpy( C030.FBroker, BrokerID.c_str(), 7 );
    memcpy( C030.FOrderNo, OID.c_str(), 5 );
    memcpy( C030.FInvestorACNo, Account.c_str(), 7 );
    C030.FInvestorFlag = R02["investor_flag"].ToString()[0];
    memcpy( C030.FProdID, symbol.c_str(), 20 );    
    C030.FBuySellCode = GetBSCode( R02 );
    memcpy( C030.FCMID, CMID.c_str(), 4 );
    if( IsCancel == TRUE) 
        C030.FTransCode = '3';
    else
        C030.FTransCode = '2';
    memcpy( C030.FBeforeQuntity, BeforQty.c_str(), 4 );
    memcpy( C030.FAfterQuntity, AfterQty.c_str(), 4 );        
    memcpy( C030.FReceiveTime, RecvTime.c_str(), 8 );
    memcpy( C030.FSeqNo, Seq.c_str(), 8 );       
    C030.AsString( FDataStr );                            
    ///< Finished.       
    ExecKey.Printf( "QCR_%s:%s_%c_%03d:%d", OID.c_str(), IB.c_str(), C030.FTransCode, FSessionID, ReportSeq );
    RecvTime.Printf("%09d", R02["org_trans_time"].ToInteger() );
    if( FObjectManager->OrderInfo.IsExecExist( ExecKey ) == FALSE )///< Execution not exists.
    {
        if( IsCancel == TRUE )///< Order Canceled
        {
            if( FObjectManager->OrderInfo.PopCancelNID( /*In*/ThisNID, /*In*/OID, /*In*/IB, Key, CKey ) == TRUE )
            {
                UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
                SendFillMessage( ReportSeq, ThisNID, TFXTime.c_str(),RecvTime.c_str(), Key, CKey, R02["uniq_id"].ToInteger(),R02["CumQty"].ToInteger(),(double)PxSubTotal,TRUE );
            }
        }
        else///< Order Replaced
        {
            if( FObjectManager->OrderInfo.PopReplaceNID( /*In*/ThisNID, /*In*/OID, /*In*/IB, Key, CKey ) == TRUE )
            {
                UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
                SendFillMessage( ReportSeq, ThisNID, TFXTime.c_str(),RecvTime.c_str(), Key, CKey ,R02["uniq_id"].ToInteger(),R02["CumQty"].ToInteger(),(double)PxSubTotal,TRUE );
            }
        }
    }
}    
//---------------------------------------------------------------------------------------------------------------------
// Translate TMP R02 to X.25 C030 (Cancel/Replace) execution.
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::CancelReplaceExecution( TMP::TMPMessage& R02, BOOL IsCancel )
{
    UFC::AnsiString BrokerID = TMP::TMPMessage::GetBrokerID( R02["fcm_id"].ToInteger() );
    UFC::AnsiString CMID     = TMP::TMPMessage::GetBrokerID( R02["cm_id"].ToInteger() );
    UFC::AnsiString OID      = R02["order_no"].ToString();
    UFC::AnsiString Account,BeforQty,AfterQty,RecvTime,Seq,Key,CKey,ExecKey,TFXTime,Status,symbol;
    UFC::AnsiString IB( BrokerID.c_str() + 4, 3 );
    UInt32          ReportSeq = R02["rpt_seq"].ToInteger();
    Int32           ThisNID   = R02["user_define"].ToInteger();
    UInt8           SymType;
    Int64           PxSubTotal;
    C030ReduceCancelBody C030;
    TMP::TMPSymbol* SymbolPtr = (TMP::TMPSymbol*)R02.GetField("Symbol");

    SymbolPtr->GetStringSymbol( SymType, symbol);
    symbol.PadThis( 20, ' ' );
    CMID.PadThis( 4, ' ' );
    OID.PadThis( 5, ' ' );
    Status.Printf("%02d", HandleSlowdownWarning( R02["status_code"].ToInteger() ) );
    Account.Printf("%07d", R02["investor_acno"].ToInteger() );
    BeforQty.Printf("%04d", R02["before_qty"].ToInteger() );
    AfterQty.Printf("%04d", R02["LeavesQty"].ToInteger() );
    RecvTime.Printf("%08d", R02["org_trans_time"].ToInteger()/10 );
    TFXTime.Printf("%09d", R02["TransactTime"].ToInteger() );
    Seq.Printf("%08d", ReportSeq );
    R02["px_subtotal"].ToBinary((UInt8*)& PxSubTotal);
    ///< Begin compose old TMP message.
    memcpy( C030.FStatusCode, Status.c_str(), 2 );
    memcpy( C030.FBroker, BrokerID.c_str(), 7 );
    memcpy( C030.FOrderNo, OID.c_str(), 5 );
    memcpy( C030.FInvestorACNo, Account.c_str(), 7 );
    C030.FInvestorFlag = R02["investor_flag"].ToString()[0];
    memcpy( C030.FProdID, symbol.c_str(), 20 );
    C030.FBuySellCode = GetBSCode( R02 );
    memcpy( C030.FCMID, CMID.c_str(), 4 );
    if( IsCancel == TRUE)
        C030.FTransCode = '3';
    else
        C030.FTransCode = '2';
    memcpy( C030.FBeforeQuntity, BeforQty.c_str(), 4 );
    memcpy( C030.FAfterQuntity, AfterQty.c_str(), 4 );
    memcpy( C030.FReceiveTime, RecvTime.c_str(), 8 );
    memcpy( C030.FSeqNo, Seq.c_str(), 8 );
    C030.AsString( FDataStr );
    ///< Finished.
    ExecKey.Printf( "OCR_%s:%s_%c_%03d:%d", OID.c_str(), IB.c_str(), C030.FTransCode, FSessionID, ReportSeq );
    RecvTime.Printf("%09d", R02["org_trans_time"].ToInteger() );
    if( FObjectManager->OrderInfo.IsExecExist( ExecKey ) == FALSE )///< Execution not exists.
    {
        if( IsCancel == TRUE )///< Order Canceled
        {
            if( FObjectManager->OrderInfo.PopCancelNID( /*In*/ThisNID, /*In*/OID, /*In*/IB, Key, CKey ) == TRUE )
            {
                UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
                SendFillMessage( ReportSeq, ThisNID, TFXTime.c_str(),RecvTime.c_str(), Key, CKey,R02["uniq_id"].ToInteger(),R02["CumQty"].ToInteger(),(double)PxSubTotal,TRUE );
            }
        }
        else///< Order Replaced
        {
            if( FObjectManager->OrderInfo.PopReplaceNID( /*In*/ThisNID, /*In*/OID, /*In*/IB, Key, CKey ) == TRUE )
            {
                UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 4, UFC::AnsiString(ThisNID),OID );
                SendFillMessage( ReportSeq, ThisNID, TFXTime.c_str(),RecvTime.c_str(), Key, CKey,R02["uniq_id"].ToInteger(),R02["CumQty"].ToInteger(),(double)PxSubTotal,TRUE );
            }
        }
    }
}
//------------------------------------------------------------------------------
BOOL SpeedyTMPOrderClient::IsIOCPartialFill( TMP::TMPMessage& R02 )
{
    if( R02["TimeInForce"].ToInteger() == 3 ) ///< IOC
    {
        int BeforQty  = R02["before_qty"].ToInteger();
        int LastQty   = R02["LastQty"].ToInteger();
        int LeavesQty = R02["LeavesQty"].ToInteger();

        if( LeavesQty == 0 && LastQty > 0 && BeforQty > LastQty )
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " IOC Partial fill Before[%d] Leaves[%d] Last[%d]", BeforQty, LeavesQty, LastQty );
            R02["rpt_seq"] = 0;
            R02["LastQty"] = 0;
            R02["LastPx"]  = 0;
            return TRUE;
        }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
// Translate TMP R02 to X.25 C030 (Single/Multileg) filled execution.
//------------------------------------------------------------------------------
void SpeedyTMPOrderClient::FillExecution( TMP::TMPMessage& R02, BOOL IsFill )
{
    UFC::AnsiString Account,Seq,Key,ExecKey,RecvTime,TFXTime,Price,Qty,symbol;
    UFC::AnsiString BrokerID = TMP::TMPMessage::GetBrokerID( R02["fcm_id"].ToInteger() );
    UFC::AnsiString CMID     = TMP::TMPMessage::GetBrokerID( R02["cm_id"].ToInteger() );
    UFC::AnsiString OID      = R02["order_no"].ToString(); 
    UFC::AnsiString IB( BrokerID.c_str() + 4, 3 );
    UInt32          ReportSeq = R02["rpt_seq"].ToInteger();
    Int32           OrigNID;
    UInt8           SymType;
    Int64           PxSubTotal;
    TMP::TMPSymbol* SymbolPtr = (TMP::TMPSymbol*)R02.GetField("Symbol");    
    
    SymbolPtr->GetStringSymbol( SymType, symbol);
    symbol.PadThis( 20, ' ' );
    CMID.PadThis( 4, ' ' );
    OID.PadThis( 5, ' ' );
    Account.Printf("%07d", R02["investor_acno"].ToInteger() );    
    RecvTime.Printf("%08d", R02["org_trans_time"].ToInteger()/10 );
    TFXTime.Printf("%08d", R02["TransactTime"].ToInteger()/10 );
    Seq.Printf("%08d", ReportSeq );
    R02["px_subtotal"].ToBinary((UInt8*)& PxSubTotal);
    if( R02["leg_side1"].ToInteger() == 0 ) ///< Single order
    {
        C030SingleBody  C030;                           

        if( IsFill == TRUE )
        {
            if( R02["LastQty"].ToInteger() > 0 )
                Price.Printf("%09d", R02["LastPx"].ToInteger() );
            else
                Price.Printf("%09d", R02["Price"].ToInteger() );
            
            Qty.Printf("%04d", R02["LastQty"].ToInteger() );
        }
        else
        {
            Price.Printf("%09d", 0 ); ///< Cancel order, Px = 0
            Qty.Printf("%04d", 0 );   ///< Cancel order, Qty = 0
        }
        memcpy( C030.FBroker, BrokerID.c_str(), 7 );
        memcpy( C030.FOrderNo, OID.c_str(), 5 );  
        memcpy( C030.FInvestorACNo, Account.c_str(), 7 );
        C030.FInvestorFlag = R02["investor_flag"].ToString()[0];
        memcpy( C030.FProdID, symbol.c_str(), 20 );
        C030.FBuySellCode = GetBSCode( R02 );
        C030.FOrderType = GetOrderType( R02 );       
        memcpy( C030.FPrice, Price.c_str(), 9 );
        memcpy( C030.FQuntity, Qty.c_str(), 4 );
        C030.FOCCode = GetPositionEffect( R02 );            
        memcpy( C030.FCMID, CMID.c_str(), 4 );
        memcpy( C030.FMatchTime, TFXTime.c_str(), 8 );
        memcpy( C030.FSeqNo, Seq.c_str(), 8 );
        if( FObjectManager->IsOptions() == TRUE)
            C030.FProdType = 'O';///< F: Future O:Options.
        else
            C030.FProdType = 'F';///< F: Future O:Options.        
        C030.AsString( FDataStr );        
    }
    else
    {
        UFC::AnsiString LegPx1,LegQty1,LegPx2,LegQty2;
        BO_SymbolDeCompose MultilegSymbol;
        C030ComboBody C030;

        if( IsFill == TRUE )
        {
            Price.Printf("%09d", R02["LastPx"].ToInteger() );
            Qty.Printf("%04d", R02["LastQty"].ToInteger() );
            LegPx1.Printf("%09d", R02["leg_px1"].ToInteger() );
            LegPx2.Printf("%09d", R02["leg_px2"].ToInteger() );
            LegQty1.Printf("%04d", R02["leg_qty1"].ToInteger() );
            LegQty2.Printf("%04d", R02["leg_qty2"].ToInteger() );
        }
        else
        {
            Price.Printf("%09d", 0 ); ///< Cancel order, Px = 0
            Qty.Printf("%04d", 0 );   ///< Cancel order, Qty = 0
            LegPx1.Printf("%09d", 0 );
            LegPx2.Printf("%09d", 0 );
            LegQty1.Printf("%04d", 0 );
            LegQty2.Printf("%04d", 0 );
        }
        memcpy( C030.FBroker, BrokerID.c_str(), 7 );
        memcpy( C030.FOrderNo, OID.c_str(), 5 );
        memcpy( C030.FInvestorACNo, Account.c_str(), 7 );
        C030.FInvestorFlag = R02["investor_flag"].ToString()[0];
        memcpy( C030.FProdID, symbol.c_str(), 20 );
        C030.FBuySellCode = GetBSCode( R02 );        
        MultilegSymbol.SetSymbol( symbol, C030.FBuySellCode );
        memcpy( C030.FProdID1, MultilegSymbol.FComm1, 10 );
        C030.FBuySellCode1 = MultilegSymbol.FBS_Code1;
        memcpy( C030.FMatchPrice1, LegPx1.c_str(), 9 );
        memcpy( C030.FMatchQuntity1, LegQty1.c_str(), 4 );                
        memcpy( C030.FProdID2, MultilegSymbol.FComm2, 10 );        
        C030.FBuySellCode2 = MultilegSymbol.FBS_Code2;
        memcpy( C030.FMatchPrice2, LegPx2.c_str(), 9 );
        memcpy( C030.FMatchQuntity2, LegQty2.c_str(), 4 );                
        C030.FOrderType = GetOrderType( R02 );              
        memcpy( C030.FPrice, Price.c_str(), 9 );
        memcpy( C030.FQuntity, Qty.c_str(), 4 );        
        C030.FOCCode = GetPositionEffect( R02 );
        memcpy( C030.FCMID, CMID.c_str(), 4 );
        memcpy( C030.FMatchTime, TFXTime.c_str(), 8 );
        memcpy( C030.FSeqNo, Seq.c_str(), 8 );
        if( FObjectManager->IsOptions() == TRUE)
        {
            C030.FProd1Type = 'O';
            C030.FProd2Type = 'O';
        }
        else
        {
            C030.FProd1Type = 'F';
            C030.FProd2Type = 'F';
        }
        C030.AsString( FDataStr );
    }
    ExecKey.Printf( "OF_%s:%s_%03d:%d", OID.c_str(), IB.c_str(), FSessionID, ReportSeq );
    if( IsFill == FALSE || FObjectManager->OrderInfo.IsExecExist( ExecKey ) == FALSE ) ///< Execution not sent(IsExecExist will add the new key to hashset)
    {
        if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/OID,/*In*/IB,/*Out*/Key,/*Out*/OrigNID ) == TRUE )
        {
            RecvTime.Printf("%09d", R02["org_trans_time"].ToInteger());
            TFXTime.Printf("%09d", R02["TransactTime"].ToInteger());
            UFC::Profiler::SetPerformanceFlag( FPerforanceGroup, 6, UFC::AnsiString(OrigNID),OID );
            SendFillMessage( ReportSeq, OrigNID, TFXTime.c_str(), RecvTime.c_str(), Key,"",R02["uniq_id"].ToInteger(),R02["CumQty"].ToInteger(),(double)PxSubTotal);
        }
    }
    else
        UFC::BufferedLog::DebugPrintf( UFC::dlWarning, " Execution:%s already exist, skip it.", ExecKey.c_str() );            
}
//---------------------------------------------------------------------------------------------------------------------
// Publish Order Confirmation to MBus.
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::SendConfirmMessage( Int32 Seq,MessageHeader* Msg, Int32 NID, UFC::AnsiString& Key, UFC::AnsiString& TFXTime, BOOL NeedLog, Int32 Status, Int32 UID,Int32 CumQty,double PxSubTotal )
{
    MTree Data;
    UFC::AnsiString X25Time;

    UFC::GetTimeString( X25Time, FALSE );
    Msg->AsString( FDataStr );
    Data.append( "CONFIRM_ORDER", FDataStr );
    Data.append( "PVC", FSID );
    Data.append( "PBNO", FPBNO );
    Data.append( "NID", NID );
    ///< Begin for TMP onky fields.
    Data.append( "TMPCODE", Status );
    Data.append( "TMPBRKID", (Int32)FLineBrokerIndex );
    Data.append( "RPTSEQ", Seq );
    Data.append( "TMPUID", UID );
    Data.append( "TMPCumQty", CumQty );
    Data.append( "TMPPxTotal", PxSubTotal );
    ///< End TMP only fields.
    Data.append( "TTIME", TFXTime );  ///< Map to TransactTime
    Data.append( "XTIME", X25Time );  ///< Map to org_trans_time
    Data.append( "PHOST", UFC::Hostname );
    Data.append( "HOST", UFC::Hostname );
    if( Key.Length() > 0 )
        Data.append( "KEY", Key );
    else
        Key = "null";
    if( NeedLog == TRUE )
        FObjectManager->OrderInfo.WriteExecutionLog( TRUE, NID, Key, FDataStr, FPBNO, UFC::Hostname, FSID, Seq ); ///< Write log
    if( MessageObj != NULL )
        MessageObj->Send( FObjectManager->GetConfirmSubject(), UFC::Hostname, Data  );
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::SendConfirmMessage( Int32 Seq,MessageHeader* Msg, Int32 NID, UFC::AnsiString& Key,UFC::AnsiString& CKey, UFC::AnsiString& TFXTime, BOOL NeedLog, Int32 Status, Int32 UID,Int32 CumQty,double PxSubTotal )
{
    MTree Data;
    UFC::AnsiString X25Time;

    UFC::GetTimeString( X25Time, FALSE );
    Msg->AsString( FDataStr );
    Data.append( "CONFIRM_ORDER", FDataStr );
    Data.append( "PVC", FSID );
    Data.append( "PBNO", FPBNO );
    Data.append( "NID", NID );
    ///< Begin for TMP onky fields.
    Data.append( "TMPCODE", Status ); 
    Data.append( "TMPBRKID", (Int32)FLineBrokerIndex );
    Data.append( "RPTSEQ", Seq );
    Data.append( "TMPUID", UID );
    Data.append( "TMPCumQty", CumQty );
    Data.append( "TMPPxTotal", PxSubTotal );
    ///< End TMP only fields.
    Data.append( "TTIME", TFXTime );
    Data.append( "XTIME", X25Time );
    Data.append( "PHOST", UFC::Hostname );
    Data.append( "HOST", UFC::Hostname );
    if( Key.Length() > 0 )
        Data.append( "KEY", Key );
    else
        Key = "null";
    if( CKey.Length() > 0 )
        Data.append( "CKEY", CKey );
    if( NeedLog == TRUE )
        FObjectManager->OrderInfo.WriteExecutionLog( TRUE, NID, Key, FDataStr, FPBNO, UFC::Hostname, FSID, Seq ); ///< Write log
    if( MessageObj != NULL )
        MessageObj->Send( FObjectManager->GetConfirmSubject(), UFC::Hostname, Data  );
}
//---------------------------------------------------------------------------------------------------------------------
// Publish Order filled to MBus.
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderClient::SendFillMessage( Int32 Seq,Int32 OrigNID, const UFC::AnsiString& TAIFEXTime, const UFC::AnsiString& X25Time, const UFC::AnsiString& Key, const UFC::AnsiString& CKey, Int32 UID,Int32 CumQty,double PxSubTotal,BOOL IsCR  )
{
    MTree Data;

    Data.append( "FILL_ORDER", FDataStr );
    Data.append( "PVC",   FSID );
    Data.append( "PBNO",  FPBNO );
    Data.append( "TTIME", TAIFEXTime );
    Data.append( "XTIME", X25Time );
    Data.append( "NID",   OrigNID );
    ///< Begin for TMP onky fields.
    Data.append( "TMPBRKID", (Int32)FLineBrokerIndex );
    Data.append( "TMPUID", UID );
    Data.append( "TMPCumQty", CumQty );
    Data.append( "TMPPxTotal", PxSubTotal );
    Data.append( "RPTSEQ", Seq );
    ///< End TMP only fields.
    if( Key.Length() > 0 )
        Data.append( "KEY", Key );
    if( CKey.Length() > 0 )
        Data.append( "CKEY", CKey );
    Data.append( "PHOST", UFC::Hostname );
    if( IsCR == TRUE )
        FObjectManager->OrderInfo.WriteExecutionLog( FALSE, OrigNID, CKey, FDataStr, FPBNO, UFC::Hostname, FSID, Seq ); ///< Write log
    else
        FObjectManager->OrderInfo.WriteExecutionLog( FALSE, OrigNID, Key, FDataStr, FPBNO, UFC::Hostname, FSID, Seq ); ///< Write log
    if( MessageObj != NULL )
        MessageObj->Send( FObjectManager->GetFillSubject(), UFC::Hostname, Data );
}
//---------------------------------------------------------------------------------------------------------------------        


