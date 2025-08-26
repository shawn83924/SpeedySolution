#include "CaptialExecConnection.h"
#include "../../UFC/NetUtility.h"
#include "TUserData.h"
//------------------------------------------------------------------------------
extern ExecSessionType ExecSession;
extern UFC::PInt32     FRecoverBeginTime;
extern UFC::AnsiString AppName;
extern BOOL DumpOnly;
extern BOOL EnableB50Cancel;
extern void Send( int type, const UFC::AnsiString& data );
extern void SendAlert(const UFC::AnsiString& Msg);
//------------------------------------------------------------------------------
UFC::AnsiString CaptialExecConnection::ApKind = "s";
//------------------------------------------------------------------------------
CaptialExecConnection::CaptialExecConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "Admin" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F020999" )
,FRecoverTime( RecoverBeginTime )
,FFistTimeRecover(TRUE)
,FSendQuoteExec( TRUE )
,FConnection( NULL )
,FIsLogon( FALSE )
{      
    LoadSetting(  );    
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( " Dump to screen only.");
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   Capital backend interface module           " );
    UFC::BufferedLog::Printf( "   startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On (Dump to stdout only)");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");    
    UFC::BufferedLog::Printf( "__________________________________________" );       
    ///< Create the connection object.
    FConnection = new TTaifexConnection( AppName, this );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.
    //FConnection->SetFutDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    //FConnection->SetOptDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    //FConnection->SetQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
    ///< Set log file name.    
    //FConnection->SetLogFile( LogName );
    //UFC::BufferedLog::SetDebugMode(TRUE);
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
void CaptialExecConnection::LoadSetting( void )
{
    UFC::AnsiString ConfigName,Value;
    ConfigName.Printf("../cfg/%s.cfg",AppName.c_str());
    try
    {        
        UFC::UiniFile   Config( ConfigName );
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
        if( Config.GetValue( "Setting", "ExcludeBrokerID", Value )  )
        {
            //FExcludeBrokerID.SetLength(0);
            UFC::PStringList Brokers;
            Brokers.SetStrings( Value, "," );
            for(register int i = 0; i < Brokers.ItemCount(); i++ )
            {
                FExcludeBrokerIDSet.Add( Brokers.GetItem(i) );
                UFC::BufferedLog::Printf( " Exclude BrkID%02d:[%s]", i, Brokers.GetItem(i).c_str() );
            }
        }
        if( Config.GetValue( "Setting", "SendQuoteExec", Value ) == FALSE )
        {
            UFC::BufferedLog::Printf(" [Setting] SendQuoteExec not set.");
            exit(1);
        }
        FSendQuoteExec = Value.ToInt();
        if( Config.GetValue( "Setting", "ApKind", CaptialExecConnection::ApKind ) == FALSE )
        {
            UFC::BufferedLog::Printf(" ApKind not set.  Use default ApKind:[s]" );
        }

    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" %s not found.",ConfigName.c_str());
        exit(1);
    }
}
//------------------------------------------------------------------------------
CaptialExecConnection::~CaptialExecConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
void CaptialExecConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );
    FIsLogon = TRUE;
    UFC::AnsiString log;
    log.Printf("%s Connected",AppName.c_str());
    SendAlert(log);
}
//------------------------------------------------------------------------------
void CaptialExecConnection::OnDisconnected( void )
{
    if( FFistTimeRecover == FALSE )
        FRecoverTime = FRecoverBeginTime.ToInt32();
    UFC::BufferedLog::Printf( "-- *** Connection broken *** --" );
    FIsLogon = FALSE;
    UFC::AnsiString log;
    log.Printf("%s Connected broken",AppName.c_str());
    SendAlert(log);
}
//------------------------------------------------------------------------------
void CaptialExecConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk )
    {
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverTime );
        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, rmAll  )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll  );
        FFistTimeRecover = FALSE;
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void CaptialExecConnection::Recover( const UFC::AnsiString& BeginTime )
{
    if( FIsLogon == TRUE )
    {
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void CaptialExecConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " CaptialExecConnection::OnRecoverFinished( Count = %d )", Count );
    
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
Int32 CaptialExecConnection::StringToInt32( const UFC::AnsiString& StringValue, Int32 DefaultValue )
{
    if( StringValue.Length() > 0 )
    {
        Int32 data = StringValue.ToInt();
        //UFC::BufferedLog::Printf( " CaptialExecConnection::StringToInt32(%s) = %d", StringValue.c_str(),data );
        return data;
    }
    return DefaultValue;
}
//------------------------------------------------------------------------------
void CaptialExecConnection::getSymbolInfo( const UFC::AnsiString& Symbol, nsOrderMessageDefine::SideEnum Side ,UFC::AnsiString& Symbol1,UFC::AnsiString& Symbol2 )
{
    char BS_Code =' ';
    switch(Side)
    {
        case nsOrderMessageDefine::sNone:
            BS_Code = ' ';
            break;
        case nsOrderMessageDefine::sBuy:
            BS_Code = 'B';
            break;
        case nsOrderMessageDefine::sSell:
            BS_Code = 'S';
            break;
    }
    BO_SymbolDeCompose symbolDeCompose(Symbol,BS_Code);
    BOOL isSetSymbol2 = FALSE;
    char CP1 =' ';
    char CP2 =' ';
    switch(symbolDeCompose.FStrategicType)
    {
        case BO_SymbolDeCompose::stSingle:
            isSetSymbol2 = TRUE;
            Symbol2.Printf("%s","       000000000000000 ");
        case BO_SymbolDeCompose::stCallPriceSpread:
        case BO_SymbolDeCompose::stPutPriceSpread:
        case BO_SymbolDeCompose::stCallTimeSpread:
        case BO_SymbolDeCompose::stPutTimeSpread:
        case BO_SymbolDeCompose::stStraddle:
        case BO_SymbolDeCompose::stStrangle:
        case BO_SymbolDeCompose::stConRev:
            CP1 = symbolDeCompose.FCP_Code1;
            CP2 = symbolDeCompose.FCP_Code2;
            break;
        case BO_SymbolDeCompose::stFuturesSingle:
            isSetSymbol2 = TRUE;
            Symbol2.Printf("%s","       000000000000000 ");
        case BO_SymbolDeCompose::stFuturesTimeSpread:
        case BO_SymbolDeCompose::stUnKnown:			// Invalid Symbol Format
            CP1 = ' ';
            CP2 = ' ';
            break;
    }
    UFC::AnsiString comm1(symbolDeCompose.FComm1);
    comm1.PadThis(3,' ');
    char data1[4];
    memcpy( data1, comm1.c_str(), 4 );
    comm1.Printf("%s   %c",data1,CP1);
    char bs1 = ' ';
    if( symbolDeCompose.FBS_Code1 == 'S')
        bs1 = '1';
    Symbol1.Printf("%s%s%s%c",comm1.c_str(),symbolDeCompose.FSettleMonth1,symbolDeCompose.FExcercisePrice1,bs1);
    if( isSetSymbol2 == FALSE )
    {
        UFC::AnsiString comm2(symbolDeCompose.FComm2);
        comm2.PadThis( 3, ' ' );
        char data2[4];
        memcpy( data2, comm2.c_str(), 4 );
        comm2.Printf("%s   %c", data2, CP2 );
        char bs2 = ' ';
        if( symbolDeCompose.FBS_Code2 == 'S')
            bs2 = '1';
        Symbol2.Printf("%s%s%s%c",comm2.c_str(),symbolDeCompose.FSettleMonth2,symbolDeCompose.FExcercisePrice2,bs2);
    }
}
//------------------------------------------------------------------------------
void CaptialExecConnection::ConfirmToBackend( TExecutionReportMessage* Msg , BOOL isQuoteAccept )
{
    UFC::AnsiString IP( UFC::PSocket::GetLocalIP() );
    UFC::AnsiString BrokerID( Msg->GetBrokerID() );//Src() + 14, 7
    UFC::AnsiString PVC(Msg->GetPVC());
    UFC::AnsiString _IBNO( Msg->GetLINBRN() );
    if( isQuoteAccept == TRUE )
    	UFC::BufferedLog::DebugPrintf( " Quote Accept" );
    
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour && ExecSession == esNormal ||
        Msg->GetTradingSessionID() == nsOrderMessageDefine::tsNormal && ExecSession == esOffHour )
        return;
    if( FSendQuoteExec == FALSE  )
    {
        if( isQuoteAccept == TRUE || *(Msg->GetSrc()) == '4' )
            return;
    }

    BrokerID.PadThis( 7, ' ' );
    if( _IBNO.Length() > 3 )
        _IBNO = _IBNO.SubString(_IBNO.Length()-3,3);
    
    UFC::AnsiString IBNO(_IBNO.c_str());//"000";
    IBNO.PadThis( 3,' ' );
    UFC::AnsiString OrderID( Msg->GetOrderID() );
    OrderID.PadThis( 5,' ' );
    IP.PadThis( 16, ' ' );

    char offsetkind = '1';
    char tradeid =' ';
    switch( Msg->GetPositionEffect() )
    {
        case nsOrderMessageDefine::peOpen:    //0
            offsetkind = ' ';
            break;
        case nsOrderMessageDefine::peClose:   //1
            offsetkind = '1';
            break;
        case nsOrderMessageDefine::peRolled:  //2
            tradeid = 'Y';
            break;
        default: break;
    }
    char ordertype = ' ';
    switch( Msg->GetOrderType() )
    {
        case nsOrderMessageDefine::otNone:
            ordertype = ' ';
            break;
        case nsOrderMessageDefine::otMarket:
            ordertype = 'M';
            break;
        case nsOrderMessageDefine::otLimit:
            ordertype = 'L';
            break;
        case nsOrderMessageDefine::otMarketWithProtection:
            ordertype = 'P';
            break;
        default:
            ordertype = 'L';
            break;
    }
    int functionkind = 0;
    switch( Msg->GetOrderStatus() )
    {        
        case nsOrderMessageDefine::osNew:             
        case nsOrderMessageDefine::osQuoteAccept:             
             if( Msg->GetExecType() == nsOrderMessageDefine::etOrderStatus )
                 functionkind = 5;
             else
                 functionkind = 1;
             break;
        case nsOrderMessageDefine::osReplaced:
            if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                    Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
                functionkind = 4;
            else
                functionkind = 2;
             break;
        case nsOrderMessageDefine::osCanceled:
             functionkind = 3;
             break;
        default:  break;
    }
    UFC::AnsiString Symbol1, Symbol2;
    char tif ='3';
    if( isQuoteAccept == FALSE )
    {
        switch(Msg->GetTimeInForce())
        {
            case nsOrderMessageDefine::tifNone:
            case nsOrderMessageDefine::tifROD:
                tif = ' ';
                break;
            case nsOrderMessageDefine::tifFOK:
                tif = '1';
                break;
            case nsOrderMessageDefine::tifIOC:
                tif = '2';
                break;
            default: break;
        }
    }
    getSymbolInfo( Msg->GetSymbol(), Msg->GetSide(), Symbol1, Symbol2);

    Int64 price = UFC::DoubleToInt64( Msg->GetPrice(), 4 );

    ///< fetch user data
    TUserData UserData( Msg->GetData() );
    UserData.DumpData();
    nsOrderMessageDefine::MarketEnum msgMarket = Msg->GetMarket();
    if ((msgMarket == nsOrderMessageDefine::mTWOptions) || 
        (msgMarket == nsOrderMessageDefine::mForeignFutures) ||
        (msgMarket == nsOrderMessageDefine::mCNOptions))
    {
        UFC::AnsiString userPE = UserData.GetPE();
        if ((userPE.Length() > 0) && (userPE == "2")) offsetkind = '2';
    }
   
    UFC::AnsiString today;
    UFC::GetYYYYMMDD( today );
    char ww_conn_type[2]={0,0};
    if( PVC.Length() == 2 )
    {
        ww_conn_type[0] = 'x';
    }
    else if( PVC.Length() == 3 )
    {
        PVC.Delete(0,1);
        ww_conn_type[0] = 't';
    }
    else
    {
        if( PVC.Length() > 2 )
        {
            PVC.Delete(0,PVC.Length()-2);
        }
        ww_conn_type[0] = ' ';
    }
    PVC.Printf("%02d",StringToInt32(PVC,0));

    UFC::AnsiString DATA( Msg->GetData() );
    UFC::AnsiString key_no;
    Int32 pos = DATA.AnsiPos("key_no=");
    if( pos > -1 )
    {
        UFC::AnsiString substr = DATA.SubString(pos + 7, 13);
        pos = substr.AnsiPos(",");
        if( pos > 0 )
        {
            substr = substr.Delete(pos,substr.Length()-pos);
        }
        if( pos != 0 )
        {
            if( substr.Length() > 0 )
              key_no.Printf("%s",substr.c_str());
        }
    }
    if( key_no.Length() > 0 && key_no.AnsiCompareIC("null") != 0 )
    {
        if( key_no.Length() < 13 )
        {
            key_no.Printf("%013d",key_no.ToInt());
        }
        else if( key_no.Length() > 13 )
        {
            int fit = key_no.Length() - 13;
            key_no = key_no.SubString(fit,13);
        }
    }
    else
    {
        key_no.Printf("999%010d",Msg->GetNID());
    }
            
    UFC::AnsiString ResultMsg;
    UFC::AnsiString TransactTime( Msg->GetTransactTime(), 8 );
    ResultMsg.Printf( "1%s%s%s%07d%c%c%c%02d%s%s%c%04d%+010lld%04d%04d%02d %s+%s%+014d%s%s%s%s%s%s%s%05d%010d"
            , BrokerID.c_str(), IBNO.c_str(), OrderID.c_str(), StringToInt32(Msg->GetAccount(),0)
            , offsetkind, tradeid, ordertype,functionkind
            , Symbol1.c_str(), Symbol2.c_str() , tif, Msg->GetOrderQty(), price, Msg->GetBeforeQty(), Msg->GetAfterQty()
            , StringToInt32(Msg->GetStatusCode(),0), PVC.c_str()
            , key_no.c_str(),0/*Msg->GetNID()*/, UserData.GetAE().c_str(), UserData.GetApKind().c_str()
            , UserData.GetSubAccount().c_str(), today.c_str(), TransactTime.c_str(), UserData.GetSourceIP().c_str(), ww_conn_type
            , atoi( Msg->GetPVC() ), Msg->GetReportSequence()   
    );

    UFC::BufferedLog::Printf( " <Confirm> <%s>", ResultMsg.c_str() );
    
    int type = 0;
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTWFutures:
            type = 1;
            break;
        case nsOrderMessageDefine::mTWOptions:
            type = 3;
            break;
        default:
            break;
    }
    if( DumpOnly == FALSE )
        Send( type, ResultMsg );

}
//------------------------------------------------------------------------------
void CaptialExecConnection::FillToBackend( TExecutionReportMessage* Msg, BOOL ISNEW )
{
    UFC::AnsiString SRC( Msg->GetSrc() );
    UFC::AnsiString PVC( Msg->GetPVC() );
    double AfterPx = 0;
    SRC.PadThis( MAXSRC_SIZE, ' ' );

    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour && ExecSession == esNormal ||
        Msg->GetTradingSessionID() == nsOrderMessageDefine::tsNormal && ExecSession == esOffHour )
        return;    
    if( FSendQuoteExec == FALSE && SRC[0] == '4' )
        return; ///< Skip Quote Cancel/Replace
    char TSession='N';
    
    
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
        TSession = 'Y';
    
    
    UFC::AnsiString TIME;
    UFC::GetTimeString( TIME,FALSE );
    TIME.PadThis( 8, ' ' );

    TUserData UserData( Msg->GetData() );
    UserData.DumpData();
    UFC::AnsiString AE = UserData.GetOriginalSubAccount();
    AE.PadThis(7, ' ');
    
    UFC::AnsiString userPE = UserData.GetPE();
    nsOrderMessageDefine::MarketEnum msgMarket = Msg->GetMarket();
    char tradeid = ' ';
    if (((msgMarket == nsOrderMessageDefine::mTWOptions) || (msgMarket == nsOrderMessageDefine::mForeignFutures) ||
         (msgMarket == nsOrderMessageDefine::mCNOptions)) &&
        ((userPE.Length() > 0) && (userPE == "2")))
        tradeid = '2';
    else
    {
        nsOrderMessageDefine::PositionEffectEnum msgPositionEffect = Msg->GetPositionEffect();
        if (msgPositionEffect == nsOrderMessageDefine::peOpen)
            tradeid = ' ';
        else if (msgPositionEffect == nsOrderMessageDefine::peClose)
            tradeid = '1';
        else if (msgPositionEffect == nsOrderMessageDefine::peRolled)
            tradeid = 'Y';            
        }

    UFC::AnsiString ResultMsg;
    char Market;
    int type = 0;
    switch( msgMarket )
    {
        case nsOrderMessageDefine::mTWFutures:
            type = 2;
            Market = 'F';
            break;
        case nsOrderMessageDefine::mTWOptions:
            type = 4;
            Market = 'O';
            break;
        default:
            Market = ' ';
            break;
    }  //switch( msgMarket )

    char tif = ' ';
    if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC )
        tif = '3';
    else if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifFOK )
        tif = '4';
    else if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifROD )
        tif = '0';
    else if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifTFXQ )
        tif = '8';

    if( PVC.Length() > 2 )
    {
        PVC.Delete(0,PVC.Length()-2);
    }

    if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
            Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
    {

        AfterPx = Msg->GetPrice();
        SRC.Printf( "3%s%s%s%s%s%-20s%c%s4%04d%04d%s%08d", Msg->GetStatusCode(), Msg->GetBrokerID(), Msg->GetOrderID(), Msg->GetAccount(),
                                         Msg->GetAccountFlag(), Msg->GetSymbol(), Msg->GetSide() == nsOrderMessageDefine::sBuy?'B':'S',
                                         Msg->GetCMID(), Msg->GetBeforeQty(), Msg->GetAfterQty(), TIME.c_str(), Msg->GetReportSequence()
                                            );
        SRC.PadThis( MAXSRC_SIZE, ' ' );
    }

    if( SRC[0] == '1' )
    {
        SRC[69] = PVC[0];
        SRC[70] = PVC[1];
        if( SRC.SubString( 52, 4 ).ToInt() == 0 )///<FillQty = 0
        {
            for( int i = 0; i < 9; i++ )
                 SRC[43 +i] = '0'; ///< Set price to 0
        }
    }
    else if( SRC[0] == '2' )
    {
        SRC[117] = PVC[0];
        SRC[118] = PVC[1];
    }
    else if( SRC[0]== '3' || SRC[0]== '4' )
    {
        SRC[65] = PVC[0];
        SRC[66] = PVC[1];
    }       

    ResultMsg.Printf( "%s%s%s%c%c%c%+010lld%010d%05d%010d", 
                        SRC.c_str(),                    ///< TXIFEX Message  X(127)
                        AE.c_str(),                     ///< Sub Account     X(7)
                        TIME.c_str(),                   ///< HHMMSSmm        X(8) 
                        Market,                         ///< F/O             X(1)
                        tradeid,                        ///< PositionEffect  X(1) ' ':Open '1':Close '2':Auto 
                        tif,                            ///< TimeInForce     X(1) 
                        UFC::DoubleToInt64( AfterPx,4 ),///< S9(5)V(4)
                        Msg->GetTMPUniqueID(),          ///< TAIFEX UniqueID X(10)        
                        atoi( Msg->GetPVC() ),          ///< Session ID      X(5)
                        Msg->GetReportSequence() );     ///< Report Sequence X(10)
    ResultMsg.PadThis( FILL_SIZE, ' ' );                ///< Total length: 180
    UFC::BufferedLog::Printf( " <Fill> <%s>", ResultMsg.c_str() );

    if( DumpOnly == FALSE )
        Send( type, ResultMsg );
}
//------------------------------------------------------------------------------
void CaptialExecConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{ 
    //if( FExcludeBrokerID.Length() > 0 && FExcludeBrokerID == Msg->GetBrokerID() )
        //return;
    if( FExcludeBrokerIDSet.Exists( Msg->GetBrokerID() ) )
    {
        UFC::BufferedLog::DebugPrintf( " Drop execution report from %s", Msg->GetBrokerID() );
        return;
    }
    if( EnableB50Cancel == FALSE && Msg->GetReportSequence() > 10000000 )
    {
        UFC::BufferedLog::Printf( " Skip B50 Execution[%s]", Msg->GetSrc() );
        return;
    }
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osNew:
             ConfirmToBackend( Msg , FALSE );
             break;
        case nsOrderMessageDefine::osQuoteAccept:
             ConfirmToBackend( Msg , TRUE );
             break;
        case nsOrderMessageDefine::osReplaced:
            UFC::BufferedLog::DebugPrintf( " Replaced: Status Code:[%s] TMP Status Code:[%d]", Msg->GetStatusCode(), Msg->GetTMPStstusCode() );
            
            ConfirmToBackend( Msg , FALSE );
            FillToBackend( Msg,FALSE );
            
             break;
        case nsOrderMessageDefine::osCanceled:
             ConfirmToBackend( Msg , FALSE );
             FillToBackend( Msg,FALSE );
             break;
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:
             FillToBackend( Msg,TRUE );
             break;
        case nsOrderMessageDefine::osPendingNew:
        case nsOrderMessageDefine::osPendingReplace:
        case nsOrderMessageDefine::osPendingCancel:
            break;
        case nsOrderMessageDefine::osRejected:
            UFC::BufferedLog::DebugPrintf( " Reject" ); break;
        default: break;
    }
}
//------------------------------------------------------------------------------
void CaptialExecConnection::Execute( void )
{      
    while( !IsTerminated() )
    {
        if( FIsLogon == FALSE )
            FConnection->Connect( FIP.c_str() , FPort, 10 );    
        sleep( 10 ); ///< wait 10 sec.
    }
}
//------------------------------------------------------------------------------
BOOL CaptialExecConnection::IsConnected( void )
{
    if( FConnection != NULL )
        return FIsLogon;
    return FALSE;
}
//------------------------------------------------------------------------------



