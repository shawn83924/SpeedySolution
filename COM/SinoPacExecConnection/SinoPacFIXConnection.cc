#include "SinoPacExecConnection.h"
#include "../../UFC/NetUtility.h"
#include <math.h>
//------------------------------------------------------------------------------
#ifdef _FOR_EC
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
    UFC::BufferedLog::Printf( "   Use Tibco RV 7.5.4 AIX 5.1 Version         " );
    UFC::BufferedLog::Printf( "          ***For EC platform version***       " );            
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
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );                            
    
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
        UFC::BufferedLog::Printf(" %s not found.", FConfigFileName.c_str());
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
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );
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
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );
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
    UFC::AnsiString PVC( Msg->GetPVC() );
    UFC::AnsiString ConfirmBody;   
    UFC::UDate CurrentDate;
    
    
    UFC::AnsiString BrokerID( Msg->GetBrokerID() );    
    UFC::AnsiString Account( Msg->GetAccount() );
    UFC::AnsiString TCODE( "TIM" );
    UFC::AnsiString HTSQ;
    UFC::AnsiString INSQ;
    UFC::AnsiString TDATE;
    UFC::AnsiString TimeNow;
    UFC::AnsiString Symbol( Msg->GetSymbol() );
    char            bs = (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? 'B' : 'S';
    UFC::AnsiString Price;
    UFC::AnsiString ot( "LMT" );
    UFC::AnsiString tif;
    char            DTRADE;
    char            ORDKND;
    UFC::AnsiString SESSION( "  " );
    char            pe;
    char            SCODE;
    char            CDI;
    UFC::AnsiString RTime( Msg->GetTransactTime() ); 
    UFC::AnsiString CODE( Msg->GetStatusCode() );
    UFC::AnsiString ERRMSG( Msg->GetText() );
    UFC::AnsiString AE( Msg->GetAE() ); 
    UFC::AnsiString MSGSEQ;
    char            ConnType; 
    char            TSession = 'I';
    UFC::AnsiString SID;  
    UFC::AnsiString Dummy( Msg->GetData() );
    
    //int Digi = FConnection->GetPricePrecision( Msg->GetMarket(), nsOrderMessageDefine::tsNormal, Symbol );
    Dummy.PadThis( 32, ' ' );   
    
    BrokerID.PadThis( 7, '0' ); 
    Account.PadThis( 7, ' ' ); 
    HTSQ.Printf( "%04d", atoi(Dummy.SubString( 0, 4)) );
    INSQ.Printf( "%06d", atoi(Dummy.SubString( 4, 6)) );
    TDATE.Printf( "%04d/%02d/%02d", CurrentDate.getYear(), CurrentDate.getMonth(), CurrentDate.getDate() );
    GetHHMMSS( TimeNow ); 
    TimeNow.PadThis( 8, ' ' );
    Symbol.PadThis( 30, ' ' );
    
    Price.Printf("%012.4f", Msg->GetPrice());
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
        TSession = 'P';
    if( Msg->GetOrderType() == nsOrderMessageDefine::otMarket) 
        ot = "MKT";
    else if( Msg->GetOrderType() == nsOrderMessageDefine::otMarketWithProtection ) 
        ot = "MKP";
    else
        ot = "LMT";    
    switch( Msg->GetTimeInForce() ) 
    {
        case nsOrderMessageDefine::tifROD:  tif = "ROD"; break;
        case nsOrderMessageDefine::tifIOC:  tif = "IOC"; break;
        case nsOrderMessageDefine::tifFOK:  tif = "FOK"; break;
        default: tif = "ROD"; break;
    }

    DTRADE = ' ';
    ORDKND = '0';
    
    switch( Msg->GetPositionEffect() ) 
    {
        case nsOrderMessageDefine::peOpen:   pe = '0';break;
        case nsOrderMessageDefine::peClose:  pe = '1';break;
        default: pe = ' '; break;
    } 
    
    //SCODE  = 'Y';
    SCODE = *Dummy.SubString( 4 + 6 + 1, 1);
    
    switch( Msg->GetOrderStatus() ) 
    {
        case nsOrderMessageDefine::osNew:       CDI = 'A';
                                                break;
        case nsOrderMessageDefine::osRejected: 
                                                {
                                                    switch ( Msg->GetCxlRejResponseTo() ) 
                                                    {
                                                        case nsOrderMessageDefine::crrNew:     CDI = 'A'; break;
                                                        case nsOrderMessageDefine::crrReplace: if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||  Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
                                                                                                   CDI = 'M';
                                                                                               else            
                                                                                                   CDI = 'C'; break;
                                                        case nsOrderMessageDefine::crrCancel:  CDI = 'D'; break;
                                                        default:  CDI = ' '; break;
                                                    }
                                                    break;
                                                }
        case nsOrderMessageDefine::osReplaced:  if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||  Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
                                                    CDI = 'M';
                                                else            
                                                    CDI = 'C';
                                                break;            
        case nsOrderMessageDefine::osCanceled:  CDI = 'D';
                                                break;
        default:  CDI = ' ';break;
    } 
    RTime.PadThis( 8, '0' );
    if (CODE.ToInt() != 0) 
        CODE.Printf( "99%s", Msg->GetStatusCode() );
    else
        CODE.PadThis( 4, '0' );
    
    ERRMSG.PadThis( 60, ' ' );
    AE.PadThis( 7, ' ' ); 
    MSGSEQ.Printf( "%d", Msg->GetReportSequence() );
    MSGSEQ.PadThis( 10, ' ' );
    
    if( PVC.Length() == 2 )
    {
        SID   = "0";
        ConnType = 'X';
    }
    else
    {
        SID   = PVC;
        ConnType = 'T';
    }    
     
    SID.PadThis( 6, ' ' );               
       

    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTWFutures:            
            ConfirmBody.Printf("%c%s%s%s%s%s%s%s%c%s%c%04d%s%s%s%c%c%s%c%c%c%s%04d%04d%s%s%s%s%s%c%s%s", 
                    TSession, // 1, %c
                    BrokerID.c_str(), // 2, %s 
                    Account.c_str(), // 3, %s
                    TCODE.c_str(), // 4, %s                  
                    HTSQ.c_str(), // 5, %s
                    INSQ.c_str(), // 6, %s
                    TDATE.c_str(), // 7, %s
                    TimeNow.c_str(), // 8, %s
                    'F', // 9, %c
                    Symbol.c_str(), // 10, %s
                    bs, // 11, %c
                    Msg->GetOrderQty(), // 12, %04d
                    Price.c_str(), // 13, %s
                    ot.c_str(), // 14, %s
                    tif.c_str(), // 15, %s
                    DTRADE, // 16, %c
                    ORDKND, // 17, %c
                    SESSION.c_str(), // 18, %s
                    pe, // 19, %c
                    SCODE, // 20, %c
                    CDI, // 21, %c
                    Msg->GetOrderID(), // 22, %s
                    Msg->GetBeforeQty(), // 23, %04d
                    Msg->GetAfterQty(), // 24, %04d
                    RTime.c_str(), // 25, %s
                    CODE.c_str(), // 26, %s
                    ERRMSG.c_str(), // 27, %s
                    AE.c_str(), // 28, %s
                    MSGSEQ.c_str(), // 29, %s
                    ConnType, // 30, %c
                    SID.c_str(), // 31, %s
                    Dummy.c_str() // 32, %s
            );
            ConfirmBody.PadThis( 256, ' ' ); 
            UFC::BufferedLog::Printf( " <FUT CONFIRM> <%s>", ConfirmBody.c_str() );                                                    
            break;
        case nsOrderMessageDefine::mTWOptions:
            ConfirmBody.Printf("%c%s%s%s%s%s%s%s%c%s%c%04d%s%s%s%c%c%s%c%c%c%s%04d%04d%s%s%s%s%s%c%s%s", 
                    TSession, // 1, %c
                    BrokerID.c_str(), // 2, %s 
                    Account.c_str(), // 3, %s
                    TCODE.c_str(), // 4, %s                  
                    HTSQ.c_str(), // 5, %s
                    INSQ.c_str(), // 6, %s
                    TDATE.c_str(), // 7, %s
                    TimeNow.c_str(), // 8, %s
                    'O', // 9, %c
                    Symbol.c_str(), // 10, %s
                    bs, // 11, %c
                    Msg->GetOrderQty(), // 12, %04d
                    Price.c_str(), // 13, %s
                    ot.c_str(), // 14, %s
                    tif.c_str(), // 15, %s
                    DTRADE, // 16, %c
                    ORDKND, // 17, %c
                    SESSION.c_str(), // 18, %s
                    pe, // 19, %c
                    SCODE, // 20, %c
                    CDI, // 21, %c
                    Msg->GetOrderID(), // 22, %s
                    Msg->GetBeforeQty(), // 23, %04d
                    Msg->GetAfterQty(), // 24, %04d
                    RTime.c_str(), // 25, %s
                    CODE.c_str(), // 26, %s
                    ERRMSG.c_str(), // 27, %s
                    AE.c_str(), // 28, %s
                    MSGSEQ.c_str(), // 29, %s
                    ConnType, // 30, %c
                    SID.c_str(), // 31, %s
                    Dummy.c_str() // 32, %s
            );
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
    UFC::AnsiString PVC( Msg->GetPVC() );    
    UFC::AnsiString FillBody;
    UFC::UDate CurrentDate;
    
    
    UFC::AnsiString BrokerID( Msg->GetBrokerID() );   
    UFC::AnsiString Account( Msg->GetAccount() );
    UFC::AnsiString TCODE( "TIM" );
    UFC::AnsiString TDATE;
    UFC::AnsiString INSQ;
    UFC::AnsiString ORDNO( Msg->GetOrderID() );  
    UFC::AnsiString Symbol( Msg->GetSymbol() );
    char            BS = (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? 'B' : 'S';
    char            pe;
    UFC::AnsiString TQTY;
    UFC::AnsiString Price(12);
    char            BS1;
    UFC::AnsiString PRICE1(12);
    UFC::AnsiString MQTY1; 
    char            BS2;
    UFC::AnsiString PRICE2(12);
    UFC::AnsiString MQTY2;
    UFC::AnsiString TimeNow;
    UFC::AnsiString MSEQ;
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString MSGSEQ;
    char            ltype;
    char            TSession = 'I';
    UFC::AnsiString SID;
    UFC::AnsiString Dummmy( Msg->GetData() );
         
    Dummmy.PadThis( 32, ' ' );
    
    BrokerID.PadThis( 7, '0' ); 
    Account.PadThis( 7, ' ' );
    TDATE.Printf( "%04d/%02d/%02d", CurrentDate.getYear(), CurrentDate.getMonth(), CurrentDate.getDate() );
    INSQ.Printf( "%06d", atoi(Dummmy.SubString( 4, 6 )) );
    ORDNO.PadThis( 5, ' ' );
    Symbol.PadThis( 30, ' ' );
    
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
        TSession = 'P';
    switch( Msg->GetPositionEffect() ) 
    {
        case nsOrderMessageDefine::peOpen:   pe = '0';break;
        case nsOrderMessageDefine::peClose:  pe = '1';break;
        default: pe = ' '; break;
    } 
      
    TQTY.Printf( "%04d", Msg->GetOrderQty() );  
    Price.Printf("%012.4f", Msg->GetPrice());
    
    BS1 = (Msg->GetLegSide1() == nsOrderMessageDefine::sBuy) ? 'B' : 'S';
    PRICE1.Printf("%012.4f", Msg->GetLegPrice1());
    MQTY1.Printf( "%04d", Msg->GetLegQty1() ); 
    
    BS2 = (Msg->GetLegSide2() == nsOrderMessageDefine::sBuy) ? 'B' : 'S';
    PRICE2.Printf("%012.4f", Msg->GetLegPrice2());
    MQTY2.Printf( "%04d", Msg->GetLegQty2() );
    
    GetHHMMSS( TimeNow ); 
    TimeNow.PadThis( 8, ' ' );
    MSEQ.Printf( "%d", Msg->GetReportSequence() );
    MSEQ.PadThis( 8, ' ' );
    AE.PadThis( 7, ' ' );    
    MSGSEQ.Printf( "%d", Msg->GetReportSequence() );
    MSGSEQ.PadThis( 10, ' ' );
    
    if( PVC.Length() == 2 )
    {
        SID   = "0";
        ltype = 'X';
    }
    else
    {
        SID   = PVC;
        ltype = 'T';
    }  
    
    SID.PadThis( 6, ' ' ); 
  
    PVC.PadThis( 2, '0' );     
    
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTWFutures:
            FillBody.Printf("%c%s%s%s%s%s%s%c%s%c%c%s%s%c%s%s%c%s%s%s%s%s%s%c%s%s", 
                    TSession, // 1, %c
                    BrokerID.c_str(), // 2, %s
                    Account.c_str(), // 3, %s
                    TCODE.c_str(), // 4, %s
                    TDATE.c_str(), // 5, %s
                    INSQ.c_str(), // 6, %s
                    ORDNO.c_str(), // 7, %s
                    'F', // 8, %c
                    Symbol.c_str(), // 9, %s 
                    BS, // 10, %c
                    pe, // 11, %c 
                    TQTY.c_str(), // 12, %s 
                    Price.c_str(), // 13, %s                  
                    BS1, // 14, %c
                    PRICE1.c_str(), // 15, %s
                    MQTY1.c_str(), // 16, %s  
                    BS2, // 17, %c
                    PRICE2.c_str(), // 18, %s
                    MQTY2.c_str(), // 19, %s
                    TimeNow.c_str(), // 20, %s
                    MSEQ.c_str(), // 21, %s
                    AE.c_str(), // 22, %s
                    MSGSEQ.c_str(), // 29, %s
                    ltype, // 30, %c
                    SID.c_str(), // 31, %s
                    Dummmy.c_str() // 32, %s
            );
            FillBody.PadThis( 292, ' ' ); 
            UFC::BufferedLog::Printf( " <FUT Fill> <%s>", FillBody.c_str() );                                                    
            break;
        case nsOrderMessageDefine::mTWOptions:
            FillBody.Printf("%c%s%s%s%s%s%s%c%s%c%c%s%s%c%s%s%c%s%s%s%s%s%s%c%s%s", 
                    TSession, // 1, %c
                    BrokerID.c_str(), // 2, %s
                    Account.c_str(), // 3, %s
                    TCODE.c_str(), // 4, %s
                    TDATE.c_str(), // 5, %s
                    INSQ.c_str(), // 6, %s
                    ORDNO.c_str(), // 7, %s
                    'O', // 8, %c
                    Symbol.c_str(), // 9, %s 
                    BS, // 10, %c
                    pe, // 11, %c 
                    TQTY.c_str(), // 12, %s 
                    Price.c_str(), // 13, %s                  
                    BS1, // 14, %c
                    PRICE1.c_str(), // 15, %s
                    MQTY1.c_str(), // 16, %s  
                    BS2, // 17, %c
                    PRICE2.c_str(), // 18, %s
                    MQTY2.c_str(), // 19, %s
                    TimeNow.c_str(), // 20, %s
                    MSEQ.c_str(), // 21, %s
                    AE.c_str(), // 22, %s
                    MSGSEQ.c_str(), // 29, %s
                    ltype, // 30, %c
                    SID.c_str(), // 31, %s
                    Dummmy.c_str() // 32, %s
            );
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
        case nsOrderMessageDefine::osRejected: 
        case nsOrderMessageDefine::osReplaced:                         
        case nsOrderMessageDefine::osCanceled:
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
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );
    FConnection->NewOrder( &Msg ); 
}
//------------------------------------------------------------------------------
#endif  ///< _FOR_EC
//------------------------------------------------------------------------------
    
    
    
    
    
    
    
