#include "SpeedyOrderConnection.h"
#include "../../UFC/NetUtility.h"
//------------------------------------------------------------------------------
SpeedyOrderConnection::SpeedyOrderConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName( "SpeedyAPIClient" )
,FIP( "192.168.0.120" )//60.248.161.188" )
,FPort( 23456 )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F021999" )
,FRecoverBeginTime( RecoverBeginTime )
,FConnection( NULL )
,FIsLogon( FALSE )
{  
    ///< Create the connection object.
    FConnection = new TTaifexConnection( NULL, &FResource, FAppName, this, TRUE );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.
    FConnection->SetFutDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetOptDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );	
    FConnection->SetQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );                            
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
void SpeedyOrderConnection::OnLogonReply( const char* ReplyString, bool IsAccepted, int CID )
{
    if( IsAccepted )
    {
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime );
        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, 2, 4 )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), 2, 4 );
    }
    else
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
void SpeedyOrderConnection::OnExecutionReport( TExecutionReportMessage* Msg, bool PossDup )
{ 
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mFutures:
            UFC::BufferedLog::Printf( " ----------- Futures Execution ----------" );                                    
            break;
        case nsOrderMessageDefine::mOptions:
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
                    break;
                case nsOrderMessageDefine::crrReplace:
                    UFC::BufferedLog::Printf( " OrderStatus: Rejected (Replace order)" );                        
                    break;
                case nsOrderMessageDefine::crrCancel:
                    UFC::BufferedLog::Printf( " OrderStatus: Rejected (Cancel order)" );                        
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }       
    if( Msg->GetNoLegs() > 0 )
        UFC::BufferedLog::Printf( " Multilegs Execution Legs: %d",Msg->GetNoLegs() );                        
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
    }    
    UFC::BufferedLog::Printf( " ------------ End of Execution ----------" );                                
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::Execute( void )
{    
    char inputStr[ 128 ];
    
    FConnection->Connect( FIP.c_str() , FPort );    
    
    while( !IsTerminated() )
    {
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
            UFC::SleepMS( 500 ); ///< wait for logon.
    }
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::New( int Qty )
{
    TNewOrderMessage Msg;     

    ///< Fill properties
    Msg.SetMarket( nsOrderMessageDefine::mFutures);    
    Msg.SetBrokerID( FBrokerID.c_str() );
    Msg.SetAccount( FAccount.c_str() );
    Msg.SetSymbol( "TXFK7" );
    Msg.SetPrice( 123.45 );
    Msg.SetSide( nsOrderMessageDefine::sBuy);
    Msg.SetOrderQty( Qty );
    Msg.SetPositionEffect(  nsOrderMessageDefine::peClose );    
    ///< Send order
    FConnection->NewOrder( &Msg );
}
//------------------------------------------------------------------------------
