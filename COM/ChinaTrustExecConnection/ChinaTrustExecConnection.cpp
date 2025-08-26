#include "ChinaTrustExecConnection.h"
#include "../../UFC/NetUtility.h"
//------------------------------------------------------------------------------
extern BOOL  DumpOnly;
extern BOOL  NoQuoteCancel;
//------------------------------------------------------------------------------
ChinaTrustExecConnection::ChinaTrustExecConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName( "ChinaTrustExecConnection" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F021999" )
,FRecoverBeginTime( RecoverBeginTime )
,FConnection( NULL )
,FIsLogon( FALSE )
,FFUTSecondFillQueueID( 0 )
,FOPTSecondFillQueueID( 0 )
,FFUTConfirmQueue( FALSE )
,FOPTConfirmQueue( FALSE )
,FFUTFillQueue( FALSE )
,FOPTFillQueue( FALSE )
,FSecondFUTFillQueue( FALSE )
,FSecondOPTFillQueue( FALSE )
{      
    UFC::AnsiString LogName = "../log/ChinaTrustExec" + UFC::GetDateString() + ".log";
    LoadSetting(  );
    if( !FOPTConfirmQueue.Open( 0x73A50999, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open Option Order confirm queue failed.");
        exit( 0 );
    }
    if( !FOPTFillQueue.Open( 0x73E10999, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open Option Order fill queue failed.");
        exit( 0 );
    }
    if( !FFUTConfirmQueue.Open( 0x72A50999, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open Fautures Order confirm queue failed.");
        exit( 0 );
    }    
    if( !FFUTFillQueue.Open( 0x72E10999, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open Fautures Order fill queue failed.");
        exit( 0 );
    }    
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( " Dump to screen only.");
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   ChinaTrust backend interface module        " );
    UFC::BufferedLog::Printf( "   Startup at:%s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On (Dump to stdout only)");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");
    if( FFUTSecondFillQueueID != 0 )
    {
        if( !FSecondFUTFillQueue.Open( FFUTSecondFillQueueID, TRUE ) )
        {
            UFC::BufferedLog::Printf( "   Open Futures Second fill queue failed.");
            exit( 0 );
        }
        UFC::BufferedLog::Printf( "   Open Futures Second fill queue 0x%x.", FFUTSecondFillQueueID );
    }
    if( FOPTSecondFillQueueID != 0 )
    {
        if( !FSecondOPTFillQueue.Open( FOPTSecondFillQueueID, TRUE ) )
        {
            UFC::BufferedLog::Printf( "   Open Options Second fill queue failed.");
            exit( 0 );
        }
        UFC::BufferedLog::Printf( "   Open Options Second fill queue 0x%x.", FOPTSecondFillQueueID );
    }
    UFC::BufferedLog::Printf( "__________________________________________" );
    if( RecoverBeginTime == 0 )
    {
        FOPTConfirmQueue.Clear();
        FFUTConfirmQueue.Clear();
        FOPTFillQueue.Clear();
        FFUTFillQueue.Clear();
    }
    ///< Create the connection object.
    FConnection = new TTaifexConnection( FAppName, this );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );                            
    ///< Set log file name.    
    FConnection->SetLogFile( LogName );
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
void ChinaTrustExecConnection::LoadSetting( void )
{
    try
    {        
        UFC::UiniFile   Config( "../cfg/ChinaTrustExecConnection.cfg" );
        if( Config.GetValue( "Setting", "IP", FIP ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Speedy Server IP not set.");
            exit(1);
        }
        Config.GetValue( "Setting", "ID",FID );
        Config.GetValue( "Setting", "Password", FPassword );
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
        if( Config.GetValue( "Setting", "Branch", FBranch ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Branch not set.");
            exit(1);
        }
        UFC::BufferedLog::DebugPrintf( " Branch:[%s]", FBranch.c_str() );

        UFC::AnsiString Value;

        if( Config.GetValue( "Setting", "FUTFillQueue", Value ) == TRUE )
        {
            if( UFC::HexStringToBinary( Value, (UInt8*)&FFUTSecondFillQueueID, 4 ) == FALSE )
                FFUTSecondFillQueueID = 0;

        }
        if( Config.GetValue( "Setting", "OPTFillQueue", Value ) == TRUE )
        {
            if( UFC::HexStringToBinary( Value, (UInt8*)&FOPTSecondFillQueueID, 4 ) == FALSE )
                FOPTSecondFillQueueID = 0;
        }
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" ChinaTrustExecConnection.cfg not found.");
        exit(1);
    }
}
//------------------------------------------------------------------------------
ChinaTrustExecConnection::~ChinaTrustExecConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
void ChinaTrustExecConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon ID=[%s]", FID.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );
}
//------------------------------------------------------------------------------
void ChinaTrustExecConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void ChinaTrustExecConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk )
    {
        FIsLogon = TRUE;
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime );
        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, rmAll )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void ChinaTrustExecConnection::Recover( UFC::AnsiString BeginTime, UFC::AnsiString EndTime, char Flag )
{
    if( FIsLogon == TRUE )
    {
        if( Flag == 'A')
        {
            if( EndTime.ToInt() == 0 )
                FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
            else
                FConnection->Recover( (char*)BeginTime.c_str(), EndTime.c_str(), rdBoth, rmAll );
            UFC::BufferedLog::Printf( " Recover from:%s to %s", BeginTime.c_str(), EndTime.c_str() );
        }
        else if( Flag == 'C')
        {
            if( EndTime.ToInt() == 0 )
                FConnection->Recover( BeginTime.c_str(), rdConfirm, rmAll );
            else
                FConnection->Recover( BeginTime.c_str(), EndTime.c_str(), rdConfirm, rmAll );
            UFC::BufferedLog::Printf( " Recover from:%s to %s", BeginTime.c_str(), EndTime.c_str() );
        }
        else if( Flag == 'F')
        {
            if( EndTime.ToInt() == 0 )
                FConnection->Recover( BeginTime.c_str(), rdFill, rmAll );
            else
                FConnection->Recover( BeginTime.c_str(), EndTime.c_str(), rdFill, rmAll );
            UFC::BufferedLog::Printf( " Recover from:%s to %s", BeginTime.c_str(), EndTime.c_str() );
        }
    }
}    
//------------------------------------------------------------------------------
void ChinaTrustExecConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " ChinaTrustExecConnection::OnRecoverFinished( Count = %d )", Count );
    
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
void ChinaTrustExecConnection::ConfirmToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString SID( Msg->GetPVC() );
    long MType;
    char QueueData[ 256 ];

    SID.PadThis( 3, '0' );
    MType = 0x70000000 + ((long)(SID[0]-'0')<<8) + ((long)(SID[1]-'0')<<4)+(long)(SID[2]-'0');
    memset( QueueData, '\0', 256 );
    UFC::AnsiString Portfolio( Msg->GetAE() );
    Portfolio.PadThis( 15, ' ' );
    if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures )
        sprintf( QueueData, "%s2000%s%08d%s         ", Msg->GetSrc(), SID.c_str(), (Int32)Msg->GetReportSequence(), Portfolio.c_str() );
    else
        sprintf( QueueData, "%s3000%s%08d%s         ", Msg->GetSrc(), SID.c_str(), (Int32)Msg->GetReportSequence(), Portfolio.c_str() );
    UFC::UQueueData OrderData( MType, QueueData, 256 );                                
    
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::Printf( " <FUT CONFIRM> <Mtype:0x%x> <%s>", MType, QueueData );                                                
            if( DumpOnly == FALSE )
                FFUTConfirmQueue.Send( OrderData );     
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " <OPT CONFIRM> <Mtype:0x%x> <%s>",MType, QueueData );                                                            
            if( DumpOnly == FALSE )
                FOPTConfirmQueue.Send( OrderData );     
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void ChinaTrustExecConnection::FillToBackend( TExecutionReportMessage* Msg, bool WithProtfolio , bool IsFilled )
{
    UFC::AnsiString SID( Msg->GetPVC() );
    long MType;
    char QueueData[ 256 ];

    SID.PadThis( 3, '0' );
    MType = 0x70000000 + ((long)(SID[0]-'0')<<8) + ((long)(SID[1]-'0')<<4)+(long)(SID[2]-'0');
    if( WithProtfolio )
    {
        UFC::AnsiString Portfolio( Msg->GetAE() );
        Portfolio.PadThis( 15, ' ' );
        UFC::BufferedLog::Printf( " Protfilio:[%s]", Portfolio.c_str() );
        sprintf( QueueData, "%s%s         ", Msg->GetSrc(), Portfolio.c_str() );
    }
    else
        sprintf( QueueData, "%s         ", Msg->GetSrc());
    if( NoQuoteCancel == TRUE && QueueData[0] == '4' )
        return; ///< skip the quote canceled execution.
    UFC::UQueueData OrderData( MType, QueueData, 256 );                                
    
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::Printf( " <FUT Fill> <Mtype:0x%x> <%s>",MType, QueueData );                                                
            if( DumpOnly == FALSE )
            {
                FFUTFillQueue.Send( OrderData );
                if( FSecondFUTFillQueue.IsOpened() == TRUE && IsFilled == TRUE )
                    FSecondFUTFillQueue.Send( OrderData );
            }
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " <OPT Fill> <Mtype:0x%x> <%s>",MType, QueueData );
            if( DumpOnly == FALSE )
            {
                FOPTFillQueue.Send( OrderData );
                if( FSecondOPTFillQueue.IsOpened() == TRUE && IsFilled == TRUE )
                    FSecondOPTFillQueue.Send( OrderData );
            }
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void ChinaTrustExecConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
    try
    {
        switch( Msg->GetOrderStatus() )
        {
            case nsOrderMessageDefine::osNew:
            case nsOrderMessageDefine::osQuoteAccept:///< Quote accept.
                 ConfirmToBackend( Msg );
                 break;
            case nsOrderMessageDefine::osReplaced:
                if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                        Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
                    ConfirmToBackend( Msg );
                else
                    FillToBackend( Msg, false );
                break;
            case nsOrderMessageDefine::osExpired: ///< Quote expired.
            case nsOrderMessageDefine::osCanceled:
                if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetNewAndFilled )
                    FillToBackend( Msg, true );
                else
                    FillToBackend( Msg, false );
                 break;
            case nsOrderMessageDefine::osPartiallyFilled:
            case nsOrderMessageDefine::osFilled:
                 FillToBackend( Msg, true, true );
                 break;
            case nsOrderMessageDefine::osPendingNew:
            case nsOrderMessageDefine::osPendingReplace:
            case nsOrderMessageDefine::osPendingCancel:
            case nsOrderMessageDefine::osRejected:
                 default: break;
        }
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::Printf( " Exception[%s]", e.what() );
    }
}
//------------------------------------------------------------------------------
void ChinaTrustExecConnection::Execute( void )
{      
    while( !IsTerminated() )
    {
        if( FIsLogon == FALSE )
            FConnection->Connect( FIP.c_str() , FPort, 5 );    
        sleep( 10 ); ///< wait 10 sec.
    }
}
//------------------------------------------------------------------------------
