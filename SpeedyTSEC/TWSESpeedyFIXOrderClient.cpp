
#include "TWSESpeedyOrderClient.h"
#include "TWSESpeedyObjectsManager.h"
#include "TWSESpeedyFIXManager.h"
#include "ThreadObjectPool.h"
#include "Profiler.h"
#include "../TSECLib/LinkSubSystem.h"
#include "../TSECLib/Connection.h"
#include "../TSECLib/ConnectionObject.h"
#include "../SpeedyGateway/Administrator.h"
#include "../TSECLib/TWSERMClass.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern MessageObject*         MessageObj;
extern BOOL                   FIXRejStaleOrd;
extern BOOL                   ExecToQueue;
extern BOOL                   NotSpeedyToOthers;
extern BOOL                   ExtremeVersion;
extern BOOL                   BENCHMARK_MODE;
extern BOOL                   IsSMPEnable;
extern BOOL                   SMP_FIllED_POS;
extern BOOL                   EnableRPXText;
//------------------------------------------------------------------------------------------------------------------
extern TWSE::ExecutionListener* gTSEFilled;
extern TWSE::ExecutionListener* gOTCFilled;
extern UFC::PThreadObjectPool<OrderRecord>* OrderRecodPoolPtr;
//-----------------------------------------------------------------------------------------
#ifdef __EXTREME_VERSION   
#include "../SpeedyTWPro/OrderServer.h"
extern SMPServer*             FSMPGateway;
#endif
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString FIXTMPFlagMappingFile( "../cfg/TWSEInvestorFlag.cfg" ); 
BOOL TWSEOrder::IsInit = FALSE;
char TWSEOrder::TMP2FIX[ 256 ];
char TWSEOrder::FIX2TMP[ 256 ];
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ 
void TWSEOrder::Init( void )
{
    if( IsInit == FALSE )
    {
        IsInit = TRUE;
        UFC::BufferedLog::Printf( " Load TMP to FIX investor flag mapping file. ");
        for( int i = 0; i < 256; i ++ )
        {
            TMP2FIX[ i ] = '1';
            FIX2TMP[ i ] = ' ';
        }
        ///< Convert TMP accout flag to FIX 
        TMP2FIX[ ' ' ] = '1';
        TMP2FIX[ 'A' ] = '2';
        TMP2FIX[ 'D' ] = '3';
        TMP2FIX[ 'I' ] = '4';
        TMP2FIX[ 'V' ] = '5';
        TMP2FIX[ 'P' ] = '6';
        ///< Convert FIX accout flag to TMP
        FIX2TMP[ '1' ] = ' ';
        FIX2TMP[ '2' ] = 'A';
        FIX2TMP[ '3' ] = 'D';
        FIX2TMP[ '4' ] = 'I';
        FIX2TMP[ '5' ] = 'V';
        FIX2TMP[ '6' ] = 'P';
        
        if( UFC::FileExists( FIXTMPFlagMappingFile ) == TRUE )
        {
            UFC::AnsiString TMPFlag,FIXFlag;    
            UFC::UiniFile   FlagMap( FIXTMPFlagMappingFile );                        
            UFC::Section*   T2FSec = FlagMap.GetSection( "TMP2FIX" );
            
            if( T2FSec != NULL ) ///< [TMP2FIX] exists
            {
                for( int i = 0; i < T2FSec->ItemCount(); i ++ )
                {
                   T2FSec->GetNameValue( i, TMPFlag, FIXFlag );
                   TMP2FIX[ (int)TMPFlag[ 0 ] ] = FIXFlag[ 0 ]; 
                   FIX2TMP[ (int)FIXFlag[ 0 ] ] = TMPFlag[ 0 ];
                }
            }
        }
	else
        {
            UFC::UiniFile   FlagMap( FIXTMPFlagMappingFile,TRUE);
            FlagMap.SetValue( "TMP2FIX", "A", "2"); ///< ATM(FIX)
            FlagMap.SetValue( "TMP2FIX", "D", "3"); ///< DMA(FIX) 
            FlagMap.SetValue( "TMP2FIX", "I", "4"); ///< Internet(FIX) 
            FlagMap.SetValue( "TMP2FIX", "V", "5"); ///< Voice(FIX)
            FlagMap.SetValue( "TMP2FIX", "P", "6"); ///< API(FIX)
            FlagMap.Save();
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline void memcpy_null( char* dest, char* src, int len )
{
    memcpy( dest, src, len);
    dest[ len ] = 0;    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline void char_str( char* dest, char src )
{    
    dest[ 0 ] = src;    
    dest[ 1 ] = 0;    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Class to get FIX required fields from TMP message.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TWSEOrder::FromTMP( MessageT010*  NormalOrder, const UFC::AnsiString& PVCID )
{
    Price    = NormalOrder->GetOrderPrice();
    OrderQty = NormalOrder->GetOrderQty(); 
    memcpy_null( OrderNO, NormalOrder->OrderNo, 5);
    memcpy_null( Account, NormalOrder->InvestorAcno, 7 );    
    memcpy_null( Symbol, NormalOrder->StockID, 6 );    
    memcpy_null( BrokerID, NormalOrder->BrokerID, 4 );    
    char_str( InvestorFlag, AccountFlagTMP2FIX( NormalOrder->InvestorFlag ) );    
    char_str( OrderType, NormalOrder->TWSEOrderType );
    char_str( ExchangeCode, NormalOrder->ExchangeCode);
    char_str( TargetSubID, '0');    
    strcpy( TradingSession, "Normal" );
    TimeInforce = NormalOrder->TimeInForce;
    PriceType   = NormalOrder->OrderType;
    Side        = (NormalOrder->BuySellCode == 'B' )? FIX::Side_BUY:FIX::Side_SELL;    
    memcpy( NormalOrder->PVCID, PVCID.c_str(), 2);
}   
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------   
void TWSEOrder::FromTMP( MessageO010*  OddOrder, const UFC::AnsiString& PVCID )
{
    Price    = OddOrder->GetOrderPrice() ;
    OrderQty = OddOrder->GetOrderQty();
    memcpy_null( OrderNO, OddOrder->OrderNo, 5);
    memcpy_null( Account, OddOrder->InvestorAcno, 7 );
    memcpy_null( Symbol, OddOrder->StockID, 6 );
    memcpy_null( BrokerID, OddOrder->BrokerID, 4 );
    char_str( InvestorFlag, AccountFlagTMP2FIX(OddOrder->InvestorFlag ) );    
    char_str( OrderType, '0' );      ///< Normol stocks
    char_str( ExchangeCode, '2' );   ///< Map to TRADE-KIND "2"    
    char_str( TargetSubID, '2');     ///< Odd lots   
    strcpy( TradingSession, "Odd" );        
    TimeInforce = '0';///< ROD
    PriceType   = '2';///< Limit
    Side        = (OddOrder->BuySellCode == 'B' )? FIX::Side_BUY:FIX::Side_SELL;    
    memcpy( OddOrder->PVCID, PVCID.c_str(), 2);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------   
void TWSEOrder::FromTMP( MessageO110*  OddOrder, const UFC::AnsiString& PVCID )
{
    Price    = OddOrder->GetOrderPrice() ;
    OrderQty = OddOrder->GetOrderQty();
    memcpy_null( OrderNO, OddOrder->OrderNo, 5);
    memcpy_null( Account, OddOrder->InvestorAcno, 7 );
    memcpy_null( Symbol, OddOrder->StockID, 6 );
    memcpy_null( BrokerID, OddOrder->BrokerID, 4 );
    char_str( InvestorFlag, AccountFlagTMP2FIX(OddOrder->InvestorFlag ) );    
    char_str( OrderType, '0' );      ///< Normol stocks
    char_str( ExchangeCode, '2' );   ///< Map to TRADE-KIND "2"    
    char_str( TargetSubID, 'C');     ///< Odd lots   
    strcpy( TradingSession, "OddEx" );        
    TimeInforce = '0';///< ROD
    PriceType   = '2';///< Limit
    Side        = (OddOrder->BuySellCode == 'B' )? FIX::Side_BUY:FIX::Side_SELL;    
    memcpy( OddOrder->PVCID, PVCID.c_str(), 2);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TWSEOrder::FromTMP( MessageP010*  PostOrder, const UFC::AnsiString& PVCID )
{
    Price    = PostOrder->GetOrderPrice();
    OrderQty = PostOrder->GetOrderQty();
    memcpy_null(OrderNO, PostOrder->OrderNo, 5);
    memcpy_null(Account, PostOrder->InvestorAcno, 7 );
    memcpy_null(Symbol, PostOrder->StockID, 6 );
    memcpy_null(BrokerID, PostOrder->BrokerID, 4 );
    char_str( InvestorFlag, AccountFlagTMP2FIX(PostOrder->InvestorFlag) );
    char_str( OrderType, PostOrder->TWSEOrderType ); ///< 0:Normal 
    char_str( ExchangeCode, '0');
    char_str( TargetSubID, '7' ); ///< Post trade    
    strcpy( TradingSession, "Post" );
    TimeInforce = '0';///< ROD
    PriceType   = '2';///< Limit    
    Side        = (PostOrder->BuySellCode == 'B' )? FIX::Side_BUY:FIX::Side_SELL;    
    memcpy( PostOrder->PVCID, PVCID.c_str(), 2);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TWSEOrder::FromTMP( MessageV010*  LendOrder, const UFC::AnsiString& PVCID ) //'4' 
{
    Price    = LendOrder->GetOrderPrice(); // 9(4)V9(4)
    OrderQty = LendOrder->GetOrderQty();  // Max 6 digits
    memcpy_null( OrderNO, LendOrder->OrderNo, 5);
    memcpy_null( Account, LendOrder->InvestorAcno, 7 );
    memcpy_null( Symbol, LendOrder->StockID, 6 );
    memcpy_null( BrokerID, LendOrder->BrokerID, 4 );
    char_str( InvestorFlag, '1' );
    char_str( OrderType, '0' ); ///< 0:Normal 
    char_str( ExchangeCode, '0');
    char_str( TargetSubID, '4' ); ///< Lend trade    
    strcpy( TradingSession, "Lend" );
    TimeInforce = '0';///< ROD
    PriceType   = '2';///< Limit    
    Side        =  FIX::Side_SELL;    
    memcpy( LendOrder->PVCID, PVCID.c_str(), 2);    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TWSEOrder::FromTMP( MessageA010*  AuctionOrder, const UFC::AnsiString& PVCID )//'5'
{
    Price    = AuctionOrder->GetOrderPrice(); // 9(5)V9(4)
    OrderQty = AuctionOrder->GetOrderQty();   // Max 12 digits
    memcpy_null(OrderNO, AuctionOrder->OrderNo, 5);
    memcpy_null(Account, AuctionOrder->InvestorAcno, 7 );
    memcpy_null(Symbol, AuctionOrder->StockID, 6 );
    memcpy_null(BrokerID, AuctionOrder->BrokerID, 4 );
    char_str( InvestorFlag, '1' );
    char_str( OrderType, '0' ); ///< 0:Normal 
    char_str( ExchangeCode, '0');
    char_str( TargetSubID, '5' ); ///< Auction trade    
    strcpy( TradingSession, "Auction" );
    TimeInforce = '0';///< ROD
    PriceType   = '2';///< Limit    
    Side        =  FIX::Side_BUY;    
    memcpy( AuctionOrder->PVCID, PVCID.c_str(), 2);  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------   
void TWSEOrder::FromTMP( MessageE010*  TenderOrder, const UFC::AnsiString& PVCID )//'6'
{
    Price    = TenderOrder->GetOrderPrice(); // 9(5)V9(4)
    OrderQty = TenderOrder->GetOrderQty();   // Max 12 digits
    memcpy_null(OrderNO, TenderOrder->OrderNo, 5);
    memcpy_null(Account, TenderOrder->InvestorAcno, 7 );
    memcpy_null(Symbol, TenderOrder->StockID, 6 );
    memcpy_null(BrokerID, TenderOrder->BrokerID, 4 );
    memcpy_null(RefOrderID, TenderOrder->StkSeqNo, 2 );
    char_str( InvestorFlag, '1' );
    char_str( OrderType, '0' ); ///< 0:Normal 
    char_str( ExchangeCode, '0');
    char_str( TargetSubID, '6' ); ///< Tender trade. Reverse Auction  
    strcpy( TradingSession, "Tender" );
    TimeInforce = '0';///< ROD
    PriceType   = '2';///< Limit    
    Side        =  FIX::Side_SELL;    
    memcpy( TenderOrder->PVCID, PVCID.c_str(), 2); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------   
void TWSEOrder::FromTMP( MessageEx010* TenderExOrder, const UFC::AnsiString& PVCID )//'B'
{
    Price    = TenderExOrder->GetOrderPrice();
    OrderQty = TenderExOrder->GetOrderQty(); // Max 12 digits
    memcpy_null(OrderNO, TenderExOrder->OrderNo, 5);
    memcpy_null(Account, TenderExOrder->InvestorAcno, 7 );
    memcpy_null(Symbol, TenderExOrder->StockID, 6 );
    memcpy_null(BrokerID, TenderExOrder->BrokerID, 4 );
    memcpy_null(RefOrderID, TenderExOrder->StkSeqNo, 2 );
    char_str( InvestorFlag, '1' );
    char_str( OrderType, '0' ); ///< 0:Normal 
    char_str( ExchangeCode, '0');
    char_str( TargetSubID, 'B' ); ///< Tender Ex trade. Reverse Auction by Securities Finance Enterprises    
    strcpy( TradingSession, "TenderEx" );
    TimeInforce = '0';///< ROD
    PriceType   = '2';///< Limit    
    Side        =  FIX::Side_SELL;    
    memcpy( TenderExOrder->PVCID, PVCID.c_str(), 2); 
}   
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline char TWSEOrder::AccountFlagTMP2FIX( char TMPFlag )
{
    return TMP2FIX[ (int)TMPFlag ];    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline char TWSEOrder::AccountFlagFIX2TMP( char FIXFlag )
{
    return FIX2TMP[ (int)FIXFlag ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Class SpeedyFIXOrderClient
// Constructor
//
//        SenderCompID(49): T/O + BrokerID + PVCID   Example: T7000E1
//        SenderSubID(50):  BrokerID                 Example: 7001
//        TargetCompID(56): XTAI/ROCO                Example: XTAI (TWSE)  
//        TargetSubID(57):  0                        Example: 0
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SpeedyFIXOrderClient::SpeedyFIXOrderClient( SpeedyObjectsManager* ObjManager,
                                            ConnectionParameter&  Param,
                                            ConnectionListener*   Listener,
                                            Int32                 SubSystem,
                                            SpeedyOrderConnectionObject* Owner,
                                            BOOL                  IsTWSE )
:FConnectionListener( Listener ),
 FOwner( Owner ),
 FListener( NULL ), ///< Disable log transfer
 FSubSystem( SubSystem ), 
 FTargetSubID( Param.GetAPCode() ),
 FPVCID( Param.GetPVCID() ),
 FLineBrokerID( Param.GetBrokerID() ), 
 FEnabled( Param.IsActive() ),
 FPort( Param.GetPort() ),
 FObjectManager( ObjManager ),
 FIsTWSE( IsTWSE )
{    
    
    if( FIsTWSE == TRUE )
    {
        FSenderCompID.Printf( "T%s%s", Param.GetBrokerID().c_str(), Param.GetPVCID().c_str() );
        FTargetCompID = "XTAI";
        FProfileID    = UFC::psTWTSE;
        FTag49.Printf( "49=%s", FSenderCompID.c_str());
        FTag56 = "56=XTAI";
    }
    else
    {
        FSenderCompID.Printf( "O%s%s", Param.GetBrokerID().c_str(), Param.GetPVCID().c_str() );
        FTargetCompID = "ROCO";
        
        FProfileID    = UFC::psTWOTC;
        FTag49.Printf( "49=%s", FSenderCompID.c_str());
        FTag56 = "56=ROCO";
    }    
    sprintf( FLinkPVCSendStr, "Port%05d PVC:%s Send:",Param.GetLocalPort(), FPVCID.c_str() );
    sprintf( FLinkPVCRecvStr, "Port%05d PVC:%s Recv:",Param.GetLocalPort(), FPVCID.c_str() );    
    FIX::SessionID  sessionID( FIX::BeginString_FIX44, FSenderCompID.c_str(),FTargetCompID.c_str());
    
    if( (FSession = FIX::Session::lookupSession( sessionID )) == NULL )
    {
        UFC::AnsiString Msg;
        
        Msg.Printf( "FIX Session %s not found!", sessionID.toString().c_str() );
        throw( UFC::Exception( Msg ) );        
    }
    FLogBuffer  = new char[ TWSE_MAX_MESSAGE_SIZE * 2 ];    
    TWSEOrder::Init( );
    FObjectManager->GetFIXManager()->AddFIXClient( sessionID.toString(), this );
    InitFIXMessages();
    if( FIXRejStaleOrd == TRUE )
    {
        FFIXRejStaleOrd = 'Y';
        FTag10004 ="10004=Y";
    }
    else
    {
        FFIXRejStaleOrd = 'N';
        FTag10004 ="10004=N";
    }
    if( BENCHMARK_MODE == TRUE )
        FFastNewOrder = &SpeedyFIXOrderClient::FastNewOrderBenchmark; 
    else
        FFastNewOrder = &SpeedyFIXOrderClient::FastNewOrder;     
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SpeedyFIXOrderClient::~SpeedyFIXOrderClient()
{
    Close();
}  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::Open()
{
    UFC::BufferedLog::Printf( " Open FIX PVCID[%s]", FPVCID.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::Close()
{
    
    UFC::BufferedLog::Printf( " Close FIX PVCID[%s]", FPVCID.c_str() );
    if( FSession->isLoggedOn() == true )
    {
        FSession->logout( "Session Disabled" );
        FSession->disconnect();        
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SpeedyFIXOrderClient::GetSequence( bool IsIn )
{
    if( IsIn == TRUE )
        return FSession->getExpectedTargetNum();
    else
        return FSession->getExpectedSenderNum();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::SetSequence( int InSeq, int OutSeq )
{
    if( InSeq == 1 && OutSeq == 1 ) ///< EOD
    {
        FSession->logout(); ///< Disconnect the FIX session and reset the store.
        sleep( 1 );
        FSession->reset();
        UFC::BufferedLog::Printf(" FIX PVCID[%s] perform EOD", FPVCID.c_str() );
    }
    else
    {
        FSession->setNextSenderMsgSeqNum( OutSeq );
        FSession->setNextTargetMsgSeqNum( InSeq );    
        UFC::BufferedLog::Printf(" Set FIX PVCID[%s] OutBound[%d] InBound[%d]", FPVCID.c_str(), OutSeq, InSeq );
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// OrderListener interface function
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Call this function when open market.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::OnOpenMarket()
{
    FOrderCount = 0;    
    Handshake( TRUE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::OnReconnect()
{
    Handshake( TRUE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::Handshake( BOOL )
{
    UFC::AnsiString ErrMsg;
    
    UFC::BufferedLog::Printf( " FIX PVCID[%s] Handshake", FPVCID.c_str() );
    if( FSession->isEnabled() == false )
        FSession->logon( );
    if( FSession->isLoggedOn() == false )
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_BROKEN );            
    if( FSession->isSessionTime() == false )
    {
        SetStatus( PVC_STATUS_IDLE );
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_PVC_SLEEPING, NULL );            
        UFC::SleepMS( 30000 );
        UFC::BufferedLog::Printf( " FIX PVCID[%s] session idle", FPVCID.c_str() );
        return;
    }
    
    for( int i = 0; i< 300; i++ )
    {        
        if( FSession->isLoggedOn() == true )        
        {
            if( GetStatus() == PVC_STATUS_TRY_OPEN )
                FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_PVC_TRY_OPEN, NULL );            
            else
                FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_READY, NULL );        
            return;
        }
        UFC::SleepMS( 100 );
    }
    UFC::BufferedLog::Printf( " FIX PVCID[%s] login timeout", FPVCID.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::OnLogon( void )
{
    if( FSession->isLoggedOn() == true )
    {
        if( GetStatus() == PVC_STATUS_TRY_OPEN )
            FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_PVC_TRY_OPEN, NULL );            
        else    
            FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_READY, NULL );        
    }        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::OnLogoff( void )
{
    if( FSession->isLoggedOn() == false )
    {
        if( FSession->isEnabled() == false )    
            FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_PVC_DISABLED );        
        else
            FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_BROKEN );        
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// When this connection idle over 60 sec, perform the idle handshake (R040/R050 exchange)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::OnIdle()
{
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " FIX Session PVCID[%s] Idle.", FPVCID.c_str() );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyFIXOrderClient::IsReady( void )
{
    return FSession->isLoggedOn();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::InitFIXMessages( void )
{
    FOrderReplace = new FIX44::OrderCancelReplaceRequest( FIX::OrigClOrdID( "ID1234567890"),FIX::ClOrdID( "ID1234567890" ),FIX::Side( FIX::Side_BUY ),FIX::TransactTime( true ),FIX::OrdType( FIX::OrdType_LIMIT ) );    
    FOrderCancel  = new FIX44::OrderCancelRequest( FIX::OrigClOrdID( "ID1234567890" ),FIX::ClOrdID( "ID1234567890" ),FIX::Side( FIX::Side_BUY ),FIX::TransactTime( true ));  
    FOrderStatus  = new FIX44::OrderStatusRequest( FIX::ClOrdID( "ID1234567890"), FIX::Side( FIX::Side_BUY ) );            
    FNewOrder     = new FIX44::NewOrderSingle( FIX::ClOrdID( "ID1234567890" ), FIX::Side( FIX::Side_BUY ), FIX::TransactTime( true ), FIX::OrdType( FIX::OrdType_LIMIT ) );         
    
    if( FIXRejStaleOrd == TRUE )
    {
        FOrderReplace->setField( 10004, "Y" ); ///< TwseRejStaleOrd
        FOrderCancel->setField( 10004, "Y" ); ///< TwseRejStaleOrd
        FNewOrder->setField( 10004, "Y" ); ///< TwseRejStaleOrd
    }
    else
    {
        FOrderReplace->setField( 10004, "N" ); ///< TwseRejStaleOrd      
        FOrderCancel->setField( 10004, "N" ); ///< TwseRejStaleOrd      
        FNewOrder->setField( 10004, "N" ); ///< TwseRejStaleOrd      
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SpeedyFIXOrderClient::SendNewOrderSingle( MessageT010* Order, OrderRecord& OI, Int64& BuildFIX )
{      
    char                     TimeStr[ 32 ];
    char                     Msg[ 256 ];
    char                     Flag = TWSEOrder::AccountFlagTMP2FIX( Order->InvestorFlag );
    char                     Side = ( Order->BuySellCode == 'B')? '1': '2'; 
    bool                     Result;   
    UFC::FixSizeMemoryStream Stream( Msg, 256 );        
    UFC::UIntToStr           NID( OI.NID, 10, true );
    UFC::IntToStr            Qty( Order->GetOrderQty(),  6 , true );    
    const int                TimeLen = 21; ///< Format: YYYYMMDD-HH:MM:SS.mmm
    
    UFC::GetFIXTimeString( TimeStr );    
    Stream.Write( FTag49.c_str(), FTag49.Length(), 1);
    Stream.Write( "50=",    3, Order->BrokerID, 4, 1 );   
    Stream.Write( "52=",    3, TimeStr, TimeLen, 1 );
    Stream.Write( FTag56.c_str(), FTag56.Length(), 1);    
    Stream.Write( "57=0",   4, 1 );    
    Stream.Write( "1=",     2, Order->InvestorAcno, 7 , 1);
    Stream.Write( "11=ID",  5, (const char*)NID, 10 , 1); ///< ClOrdID 11=ID1234567890  (ID+NID)
    Stream.Write( "37=",    3, Order->OrderNo, 5, 1 );    ///< OrderID 
    Stream.Write( "38=",    3, (const char*)Qty, Qty.Length(), 1 ); ///< Order Qty   
    if( Order->OrderType == '2' )//< 2:Limit  
    {
        UFC::DoubleToStr Px( Order->GetOrderPrice() + 0.00001, 5, 4 );
        Stream.Write( "44=",    3, (const char*)Px, Px.Length(), 1 );
    }
    else//< 1:Market
        Stream.Write( "44=",    3, "00000.0000", 10, 1 );        
    Stream.Write( "40=",    3, Order->OrderType, 1 );   ///< New OrderType  
    Stream.Write( "59=",    3, Order->TimeInForce, 1 ); ///< New TimeInForce 
    Stream.Write( "54=",    3, Side, 1 );               ///< Side    
    Stream.Write( "55=",    3, Order->StockID, 6, 1 );  ///< Symbol
    Stream.Write( "60=",    3, TimeStr, TimeLen, 1 );   
    Stream.Write( "10000=", 6, Flag , 1 ); 
    Stream.Write( "10001=", 6, Order->TWSEOrderType , 1 ); 
    Stream.Write( "10002=", 6, Order->ExchangeCode,  1 ); 
    Stream.Write( FTag10004.c_str(), 7, 1 );    
    
    BuildFIX = UFC::GetTickCountUS();                         
    Result = FSession->sendNewOrderSingleB( Msg, Stream.GetPosition() );    
    return Result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SpeedyFIXOrderClient::SendNewOrderSingle( MessageT010* Order, OrderRecord& OI )
{      
    char                     TimeStr[ 32 ];
    char                     Msg[ 256 ];
    char                     Flag = TWSEOrder::AccountFlagTMP2FIX( Order->InvestorFlag );
    char                     Side = ( Order->BuySellCode == 'B')? '1': '2'; 
    bool                     Result;   
    UFC::FixSizeMemoryStream Stream( Msg, 256 );        
    UFC::UIntToStr           NID( OI.NID, 10, true );
    UFC::IntToStr            Qty( Order->GetOrderQty(),  6 , true );    
    const int                TimeLen = 21; ///< Format: YYYYMMDD-HH:MM:SS.mmm
    
    UFC::GetFIXTimeString( TimeStr );    
    Stream.Write( FTag49.c_str(), FTag49.Length(), 1);
    Stream.Write( "50=",    3, Order->BrokerID, 4, 1 );   
    Stream.Write( "52=",    3, TimeStr, TimeLen, 1 );
    Stream.Write( FTag56.c_str(), FTag56.Length(), 1);    
    Stream.Write( "57=0",   4, 1 );    
    Stream.Write( "1=",     2, Order->InvestorAcno, 7 , 1);
    Stream.Write( "11=ID",  5, (const char*)NID, 10 , 1); ///< ClOrdID 11=ID1234567890  (ID+NID)
    Stream.Write( "37=",    3, Order->OrderNo, 5, 1 );    ///< OrderID 
    Stream.Write( "38=",    3, (const char*)Qty, Qty.Length(), 1 ); ///< Order Qty   
    if( Order->OrderType == '2' )//< 2:Limit  
    {
        UFC::DoubleToStr Px( Order->GetOrderPrice() + 0.00001, 5, 4 );
        Stream.Write( "44=",    3, (const char*)Px, Px.Length(), 1 );
    }
    else//< 1:Market
        Stream.Write( "44=",    3, "00000.0000", 10, 1 );        
    Stream.Write( "40=",    3, Order->OrderType, 1 );   ///< New OrderType  
    Stream.Write( "59=",    3, Order->TimeInForce, 1 ); ///< New TimeInForce 
    Stream.Write( "54=",    3, Side, 1 );               ///< Side    
    Stream.Write( "55=",    3, Order->StockID, 6, 1 );  ///< Symbol
    Stream.Write( "60=",    3, TimeStr, TimeLen, 1 );   
    Stream.Write( "10000=", 6, Flag , 1 ); 
    Stream.Write( "10001=", 6, Order->TWSEOrderType , 1 ); 
    Stream.Write( "10002=", 6, Order->ExchangeCode,  1 ); 
    Stream.Write( FTag10004.c_str(), 7, 1 );    
    Result = FSession->sendNewOrderSingle( Msg, Stream.GetPosition() );    
    return Result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
FIX::Message* SpeedyFIXOrderClient::CreateNewOrderSingle( TWSEOrder& Order, OrderRecord& OI )
{
   UFC::AnsiString    ClOrdIDStr; 
  
   ClOrdIDStr.Printf( "ID%010u", OI.NID );      
   /// Add Header Fields
   ///< TargetSubID 0: Normal 2:Odd 7:Post C:IntradayOdd  4:Lend 5:Auction 6:Tender B: TenderEx
   FNewOrder->getHeader().set( FIX::TargetSubID( Order.TargetSubID ) );       
   FNewOrder->getHeader().set( FIX::SenderSubID( Order.BrokerID ) );      
   /// Fields for NewOrderSingle
   FNewOrder->set( FIX::Account( Order.Account ) );
   FNewOrder->set( FIX::Symbol( Order.Symbol ) );
   FNewOrder->set( FIX::OrderQty( Order.OrderQty ) );   
   FNewOrder->set( FIX::OrdType( Order.PriceType ) );
   FNewOrder->set( FIX::TimeInForce( Order.TimeInforce ) );
   FNewOrder->set( FIX::Price( Order.Price ));
   ///< Fields requied.
   FNewOrder->set( FIX::ClOrdID( ClOrdIDStr.c_str() )); 
   FNewOrder->set( FIX::Side( Order.Side ) );
   FNewOrder->set( FIX::TransactTime( true ) );
   /// TWSE modify FIX SPEC.
   if( Order.TargetSubID[0] == '6' || Order.TargetSubID[0] == 'B')
       FNewOrder->setField( 1080, Order.RefOrderID ); ///< RefOrderID   
   FNewOrder->setField( 37,    Order.OrderNO );      ///< OrderID    
   FNewOrder->setField( 10000, Order.InvestorFlag ); ///< TwseIvancnoFlag
   FNewOrder->setField( 10001, Order.OrderType );    ///< TwseOrdType
   FNewOrder->setField( 10002, Order.ExchangeCode ); ///< TwseExCode      
   return FNewOrder;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
FIX::Message* SpeedyFIXOrderClient::CreateOrderCancelReplace( TWSEOrder& Reduce, OrderRecord& OI )
{
   UFC::AnsiString   ClOrdIDStr,RPXText;  
   UFC::AnsiString   OrigClOrdIDStr;     
   OrderInfo*        OrderInfo;
   char              TSession;
      
   if( Reduce.OrderQty == 0 )
       ClOrdIDStr.Printf( "RX%010u", OI.NID ); ///< Replace Price          
   else
       ClOrdIDStr.Printf( "ID%010u", OI.NID ); ///< Replace Qty        
   if( EnableRPXText == TRUE &&         
       FObjectManager->OrderInfo.GetOrderInfo( Reduce.OrderNO, Reduce.BrokerID, OrderInfo ) == TRUE )
   {
       UFC::DoubleToStr Px( OrderInfo->GetLastPx() + 0.00001, 5, 4, true );       
       UFC::AnsiString  PsStr( (const char*) Px );
       
       PsStr.PadThis( 10, '0', false );           
       RPXText.Printf("[%06d,%s]", OrderInfo->GetLeavesQty(), PsStr.c_str() );
       FOrderReplace->set( FIX::Text( RPXText.c_str() ));
       OrigClOrdIDStr.Printf( "ID%010u", OrderInfo->GetNID() );
   }
   else
       OrigClOrdIDStr.Printf( "ID%010u", OI.CNID );   
   /// Add Header Fields 
   ///< TargetSubID 0: Normal 2:Odd 7:Post C:IntradayOdd  4:Lend 5:Auction 6:Tender B: TenderEx
   FOrderReplace->getHeader().set( FIX::SenderSubID( Reduce.BrokerID ));   
   FOrderReplace->getHeader().set( FIX::TargetSubID( Reduce.TargetSubID ) );    
   /// Fields for OrderCancelReplaceRequest
   FOrderReplace->set( FIX::OrderID( Reduce.OrderNO ) ); ///< OrderID 
   FOrderReplace->set( FIX::Account( Reduce.Account ) );
   FOrderReplace->set( FIX::Symbol( Reduce.Symbol ) );
   FOrderReplace->set( FIX::OrderQty( Reduce.OrderQty ) );      
   FOrderReplace->set( FIX::OrdType( Reduce.PriceType ) );
   FOrderReplace->set( FIX::TimeInForce( Reduce.TimeInforce ) );   
   TSession = Reduce.TargetSubID[0]; 
   if( TSession == '0' ) ///< 0: Normal
   {   
        if( Reduce.OrderQty == 0 )
        {
            UFC::DoubleToStr Px( Reduce.Price + 0.00001, 5, 4 );       

            FOrderReplace->setField( 44, (const char*) Px );
            FOrderReplace->set( FIX::OrderQty( 0 ) );      
        }    
        else
        {
            FOrderReplace->set( FIX::Price( 0.0 ));
            FOrderReplace->set( FIX::OrderQty( Reduce.OrderQty ) );      
        }
   }
   else if( TSession == '2' || TSession == 'C' || TSession == '7' ) ///< 2:Odd 7:Post C:IntradayOdd 
   {                                                                ///< Reduce Qty only. price should be 0. 
        FOrderReplace->set( FIX::Price( 0.0 ));
        FOrderReplace->set( FIX::OrderQty( Reduce.OrderQty ) );             
   }
   else ///< 4:Lend  Reduce qty only, but needs original price.        
   {    ///< 5:Auction 6:Tender B: TenderEx Replace Px,Qty at the smae time.
       FOrderReplace->set( FIX::Price( Reduce.Price ));      
       FOrderReplace->set( FIX::OrderQty( Reduce.OrderQty ) );      
   }
   ///< Fields requied.
   FOrderReplace->set( FIX::OrigClOrdID( OrigClOrdIDStr.c_str()) ); 
   FOrderReplace->set( FIX::ClOrdID( ClOrdIDStr.c_str() )); 
   FOrderReplace->set( FIX::Side( Reduce.Side ) );
   FOrderReplace->set( FIX::TransactTime( true ) );
   /// TWSE modify FIX SPEC.
   FOrderReplace->setField( 10000, Reduce.InvestorFlag ); ///< TwseIvancnoFlag
   FOrderReplace->setField( 10001, Reduce.OrderType );    ///< TwseOrdType
   FOrderReplace->setField( 10002, Reduce.ExchangeCode ); ///< TwseExCode   
   return FOrderReplace;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
FIX::Message* SpeedyFIXOrderClient::CreateOrderCancel( TWSEOrder& Cancel, OrderRecord& OI )
{
   UFC::AnsiString   ClOrdIDStr;  
   UFC::AnsiString   OrigClOrdIDStr;     
   OrderInfo*        OrderInfo;
      
   ClOrdIDStr.Printf( "ID%010u", OI.NID );   
   if( FObjectManager->OrderInfo.GetOrderInfo( Cancel.OrderNO, Cancel.BrokerID, OrderInfo ) == TRUE )
       OrigClOrdIDStr.Printf( "ID%010u", OrderInfo->GetNID() );
   else
       OrigClOrdIDStr.Printf( "ID%010u", OI.CNID );
   /// Add Header Fields
   ///< TargetSubID 0: Normal 2:Odd 7:Post C:IntradayOdd  4:Lend 5:Auction 6:Tender B: TenderEx
   FOrderCancel->getHeader().set( FIX::SenderSubID( Cancel.BrokerID ));
   FOrderCancel->getHeader().set( FIX::TargetSubID( Cancel.TargetSubID) );    
   /// Fields for OrderCancelRequest
   FOrderCancel->set( FIX::OrderID( Cancel.OrderNO ) ); ///< OrderID 
   FOrderCancel->set( FIX::Account( Cancel.Account ) );
   FOrderCancel->set( FIX::Symbol( Cancel.Symbol ) );
   ///< Fields requied.
   FOrderCancel->set( FIX::OrigClOrdID( OrigClOrdIDStr.c_str()) ); 
   FOrderCancel->set( FIX::ClOrdID( ClOrdIDStr.c_str() )); 
   FOrderCancel->set( FIX::Side( Cancel.Side ) );
   FOrderCancel->set( FIX::TransactTime( true ) );
   /// TWSE modify FIX SPEC.
   FOrderCancel->setField( 10000, Cancel.InvestorFlag ); ///< TwseIvancnoFlag
   FOrderCancel->setField( 10001, Cancel.OrderType );    ///< TwseOrdType
   FOrderCancel->setField( 10002, Cancel.ExchangeCode ); ///< TwseExCode   
   return FOrderCancel;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
FIX::Message* SpeedyFIXOrderClient::CreateOrderStatus( TWSEOrder& Status, OrderRecord& OI )
{
   UFC::AnsiString   ClOrdIDStr;     
   
   ClOrdIDStr.Printf( "ID%010u", OI.NID );         
   /// Add Header Fields
   ///< TargetSubID 0: Normal 2:Odd 7:Post
   FOrderStatus->getHeader().set( FIX::SenderSubID( Status.BrokerID ));
   FOrderStatus->getHeader().set( FIX::TargetSubID( Status.TargetSubID ) );    
   /// Fields for OrderStatusRequest
   FOrderStatus->set( FIX::Symbol( Status.Symbol ) );
   FOrderStatus->set( FIX::OrderID( Status.OrderNO ) ); ///< OrderID 
   ///< Fields requied.
   FOrderStatus->set( FIX::ClOrdID( ClOrdIDStr.c_str() )); 
   FOrderStatus->set( FIX::Side( Status.Side ) );
   /// TWSE modify FIX SPEC.
   FOrderStatus->setField( 10000, Status.InvestorFlag ); ///< TwseIvancnoFlaf
   FOrderStatus->setField( 10001, Status.OrderType );    ///< TwseOrdType
   FOrderStatus->setField( 10002, Status.ExchangeCode ); ///< TwseExCode   
   return FOrderStatus;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsSpecialSession( char  TargetSubID )
{
    switch( TargetSubID )
    {
        case '0':
        case '2':
        case 'C':
        case '7': return false;
        case '4':
        case '5':
        case '6':
        case 'B': return true;
        default:  return false;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Normal Session TargetSubID:'0' Subsystem:30
// Function code:
// 1: Buy 2:Sell 3: Reduce 4: Cancel 5:Query 6:Replace
//
// Post Session TargetSubID:'7' Subsystem:32
// Intra-day odd Session 'C' Subsystem:33
// Odd Session TargetSubID:'2' Subsystem:40
// Function code:
// 1: Buy 2:Sell 3: Reduce 4: Cancel 5:Query 
//
// Lend Session TargetSubID:'4' Subsystem:31
// Auction Session TargetSubID:'5' Subsystem:70
// Tender Session TargetSubID:'6' Subsystem:41
// TenderEx Session TargetSubID:'B' Subsystem:41
// Function code:
// 1: Buy 2:Cancel 3: Reduce 4:Query 
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
FIX::Message* SpeedyFIXOrderClient::CreateFIXMessage( int Func, TWSEOrder& Order, OrderRecord& OI )
{
    FIX::Message* FIXMsg;
    
    if( IsSpecialSession( Order.TargetSubID[0] ) == false )
    {
        switch( Func )
        {
            case 1: ///< New Order, Buy
            case 2: ///< New Order, Sell
                    FIXMsg = CreateNewOrderSingle( Order, OI );
                    break;
            case 3: ///< Reduce Qty       
                    FIXMsg = CreateOrderCancelReplace( Order, OI);
                    break;    
            case 4: ///< Cancel Order   
                    FIXMsg = CreateOrderCancel( Order, OI );
                    break;    
            case 5: ///< order status request            
                    FIXMsg = CreateOrderStatus( Order, OI );
                    break;
            case 6: ///< Replace Price
                    if( Order.TargetSubID[0] != '0' )
                    {
                        FIXMsg = NULL;
                        UFC::BufferedLog::Printf( " FIX Session[%s] not support replace price.", Order.TargetSubID );    
                    }
                    else
                        FIXMsg = CreateOrderCancelReplace( Order, OI);
                    break;                        
                    
            default:FIXMsg = NULL;
                    UFC::BufferedLog::Printf( " FIX Session[%s] not support Function[%d]", Order.TargetSubID, Func );    
                    break;
        }
    }
    else
    {
        switch( Func )
        {
            case 1: ///< New Order
                    FIXMsg = CreateNewOrderSingle( Order, OI );
                    break;
            case 2: ///< Cancel Order   
                    FIXMsg = CreateOrderCancel( Order, OI );
                    break;    
            case 3: ///< Reduce Qty       
                    FIXMsg = CreateOrderCancelReplace( Order, OI);
                    break;    
            case 4: ///< order status request            
                    FIXMsg = CreateOrderStatus( Order, OI );
                    break;
            default:FIXMsg = NULL;
                    UFC::BufferedLog::Printf( " FIX Session[%s] not support Function[%d]", Order.TargetSubID, Func );    
                    break;
        }
    }
    return FIXMsg;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SpeedyFIXOrderClient::FastNewOrder( MessageT010* NormalOrder, OrderRecord& OI  )
{    
    bool Rtn;
    
    memcpy( NormalOrder->PVCID, FPVCID.c_str(),2);
    Rtn = SendNewOrderSingle( NormalOrder, OI );            
    NormalOrder->UpdateTime();                 
    FOrderCount++;      
    return Rtn;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SpeedyFIXOrderClient::FastNewOrderBenchmark( MessageT010* NormalOrder, OrderRecord& OI  )
{    
    Int64         Begin = UFC::GetTickCountUS();
    Int64         BuildFIX,SentFIX;        
    bool          Rtn;
       
    memcpy( NormalOrder->PVCID, FPVCID.c_str(),2);
    Rtn = SendNewOrderSingle( NormalOrder, OI, BuildFIX  );
    SentFIX = UFC::GetTickCountUS();    
    UFC::BufferedLog::Printf( " #1 [SpeedyFIXOrderClient::SendNewOrderSingle] call SendNewOrderSingle[%d]us", SentFIX - Begin );    
    UFC::BufferedLog::Printf( " #2 [SpeedyFIXOrderClient::FastNewOrder] Set FIX message fields[%d]us", BuildFIX - Begin );        
    NormalOrder->UpdateTime();                 
    FOrderCount++;      
    return Rtn;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FIX Message From Broker to Exchange
// 35=D New Order Single
// 35=G Order Cancel/Replace Request
// 35=F Order Cancel Request
// 35=H Order Status Request
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Call this function to send order to TAIFEX
//
// ### TWSE FIX support normal trading session now ###
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyFIXOrderClient::OnOrder( MessageHeader* OrderMessage, OrderRecord& OI  )
{        
    int           Func = OrderMessage->GetFunctionCode();    
    TWSEOrder     Order; 
    
    switch( OrderMessage->MessageType( ) )
    {       
        case mtT010: ///< Normal trading session. TargetSubID = "0"
                    if( Func == 1 || Func == 2 )    
                        return (this->*FFastNewOrder)( static_cast<MessageT010*>(OrderMessage), OI );
                        //return FastNewOrder( static_cast<MessageT010*>(OrderMessage), OI );        
                    else        
                        Order.FromTMP( static_cast<MessageT010*>(OrderMessage), FPVCID );                                               
                    break;                            
        case mtO010: ///< Odd trading session. TargetSubID = "2"
                    Order.FromTMP( static_cast<MessageO010*>(OrderMessage), FPVCID );                         
                    break;    
        case mtO110: ///< OddEx trading session. TargetSubID = "C"
                    Order.FromTMP( static_cast<MessageO110*>(OrderMessage), FPVCID );                         
                    break;                        
        case mtP010: ///< Post trading session. TargetSubID = "7"
                    Order.FromTMP( static_cast<MessageP010*>(OrderMessage), FPVCID );                         
                    break;
        case mtV010: ///< 標借 TargetSubID = “4” Lending Auction
                    Order.FromTMP( static_cast<MessageV010*>(OrderMessage), FPVCID );                         
                    break;
        case mtA010: ///< 拍賣 TargetSubID = “5” Auction
                    Order.FromTMP( static_cast<MessageA010*>(OrderMessage), FPVCID );                         
                    break;
        case mtE010: ///< 標購 TargetSubID = “6” Reverse Auction
                    Order.FromTMP( static_cast<MessageE010*>(OrderMessage), FPVCID );                         
                    break;
        case mtEx010:///< 證金標購 TargetSubID = “B” Reverse Auction by Securities Finance 
                    Order.FromTMP( static_cast<MessageEx010*>(OrderMessage), FPVCID );                         
                    break;
        default:    UFC::BufferedLog::Printf( " FIX not support Message Type[%d]", OrderMessage->MessageType() );                   
                    return false;               
    }
    ///< Send FIX message out
    FIX::Message* MsgOut = CreateFIXMessage( OrderMessage->GetFunctionCode(), Order, OI );     
    bool          SentResult = FALSE;    
    
    if( MsgOut != NULL )
    {                      
         SentResult = FSession->send( *MsgOut );
         UFC::BufferedLog::Printf( "  Order NID[%010u] Sent to FIX session.",  OI.NID  );         
         OrderMessage->UpdateTime();                 
         FOrderCount++;                    
    }                                   
    return SentResult;               
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Call this function to retrieve the reply message from the TAIFEX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* SpeedyFIXOrderClient::OnHandleReplyMessage( TMessageType& Type , BOOL& NeedLog, BOOL& IsCancelDel, UFC::AnsiString& ExecKey, UFC::AnsiString& OrigKey, UFC::AnsiString& BrokerID )
{    
    return NULL;
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::SendFillMessage( char* Buffer, const char* TSECTime, const char* LineTime, Int32 OrigNID, const char* Key, BOOL IsSpeedy )
{    
    if( MessageObj != NULL )
    {
        MTree   Data;

        Data.append( "FILL_ORDER", Buffer );
        Data.append( "PVC",   FPVCID );
        Data.append( "PBNO",  FLineBrokerID );
        Data.append( "NID",   OrigNID );
        Data.append( "TTIME", TSECTime );
        Data.append( "XTIME", LineTime );
        Data.append( "PHOST", UFC::Hostname );         
        if( Key != NULL && strlen( Key ) > 0 )
            Data.append( "KEY", Key );        
        if( ExecToQueue == TRUE && ExtremeVersion == FALSE )
            FObjectManager->SendExecutionToQueue( 2, &Data );    
        if( IsSpeedy == TRUE )
        {
            if( ExtremeVersion == TRUE )
            {
                if( FIsTWSE == TRUE )    
                    gTSEFilled->OnExecution( Buffer, OrigNID, Key, "", &Data  );
                else
                    gOTCFilled->OnExecution( Buffer, OrigNID, Key, "", &Data  );
            }
            else
                MessageObj->Send( FObjectManager->GetFillSubject(), UFC::Hostname, Data );
        }
        else if( NotSpeedyToOthers == TRUE )
        {
            Data.append( "KEY", "Others" );
            if( ExtremeVersion == TRUE )
            {
                if( FIsTWSE == TRUE )    
                    gTSEFilled->OnExecution( Buffer, OrigNID, Key, "", &Data  );
                else
                    gOTCFilled->OnExecution( Buffer, OrigNID, Key, "", &Data  );
            }
            else
                MessageObj->Send( FObjectManager->GetFillSubject(), UFC::Hostname, Data );
        }
        else
            MessageObj->Send( FObjectManager->GetNotSpeedyFillSubject(), UFC::Hostname, Data );        
    }    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::FIXFill( const FIX44::ExecutionReport& Report, OrderInfo* OI )
{
    FIX::TargetSubID  TargetSubID;
    FIX::SenderSubID  SenderSubID;
    FIX::OrderID      OrderID; //Req
    FIX::ClOrdID      ClOrdID;
    FIX::ExecID       ExecID;  //Req *
    FIX::Account      Account;
    FIX::Symbol       Symbol;//Req
    FIX::Side         Side;  //Req
    FIX::TransactTime TransactTime;//Req
    FIX::OrderQty     OrderQty( 0.0 );
    FIX::Price        Price( 0.0 );
    FIX::LastQty      LastQty( 0.0 );
    FIX::LastPx       LastPx( 0.0 );
    FIX::LeavesQty    LeavesQty;//Req
    string            TwseAccFlag( " " ); ///< 10000
    string            TwseOrdType( "0" ); ///< 10001
    string            TwseExCode( "0" );  ///< 10002
    UFC::AnsiString   Value;
    char              TSessoin; 
    int               Y,M,D,h,m,s,ms;
    
    Report.getHeader().get( TargetSubID );
    Report.getHeader().get( SenderSubID );
    Report.get( OrderID );
    Report.get( ExecID );
    Report.get( Symbol );
    Report.get( Side );
    Report.get( TransactTime );
    Report.get( LeavesQty );
    Report.get( LastQty );
    Report.get( LastPx );
    if( Report.isSet( ClOrdID ) )
        Report.get( ClOrdID );
    if( Report.isSet( Account ) )
        Report.get( Account );
    if( Report.isSet( Price ) )
        Report.get( Price );
    if( Report.isSet( OrderQty ) )
        Report.get( OrderQty );
    if( Report.isSetField( 10000 ) )
        TwseAccFlag = Report.getField( 10000 );
    if( Report.isSetField( 10001 ) )
        TwseOrdType = Report.getField( 10001 );
    if( Report.isSetField( 10002 ) )
        TwseExCode  = Report.getField( 10002 ); 
    
    UFC::AnsiString stockNo( Symbol.getString().c_str() );
    UFC::AnsiString OrderNO( OrderID.getString().c_str() );
    UFC::AnsiString BrokerID( TargetSubID.getString().c_str() );
    UFC::AnsiString ExecKey( ExecID.getString().c_str() );
    TransactTime.getValue().getYMD( Y,M,D );
    TransactTime.getValue().getHMS( h, m, s, ms );
    int FIXSeq = ExecKey.SubString( 0, 4 ).ToInt();
    int TMPSeq = ExecKey.SubString( 4, 8 ).ToInt();

    TSessoin = SenderSubID.getString()[0];     
    stockNo.TrimRight(' ');
    ExecKey.Printf( "%s_%s_%c%08d", OrderNO.c_str(), stockNo.c_str(), Side.getValue(), TMPSeq );
    
    R3Body R3Fill;
    stockNo.PadThis( 6, ' ');
    memcpy( R3Fill.FStockID, stockNo.c_str() , 6 );
    R3Fill.SetQty( (int)LastQty.getValue() );
    R3Fill.SetPrice( LastPx.getValue() );
    R3Fill.SetTime( (h+8)%24, m, s, ms );
    R3Fill.FEXCD = TwseExCode[0];

    if( Side == FIX::Side_BUY )
        R3Fill.FBuySellCode = 'B';
    else
        R3Fill.FBuySellCode = 'S';
    memcpy( R3Fill.FOrderNo,  OrderNO.c_str(), 5 );
    memcpy( R3Fill.FInvestorACNo, Account.getString().c_str(), 7 );
    R3Fill.FOrderType = TwseOrdType[0];
    Value.Printf( "%06d",   FIXSeq );
    memcpy( R3Fill.FSeq,    Value.c_str(), 6 );
    memcpy( R3Fill.FBroker, BrokerID.c_str(), 4 );
    Value.Printf( "%08d",   TMPSeq );
    memcpy( R3Fill.FRecNo,  Value.c_str(), 8 );
    R3Fill.Fmarks = ' ';
    if( FObjectManager->OrderInfo.IsExecExist( ExecKey ) == FALSE ) ///< Execution not sent
    {
        if( IsSMPEnable == TRUE && SMP_FIllED_POS == TRUE )
        {
            if( Side == FIX::Side_BUY )
            {
               if( TSessoin == '0')                
                   CheckPositionFilled( ClOrdID, Symbol, OrderID, (int)LastQty.getValue(), false );            
            }                        
        }        
        if( FObjectManager->IsRiskManagerEnabled() && ( FObjectManager->FRMObjPtr != 0 ) )
        {
            if( !FObjectManager->FRMObjPtr->IsFillRptExecExist( ExecKey, UFC::rwLockForRead ) )
            {
                FObjectManager->FRMObjPtr->AddFillRptExec(ExecKey, UFC::rwLockForWrite);
                FObjectManager->FRMObjPtr->AddRiskMatchQty(&R3Fill, ExecKey); 
            }
            else
                twserm::CTWSERiskManagerClass::GetLogObject()->fprintf("SpeedyFIXOrderClient::FIXFill() [%s][%s] has Processed. skip it.", OrderNO.c_str(), ExecKey.c_str());
        }
                    
        UFC::AnsiString TFXTime, RecvTime;
        char            Buffer[ 1024 ];

        R3Fill.AsString( Buffer );
        TFXTime.Printf( "%02d%02d%02d%03d", (h+8)%24, m, s, ms );
        UFC::GetTimeString( RecvTime, FALSE );
        SendFillMessage( Buffer, TFXTime.c_str(),RecvTime.c_str(), OI->GetNID(), OI->GetKeyString(), TRUE  );
        FObjectManager->OrderInfo.WriteExecutionLog( FALSE, OI->GetNID(), OI->GetKeyString(), Buffer, FLineBrokerID, UFC::Hostname, FPVCID ); ///< Write log
    }
    else
       UFC::BufferedLog::Printf( " SpeedyFIXOrderClient::FIXFill() FIX Fill Execution:%s already exist, skip it.", ExecKey.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyFIXOrderClient::ProcessFIXErrorMsg( const FIX44::ExecutionReport& Report, MessageHeader* Reply, UFC::AnsiString& Msg )
{
    BOOL isQuantityWasCut = FALSE;
    int errNo = 0;
    FIX::Text fdText;
    if( Report.isSet( fdText ) ) ///< FIX message contains tag 58 ?
    {
        Report.get( fdText );
        if( fdText.getLength() > 5 )
        {
            UFC::AnsiString errCode( fdText.getString().substr( 0, 4 ).c_str() );
            Msg = UFC::AnsiString( fdText.getString().c_str() + 5 );
            
            FIX::SenderSubID fdSenderSubID;
            if( Report.getHeader().isSet( fdSenderSubID ) )
            {
                Report.getHeader().get( fdSenderSubID );               
                UFC::AnsiString senderSubID( fdSenderSubID.getValue().c_str() );
                char tradingSession = senderSubID[0];
                if( ( ( tradingSession == '0' ) && ( errCode == "0031" ) ) ||  //Regular
                    ( ( tradingSession == '7' ) && ( errCode == "7031" ) ) ||  //Fixed Price
                    ( ( tradingSession == 'C' ) && ( errCode == "C031" ) ) )   //Intrady odd lot
                {
                    isQuantityWasCut = TRUE;
                    errNo = 31;  //Quantity was cut
                }
                else if( ( tradingSession == '0' ) && ( errCode == "0051" ) )  //Regular
                {
                    isQuantityWasCut = TRUE;
                    errNo = 51;  //Quantity was cut
                }
                else if( ( tradingSession == '2' ) &&  ( errCode == "2032" ) )  //Odd Lots
                {
                    isQuantityWasCut = TRUE;
                    errNo = 32;  //Quantity was cut
                }
                else
                    errNo = 0;
                
                if( errNo != 0)
                    UFC::BufferedLog::Printf( " %s() Session[%c] StatusCode[%s] Quantity was cut.", __func__, tradingSession, errCode.c_str() );
            }  //if( Report.getHeader().isSet( fdSenderSubID ) )
        }  //if( fdText.getLength() > 5 )
    }  //if( Report.isSet( fdText ) )
    
    Reply->SetStatusCode( errNo );
    return isQuantityWasCut;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyFIXOrderClient::ProcessFIXRejectMsg( const FIX44::ExecutionReport& Report, MessageHeader* Reply, UFC::AnsiString& Msg  )
{
    FIX::Text         Text; 
    UFC::AnsiString   Value;
    int               ErrCode;
    
    if( Report.isSet( Text ) ) ///< FIX message contains tag 58 ?                                      
    {   
        Report.get( Text );
        if( Text.getLength() > 5 ) ///< Format: 0031-Error Message String
        {
            FObjectManager->WarnningStatusCode( Text.getString().c_str() );
            Value = Text.getString().substr( 2, 2 ).c_str();
            ErrCode = Value.ToInt();
            if( ErrCode == 0 ) ///< Code: XX00
            {
                ErrCode = 99;
                Msg = UFC::AnsiString( Text.getString().c_str() );
            }
            else
                Msg = UFC::AnsiString( Text.getString().c_str() + 5 );
            Reply->SetStatusCode( ErrCode );    
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
OrderRecord* SpeedyFIXOrderClient::GetSMPOrderRecord( OrderInfo* OI, MessageHeader* Msg )
{
    char         Order[ 512 ]; 
    OrderRecord* OR = OrderRecodPoolPtr->GetInstance();
    
    Msg->AsString( Order );
    OR->InitTWSE(OI->GetNID(),OI->GetFuncCode(), OI->GetKeyString(), Order );    
    return OR;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::CheckPositionFilled( FIX::ClOrdID& ClOrdID, FIX::Symbol& Symbol, FIX::OrderID& OID, int Qty, bool IsOdd )
{
#ifdef __EXTREME_VERSION           
    UFC::AnsiString SubAcc( ClOrdID.getString().c_str() + 1 , 4 );
    UFC::AnsiString Sym( Symbol.getString().c_str() );
    UFC::AnsiString OrderID( OID.getString().c_str() );
    int  Shares;
                                       
    if( IsOdd == true )
        Shares = Qty;
    else
        Shares = Qty*1000;    
    Sym.PadThis( 6, ' ');        
    if( FSMPGateway != NULL )    
        FSMPGateway->AddFilledPosition( SubAcc, Sym, OrderID, Shares );
#endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::CheckPartiallyConfirmPositionRelease( FIX::ClOrdID& ClOrdID, FIX::OrderID& OrderID, FIX::Symbol& Symbol, FIX::OrderQty& Qty, char TwseOrdType, bool IsNorman )
{
#ifdef __EXTREME_VERSION         
    UFC::AnsiString SubAcc( ClOrdID.getString().c_str() + 1 , 4 );
    UFC::AnsiString Sym( Symbol.getString().c_str() );
    UFC::AnsiString OID( OrderID.getString().c_str() );
    int  Shares      = (int)Qty.getValue()*1000;
    bool IsShortSell = false; 
                                        
    if( TwseOrdType == '5' || TwseOrdType == '6' )
        IsShortSell = true;  
    Sym.PadThis( 6, ' ');    
    if( FSMPGateway != NULL )    
        FSMPGateway->AddPartiallyConfirmStockPosition( SubAcc, Sym, OID, Shares, IsShortSell, IsNorman );
#endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::CheckPositionRelease( FIX::ClOrdID& ClOrdID, FIX::Symbol& Symbol, FIX::OrderID& OID, FIX::OrderQty& Qty, char TwseOrdType, bool IsOdd, bool IsPost  )
{
#ifdef __EXTREME_VERSION           
    UFC::AnsiString SubAcc( ClOrdID.getString().c_str() + 1 , 4 );
    UFC::AnsiString Sym( Symbol.getString().c_str() );
    UFC::AnsiString OrderID( OID.getString().c_str() );
    int  Shares;
    bool IsShortSell = false; 
                                        
    if( IsOdd == true )
        Shares = (int)Qty.getValue();
    else
        Shares = (int)Qty.getValue()*1000;
    if( TwseOrdType == '5' || TwseOrdType == '6' )
        IsShortSell = true;  
    Sym.PadThis( 6, ' ');    

    if( FSMPGateway != NULL )    
    {
        if( IsOdd == true || IsPost == true )
            FSMPGateway->AddStockPosition( SubAcc, Sym, OrderID, Shares, IsShortSell, true  );
        else
            FSMPGateway->AddStockPosition( SubAcc, Sym, OrderID, Shares, IsShortSell, false  );
    }    
           
#endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::CheckPartiallyConfirmFundRelease( FIX::Account& Account, FIX::OrderID& OrderID, FIX::Symbol& Symbol, FIX::OrderQty& Qty, FIX::Price& Price )
{
#ifdef __EXTREME_VERSION           
    UFC::AnsiString Acc( Account.getString().c_str()  );
    UFC::AnsiString Sym( Symbol.getString().c_str() );
    UFC::AnsiString OID( OrderID.getString().c_str() );
    int  Shares      = (int)Qty.getValue()*1000;
    
    Sym.PadThis( 6, ' ');        
    if( FSMPGateway != NULL )   
        FSMPGateway->AddPartiallyConfirmFund(  Acc,  Sym, OID,  Shares, Price.getValue() );            
#endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::CheckFundRelease( FIX::Account& Account, FIX::Symbol& Symbol, FIX::OrderID& OID, FIX::Price& Px, FIX::OrderQty& Qty, bool IsOdd )
{
#ifdef __EXTREME_VERSION           
    UFC::AnsiString Acc( Account.getString().c_str() );
    UFC::AnsiString Sym( Symbol.getString().c_str() );
    UFC::AnsiString OrderID( OID.getString().c_str() );
    int    Shares;
    Int64  Fund;
                                            
    if( IsOdd == true )
        Shares = (int)Qty.getValue();
    else
        Shares = (int)Qty.getValue()*1000;
    Fund = (Int64)((double)Shares* Px.getValue());
    
    if( FSMPGateway != NULL )    
           FSMPGateway->AddStockFund( Acc, Sym, OrderID, Fund );
#endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::FIXConfirmNormal( const FIX44::ExecutionReport& Report, OrderInfo* OI )
{
    FIX::TargetSubID  TargetSubID;
    FIX::OrderID      OrderID; //Req
    FIX::ClOrdID      ClOrdID; 
    FIX::ExecID       ExecID;  //Req *
    FIX::ExecType     ExecType;//Req
    FIX::OrdType      OrdType( FIX::OrdType_LIMIT );  
    FIX::TimeInForce  TimeInForce( FIX::TimeInForce_DAY );
    FIX::Account      Account;
    FIX::Symbol       Symbol;//Req
    FIX::Side         Side;  //Req
    FIX::TransactTime TransactTime;//Req 
    FIX::OrderQty     OrderQty( 0.0 );
    FIX::Price        Price( 0.0 );
    FIX::LeavesQty    LeavesQty;//Req     
    string            TwseAccFlag( " " ); ///< 10000
    string            TwseOrdType( "0" ); ///< 10001
    string            TwseExCode( "0" );  ///< 10002
    UFC::AnsiString   Value,Msg;
    UFC::AnsiString   OrderNO;
    UFC::AnsiString   BrokerID;
    UFC::AnsiString   ExecKey;
    UFC::AnsiString   ID;
    int               Y, M, D, h, m, s, ms;
    int               NID;
    OrderRecord*      OR;             
    
    Report.getHeader().get( TargetSubID );
    Report.get( OrderID );
    Report.get( ExecID );    
    Report.get( ExecType );    
    Report.get( Symbol );
    Report.get( Side );    
    Report.get( TransactTime );
    Report.get( LeavesQty );        
    if( Report.isSet( ClOrdID ) )
        Report.get( ClOrdID );
    if( Report.isSet( Account ) )
        Report.get( Account );
    else
        Account.setString( "0000000" );
    if( Report.isSet( Price ) )
        Report.get( Price );    
    if( Report.isSet( OrderQty ) )
        Report.get( OrderQty );
    if( Report.isSet( OrdType ) )
        Report.get( OrdType );
    if( Report.isSet( TimeInForce ) )
        Report.get( TimeInForce );
    if( Report.isSetField( 10000 ) )
        TwseAccFlag = Report.getField( 10000 );
    if( Report.isSetField( 10001 ) )
        TwseOrdType = Report.getField( 10001 );
    if( Report.isSetField( 10002 ) )
        TwseExCode  = Report.getField( 10002 );    
    OrderNO  = OrderID.getString().c_str();
    ExecKey  = ExecID.getString().c_str();
    BrokerID = TargetSubID.getString().c_str();    
    NID = UFC::AnsiString( ClOrdID.getString().c_str() + 2 ).ToInt();    
    ID =  UFC::AnsiString( ClOrdID.getString().c_str(), 2 );
    TransactTime.getValue().getYMD( Y, M, D );
    TransactTime.getValue().getHMS( h, m, s, ms );       
            
    if( (OR = FObjectManager->PopOrderRecord( NID ) ) == NULL )
        OR = FObjectManager->PopOrderRecord( ClOrdID.getString().c_str() ); ///< From SMP protocol , pop OrderRecord from ClOrdID    
    if( OR != NULL || IsSMPEnable == TRUE || ExecType == FIX::ExecType_RESTATED )
    {   ///< Fill T020 fields
        MessageT020 NormalReply( FIsTWSE ); 

        memcpy( NormalReply.PVCID,        FPVCID.c_str(), 2 );
        memcpy( NormalReply.OrderNo,      OrderID.getString().c_str(), 5 );
        memcpy( NormalReply.BrokerID,     BrokerID.c_str(), 4 );
        memcpy( NormalReply.InvestorAcno, Account.getString().c_str(), 7 );
        NormalReply.InvestorFlag = TWSEOrder::AccountFlagFIX2TMP( TwseAccFlag[0] );
        memcpy( NormalReply.StockID,      Symbol.getString().c_str(), 6 );            
        NormalReply.SetPrice( Price.getValue() );
        NormalReply.SetQty( (int)OrderQty.getValue() );                    
        NormalReply.BuySellCode = ( Side == FIX::Side_BUY )? 'B':'S';
        NormalReply.ExchangeCode  = TwseExCode[0];            
        NormalReply.TWSEOrderType = TwseOrdType[0];            
        NormalReply.OrderType     = OrdType.getValue();            
        NormalReply.TimeInForce   = TimeInForce.getValue();
        NormalReply.SetDate( Y, M, D );
        NormalReply.SetTime( ( h + 8 ) % 24, m, s, ms );
        switch( ExecType )
        {
            case FIX::ExecType_NEW: ///< NewOrder.                                        
                                    NormalReply.SetBeforeQty( 0 );
                                    NormalReply.SetAfterQty( (int)LeavesQty.getValue() );                                                                        
                                    if( Side == FIX::Side_BUY )
                                        NormalReply.SetFunctionCode( 1 );
                                    else
                                        NormalReply.SetFunctionCode( 2 );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &NormalReply );
                                    if( ProcessFIXErrorMsg( Report, &NormalReply, Msg ) == TRUE )
                                    {   ///<Quantity was cut: Error code 31(Regular, Fix Price, IntraDay Odd Lot) or 51(Regular)
                                        if( IsSMPEnable == TRUE )
                                        {
                                            if( Side == FIX::Side_SELL )                                    
                                                CheckPartiallyConfirmPositionRelease( ClOrdID, OrderID, Symbol, OrderQty, TwseOrdType[0], TRUE );
                                            else
                                                CheckPartiallyConfirmFundRelease(  Account,  OrderID,  Symbol, OrderQty,  Price );
                                        }
                                    }
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                    if( FOwner->IsTrying() == TRUE )                                                    
                                        FOwner->HandleExchangeReplyTryMessage( &NormalReply, *OR, mtT020, TRUE, FALSE, ExecKey, OR->Key );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &NormalReply, *OR, 0, mtT020, TRUE, FALSE, ExecKey, OR->Key, Msg );
                                    break;
            case FIX::ExecType_REPLACE:  ///< Replaced. ///< Before Qty = OrderQty + LeavesQty                                                                                           
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &NormalReply );
                                    if( ID == "RX" || OR->FuncCode == 6 ) ///< Change Price ///< From SMP protocol , only FuncCode == 6
                                    {
                                        NormalReply.SetFunctionCode( 6 ); 
                                        NormalReply.SetBeforeQty( (int)LeavesQty.getValue() );
                                        OI->SetLastPx( Price.getValue() );
                                    }
                                    else ///< Reduce Qty
                                    {
                                        NormalReply.SetFunctionCode( 3 ); 
                                        NormalReply.SetBeforeQty( (int)(OrderQty.getValue() + LeavesQty.getValue()) );
                                        OI->SetLeavesQty( (int)LeavesQty.getValue() );
                                        if( IsSMPEnable == TRUE )
                                        {
                                            if( Side == FIX::Side_SELL )
                                                CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], false,false );
                                            else
                                                CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, false );
                                        }
                                    }
                                    NormalReply.SetAfterQty( (int)LeavesQty.getValue() );
                                    NormalReply.SetStatusCode( 0 );              
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );
                                    if( FOwner->IsTrying() == TRUE )
                                        FOwner->HandleExchangeReplyTryMessage( &NormalReply, *OR, mtT020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &NormalReply, *OR, OI->GetNID(), mtT020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    break;
            case FIX::ExecType_RESTATED:
                                    NormalReply.SetBeforeQty( (int)OrderQty.getValue() );
                                    NormalReply.SetAfterQty( (int)LeavesQty.getValue() );
                                    NormalReply.SetStatusCode( 51 );
                                    NormalReply.SetFunctionCode( 4 );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &NormalReply );
                                    if( IsSMPEnable == TRUE )
                                    {
                                        if( Side == FIX::Side_SELL )
                                            CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], false, false );
                                        else
                                            CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, false );
                                    }
                                    FOwner->HandleExchangeReplyMessage( &NormalReply, OI, mtT020, TRUE, TRUE, ExecKey, OI->GetKeyString(), "Unsolicited cancel by Exchange" );
                                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_RESTATED OrderID[%s] canceled by exchange.", OrderNO.c_str() );
                                    break;
            case FIX::ExecType_CANCELED: ///< Canceled.                                               
                                    NormalReply.SetBeforeQty( (int)OrderQty.getValue() );
                                    NormalReply.SetAfterQty( (int)LeavesQty.getValue() );
                                    NormalReply.SetStatusCode( 0 );
                                    NormalReply.SetFunctionCode( 4 );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &NormalReply );                                    
                                    if( IsSMPEnable == TRUE )
                                    {
                                        if( Side == FIX::Side_SELL )
                                            CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], false, false );
                                        else
                                            CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, false );
                                    }
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );
                                    if( FOwner->IsTrying() == TRUE )
                                        FOwner->HandleExchangeReplyTryMessage( &NormalReply, *OR, mtT020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &NormalReply, *OR, OI->GetNID(), mtT020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    break;
            case FIX::ExecType_REJECTED: ///< New order Reject.
                                    if( ProcessFIXRejectMsg(  Report, &NormalReply, Msg ) == true )
                                    {
                                        NormalReply.SetBeforeQty( 0 );
                                        NormalReply.SetAfterQty( (int)LeavesQty.getValue() );
                                        if( Side == FIX::Side_BUY )
                                            NormalReply.SetFunctionCode( 1 );
                                        else
                                            NormalReply.SetFunctionCode( 2 );
                                        if( OR == NULL && IsSMPEnable == TRUE )
                                            OR = GetSMPOrderRecord( OI, &NormalReply );                                        
                                        if( IsSMPEnable == TRUE )
                                        {
                                            if( Side == FIX::Side_SELL )
                                                CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], false, false );
                                            else
                                                CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, false );
                                        }                                        
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );
                                        if( FOwner->IsTrying() == TRUE )
                                            FOwner->HandleExchangeReplyTryMessage( &NormalReply, *OR, mtT020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                        else
                                            FOwner->HandleExchangeReplyMessage( &NormalReply, *OR, OI->GetNID(), mtT020, TRUE, TRUE, ExecKey, OI->GetKeyString(), Msg );
                                        if( NormalReply.GetStatusCode() == 89 && FObjectManager->StopOnOverLimit() )
                                        {
                                            UFC::BufferedLog::Printf( " Disable Session[%s:%d:%s] [89]Error Over Limit", FLineBrokerID.c_str(), FPort, FPVCID.c_str());
                                            FObjectManager->EnableConnection( FLineBrokerID, FPort, FPVCID, false );
                                        }
                                    }
                                    else
                                        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_REJECTED OrderID[%s] without Tag 58.", OrderNO.c_str() );
                                    break;
            case FIX::ExecType_ORDER_STATUS: ///< Order ststus
                                    NormalReply.SetBeforeQty( (int)OrderQty.getValue() );
                                    NormalReply.SetAfterQty( (int)OrderQty.getValue() );
                                    NormalReply.SetStatusCode( 0 );
                                    NormalReply.SetFunctionCode( 5 );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &NormalReply );
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );
                                    if( FOwner->IsTrying() == TRUE )
                                        FOwner->HandleExchangeReplyTryMessage( &NormalReply, *OR, mtT020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &NormalReply, *OR, 0, mtT020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    break;
            default:                UFC::BufferedLog::Printf( " [ExecutionReport] un-handled ExecType[%c]",ExecType.getValue() );
                                    break;
        }
        OrderRecodPoolPtr->Recycle( OR );
    }
    else
        UFC::BufferedLog::Printf( " [ExecutionReport] Execution with out New,Cancel, OrderID[%s] BrokerID[%s] NID[%d]", OrderNO.c_str(), BrokerID.c_str(), NID );
        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::FIXConfirmOdd( const FIX44::ExecutionReport& Report, OrderInfo* OI )
{
    FIX::TargetSubID  TargetSubID;
    FIX::OrderID      OrderID; //Req
    FIX::ClOrdID      ClOrdID; 
    FIX::ExecID       ExecID;  //Req *
    FIX::ExecType     ExecType;//Req
    FIX::OrdType      OrdType( FIX::OrdType_LIMIT );  
    FIX::TimeInForce  TimeInForce( FIX::TimeInForce_DAY );    
    FIX::Account      Account;
    FIX::Symbol       Symbol;//Req
    FIX::Side         Side;  //Req
    FIX::TransactTime TransactTime;//Req 
    FIX::OrderQty     OrderQty( 0.0 );
    FIX::Price        Price( 0.0 );
    FIX::LeavesQty    LeavesQty;//Req 
    FIX::Text         Text; 
    string            TwseAccFlag( " " ); ///< 10000
    string            TwseOrdType( "0" ); ///< 10001
    string            TwseExCode( "0" );  ///< 10002
    UFC::AnsiString   Value,Msg;
    UFC::AnsiString   OrderNO;
    UFC::AnsiString   BrokerID;
    UFC::AnsiString   ExecKey;
    int               Y, M, D, h, m, s, ms;
    int               NID;
    OrderRecord*      OR;             
    
    Report.getHeader().get( TargetSubID );
    Report.get( OrderID );
    Report.get( ExecID );    
    Report.get( ExecType );    
    Report.get( Symbol );
    Report.get( Side );    
    Report.get( TransactTime );
    Report.get( LeavesQty );    
    if( Report.isSet( ClOrdID ) )
        Report.get( ClOrdID );
    if( Report.isSet( Account ) )
        Report.get( Account );
    if( Report.isSet( Price ) )
        Report.get( Price );    
    if( Report.isSet( OrderQty ) )
        Report.get( OrderQty );    
    if( Report.isSet( OrdType ) )
        Report.get( OrdType );    
    if( Report.isSet( TimeInForce ) )
        Report.get( TimeInForce );    
    if( Report.isSetField( 10000 ) )
        TwseAccFlag = Report.getField( 10000 );
    if( Report.isSetField( 10001 ) )
        TwseOrdType = Report.getField( 10001 );
    if( Report.isSetField( 10002 ) )
        TwseExCode  = Report.getField( 10002 );    
    OrderNO  = OrderID.getString().c_str();
    ExecKey  = ExecID.getString().c_str();
    BrokerID = TargetSubID.getString().c_str();
    NID = UFC::AnsiString( ClOrdID.getString().c_str() + 2 ).ToInt();
    TransactTime.getValue().getYMD( Y,M,D );
    TransactTime.getValue().getHMS( h, m, s, ms );       
    if( (OR = FObjectManager->PopOrderRecord( NID ) ) == NULL )
         OR = FObjectManager->PopOrderRecord( ClOrdID.getString().c_str() ); ///< From SMP protocol , pop OrderRecord from ClOrdID    
    if( OR != NULL || IsSMPEnable == TRUE || ExecType == FIX::ExecType_RESTATED )
    {
        ///< Fill O020 fields
        MessageO020 OddReply( FIsTWSE ); 

        memcpy( OddReply.PVCID,        FPVCID.c_str(), 2 );
        memcpy( OddReply.OrderNo,      OrderID.getString().c_str(), 5 );
        memcpy( OddReply.BrokerID,     BrokerID.c_str(), 4 );
        memcpy( OddReply.InvestorAcno, Account.getString().c_str(), 7 );
        OddReply.InvestorFlag = TWSEOrder::AccountFlagFIX2TMP( TwseAccFlag[0] );
        memcpy( OddReply.StockID,      Symbol.getString().c_str(), 6 );            
        OddReply.SetPrice( Price.getValue() );
        OddReply.SetQty( (int)OrderQty.getValue() );                                
        OddReply.BuySellCode = ( Side == FIX::Side_BUY )? 'B':'S';            
        OddReply.ExchangeCode  = TwseExCode[0];                        
        OddReply.TWSEOrderType = TwseOrdType[0];            
        OddReply.OrderType     = OrdType.getValue();            
        OddReply.TradeKind     = TwseExCode[0];
        OddReply.TimeInForce   = TimeInForce.getValue();            
        memcpy( OddReply.ObjectBroker, BrokerID.c_str(), 4 );                                    
        OddReply.SetDate( Y, M, D );
        OddReply.SetTime( ( h + 8 ) % 24, m, s, ms );            

        switch( ExecType )
        {
            case FIX::ExecType_NEW: ///< NewOrder.                                           
                                    OddReply.SetBeforeQty( 0 );///< Before Qty = 0                                        
                                    OddReply.SetAfterQty((int)LeavesQty.getValue() );///< After Qty = LeavesQty
                                    OddReply.SetStatusCode( 0 );
                                    if( Side == FIX::Side_BUY )
                                        OddReply.SetFunctionCode( 1 );
                                    else
                                        OddReply.SetFunctionCode( 2 );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &OddReply );
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                    if( FOwner->IsTrying() == TRUE )
                                        FOwner->HandleExchangeReplyTryMessage( &OddReply, *OR, mtO020, TRUE, FALSE, ExecKey, OR->Key );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &OddReply, *OR, 0, mtO020, TRUE, FALSE, ExecKey, OR->Key );
                                    break;
            case FIX::ExecType_REPLACE:  ///< Replaced. ( Not support replace price )                                       
                                    OddReply.SetBeforeQty( (int)(OrderQty.getValue() + LeavesQty.getValue()) );///< Before Qty = OrderQty + LeavesQty                                           
                                    OddReply.SetAfterQty( (int)LeavesQty.getValue() );///< After Qty = LeavesQty
                                    OddReply.SetStatusCode( 0 );
                                    OddReply.SetFunctionCode( 3 ); 
                                    OI->SetLeavesQty( (int)LeavesQty.getValue() );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &OddReply );
                                    if( IsSMPEnable == TRUE )
                                    {
                                        if( Side == FIX::Side_SELL )
                                            CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], true, false );
                                        else
                                            CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, true );
                                    }
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );
                                    //UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_REPLACE OrderID[%s]", OrderNO.c_str() );
                                    if( FOwner->IsTrying() == TRUE )
                                        FOwner->HandleExchangeReplyTryMessage( &OddReply, *OR, mtO020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &OddReply, *OR, OI->GetNID(), mtO020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    break;
            case FIX::ExecType_RESTATED:                                         
                                    OddReply.SetBeforeQty(  (int)OrderQty.getValue() );///< Before Qty = OrderQty
                                    OddReply.SetAfterQty((int)LeavesQty.getValue() );///< After Qty = LeavesQty
                                    OddReply.SetStatusCode( 51 );
                                    OddReply.SetFunctionCode( 4 );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &OddReply );
                                    if( IsSMPEnable == TRUE )
                                    {
                                        if( Side == FIX::Side_SELL )
                                            CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], true, false );
                                        else
                                            CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, true );
                                    }
                                    FOwner->HandleExchangeReplyMessage( &OddReply, OI, mtT020, TRUE, TRUE, ExecKey, OI->GetKeyString(), "Unsolicited cancel by Exchange" );
                                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_RESTATED OrderID[%s] canceled by exchange.", OrderNO.c_str() );
                                    break;
            case FIX::ExecType_CANCELED: ///< Canceled.
                                    OddReply.SetBeforeQty( (int)OrderQty.getValue() ); ///< Before Qty = OrderQty
                                    OddReply.SetAfterQty( (int)LeavesQty.getValue() ); ///< After Qty = LeavesQty
                                    OddReply.SetStatusCode( 0 );
                                    OddReply.SetFunctionCode( 4 );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &OddReply );
                                    if( IsSMPEnable == TRUE )
                                    {
                                        if( Side == FIX::Side_SELL )
                                            CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], true, false );
                                        else
                                            CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, true );
                                    }
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                    if( FOwner->IsTrying() == TRUE )
                                        FOwner->HandleExchangeReplyTryMessage( &OddReply, *OR, mtO020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &OddReply, *OR, OI->GetNID(), mtO020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    break;
            case FIX::ExecType_REJECTED: ///< New order Reject.
                                    Report.get( Text ); ///< Format: 0000-String
                                    if( ProcessFIXRejectMsg(  Report, &OddReply, Msg ) == true )
                                    {
                                        OddReply.SetBeforeQty( 0 );                       ///< Before Qty = 000
                                        OddReply.SetAfterQty( (int)LeavesQty.getValue() );///< After Qty = LeavesQty
                                        if( Side == FIX::Side_BUY )
                                            OddReply.SetFunctionCode( 1 );
                                        else
                                            OddReply.SetFunctionCode( 2 );
                                        if( OR == NULL && IsSMPEnable == TRUE )
                                            OR = GetSMPOrderRecord( OI, &OddReply );
                                        if( IsSMPEnable == TRUE )
                                        {
                                            if( Side == FIX::Side_SELL )
                                                CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], true, false );
                                            else
                                                CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, true );
                                        }
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                        if( FOwner->IsTrying() == TRUE )
                                            FOwner->HandleExchangeReplyTryMessage( &OddReply, *OR, mtO020,TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                        else
                                            FOwner->HandleExchangeReplyMessage( &OddReply, *OR, OI->GetNID(), mtO020, TRUE, TRUE, ExecKey, OI->GetKeyString(), Msg );
                                        if( OddReply.GetStatusCode() == 89 && FObjectManager->StopOnOverLimit() )
                                        {
                                            UFC::BufferedLog::Printf( " Disable Session[%s:%d:%s] [89]Error Over Limit", FLineBrokerID.c_str(), FPort, FPVCID.c_str());
                                            FObjectManager->EnableConnection( FLineBrokerID, FPort, FPVCID, false );
                                        }
                                    }
                                    else
                                       UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_REJECTED OrderID[%s] without Tag 58.", OrderNO.c_str() );
                                    break;                
            case FIX::ExecType_ORDER_STATUS: ///< Order ststus
                                    OddReply.SetBeforeQty( (int)OrderQty.getValue() );
                                    OddReply.SetAfterQty( (int)OrderQty.getValue() );
                                    OddReply.SetStatusCode( 0 );
                                    OddReply.SetFunctionCode( 5 );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &OddReply );
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );
                                    if( FOwner->IsTrying() == TRUE )
                                        FOwner->HandleExchangeReplyTryMessage( &OddReply,*OR, mtO020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &OddReply,*OR, 0, mtO020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                    break;
            default:                UFC::BufferedLog::Printf( " [ExecutionReport] un-handled ExecType[%c]",ExecType.getValue() );
                                    break;
        }
        //delete OR; ///< delete object get from PopOrderRecord.
        OrderRecodPoolPtr->Recycle( OR );
    }
    else
        UFC::BufferedLog::Printf( " [ExecutionReport] Execution with out New,Cancel, OrderID[%s] BrokerID[%s]", OrderNO.c_str(), BrokerID.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::FIXConfirmOddEx( const FIX44::ExecutionReport& Report, OrderInfo* OI )
{
    FIX::TargetSubID  TargetSubID;
    FIX::OrderID      OrderID; //Req
    FIX::ClOrdID      ClOrdID; 
    FIX::ExecID       ExecID;  //Req *
    FIX::ExecType     ExecType;//Req
    FIX::OrdType      OrdType( FIX::OrdType_LIMIT );  
    FIX::TimeInForce  TimeInForce( FIX::TimeInForce_DAY );    
    FIX::Account      Account;
    FIX::Symbol       Symbol;//Req
    FIX::Side         Side;  //Req
    FIX::TransactTime TransactTime;//Req 
    FIX::OrderQty     OrderQty( 0.0 );
    FIX::Price        Price( 0.0 );
    FIX::LeavesQty    LeavesQty;//Req 
    FIX::Text         Text; 
    string            TwseAccFlag( " " ); ///< 10000
    string            TwseOrdType( "0" ); ///< 10001
    //string            TwseExCode( "0" );  ///< 10002
    UFC::AnsiString   Value,Msg;
    UFC::AnsiString   OrderNO;
    UFC::AnsiString   BrokerID;
    UFC::AnsiString   ExecKey;
    int               Y, M, D, h, m, s, ms;
    int               NID;
    OrderRecord*      OR;             
    
    Report.getHeader().get( TargetSubID );
    Report.get( OrderID );
    Report.get( ExecID );    
    Report.get( ExecType );    
    Report.get( Symbol );
    Report.get( Side );    
    Report.get( TransactTime );
    Report.get( LeavesQty );    
    if( Report.isSet( ClOrdID ) )
        Report.get( ClOrdID );
    if( Report.isSet( Account ) )
        Report.get( Account );
    if( Report.isSet( Price ) )
        Report.get( Price );    
    if( Report.isSet( OrderQty ) )
        Report.get( OrderQty );    
    if( Report.isSet( OrdType ) )
        Report.get( OrdType );    
    if( Report.isSet( TimeInForce ) )
        Report.get( TimeInForce );    
    if( Report.isSetField( 10000 ) )
        TwseAccFlag = Report.getField( 10000 );    
    if( Report.isSetField( 10001 ) )
        TwseOrdType = Report.getField( 10001 );
    OrderNO  = OrderID.getString().c_str();
    ExecKey  = ExecID.getString().c_str();
    BrokerID = TargetSubID.getString().c_str();
    NID = UFC::AnsiString( ClOrdID.getString().c_str() + 2 ).ToInt();
    TransactTime.getValue().getYMD( Y, M, D );
    TransactTime.getValue().getHMS( h, m, s, ms );       
    if( (OR = FObjectManager->PopOrderRecord( NID ) ) == NULL )
         OR = FObjectManager->PopOrderRecord( ClOrdID.getString().c_str() ); ///< From SMP protocol , pop OrderRecord from ClOrdID
    if( OR != NULL || IsSMPEnable == TRUE || ExecType == FIX::ExecType_RESTATED )
    {
        ///< Fill O120 fields
        MessageO120       OddReply( FIsTWSE );

        memcpy( OddReply.PVCID,        FPVCID.c_str(), 2 );
        memcpy( OddReply.OrderNo,      OrderID.getString().c_str(), 5 );
        memcpy( OddReply.BrokerID,     BrokerID.c_str(), 4 );
        memcpy( OddReply.InvestorAcno, Account.getString().c_str(), 7 );
        OddReply.InvestorFlag = TWSEOrder::AccountFlagFIX2TMP( TwseAccFlag[0] );
        memcpy( OddReply.StockID, Symbol.getString().c_str(), 6 );
        OddReply.SetPrice( Price.getValue() );
        OddReply.SetQty( (int)OrderQty.getValue() ); 
        OddReply.BuySellCode = ( Side == FIX::Side_BUY )? 'B':'S';
        OddReply.ExchangeCode  = '2';
        OddReply.TWSEOrderType = '0';
        OddReply.OrderType     = OrdType.getValue();
        OddReply.TimeInForce   = TimeInForce.getValue();
        OddReply.SetDate( Y, M, D );
        OddReply.SetTime( ( h + 8 ) % 24, m, s, ms );            

        switch( ExecType )
        {
            case FIX::ExecType_NEW: ///< NewOrder.
                                    OddReply.SetBeforeQty( 0 );///< Before Qty = 0
                                    OddReply.SetAfterQty((int)LeavesQty.getValue() );///< After Qty = LeavesQty
                                    OddReply.SetStatusCode( 0 );
                                    if( Side == FIX::Side_BUY )
                                        OddReply.SetFunctionCode( 1 );
                                    else
                                        OddReply.SetFunctionCode( 2 );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &OddReply );
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                    if( FOwner->IsTrying() == TRUE )
                                        FOwner->HandleExchangeReplyTryMessage( &OddReply, *OR, mtO120, TRUE, FALSE, ExecKey, OR->Key );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &OddReply, *OR, 0, mtO120, TRUE, FALSE, ExecKey, OR->Key );
                                    break;
            case FIX::ExecType_REPLACE:  ///< Replaced. ( Not support replace price )                                       
                                    OddReply.SetBeforeQty( (int)(OrderQty.getValue() + LeavesQty.getValue()) );///< Before Qty = OrderQty + LeavesQty                                           
                                    OddReply.SetAfterQty( (int)LeavesQty.getValue() );///< After Qty = LeavesQty
                                    OddReply.SetStatusCode( 0 );
                                    OddReply.SetFunctionCode( 3 ); 
                                    OI->SetLeavesQty( (int)LeavesQty.getValue() );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &OddReply );
                                    if( IsSMPEnable == TRUE )
                                    {
                                        if( Side == FIX::Side_SELL )
                                            CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], true, false );
                                        else
                                            CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, true );
                                    }
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                    //UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_REPLACE OrderID[%s]", OrderNO.c_str() );
                                    if( FOwner->IsTrying() == TRUE )
                                        FOwner->HandleExchangeReplyTryMessage( &OddReply,*OR, mtO120,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &OddReply,*OR, OI->GetNID(), mtO120,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                    break;
            case FIX::ExecType_RESTATED:                                         
                                    OddReply.SetBeforeQty(  (int)OrderQty.getValue() );///< Before Qty = OrderQty
                                    OddReply.SetAfterQty((int)LeavesQty.getValue() );///< After Qty = LeavesQty
                                    OddReply.SetStatusCode( 51 );
                                    OddReply.SetFunctionCode( 4 );                                                                    
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &OddReply );                                    
                                    if( IsSMPEnable == TRUE )
                                    {
                                        if( Side == FIX::Side_SELL )
                                            CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], true, false );
                                        else
                                            CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, true );
                                    }
                                    FOwner->HandleExchangeReplyMessage( &OddReply, OI, mtO120, TRUE, TRUE, ExecKey, OI->GetKeyString(), "Unsolicited cancel by Exchange" );
                                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_RESTATED OrderID[%s] canceled by exchange.", OrderNO.c_str() );                                                     
                                    break;
            case FIX::ExecType_CANCELED: ///< Canceled.                                               
                                    OddReply.SetBeforeQty( (int)OrderQty.getValue() ); ///< Before Qty = OrderQty                                                                               
                                    OddReply.SetAfterQty( (int)LeavesQty.getValue() ); ///< After Qty = LeavesQty
                                    OddReply.SetStatusCode( 0 );
                                    OddReply.SetFunctionCode( 4 );                                
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &OddReply );
                                    if( IsSMPEnable == TRUE )
                                    {
                                        if( Side == FIX::Side_SELL )
                                            CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], true, false );
                                        else
                                            CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, true );
                                    }
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                    if( FOwner->IsTrying() == TRUE )
                                        FOwner->HandleExchangeReplyTryMessage( &OddReply, *OR, mtO120, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &OddReply, *OR, OI->GetNID(), mtO120, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    break;
            case FIX::ExecType_REJECTED: ///< New order Reject.
                                    Report.get( Text ); ///< Format: 0000-String
                                    if( ProcessFIXRejectMsg(  Report, &OddReply, Msg ) == true )
                                    {                    
                                        OddReply.SetBeforeQty( 0 );                       ///< Before Qty = 000                                                  
                                        OddReply.SetAfterQty( (int)LeavesQty.getValue() );///< After Qty = LeavesQty                
                                        if( Side == FIX::Side_BUY )
                                            OddReply.SetFunctionCode( 1 );
                                        else
                                            OddReply.SetFunctionCode( 2 );
                                        if( OR == NULL && IsSMPEnable == TRUE )
                                            OR = GetSMPOrderRecord( OI, &OddReply );
                                        if( IsSMPEnable == TRUE )
                                        {
                                            if( Side == FIX::Side_SELL )
                                                CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], true, false );
                                            else
                                                CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, true );
                                        }
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                        if( FOwner->IsTrying() == TRUE )
                                            FOwner->HandleExchangeReplyTryMessage( &OddReply, *OR, mtO120, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                        else
                                            FOwner->HandleExchangeReplyMessage( &OddReply, *OR, OI->GetNID(), mtO120, TRUE, TRUE, ExecKey, OI->GetKeyString(), Msg );                                    
                                        if( OddReply.GetStatusCode() == 89 && FObjectManager->StopOnOverLimit() )
                                        {
                                            UFC::BufferedLog::Printf( " Disable Session[%s:%d:%s] [89]Error Over Limit", FLineBrokerID.c_str(), FPort, FPVCID.c_str());       
                                            FObjectManager->EnableConnection( FLineBrokerID, FPort, FPVCID, false );
                                        }
                                    }
                                    else
                                       UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_REJECTED OrderID[%s] without Tag 58.", OrderNO.c_str() );                                                     
                                    break;
            case FIX::ExecType_ORDER_STATUS: ///< Order ststus
                                    OddReply.SetBeforeQty( (int)OrderQty.getValue() );
                                    OddReply.SetAfterQty( (int)OrderQty.getValue() );
                                    OddReply.SetStatusCode( 0 );
                                    OddReply.SetFunctionCode( 5 );
                                    if( OR == NULL && IsSMPEnable == TRUE )
                                        OR = GetSMPOrderRecord( OI, &OddReply );
                                    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                    if( FOwner->IsTrying() == TRUE )
                                        FOwner->HandleExchangeReplyTryMessage( &OddReply, *OR, mtO120, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    else
                                        FOwner->HandleExchangeReplyMessage( &OddReply, *OR, 0, mtO120, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                    break;
            default:                UFC::BufferedLog::Printf( " [ExecutionReport] un-handled ExecType[%c]",ExecType.getValue() );
                                    break;
        }
        //delete OR; ///< delete object get from PopOrderRecord.
        OrderRecodPoolPtr->Recycle( OR );
    }
    else
        UFC::BufferedLog::Printf( " [ExecutionReport] Execution with out New,Cancel, OrderID[%s] BrokerID[%s]", OrderNO.c_str(), BrokerID.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::FIXConfirmPost( const FIX44::ExecutionReport& Report, OrderInfo* OI )
{
    FIX::TargetSubID  TargetSubID;
    FIX::OrderID      OrderID; //Req
    FIX::ClOrdID      ClOrdID; 
    FIX::ExecID       ExecID;  //Req *
    FIX::ExecType     ExecType;//Req
    FIX::OrdType      OrdType( FIX::OrdType_LIMIT );  
    FIX::TimeInForce  TimeInForce( FIX::TimeInForce_DAY );    
    FIX::Account      Account;
    FIX::Symbol       Symbol;//Req
    FIX::Side         Side;  //Req
    FIX::TransactTime TransactTime;//Req 
    FIX::OrderQty     OrderQty( 0.0 );
    FIX::Price        Price( 0.0 );
    FIX::LeavesQty    LeavesQty;//Req 
    //FIX::Text         Text; 
    string            TwseAccFlag( " " ); ///< 10000
    string            TwseOrdType( "0" ); ///< 10001
    string            TwseExCode( "0" );  ///< 10002
    UFC::AnsiString   Msg;
    UFC::AnsiString   OrderNO;
    UFC::AnsiString   BrokerID;
    UFC::AnsiString   ExecKey;
    int               Y, M, D, h, m, s, ms;
    int               NID;
    OrderRecord*      OR;
    
    Report.getHeader().get( TargetSubID );
    Report.get( OrderID );
    Report.get( ExecID );
    Report.get( ExecType );
    Report.get( Symbol );
    Report.get( Side );
    Report.get( TransactTime );
    Report.get( LeavesQty );
    if( Report.isSet( ClOrdID ) )
        Report.get( ClOrdID );
    if( Report.isSet( Account ) )
        Report.get( Account );
    if( Report.isSet( Price ) )
        Report.get( Price );    
    if( Report.isSet( OrderQty ) )
        Report.get( OrderQty );
    if( Report.isSet( OrdType ) )
        Report.get( OrdType );    
    if( Report.isSet( TimeInForce ) )
        Report.get( TimeInForce );        
    if( Report.isSetField( 10000 ) )
        TwseAccFlag = Report.getField( 10000 );
    if( Report.isSetField( 10001 ) )
        TwseOrdType = Report.getField( 10001 );
    if( Report.isSetField( 10002 ) )
        TwseExCode  = Report.getField( 10002 );    
    OrderNO  = OrderID.getString().c_str();
    ExecKey  = ExecID.getString().c_str();
    BrokerID = TargetSubID.getString().c_str();
    NID = UFC::AnsiString( ClOrdID.getString().c_str() + 2 ).ToInt();
    TransactTime.getValue().getYMD( Y,M,D );
    TransactTime.getValue().getHMS( h, m, s, ms );
    if( (OR = FObjectManager->PopOrderRecord( NID ) ) == NULL )
         OR = FObjectManager->PopOrderRecord( ClOrdID.getString().c_str() ); ///< From SMP protocol , pop OrderRecord from ClOrdID
    if( OR != NULL || IsSMPEnable == TRUE || ExecType == FIX::ExecType_RESTATED )
    {     
        ///< Fill T020 fields
         MessageP020       PostReply( FIsTWSE ); 

         memcpy( PostReply.PVCID,        FPVCID.c_str(), 2 );
         memcpy( PostReply.OrderNo,      OrderID.getString().c_str(), 5 );
         memcpy( PostReply.BrokerID,     BrokerID.c_str(), 4 );
         memcpy( PostReply.InvestorAcno, Account.getString().c_str(), 7 );
         PostReply.InvestorFlag = TWSEOrder::AccountFlagFIX2TMP( TwseAccFlag[0] );
         memcpy( PostReply.StockID,      Symbol.getString().c_str(), 6 );
         PostReply.SetPrice( Price.getValue() );
         PostReply.SetQty( (int)OrderQty.getValue() );                  
         PostReply.BuySellCode = ( Side == FIX::Side_BUY )? 'B':'S';
         PostReply.ExchangeCode  = TwseExCode[0];
         PostReply.TWSEOrderType = TwseOrdType[0];
         PostReply.OrderType     = OrdType.getValue();
         PostReply.TimeInForce   = TimeInForce.getValue();
         PostReply.SetDate( Y, M, D );
         PostReply.SetTime( (h + 8) % 24, m, s, ms );              

         switch( ExecType )
         {
             case FIX::ExecType_NEW: ///< NewOrder.
                                     ///< Before Qty = 000
                                     PostReply.SetBeforeQty( 0 );
                                     ///< After Qty = LeavesQty
                                     PostReply.SetAfterQty( (int)LeavesQty.getValue() );
                                     ProcessFIXErrorMsg( Report, &PostReply, Msg );
                                     if( Side == FIX::Side_BUY )
                                         PostReply.SetFunctionCode( 1 );
                                     else
                                         PostReply.SetFunctionCode( 2 );
                                     if( OR == NULL && IsSMPEnable == TRUE )
                                         OR = GetSMPOrderRecord( OI, &PostReply );
                                     UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );
                                     if( FOwner->IsTrying() == TRUE )
                                         FOwner->HandleExchangeReplyTryMessage( &PostReply, *OR, mtP020, TRUE, FALSE, ExecKey, OR->Key );
                                     else
                                         FOwner->HandleExchangeReplyMessage( &PostReply, *OR, 0, mtP020, TRUE, FALSE, ExecKey, OR->Key );
                                     break;
             case FIX::ExecType_REPLACE:  ///< Replaced. ( Not support replace price )
                                     ///< Before Qty = OrderQty + LeavesQty
                                     PostReply.SetBeforeQty( (int)(OrderQty.getValue() + LeavesQty.getValue()) );
                                     ///< After Qty = LeavesQty
                                     PostReply.SetAfterQty( (int)LeavesQty.getValue() );
                                     PostReply.SetStatusCode( 0 );
                                     PostReply.SetFunctionCode( 3 ); 
                                     OI->SetLeavesQty( (int)LeavesQty.getValue() );
                                     if( OR == NULL && IsSMPEnable == TRUE )
                                         OR = GetSMPOrderRecord( OI, &PostReply );                                     
                                     if( IsSMPEnable == TRUE )
                                     {
                                        if( Side == FIX::Side_SELL )
                                            CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], false, true );
                                        else
                                            CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, false );
                                     }                                        
                                     UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );
                                     if( FOwner->IsTrying() == TRUE )
                                         FOwner->HandleExchangeReplyTryMessage( &PostReply, *OR, mtP020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                     else
                                         FOwner->HandleExchangeReplyMessage( &PostReply, *OR, OI->GetNID(), mtP020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                     break;
             case FIX::ExecType_RESTATED: 
                                     PostReply.SetBeforeQty( (int)OrderQty.getValue() );
                                     PostReply.SetAfterQty( (int)LeavesQty.getValue() );
                                     PostReply.SetStatusCode( 51 );
                                     PostReply.SetFunctionCode( 4 );
                                     if( OR == NULL && IsSMPEnable == TRUE )
                                         OR = GetSMPOrderRecord( OI, &PostReply );
                                     if( IsSMPEnable == TRUE )
                                     {
                                        if( Side == FIX::Side_SELL )
                                            CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], false, true );
                                        else
                                            CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, false );
                                     }                                        
                                     FOwner->HandleExchangeReplyMessage( &PostReply, OI, mtT020, TRUE, TRUE, ExecKey, OI->GetKeyString(), "Unsolicited cancel by Exchange" );
                                     UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_RESTATED OrderID[%s] canceled by exchange.", OrderNO.c_str() );
                                     break;
             case FIX::ExecType_CANCELED: ///< Canceled.       
                                     ///< Before Qty = OrderQty
                                     PostReply.SetBeforeQty( (int)OrderQty.getValue() );
                                     ///< After Qty = LeavesQty
                                     PostReply.SetAfterQty( (int)LeavesQty.getValue() );
                                     PostReply.SetStatusCode( 0 );
                                     PostReply.SetFunctionCode( 4 );  
                                     if( OR == NULL && IsSMPEnable == TRUE )
                                         OR = GetSMPOrderRecord( OI, &PostReply );
                                     if( IsSMPEnable == TRUE )
                                     {
                                        if( Side == FIX::Side_SELL )
                                            CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], false, true );
                                        else
                                            CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, false );
                                     }                                        
                                     UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                     if( FOwner->IsTrying() == TRUE )
                                         FOwner->HandleExchangeReplyTryMessage( &PostReply, *OR, mtP020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                     else
                                         FOwner->HandleExchangeReplyMessage( &PostReply, *OR, OI->GetNID(), mtP020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                     break;
             case FIX::ExecType_REJECTED: ///< New order Reject.
                                     if( ProcessFIXRejectMsg(  Report, &PostReply, Msg ) == true )
                                     {
                                         PostReply.SetBeforeQty( 0 );  ///< Before Qty = 000      
                                         PostReply.SetAfterQty( (int)LeavesQty.getValue() );
                                         if( Side == FIX::Side_BUY )
                                             PostReply.SetFunctionCode( 1 );
                                         else
                                             PostReply.SetFunctionCode( 2 );
                                         if( OR == NULL && IsSMPEnable == TRUE )
                                             OR = GetSMPOrderRecord( OI, &PostReply );
                                         if( IsSMPEnable == TRUE )
                                         {
                                            if( Side == FIX::Side_SELL )
                                                CheckPositionRelease( ClOrdID, Symbol, OrderID, OrderQty, TwseOrdType[0], false, true );
                                            else
                                                CheckFundRelease( Account, Symbol, OrderID, Price, OrderQty, false );
                                         }                                        
                                         UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );
                                         if( FOwner->IsTrying() == TRUE )
                                             FOwner->HandleExchangeReplyTryMessage( &PostReply, *OR, mtP020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                         else
                                             FOwner->HandleExchangeReplyMessage( &PostReply, *OR, OI->GetNID(), mtP020, TRUE, TRUE, ExecKey, OI->GetKeyString(), Msg );
                                         if( PostReply.GetStatusCode() == 89 && FObjectManager->StopOnOverLimit() )
                                         {
                                             UFC::BufferedLog::Printf( " Disable Session[%s:%d:%s] [89]Error Over Limit", FLineBrokerID.c_str(), FPort, FPVCID.c_str());       
                                             FObjectManager->EnableConnection( FLineBrokerID, FPort, FPVCID, false );
                                         }                                        
                                     }
                                     else
                                        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_REJECTED OrderID[%s] without Tag 58.", OrderNO.c_str() );                                                     
                                     break;                
             case FIX::ExecType_ORDER_STATUS: ///< Order ststus
                                     PostReply.SetBeforeQty( (int)OrderQty.getValue() );
                                     PostReply.SetAfterQty( (int)OrderQty.getValue() );
                                     PostReply.SetStatusCode( 0 );
                                     PostReply.SetFunctionCode( 5 ); 
                                     if( OR == NULL && IsSMPEnable == TRUE )
                                         OR = GetSMPOrderRecord( OI, &PostReply );
                                     UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                     if( FOwner->IsTrying() == TRUE )
                                         FOwner->HandleExchangeReplyTryMessage( &PostReply, *OR, mtP020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                     else
                                         FOwner->HandleExchangeReplyMessage( &PostReply, *OR, 0, mtP020, TRUE, TRUE, ExecKey, OI->GetKeyString() );
                                     break;       
             default:                UFC::BufferedLog::Printf( " [ExecutionReport] un-handled ExecType[%c]", ExecType.getValue() );       
                                     break;     
         }       
        //delete OR; ///< delete object get from PopOrderRecord.
        OrderRecodPoolPtr->Recycle( OR );
    }
    else
        UFC::BufferedLog::Printf( " [ExecutionReport] Execution without New,Cancel, OrderID[%s] BrokerID[%s]", OrderNO.c_str(), BrokerID.c_str() );    
        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
OrderInfo*  SpeedyFIXOrderClient::AddNewOrderInfo( const UFC::AnsiString& OrderNO, const UFC::AnsiString& BrokerID, int TSessoin, const FIX44::ExecutionReport& Report )
{
#ifdef __EXTREME_VERSION       
    Int32           Msgtype;
    FIX::ClOrdID    ClOrdID; 
    UFC::AnsiString Key;

    Report.get( ClOrdID );            
    UFC::AnsiString AE( ClOrdID.getString().c_str() + 1, 4 );        
    Key.Printf( "%s,%s", AE.c_str(), AE.c_str() );                
    switch( TSessoin )
    {
         case '0': Msgtype = TWSE::mtT010; break;
         case '2': Msgtype = TWSE::mtO010; break; 
         case 'C': Msgtype = TWSE::mtO010; break;            
         case '7': Msgtype = TWSE::mtP010; break; 
         default:  Msgtype = TWSE::mtT010;  break;
    }                 
    return FObjectManager->OrderInfo.AddOrder( OrderNO, BrokerID, 1 , Msgtype, FSMPGateway->GenerateNID( 'D', 1 ), Key, UFC::Hostname );    
#else
     return NULL;
#endif     
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::onMessage( const FIX44::ExecutionReport& Report )
{
    FIX::TargetSubID  TargetSubID;///< Req    
    FIX::SenderSubID  SenderSubID;///< Req
    FIX::OrderID      OrderID;    ///< Req
    FIX::ExecType     ExecType;   ///< Req
    UFC::AnsiString   OrderNO,BrokerID;
    char              TSessoin;     
    OrderInfo*        OI;                
    
    Report.getHeader().get( TargetSubID );              
    Report.getHeader().get( SenderSubID );               
    Report.get( OrderID );    
    Report.get( ExecType );        
    OrderNO  = OrderID.getString().c_str();
    BrokerID = TargetSubID.getString().c_str();
    TSessoin = SenderSubID.getString()[0];    
    ///< Get OrderInfo object.
    if( FObjectManager->OrderInfo.GetOrderInfo( OrderNO, BrokerID, OI ) == FALSE )        
    {
#ifdef __EXTREME_VERSION               
        if( IsSMPEnable == TRUE  &&  FSMPGateway != NULL ) 
        {
            UFC::BufferedLog::Printf( " [ExecutionReport] Can't not get OrderID[%s] BrokerID[%s] create a new one.", OrderNO.c_str(), BrokerID.c_str() );    
            OI = AddNewOrderInfo(  OrderNO, BrokerID, TSessoin, Report );
        }
        else
#endif                 
        {
            UFC::BufferedLog::Printf( " [ExecutionReport] Can't not get OrderID[%s] BrokerID[%s]", OrderNO.c_str(), BrokerID.c_str() );    
            return;
        }
    }
    ///< Order Confirm,Regect,Canceled,Replaced Execution report,
     if( ExecType != FIX::ExecType_TRADE )      
     {
         switch( TSessoin )
         {
             case '0': FIXConfirmNormal( Report, OI ); break;
             case '2': FIXConfirmOdd( Report, OI ); break; 
             case 'C': FIXConfirmOddEx( Report, OI ); break;            
             case '7': FIXConfirmPost( Report, OI ); break; 
             case '4': FIXConfirmLend( Report, OI ); break; 
             case '5': FIXConfirmAuction( Report, OI ); break; 
             case '6': FIXConfirmTender( Report, OI ); break; 
             case 'B': FIXConfirmTenderEx( Report, OI ); break; 
             default:  UFC::BufferedLog::Printf( " [ExecutionReport] Not support trading session[%c]", TSessoin );  break;
         }             
     }  
     else ///< Order Fill Execution report         
         FIXFill( Report, OI );  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::onMessage( const FIX44::OrderCancelReject& Reject )
{
    FIX::TargetSubID      TargetSubID;
    FIX::SenderSubID      SenderSubID;
    FIX::OrderID          OrderID; //Req
    FIX::ClOrdID          ClOrdID; //Req
    FIX::TransactTime     TransactTime;
    FIX::Text             Text; 
    FIX::CxlRejResponseTo CxlRejResponseTo;//Req 
    UFC::AnsiString       X020,OrderNO,BrokerID,ErrMsg;
    UFC::AnsiString       ExecKey( "CxlRej" );    
    int                   Y,M,D,h,m,s,ms;
    Int32                 NID,ErrorCode;
    OrderInfo*            OI;
    OrderRecord*          OR;    
      
    Reject.getHeader().get( TargetSubID );
    Reject.getHeader().get( SenderSubID );           
    Reject.get( ClOrdID );    
    Reject.get( OrderID );
    Reject.get( CxlRejResponseTo );
    if( Reject.isSet( TransactTime ) )
        Reject.get( TransactTime );    
    if( Reject.isSet( Text ) )
        Reject.get( Text );      
    OrderNO   = OrderID.getString().c_str();
    BrokerID  = TargetSubID.getString().c_str();
    NID       = UFC::AnsiString( ClOrdID.getString().c_str() + 2 ).ToInt();
    TransactTime.getValue().getYMD( Y,M,D );
    TransactTime.getValue().getHMS( h,m,s,ms );            
    if( Text.getString().length() < 5 )
    {
        ErrorCode = 96;        
        UFC::BufferedLog::Printf( " [OrderCancelReject] OrderID[%s] BrokerID[%s] miss tag(58) Text.", OrderNO.c_str(), BrokerID.c_str());    
        ErrMsg = "Missing tag(58) Text.";
    }
    else
    {
        ErrorCode = UFC::AnsiString( Text.getString().c_str() + 2, 2 ).ToInt();       
        ErrMsg = UFC::AnsiString( Text.getString().c_str() + 5 );       
    }
    if( ErrorCode > 99 || ErrorCode == 0 ) 
    {
        UFC::BufferedLog::Printf( " [OrderCancelReject] Error Code > 99 OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
        ErrorCode = 98;        
    }
    if( (OR = FObjectManager->PopOrderRecord( NID ) ) == NULL )
         OR = FObjectManager->PopOrderRecord( ClOrdID.getString().c_str() ); ///< From SMP protocol , pop OrderRecord from ClOrdID    
    if( FObjectManager->OrderInfo.GetOrderInfo( OrderNO, BrokerID, OI ) == TRUE && OR != NULL )
    {      
        if( OrderNO.AnsiCompare( OR->GetOrderID() ) == 0 ) ///< Normal
        {            
            X010ToX020( OR->GetOrder(), ErrorCode, X020, TWSE::MessageHeader::FNewVersion , TWSE::MessageHeader::FNewExVersion );        
            
            if( SenderSubID.getString() == "0" ) ///< Normal
            {
                MessageT020  Reply( X020.c_str() );
                
                Reply.SetDate( Y,M,D );                
                Reply.SetTime( (h+8)%24, m, s, ms );
                if( CxlRejResponseTo == FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST )
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Cancel OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 4 );
                }
                else
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Replace OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    UFC::AnsiString ID =  UFC::AnsiString( ClOrdID.getString().c_str(), 2 );
                    if( ID == "RX" ) ///< Change Price
                        Reply.SetFunctionCode( 6 ); 
                    else    
                        Reply.SetFunctionCode( 3 ); 
                }                
                UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                FOwner->HandleExchangeReplyMessage( &Reply,*OR, 0, mtT020,FALSE,TRUE,ExecKey,OI->GetKeyString(), ErrMsg);                                            
            }
            else if( SenderSubID.getString() == "C" ) ///< Intraday Odd 
            { 
                MessageO120  Reply( X020.c_str() );           

                Reply.SetDate( Y,M,D );
                Reply.SetTime( (h+8)%24, m, s, ms );                
                if( CxlRejResponseTo == FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST )
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Cancel OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 4 );
                }
                else
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Replace OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 3 );
                }
                UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                FOwner->HandleExchangeReplyMessage( &Reply,*OR,  0, mtO120,FALSE,TRUE,ExecKey,OI->GetKeyString(), ErrMsg );                                            
            }     
            else if( SenderSubID.getString() == "2" )  ///< Odd
            { 
                MessageO020  Reply( X020.c_str() );           

                Reply.SetDate( Y,M,D );
                Reply.SetTime( (h+8)%24, m, s, ms );                
                if( CxlRejResponseTo == FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST )
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Cancel OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 4 );
                }
                else
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Replace OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 3 );
                }
                UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                FOwner->HandleExchangeReplyMessage( &Reply,*OR, 0, mtO020,FALSE,TRUE,ExecKey,OI->GetKeyString(), ErrMsg );                                            
            } 
            else if( SenderSubID.getString() == "7" )  ///< Post 
            {
                MessageP020  Reply( X020.c_str() );
            
                Reply.SetDate( Y,M,D );
                Reply.SetTime( (h+8)%24, m, s, ms );
                if( CxlRejResponseTo == FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST )
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Cancel OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 4 );
                }
                else
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Replace OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 3 );
                }
                UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                FOwner->HandleExchangeReplyMessage( &Reply,*OR,  0, mtP020,FALSE,TRUE,ExecKey,OI->GetKeyString(), ErrMsg );                                            
            }    
            else if( SenderSubID.getString() == "4" )  ///< Lend 
            {
                MessageV020  Reply( X020.c_str() );
            
                Reply.SetDate( Y,M,D );
                Reply.SetTime( (h+8)%24, m, s, ms );
                if( CxlRejResponseTo == FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST )
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Cancel OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 4 );
                }
                else
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Replace OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 3 );
                }
                UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                FOwner->HandleExchangeReplyMessage( &Reply,*OR,  0, mtP020,FALSE,TRUE,ExecKey,OI->GetKeyString(), ErrMsg );                                                            
            }
            else if( SenderSubID.getString() == "5" ) ///< Auction
            {
                MessageA020  Reply( X020.c_str() );
            
                Reply.SetDate( Y,M,D );
                Reply.SetTime( (h+8)%24, m, s, ms );
                if( CxlRejResponseTo == FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST )
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Cancel OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 4 );
                }
                else
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Replace OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 3 );
                }
                UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                FOwner->HandleExchangeReplyMessage( &Reply,*OR,  0, mtP020,FALSE,TRUE,ExecKey,OI->GetKeyString(), ErrMsg );        
            }
            else if( SenderSubID.getString() == "6" )///< Tender
            {
                MessageE020  Reply( X020.c_str() );
            
                Reply.SetDate( Y,M,D );
                Reply.SetTime( (h+8)%24, m, s, ms );
                if( CxlRejResponseTo == FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST )
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Cancel OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 4 );
                }
                else
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Replace OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 3 );
                }
                UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                FOwner->HandleExchangeReplyMessage( &Reply,*OR,  0, mtP020,FALSE,TRUE,ExecKey,OI->GetKeyString(), ErrMsg );                        
            }
            else if( SenderSubID.getString() == "B" )///< Tender Ex
            {
                MessageEx020  Reply( X020.c_str() );
            
                Reply.SetDate( Y,M,D );
                Reply.SetTime( (h+8)%24, m, s, ms );
                if( CxlRejResponseTo == FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST )
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Cancel OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 4 );
                }
                else
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [OrderCancelReject] Reject Replace OrderID[%s] BrokerID[%s] Text[%s]", OrderNO.c_str(), BrokerID.c_str(),Text.getString().c_str() );    
                    Reply.SetFunctionCode( 3 );
                }
                UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                FOwner->HandleExchangeReplyMessage( &Reply,*OR,  0, mtP020,FALSE,TRUE,ExecKey,OI->GetKeyString(), ErrMsg );                        
            }
            else
               UFC::BufferedLog::Printf( " [ExecutionReport] Not support trading session[%s]", SenderSubID.getString().c_str() );    
        }
        else
            UFC::BufferedLog::Printf( " *** [OrderCancelReject] OrderID not match Should be[%s] but[%s] NID[%d] ***", OrderNO.c_str(), OR->GetOrderID()/*.c_str()*/, NID );
        OrderRecodPoolPtr->Recycle( OR );        
    }    
    else
        UFC::BufferedLog::Printf( " [OrderCancelReject] Can't not get OrderID[%s] BrokerID[%s]", OrderNO.c_str(), BrokerID.c_str() );    
        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::onMessage( const FIX44::BusinessMessageReject& Reject )
{
    FIX::RefSeqNum  RefSeqNum;
    FIX::RefMsgType RefMsgType;
    FIX::Text       Text;
    
    Reject.get( RefMsgType );     
    if( Reject.isSet( RefSeqNum ) )
        Reject.get( RefSeqNum ); 
    if( Reject.isSet( Text ) )
        Reject.get( Text );     
    UFC::BufferedLog::Printf( " [BusinessMessageReject] Reject MsgType[%s] Seq[%d] Msg[%s]",
                              RefMsgType.getString().c_str(), 
                              RefSeqNum.getValue(),
                              Text.getString().c_str() );            
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  SpeedyFIXOrderClient::SetTradeSession( char APCode, Int32 SubSystem )
{
    FSubSystem = SubSystem;
    UFC::BufferedLog::Printf( " SpeedyOrderClient::Change to AP code:%c SubSystem: %d", APCode, FSubSystem );    
    if( APCode != FTargetSubID[ 0 ] )
        FTargetSubID[0] = APCode;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MarketSchedule* SpeedyFIXOrderClient::GetMarket( void )
{
    return FObjectManager->GetScheduleObjectBySubsystemID( FSubSystem );
}        
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 SpeedyFIXOrderClient::GetStatus()
{
    if( FEnabled == false )
        return PVC_STATUS_DISABLE;
    if( FSession->isLoggedOn() )
    {
        if( FStatus == PVC_STATUS_TRY_OPEN )
            return PVC_STATUS_TRY_OPEN;
        else if( FStatus == PVC_STATUS_WAIT_OPEN )
            return PVC_STATUS_WAIT_OPEN;
        else
            return PVC_STATUS_ONLINE_READY;
    }
    else
    {
        if( FStatus == PVC_STATUS_SLEEPING )
            return PVC_STATUS_SLEEPING;
        else
        return PVC_STATUS_BROKEN;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::SetStatus( Int32 Status)
{
    if( Status == PVC_STATUS_DISABLEING )
    {
        FEnabled = FALSE;
        FSession->logout( "User disable this Session" );
        FSession->disconnect();
    }
    else if( Status == PVC_STATUS_ENABLE )
    {
        FEnabled = TRUE;        
        FSession->logon();
    }
    FStatus = Status;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::Listen( BOOL IsListen )
{    
    if( IsListen == TRUE )
    {
        FObjectManager->GetFIXManager()->SetIsMonitoring( TRUE );
        FListener = FOwner;
    }
    else
    {
        FObjectManager->GetFIXManager()->SetIsMonitoring( FALSE );
        FListener = NULL; 
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::onMessage( BOOL IsSend, const FIX::Message& Msg )
{
    if( FListener != NULL )
    {
        ThreadClock::GetTimeString( FLogBuffer, TRUE, TRUE );
        string FIXMsg( Msg.toString() );

        if( IsSend == TRUE )
            strcat( FLogBuffer, FLinkPVCSendStr );
        else
            strcat( FLogBuffer, FLinkPVCRecvStr );                               
        strncat( FLogBuffer, FIXMsg.c_str(), FIXMsg.length() );
        if( IsSend == TRUE )
            FListener->OnCommand( EVENT_PVC_WRITE, (void*)FLogBuffer);
        else
            FListener->OnCommand( EVENT_PVC_READ, (void*)FLogBuffer);        
    }     
}
//-----------------------------------------------------------------------------------------
ConnectionObjectBase* SpeedyFIXOrderClient::GetOwner( void ) 
{ 
    return FOwner; 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




