#include "UFC.h"
#include "CancelOrderConnection.h"

extern UFC::AnsiString ApplicationName;
extern UFC::AnsiString ServerIP;
extern UFC::AnsiString ServerUserID;
extern UFC::AnsiString ServerPassword;
extern UFC::AnsiString ServerAccount;
extern UFC::AnsiString ServerBrokerID;
//extern UFC::AnsiString ServerWebID;
extern long ServerPort;
extern nsOrderMessageDefine::MarketEnum MarketType;

//------------------------------------------------------------------------------
CancelOrderConnection::CancelOrderConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName(ApplicationName)
,FIP( ServerIP )
,FPort( ServerPort )
,FID( ServerUserID )
,FPassword( ServerPassword )
,FAccount( ServerAccount )
,FBrokerID( ServerBrokerID )
,FRecoverBeginTime( RecoverBeginTime )
,FConnection( NULL )
,FIsLogon( FALSE )
{
    UFC::BufferedLog::Printf(  "[%s:%d]", ServerIP.c_str(), ServerPort );
    ///< Create the connection object.
    FConnection = new TTaifexConnection( FAppName, this  );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( MarketType, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0, 4).c_str() );
    ///< Load TAIFEX message format config files.
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
    //FConnection->SetFutDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    //FConnection->SetOptDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    //FConnection->SetQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
CancelOrderConnection::~CancelOrderConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;
}
//------------------------------------------------------------------------------
void CancelOrderConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str(), ctBoth );
}
//------------------------------------------------------------------------------
void CancelOrderConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void CancelOrderConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk )
    {
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );
        FIsLogon = TRUE;

/*
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime );

        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, rmAll )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
 */
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void CancelOrderConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " TFubonOrderConnection::OnRecoverFinished( Count = %d )", Count );

    UFC::SleepMS( 1000 );
    FIsLogon = TRUE;
}
//------------------------------------------------------------------------------
void CancelOrderConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup  )
{
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::Printf( " ----------- Futures Execution ----------" );
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " ----------- Option  Execution ----------" );
            break;
        default:
            break;
    }
    
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
    if( Msg->GetNoLegs() > 0 )
        UFC::BufferedLog::Printf( " Multilegs Execution Legs: %d", Msg->GetNoLegs() );
    else
        UFC::BufferedLog::Printf( " Single Order Execution" );
    UFC::BufferedLog::Printf( " Account: %s",Msg->GetAccount() );
    UFC::BufferedLog::Printf( " AccountFlag: %s",Msg->GetAccountFlag() );
    UFC::BufferedLog::Printf( " Symbol: %s",Msg->GetSymbol() );
    UFC::BufferedLog::Printf( " Side: %s",(Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "Buy" : "Sell");
    UFC::BufferedLog::Printf( " OrderID: %s",Msg->GetOrderID() );
    UFC::BufferedLog::Printf( " Qty:%d",Msg->GetOrderQty() );
    UFC::BufferedLog::Printf( " Price:%f",Msg->GetPrice() );
    switch( Msg->GetPositionEffect() )
    {
        case nsOrderMessageDefine::peOpen:UFC::BufferedLog::Printf( " PositionEffect: Open" ); break;
        case nsOrderMessageDefine::peClose:UFC::BufferedLog::Printf( " PositionEffect: Close" ); break;
        case nsOrderMessageDefine::peRolled:UFC::BufferedLog::Printf( " PositionEffect: Rolled" ); break;
        default: break;
    }
    UFC::BufferedLog::Printf( " ------------ End of Execution ----------" );
}
//------------------------------------------------------------------------------
void CancelOrderConnection::Execute( void )
{
    UFC::BufferedLog::Printf( " Connect to [%s:%d]",FIP.c_str() , FPort  );
    FConnection->Connect( FIP.c_str() , FPort, 10 );

    while( !IsTerminated() )
    {
/*
        if( FIsLogon == TRUE )
        {
            UFC::BufferedLog::Printf( " Input Qty or x to quit." );
            scanf("%s",inputStr);
            if( inputStr[0] == 'x' || inputStr[0] == 'X')
                exit( 0 );
            int Qty = atoi( inputStr );
            if( Qty <= 0 )
                exit( 0 );
            else
                New( Qty );
        }
        else
 */
            UFC::SleepMS( 600000 ); ///< wait for logon.
    }
}

//------------------------------------------------------------------------------
void CancelOrderConnection::Cancel(TCancelOrderMessage& CancelMsg)
{
    FConnection->CancelOrder(&CancelMsg);
}

//------------------------------------------------------------------------------
void CancelOrderConnection::QuoteCancel(TQuoteCancelMessage& CancelMsg)
{
    FConnection->QuoteCancel(&CancelMsg);
}

//------------------------------------------------------------------------------
BOOL CancelOrderConnection::IsLogon()
{
    return FIsLogon;
}  //IsLogon()
