#include "SpeedyOrderConnection.h"
#include "../../UFC/NetUtility.h"
//------------------------------------------------------------------------------
extern BOOL FIsTWSE;
//------------------------------------------------------------------------------
SpeedyOrderConnection::SpeedyOrderConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName( "SpeedyAPIClient" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "9903758" )
,FTFXBrokerID( "F001000" )
,FTWSEBrokerID( "7000" )
,FRecoverBeginTime( RecoverBeginTime )
,FConnection( NULL )
,FIsLogon( FALSE )
{
    LoadSetting();    
    ///< Create the TAIFEX connection object.
    FConnection = new TTaifexConnection( FAppName, this  );
    if( FIsTWSE == TRUE ) ///< For TWSE/OTC
    {
        FConnection->SetBrokerID( nsOrderMessageDefine::mTSE, FTWSEBrokerID.c_str() );
        ///< Load TWSE/OTC message format config files.
        FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
        FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
        FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
        FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );        
    }
    else ///< For TAIFEX
    {
    	FConnection->SetUseNewFuturesSymbol( TRUE );
    	///< Set the broker ID of this connection.
        FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FTFXBrokerID.c_str() );
        FConnection->SetCMID( FTFXBrokerID.SubString(0,4).c_str() );
    	///< Load TAIFEX message format config files.
    	FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    	FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
    }    
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
SpeedyOrderConnection::~SpeedyOrderConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::LoadSetting( void )
{
    UFC::UiniFile* pConfig;
    UFC::AnsiString Value;
    
    try
    {
        pConfig = new UFC::UiniFile( "SpeedyOrderConnection.cfg" );
        if (pConfig->GetValue("Setting", "IP", FIP) == FALSE)
            UFC::BufferedLog::Printf(" Speedy Server IP not set.");            
        if (pConfig->GetValue("Setting", "ID", FID) == FALSE)
            UFC::BufferedLog::Printf(" Logon ID not set.");
        if (pConfig->GetValue("Setting", "Password", FPassword) == FALSE)
            UFC::BufferedLog::Printf(" Logon Password not set.");
        if (pConfig->GetValue("Setting", "Account", FAccount) == FALSE)
            UFC::BufferedLog::Printf(" Logon Account not set.");
        if (pConfig->GetValue("Setting", "BrokerID", FTFXBrokerID ) == FALSE)
            UFC::BufferedLog::Printf(" TAIFEX BrokerID not set.");
        if (pConfig->GetValue("Setting", "TWSEBrokerID", FTWSEBrokerID ) == FALSE)
            UFC::BufferedLog::Printf(" TWSE BrokerID not set.");
    }
    catch (...)
    {
        UFC::BufferedLog::Printf(" SpeedyOrderConnection.cfg not found, Use default values.");        
    }    
    UFC::BufferedLog::Printf( " ----------- Speedy Settings----------" );                                        
    UFC::BufferedLog::Printf(  " Speedy Server[%s:%d]", FIP.c_str(), FPort );    
    UFC::BufferedLog::Printf(  " Login ID[%s]", FID.c_str() );    
    UFC::BufferedLog::Printf(  " Password[%s]", FPassword.c_str() );    
    UFC::BufferedLog::Printf(  " Account[%s]", FAccount.c_str() );    
    UFC::BufferedLog::Printf(  " TWSE BrokerID[%s]", FTWSEBrokerID.c_str() );    
    UFC::BufferedLog::Printf(  " TAIFEX BrokerID[%s]", FTFXBrokerID.c_str() );    
    UFC::BufferedLog::Printf( " -------------------------------------" );                                        
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk ) ///< Logon OK!
    {
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime );
        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, rmAll )", BeginTime.c_str() );
        ///< Recover execution reports.
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
    }
    else ///< Logon failed.
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " TFubonOrderConnection::OnRecoverFinished( Count = %d )", Count );
    
    UFC::SleepMS( 1000 );
    FIsLogon = TRUE;
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{ 
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::Printf( " ----------- Futures Execution ----------" );                                    
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " ----------- Options  Execution ----------" );                        
            break;
        case nsOrderMessageDefine::mTSE:
            UFC::BufferedLog::Printf( " ----------- TWSE  Execution ----------" );                        
            break;
        case nsOrderMessageDefine::mOTC:
            UFC::BufferedLog::Printf( " ----------- OTC  Execution ----------" );                        
            break;
        default:
            break;
    }    
    UFC::BufferedLog::Printf( " NID:%010u", Msg->GetNID() );            
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osPendingNew:
            UFC::BufferedLog::Printf( " OrderStatus: PendingNew" );            
            break;
        case nsOrderMessageDefine::osPendingReplace:
            UFC::BufferedLog::Printf( " OrderStatus: PendingReplace" );            
            break;
        case nsOrderMessageDefine::osPendingCancel:
            UFC::BufferedLog::Printf( " OrderStatus: PendingCancel" );            
            break;
        case nsOrderMessageDefine::osNew:
            UFC::BufferedLog::Printf( " OrderStatus: New" );            
            break;
        case nsOrderMessageDefine::osReplaced:
            UFC::BufferedLog::Printf( " OrderStatus: Replaced" );            
            break;
        case nsOrderMessageDefine::osCanceled:
            UFC::BufferedLog::Printf( " OrderStatus: Canceled" );                        
            break;
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:
            UFC::BufferedLog::Printf( " OrderStatus: Filled" );                        
            break;
        case nsOrderMessageDefine::osRejected:
            switch( Msg->GetCxlRejResponseTo() )
            {
                case nsOrderMessageDefine::crrNew:
                    UFC::BufferedLog::Printf( " OrderStatus: Rejected (New order)" );                        
                    UFC::BufferedLog::Printf( " Reason:%s", Msg->GetText() );
                    break;
                case nsOrderMessageDefine::crrReplace:
                    UFC::BufferedLog::Printf( " OrderStatus: Rejected (Replace order)" );                        
                    UFC::BufferedLog::Printf( " Reason:%s", Msg->GetText() );
                    break;
                case nsOrderMessageDefine::crrCancel:
                    UFC::BufferedLog::Printf( " OrderStatus: Rejected (Cancel order)" );                        
                    UFC::BufferedLog::Printf( " Reason:%s", Msg->GetText() );
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }       
    UFC::BufferedLog::Printf( " Symbol: %s",Msg->GetSymbol() );                        
    UFC::BufferedLog::Printf( " Side: %s",(Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "Buy" : "Sell");                            
    UFC::BufferedLog::Printf( " Account: %s",Msg->GetAccount() );                        
    UFC::BufferedLog::Printf( " AccountFlag: %s",Msg->GetAccountFlag() );                            
    UFC::BufferedLog::Printf( " OrderID: %s",Msg->GetOrderID() );                            
    UFC::BufferedLog::Printf( " Qty:%d",Msg->GetOrderQty() );
    UFC::BufferedLog::Printf( " Price:%f",Msg->GetPrice() );  
    if( FIsTWSE == FALSE ) ///< Futures/Options
    {
        if( Msg->GetNoLegs() > 0 )
            UFC::BufferedLog::Printf( " Multi-legs Execution Legs: %d",Msg->GetNoLegs() );                        
        else
            UFC::BufferedLog::Printf( " Single Order Execution" );                        
        switch( Msg->GetPositionEffect() )
        {
            case nsOrderMessageDefine::peDayTradeOpen:
            case nsOrderMessageDefine::peOpen:UFC::BufferedLog::Printf( " PositionEffect: Open" ); break;
            case nsOrderMessageDefine::peClose:UFC::BufferedLog::Printf( " PositionEffect: Close" ); break;
            case nsOrderMessageDefine::peDayTrade:UFC::BufferedLog::Printf( " PositionEffect: Rolled" ); break;
            case nsOrderMessageDefine::peTMPMarketMaker:UFC::BufferedLog::Printf( " PositionEffect: Market maker" ); break;
            case nsOrderMessageDefine::peAuto:UFC::BufferedLog::Printf( " PositionEffect: Auto" ); break;
            default: UFC::BufferedLog::Printf( " PositionEffect: Open" ); break;
        }    
    }
    UFC::BufferedLog::Printf( " ------------ End of Execution ----------" );                                
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnChangePassword( ChangePwdResult Result )
{
    UFC::BufferedLog::Printf( " ----------- Change Password Result ----------" );                                        
    switch( Result )
    {
	case crModifyOk:      UFC::BufferedLog::Printf( " Modify Password OK! " );break;
	case crFailed:        UFC::BufferedLog::Printf( " Modify Password Failed! " );break;
	case crNotLogin:      UFC::BufferedLog::Printf( " Failed! Login before change password." );break;
	case crSameAsOld:     UFC::BufferedLog::Printf( " Failed! The new password is same as the original. " );break;
	case crLengthNeeds8:  UFC::BufferedLog::Printf( " Failed! Password at last need 8 chars." );break;
	case crNeedsNumbers:  UFC::BufferedLog::Printf( " Failed! Password must contains number chars." );break;
	case crNeedsSymbols:  UFC::BufferedLog::Printf( " Failed! Password must contains symbol chars. " );break;
	case crUpperLowerCase:UFC::BufferedLog::Printf( " Failed! Password must contains both upper and lower case chars. " );break;
	case crRequestTimeOut:UFC::BufferedLog::Printf( " Failed! Request timeout." );break;
	case crWrongPassword: UFC::BufferedLog::Printf( " Failed! Wrong original Password." );break;  
    }
    UFC::BufferedLog::Printf( " ---------------------------------------------" );                                        
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::Execute( void )
{    
    char inputStr[ 128 ];

    UFC::BufferedLog::Printf( " Connect to [%s:%d]",FIP.c_str() , FPort  );
    FConnection->Connect( FIP.c_str() , FPort , 5 );    
    
    while( !IsTerminated() )
    {
        if( FIsLogon == TRUE )
        {            
            UFC::BufferedLog::Printf( " Input Qty or x to quit. p to change password." );            
            scanf("%s",inputStr);
            
            if( inputStr[0] == 'p' || inputStr[0] == 'P') 
            {
                ChangePassword( );
            }
            else if( inputStr[0] == 'x' || inputStr[0] == 'X')             
            {
                exit( 0 );
            }
            else
            {
                int Qty = atoi( inputStr );
                if( Qty <= 0 )
                    exit( 0 );
                else
                {
                    if( FIsTWSE == TRUE ) ///< For TWSE/OTC
                        NewTWSE( Qty );
                    else
                        NewFutures( Qty );
	        }                
            }
        }
        else
            UFC::SleepMS( 500 ); ///< wait for logon.
    }
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::ChangePassword( void )
{
    char OrigPassword[ 128 ];
    char NewPassword[ 128 ];
    
    UFC::BufferedLog::Printf( " Input the original password:" );            
    scanf("%s",OrigPassword);     
    UFC::BufferedLog::Printf( " Input the New password:" );            
    scanf("%s",NewPassword);     
    FConnection->ChangePassword( OrigPassword, NewPassword );
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::NewFutures( int Qty )
{
    TNewOrderMessage Msg;     

    ///< Fill properties
    Msg.SetMarket( nsOrderMessageDefine::mTWFutures);
    Msg.SetAccount( FAccount.c_str() );
    Msg.SetSymbol( "TXFC9" );
    Msg.SetPrice( 12 );
    Msg.SetSide( nsOrderMessageDefine::sBuy);
    Msg.SetOrderQty( Qty );
    Msg.SetPositionEffect(  nsOrderMessageDefine::peClose );    
    ///< Send order
    FConnection->NewOrder( &Msg );
    UFC::BufferedLog::Printf( " New Order NID:%010u", Msg.GetNID() );            
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::NewTWSE( int Qty )
{
    TNewOrderMessage Msg;     

    ///< Fill properties
    Msg.SetMarket( nsOrderMessageDefine::mTSE);    
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
    Msg.SetAccount( FAccount.c_str() );
    Msg.SetSymbol( "2357" );
    Msg.SetPrice( 252.5 );
    Msg.SetSide( nsOrderMessageDefine::sBuy);
    Msg.SetOrderQty( Qty );    
    ///< Send order
    FConnection->NewOrder( &Msg );
    UFC::BufferedLog::Printf( " New Order NID:%010u", Msg.GetNID() );            
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::CancelTWSE( int Qty )
{
    TCancelOrderMessage Msg;     

    ///< Fill properties
    Msg.SetMarket( nsOrderMessageDefine::mTSE);    
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
    Msg.SetAccount( FAccount.c_str() );
    Msg.SetSymbol( "2357" );
    Msg.SetPrice( 252.5 );
    Msg.SetOrderID( "A0001" );
    Msg.SetSide( nsOrderMessageDefine::sBuy);
    Msg.SetOrderQty( Qty );    
    ///< Send order
    FConnection->CancelOrder( &Msg );
    UFC::BufferedLog::Printf( " Cancel Order NID:%010u", Msg.GetNID() );            
}
//------------------------------------------------------------------------------
bool SpeedyOrderConnection::IsConnect( void )
{
    if( FConnection != NULL )
        return FConnection->IsConnected();
    return false;
}
//------------------------------------------------------------------------------    
void SpeedyOrderConnection::Connect( void )
{
    if( FConnection != NULL )
        FConnection->Connect( FIP.c_str() , FPort , 5 );    
}
//------------------------------------------------------------------------------
