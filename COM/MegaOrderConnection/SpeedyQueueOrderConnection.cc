#include "SpeedyQueueOrderConnection.h"
#include "iniFile.h"
#include "Utility.h"
//------------------------------------------------------------------------------
extern BOOL                  DumpOnly;
extern TConnectionStateEnum  GConnectionState;
extern Int32                 FGroup;
const Int32 MSG_LENGTH       = 276;  ///< New order message length. (75 + 1 + 10 + 180 + 9 + 1 )
const Int32 TWSE_EXEC_LENGTH = 321;  ///< Execution report length.  (120 + 1 + 10 + 180 + 9 + 1 )
const Int32 TFX_EXEC_LENGTH  = 409;  ///< Execution report length.  (200 + 1 + 10 + 180 + 3 + 5 +9 + 1 ) FLEX Add PartID 9(3) status_code 9(5)
const Int32 TFX_FILL_LENGTH  = 329;  ///< Execution report length.  (120 + 1 + 10 + 180 + 3 + 5 +9 + 1 )
const Int32 UDD_LENGTH       = 180;  ///< User define length
const Int32 DATA_LENGTH      = 1024; ///< Size of execution in queue
//---------------------------------------------------------------------------
const UFC::AnsiString PROXY_PREFIX( "<Proxy:" );
const int             PROXY_PREFIX_LEN = PROXY_PREFIX.Length(); ///< 7
const int             PROXY_NID_LEN      = 10;
const int             PROXY_MINUS_AE_LEN = PROXY_PREFIX_LEN + PROXY_NID_LEN;
//------------------------------------------------------------------------------
SpeedyQueueOrderConnection::SpeedyQueueOrderConnection( TTaifexConnection* Dispatcher, 
						        key_t OrderKey, 
						        key_t TSEConfirmKey, key_t TSEFillKey,
						        key_t OTCconfirmKey, key_t OTCFillKey,                                                  
                                                        key_t TFXExecKey )
