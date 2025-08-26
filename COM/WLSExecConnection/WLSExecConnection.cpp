#include "WLSExecConnection.h"
#include "../../UFC/NetUtility.h"
//------------------------------------------------------------------------------
extern BOOL            FFirstTimeStartup;
extern UFC::AnsiString FConfigFileName;
//------------------------------------------------------------------------------
WLSExecConnection::WLSExecConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName( "WLSExecConnection" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "Admin" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "7790" )
,FRecoverBeginTime( RecoverBeginTime )
,FConnection( NULL )
,FIsLogon( FALSE )
{         
    LoadSetting(  );        
    UFC::AnsiString LogName = FLogPath + "/WLSExec" + UFC::GetDateString() + ".log";
    
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   WLS backend interface module        " );
    UFC::BufferedLog::Printf( "   startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );    
    UFC::BufferedLog::Printf( "__________________________________________" );       
    ///< Create the connection object.
    FConnection = new TTaifexConnection( FAppName, this, FALSE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTSE, FBrokerID.c_str() );    
    ///< Load TSE message format config files.
    FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
    FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
    FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
    FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
    ///< Set log file name.    
    FConnection->SetLogFile( LogName );
    ///< Open Order,Confirm,fill files.    
    OpenFileStream();
    ///< Start running thread.    
    Start();
}
//---------------------------------------------------------------------------
void WLSExecConnection::OpenFileStream( void )
{    	
    UFC::UDate Today;
    int MM = Today.getMonth();
    int DD = Today.getDate();
    int YY = Today.getYear() - 1911;       

    try
    {	
        UFC::AnsiString OrderFileName,ConfirmFileName,FillFileName;
        UFC::AnsiString PINTName( "WLS.OrderPos" );
        
        FReadPos.Attach( PINTName );		        
        if( FFirstTimeStartup == TRUE ) ///< Reset file pos if it's first time startup.
	    FReadPos = 0;	        
        OrderFileName.Printf( "%s/QBOR%02d%02d.%03d", FOrderPath.c_str(), MM, DD, YY );    
        ConfirmFileName.Printf( "%s/QFOR%02d%02d.%03d", FConfirmPath.c_str(), MM, DD, YY );    
        FillFileName.Printf( "%s/QMOR%02d%02d.%03d", FFillPath.c_str(), MM, DD, YY );            	
	
        UFC::BufferedLog::Printf( " Order File[%s]", OrderFileName.c_str());    
        if( FFirstTimeStartup == TRUE ) ///< Reset file pos if it's first time startup.
            FOrderStream   = new UFC::FileStreamEx( OrderFileName, "w+" );
        else
            FOrderStream   = new UFC::FileStreamEx( OrderFileName, "r" );
        UFC::BufferedLog::Printf( " Confirm File[%s]", ConfirmFileName.c_str());    
	FConfirmStream = new UFC::FileStreamEx( ConfirmFileName, "a", TRUE );
        UFC::BufferedLog::Printf( " Fill File[%s]", FillFileName.c_str());    
        FFillStream    = new UFC::FileStreamEx( FillFileName, "a", TRUE );        	
	FOrderStream->Seek( FReadPos.ToInt32(), SEEK_SET );
    }
    catch( UFC::Exception& ex )
    {
	UFC::BufferedLog::Printf( " %s", ex.what() );
        exit( 0 );
    }
}
//------------------------------------------------------------------------------
void WLSExecConnection::LoadSetting( void )
{
    try
    {        
        UFC::UiniFile   Config( FConfigFileName );
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
        if( Config.GetValue( "Path", "Order", FOrderPath ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Set order file path to .");
            FOrderPath = ".";
        }
        if( Config.GetValue( "Path", "Confirm", FConfirmPath ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Set Confirm file path to .");
            FConfirmPath = ".";
        }
        if( Config.GetValue( "Path", "Fill", FFillPath ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Set Fill file path to .");
            FFillPath = ".";
        }
        if( Config.GetValue( "Path", "Log", FLogPath ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Set log file path to .");
            FLogPath = ".";
        }
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" %s not found.", FConfigFileName.c_str() );
        exit(1);
    }
}
//------------------------------------------------------------------------------
WLSExecConnection::~WLSExecConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
void WLSExecConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );
    FIsLogon = TRUE;
}
//------------------------------------------------------------------------------
void WLSExecConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void WLSExecConnection::OnLogonReply( const char* ReplyString, bool IsAccepted, int CID )
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
void WLSExecConnection::Recover( UFC::AnsiString BeginTime )
{
    if( FIsLogon == TRUE )
    {
        FConnection->Recover( BeginTime.c_str(), 2, 4 );    
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void WLSExecConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " WLSExecConnection::OnRecoverFinished( Count = %d )", Count );
    
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
void WLSExecConnection::ConfirmToBackend( TExecutionReportMessage* Msg )
{   
    char WLSConfirm[ 256 ];   
    
    sprintf( WLSConfirm, "%s%s", Msg->GetSrc(), Msg->GetData( ) );          
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTSE:
            UFC::BufferedLog::Printf( " WLS-TSE Confirm[%s]", WLSConfirm );                                                
            FConfirmStream->Write( WLSConfirm, WLS_CONFIRM_LEN );
            FConfirmStream->Flush();
            break;
        case nsOrderMessageDefine::mOTC:
            UFC::BufferedLog::Printf( " WLS-OTC Confirm[%s]", WLSConfirm );                                                            
            FConfirmStream->Write( WLSConfirm, WLS_CONFIRM_LEN );            
            FConfirmStream->Flush();
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void WLSExecConnection::FillToBackend( TExecutionReportMessage* Msg )
{
    
    char WLSFill[ 256 ]; 
    UFC::AnsiString TimeNow;
    
    GetHHMMSS( TimeNow );
    sprintf( WLSFill, "501000%s00%s%s", TimeNow.c_str(), Msg->GetSrc(), Msg->GetData( ) );          
    
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTSE:
            UFC::BufferedLog::Printf( " WLS-TSE Filled[%s]", WLSFill );                                                
            FFillStream->Write( WLSFill, WLS_FILL_LEN );
            FFillStream->Flush();
            break;
        case nsOrderMessageDefine::mOTC:
            UFC::BufferedLog::Printf( " WLS-OTC Filled[%s]", WLSFill );                                                            
            FFillStream->Write( WLSFill, WLS_FILL_LEN );            
            FFillStream->Flush();
            break;
        default:
            break;
    }   
}
//------------------------------------------------------------------------------
void WLSExecConnection::OnExecutionReport( TExecutionReportMessage* Msg, bool PossDup )
{ 
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osNew:
        case nsOrderMessageDefine::osReplaced:            
        case nsOrderMessageDefine::osCanceled:            
        case nsOrderMessageDefine::osRejected:                        
             ConfirmToBackend( Msg );
             break;        
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:            
             FillToBackend( Msg );
             break;                
        case nsOrderMessageDefine::osPendingNew:
        case nsOrderMessageDefine::osPendingReplace:           
        case nsOrderMessageDefine::osPendingCancel:                   
        default: break;
    }           
}
//---------------------------------------------------------------------------
BOOL WLSExecConnection::ReadOrder( void )
{   
    char* DataBuffer = FMessage;
    Int32 LeftSize   = WLS_ORDER_LEN;
    Int32 ReadSize   = 0;

    while( LeftSize > 0 )
    {
        ReadSize = FOrderStream->Read( DataBuffer, LeftSize );        
        if( ReadSize == 0 )
            break;        
        DataBuffer += ReadSize;
        LeftSize   -= ReadSize;
    }
    if( LeftSize == 0 )
    {
        FReadPos += WLS_ORDER_LEN;       
        FMessage[ WLS_ORDER_LEN ] = 0;
        UFC::BufferedLog::Printf( " WLS-Order[%s]", FMessage );    
        return TRUE;
    }
    return FALSE;   
}
//---------------------------------------------------------------------------
void WLSExecConnection::SendOrder( void ) 
{        
    UFC::AnsiString  TSEMsg;
    UFC::AnsiString  WLSMsg( FMessage +39, 47 ); 
    UFC::AnsiString  Header( FMessage, 4);
    UFC::AnsiString  Broker( FMessage+4, 4 );
    UFC::AnsiString  Body( FMessage+8, 31 );    
    TNewOrderMessage OrderMsg;
    
    if( FMessage[0] == '9' )
        OrderMsg.SetMarket( nsOrderMessageDefine::mOTC );
    else
        OrderMsg.SetMarket( nsOrderMessageDefine::mTSE );    
    ///< Compose TSE message.
    TSEMsg.Printf( "%s0012595900%sXX%s",Header.c_str(), Broker.c_str(),Body.c_str() );
    
    OrderMsg.SetSrc( TSEMsg.c_str() ); 
    OrderMsg.SetData( WLSMsg.c_str() );  
    UFC::BufferedLog::Printf( " TSC Msg[%s]", TSEMsg.c_str() );        
    UFC::BufferedLog::Printf( " WSL Msg[%s]", WLSMsg.c_str() );        
    ///< Send order
    FConnection->NewOrder( &OrderMsg );
}     
//------------------------------------------------------------------------------
void WLSExecConnection::Execute( void )
{      
    while( !IsTerminated() )
    {
        if( FIsLogon == FALSE )
        {
            FConnection->Connect( FIP.c_str() , FPort );
            sleep( 5 );
        }
        else
        {
            if( ReadOrder( ) == TRUE )
                SendOrder( );
            else
                UFC::SleepMS( 10 ); ///< wait 10 ms.
        }
    }
}
//------------------------------------------------------------------------------
