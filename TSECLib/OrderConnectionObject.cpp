#include "OrderConnectionObject.h"
#include "LinkSubSystem.h"
//----------------------------------------------------------------------------------------------------------------------
extern BOOL IsTSEC;
extern BOOL IsOldVersion;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
//
//  ### TWSE side ###
//  Connection object for AP code 0 Order subsystem.
//
//  Warp TWSEFIXConnection or TWSETCPConnection
//  can supports both TMP and FIX protocol.    
//
//----------------------------------------------------------------------------------------------------------------------
TSECOrderConnectionObject::TSECOrderConnectionObject( BOOL IsTWSE, ConnectionParameter& Info, TransactionObject* TObj, TSECOrderManager* Owner )
:ConnectionObjectBase( Info ),
 UFC::PThread( Owner, FALSE ),
 FNeedLock( FALSE ),
 FNeedReset( FALSE ),
 FConnected( FALSE ),
 FBackup( FALSE ),
 FTransactionObj( TObj ),
 FConnectionListener( Owner ),
 FOwner( Owner ),
 FConnection( NULL ),
 FLog( NULL )
{    
    if( Info.GetThroughput() > TMP_THROUGHPUT )
    {
        FIsFIX = TRUE; 
        FConnection = new TWSEFIXConnection( IsTSEC, FParams, this );        
    }
    else
    {
        FIsFIX = FALSE;    
        FConnection = new TWSETCPConnection( IsTSEC, FParams );
    }
        
}
//----------------------------------------------------------------------------------------------------------------------
TSECOrderConnectionObject::~TSECOrderConnectionObject()
{
    Close();
}
//----------------------------------------------------------------------------------------------------------------------
void TSECOrderConnectionObject::UpdateConnectionParameter( ConnectionParameter& Info )
{
    FParams     = Info;
    FNeedLock   = FALSE;
    FNeedReset  = TRUE;
    FBackup     = FALSE;
    UFC::BufferedLog::Printf( " PVC:%d Changed to FCM:%s Password:%s", FParams.GetPVC(), FParams.GetBrokerID().c_str(), FParams.GetPassword().c_str());
}
//----------------------------------------------------------------------------------------------------------------------
void TSECOrderConnectionObject::OnCommand( int Command, void* Data )
{
    switch( Command )
    {
        case COMMAND_OPEN_MARKET: break;
        case COMMAND_CLOSE_MARKET:Terminate();
                                  break;
        case COMMAND_UNLOCK_PVC:  FNeedLock  = FALSE;
                                  FBackup    = FALSE;
                                  if( FConnected == TRUE )
                                      FNeedReset = TRUE;
                                  break;
        case COMMAND_LOCK_PVC:    FNeedLock  = TRUE;
                                  FBackup    = TRUE;
                                  break;
        case COMMAND_GET_STATE:   if( FBackup == TRUE )
                                      *((int*)Data ) = PVC_STATUS_BACKUP;
                                  else
                                      *((int*)Data ) = FConnection->GetStatus();
                                  break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TSECOrderConnectionObject::Close( void )
{
    FConnection->StopHandshake();
    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TSECOrderConnectionObject::ConfirmsubSystemHandshake( BOOL ReceivedL010 )
{
    if( FConnected == TRUE )
    {
        FConnected = FALSE;
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
    }
    FNeedLock = (!FConnection->OnlineHandshake( ReceivedL010 ));
    if( FNeedLock == TRUE )
    {
    	FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_LOCKED );// Lock this connection.
    }
    else
    {
        FConnected = TRUE;
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY );
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TSECOrderConnectionObject::CheckReceiveMessage( void )
{
    MessageHeader* InMsg;

    for( int i = 0; i < TWSE_ORDERSUBSYSTEM_TIMEOUT; i++ )
    {
         if( IsTerminated() == TRUE ) ///< Close this connection.
             return NULL;
         if( FNeedReset == TRUE ) // Received PVC reset command.
         {
             FNeedReset = FALSE;
             throw( X25Exception( 0, "Received PVC reset.") );
         }
         if( ( InMsg = FConnection->ReceiveMessage( 1, FALSE ) ) != NULL )
         {
            if( InMsg->IsMessage( mtL010 ) == TRUE )
                throw( X25ReconnectException( "Receive a L010." ) );
            
            char buffer[512];
            
            InMsg->AsString( buffer );
            UFC::BufferedLog::Printf( " In Message[%s]", buffer );
            return InMsg;
         }
    }
    ///< Not receive any message during ORDERSUBSYSTEM_TIMEOUT.
    if( FIsFIX == FALSE ) ///< FIX session no message
        throw( X25ReconnectException( "Receive data timeout" ) );
    return NULL;
}
//----------------------------------------------------------------------------------------------------------------------
void TSECOrderConnectionObject::MessageLoop( void )
{
    MessageHeader* InMsg;
    MessageHeader* OutMsg;
    
    while( IsTerminated() == FALSE )
    {
        try
        {
            do
            {                
                if( IsTerminated() == TRUE ) ///< Force close this connection.
                    return;
                else if( FNeedLock == TRUE ) ///< This PVC had been locked.
                    sleep( 2 );
                else                         ///< Normal state...Process the message.
                {
                    ///< Get a New/Cancel/Replaced message from client
                    if( (InMsg = CheckReceiveMessage() ) != NULL )
                    {
                        ///< Send to match engine.
                        if( ( OutMsg = FTransactionObj->OnMessage( this, InMsg, FIsFIX ) ) != NULL )
                        {
                            if( FIsFIX == TRUE )
                                UFC::SleepMS( 2 ); 
                            else
                                UFC::SleepMS( 33 );                            
                            ///< Reply exeution report. 
                            SendMessage( OutMsg );
                            delete OutMsg;
                        }
                        delete InMsg;
                    }
                    ///< Should else need to throw reconnect exception ???
                }
            } while(  IsTerminated() == FALSE );
        }
        catch( X25RehandshakeException& e) // Receive a L010...go back to handshake.
        {
            UFC::BufferedLog::Printf( " X25RehandshakeException[%s]", e.what() );
            ConfirmsubSystemHandshake( TRUE );
        }
        catch( X25ReconnectException& e)  // Timeout or reconnect
        {
            UFC::BufferedLog::Printf( " X25RehandshakeException[%s]", e.what() );
            ConfirmsubSystemHandshake( FALSE );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TSECOrderConnectionObject::LockLoop( void )
{
    while( FNeedLock == TRUE &&  IsTerminated() == FALSE ) // This PVC had been lock.
    {
        sleep( 1 );///< Empty loop 
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TSECOrderConnectionObject::SetLogManager( UFC::BufferedLog* LogObj )
{
    FLog = LogObj;
    if( FConnection != NULL )
        FConnection->SetLogManager( FLog );
}
//----------------------------------------------------------------------------------------------------------------------
void TSECOrderConnectionObject::WaitForConnected( void )
{
    while( FConnection->GetSocketFD() == -1 ) 
    {
        UFC::SleepMS( 200 );
    }
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TSECOrderConnectionObject::Accept( int FD )
{
    if( FConnection == NULL )
        return FALSE;
    FConnection->SetSocketFD( FD );
    return TRUE;
}
//----------------------------------------------------------------------------------------------------------------------
void TSECOrderConnectionObject::Execute( void )
{

    for( int i = 0; i< TWSE_PVC_BUSY_RETRY_TIMES; i++ )
    {
        if( IsTerminated() == TRUE )
            break;
        try
        {
            if( i == TWSE_PVC_BUSY_RETRY_TIMES - 1 )
            {
                FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR );// Can not establish the X.25 connection
                i = 0;
                FNeedLock = TRUE;
                LockLoop();
            }
            WaitForConnected();            
            FConnection->Open( FParams );
            ConfirmsubSystemHandshake( FALSE );
            MessageLoop();
            FConnection->Close();
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_NORMAL_CLOSE );// Close the connection normal.
            break;
        }
        catch( X25Exception& e )
        {
            UFC::BufferedLog::Printf(" %s. Try to reconnect after 2 sec...",e.what() );
            FConnection->Close();
            FConnection->SetSocketFD( -1 );
            sleep( 2 );
            continue;
        }
        catch( exception& e )
        {
            FConnection->SetSocketFD( -1 );
            UFC::BufferedLog::Printf( " PVC %s Exception:%s",GetConnectionParameter( )->GetPVCID().c_str(),e.what());
            FNeedLock = TRUE;
            LockLoop();
        }
        catch(...)
        {
            FConnection->SetSocketFD( -1 );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
            FNeedLock = TRUE;
            LockLoop();
        }        
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TSECOrderConnectionObject::SendMessage( MessageHeader* Msg )
{
    FConnection->SendMessage( Msg, TWSE_ORDERSUBSYSTEM_TIMEOUT );
}
//----------------------------------------------------------------------------------------------------------------------
//
//  ### TWSE side ###
// Class TWSEFIXConnection 
//
//----------------------------------------------------------------------------------------------------------------------
TWSEFIXConnection::TWSEFIXConnection( BOOL IsTSEC, ConnectionParameter& Info, TSECOrderConnectionObject* Owner  )
:FSession( NULL )
,FFD( -1 )
,FPVCID( Info.GetPVCID() )
,FOwner( Owner )
,FMessages( 2048 )
{
    if( IsTSEC == TRUE )
    {
        FTargetCompID.Printf( "T%s%s", Info.GetBrokerID().c_str(), Info.GetPVCID().c_str() );
        FSenderCompID = "XTAI";
    }
    else
    {
        FTargetCompID.Printf( "O%s%s", Info.GetBrokerID().c_str(), Info.GetPVCID().c_str() );
        FSenderCompID = "ROCO";
    }    
    FIX::SessionID  sessionID( FIX::BeginString_FIX44, FSenderCompID.c_str(),FTargetCompID.c_str());    
    if( (FSession = FIX::Session::lookupSession( sessionID )) == NULL )
    {
        UFC::AnsiString Msg;

        Msg.Printf( "FIX Session %s not found!", sessionID.toString().c_str() );
        throw( UFC::Exception( Msg ) );
    }   
    else
        FOwner->GetManager()->GetFIXManager()->AddFIXClient( sessionID.toString(), this );
}
//----------------------------------------------------------------------------------------------------------------------
int  TWSEFIXConnection::GetStatus()                               
{ 
    if( FSession->isLoggedOn() )
        return PVC_STATUS_ONLINE_READY;
    else
        return PVC_STATUS_BROKEN;
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::StopHandshake()                           
{
    UFC::BufferedLog::Printf( " FIX PVCID[%s] Stop Handshake", FPVCID.c_str() );
    FShaking = FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TWSEFIXConnection::OnlineHandshake( BOOL )      
{ 
    UFC::BufferedLog::Printf( " FIX PVCID[%s] Handshake", FPVCID.c_str() );
    if( FSession->isEnabled() == false )
        FSession->logon( );
    FShaking = TRUE;
    for( int i = 0; i< 300; i++ )
    {
        if( FSession->isLoggedOn() == true )        
            return TRUE;
        UFC::SleepMS( 100 );
        if( FShaking == FALSE )
            break;
    }
    UFC::AnsiString ErrMsg;
    
    ErrMsg.Printf( "FIX PVCID[%s] login timeout", FPVCID.c_str() );
    throw( X25RehandshakeException( ErrMsg ));    
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::SetLogManager( UFC::BufferedLog* LogObj ) 
{ 
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::Open( ConnectionParameter& )       
{
    UFC::BufferedLog::Printf( " FIX PVCID[%s] open", FPVCID.c_str() );
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::Close()                                   
{ 
    UFC::BufferedLog::Printf( " FIX PVCID[%s] close", FPVCID.c_str() );
}    
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TWSEFIXConnection::ReceiveMessage( int Sec, BOOL )
{ 
    MessageHeader* Msg = NULL;
    
    Msg = FMessages.Dequeue( Sec );
    if( Msg != NULL ) 
    {
        char Buf[256];
        Msg->AsString(Buf);
        UFC::BufferedLog::Printf( " Get a FIX Order[%s]",Buf );    
    }
    return Msg;
}     
//----------------------------------------------------------------------------------------------------------------------
BOOL TWSEFIXConnection::SendMessage( MessageHeader* Msg, int Timeout )
{    
    char T010[256];
    
    Msg->AsString( T010 );
    UFC::BufferedLog::Printf( " FIX Exec[%s]", T010 );
    switch( Msg->MessageType() )
    {        
        ///< Handle order messages.( New/Cancel/Replace and query order state messages).
        case mtT020: NormalExecution( (MessageT020*)Msg ); return TRUE;
        case mtP020: PostExecution( (MessageP020*) Msg ); return TRUE;
        case mtO020: OddExecution( (MessageO020*)Msg ); return TRUE;
        case mtO120: IntradayOddExecution( (MessageO120*)Msg ); return TRUE;
        case mtA020: ///< FIX not support
        case mtE010: ///< FIX not support
        case mtEx020:///< FIX not support 
        case mtV020: ///< FIX not support
        ///< Unknown essage
        UFC::BufferedLog::Printf( " Not support Message[%s]",T010 );    
        default :    return FALSE;
    }
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::OnLogon( void )
{
    UFC::BufferedLog::Printf( " FIX PVCID[%s] Logon FD=1", FPVCID.c_str() );
    SetSocketFD( 1 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::OnLogoff( void )
{
    UFC::BufferedLog::Printf( " FIX PVCID[%s] Logoff FD=-1", FPVCID.c_str() );
    SetSocketFD( -1 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char AccountFlagFIX2TMP( char FIXFlag )
{
    switch( FIXFlag )
    {
        case '1': return ' ';
        case '2': return 'A';
        case '3': return 'D';
        case '4': return 'I';
        case '5': return 'V';
        case '6': return 'P';
        default:  return FIXFlag;                 
    }
    return FIXFlag;
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::onMessage( const FIX44::NewOrderSingle& NewOrder )
{
   /// TWSE modify FIX SPEC.
   const std::string& OrderNo      = NewOrder.getField( 37 ); ///< OrderID    
   const std::string& InvestorFlag = NewOrder.getField( 10000 ); ///< TwseIvancnoFlaf
   const std::string& OrderType    = NewOrder.getField( 10001 ); ///< TwseOrdType
   const std::string& ExchangeCode = NewOrder.getField( 10002 ); ///< TwseExCode   
   /// Fields for NewOrderSingle
   FIX::TargetSubID TargetSubID;
   FIX::SenderSubID SenderSubID; 
   FIX::Account     Account;
   FIX::Symbol      Symbol; 
   FIX::OrderQty    OrderQty;
   FIX::Price       Price;    
   FIX::Side        Side;
   FIX::ClOrdID     ClOrdID;
   FIX::OrdType     OrdType( FIX::OrdType_LIMIT );  
   FIX::TimeInForce TimeInForce( FIX::TimeInForce_DAY );
   
   UFC::BufferedLog::Printf( " Receive FIX NewOrder OrderID[%s]", OrderNo.c_str() );
   NewOrder.get( ClOrdID );
   NewOrder.get( Account );
   NewOrder.get( Symbol );
   NewOrder.get( OrderQty );      
   NewOrder.get( Price );
   NewOrder.get( Side );      
   if( NewOrder.isSet( OrdType ) )
       NewOrder.get( OrdType );
   if( NewOrder.isSet( TimeInForce ) )
       NewOrder.get( TimeInForce );
   ///< TargetSubID 0: Normal 2:Odd 7:Post
   NewOrder.getHeader().get( TargetSubID );
   ///< SenderSubID: BrokerID
   NewOrder.getHeader().get( SenderSubID );      
   UFC::AnsiString StockID( Symbol.getString().c_str() ); 
   UFC::AnsiString Value;
   
   UFC::BufferedLog::Printf( " Symbol[%s] ClOrdID[%s]", StockID.c_str(), ClOrdID.getString().c_str() );
   StockID.PadThis( 6, ' ' );   
   ///< Record Order ID
   if( OrderNo.length() > 0 )
   {
       ClOrdIDInfo*     Info;
       UFC::PLockObject Lock( FClOrdIDLock );
       if( FClOrdIDMap.GetObjectByKey( OrderNo.c_str(), Info) == false ) 
       {
           Info = new ClOrdIDInfo(); 
           FClOrdIDMap.Add(OrderNo.c_str(), Info);
       }
       Info->OrdClOrdID  = ClOrdID.getString().c_str();
       Info->LastClOrdID = ClOrdID.getString().c_str();           
   }
   ///< Generate order and in-queue.
   if( TargetSubID.getString() == "0" )///<Normal
   {
        MessageT010*    NormalOrder;
        
        if( Side == FIX::Side_BUY )
        {
            NormalOrder = new MessageT010( IsTSEC, "01" );
            NormalOrder->BuySellCode = 'B';
        }
        else
        {
            NormalOrder = new MessageT010( IsTSEC, "02" );
            NormalOrder->BuySellCode = 'S';
        }
        memcpy( NormalOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( NormalOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( NormalOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( NormalOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( NormalOrder->StockID,       StockID.c_str(), 6 );        
        NormalOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        NormalOrder->ExchangeCode  = ExchangeCode[0];
        NormalOrder->TWSEOrderType = OrderType[0];
        NormalOrder->OrderType     = OrdType.getValue();
        NormalOrder->TimeInForce   = TimeInForce.getValue();;
        if( IsOldVersion == true )
        {
            Value.Printf("%03d",(int) OrderQty.getValue() );
            memcpy( NormalOrder->OrderQty,      Value.c_str(), 3 );
            Value.Printf("%06d",(int)((Price.getValue()+0.001)*100.0) );
            memcpy( NormalOrder->OrderPrice,    Value.c_str(), 6 );
        }
        else
        {
            Value.Printf("%06d",(int) OrderQty.getValue() );
            memcpy( NormalOrder->OrderQty,      Value.c_str(), 6 );
            Value.Printf("%09d",(int)((Price.getValue()+0.00001)*10000.0) );
            memcpy( NormalOrder->OrderPrice,    Value.c_str(), 9 );
        }
        UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Normal order[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( NormalOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
   }
   else if( TargetSubID.getString() == "2" )///<Odd
   {
        MessageO010*  OddOrder;

        if( Side == FIX::Side_BUY )
        {
            OddOrder = new MessageO010( IsTSEC, "01" );
            OddOrder->BuySellCode = 'B';
        }
        else
        {
            OddOrder = new MessageO010( IsTSEC, "02" );
            OddOrder->BuySellCode = 'S';
        }
        memcpy( OddOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( OddOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( OddOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( OddOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( OddOrder->StockID,       StockID.c_str(), 6 );
        memcpy( OddOrder->ObjectBroker,  "    ", 4 );        
        OddOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        OddOrder->TradeKind     = ExchangeCode[0];
        OddOrder->TWSEOrderType = OrderType[0];
        OddOrder->OrderType     = OrdType.getValue();
        OddOrder->TimeInForce   = TimeInForce.getValue();;
        if( IsOldVersion == true )
        {            
            Value.Printf("%08d",(int) OrderQty.getValue() );
            memcpy( OddOrder->OrderQty,      Value.c_str(), 3 );
            Value.Printf("%06d",(int)((Price.getValue()+0.001)*100.0) );
            memcpy( OddOrder->OrderPrice,    Value.c_str(), 6 );
        }
        else
        {
            Value.Printf("%06d",(int) OrderQty.getValue() );
            memcpy( OddOrder->OrderQty,      Value.c_str(), 6 );
            Value.Printf("%09d",(int)((Price.getValue()+0.00001)*10000.0) );
            memcpy( OddOrder->OrderPrice,    Value.c_str(), 9 );
        }
        UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Odd order[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( OddOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
   }
   else if( TargetSubID.getString() == "7" )///<Post
   {
        MessageP010*  PostOrder;

        if( Side == FIX::Side_BUY )
        {
            PostOrder = new MessageP010( IsTSEC, "01" );
            PostOrder->BuySellCode = 'B';
        }
        else
        {
            PostOrder = new MessageP010( IsTSEC, "02" );
            PostOrder->BuySellCode = 'S';
        }
        memcpy( PostOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( PostOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( PostOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( PostOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( PostOrder->StockID,       StockID.c_str(), 6 );        
        PostOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        PostOrder->ExchangeCode  = ExchangeCode[0];
        PostOrder->TWSEOrderType = OrderType[0];
        PostOrder->OrderType     = OrdType.getValue();
        PostOrder->TimeInForce   = TimeInForce.getValue();;       
        if( IsOldVersion == true )
        {            
            Value.Printf("%08d",(int) OrderQty.getValue() );
            memcpy( PostOrder->OrderQty,      Value.c_str(), 3 );
            Value.Printf("%06d",(int)((Price.getValue()+0.001)*100.0) );
            memcpy( PostOrder->OrderPrice,    Value.c_str(), 6 );
        }
        else
        {
            Value.Printf("%06d",(int) OrderQty.getValue() );
            memcpy( PostOrder->OrderQty,      Value.c_str(), 6 );
            Value.Printf("%09d",(int)((Price.getValue()+0.00001)*10000.0) );
            memcpy( PostOrder->OrderPrice,    Value.c_str(), 9 );
        }
        UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Post order[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( PostOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
    }
    else if( TargetSubID.getString() == "C" )///<Intrady odd 
    {
        MessageO110*    NormalOrder;
        
        if( Side == FIX::Side_BUY )
        {
            NormalOrder = new MessageO110( IsTSEC, "01" );
            NormalOrder->BuySellCode = 'B';
        }
        else
        {
            NormalOrder = new MessageO110( IsTSEC, "02" );
            NormalOrder->BuySellCode = 'S';
        }
        memcpy( NormalOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( NormalOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( NormalOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( NormalOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( NormalOrder->StockID,       StockID.c_str(), 6 );        
        NormalOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        NormalOrder->ExchangeCode  = ExchangeCode[0];
        NormalOrder->TWSEOrderType = OrderType[0];
        NormalOrder->OrderType     = OrdType.getValue();
        NormalOrder->TimeInForce   = TimeInForce.getValue();;        
        Value.Printf("%06d",(int) OrderQty.getValue() );
        memcpy( NormalOrder->OrderQty,      Value.c_str(), 6 );
        Value.Printf("%09d",(int)((Price.getValue()+0.00001)*10000.0) );
        memcpy( NormalOrder->OrderPrice,    Value.c_str(), 9 );
        
        UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Intra-day Odd order[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( NormalOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
   }
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::onMessage( const FIX44::OrderCancelRequest& CXOrder )
{
    ///< OrderCancelRequest to TMP T010/P010/O010 cancel message         
   const std::string& OrderNo      = CXOrder.getField( 37 ); ///< OrderID    
   const std::string& InvestorFlag = CXOrder.getField( 10000 ); ///< TwseIvancnoFlaf
   const std::string& OrderType("0");//    = CXOrder.getField( 10001 ); ///< TwseOrdType
   const std::string& ExchangeCode = CXOrder.getField( 10002 ); ///< TwseExCode   
   /// Fields for NewOrderSingle
   FIX::TargetSubID TargetSubID;
   FIX::SenderSubID SenderSubID; 
   FIX::Account     Account;
   FIX::Symbol      Symbol; 
   FIX::OrderQty    OrderQty(0);
   FIX::Side        Side;
   FIX::ClOrdID     ClOrdID;  
   FIX::OrdType     OrdType( FIX::OrdType_LIMIT );  
   FIX::TimeInForce TimeInForce( FIX::TimeInForce_DAY );
   
   UFC::BufferedLog::Printf( " Receive FIX CancelOrder OrderID[%s]", OrderNo.c_str() );
   CXOrder.get( ClOrdID );
   CXOrder.get( Account );
   CXOrder.get( Symbol );   
   //CXOrder.get( OrderQty );      
   CXOrder.get( Side );      
   ///< TargetSubID 0: Normal 2:Odd 7:Post
   CXOrder.getHeader().get( TargetSubID );
   ///< SenderSubID: BrokerID
   CXOrder.getHeader().get( SenderSubID );      
   UFC::AnsiString StockID( Symbol.getString().c_str() ); 
   UFC::AnsiString Value;
   
   UFC::BufferedLog::Printf( " Symbol[%s] ClOrdID[%s]", StockID.c_str(), ClOrdID.getString().c_str() );
   StockID.PadThis( 6, ' ' );
   
   if( OrderNo.length() > 0 )
   {
       ClOrdIDInfo*     Info;
       UFC::PLockObject Lock( FClOrdIDLock );
       if( FClOrdIDMap.GetObjectByKey( OrderNo.c_str(), Info) == false ) 
       {
           Info = new ClOrdIDInfo(); 
           FClOrdIDMap.Add(OrderNo.c_str(), Info);
           Info->OrdClOrdID  = ClOrdID.getString().c_str();
       }
       Info->LastClOrdID = ClOrdID.getString().c_str();           
   }
   if( TargetSubID.getString() == "0" )///<Normal
   {
        MessageT010*    NormalOrder;
        
        NormalOrder = new MessageT010( IsTSEC, "04" ); ///< 04->Cancel
        if( Side == FIX::Side_BUY )
            NormalOrder->BuySellCode = 'B';
        else
            NormalOrder->BuySellCode = 'S';
        memcpy( NormalOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( NormalOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( NormalOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( NormalOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( NormalOrder->StockID,       StockID.c_str(), 6 );        
        NormalOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        NormalOrder->ExchangeCode  = ExchangeCode[0];
        NormalOrder->TWSEOrderType = OrderType[0];
        NormalOrder->OrderType     = OrdType.getValue();
        NormalOrder->TimeInForce   = TimeInForce.getValue();;
        if( IsOldVersion == true )
        {
            Value.Printf("%03d",(int) OrderQty.getValue() );
            memcpy( NormalOrder->OrderQty,    Value.c_str(), 3 );            
            memcpy( NormalOrder->OrderPrice,  "000000", 6  );
        }
        else
        {
            Value.Printf("%06d",(int) OrderQty.getValue() );
            memcpy( NormalOrder->OrderQty,      Value.c_str(), 6 );            
            memcpy( NormalOrder->OrderPrice,  "000000000", 9 );
        }
        
        UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Normal CancelOrder[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( NormalOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
   }
   else if( TargetSubID.getString() == "2" )///<Odd
   {
        MessageO010*  OddOrder;

        OddOrder = new MessageO010( IsTSEC, "04" );
        if( Side == FIX::Side_BUY )
            OddOrder->BuySellCode = 'B';
        else
            OddOrder->BuySellCode = 'S';
        memcpy( OddOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( OddOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( OddOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( OddOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( OddOrder->StockID,       StockID.c_str(), 6 );
        memcpy( OddOrder->ObjectBroker,  "    ", 4 );        
        OddOrder->TradeKind = ExchangeCode[0];
        OddOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        OddOrder->ExchangeCode  = ExchangeCode[0];
        OddOrder->TWSEOrderType = OrderType[0];
        OddOrder->OrderType     = OrdType.getValue();
        OddOrder->TimeInForce   = TimeInForce.getValue();
        if( IsOldVersion == true )
        {            
            Value.Printf("%08d",(int) OrderQty.getValue() );
            memcpy( OddOrder->OrderQty,      Value.c_str(), 8 );
            memcpy( OddOrder->OrderPrice,    "000000", 6 );
        }
        else
        {
            Value.Printf("%06d",(int) OrderQty.getValue() );
            memcpy( OddOrder->OrderQty,      Value.c_str(), 6 );
            memcpy( OddOrder->OrderPrice,    "000000000", 9 );
        }
        UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Odd CancelOrder[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( OddOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
   }
   else if( TargetSubID.getString() == "7" )///<Post
   {
        MessageP010*  PostOrder;

        PostOrder = new MessageP010( IsTSEC, "04" );
        if( Side == FIX::Side_BUY )
            PostOrder->BuySellCode = 'B';
        else
            PostOrder->BuySellCode = 'S';
        memcpy( PostOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( PostOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( PostOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( PostOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( PostOrder->StockID,       StockID.c_str(), 6 );        
        PostOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        PostOrder->ExchangeCode  = ExchangeCode[0];
        PostOrder->TWSEOrderType = OrderType[0];
        PostOrder->OrderType     = OrdType.getValue();
        PostOrder->TimeInForce   = TimeInForce.getValue();;        
        if( IsOldVersion == true )
        {            
            Value.Printf("%03d",(int) OrderQty.getValue() );
            memcpy( PostOrder->OrderQty,      Value.c_str(), 3 );
            memcpy( PostOrder->OrderPrice,    "000000", 6 );
        }
        else
        {
            Value.Printf("%06d",(int) OrderQty.getValue() );
            memcpy( PostOrder->OrderQty,      Value.c_str(), 6 );
            memcpy( PostOrder->OrderPrice,    "000000000", 9 );
        }
        UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Post CancelOrder[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( PostOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
    }
    else if( TargetSubID.getString() == "C" )///< Intrady Odd
    {
        MessageO110*    NormalOrder;
        
        NormalOrder = new MessageO110( IsTSEC, "04" ); ///< 04->Cancel
        if( Side == FIX::Side_BUY )
            NormalOrder->BuySellCode = 'B';
        else
            NormalOrder->BuySellCode = 'S';
        memcpy( NormalOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( NormalOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( NormalOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( NormalOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( NormalOrder->StockID,       StockID.c_str(), 6 );        
        NormalOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        NormalOrder->ExchangeCode  = ExchangeCode[0];
        NormalOrder->TWSEOrderType = OrderType[0];
        NormalOrder->OrderType     = OrdType.getValue();
        NormalOrder->TimeInForce   = TimeInForce.getValue();;        
        Value.Printf("%06d",(int) OrderQty.getValue() );
        memcpy( NormalOrder->OrderQty,      Value.c_str(), 6 );            
        memcpy( NormalOrder->OrderPrice,  "000000000", 9 );        
        
        UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Intra-day Odd CancelOrder[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( NormalOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::onMessage( const FIX44::OrderCancelReplaceRequest& RpxOrder )
{
    ///< OrderCancelRequest to TMP T010/P010/O010 cancel message         
   const std::string& OrderNo      = RpxOrder.getField( 37 ); ///< OrderID    
   const std::string& InvestorFlag = RpxOrder.getField( 10000 ); ///< TwseIvancnoFlaf
   const std::string& OrderType("0");//    = CXOrder.getField( 10001 ); ///< TwseOrdType
   const std::string& ExchangeCode = RpxOrder.getField( 10002 ); ///< TwseExCode   
   /// Fields for NewOrderSingle
   FIX::TargetSubID TargetSubID;
   FIX::SenderSubID SenderSubID; 
   FIX::Account     Account;
   FIX::Symbol      Symbol; 
   FIX::Price       Price(0);       
   FIX::OrderQty    OrderQty(0);
   FIX::Side        Side;
   FIX::ClOrdID     ClOrdID;  
   FIX::OrdType     OrdType( FIX::OrdType_LIMIT );  
   FIX::TimeInForce TimeInForce( FIX::TimeInForce_DAY );
   
   UFC::BufferedLog::Printf( " Receive FIX CancelOrder OrderID[%s]", OrderNo.c_str() );
   RpxOrder.get( ClOrdID );
   RpxOrder.get( Account );
   RpxOrder.get( Symbol );   
   RpxOrder.get( OrderQty );         
   RpxOrder.get( Side );      
   ///< TargetSubID 0: Normal 2:Odd 7:Post
   RpxOrder.getHeader().get( TargetSubID );
   ///< SenderSubID: BrokerID
   RpxOrder.getHeader().get( SenderSubID );      
   UFC::AnsiString StockID( Symbol.getString().c_str() ); 
   int Qty = (int) OrderQty.getValue();
   double Px;
   UFC::AnsiString Value;
   
   UFC::BufferedLog::Printf( " Symbol[%s] ClOrdID[%s]", StockID.c_str(), ClOrdID.getString().c_str() );
   StockID.PadThis( 6, ' ' );
   
   if( OrderNo.length() > 0 )
   {
       ClOrdIDInfo*     Info;
       UFC::PLockObject Lock( FClOrdIDLock );
       if( FClOrdIDMap.GetObjectByKey( OrderNo.c_str(), Info) == false ) 
       {
           Info = new ClOrdIDInfo(); 
           FClOrdIDMap.Add(OrderNo.c_str(), Info);
           Info->OrdClOrdID  = ClOrdID.getString().c_str();
       }
       Info->LastClOrdID = ClOrdID.getString().c_str();           
   }
   if( TargetSubID.getString() == "0" )///<Normal
   {
        MessageT010*    NormalOrder;
        
        RpxOrder.get( Price );           
        Px = Price.getValue();
        if( Qty == 0 ) ///< Replace Price            
            NormalOrder = new MessageT010( IsTSEC, "06" ); ///< 06->Replace Price
        else
            NormalOrder = new MessageT010( IsTSEC, "03" ); ///< 03->Replace Quantity
        if( Side == FIX::Side_BUY )
            NormalOrder->BuySellCode = 'B';
        else
            NormalOrder->BuySellCode = 'S';
        memcpy( NormalOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( NormalOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( NormalOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( NormalOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( NormalOrder->StockID,       StockID.c_str(), 6 );        
        NormalOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        NormalOrder->ExchangeCode  = ExchangeCode[0];
        NormalOrder->TWSEOrderType = OrderType[0];
        NormalOrder->OrderType     = OrdType.getValue();
        NormalOrder->TimeInForce   = TimeInForce.getValue();;
        Value.Printf("%06d",Qty );
        memcpy( NormalOrder->OrderQty,    Value.c_str(), 6 );            
        Value.Printf("%09d",(int)((Px+0.00001)*10000.0) );
        memcpy( NormalOrder->OrderPrice,  Value.c_str(), 9 );                
        if( Qty == 0 )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Normal ReplacePx[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        else
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Normal ReplaceQty[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( NormalOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
   }
   else if( TargetSubID.getString() == "2" )///<Odd
   {
        MessageO010*  OddOrder;

        OddOrder = new MessageO010( IsTSEC, "03" ); ///< 03->Replace Quantity
        if( Side == FIX::Side_BUY )
            OddOrder->BuySellCode = 'B';
        else
            OddOrder->BuySellCode = 'S';
        memcpy( OddOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( OddOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( OddOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( OddOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( OddOrder->StockID,       StockID.c_str(), 6 );
        memcpy( OddOrder->ObjectBroker,  "    ", 4 );        
        OddOrder->TradeKind = ExchangeCode[0];
        OddOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        OddOrder->ExchangeCode  = ExchangeCode[0];
        OddOrder->TWSEOrderType = OrderType[0];
        OddOrder->OrderType     = OrdType.getValue();
        OddOrder->TimeInForce   = TimeInForce.getValue();
        Value.Printf("%06d",(int) OrderQty.getValue() );
        memcpy( OddOrder->OrderQty,      Value.c_str(), 6 );
        memcpy( OddOrder->OrderPrice,    "000000000", 9 );
        UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Odd ReplaceQty[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( OddOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
   }
   else if( TargetSubID.getString() == "7" )///<Post
   {
        MessageP010*  PostOrder;

        PostOrder = new MessageP010( IsTSEC, "03" ); ///< 03->Replace Quantity
        if( Side == FIX::Side_BUY )
            PostOrder->BuySellCode = 'B';
        else
            PostOrder->BuySellCode = 'S';
        memcpy( PostOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( PostOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( PostOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( PostOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( PostOrder->StockID,       StockID.c_str(), 6 );        
        PostOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        PostOrder->ExchangeCode  = ExchangeCode[0];
        PostOrder->TWSEOrderType = OrderType[0];
        PostOrder->OrderType     = OrdType.getValue();
        PostOrder->TimeInForce   = TimeInForce.getValue();;                
        Value.Printf("%06d",(int) OrderQty.getValue() );
        memcpy( PostOrder->OrderQty,      Value.c_str(), 6 );
        memcpy( PostOrder->OrderPrice,    "000000000", 9 );        
        UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Post ReplaceQty[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( PostOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
    }
    else if( TargetSubID.getString() == "C" )///< Intra-day odd
    {
        MessageO110*    NormalOrder;        
        
        NormalOrder = new MessageO110( IsTSEC, "03" ); ///< 03->Replace Quantity
        if( Side == FIX::Side_BUY )
            NormalOrder->BuySellCode = 'B';
        else
            NormalOrder->BuySellCode = 'S';
        memcpy( NormalOrder->BrokerID,      SenderSubID.getString().c_str(), 4 );
        memcpy( NormalOrder->PVCID,         FPVCID.c_str(), 2 );
        memcpy( NormalOrder->OrderNo,       OrderNo.c_str(), 5 );
        memcpy( NormalOrder->InvestorAcno,  Account.getString().c_str(), 7 );
        memcpy( NormalOrder->StockID,       StockID.c_str(), 6 );        
        NormalOrder->InvestorFlag  = AccountFlagFIX2TMP( InvestorFlag[0] );
        NormalOrder->ExchangeCode  = ExchangeCode[0];
        NormalOrder->TWSEOrderType = OrderType[0];
        NormalOrder->OrderType     = OrdType.getValue();
        NormalOrder->TimeInForce   = TimeInForce.getValue();;
        Value.Printf("%06d",Qty );
        memcpy( NormalOrder->OrderQty,    Value.c_str(), 6 );            
        memcpy( NormalOrder->OrderPrice,    "000000000", 9 );                
        UFC::BufferedLog::Printf( " FIX PVCID[%s] Add Intra-day odd ReplaceQty[%s] to queue", FPVCID.c_str(), OrderNo.c_str() );
        if( FMessages.Inqueue( NormalOrder ) == FALSE )
            UFC::BufferedLog::Printf( " FIX PVCID[%s] Add[%s] to queue failed, queue full!", FPVCID.c_str(), OrderNo.c_str() );
    }   
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::onMessage( const FIX44::OrderStatusRequest& )
{
    ///< To do 
    ///< OrderStatusRequest to TMP T010/P010/O010 StatusRequest message             
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char TWSEFIXConnection::AccountFlagTMP2FIX( char TMPFlag )
{
    switch( TMPFlag )
    {
        case ' ': return '1';
        case 'A': return '2';
        case 'D': return '3';
        case 'I': return '4';
        case 'V': return '5';
        case 'P': return '6';
        default:  return '1';                 
    }
    return TMPFlag;
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::NormalExecution( MessageT020* Msg )
{
    UFC::AnsiString    OrderNO( Msg->OrderNo, 5 );
    UFC::AnsiString    Symbol( Msg->StockID, 6 );
    UFC::AnsiString    Account( Msg->InvestorAcno, 7 );
    UFC::AnsiString    TwseAccFlag( AccountFlagTMP2FIX( Msg->InvestorFlag ));
    UFC::AnsiString    TwseOrdType( Msg->TWSEOrderType );
    UFC::AnsiString    ClOrdID;
    UFC::AnsiString    ErrText;
    FIX::TargetSubID   TargetSubID( UFC::AnsiString( Msg->BrokerID, 4 ).c_str() );
    int                OrderQty;
    int                AfterQty;
    FIX::ExecID        ExecID;  //?
    FIX::ExecType      ExecType; //OK
    FIX::OrdStatus     OrdStatus;//OK 
    FIX::Side          Side;//OK
    FIX::TransactTime  TransactTime;//OK    
    int                Status = Msg->GetStatusCode();
        
    
    if( IsOldVersion == true )
    {
        OrderQty  = UFC::AnsiString::StrToInt32( Msg->OrderQty, 3 );
        AfterQty  = UFC::AnsiString::StrToInt32( Msg->AfterQty, 3 );
    }
    else
    {
        OrderQty  = UFC::AnsiString::StrToInt32( Msg->OrderQty, 6 );
        AfterQty  = UFC::AnsiString::StrToInt32( Msg->AfterQty, 6 );
    }
    if( Status == 0 )
    {    
        switch( Msg->GetFunctionCode() )
        {
            case 1: 
            case 2: OrdStatus = FIX::OrdStatus_NEW;
                    ExecType  = FIX::ExecType_NEW;
                    break;
            case 3:
            case 6: OrdStatus = FIX::OrdStatus_REPLACED;
                    ExecType  = FIX::ExecType_REPLACE;
                    break;
            case 4: OrdStatus = FIX::OrdStatus_CANCELED;
                    ExecType  = FIX::ExecType_CANCELED;
                    break;
            case 5: if( AfterQty == 0 )
                        OrdStatus = FIX::OrdStatus_CANCELED;
                    else
                        OrdStatus = FIX::OrdStatus_NEW;
                    ExecType  = FIX::ExecType_ORDER_STATUS;
                    break;
            default:
                    UFC::BufferedLog::Printf( " Wrong FunctionCode[%d]", Msg->GetFunctionCode() );
                    return; 
        }
    }
    else
    {
        OrdStatus = FIX::OrdStatus_REJECTED;
        ExecType  = FIX::ExecType_REJECTED;
        switch( Status )
        {
            case 20: ErrText = "0020-STOCK NO ERROR";break;
            case 21: ErrText = "0021-PRICE ERROR";break;
            case 5: ErrText = "0018-ORDER NOT FOND";break;            
        }
    }
    if( OrderNO.Length() > 0 )
    {
       ClOrdIDInfo*     Info;
       UFC::PLockObject Lock( FClOrdIDLock );
       
       if( FClOrdIDMap.GetObjectByKey( OrderNO, Info) == true )        
       {
           if( OrdStatus == FIX::OrdStatus_NEW )
               ClOrdID = Info->OrdClOrdID;
           else
               ClOrdID = Info->LastClOrdID;
       }
       else
           ClOrdID = "ID1234567890";       
    }
    if( Msg->BuySellCode == 'B' )
        Side = FIX::Side_BUY;
    else
        Side = FIX::Side_SELL;
    FIX44::ExecutionReport report( FIX::OrderID( OrderNO.c_str() ), /// OK
                                   ExecID, /// OK
                                   ExecType, /// OK
                                   OrdStatus, /// OK
                                   Side, /// OK
                                   FIX::LeavesQty( AfterQty ), /// OK
                                   FIX::CumQty( 0 ),  /// OK
                                   FIX::AvgPx( 0 ) ); /// OK 
    report.getHeader().set( FIX::SenderSubID( "0" ) );///< "0": Normal "2": Odd "7":Post
    report.getHeader().set( TargetSubID );///< BrokerID
    report.set( FIX::Symbol( Symbol.c_str() ) ); /// OK
    report.set( FIX::ClOrdID( ClOrdID.c_str() ) );
    report.set( FIX::ExecID( ClOrdID.c_str() ) );
    report.set( FIX::Account( Account.c_str() ) ); 
    report.set( TransactTime ); /// OK
    report.set( FIX::TimeInForce( Msg->TimeInForce));/// OK
    report.set( FIX::OrdType( Msg->OrderType ) );/// OK    
    if( IsOldVersion == true )
        report.set( FIX::Price( (double)UFC::AnsiString::StrToInt32( Msg->OrderPrice, 6 ) / 100.0 ) );
    else
        report.set( FIX::Price( (double)UFC::AnsiString::StrToInt32( Msg->OrderPrice, 9 ) / 10000.0 ) );
    report.set( FIX::OrderQty( OrderQty ) );/// OK
    report.set( FIX::LastQty( 0 ) );  /// OK
    report.set( FIX::LastPx( 0.0 ) ); /// OK
    if( Status != 0 )
        report.set( FIX::Text( ErrText.c_str() ) );
    report.setField( 10000, TwseAccFlag.c_str() );    /// OK
    report.setField( 10001, TwseOrdType.c_str() );    /// OK
    report.setField( 10002, "0" ); /// OK
    UFC::BufferedLog::Printf( " FIX PVCID[%s] Exec[%s]", FPVCID.c_str(), report.toString().c_str() );
    FSession->send( report );
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::OddExecution( MessageO020* Msg )
{
    UFC::AnsiString    OrderNO( Msg->OrderNo, 5 );
    UFC::AnsiString    Symbol( Msg->StockID, 6 );
    UFC::AnsiString    Account( Msg->InvestorAcno, 7 );
    UFC::AnsiString    TwseAccFlag( AccountFlagTMP2FIX( Msg->InvestorFlag ));
    UFC::AnsiString    TwseOrdType( Msg->TWSEOrderType );
    UFC::AnsiString    ClOrdID;
    UFC::AnsiString    ErrText;
    FIX::TargetSubID   TargetSubID( UFC::AnsiString( Msg->BrokerID, 4 ).c_str() );
    int                OrderQty;
    int                AfterQty;
    FIX::ExecID        ExecID;  //?
    FIX::ExecType      ExecType; //OK
    FIX::OrdStatus     OrdStatus;//OK 
    FIX::Side          Side;//OK
    FIX::TransactTime  TransactTime;//OK
    int                Status = Msg->GetStatusCode();    
        
    
    if( IsOldVersion == true )
    {
        OrderQty  = UFC::AnsiString::StrToInt32( Msg->OrderQty, 8 );
        AfterQty  = UFC::AnsiString::StrToInt32( Msg->AfterQty, 8 );
    }
    else
    {
        OrderQty  = UFC::AnsiString::StrToInt32( Msg->OrderQty, 6 );
        AfterQty  = UFC::AnsiString::StrToInt32( Msg->AfterQty, 6 );
    }
    if( Status == 0 )
    {    
        switch( Msg->GetFunctionCode() )
        {
            case 1: 
            case 2: OrdStatus = FIX::OrdStatus_NEW;
                    ExecType  = FIX::ExecType_NEW;
                    break;
            case 3:
            case 6: OrdStatus = FIX::OrdStatus_REPLACED;
                    ExecType  = FIX::ExecType_REPLACE;
                    break;
            case 4: OrdStatus = FIX::OrdStatus_CANCELED;
                    ExecType  = FIX::ExecType_CANCELED;
                    break;
            case 5: if( AfterQty == 0 )
                        OrdStatus = FIX::OrdStatus_CANCELED;
                    else
                        OrdStatus = FIX::OrdStatus_NEW;
                    ExecType  = FIX::ExecType_ORDER_STATUS;
                    break;
            default:
                    UFC::BufferedLog::Printf( " Wrong FunctionCode[%d]", Msg->GetFunctionCode() );
                    return; 
        }
    }
    else
    {
        OrdStatus = FIX::OrdStatus_REJECTED;
        ExecType  = FIX::ExecType_REJECTED;
        switch( Status )
        {
            case 20: ErrText = "2020-STOCK NO ERROR";break;
            case 21: ErrText = "2021-PRICE ERROR";break;
            case 24: ErrText = "2024-ORDER NOT FOND";break;            
        }
    }
    if( OrderNO.Length() > 0 )
    {
       ClOrdIDInfo*     Info;
       UFC::PLockObject Lock( FClOrdIDLock );
       
       if( FClOrdIDMap.GetObjectByKey( OrderNO, Info) == true )        
       {
           if( OrdStatus == FIX::OrdStatus_NEW )
               ClOrdID = Info->OrdClOrdID;
           else
               ClOrdID = Info->LastClOrdID;
       }
       else
           ClOrdID = "ID1234567890";       
    }
    if( Msg->BuySellCode == 'B' )
        Side = FIX::Side_BUY;
    else
        Side = FIX::Side_SELL;
    FIX44::ExecutionReport report( FIX::OrderID( OrderNO.c_str() ), /// OK
                                   ExecID, /// OK
                                   ExecType, /// OK
                                   OrdStatus, /// OK
                                   Side, /// OK
                                   FIX::LeavesQty( AfterQty ), /// OK
                                   FIX::CumQty( 0 ),  /// OK
                                   FIX::AvgPx( 0 ) ); /// OK 
    report.getHeader().set( FIX::SenderSubID( "2" ) );///< "0": Normal "2": Odd "7":Post
    report.getHeader().set( TargetSubID );///< BrokerID
    report.set( FIX::Symbol( Symbol.c_str() ) ); /// OK
    report.set( FIX::ClOrdID( ClOrdID.c_str() ) );
    report.set( FIX::ExecID( ClOrdID.c_str() ) );
    report.set( FIX::Account( Account.c_str() ) ); 
    report.set( TransactTime ); /// OK
    report.set( FIX::TimeInForce( Msg->TimeInForce));/// OK
    report.set( FIX::OrdType( Msg->OrderType ) );/// OK    
    if( IsOldVersion == true )
        report.set( FIX::Price( (double)UFC::AnsiString::StrToInt32( Msg->OrderPrice, 6 ) / 100.0 ) );
    else
        report.set( FIX::Price( (double)UFC::AnsiString::StrToInt32( Msg->OrderPrice, 9 ) / 10000.0 ) );
    report.set( FIX::OrderQty( OrderQty ) );/// OK
    report.set( FIX::LastQty( 0 ) );  /// OK
    report.set( FIX::LastPx( 0.0 ) ); /// OK
    if( Status != 0 )
        report.set( FIX::Text( ErrText.c_str() ) );
    report.setField( 10000, "1" );    /// OK
    report.setField( 10001, "0" );    /// OK
    report.setField( 10002, "2" ); /// OK
    UFC::BufferedLog::Printf( " FIX PVCID[%s] Exec[%s]", FPVCID.c_str(), report.toString().c_str() );
    FSession->send( report );
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::PostExecution( MessageP020* Msg )
{
    UFC::AnsiString    OrderNO( Msg->OrderNo, 5 );
    UFC::AnsiString    Symbol( Msg->StockID, 6 );
    UFC::AnsiString    Account( Msg->InvestorAcno, 7 );
    UFC::AnsiString    TwseAccFlag( AccountFlagTMP2FIX( Msg->InvestorFlag ));
    UFC::AnsiString    TwseOrdType( Msg->TWSEOrderType );
    UFC::AnsiString    ClOrdID;
    UFC::AnsiString    ErrText;
    FIX::TargetSubID   TargetSubID( UFC::AnsiString( Msg->BrokerID, 4 ).c_str() );
    int                OrderQty;
    int                AfterQty;
    FIX::ExecID        ExecID;  //?
    FIX::ExecType      ExecType; //OK
    FIX::OrdStatus     OrdStatus;//OK 
    FIX::Side          Side;//OK
    FIX::TransactTime  TransactTime;//OK    
    int                Status = Msg->GetStatusCode();
        
    if( IsOldVersion == true )
    {
        OrderQty  = UFC::AnsiString::StrToInt32( Msg->OrderQty, 3 );
        AfterQty  = UFC::AnsiString::StrToInt32( Msg->AfterQty, 3 );
    }
    else
    {
        OrderQty  = UFC::AnsiString::StrToInt32( Msg->OrderQty, 6 );
        AfterQty  = UFC::AnsiString::StrToInt32( Msg->AfterQty, 6 );
    }
    if( Status == 0 )
    {    
        switch( Msg->GetFunctionCode() )
        {
            case 1: 
            case 2: OrdStatus = FIX::OrdStatus_NEW;
                    ExecType  = FIX::ExecType_NEW;
                    break;
            case 3:
            case 6: OrdStatus = FIX::OrdStatus_REPLACED;
                    ExecType  = FIX::ExecType_REPLACE;
                    break;
            case 4: OrdStatus = FIX::OrdStatus_CANCELED;
                    ExecType  = FIX::ExecType_CANCELED;
                    break;
            case 5: if( AfterQty == 0 )
                        OrdStatus = FIX::OrdStatus_CANCELED;
                    else
                        OrdStatus = FIX::OrdStatus_NEW;
                    ExecType  = FIX::ExecType_ORDER_STATUS;
                    break;
            default:
                    UFC::BufferedLog::Printf( " Wrong FunctionCode[%d]", Msg->GetFunctionCode() );
                    return; 
        }
    }
    else
    {
        OrdStatus = FIX::OrdStatus_REJECTED;
        ExecType  = FIX::ExecType_REJECTED;
        switch( Status )
        {
            case 20: ErrText = "7020-STOCK NO ERROR";break;
            case 21: ErrText = "7021-PRICE ERROR";break;
            case 24: ErrText = "7024-ORDER NOT FOND";break;            
        }
    }
    if( OrderNO.Length() > 0 )
    {
       ClOrdIDInfo*     Info;
       UFC::PLockObject Lock( FClOrdIDLock );
       
       if( FClOrdIDMap.GetObjectByKey( OrderNO, Info) == true )        
       {
           if( OrdStatus == FIX::OrdStatus_NEW )
               ClOrdID = Info->OrdClOrdID;
           else
               ClOrdID = Info->LastClOrdID;
       }
       else
           ClOrdID = "ID1234567890";       
    }
    if( Msg->BuySellCode == 'B' )
        Side = FIX::Side_BUY;
    else
        Side = FIX::Side_SELL;
    FIX44::ExecutionReport report( FIX::OrderID( OrderNO.c_str() ), /// OK
                                   ExecID, /// OK
                                   ExecType, /// OK
                                   OrdStatus, /// OK
                                   Side, /// OK
                                   FIX::LeavesQty( AfterQty ), /// OK
                                   FIX::CumQty( 0 ),  /// OK
                                   FIX::AvgPx( 0 ) ); /// OK 
    report.getHeader().set( FIX::SenderSubID( "7" ) );///< "0": Normal "2": Odd "7":Post
    report.getHeader().set( TargetSubID );///< BrokerID
    report.set( FIX::Symbol( Symbol.c_str() ) ); /// OK
    report.set( FIX::ClOrdID( ClOrdID.c_str() ) );
    report.set( FIX::ExecID( ClOrdID.c_str() ) );
    report.set( FIX::Account( Account.c_str() ) ); 
    report.set( TransactTime ); /// OK
    report.set( FIX::TimeInForce( Msg->TimeInForce));/// OK
    report.set( FIX::OrdType( Msg->OrderType ) );/// OK    
    if( IsOldVersion == true )
        report.set( FIX::Price( (double)UFC::AnsiString::StrToInt32( Msg->OrderPrice, 6 ) / 100.0 ) );
    else
        report.set( FIX::Price( (double)UFC::AnsiString::StrToInt32( Msg->OrderPrice, 9 ) / 10000.0 ) );
    report.set( FIX::OrderQty( OrderQty ) );/// OK
    report.set( FIX::LastQty( 0 ) );  /// OK
    report.set( FIX::LastPx( 0.0 ) ); /// OK
    if( Status != 0 )
        report.set( FIX::Text( ErrText.c_str() ) );
    report.setField( 10000, TwseAccFlag.c_str() );    /// OK
    report.setField( 10001, TwseOrdType.c_str() );    /// OK
    report.setField( 10002, "0" ); /// OK
    UFC::BufferedLog::Printf( " FIX PVCID[%s] Exec[%s]", FPVCID.c_str(), report.toString().c_str() );
    FSession->send( report );
}
//----------------------------------------------------------------------------------------------------------------------
void TWSEFIXConnection::IntradayOddExecution( MessageO120* Msg )
{
    UFC::AnsiString    OrderNO( Msg->OrderNo, 5 );
    UFC::AnsiString    Symbol( Msg->StockID, 6 );
    UFC::AnsiString    Account( Msg->InvestorAcno, 7 );
    UFC::AnsiString    TwseAccFlag( AccountFlagTMP2FIX( Msg->InvestorFlag ));
    UFC::AnsiString    TwseOrdType( Msg->TWSEOrderType );
    UFC::AnsiString    ClOrdID;
    UFC::AnsiString    ErrText;
    FIX::TargetSubID   TargetSubID( UFC::AnsiString( Msg->BrokerID, 4 ).c_str() );
    int                OrderQty;
    int                AfterQty;
    FIX::ExecID        ExecID;  //?
    FIX::ExecType      ExecType; //OK
    FIX::OrdStatus     OrdStatus;//OK 
    FIX::Side          Side;//OK
    FIX::TransactTime  TransactTime;//OK    
    int                Status = Msg->GetStatusCode();
        
    
    OrderQty  = UFC::AnsiString::StrToInt32( Msg->OrderQty, 6 );
    AfterQty  = UFC::AnsiString::StrToInt32( Msg->AfterQty, 6 );
    if( Status == 0 )
    {    
        switch( Msg->GetFunctionCode() )
        {
            case 1: 
            case 2: OrdStatus = FIX::OrdStatus_NEW;
                    ExecType  = FIX::ExecType_NEW;
                    break;
            case 3: OrdStatus = FIX::OrdStatus_REPLACED;
                    ExecType  = FIX::ExecType_REPLACE;
                    break;
            case 4: OrdStatus = FIX::OrdStatus_CANCELED;
                    ExecType  = FIX::ExecType_CANCELED;
                    break;
            case 5: if( AfterQty == 0 )
                        OrdStatus = FIX::OrdStatus_CANCELED;
                    else
                        OrdStatus = FIX::OrdStatus_NEW;
                    ExecType  = FIX::ExecType_ORDER_STATUS;
                    break;
            default:
                    UFC::BufferedLog::Printf( " Wrong FunctionCode[%d]", Msg->GetFunctionCode() );
                    return; 
        }
    }
    else
    {
        OrdStatus = FIX::OrdStatus_REJECTED;
        ExecType  = FIX::ExecType_REJECTED;
        switch( Status )
        {
            case 20: ErrText = "C020-STOCK NO ERROR";break;
            case 21: ErrText = "C021-PRICE ERROR";break;
            case 24: ErrText = "C024-ORDER NOT FOND";break;            
        }
    }
    if( OrderNO.Length() > 0 )
    {
       ClOrdIDInfo*     Info;
       UFC::PLockObject Lock( FClOrdIDLock );
       
       if( FClOrdIDMap.GetObjectByKey( OrderNO, Info) == true )        
       {
           if( OrdStatus == FIX::OrdStatus_NEW )
               ClOrdID = Info->OrdClOrdID;
           else
               ClOrdID = Info->LastClOrdID;
       }
       else
           ClOrdID = "ID1234567890";       
    }
    if( Msg->BuySellCode == 'B' )
        Side = FIX::Side_BUY;
    else
        Side = FIX::Side_SELL;
    FIX44::ExecutionReport report( FIX::OrderID( OrderNO.c_str() ), /// OK
                                   ExecID, /// OK
                                   ExecType, /// OK
                                   OrdStatus, /// OK
                                   Side, /// OK
                                   FIX::LeavesQty( AfterQty ), /// OK
                                   FIX::CumQty( 0 ),  /// OK
                                   FIX::AvgPx( 0 ) ); /// OK 
    report.getHeader().set( FIX::SenderSubID( "C" ) );///< "0": Normal "2": Odd "7":Post "C": Intra-day Odd
    report.getHeader().set( TargetSubID );///< BrokerID
    report.set( FIX::Symbol( Symbol.c_str() ) ); /// OK
    report.set( FIX::ClOrdID( ClOrdID.c_str() ) );
    report.set( FIX::ExecID( ClOrdID.c_str() ) );
    report.set( FIX::Account( Account.c_str() ) ); 
    report.set( TransactTime ); /// OK
    report.set( FIX::TimeInForce( Msg->TimeInForce));/// OK
    report.set( FIX::OrdType( Msg->OrderType ) );/// OK        
    report.set( FIX::Price( (double)UFC::AnsiString::StrToInt32( Msg->OrderPrice, 9 ) / 10000.0 ) );
    report.set( FIX::OrderQty( OrderQty ) );/// OK
    report.set( FIX::LastQty( 0 ) );  /// OK
    report.set( FIX::LastPx( 0.0 ) ); /// OK
    if( Status != 0 )
        report.set( FIX::Text( ErrText.c_str() ) );
    report.setField( 10000, TwseAccFlag.c_str() );    /// OK
    report.setField( 10001, "0" );    /// OK
    report.setField( 10002, "2" ); /// OK
    UFC::BufferedLog::Printf( " FIX PVCID[%s] Exec[%s]", FPVCID.c_str(), report.toString().c_str() );
    FSession->send( report );
}
//-----------------------------------------------------------------------------------------
void TWSEFIXConnection::SendFIXFill( R3Body* Msg, OrderInfo* MatchOrder,BOOL Filled )
{
    Msg->print();
    UFC::AnsiString    OrderNO( Msg->FOrderNo, 5 );
    UFC::AnsiString    Symbol( Msg->FStockID,6 );
    UFC::AnsiString    Account( Msg->FInvestorACNo, 7 );
    UFC::AnsiString    TwseAccFlag( AccountFlagTMP2FIX( MatchOrder->FInvestorFlag));
    UFC::AnsiString    TwseOrdType( MatchOrder->FOrderType );
    UFC::AnsiString    ClOrdID,ExecID;
    UFC::AnsiString    ExchangeCode( Msg->FEXCD );
    FIX::TargetSubID   TargetSubID( UFC::AnsiString( Msg->FBroker, 4 ).c_str()  );
    int                OrderQty  = UFC::AnsiString::StrToInt32( Msg->FQuntity, 8 );
    int                LeavesQty = MatchOrder->FQty - MatchOrder->FReduceQty - MatchOrder->FFillQty;    
    int                Seq       = UFC::AnsiString::StrToInt32( Msg->FSeq,6 );
    int                RecNo     = UFC::AnsiString::StrToInt32( Msg->FRecNo,8 );
    double             LastPx; 
    FIX::ExecType      ExecType  = FIX::ExecType_TRADE; //OK
    FIX::OrdStatus     OrdStatus;//OK 
    FIX::Side          Side;//OK
    FIX::TransactTime  TransactTime;//OK    
        

    if( IsOldVersion == true )
        LastPx    = (double)UFC::AnsiString::StrToInt32( Msg->FPrice, 6 )/100.0; 
    else
        LastPx    = (double)UFC::AnsiString::StrToInt32( Msg->FPrice, 9 )/10000.0; 
    
    ExecID.Printf("%04d%08d",Seq%10000,RecNo%100000000);
    if( Filled == TRUE )
        OrdStatus = FIX::OrdStatus_FILLED;
    else
        OrdStatus = FIX::OrdStatus_PARTIALLY_FILLED;
    if( OrderNO.Length() > 0 )
    {
       ClOrdIDInfo*     Info;
       UFC::PLockObject Lock( FClOrdIDLock );
       
       if( FClOrdIDMap.GetObjectByKey( OrderNO, Info) == true )        
           ClOrdID = Info->LastClOrdID;
       else
           ClOrdID = "ID1234567890";       
    }
    if( Msg->FBuySellCode == 'B' )
        Side = FIX::Side_BUY;
    else
        Side = FIX::Side_SELL;
    FIX44::ExecutionReport report( FIX::OrderID( OrderNO.c_str() ), ///< 37
                                   FIX::ExecID(ExecID.c_str()), ///< 17
                                   ExecType, ///< 150
                                   OrdStatus, ///< 39  
                                   Side, ///< 54
                                   FIX::LeavesQty( LeavesQty ), ///< 151 
                                   FIX::CumQty( MatchOrder->FFillQty ), ///< 14
                                   FIX::AvgPx( 0 ) ); ///< 6
    report.getHeader().set( FIX::SenderSubID( MatchOrder->GetFIXSenderSubID() ) );///< "0": Normal "2": Odd "7":Post "C" Intraday Odd
    report.getHeader().set( TargetSubID );///< BrokerID 57
    report.set( FIX::Symbol( Symbol.c_str() ));  ///< 55
    report.set( FIX::ClOrdID( ClOrdID.c_str() ));///< 11
    report.set( FIX::Account( Account.c_str() ) ); ///< 1
    report.set( TransactTime ); ///< 60
    report.set( FIX::TimeInForce( MatchOrder->FTimeInforce));/// 59
    report.set( FIX::OrdType( MatchOrder->FOrdType ) );/// 40
    report.set( FIX::OrderQty( OrderQty ) ); ///< 38
    report.set( FIX::Price( LastPx ) ); ///< 44
    report.set( FIX::LastPx( LastPx ) ); ///< 31
    report.set( FIX::LastQty( OrderQty ) ); ///< 32     
    report.setField( 10000, TwseAccFlag.c_str() ); ///< TWSE no this field.   
    report.setField( 10001, TwseOrdType.c_str() );    
    report.setField( 10002, ExchangeCode.c_str() );    
    UFC::BufferedLog::Printf( " FIX PVCID[%s] Exec[%s]", FPVCID.c_str(), report.toString().c_str() );
    FSession->send( report );
}
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------