:UFC::PThread( NULL, FALSE )
,FConnection( Dispatcher )
,FOrderQueue( FALSE )
,FTSEConfirmQueue( FALSE )
,FTSEFillQueue( FALSE )
,FOTCConfirmQueue( FALSE )
,FOTCFillQueue( FALSE )
,FTFXExecQueue( FALSE )
{    
    if( !FOrderQueue.Open( OrderKey, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open order queue 0x%x failed.", OrderKey );
        exit( 0 );
    }    
    if( !FTSEConfirmQueue.Open( TSEConfirmKey, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open TWSE Confirm queue 0x%x failed.", TSEConfirmKey );
        exit( 0 );
    }        
    if( !FTSEFillQueue.Open( TSEFillKey, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open TWSE Filled queue 0x%x failed.", TSEFillKey );
        exit( 0 );
    }        
    if( !FOTCConfirmQueue.Open( OTCconfirmKey, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open OTC Confirm queue 0x%x failed.", OTCconfirmKey );
        exit( 0 );
    }        
    if( !FOTCFillQueue.Open( OTCFillKey, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open OTC Filled queue 0x%x failed.", OTCFillKey );
        exit( 0 );
    }        
    if( !FTFXExecQueue.Open( TFXExecKey, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open TAIFEX Execution queue 0x%x failed.", TFXExecKey );
        exit( 0 );
    }        
    Start();
}
//---------------------------------------------------------------------------
bool GetUserData( const UFC::AnsiString& UserData, UFC::AnsiString& Data )
{
    if( UserData.Length() >= 21 ) ///< Need 21 bytes "<Proxy:"(7) + AE(>3) + NID(10) + ">"(1)
    {
        Int32 RightQuote = UserData.AnsiPos( ">" );

        if( UserData.AnsiNCompare( PROXY_PREFIX, PROXY_PREFIX_LEN ) == 0 && RightQuote > PROXY_PREFIX_LEN )
        {       
            if( UserData.Length() > RightQuote + 1 ) ///< Has User Define                         
                Data = UFC::AnsiString( UserData.c_str() + RightQuote + 1 );
            else          
                Data = ""; ///<  No User Data
            return true;            
        }
    }
    Data = UserData;
    return false;
}
//------------------------------------------------------------------------------
nsOrderMessageDefine::TradingSessionIDEnum GetTradingSession( char ch )
{
        switch(ch)
        {
            case '0':return nsOrderMessageDefine::tsNormal;
            case '1':return nsOrderMessageDefine::tsOffHour;
            default :return nsOrderMessageDefine::tsAuto;
        }
}
//------------------------------------------------------------------------------
nsOrderMessageDefine::MarketEnum GetMarket( char ch )
{
        switch(ch)
        {
            case 'T':return nsOrderMessageDefine::mTSE;
            case 'O':return nsOrderMessageDefine::mOTC;
            case 'E':return nsOrderMessageDefine::mES;            
            case 'F':return nsOrderMessageDefine::mTWFutures;
            case 'P':return nsOrderMessageDefine::mTWOptions;
            default :return nsOrderMessageDefine::mTSE;
        }
}
//------------------------------------------------------------------------------
char GetMarketCh( nsOrderMessageDefine::MarketEnum Market )
{
        switch(Market)
        {
            case nsOrderMessageDefine::mTSE: return 'T';
            case nsOrderMessageDefine::mOTC: return 'O';
            case nsOrderMessageDefine::mES: return 'E';
            case nsOrderMessageDefine::mTWFutures: return 'F';
            case nsOrderMessageDefine::mTWOptions: return 'P';
            default :return 'T';
        }
}
//------------------------------------------------------------------------------
const char* GetMarketName( nsOrderMessageDefine::MarketEnum Market )
{
        switch(Market)
        {
            case nsOrderMessageDefine::mTSE: return "TSE";
            case nsOrderMessageDefine::mOTC: return "OTC";
            case nsOrderMessageDefine::mES:  return "ES";
            case nsOrderMessageDefine::mTWFutures: return "FUT";
            case nsOrderMessageDefine::mTWOptions: return "OPT";
            default :return "XXX";
        }
}
//------------------------------------------------------------------------------
SpeedyQueueOrderConnection::~SpeedyQueueOrderConnection()
{
    
}
//------------------------------------------------------------------------------
//  Length 276
//
//  R010   (75)   0-74
//  Market (1)    75            T:TWSE   O: OTC  F:Futures P:Options
//  NID    (10)   76-85
//  Data   (180)) 86-265
//  Time   (9)    266-274
//------------------------------------------------------------------------------
void SpeedyQueueOrderConnection::SendOrder( void )
{
    UFC::UQueueData OrderData( 0, DATA_LENGTH );	
    char* Buffer;

    try ///< Dequeue an execution from Speedy.
    {   ///< Get order data from queue. 	
	BOOL  IsQuoteRequest = FALSE;
        OrderData.SetMType( 0 );
	FOrderQueue.Recv( OrderData );	            
	Buffer = (char*)OrderData.GetData();
        
        UFC::AnsiString                  HeaderStr( Buffer, 6 );       
        UFC::AnsiString                  Order( Buffer , MSG_LENGTH );      ///< Total: 276	
        nsOrderMessageDefine::MarketEnum Market = GetMarket( Order[ 75 ] ); ///< Market length 1.
        UFC::AnsiString                  NIDStr( Buffer + 76, 10 );	    ///< NID Length 10.
        UFC::AnsiString                  UserData( Buffer + 86, 180 );      ///< UserDef Length 180.
	UFC::AnsiString                  MessageTime( Buffer + 266, 9 );    ///< Time Length 9  
        
        if( HeaderStr == "300106" )
            IsQuoteRequest = TRUE;
        UFC::BufferedLog::DebugPrintf( " Read[%s]",        Buffer );                	            
        UFC::BufferedLog::DebugPrintf( " Header[%s]",      HeaderStr.c_str() );                	            
	UFC::BufferedLog::DebugPrintf( " MessageTime[%s]", MessageTime.c_str() );                        	
	UFC::BufferedLog::DebugPrintf( " Market[%c]",      Order[ 75 ] );        ///< T,O,F,P
	UFC::BufferedLog::DebugPrintf( " UserData[%s]",    UserData.c_str() );
	if( GConnectionState == csReady  )
	{
            if( IsQuoteRequest == FALSE )
            {
                UFC::AnsiString R010( Buffer, 75  );                                ///< length of R010 is 75.                    
                
                UFC::BufferedLog::DebugPrintf( " R010[%s]",  R010.c_str() );            
                if( Order[3] == '1' || Order[3] == '2') ///< New order
                {
                    TNewOrderMessage NewOrder;

                    NewOrder.SetMarket( Market );                        
                    NewOrder.SetSrc( R010.c_str() );
                    NewOrder.SetData( UserData.c_str() );		    
                    NewOrder.SetGroup( FGroup );
                    if( Market == nsOrderMessageDefine::mTWFutures || Market == nsOrderMessageDefine::mTWOptions )
                        NewOrder.SetTradingSessionID(  GetTradingSession( UserData[179]) );
                    FConnection->NewOrder( &NewOrder );                            
                }
                else if( Order[3] == '3' ) ///< Reduce Qty
                {
                    TReplaceOrderMessage Replace;

                    Replace.SetMarket( Market );
                    Replace.SetSrc( R010.c_str() );
                    Replace.SetData( UserData.c_str() );		    
                    Replace.SetGroup( FGroup );
                    if( Market == nsOrderMessageDefine::mTWFutures || Market == nsOrderMessageDefine::mTWOptions )
                        Replace.SetTradingSessionID(  GetTradingSession( UserData[179]) );
                    FConnection->ReplaceOrder( &Replace );
                }
                else if( Order[3] == '4' )///< Order Cancel
                {
                    TCancelOrderMessage Cancel;

                    Cancel.SetMarket( Market );
                    Cancel.SetSrc( R010.c_str() );
                    Cancel.SetData( UserData.c_str() );
                    Cancel.SetGroup( FGroup );
                    if( Market == nsOrderMessageDefine::mTWFutures || Market == nsOrderMessageDefine::mTWOptions )
                        Cancel.SetTradingSessionID(  GetTradingSession( UserData[179]) );
                    FConnection->CancelOrder( &Cancel );
                }
                else if( Order[3] == '5' )///< Order Status request
                {
                    TOrderStatusRequest Request;                                               

                    Request.SetMarket( Market );                        
                    Request.SetSrc( R010.c_str() );
                    Request.SetData( UserData.c_str() );
                    FConnection->OrderStatusRequest( &Request );
                }                        				
                else if( Order[3] == '6' ) ///< Replace Price
                {
                    TReplaceOrderMessage Replace;

                    Replace.SetMarket( Market );
                    Replace.SetSrc( R010.c_str() );
                    Replace.SetData( UserData.c_str() );		    
                    Replace.SetGroup( FGroup );
                    if( Market == nsOrderMessageDefine::mTWFutures || Market == nsOrderMessageDefine::mTWOptions )
                        Replace.SetTradingSessionID(  GetTradingSession( UserData[179]) );
                    FConnection->ReplaceOrder( &Replace );
                }
            }
            else
            {
                UFC::AnsiString R070( Buffer, 46  );                 ///< length of R070 is 46.                    

                UFC::BufferedLog::DebugPrintf( " R070[%s]",        R070.c_str() );            
                TQuoteRequestMessage QuoteRequest;

                QuoteRequest.SetMarket( Market );
                QuoteRequest.SetSrc( R070.c_str() );
                QuoteRequest.SetData( UserData.c_str() );		                    
                if( Market == nsOrderMessageDefine::mTWFutures || Market == nsOrderMessageDefine::mTWOptions )
                    QuoteRequest.SetTradingSessionID(  GetTradingSession( UserData[179]) );
                FConnection->QuoteRequest( &QuoteRequest );
            }        
        }
        else
        {    
            if( IsQuoteRequest == FALSE )
            {
                UFC::AnsiString R010( Buffer, 75  );                                ///< length of R010 is 75.                    
                RejectToBackend( 0, R010, UserData, Market );        
            }
            else
            {
                UFC::AnsiString R070( Buffer, 46  );                                ///< length of R010 is 75.                    
                RejectR070Backend( 0, R070, UserData, Market );        
            }
        }
    }        
    catch( UFC::Exception& e )
    {
	UFC::BufferedLog::Printf( " OrderQueue.Recv Exception:%s", e.what() );
	UFC::BufferedLog::Printf( " MType:%d Len:%d", OrderData.GetMType(), OrderData.GetSize() );
    }	
}
//------------------------------------------------------------------------------
void SpeedyQueueOrderConnection::Execute( void )
{
    while( TRUE )
    {            
        try
        {
            if( FConnection->IsLogon() == TRUE )
                SendOrder( );	    
            else
                UFC::SleepMS( 1000 );
        }   
        catch( UFC::Exception& ex )
        {
            UFC::BufferedLog::Printf( "Exception:[%s]", ex.what() );
        }        
    }
}
//------------------------------------------------------------------------------
//
// TWSE/OTC Length 321
// Exchange Filled(120) +Market(1) + NID(10)  + UserDefine(180) + Time(9) + End(1) = 321
//
// TAIFEX Length 409
// Exchange Filled(200) +Market(1) + NID(10)  + UserDefine(180) + PartiID(3) + status_code(5) + Time(9) + End(1) = 409
//------------------------------------------------------------------------------
void SpeedyQueueOrderConnection::FillToBackend( TExecutionReportMessage* Msg )
{
    char            ExecData[ DATA_LENGTH ];
    UFC::AnsiString Body( Msg->GetSrc() );
    UFC::AnsiString TFXMsg,Header;
    UFC::AnsiString UserDef;
    UFC::AnsiString NIDStr, MessageTime,MatchTime;    
    UFC::AnsiString LineBroker;
    char            MarketC = GetMarketCh( Msg->GetMarket() );    
    int             MsgLen;

    GetUserData( Msg->GetData(), UserDef );    
    memset( ExecData, 0, DATA_LENGTH );
    if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures || Msg->GetMarket() == nsOrderMessageDefine::mTWOptions )
    {
        if( Body[0] == '1' )///< Single filled
            MatchTime = Body.SubString( 61, 6 );
        else if( Body[0] == '2' )///< Multileg filled 
            MatchTime = Body.SubString( 109, 6 );
        else  ///< Canceled,Replaced.
            MatchTime = Body.SubString( 57, 6 );            

        if( Msg->GetTMPStstusCode() == 47 || Msg->GetTMPStstusCode() == 48 )
            Header.Printf("500002%s%02d%s01",MatchTime.c_str(),Msg->GetTMPStstusCode(), Msg->GetBrokerID());
        else
            Header.Printf("500002%s00%s01",MatchTime.c_str(), Msg->GetBrokerID());    
        TFXMsg.Printf("%s%s%04d%04d", Header.c_str(), Body.c_str(), Msg->GetBeforeQty(), Msg->GetLeavesQty());    
        TFXMsg.PadThis( 200, ' ' );                     ///< 200 
        NIDStr.Printf( "%010u",(UInt32) Msg->GetNID() );///< 10
        LineBroker = Msg->GetPVC();
        LineBroker.PadThis( 3, '0' );                   ///< 3
        UserDef.PadThis( UDD_LENGTH - 3, ' ' );         ///< 177    
        if( Msg->GetTradingSessionID( ) == nsOrderMessageDefine::tsOffHour )
            UserDef[ UserDef.Length() -1 ] = '1';
        else
            UserDef[ UserDef.Length() -1 ] = '0';
        MsgLen = TFX_EXEC_LENGTH;                           ///< Length :409
        UFC::GetTimeString( MessageTime, FALSE );  ///< 9            
        sprintf( ExecData, "%s%c%s%s%s%03d%05d%s", TFXMsg.c_str(), MarketC, NIDStr.c_str(), LineBroker.c_str(), UserDef.c_str(), Msg->GetTMPPartID(), Msg->GetTMPStstusCode(), MessageTime.c_str() );
    }
    else
    {
        Body.PadThis( 120, ' ' );                       ///< 120    
        TFXMsg = Body; 
        NIDStr.Printf( "%010u",(UInt32) Msg->GetNID() );///< 10
        LineBroker = Msg->GetLINBRN();
        LineBroker.PadThis( 4, '0' );                   ///< 4
        UserDef.PadThis( UDD_LENGTH - 4, ' ' );         ///< 176        
        MsgLen = TWSE_EXEC_LENGTH;                     ///< Length :321  
        UFC::GetTimeString( MessageTime, FALSE );  ///< 9            
        sprintf( ExecData, "%s%c%s%s%s%s", TFXMsg.c_str(), MarketC, NIDStr.c_str(), LineBroker.c_str(), UserDef.c_str(), MessageTime.c_str() );
    }   
    UFC::BufferedLog::DebugPrintf( " [%s]Filled to Queue[%s]", GetMarketName(Msg->GetMarket()), ExecData );    	    
    ExecData[ MsgLen -1 ] = 0x0A;         ///< 1    
    UFC::UQueueData QData( 1, ExecData, DATA_LENGTH );
    
    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE  || Msg->GetMarket() == nsOrderMessageDefine::mES  )
	FTSEFillQueue.Send( QData );
    else if( Msg->GetMarket() == nsOrderMessageDefine::mOTC )
	FOTCFillQueue.Send( QData );
    else 
        FTFXExecQueue.Send( QData );
}    
//------------------------------------------------------------------------------
// Exchange Confirm(120) +Market(1) + NID(10) + Line Broker(4) + UserDefine(176) + Time(9) + End(1) = 321
//------------------------------------------------------------------------------
void SpeedyQueueOrderConnection::ConfirmToBackend( TExecutionReportMessage* Msg )
{		
    char            ExecData[ DATA_LENGTH ];
    UFC::AnsiString Body( Msg->GetSrc() );
    UFC::AnsiString UserDef;
    UFC::AnsiString NIDStr, MessageTime;
    UFC::AnsiString LineBroker;
    char            MarketC = GetMarketCh( Msg->GetMarket() );
    int             MsgLen;
    
    GetUserData( Msg->GetData(), UserDef );    
    memset( ExecData, 0, DATA_LENGTH );
    Body.PadThis( 120, ' ' );                ///< 120
    NIDStr.Printf( "%010u",(UInt32) Msg->GetNID() ); ///< 10         
    if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures || Msg->GetMarket() == nsOrderMessageDefine::mTWOptions )
    {
        UserDef.PadThis( UDD_LENGTH - 3, ' ' );  ///< 177
        if( Msg->GetTradingSessionID( ) == nsOrderMessageDefine::tsOffHour )
            UserDef[ UserDef.Length() -1 ] = '1';
        else
            UserDef[ UserDef.Length() -1 ] = '0';
        LineBroker = Msg->GetPVC();
        LineBroker.PadThis( 3, '0' );            ///< 3 SessionID
        MsgLen = TFX_FILL_LENGTH;
        UFC::GetTimeString( MessageTime, FALSE );///< 9
        sprintf( ExecData, "%s%c%s%s%s%03d%05d%s", Body.c_str(), MarketC, NIDStr.c_str(), LineBroker.c_str(),UserDef.c_str(), Msg->GetTMPPartID(), Msg->GetTMPStstusCode(), MessageTime.c_str() );
    }
    else
    {
        UserDef.PadThis( UDD_LENGTH - 4, ' ' );  ///< 176        
        LineBroker = Msg->GetLINBRN();
        LineBroker.PadThis( 4, '0' );            ///< 4        
        MsgLen = TWSE_EXEC_LENGTH;
        UFC::GetTimeString( MessageTime, FALSE );///< 9
        sprintf( ExecData, "%s%c%s%s%s%s", Body.c_str(), MarketC, NIDStr.c_str(), LineBroker.c_str(),UserDef.c_str(), MessageTime.c_str() );
    }    
    UFC::BufferedLog::DebugPrintf( " [%s] Confirm to Queue[%s]", GetMarketName(Msg->GetMarket()), ExecData );    	    
    ExecData[ MsgLen -1 ] = 0x0A;       ///< 1     	
    UFC::UQueueData QData( 1, ExecData, DATA_LENGTH );
    	
    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE || Msg->GetMarket() == nsOrderMessageDefine::mES )
	FTSEConfirmQueue.Send( QData );
    else  if( Msg->GetMarket() == nsOrderMessageDefine::mOTC )
	FOTCConfirmQueue.Send( QData );    
    else 
        FTFXExecQueue.Send( QData );
}
//------------------------------------------------------------------------------
void SpeedyQueueOrderConnection::RejectToBackend( int NID,
						  UFC::AnsiString Order,
						  UFC::AnsiString UserDef,
						  nsOrderMessageDefine::MarketEnum Mkt )
{		
    char ExecData[ DATA_LENGTH ];    
    UFC::AnsiString NIDStr, MessageTime,FateR020;
    char            MarketC = GetMarketCh( Mkt );
    int             MsgLen;

    memset( ExecData, 0, DATA_LENGTH );
    X010ToX020( Order , 97, FateR020, FConnection->IsTWSENewVersion(), FConnection->IsTWSEExNewVersion() );    
    FateR020.PadThis( 120, ' ' );             ///< 120 
    NIDStr.Printf( "%010u", NID );            ///< 10
    UserDef.PadThis( UDD_LENGTH - 4, ' ' );   ///< 176     
    UFC::GetTimeString( MessageTime, FALSE ); ///< 9
    if( Mkt == nsOrderMessageDefine::mTWFutures || Mkt == nsOrderMessageDefine::mTWOptions )
    {
        UserDef.PadThis( UDD_LENGTH - 3, ' ' );  ///< 177
        MsgLen = TFX_EXEC_LENGTH;
        sprintf( ExecData, "%s%c%s%s%s00000097%s",  FateR020.c_str(), MarketC, NIDStr.c_str(), "000", UserDef.c_str(), MessageTime.c_str() );
    }
    else
    {
        UserDef.PadThis( UDD_LENGTH - 4, ' ' );  ///< 176
        MsgLen = TWSE_EXEC_LENGTH;
        sprintf( ExecData, "%s%c%s%s%s%s",  FateR020.c_str(), MarketC, NIDStr.c_str(), "0000", UserDef.c_str(), MessageTime.c_str() );
    }
    UFC::BufferedLog::DebugPrintf( " [%s] Confirm to Queue[%s]", GetMarketName(Mkt), ExecData );    	    
    ExecData[ MsgLen -1 ] = 0x0A;        ///< 1    	
    UFC::UQueueData QData( 1, ExecData, DATA_LENGTH );
    	
    if( Mkt == nsOrderMessageDefine::mTSE || Mkt == nsOrderMessageDefine::mES )
	FTSEConfirmQueue.Send( QData );
    else if( Mkt == nsOrderMessageDefine::mOTC )
	FOTCConfirmQueue.Send( QData );    
    else 
       FTFXExecQueue.Send( QData );
}
//------------------------------------------------------------------------------
void SpeedyQueueOrderConnection::RejectR070Backend( int NID,
						  UFC::AnsiString R070,
						  UFC::AnsiString UserDef,
						  nsOrderMessageDefine::MarketEnum Mkt )
{		
    char ExecData[ DATA_LENGTH ];    
    UFC::AnsiString NIDStr, MessageTime,FateR080;
    char            MarketC = GetMarketCh( Mkt );
    int             MsgLen;

    memset( ExecData, 0, DATA_LENGTH );
    
    UFC::GetTimeString( MessageTime, FALSE ); ///< 9
    UFC::AnsiString Time6( MessageTime.c_str(), 6 ); 
    UFC::AnsiString Time8( MessageTime.c_str(), 8 );
    UFC::AnsiString BKIDSym( R070.c_str() + 14, 27 );
    
    FateR080.Printf( "300107%s97%s%s0000000000000", Time6.c_str(), BKIDSym.c_str(), Time8.c_str() );    
    FateR080.PadThis( 120, ' ' );             ///< 120 
    NIDStr.Printf( "%010u", NID );            ///< 10
    UserDef.PadThis( UDD_LENGTH - 4, ' ' );   ///< 176     
    
    UserDef.PadThis( UDD_LENGTH - 3, ' ' );  ///< 177
    MsgLen = TFX_EXEC_LENGTH;
    sprintf( ExecData, "%s%c%s%s%s00000097%s",  FateR080.c_str(), MarketC, NIDStr.c_str(), "000", UserDef.c_str(), MessageTime.c_str() );
    
    UFC::BufferedLog::DebugPrintf( " [%s] Confirm to Queue[%s]", GetMarketName(Mkt), ExecData );    	    
    ExecData[ MsgLen -1 ] = 0x0A;        ///< 1    	
    UFC::UQueueData QData( 1, ExecData, DATA_LENGTH );
     
    FTFXExecQueue.Send( QData );
}
//------------------------------------------------------------------------------

