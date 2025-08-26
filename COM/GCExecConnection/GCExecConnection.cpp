#include "GCExecConnection.h"
#include "../../UFC/NetUtility.h"
#include <math.h>
//------------------------------------------------------------------------------
extern BOOL            DumpOnly;
extern MessageObject*  MessageObj;
extern UFC::PInt32     FRecoverBeginTime;
//------------------------------------------------------------------------------
UFC::PInt32            GCExecConnection::FSequence;
//------------------------------------------------------------------------------
GCExecConnection::GCExecConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName( "GCExecConnection" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F021999" )
,FOrderIDPrefix( 'A' )
,FWEBID( "000" )
,FConnection( NULL )
,FIsLogon( FALSE )
{      
    UFC::AnsiString ShmName( "GC_PATS_RPTSEQ" );
    UFC::AnsiString LogName = "../log/GCExec" + UFC::GetDateString() + ".log";
    LoadSetting(  );    
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( " Dump to screen only.");
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "  DDSC AS400 backend interface module   " );
    UFC::BufferedLog::Printf( "  Copyright (c) %d MDBS       ", UFC::ThisYear() );
    UFC::BufferedLog::Printf( "  startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "  Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "  Debug Mode:On (Dump to stdout only)");    
    else
        UFC::BufferedLog::Printf( "  Debug Mode:Off");    
    UFC::BufferedLog::Printf( "  OrderID Prefix:%c", FOrderIDPrefix );    
    UFC::BufferedLog::Printf( "__________________________________________" );       
    ///< Create the connection object.
    
    UFC::BufferedLog::SetDebugMode( TRUE );
    FConnection = new TTaifexConnection(  FAppName, this );

    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.
    FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
    FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
    FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
    FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
    ///< Set log file name.    
    FConnection->SetLogFile( LogName );
    FConnection->AddSellSide( ssForeignExchange );    
    GCExecConnection::FSequence.Attach( ShmName );    
    
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
void GCExecConnection::LoadSetting( void )
{
    try
    {        
        UFC::UiniFile   Config( "../cfg/GCExecConnection.cfg" );
        UFC::AnsiString Value;
        if( Config.GetValue( "Setting", "IP", FIP ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Speedy Server IP not set.");
            exit(1);
        }
        if( Config.GetValue( "Setting", "ID",FID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon ID not set.");
            exit(1);
        }    
        if( Config.GetValue( "Setting", "Password", FPassword ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon Password not set.");
            exit(1);
        }    
        if( Config.GetValue( "Setting", "Account", FAccount ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon Account not set.");
            exit(1);
        }    
        if( Config.GetValue( "Setting", "BrokerID", FBrokerID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" BrokerID not set.");
            exit(1);
        }            
        if( Config.GetValue( "Setting", "WebID", FWEBID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" WebID not set.");
            exit(1);
        }
        if( Config.GetValue( "Setting", "OrderIDPrefix", Value ) == TRUE && Value.Length() > 0)
        {
            FOrderIDPrefix = Value[0];
        }
        FWEBID.PadThis( 3, ' ' ); 
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::Printf(" %s", e.what());
        exit(1);
    }

    if( UFC::FileExists( "../cfg/AE400.cfg" ) )
    {
        UFC::UiniFile AEConfig( "../cfg/AE400.cfg" );
        UFC::Section* pSec = AEConfig.GetSection( "AE" );
        if( pSec )
        {
            for( register int i = 0; i < pSec->ItemCount(); i++ )
            {
                UFC::AnsiString Name, Value;

                pSec->GetNameValue( i, Name, Value );

                UFC::AnsiString AE, WebID;
                AE = Value.SubString( 0, Value.AnsiPos( ',' ) );
                WebID = Value.SubString( Value.AnsiPos( ',' ) + 1, Value.Length() - Value.AnsiPos( ',' ) );
                UFC::BufferedLog::Printf( " AE:[%s] WebID:[%s]", AE.c_str(), WebID.c_str() );
                AE.PadThis( 10, ' ' );
                WebID.PadThis( 2, ' ' );
                UFC::BufferedLog::Printf( " AE:[%s] WebID:[%s]", AE.c_str(), WebID.c_str() );
                FAETable.Add( Name, new AEInfo( AE, WebID ) );
            }
        }
    }
}
//------------------------------------------------------------------------------
GCExecConnection::~GCExecConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
void GCExecConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str(),  ctRecvExecution, 20083 );
    FIsLogon = TRUE;
}
//------------------------------------------------------------------------------
void GCExecConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void GCExecConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk )
    {
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime.ToInt32() );
        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, rmAll )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll  );
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void GCExecConnection::Recover( UFC::AnsiString BeginTime, char Type )
{
    if( FIsLogon == TRUE )
    {
        if( Type == 'P' )
        {
            FConnection->Recover( BeginTime.c_str(), rdBoth, rmForeignFutures );
            UFC::BufferedLog::Printf( " Recover PATS Execution from:%s", BeginTime.c_str() );
        }
        else if( Type == 'F' )
        {
            FConnection->Recover( BeginTime.c_str(), rdBoth, rmFutures );
            UFC::BufferedLog::Printf( " Recover TAIFEX Futures Execution from:%s", BeginTime.c_str() );
        }
        else if( Type == 'O' )
        {
            FConnection->Recover( BeginTime.c_str(), rdBoth, rmOption );
            UFC::BufferedLog::Printf( " Recover TAIFEX Options Execution from:%s", BeginTime.c_str() );
        }
        else
        {
            FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
            UFC::BufferedLog::Printf( " Recover all Execution from:%s", BeginTime.c_str() );
        }
    }    
}    
//------------------------------------------------------------------------------
void GCExecConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " GCExecConnection::OnRecoverFinished( Count = %d )", Count );
    
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
void GCExecConnection::ConfirmToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString GIGO( Msg->GetData() );
    UFC::AnsiString TFXSRC(  Msg->GetSrc()+2 );
    UFC::AnsiString TFX,Backend, SSNID,PE;
    UFC::NameValueMessage NV("^");
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;
    char  QueueData[ 513 ];
    char  TSession = 'R';
    char  ErrC[2];
    int   System = 93;
    
    NV.FromString( GIGO );
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
        TSession = 'P';
    if( NV.Get( "Sub", AE ) )
        UFC::BufferedLog::Printf( " In UserData[Sub=%s]", AE.c_str() );
    if( NV.Get( "PE", PE )  )
    {
        if( PE == "9" )
            TFXSRC[ 68 ] = '9'; ///< OpenCloseOffset
        UFC::BufferedLog::Printf( " In UserData[PE=%s]", PE.c_str() );
    }
    ///< TAIFEX part.
    if( Market == nsOrderMessageDefine::mTWFutures )
        System = 30;            
    TFX.Printf( "%s%02d%s", "        ", System, TFXSRC.c_str() );
    memcpy( ErrC, Msg->GetStatusCode(), 2 );
    TFX[ 12+8 ] = ErrC[ 0 ]; 
    TFX[ 13+8 ] = ErrC[ 1 ];    
    ///< SSNID
    SSNID.Printf( "%05d", atoi( Msg->GetPVC() ) ) ;
    UFC::BufferedLog::Printf( " SSNID:[%s]", SSNID.c_str() );
    ///< Backend part.    
    AE.PadThis( 6, ' ' );
    GIGO.PadThis( 23, ' ' );
    char TIF;
    if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC )
        TIF = 'I';
    else if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifFOK )
        TIF = 'F';
    else
        TIF = 'R';
    ///< WEBID(FWEBID), ORDERIM(0),UPD1(' '),OFFQTY(0),QEMP+QDEV(GIGO),QSAL(AE)
    Backend.Printf( "%s%011d %04d%s%s",FWEBID.c_str(),0,0,GIGO.c_str(),AE.c_str());
    ///< Compose to a full As400 report.
    memset( QueueData, ' ', 512 );
    QueueData[512] = 0;
    memcpy( QueueData, TFX.c_str(), TFX.Length());
    QueueData[421] = TSession;
    memcpy( QueueData + 424, SSNID.c_str(), 5 );
    memcpy( QueueData + 462, Backend.c_str(), Backend.Length());        
    Data.append("Data", QueueData );
    UFC::BufferedLog::Printf( " Px=%5.3f", Msg->GetPrice() );
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::Printf( " <FUT CONFIRM> <%s>", QueueData );
            if( DumpOnly == FALSE )
            {
                MessageObj->Send( "Confirm","To400", Data );
                if( TIF == 'R')
                    MessageObj->Send( "Confirm","To400Front", Data );
            }
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " <OPT CONFIRM> <%s>", QueueData );                                                            
            if( DumpOnly == FALSE )
            {
                MessageObj->Send( "Confirm","To400", Data );
                if( TIF == 'R')
                    MessageObj->Send( "Confirm","To400Front", Data );
            }
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void GCExecConnection::FillToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString Src( Msg->GetSrc() );
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString GIGO( Msg->GetData() );
    UFC::NameValueMessage NV("^");
    UFC::AnsiString TFX,Backend,MatchTime, SSNID,PE,QtyStr;
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;
    char QueueData[ 513 ];     
    char TSession = 'R';
    bool AutoPos = false; 
    int  System = 95;
    bool IsFilledExec;

    if( Market == nsOrderMessageDefine::mTWFutures )
        System = 50;    
    NV.FromString( GIGO );    
    if( NV.IsExists( "Sub" ) )
        NV.Get( "Sub", AE );
    if( NV.Get( "PE", PE ) && PE == "9" )
        AutoPos = true;     
    if( Src[0] == '1' )      ///< 1:Single filled.
    {
        MatchTime = Src.SubString( 61, 8 );
        IsFilledExec = true;
    }
    else if( Src[0] == '2' ) ///< 2:Multileg filled.
    {
        MatchTime = Src.SubString( 109 , 8 );
        IsFilledExec = true;
    }
    else                     ///< 3,4: Canceled,Replaced 
    {
        MatchTime = Src.SubString( 57, 8 );
        IsFilledExec = false;
    }
    UFC::BufferedLog::DebugPrintf( " MatchTime:[%s]", MatchTime.c_str() );
    UFC::BufferedLog::DebugPrintf( " BrokerID:[%s]", Msg->GetBrokerID() );
    ///< TAIFEX part.
    if( Msg->GetTMPStstusCode() == 47 || Msg->GetTMPStstusCode() == 48 )
        TFX.Printf( "        %02d0002%s%02d%s01%s",System,MatchTime.SubString( 0, 6 ).c_str(),Msg->GetTMPStstusCode(), Msg->GetBrokerID(), Msg->GetSrc() );                       
    else
        TFX.Printf( "        %02d0002%s%02d%s01%s",System,MatchTime.SubString( 0, 6 ).c_str(),0, Msg->GetBrokerID(), Msg->GetSrc() );                
    ///< Backend part.1
    AE.PadThis( 6, ' ' );
    GIGO.PadThis( 23, ' ' );
    Backend.Printf( "%s%011d %04d%s%s",FWEBID.c_str(),0,0,GIGO.c_str(), AE.c_str());
    char TIF;
    if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC )
        TIF = 'I';
    else if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifFOK )
        TIF = 'F';
    else
        TIF = 'R';
    SSNID.Printf( "%05d%c", atoi( Msg->GetPVC()), TIF );
    UFC::BufferedLog::Printf( " SSNID:[%s]", SSNID.c_str() );
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
        TSession = 'P';
    ///< Compose to a full As400 report.
    memset( QueueData, ' ', 512 );
    QueueData[512] = 0;
    memcpy( QueueData, TFX.c_str(), TFX.Length());
    if( IsFilledExec == true )  ///< Filled Doc[421]
    {
        QtyStr.Printf( "%04d%04d%04d", Msg->GetCumQty(), Msg->GetLeavesQty(),Msg->GetBeforeQty());        
        memcpy( QueueData + 158, QtyStr.c_str(), 12 );
        QueueData[420] = TSession;
    }
    else ///< Confirm, Cancel,Replaced  Doc[422]
        QueueData[421] = TSession;    
    memcpy( QueueData + 424, SSNID.c_str(), SSNID.Length() );
    memcpy( QueueData + 462, Backend.c_str(), Backend.Length());
    if( Src[0] == '3' || Src[0] == '4') ///< Canceled.
        QueueData[460] = 'S';
    Data.append("Data", QueueData );
    
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::Printf( " <FUT Fill> <%s>", QueueData );                                                
            if( DumpOnly == FALSE )
            {
                MessageObj->Send( "Fill","To400", Data );
                if( TIF == 'R')
                    MessageObj->Send( "Fill","To400Front", Data );
            }
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " <OPT Fill> <%s>", QueueData );
            if( DumpOnly == FALSE )
            {
                MessageObj->Send( "Fill","To400", Data );
                if( TIF == 'R')
                    MessageObj->Send( "Fill","To400Front", Data );
            }
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void GCExecConnection::TSEConfirmToBackend( TExecutionReportMessage* Msg )
{
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;
    UFC::AnsiString QueueData( Msg->GetSrc() );

    UFC::AnsiString Key;
    UFC::AnsiString AE = "          ";
    UFC::AnsiString WebID = "  ";
    AEInfo *info;
    QueueData.PadThis( 256, ' ' );

    Key.Printf( "%s%s", Msg->GetAccount(), Msg->GetAE() );
    if( (info = FAETable.GetObjectByKey( Key )) != NULL )
    {
        AE = info->AE;
        WebID = info->WebID;
        //AE.PadThis( 11, ' ' );
    }
    UFC::BufferedLog::DebugPrintf( " [%s]=>[%s]:[%s]", Key.c_str(), AE.c_str(), WebID.c_str() );

    //memcpy( (void*)(QueueData.c_str() + 150), AE.c_str(), 11 );
    memcpy( (void*)(QueueData.c_str() + 244), AE.c_str(), 10 );
    memcpy( (void*)(QueueData.c_str() + 254), WebID.c_str(), 2 );

    Data.append("Data", QueueData );

    switch( Market )
    {
        case nsOrderMessageDefine::mTSE:
            UFC::BufferedLog::Printf( " <TSE CONFIRM> <%s>", QueueData.c_str() );
            if( DumpOnly == FALSE )
                MessageObj->Send( "TSEConfirm","To400", Data );
            break;
        case nsOrderMessageDefine::mOTC:
            UFC::BufferedLog::Printf( " <OTC CONFIRM> <%s>", QueueData.c_str() );
            if( DumpOnly == FALSE )
                MessageObj->Send( "TSEConfirm","To400", Data );
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void GCExecConnection::TSEFillToBackend( TExecutionReportMessage* Msg )
{
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;
    UFC::AnsiString QueueData;
    UFC::AnsiString MatchTime( Msg->GetTransactTime() );
    UFC::AnsiString Key;
    UFC::AnsiString AE = "          ";
    UFC::AnsiString WebID = "  ";
    AEInfo *info;

    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE )
        QueueData.Printf( "501000%06s00006201%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetSrc() );
    else
        QueueData.Printf( "951000%06s00006201%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetSrc() );
    QueueData.PadThis( 256, ' ' );

    Key.Printf( "%s%s", Msg->GetAccount(), Msg->GetAE() );
    if( (info = FAETable.GetObjectByKey( Key )) != NULL )
    {
        AE = info->AE;
        WebID = info->WebID;
        //AE.PadThis( 11, ' ' );
    }
    UFC::BufferedLog::DebugPrintf( " [%s]=>[%s]:[%s]", Key.c_str(), AE.c_str(), WebID.c_str() );

    //memcpy( (void*)(QueueData.c_str() + 150), AE.c_str(), 11 );
    memcpy( (void*)(QueueData.c_str() + 244), AE.c_str(), 10 );
    memcpy( (void*)(QueueData.c_str() + 254), WebID.c_str(), 2 );

    Data.append("Data", QueueData );



    switch( Market )
    {
        case nsOrderMessageDefine::mTSE:
            UFC::BufferedLog::Printf( " <TSE Fill> <%s>", QueueData.c_str() );
            if( DumpOnly == FALSE )
                MessageObj->Send( "TSEFill","To400", Data );
            break;
        case nsOrderMessageDefine::mOTC:
            UFC::BufferedLog::Printf( " <OTC Fill> <%s>", QueueData.c_str() );
            if( DumpOnly == FALSE )
                MessageObj->Send( "TSEFill","To400", Data );
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void GCExecConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup  )
{
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    
    UFC::BufferedLog::Printf( "<Market> <%d>", Market );                                                                
    if( Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mOTC )
    {
        switch( Msg->GetOrderStatus() )
        {
            case nsOrderMessageDefine::osNew:
            case nsOrderMessageDefine::osReplaced:
            case nsOrderMessageDefine::osCanceled:
                 TSEConfirmToBackend( Msg );
                 break;
            
            case nsOrderMessageDefine::osPartiallyFilled:
            case nsOrderMessageDefine::osFilled:
                TSEFillToBackend( Msg );
                break;
            case nsOrderMessageDefine::osPendingNew:
            case nsOrderMessageDefine::osPendingReplace:
            case nsOrderMessageDefine::osPendingCancel:
            case nsOrderMessageDefine::osRejected:
            default: break;
        }
    }
    else
    {
        switch( Msg->GetOrderStatus() )
        {
            case nsOrderMessageDefine::osNew:
                 if( Market == nsOrderMessageDefine::mForeignFutures ||
                     Market == nsOrderMessageDefine::mForeignOptions )
                     PATSExecutionReport( Msg, TRUE );
                 else
                     ConfirmToBackend( Msg );
                 break;
            case nsOrderMessageDefine::osReplaced:
                if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                    Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
                    ConfirmToBackend( Msg );
                else
                    FillToBackend( Msg );
                break;
            case nsOrderMessageDefine::osCanceled:
                if( Market == nsOrderMessageDefine::mForeignFutures ||
                    Market == nsOrderMessageDefine::mForeignOptions )
                {
                     PATSExecutionReport( Msg ,FALSE );
                }
                else                
                {
                    if ( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC && Msg->GetCumQty() > 0 && (Msg->GetLeavesQty() == 0) && (Msg->GetOrderQty() == 0))
                        return;
                    else
                        FillToBackend( Msg );
                }
                break;
            case nsOrderMessageDefine::osPartiallyFilled:
            case nsOrderMessageDefine::osFilled:
                 if( Market == nsOrderMessageDefine::mForeignFutures ||
                     Market == nsOrderMessageDefine::mForeignOptions )
                     PATSExecutionReport( Msg, FALSE );
                 else
                     FillToBackend( Msg );
                 break;
            case nsOrderMessageDefine::osPendingNew:
            case nsOrderMessageDefine::osPendingReplace:
            case nsOrderMessageDefine::osPendingCancel:break;
            case nsOrderMessageDefine::osRejected:
                if( Market == nsOrderMessageDefine::mForeignFutures ||
                    Market == nsOrderMessageDefine::mForeignOptions )
                    PATSExecutionReport( Msg ,FALSE );
                break;
            default: break;
        }
    }
}
//------------------------------------------------------------------------------
void GCExecConnection::Execute( void )
{      
    while( !IsTerminated() )
    {
        if( FIsLogon == FALSE )
            FConnection->Connect( FIP.c_str() , FPort, 10 );    
        sleep( 10 ); ///< wait 10 sec.
    }
}
//------------------------------------------------------------------------------
Int64 DoubleToInt64( double DoubleVal, int Digi)
{
    double precision = pow(0.1, (double) (Digi + 1));
    double multiple = pow(10.0, (double) Digi);

    if (DoubleVal >= 0.0)
        return (Int64) ((DoubleVal + precision) * multiple);
    else
        return (Int64) ((DoubleVal - precision) * multiple);
}
//------------------------------------------------------------------------------
void GCExecConnection::PATSExecutionReport( TExecutionReportMessage* Msg, BOOL IsConfirm )
{
    MTree Data;
    UFC::AnsiString Today,Value,OrderID;
    UFC::AnsiString RptTime( Msg->GetTransactTime());
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString Account( Msg->GetAccount() );
    UFC::AnsiString ExecID( Msg->GetExecID() );
    UFC::AnsiString Exchange( Msg->GetExchangeCode());
    UFC::AnsiString Symbol( Msg->GetSymbol());
    UFC::AnsiString MaturityMonthYear( Msg->GetMaturityMonthYear() );
    UFC::AnsiString PATSOrderID( Msg->GetClOrdID());
    PATSExec Rpt;
    
    UFC::GetYYYYMMDD( Today );
    
    OrderID.Printf("%c%s", FOrderIDPrefix, Msg->GetOrderID() );
    AE.PadThis( 6,' ' );
    ExecID.PadThis( 70, ' ');
    Account.PadThis( 20, ' ');
    Exchange.PadThis( 20, ' ');
    Symbol.PadThis( 20, ' ');
    OrderID.PadThis( 20, ' ');
    PATSOrderID.PadThis( 20, ' ');
    MaturityMonthYear.PadThis( 10, ' ');
    memset( &Rpt, ' ', sizeof( PATSExec ) );
    if( Msg->GetOrderStatus() == nsOrderMessageDefine::osNew )
        memcpy( Rpt.FunctionCode, "201", 3 ); ///< New Order
    else 
        memcpy( Rpt.FunctionCode, "203", 3 ); ///< Canceled and Filled.   
    GCExecConnection::FSequence += 1;
    if( FSequence.ToInt32() > 99999999 )
        FSequence = 1;
    Value.Printf( "%08d", FSequence.ToInt32() );
    memcpy( Rpt.SeqNo, Value.c_str(), 8 ); ///?
    memcpy( Rpt.FCM, "UOB", 3 );
    memcpy( Rpt.Account, Account.c_str(), 20 );
    memcpy( Rpt.OrderID, PATSOrderID.c_str(), 20 );
    memcpy( Rpt.OOrderID, OrderID.c_str(), 20 );            //X(20) Speedy Order ID x(6)
    memcpy( Rpt.SecondaryOrderID, PATSOrderID.c_str(), 20 );//X(20) ----- Not Use ----- ??    
    memcpy( Rpt.ExecID, ExecID.c_str(), 70 );               //X(70) PATS FIll execid
    memcpy( Rpt.OrderKind, "QUEST", 5 );                    //X(20) QUEST,QUEST+1,EDS,REG... ??
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osNew:             Rpt.OrderStatus[0] = '0'; break;
        case nsOrderMessageDefine::osPartiallyFilled: Rpt.OrderStatus[0] = '1'; break;
        case nsOrderMessageDefine::osFilled:          Rpt.OrderStatus[0] = '2'; break;    
        case nsOrderMessageDefine::osCanceled:        Rpt.OrderStatus[0] = '4'; break;          
        case nsOrderMessageDefine::osRejected:        Rpt.OrderStatus[0] = '8'; break;           
    
    }                                                              //X(1)  New:0 Partially Filled:1 Filled:2 Canceled:4 Reject:8
    memcpy( Rpt.Exchange, Exchange.c_str(), 20);                   //X(20) Exchange
    memcpy( Rpt.Symbol, Symbol.c_str(), 20 );                      //X(20) Symbol
    if( Msg->GetMarket() == nsOrderMessageDefine::mForeignFutures  )
        memcpy( Rpt.SecurityType, "FUT",3 );                       //X(5)  "FUT"
    else
        memcpy( Rpt.SecurityType, "OPT",3 );                       //X(5)  "OPT"
    memcpy( Rpt.MaturityMonthYear, MaturityMonthYear.c_str(), 10 );//X(10) Maturity Month Year
    
    Value.Printf("%013lld", DoubleToInt64( Msg->GetStrikePrice(), 6 ) );    
    memcpy( Rpt.StrikePrice, Value.c_str(),13 );                  //9(7)V9(6) Strike Price    
    switch( Msg->GetEventType() )
    {
        case nsOrderMessageDefine::evtCall: Rpt.PutOrCall[0] = '1';break;       //X(1)  Put:'0' Call:'1'        
        case nsOrderMessageDefine::evtPut:  Rpt.PutOrCall[0] = '0';break;       //X(1)  Put:'0' Call:'1'  
        case nsOrderMessageDefine::evtNone: Rpt.PutOrCall[0] = ' ';break;       //X(1)  Put:'0' Call:'1'
    }    
    if( Msg->GetSide() == nsOrderMessageDefine::sBuy)
        Rpt.Side[0] = '1';                 //X(1)  Buy:'1' Sell:'2'
    else
        Rpt.Side[0] = '2';                 //X(1)  Buy:'1' Sell:'2'
    Value.Printf( "%010d", Msg->GetOrderQty());
    memcpy( Rpt.OrderQuantity, Value.c_str(), 10 );       //9(10) Qty    
    Value.Printf("%013lld", DoubleToInt64( Msg->GetPrice(), 6 ) );    
    memcpy( Rpt.Price, Value.c_str(), 13 ); //9(7)V9(6) Price
    Value.Printf("%013lld", DoubleToInt64( Msg->GetStopPx(), 6 ) );    
    memcpy( Rpt.StopPrice, Value.c_str(), 13 ); //9(7)V9(6) Stop Price
    switch( Msg->GetOrderType() )
    {
        case nsOrderMessageDefine::otLimit:     Rpt.OrderType[0] = '2';break;
        case nsOrderMessageDefine::otMarket:    Rpt.OrderType[0] = '1';break; 
        case nsOrderMessageDefine::otStop:      Rpt.OrderType[0] = '3';break; 
        case nsOrderMessageDefine::otStopLimit: Rpt.OrderType[0] = '4';break;
    } //X(1) Market:1 Limit:2 Stop:3 StopLimit:4     
    Rpt.TimeInForce[0] = '0'; ///< ROD Only
    Value.Printf( "%010d", Msg->GetLastQty());
    memcpy( Rpt.LastShares, Value.c_str(), 10 );      //9(10) LastQty    
    Value.Printf("%013lld", DoubleToInt64( Msg->GetLastPx(), 6 ) );    
    memcpy( Rpt.LastPrice, Value.c_str(), 13 );       //9(7)V9(6) Last Price    
    Value.Printf( "%010d", Msg->GetLeavesQty() );
    memcpy( Rpt.LeavesQuantity, Value.c_str(), 10 );  //9(10) Leaves Qty    
    Value.Printf( "%010d", Msg->GetCumQty() );
    memcpy( Rpt.CumQuantity, Value.c_str(), 10 );     //9(10) Cum Qty        
    Value.Printf("%013lld", DoubleToInt64( Msg->GetAvgPx(), 6 ) );    
    memcpy( Rpt.AveragePrice, Value.c_str(), 13 );    //9(7)V9(6) ----- Not Use -----*/
    Rpt.RejectResponseTo[0] = '1';     //X(1) Cancel:'1' Replace:'2'
    if( Msg->GetPositionEffect() == nsOrderMessageDefine::peDayTradeOpen )
        Rpt.Day_Trade[0] = 'Y';        //X(1)  Yes
    else
        Rpt.Day_Trade[0] = 'N';        //X(1)  No
    memcpy( Rpt.Branch, "999", 3 );    //X(3) "999"
    memcpy( Rpt.Cust_No,"0000000", 7 );//X(7) "0000000"    
    memcpy( Rpt.Ae, AE.c_str(), 6);    //X(6) "A01001"
    memcpy( Rpt.Web, "801",3 );        //X(3) "801"
    Rpt.Original[0] = 'A';             //X(1) 'A' 
    memcpy( Rpt.OrdDate,Today.c_str(),Today.Length());  //X(8) "20111227"
    
    if( RptTime.Length() != 6 )       
        UFC::GetHHMMSS( RptTime );
    memcpy( Rpt.OrdTime,RptTime.c_str(),RptTime.Length());  //X(6) "HHMMSS"    
    
    Rpt.EOS = 0;
    Data.append("Data", (char*)&Rpt );
    UFC::BufferedLog::Printf( " <PATSExec> <%s>", (char*)&Rpt );                                                            
    if( DumpOnly == FALSE )
    {
        if( IsConfirm == TRUE )
            MessageObj->Send( "PATSConfirm","To400", Data );    
        else
            MessageObj->Send( "PATSFill","To400", Data );    
    }        
}
//------------------------------------------------------------------------------
