#include "FubonDBTFXExecution.h"
#include "../../UFC/NetUtility.h"
//------------------------------------------------------------------------------
extern BOOL            DumpOnly;
extern BOOL            RecoverMode;
//------------------------------------------------------------------------------
YuantaExecConnection::YuantaExecConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName( "FubonDBTFXExecution" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "Admin" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F021999" )
,FRecoverBeginTime( RecoverBeginTime )
,FConnection( NULL )
,FIsLogon( FALSE )
{      
    UFC::AnsiString LogName = "../log/FBDBTFXExec" + UFC::GetDateString() + ".log";
    LoadSetting(  );

    if( RecoverMode == TRUE )
    {
        FFUTConfirmFile = new UFC::FileStreamEx( FFUTCFileName, "r+" );
        FFUTFillFile    = new UFC::FileStreamEx( FFUTFFileName, "r+" );
        FOPTConfirmFile = new UFC::FileStreamEx( FOPTCFileName, "r+" );
        FOPTFillFile    = new UFC::FileStreamEx( FOPTFFileName, "r+" );        
        FFUTConfirmFile->Seek( 0, UFC::soFromEnd );
        FFUTFillFile->Seek( 0, UFC::soFromEnd );
        FOPTConfirmFile->Seek( 0, UFC::soFromEnd );
        FOPTFillFile->Seek( 0, UFC::soFromEnd );                
    }
    else
    {
        FFUTConfirmFile = new UFC::FileStreamEx( FFUTCFileName, "w+" );
        FFUTFillFile    = new UFC::FileStreamEx( FFUTFFileName, "w+" );
        FOPTConfirmFile = new UFC::FileStreamEx( FOPTCFileName, "w+" );
        FOPTFillFile    = new UFC::FileStreamEx( FOPTFFileName, "w+" );                
    }
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( " Dump to screen only.");
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   Fubon TAIFEX backend interface module        " );
    UFC::BufferedLog::Printf( "   startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On (Dump to stdout only)");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");    
    UFC::BufferedLog::Printf( "__________________________________________" );       
    ///< Create the connection object.
    FConnection = new TTaifexConnection( FAppName, this );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.
    //FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
    //FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
    //FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
    //FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
    FConnection->SetFutDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetOptDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );	
    FConnection->SetQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );                            
    ///< Set log file name.    
    FConnection->SetLogFile( LogName );
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
void YuantaExecConnection::LoadSetting( void )
{
    try
    {        
        UFC::UiniFile   Config( "../cfg/FubonDBTFXExecution.cfg" );
        UFC::AnsiString FutPath,OptPath;
        
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
        if( Config.GetValue( "Futures", "FilePath", FutPath ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Futures FilePath not set.");
            exit(1);
        }         
        if( Config.GetValue( "Options", "FilePath", OptPath ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Options ConfirmFileName not set.");
            exit(1);
        }         
        UFC::UDate Today;
        Today.setCurrent();
        
        FFUTCFileName.Printf( "%s/FCFile%02d%02d.%04d",FutPath.c_str(), Today.getMonth(),Today.getDate(),Today.getYear() );
        FFUTFFileName.Printf( "%s/FFFile%02d%02d.%04d",FutPath.c_str(), Today.getMonth(),Today.getDate(),Today.getYear() );                
        FOPTCFileName.Printf( "%s/OCFile%02d%02d.%04d",OptPath.c_str(), Today.getMonth(),Today.getDate(),Today.getYear() );
        FOPTFFileName.Printf( "%s/OFFile%02d%02d.%04d",OptPath.c_str(), Today.getMonth(),Today.getDate(),Today.getYear() );                
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" FubonDBTFXExecution.cfg not found.");
        exit(1);
    }
}
//------------------------------------------------------------------------------
YuantaExecConnection::~YuantaExecConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
void YuantaExecConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( 20072, FID.c_str(), FPassword.c_str(), FAccount.c_str() );    
}
//------------------------------------------------------------------------------
void YuantaExecConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void YuantaExecConnection::OnLogonReply( const char* ReplyString, bool IsAccepted, int CID )
{
    if( IsAccepted )
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
void YuantaExecConnection::Recover( UFC::AnsiString BeginTime )
{
    if( FIsLogon == TRUE )
    {
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );    
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void YuantaExecConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " FubonDBTFXExecution::OnRecoverFinished( Count = %d )", Count );
    
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
void YuantaExecConnection::ConfirmToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString BRN( Msg->GetPVC() ); 
    UFC::AnsiString AE( Msg->GetData() );    
    UFC::AnsiString Confirm;
   
    AE.PadThis( 6, '0' );
    BRN.PadThis( 3, ' ' );    
    Confirm.Printf( "%s%s%s%08d", Msg->GetSrc(), AE.c_str(), BRN.c_str(),(int)Msg->GetReportSequence() );
    Confirm.PadThis( 120, ' ' );    
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mFutures:
            UFC::BufferedLog::Printf( " [FUT CONFIRM][%s]",Confirm.c_str() );                                                
            if( DumpOnly == FALSE )
            {
                FFUTConfirmFile->Write( Confirm.c_str(), Confirm.Length() );
                FFUTConfirmFile->Flush();
            }
            break;
        case nsOrderMessageDefine::mOptions:
            UFC::BufferedLog::Printf( " [OPT CONFIRM][%s]",Confirm.c_str() );                                                            
            if( DumpOnly == FALSE )
            {
                FOPTConfirmFile->Write( Confirm.c_str(), Confirm.Length() );///<120
                FOPTConfirmFile->Flush();
            }
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void YuantaExecConnection::FillToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString BRN( Msg->GetPVC() ); 
    UFC::AnsiString Fill;
    
    BRN.PadThis( 3, ' ' );    
    Fill.Printf( "%s%s", Msg->GetSrc(), BRN.c_str());
    Fill.PadThis( 160, ' ' );    
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mFutures:
            UFC::BufferedLog::Printf( " [FUT Fill][%s]", Fill.c_str() );                                                
            if( DumpOnly == FALSE )
            {
                FFUTFillFile->Write( Fill.c_str(), Fill.Length());
                FFUTFillFile->Flush();
            }
            break;
        case nsOrderMessageDefine::mOptions:
            UFC::BufferedLog::Printf( " [OPT Fill][%s]", Fill.c_str() );
            if( DumpOnly == FALSE )
            {
                FOPTFillFile->Write( Fill.c_str(), Fill.Length() ); ///< 160
                FOPTFillFile->Flush();
            }
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void YuantaExecConnection::OnExecutionReport( TExecutionReportMessage* Msg, bool PossDup )
{
    //if( Msg->GetMarket() == nsOrderMessageDefine::mTSE || Msg->GetMarket() == nsOrderMessageDefine::mOTC )
    //    return;
    
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osNew:
             ConfirmToBackend( Msg );
             break;
        case nsOrderMessageDefine::osReplaced:            
             if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                 Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )///< ExecType = 'M' or 'm'
                 ConfirmToBackend( Msg );
             else
                 FillToBackend( Msg );
        case nsOrderMessageDefine::osCanceled:
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:            
             FillToBackend( Msg );
             break;                
        case nsOrderMessageDefine::osPendingNew:
        case nsOrderMessageDefine::osPendingReplace:            
        case nsOrderMessageDefine::osPendingCancel:           
        case nsOrderMessageDefine::osRejected:            
        default: break;
    }           
}
//------------------------------------------------------------------------------
void YuantaExecConnection::Execute( void )
{      
    while( !IsTerminated() )
    {
        if( FIsLogon == FALSE )
            FConnection->Connect( FIP.c_str() , FPort );    
        sleep( 10 ); ///< wait 10 sec.
    }
}
//------------------------------------------------------------------------------
