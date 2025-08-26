#include "SpeedyOrderConnection.h"
#include "../../UFC/NetUtility.h"

//------------------------------------------------------------------------------
// Strings[1]: Type(O,Q Order or quote)
// Strings[2]: Account.(1234567)
// Strings[3]: BrokerID.(F001000)
// Strings[4]: OrderID.(AAA01)
// Strings[5]: Symbol.(TXFG0)
// Strings[6]: Buy/Sell.(B/S)
//------------------------------------------------------------------------------
SpeedyOrderConnection::SpeedyOrderConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName( "SpeedyAPIClient" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F021999" )
,FRecoverBeginTime( RecoverBeginTime )
,FConnection( NULL )
,FIsLogon( FALSE )
,FThroughput( 25 )
,FOutCount( 0 )
,FFirstOrderTick( UFC::GetTickCountMS() )
,FExecID( 300000 )
{
    LoadSetting( );
    UFC::BufferedLog::Printf( " ______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   Speedy Cancel working order module " );
    UFC::BufferedLog::Printf( "   Copyright (c) 2007~%d MDBS", UFC::ThisYear() );
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");    
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );    
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   SpeedyGateway[%s:%d]", FIP.c_str(), FPort );
    UFC::BufferedLog::Printf( "   Login ID[%s]", FID.c_str() );
    UFC::BufferedLog::Printf( "   Account [%s]", FAccount.c_str() );
    UFC::BufferedLog::Printf( "   BrokerID[%s]", FBrokerID.c_str() );
    UFC::BufferedLog::Printf( "   Throughput[%d] Cancel per sec", FThroughput );
    UFC::BufferedLog::Printf( " ______________________________________________" );
    ///< Create the TAIFEX connection object.
    FConnection = new TTaifexConnection( FAppName, this  );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
    FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
    FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
    FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
    FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );    
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::LoadSetting( void )
{
    UFC::AnsiString FileName,Name,Value;
    FileName.Printf( "../cfg/SpeedyWorkingCancel.cfg" );
    try
    {
        UFC::UiniFile   Config( FileName );        
        
        Config.GetValue( "Setting", "GatewayIP", FIP );
        if( Config.GetValue( "Setting", "GatewayPort", Value ) )
            FPort = Value.ToInt();
        Config.GetValue( "Setting", "ID",       FID );
        Config.GetValue( "Setting", "Password", FPassword );
        Config.GetValue( "Setting", "Account",  FAccount );
        Config.GetValue( "Setting", "BrokerID", FBrokerID );
        if( Config.GetValue( "Setting", "Throughput", Value ) )
            FThroughput = Value.ToInt();
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::Printf( " Exception[%s].", e.what() );
        exit(1);
    }
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
    UFC::BufferedLog::Printf( " Login( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " Speedy API Connection broken" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk ) ///< Logon OK!
    {
        UFC::BufferedLog::Printf( " Login OK[%s]( CID=%d )", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime );
        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, rmAll )", BeginTime.c_str() );
        ///< Recover execution reports.        
        FConnection->RecoverFromLocalFile(  BeginTime.c_str(), rdBoth, rmAll, rsAll );
    }
    else ///< Logon failed.
    {
        UFC::BufferedLog::Printf( " Login failed[%s]", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " OnRecoverFinished( Count = %d )", Count );
    
    UFC::SleepMS( 1000 );
    FIsLogon = TRUE;
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{ 
    if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures ||
        Msg->GetMarket() == nsOrderMessageDefine::mTWOptions )
    {
        if( FExecID.Exists( Msg->GetExecID() ) == false )
        {
            FExecID.Add( Msg->GetExecID() );
            switch( Msg->GetOrderStatus() )
            {
                case nsOrderMessageDefine::osPendingNew:    
                case nsOrderMessageDefine::osPendingReplace:
                case nsOrderMessageDefine::osPendingCancel: 
                case nsOrderMessageDefine::osRejected:      break; ///< Not handle  
                case nsOrderMessageDefine::osNew:
                     OnNewOrder( Msg );            
                     break;
                case nsOrderMessageDefine::osReplaced:
                    if( Msg->GetTMPExecType() != nsOrderMessageDefine::tetPxReplaced &&
                        Msg->GetTMPExecType() != nsOrderMessageDefine::tetPxReplaced2 ) ///< Not Handle Replace Price                
                        OnReduceQty( Msg );        
                     break;
                case nsOrderMessageDefine::osPartiallyFilled:
                case nsOrderMessageDefine::osFilled:
                     OnReduceQty( Msg );
                     break;
                case nsOrderMessageDefine::osCanceled:
                     OnCancelOrder( Msg );                        
                     break;            
                default:
                     break;
            }
        }
    }
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnNewOrder( TExecutionReportMessage* Msg )
{ 
    UFC::AnsiString Key;
    UFC::PLockObject Lock( FOrderBookCS );
    
    Key.Printf( "%s%s", Msg->GetOrderID(), Msg->GetBrokerID() );    
    if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures )
    {
        if( FFutWorking.IsExists( Key ) == FALSE )
            FFutWorking.Add( Key, new WorkingOrder(Msg) );
    }
    else
    {
        if( FOptWorking.IsExists( Key ) == FALSE )
            FOptWorking.Add( Key, new WorkingOrder(Msg) );
    }
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnCancelOrder( TExecutionReportMessage* Msg )
{
    UFC::AnsiString Key;
    WorkingOrder* CxlOrd;
    UFC::PLockObject Lock( FOrderBookCS );
    
    Key.Printf( "%s%s", Msg->GetOrderID(), Msg->GetBrokerID() );
    if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures )
    {
        if( FFutWorking.GetObjectByKey( Key, CxlOrd ) == TRUE )
        {
            FFutWorking.DeleteByKey( Key );
            delete CxlOrd;
        }
    }
    else
    {
        if( FOptWorking.GetObjectByKey( Key, CxlOrd  ) == TRUE )
        {
            FOptWorking.DeleteByKey( Key );
            delete CxlOrd;
        }
    }
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::OnReduceQty( TExecutionReportMessage* Msg )
{
    UFC::AnsiString Key;
    WorkingOrder* CxlOrd;
    Int32 RQty = Msg->GetBeforeQty() - Msg->GetAfterQty();
    UFC::PLockObject Lock( FOrderBookCS );
    
    Key.Printf( "%s%s", Msg->GetOrderID(), Msg->GetBrokerID() );
    if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures )
    {
        if( FFutWorking.GetObjectByKey( Key, CxlOrd ) == TRUE )
        {
            if( CxlOrd->Reduce( RQty ) == true )
            {
                FFutWorking.DeleteByKey( Key );
                delete CxlOrd;
            }
        }
    }
    else
    {
        if( FOptWorking.GetObjectByKey( Key, CxlOrd  ) == TRUE )
        {
            if( CxlOrd->Reduce( RQty ) == true )
            {
                FOptWorking.DeleteByKey( Key );
                delete CxlOrd;
            }
        }
    }    
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::PrintWorking( void )
{
    UFC::PLockObject Lock( FOrderBookCS );
    WorkingOrder* Ord;
    
    if( FFutWorking.ItemCount() > 0 )
    {
        Ord = FFutWorking.First();
        UFC::BufferedLog::Printf( " -------------- Futures working order --------------" );    
        while( Ord != NULL )
        {
            Ord->print();
            Ord = FFutWorking.Next();
        };
        UFC::BufferedLog::Printf( " ---------------------------------------------------" );            
    }
    if( FOptWorking.ItemCount() > 0 )
    {
        Ord = FOptWorking.First();
        UFC::BufferedLog::Printf( " -------------- Options working order --------------" );    
        while( Ord != NULL )
        {
            Ord->print();
            Ord = FOptWorking.Next();
        };
        UFC::BufferedLog::Printf( " ---------------------------------------------------" );            
    }
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::Execute( void )
{    
    UFC::BufferedLog::Printf( " Connect to [%s:%d]",FIP.c_str() , FPort  );
    FConnection->Connect( FIP.c_str() , FPort , 3 );    
    
    while( !IsTerminated() )
    {
        if( FIsLogon == FALSE && FConnection->IsConnected() == FALSE )
            FConnection->Connect( FIP.c_str() , FPort , 3 );    
        UFC::SleepMS( 5000 ); ///< wait for logon.
        //if( FIsLogon == TRUE )
        //    PrintWorking( );
    }
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::AddToList(  UFC::PHashMap< UFC::AnsiString,WorkingOrder*>& Working,
                                        UFC::List<TCancelOrderMessage*>& CxlList,
                                        const UFC::AnsiString& ChkAcc,
                                        const UFC::AnsiString& ChkAE,
                                        const UFC::AnsiString& ChkData, 
                                        const UFC::AnsiString& ChkPID )
{
    WorkingOrder* Ord;    
    
    Ord = Working.First();
    while( Ord != NULL )
    {
        if( Ord->MatchAccount( ChkAcc ) == true &&
            Ord->MatchAE( ChkAE ) == true &&
            Ord->MatchData( ChkData ) == true &&
            Ord->MatchProduct( ChkPID ) == true )
        {
            CxlList.Add( Ord->GetCancelOrderMessage() );        
            Ord->print();
        }
        Ord = Working.Next();
    };    
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::CancelList( UFC::List<TCancelOrderMessage*>& CxlList,
                                     CancelMarket Market,
                                     const UFC::AnsiString& ChkAcc,
                                     const UFC::AnsiString& ChkAE,
                                     const UFC::AnsiString& ChkData, 
                                     const UFC::AnsiString& ChkPID )
{      
    UFC::PLockObject Lock( FOrderBookCS );
    
    UFC::BufferedLog::Printf( " -------------- Cancel working orders --------------" );    
    if( Market == cmTWFutures )
        AddToList( FFutWorking, CxlList, ChkAcc, ChkAE, ChkData, ChkPID );
    else if( Market == cmTWOptions )
        AddToList( FOptWorking, CxlList, ChkAcc, ChkAE, ChkData, ChkPID );
    else if( Market == cmAll )
    {
        AddToList( FFutWorking, CxlList, ChkAcc, ChkAE, ChkData, ChkPID );    
        AddToList( FOptWorking, CxlList, ChkAcc, ChkAE, ChkData, ChkPID );
    }    
    UFC::BufferedLog::Printf( " ---------------------------------------------------" );    
}
//------------------------------------------------------------------------------
void  SpeedyOrderConnection::ControlTroughput( void )
{
    Int32 Now = UFC::GetTickCountMS();
    Int32 TimeDiff = Now - FFirstOrderTick;
    
    FOutCount++;
    if( TimeDiff >= 1000 ) ///< More then 1 sec
    {
        FFirstOrderTick = Now;///< Reset timer.
        FOutCount       = 0;  ///< Reset counter.
    }
    else 
    {
        if( FOutCount >= FThroughput )
        {
            Int32 sms = (1000 - TimeDiff)%1000;
            
            UFC::BufferedLog::Printf( " Cancel order reach throughput[%d] sleep[%d]ms", FOutCount, sms );            
            UFC::SleepMS( sms );
        }
    }
}
//------------------------------------------------------------------------------
void SpeedyOrderConnection::Cancel( UFC::List<TCancelOrderMessage*>& CxlList )
{
    TCancelOrderMessage* CxlOrd;
    
    for( int i = 0 ;i < CxlList.ItemCount(); i++ )
    {
        CxlOrd = CxlList[ i ];
        ///< Send order
        FConnection->CancelOrder( CxlOrd );
        delete CxlOrd;
        ControlTroughput();
    }
    CxlList.Clear();
}
//------------------------------------------------------------------------------

