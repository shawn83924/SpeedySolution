#include "SinoPacExecConnection.h"
#include "../../UFC/NetUtility.h"
#include <math.h>
//------------------------------------------------------------------------------
#ifndef _FOR_EC
//------------------------------------------------------------------------------
extern BOOL            DumpOnly;
extern UFC::AnsiString FConfigFileName;
extern UFC::AnsiString FRecoverTimeFileName;
//------------------------------------------------------------------------------
SinoPacExecConnection::SinoPacExecConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName( "SinoPacExecConnection" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "Admin" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F021999" )
,FLineNo("XX")
,FService("")          
,FNetwork("")           
,FDaemon("")
,FRecoverBeginTime( RecoverBeginTime )
,FConnection( NULL )
,FIsLogon( FALSE )
,FOrderSubject("STG1.SPDT.FOTORD.T")
,FConfirmSubject( "SPDT.STG1.ORDER.T" )  
,FFillSubject( "SPDT.STG1.MATCH.T" )  
{     
    UFC::AnsiString LogName = "../log/"+ FRecoverTimeFileName + UFC::GetDateString() + ".log";    
    
    UFC::BufferedLog::SetLogObject(new UFC::BufferedLog( LogName, 1024, TRUE));
    LoadSetting(  );    
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( " Dump to screen only.");
    UFC::BufferedLog::Printf( "______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   SinoPac Execution backend interface module " );
    #ifdef __LINUX
        UFC::BufferedLog::Printf( "   Use Tibco RV 7.5.2 Linux Version         " );
        UFC::BufferedLog::Printf( "          ***For backend Linux version***        " );        
    #else
        UFC::BufferedLog::Printf( "   Use Tibco RV 7.5.4 AIX 5.1 Version         " );
        UFC::BufferedLog::Printf( "          ***For backend R6 version***        " );        
    #endif
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On (Dump to stdout only)");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");    
    UFC::BufferedLog::Printf( "   Order Subject[%s]", FOrderSubject.c_str() );        
    UFC::BufferedLog::Printf( "   Confirm Subject[%s]", FConfirmSubject.c_str() );    
    UFC::BufferedLog::Printf( "   - PrefixListDefault[%s]", FConfirmPrefixDef.c_str() );
    UFC::BufferedLog::Printf( "   - TrackingID[%s]", FConfirmTrackingID.c_str() );
    UFC::BufferedLog::Printf( "   - Tracking1[%s]", FConfirmTracking1.c_str() );    
    UFC::BufferedLog::Printf( "   Filled  Subject[%s]",  FFillSubject.c_str() );    
    UFC::BufferedLog::Printf( "   - PrefixListDefault[%s]", FFillPrefixDef.c_str() );
    UFC::BufferedLog::Printf( "   - TrackingID[%s]", FFillTrackingID.c_str() );
    UFC::BufferedLog::Printf( "   - Tracking1[%s]", FFillTracking1.c_str() );        
    UFC::BufferedLog::Printf( "   Service[%s]", FService.c_str() );        
    UFC::BufferedLog::Printf( "   Network[%s]", FNetwork.c_str() );        
    UFC::BufferedLog::Printf( "   Daemon[%s]", FDaemon.c_str() );        
    UFC::BufferedLog::Printf( "   LineNo[%s]",FLineNo.c_str() );        
    UFC::BufferedLog::Printf( "   Log File[%s]",LogName.c_str() );        
    UFC::BufferedLog::Printf( "__________________________________________" );       
    ///< Create the connection object.
    FConnection = new TTaifexConnection( FAppName, this );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.    
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );                            
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );

    FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
    FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
    FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
    FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );    
    ///< Set log file name.    
    FConnection->SetLogFile( LogName );
    InitRV( );
    CreateRVMsg();
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::InitRV( void )
{
    TibrvStatus status;        
    // Open Tibrv
    status = Tibrv::open();
    if (status != TIBRV_OK)
    {
        UFC::BufferedLog::Printf( " Error: could not open TIB/RV, status=%d, text=%s", (int)status, status.getText());
        exit(-1);
    }
    // Initialize the transport with the given parameters or default NULLs.
    status = FRVTransport.create( FService.c_str() ,FNetwork.c_str(), FDaemon.c_str() );
    if (status != TIBRV_OK)
    {
        UFC::BufferedLog::Printf( " Error: could not create transport, status=%d, text=%s",(int)status,status.getText());
        Tibrv::close();
        exit(-1);
    }
    FRVTransport.setDescription( FAppName.c_str() );
    
    // Create listeners for ORDER subject.
    status = FOrderlistener.create( Tibrv::defaultQueue(), this, &FRVTransport,FOrderSubject.c_str());
    if (status != TIBRV_OK)
    {
       UFC::BufferedLog::Printf( " Error: could not create listener on %s, status=%d, text=%s",  FOrderSubject.c_str(),(int)status,status.getText());
       Tibrv::close();
       exit(-1);
    }
    UFC::BufferedLog::Printf( " Listening on: %s", FOrderSubject.c_str() );
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::CreateRVMsg( void )
{
    TibrvStatus status;        
    UFC::AnsiString Today;
    
    GetYYYYMMDD( Today );
    FConfirmPrefixMsg.addString("default",FConfirmPrefixDef.c_str());
    FConfirmTrackingMsg.addString("^id^",FConfirmTrackingID.c_str());
    FConfirmTrackingMsg.addString("^1^[1]",FConfirmTracking1.c_str());
    
    FConfirmDataMsg.addString("^class^","ORDER_DATA");
    FConfirmDataMsg.addString("TDATE",Today.c_str());
    FConfirmDataMsg.addString("BODY","                            ");    
    
    status = FConfirmMsg.setSendSubject( FConfirmSubject.c_str() );
    if (status != TIBRV_OK)
    {   // likely wrong subject specified        
        UFC::BufferedLog::Printf( "Error: could not set subject %s into message, status=%d, text=%s\n",FConfirmSubject.c_str(),(int)status,status.getText());
        Tibrv::close();
        exit(-1);
    }    
    FConfirmMsg.addI32( "^type^", 1);
    FConfirmMsg.addI32( "^pfmt^", 10);
    FConfirmMsg.addI32( "^ver^", 30);
    FConfirmMsg.addI32( "^encoding^", 2);
    FConfirmMsg.addMsg( "^prefixList^", FConfirmPrefixMsg );
    FConfirmMsg.addMsg( "^tracking^", FConfirmTrackingMsg );
    FConfirmMsg.addMsg( "^data^", FConfirmDataMsg );
    
    
    FFillPrefixMsg.addString("default",FFillPrefixDef.c_str());
    FFillTrackingMsg.addString("^id^",FFillTrackingID.c_str());
    FFillTrackingMsg.addString("^1^[1]",FFillTracking1.c_str());
    
    FFillDataMsg.addString("^class^","MATCH_DATA");
    FFillDataMsg.addString("TDATE",Today.c_str());
    FFillDataMsg.addString("BODY","                            ");    
    
    status = FFillMsg.setSendSubject( FFillSubject.c_str() );
    if (status != TIBRV_OK)
    {   // likely wrong subject specified        
        UFC::BufferedLog::Printf( "Error: could not set subject %s into message, status=%d, text=%s\n",FFillSubject.c_str(),(int)status,status.getText());
        Tibrv::close();
        exit(-1);
    }    
    FFillMsg.addI32( "^type^", 1);
    FFillMsg.addI32( "^pfmt^", 10);
    FFillMsg.addI32( "^ver^", 30);
    FFillMsg.addI32( "^encoding^", 2);
    FFillMsg.addMsg( "^prefixList^", FFillPrefixMsg );
    FFillMsg.addMsg( "^tracking^", FFillTrackingMsg );
    FFillMsg.addMsg( "^data^", FFillDataMsg );
}   
//------------------------------------------------------------------------------
void SinoPacExecConnection::LoadSetting( void )
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
        Config.GetValue( "Setting", "Service", FService );
        Config.GetValue( "Setting", "Network", FNetwork );
        Config.GetValue( "Setting", "Daemon",  FDaemon  );        
        Config.GetValue( "Setting", "LineNo",  FLineNo  );
        
        Config.GetValue( "Order", "Subject",  FOrderSubject );
        
        Config.GetValue( "Confirm", "Subject",       FConfirmSubject );
        Config.GetValue( "Confirm", "PrefixDefault", FConfirmPrefixDef );
        Config.GetValue( "Confirm", "TrackingID",    FConfirmTrackingID );
        Config.GetValue( "Confirm", "Tracking1",     FConfirmTracking1 );
        
        Config.GetValue( "Fill", "Subject",       FFillSubject );
        Config.GetValue( "Fill", "PrefixDefault", FFillPrefixDef );
        Config.GetValue( "Fill", "TrackingID",    FFillTrackingID );
        Config.GetValue( "Fill", "Tracking1",     FFillTracking1 );        
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" SinoPacExecConnection.cfg not found.");
        exit(1);
    }
}
//------------------------------------------------------------------------------
SinoPacExecConnection::~SinoPacExecConnection( void )
{
    // Tibrv::close() will destroy the transport and guarantee delivery
    Tibrv::close();
    ///< Delete TTaifexConnection object if exists.
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );    
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
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
void SinoPacExecConnection::Recover( UFC::AnsiString BeginTime )
{
    if( FIsLogon == TRUE )
    {
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );    
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void SinoPacExecConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " SinoPacExecConnection::OnRecoverFinished( Count = %d )", Count );    
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::CheckConnection( void )
{
    if( FIsLogon == FALSE )
        FConnection->Connect( FIP.c_str() , FPort, 10 );    
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::Execute( void )
{      
    TibrvStatus status;            
    
    while( !IsTerminated() )
    {        
        status=Tibrv::defaultQueue()->dispatch();
        if( status != TIBRV_OK )
        {
            UFC::BufferedLog::Printf( " Error: dispatch failed, status=%d, text=%s\n", (int)status,status.getText());
            Tibrv::close();
            exit(-1);
        }
    }
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::ConfirmToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString Src( Msg->GetSrc(), 6 );
    int             Header = Src.ToInt();
    
    if( Header == 300109 || Header == 300209 || Header == 300309 )
        return;
    
    UFC::AnsiString ConfirmBody;
    UFC::AnsiString BrokerID( Msg->GetBrokerID());
    UFC::AnsiString Symbol( Msg->GetSymbol() );
    char pe;
    char bs;
    char ot;
    char tif;
    char sign;
    UFC::AnsiString Price(12);
    UFC::AnsiString INSPDYTIME;
    UFC::AnsiString TFETIME( Msg->GetTransactTime() );
    char SCODE = 'Y';
    char FNCODE;
    int  Status = UFC::AnsiString( Msg->GetStatusCode() ).ToInt(); 
    char err;
    char ltype; 
    char TSession = 'C';      
    UFC::AnsiString PVC( Msg->GetPVC() ); 
    int  PVCID;
    int  SID;     
    UFC::AnsiString Dummy( Msg->GetData() );
    UFC::BufferedLog::Printf( " GIGO:[%s]", Dummy.c_str() );
    
    UFC::AnsiString SubAccount;
    if( Dummy.Length() >= 19 )    
    {
        UFC::AnsiString TmpStr;
        TmpStr = Dummy.SubString( 12, 7 );
        if( TmpStr == "       "  )
            TmpStr = "0000000";
        SubAccount.Printf( "%07d", TmpStr.ToInt() );
        
        UFC::BufferedLog::Printf( " SubAccount:[%s]", SubAccount.c_str() );
    }    
    BrokerID.PadThis( 7, '0' ); 
    Symbol.PadThis( 20, ' ' );
    switch( Msg->GetPositionEffect() ) 
    {
        case nsOrderMessageDefine::peOpen:   pe = '0';break;
        case nsOrderMessageDefine::peClose:  pe = '1';break;
        case nsOrderMessageDefine::peRolled: pe = '2';break;
        case nsOrderMessageDefine::peTMPMarketMaker: return;
        default: pe = '0'; break;
    }  
    if( Msg->GetSide() == nsOrderMessageDefine::sBuy) 
        bs = 'B';
    else
        bs = 'S';    
    if( Msg->GetOrderType() == nsOrderMessageDefine::otMarket) 
        ot = 'M';
    else if( Msg->GetOrderType() == nsOrderMessageDefine::otMarketWithProtection ) 
        ot = 'P';
    else
        ot = 'L';    
    switch( Msg->GetTimeInForce() ) 
    {
        case nsOrderMessageDefine::tifROD:  tif = 'R'; break;
        case nsOrderMessageDefine::tifIOC:  tif = 'I'; break;
        case nsOrderMessageDefine::tifFOK:  tif = 'F'; break;
        default: tif = 'R'; break;
    }
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
        TSession = 'F';
    if( Msg->GetPrice() >= 0 )
       sign = '+';
    else
       sign = '-';    
    Price.Printf("%012.4f", fabs(Msg->GetPrice()));    
    GetHHMMSS( INSPDYTIME );
    INSPDYTIME.PadThis( 8, ' ' );
    TFETIME.PadThis( 8, ' ' );
    
    switch( Msg->GetOrderStatus() ) 
    {
        case nsOrderMessageDefine::osNew:       FNCODE = '1';break;
        case nsOrderMessageDefine::osRejected: 
                                                {
                                                    switch ( Msg->GetCxlRejResponseTo() ) 
                                                    {
                                                        case nsOrderMessageDefine::crrNew:     FNCODE = '1'; break;
                                                        case nsOrderMessageDefine::crrReplace: FNCODE = '2'; break;
                                                        case nsOrderMessageDefine::crrCancel:  FNCODE = '3'; break;
                                                        default:  FNCODE = ' '; break;
                                                    }
                                                    break;
                                                }
        case nsOrderMessageDefine::osReplaced:  if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||  Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
                                                    FNCODE = '4';
                                                else            
                                                    FNCODE = '2';
                                                break;        
        case nsOrderMessageDefine::osCanceled:  FNCODE = '3';
                                                break;
        default:  FNCODE = ' ';break;
    }
    if( Status == 0 || Status == 32 )
        err = 'R';
    else
        err = 'E';
   
    if( PVC.Length() == 2 )
    {
        PVCID = PVC.ToInt();
        SID   = 0;
        ltype = 'X';
    }
    else
    {
        PVCID = 0;
        SID   = PVC.ToInt();
        ltype = 'T';
    }

    PVC.PadThis( 2, '0' ); 
    BrokerID.PadThis( 7, '0' );  
    Dummy.PadThis( 32, ' ' );
 
      
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTWFutures:
            ConfirmBody.Printf("%s%s%s%06d%010d%c%s%c%c%c%c%c%s%05d%s%s%c%c%02d%c%05d%05d%c%s%02d%06d%06d%c%s%s",            
                    BrokerID.c_str(), // 1, %s
                    Msg->GetAccount(), // 2, %s 
                    Msg->GetOrderID(), // 3, %s
                    atoi(Dummy.SubString(4, 6)), // 4, %06d
                    Msg->GetReportSequence(), // 4.1, %010d
                    'F', // 5, %c
                    Symbol.c_str(), // 6, %s
                    pe, // 7, %c
                    bs, // 8, %c
                    ot, // 9, %c
                    tif, // 10, %c
                    sign, // 11, %c
                    Price.c_str(), // 12, %s
                    Msg->GetOrderQty(), // 13, %05d
                    INSPDYTIME.c_str(), // 15, %s
                    TFETIME.c_str(), // 16, %s
                    SCODE, // 17, %c
                    FNCODE, // 18, %c
                    Status, // 19, %02d
                    err, // 20, %c
                    Msg->GetBeforeQty(), // 21, %05d
                    Msg->GetAfterQty(), // 22, %05d
                    ltype, // 23, %c
                    FLineNo.c_str(), // 24, %s
                    PVCID, // 25, %02d
                    SID, // 26, %06d
                    0, // 27, %06d
                    TSession, // 28, %c
                    SubAccount.c_str(), // 29, %s
                    Dummy.c_str()); // 30, %s
                                                
            ConfirmBody.PadThis( 256, ' ' ); 
            UFC::BufferedLog::Printf( " <FUT CONFIRM> <%s>", ConfirmBody.c_str() );                                                    
            break;
        case nsOrderMessageDefine::mTWOptions:
            ConfirmBody.Printf("%s%s%s%06d%010d%c%s%c%c%c%c%c%s%05d%s%s%c%c%02d%c%05d%05d%c%s%02d%06d%06d%c%s%s",            
                    BrokerID.c_str(), // 1, %s
                    Msg->GetAccount(), // 2, %s 
                    Msg->GetOrderID(), // 3, %s
                    atoi(Dummy.SubString(4, 6)), // 4, %06d
                    Msg->GetReportSequence(), // 4.1, %010d
                    'O', // 5, %c
                    Symbol.c_str(), // 6, %s
                    pe, // 7, %c
                    bs, // 8, %c
                    ot, // 9, %c
                    tif, // 10, %c
                    sign, // 11, %c
                    Price.c_str(), // 12, %s
                    Msg->GetOrderQty(), // 13, %05d
                    INSPDYTIME.c_str(), // 15, %s
                    TFETIME.c_str(), // 16, %s
                    SCODE, // 17, %c
                    FNCODE, // 18, %c
                    Status, // 19, %02d
                    err, // 20, %c
                    Msg->GetBeforeQty(), // 21, %05d
                    Msg->GetAfterQty(), // 22, %05d
                    ltype, // 23, %c
                    FLineNo.c_str(), // 24, %s
                    PVCID, // 25, %02d
                    SID, // 26, %06d
                    0, // 27, %06d
                    TSession, // 28, %c
                    SubAccount.c_str(), // 29, %s
                    Dummy.c_str()); // 30, %s
            
            ConfirmBody.PadThis( 256, ' ' ); 
            UFC::BufferedLog::Printf( " <OPT CONFIRM> <%s>", ConfirmBody.c_str() );                                                                        
            break;
        default:
            return;
    }        
    if( DumpOnly == FALSE )
    {
        TibrvStatus status;        
        
        FConfirmDataMsg.updateString( "BODY", ConfirmBody.c_str() );
        FConfirmMsg.updateMsg( "^data^", FConfirmDataMsg );
        FConfirmMsg.setSendSubject( FConfirmSubject.c_str() );
        status = FRVTransport.send( FConfirmMsg );
        if (status != TIBRV_OK)               
            UFC::BufferedLog::Printf( " Error sending message, status=%d, text=%s\n",(int)status,status.getText());    
    }
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::FillToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString Src( Msg->GetSrc() );    
    int             Header = Src.SubString( 0, 3 ).ToInt();
    
    if( Header == 400 && Src[48] == '3' )
        return; ///< Skip quote canceled 
    
    UFC::AnsiString TimeNow;
    UFC::AnsiString ExecSrc("PBOX");
    UFC::AnsiString BRN( Msg->GetLINBRN() );
    UFC::AnsiString Host( UFC::Hostname );
    char ltype; 
    char TSession = 'C';      
    int  PVCID;
    int  SID;    
    UFC::AnsiString PVC( Msg->GetPVC() );  
    UFC::AnsiString TFXMsg( Msg->GetSrc() );
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString Dummmy( Msg->GetData() );
    UFC::AnsiString FillBody;
    UFC::AnsiString QtyString;
    

    GetHHMMSS( TimeNow );
    ExecSrc.PadThis( 10, ' ' ); 
    BRN.PadThis( 10, ' ' ); 
    Host.PadThis( 10, ' ' ); 
       
    if( PVC.Length() == 2 )
    {
        PVCID = PVC.ToInt();
        SID   = 0;
        ltype = 'X';
    }
    else
    {
        PVCID = 0;
        SID   = PVC.ToInt();
        ltype = 'T';
    }
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
        TSession = 'F';
    PVC.PadThis( 2, '0' ); 
    TFXMsg.PadThis( 160, ' ' );    
    AE.PadThis( 7, ' ' );          
    Dummmy.PadThis( 32, ' ' );
    
    QtyString.Printf( "%04d%04d", Msg->GetBeforeQty(), Msg->GetLeavesQty() );
    QtyString.PadThis( 20, ' ' );
    
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTWFutures:
            FillBody.Printf("%s%c%06d%010d%s%s%s%c%s%02d%06d%06d%c%s%s%s%s",
                            TimeNow.c_str(), // 1, %s
                            'F', // 2, %c
                            atoi(Dummmy.SubString(4, 6)), // 3, %06d
                            Msg->GetReportSequence(), // 4, %010d
                            ExecSrc.c_str(), // 5, %s
                            BRN.c_str(), // 6, %s
                            Host.c_str(), // 7, %s
                            ltype, // 8, %c
                            FLineNo.c_str(), // 9, %s
                            PVCID, // 10, %02d
                            SID, // 11, %06d
                            0, // 12, %06d
                            TSession, // 13, %c
                            TFXMsg.c_str(), // 15, %s
                            QtyString.c_str(), // 15.x %s
                            AE.c_str(), // 16, %s
                            Dummmy.c_str()); // 17, %s
            FillBody.PadThis( 292, ' ' ); 
            UFC::BufferedLog::Printf( " <FUT Fill> <%s>", FillBody.c_str() );                                                    
            break;
        case nsOrderMessageDefine::mTWOptions:
            FillBody.Printf("%s%c%06d%010d%s%s%s%c%s%02d%06d%06d%c%s%s%s%s",
                            TimeNow.c_str(), // 1, %s
                            'O', // 2, %c
                            atoi(Dummmy.SubString(4, 6)), // 3, %06d
                            Msg->GetReportSequence(), // 4, %010d
                            ExecSrc.c_str(), // 5, %s
                            BRN.c_str(), // 6, %s
                            Host.c_str(), // 7, %s
                            ltype, // 8, %c
                            FLineNo.c_str(), // 9, %s
                            PVCID, // 10, %02d
                            SID, // 11, %06d
                            0, // 12, %06d
                            TSession, // 13, %c
                            TFXMsg.c_str(), // 15, %s
                            QtyString.c_str(), // 15.x %s
                            AE.c_str(), // 16, %s
                            Dummmy.c_str()); // 17, %s
            FillBody.PadThis( 292, ' ' ); 
            UFC::BufferedLog::Printf( " <OPT Fill> <%s>", FillBody.c_str() );                                                                        
            break;
        default:
            return;
    }        
    if( DumpOnly == FALSE )
    {
        TibrvStatus status;        
        
        FFillDataMsg.updateString( "BODY", FillBody.c_str() );
        FFillMsg.updateMsg( "^data^", FFillDataMsg );
        FFillMsg.setSendSubject( FFillSubject.c_str() );
        status = FRVTransport.send( FFillMsg );
        if (status != TIBRV_OK)               
            UFC::BufferedLog::Printf( " Error sending message, status=%d, text=%s\n",(int)status,status.getText());    
    }
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::FillToCancel( TExecutionReportMessage* Msg )
{    
    char Buffer[ 128 ];  
    char FRecordType   = '3'; ///< 3:Cancel
    char FStatusCode[] = "47";
    char FBuySellCode  = (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? 'B':'S';    
    char FTransCode    = '3'; ///< 2:Replaced 3:Cancel
    char FBeforeQuntity[5];
    char FAfterQuntity[] = "0000";    
    char FSeqNo[] = "00000000"; 
    UFC::AnsiString Symbol( Msg->GetSymbol() );
    MStreamWriter   MemWriter( Buffer );    
    
    Symbol.PadThis( 20, ' ' );
    sprintf( FBeforeQuntity, "%04d", (int)(Msg->GetBeforeQty() - Msg->GetLastQty()) );
    MemWriter.Write( &FRecordType,   1 );
    MemWriter.Write( FStatusCode,    2 );
    MemWriter.Write( Msg->GetBrokerID(),    7 );
    MemWriter.Write( Msg->GetOrderID(),     5 );
    MemWriter.Write( Msg->GetAccount(),     7 );
    MemWriter.Write( Msg->GetAccountFlag(), 1 );
    MemWriter.Write( Symbol.c_str(), 20 );    
    MemWriter.Write( &FBuySellCode,  1 );
    MemWriter.Write( Msg->GetCMID(), 4 );
    MemWriter.Write( &FTransCode,    1 );
    MemWriter.Write( FBeforeQuntity, 4 );
    MemWriter.Write( FAfterQuntity,  4 );
    MemWriter.Write( Msg->GetTransactTime(),   8 );
    MemWriter.Write( FSeqNo,         8 );
    MemWriter.AppendZero();
    Msg->SetLastQty( 0 );
    Msg->SetLeavesQty( 0 );
    Msg->SetReportSequence( 0 );
    Msg->SetSrc( Buffer );    
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{    
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osNew:
        case nsOrderMessageDefine::osReplaced:                         
        case nsOrderMessageDefine::osCanceled:             
        case nsOrderMessageDefine::osRejected:            
             if( Msg->GetPositionEffect() != nsOrderMessageDefine::peTMPMarketMaker )
                 ConfirmToBackend( Msg );            
             break; 
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:            
             FillToBackend( Msg );
             if( Msg->GetTMPStstusCode() == 47 )
             {
                FillToCancel( Msg );///< Convert filled to canceled
                FillToBackend( Msg );
             }
             break;                
        case nsOrderMessageDefine::osPendingNew:
        case nsOrderMessageDefine::osPendingReplace:            
        case nsOrderMessageDefine::osPendingCancel:           
             break;
        default: break;
    }      
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::NewOrder(  UFC::AnsiString& Order ) 
{
    UFC::AnsiString BrokerID = Order.SubString(  1, 7  );
    UFC::AnsiString OrderID  = Order.SubString(  8, 5  );
    UFC::AnsiString Account  = Order.SubString( 13, 7  );
    UFC::AnsiString Flag     = Order.SubString( 20, 1  );
    UFC::AnsiString Symbol   = Order.SubString( 21, 20 );
    UFC::AnsiString Side     = Order.SubString( 41, 1  );
    UFC::AnsiString OrdType  = Order.SubString( 42, 1  );
    //UFC::AnsiString Sign     = Order.SubString( 43, 1  );
    UFC::AnsiString Price    = Order.SubString( 43, 11 );
    UFC::AnsiString Qty      = Order.SubString( 54, 4  );
    UFC::AnsiString TIF      = Order.SubString( 58, 1  );
    UFC::AnsiString PE       = Order.SubString( 59, 1  );
    UFC::AnsiString CMID     = Order.SubString( 60, 4  );  
    // nouse                 = Order.SubString( 64, 1  );
    UFC::AnsiString GIGO     = Order.SubString( 65, 32 );
    TNewOrderMessage Msg;
    
    if( Order[75] == '0' ) ///<0: Futures 1: Option
        Msg.SetMarket( nsOrderMessageDefine::mTWFutures );
    else
        Msg.SetMarket( nsOrderMessageDefine::mTWOptions );
    Msg.SetBrokerID( BrokerID.c_str() );
    Msg.SetAccount( Account.c_str() );        
    Msg.SetSymbol( Symbol.c_str() );
    Msg.SetPrice( Price.ToDouble() );        
    Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
    Msg.SetOrderID( OrderID.c_str() );                
    Msg.SetData( GIGO.c_str() );           
    FConnection->SetCMID( CMID );        
    ///< Set Order Type ( Limit or market )
    if( OrdType[0] == 'M' )
        Msg.SetOrderType( nsOrderMessageDefine::otMarket );
    else if( OrdType[0] == 'm' || OrdType[0] == 'P' )
        Msg.SetOrderType( nsOrderMessageDefine::otMarketWithProtection );	
    else
        Msg.SetOrderType( nsOrderMessageDefine::otLimit );  
    ///< Set TimeInForce ( FOK,IOC,ROD )
    switch( TIF[0] )
    {        
        case 'F': Msg.SetTimeInForce( nsOrderMessageDefine::tifFOK );break;
        case 'I': Msg.SetTimeInForce( nsOrderMessageDefine::tifIOC );break;
        default:  Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );break;
    }
    ///< Set PositionEffect ( Open,Close,DayTrade )
    if( PE == " " )
        Msg.SetPositionEffect(nsOrderMessageDefine::peDayTradeOpen);
    else
    {
        switch( PE.ToInt() )
        {
            case 1:  Msg.SetPositionEffect(nsOrderMessageDefine::peClose);break;
            case 2:  Msg.SetPositionEffect(nsOrderMessageDefine::peRolled);break;
            default: Msg.SetPositionEffect(nsOrderMessageDefine::peOpen);break;
        }
    }
    Msg.SetAccountFlag( Flag.c_str() );
    Msg.SetOrderQty( Qty.ToInt() );
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
    FConnection->NewOrder( &Msg );
}
//------------------------------------------------------------------------------
void SinoPacExecConnection::ReduceOrder(  UFC::AnsiString& Order, BOOL IsReplacePX ) 
{
    UFC::AnsiString BrokerID = Order.SubString(  1, 7  );
    UFC::AnsiString OrderID  = Order.SubString(  8, 5  );
    UFC::AnsiString Account  = Order.SubString( 13, 7  );
    UFC::AnsiString Flag     = Order.SubString( 20, 1  );
    UFC::AnsiString Symbol   = Order.SubString( 21, 20 );
    UFC::AnsiString Side     = Order.SubString( 41, 1  );
    UFC::AnsiString OrdType  = Order.SubString( 42, 1  );
    UFC::AnsiString Price    = Order.SubString( 43, 11 );
    UFC::AnsiString Qty      = Order.SubString( 54, 4  );
    UFC::AnsiString TIF      = Order.SubString( 58, 1  );
    UFC::AnsiString PE       = Order.SubString( 59, 1  );
    UFC::AnsiString CMID     = Order.SubString( 60, 4  );  
    UFC::AnsiString GIGO     = Order.SubString( 65, 32 );
    TReplaceOrderMessage Msg;
    
    if( Order[75] == '0' ) ///<0: Futures 1: Option
        Msg.SetMarket( nsOrderMessageDefine::mTWFutures );
    else
        Msg.SetMarket( nsOrderMessageDefine::mTWOptions );
    Msg.SetBrokerID( BrokerID.c_str() );
    Msg.SetAccount( Account.c_str() );        
    Msg.SetSymbol( Symbol.c_str() );
    Msg.SetPrice( Price.ToDouble() );        
    Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
    Msg.SetOrderID( OrderID.c_str() );                
    Msg.SetData( GIGO.c_str() );           
    FConnection->SetCMID( CMID );        
    ///< Set Order Type ( Limit or market )
    if( OrdType[0] == 'M' )
        Msg.SetOrderType( nsOrderMessageDefine::otMarket );
    else if( OrdType[0] == 'm' || OrdType[0] == 'P' )
        Msg.SetOrderType( nsOrderMessageDefine::otMarketWithProtection );	
    else
        Msg.SetOrderType( nsOrderMessageDefine::otLimit );  
    ///< Set TimeInForce ( FOK,IOC,ROD )
    switch( TIF[0] )
    {        
        case 'F': Msg.SetTimeInForce( nsOrderMessageDefine::tifFOK );break;
        case 'I': Msg.SetTimeInForce( nsOrderMessageDefine::tifIOC );break;
        default:  Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );break;
    }
    ///< Set PositionEffect ( Open,Close,DayTrade )
    switch( PE.ToInt() )
    {
        case 1:  Msg.SetPositionEffect(nsOrderMessageDefine::peClose);break;
        case 2:  Msg.SetPositionEffect(nsOrderMessageDefine::peRolled);break;
        default: Msg.SetPositionEffect(nsOrderMessageDefine::peOpen);break;
    }
    Msg.SetAccountFlag( Flag.c_str() );
    if( IsReplacePX == TRUE )
        Msg.SetOrderQty( 0 );
    else
        Msg.SetOrderQty( Qty.ToInt() );
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
    FConnection->ReplaceOrder( &Msg );    
}   
//------------------------------------------------------------------------------
void SinoPacExecConnection::CancelOrder(  UFC::AnsiString& Order ) 
{
    UFC::AnsiString BrokerID = Order.SubString(  1, 7  );
    UFC::AnsiString OrderID  = Order.SubString(  8, 5  );
    UFC::AnsiString Account  = Order.SubString( 13, 7  );
    UFC::AnsiString Flag     = Order.SubString( 20, 1  );
    UFC::AnsiString Symbol   = Order.SubString( 21, 20 );
    UFC::AnsiString Side     = Order.SubString( 41, 1  );
    UFC::AnsiString OrdType  = Order.SubString( 42, 1  );
    UFC::AnsiString Price    = Order.SubString( 43, 11 );
    UFC::AnsiString Qty      = Order.SubString( 54, 4  );
    UFC::AnsiString TIF      = Order.SubString( 58, 1  );
    UFC::AnsiString PE       = Order.SubString( 59, 1  );
    UFC::AnsiString CMID     = Order.SubString( 60, 4  );  
    UFC::AnsiString GIGO     = Order.SubString( 65, 32 );
    TCancelOrderMessage Msg;
    
    if( Order[75] == '0' ) ///<0: Futures 1: Option
        Msg.SetMarket( nsOrderMessageDefine::mTWFutures );
    else
        Msg.SetMarket( nsOrderMessageDefine::mTWOptions );
    Msg.SetBrokerID( BrokerID.c_str() );
    Msg.SetAccount( Account.c_str() );        
    Msg.SetSymbol( Symbol.c_str() );
    Msg.SetPrice( Price.ToDouble() );        
    Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
    Msg.SetOrderID( OrderID.c_str() );                
    Msg.SetData( GIGO.c_str() );           
    FConnection->SetCMID( CMID );        
    ///< Set Order Type ( Limit or market )
    if( OrdType[0] == 'M' )
        Msg.SetOrderType( nsOrderMessageDefine::otMarket );
    else if( OrdType[0] == 'm' || OrdType[0] == 'P' )
        Msg.SetOrderType( nsOrderMessageDefine::otMarketWithProtection );	
    else
        Msg.SetOrderType( nsOrderMessageDefine::otLimit );  
    ///< Set TimeInForce ( FOK,IOC,ROD )
    switch( TIF[0] )
    {        
        case 'F': Msg.SetTimeInForce( nsOrderMessageDefine::tifFOK );break;
        case 'I': Msg.SetTimeInForce( nsOrderMessageDefine::tifIOC );break;
        default:  Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );break;
    }
    Msg.SetAccountFlag( Flag.c_str() );
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
    FConnection->CancelOrder( &Msg );        
}   
//------------------------------------------------------------------------------
void SinoPacExecConnection::onMsg(TibrvListener* listener, TibrvMsg& msg) 
{
    const char* msgString    = NULL;           
    // Convert the incoming message to a string
    msg.convertToString( msgString );    
    if( msgString != NULL )
    {
    		UFC::BufferedLog::Printf( " Recv[%s]", msgString);    
        UFC::AnsiString rvmsg( msgString );
        int Pos = rvmsg.AnsiPos( "BODY=" );
        UFC::AnsiString OrderBody( rvmsg.c_str() + Pos + 6, 256 ); 
        
        UFC::BufferedLog::Printf( " Recv Order[%s]",OrderBody.c_str());    
        switch( OrderBody[0] )
        {
            case '1': NewOrder( OrderBody );break;
            case '2': ReduceOrder( OrderBody, FALSE );break;
            case '3': CancelOrder( OrderBody );break;
            case '4': ReduceOrder( OrderBody, TRUE );break;
        }        
    }
    else
    	UFC::BufferedLog::Printf( " Can't convert msg to string.");    
}
//------------------------------------------------------------------------------
#endif
    
    
    
    
    
    
    
