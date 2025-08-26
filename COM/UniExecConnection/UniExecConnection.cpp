#include <string.h>
#include <math.h>
#include "NetUtility.h"
#include "../../TAIFEXLib/TMPMessage.h"
#include "UniExecConnection.h"
//------------------------------------------------------------------------------
extern UFC::PInt32     FRecoverBeginTime;
extern UFC::UDateTime  gAppStartTime;
extern UFC::AnsiString gTradeDate;
extern UFC::AnsiString gSendSubject;
extern UFC::AnsiString gSendKey;
extern UFC::AnsiString gSendFrontKey;
extern bool gIsFillReportOnly;


//------------------------------- ProcessMonitor -------------------------------
//------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        UFC::BufferedLog::Printf( " ***** Process SpeedyManager already exists *****"  );
        sleep( 2 );
        exit( 0 );
    }
}  //ProcessMonitor::OnProcessConnected()

//----------------------------- UniExecConnection ------------------------------
UFC::PInt32 UniExecConnection::FSequence;
bool UniExecConnection::FIsDumpOnly = false;
MessageObject *UniExecConnection::FMsgObjPtr = 0;
//------------------------------------------------------------------------------
UFC::AnsiString UniExecConnection::TrimBeginZero(const UFC::AnsiString& SourceStr)
{
    UFC::AnsiString resultStr = SourceStr;
    if ((resultStr.Length() >= 1) && (resultStr[0] == '0')) resultStr.TrimLeft('0');
    return resultStr;
}  //UniExecConnection::TrimBeginZero()
//------------------------------------------------------------------------------
UniExecConnection::UniExecConnection( const UFC::AnsiString& AppName, const UFC::AnsiString& IP, int Port, 
                                      const UFC::AnsiString& ID, const UFC::AnsiString& Password, const UFC::AnsiString& Account,
                                      const UFC::AnsiString& BrokerID, const UFC::AnsiString WEBID, char OrderIDPrefix )
:UFC::PThread( NULL, FALSE )
,FAppName( AppName )
,FIP( IP )
,FPort( Port )
,FID( ID )
,FPassword( Password )
,FAccount( Account )
,FBrokerID( BrokerID )
,FWEBID( WEBID )
,FOrderIDPrefix( OrderIDPrefix )
,FConnection( 0 )
,FIsLogon( FALSE )
,FExecReportLogPtr( 0 )
{      
    UFC::AnsiString ShmName( "UNI_PATS_RPTSEQ" );
    UFC::BufferedLog::SetDebugMode( TRUE );

    ///< Create the connection object.
    FConnection = new TTaifexConnection( NULL, &FResource, FAppName, this, atSpeedy, TRUE );
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
    FConnection->AddSellSide( ssForeignExchange );
    
    UniExecConnection::FSequence.Attach( ShmName );    
    
    UFC::AnsiString execRptLogFileName;
    execRptLogFileName.Printf( "../log/UniExecReport.%s.log", gTradeDate.c_str() );
    if (UFC::FileExists(execRptLogFileName) == TRUE)
        FExecReportLogPtr = new UFC::FileStreamEx(execRptLogFileName, "a+");
    else
        FExecReportLogPtr = new UFC::FileStreamEx(execRptLogFileName, "w");
    
    UFC::BufferedLog::Printf( " Execution Report Log File:[%s]", execRptLogFileName.c_str());
    
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
void UniExecConnection::LoadSetting( void )
{
    try
    {        
        UFC::UiniFile   Config( "../cfg/UniExecConnection.cfg" );
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
UniExecConnection::~UniExecConnection( void )
{
    if ( FExecReportLogPtr != 0 )
    {
        FExecReportLogPtr->Flush();
        delete FExecReportLogPtr;
    }
    
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
void UniExecConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str(),  ctBoth );
    FIsLogon = TRUE;
}
//------------------------------------------------------------------------------
void UniExecConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void UniExecConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
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
void UniExecConnection::Recover( UFC::AnsiString BeginTime, char Type )
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
void UniExecConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " UniExecConnection::OnRecoverFinished( Count = %d )", Count );
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
void UniExecConnection::ConfirmToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString logHead = "UniExecConnection::ConfirmToBackend()";
    UniBackEndMessage beMsg;
    memset(beMsg.buffer, ' ', 256);
    nsOrderMessageDefine::OrderStatusEnum msgOrderStatus = Msg->GetOrderStatus();
    UFC::AnsiString messageTypeStr;
    switch (msgOrderStatus)
    {
        case nsOrderMessageDefine::osNew :
        case nsOrderMessageDefine::osReplaced :
        case nsOrderMessageDefine::osCanceled :
        case nsOrderMessageDefine::osPartiallyFilled :
        case nsOrderMessageDefine::osFilled :
        case nsOrderMessageDefine::osQuoteAccept :
            messageTypeStr = "102";
            break;
        case nsOrderMessageDefine::osRejected :
            messageTypeStr = "103";
            break;
        case nsOrderMessageDefine::osPendingNew :
        case nsOrderMessageDefine::osPendingReplace :
        case nsOrderMessageDefine::osPendingCancel :
        default:
            return;
    }  //switch (msgOrderStatus)
    memcpy(beMsg.detail.msgid, messageTypeStr.c_str(), 3);  //  1
    
    UFC::AnsiString tmpStstusCodeStr;
    tmpStstusCodeStr.Printf("%03d", Msg->GetTMPStstusCode());
    memcpy(beMsg.detail.statuscode, tmpStstusCodeStr.c_str(), 3);  //  2
    
    Int32 msgPriceDigit = Msg->GetPxDigit();
    nsOrderMessageDefine::TMPExecTypeEnum msgTMPExecType = Msg->GetTMPExecType();
    char execType;
    Int32 intLastPrice = 0;
    Int32 msgLastQty = 0;
    switch(Msg->GetExecType())
    {
        case nsOrderMessageDefine::etNew:
            execType = '0';
            break;
        case nsOrderMessageDefine::etReplaced:
            if ((msgTMPExecType == nsOrderMessageDefine::tetPxReplaced) ||
                (msgTMPExecType == nsOrderMessageDefine::tetPxReplaced2))
                execType = 'M';
            else
                execType = '5';
            break;
        case nsOrderMessageDefine::etCanceled:
            execType = '4';
            break;
        case nsOrderMessageDefine::etPartiallyFilled:
        case nsOrderMessageDefine::etFilled:
            execType = 'F';
            intLastPrice = UFC::DoubleToInt(Msg->GetPrice(), msgPriceDigit);
            msgLastQty = Msg->GetOrderQty();
            break;
        case nsOrderMessageDefine::etOrderStatus:
            execType = 'I';
            break;
        case nsOrderMessageDefine::etRejected:
            switch( Msg->GetCxlRejResponseTo() )
            {
                case nsOrderMessageDefine::crrNew:         execType = '0'; break;
                case nsOrderMessageDefine::crrReplace:     if ((msgTMPExecType == nsOrderMessageDefine::tetPxReplaced) ||
                                                               (msgTMPExecType == nsOrderMessageDefine::tetPxReplaced2))
                                                               execType = 'M';
                                                           else
                                                               execType = '5';
                                                           break;
                case nsOrderMessageDefine::crrCancel:      execType = '4'; break;
                case nsOrderMessageDefine::crrOrderStatus: execType = 'I'; break;
                default:                                   execType = ' '; break;
            }
            break;
        default:
            execType = ' ';
            break;
    }
    
    //  3
    if ((msgTMPExecType == nsOrderMessageDefine::tetNewAndFilled) &&
        ((execType == 'F') || (execType == '4')))
        beMsg.detail.exectype = '6';
    else
        beMsg.detail.exectype = execType;

    UFC::AnsiString clearMemberIdStr;
    int clearMemberIndex = TMP::TMPMessage::GetCMID(Msg->GetCMID(), Msg->GetBrokerID());
    clearMemberIdStr.Printf("%05d", clearMemberIndex);
    memcpy(beMsg.detail.cmdid, clearMemberIdStr.c_str(), 5);  //  4

    UFC::AnsiString brokerIdNoStr;
    int brokerIndex = TMP::TMPMessage::GetBrokerID(Msg->GetBrokerID());
    brokerIdNoStr.Printf("%05d", brokerIndex);
    memcpy(beMsg.detail.fcmid, brokerIdNoStr.c_str(), 5);  //  5

    UFC::AnsiString orderNoStr;
    orderNoStr.Printf("%s", Msg->GetOrderID());
    orderNoStr.PadThis(5, ' ');
    memcpy(beMsg.detail.orderno, orderNoStr.c_str(), 5);  //  6

    UFC::AnsiString ordIdStr = "0000000";
    memcpy(beMsg.detail.ordid, ordIdStr.c_str(), 7);  // 7

    UFC::AnsiString userDefine = "        ";
    FSequence += 1;
    if( FSequence.ToInt32() > 99999999 )
        FSequence = 1;
    userDefine.Printf( "%08d", FSequence.ToInt32() );
    memcpy(beMsg.detail.userdf, userDefine.c_str(), 8);  //  8

    char symbolType = '2';
    beMsg.detail.commod = symbolType;  //  9

    UFC::AnsiString msgSymbol = Msg->GetSymbol();
    msgSymbol.PadThis(20, ' ');
    memcpy(beMsg.detail.commodx, msgSymbol.c_str(), 20);  // 10

    Int32 intOrderPrice = UFC::DoubleToInt(Msg->GetLastPx(), msgPriceDigit);
    UFC::AnsiString orderPriceStr;
    if (intOrderPrice >= 0)
        orderPriceStr.Printf("%09d", intOrderPrice);
    else
        orderPriceStr.Printf("%+09d", intOrderPrice);
    memcpy(beMsg.detail.price, orderPriceStr.c_str(), 9);  // 11

    UFC::AnsiString orderQtyStr;
    orderQtyStr.Printf("%05d", Msg->GetTMPQty());
    memcpy(beMsg.detail.quantity, orderQtyStr.c_str(), 5);  // 12

    UFC::AnsiString msgInvestorAccountNo = Msg->GetAccount();
    msgInvestorAccountNo.PadThis(7, ' ');
    memcpy(beMsg.detail.ivacno, msgInvestorAccountNo.c_str(), 7);  // 13

    UFC::AnsiString msgInvestorFlagStr = Msg->GetAccountFlag();
    msgInvestorFlagStr.PadThis(1, ' ');
    beMsg.detail.ivacflag = msgInvestorFlagStr[0];  // 14

    //char sideChar;
    
    switch( Msg->GetSide() )
    {
        case nsOrderMessageDefine::sBuy:
            beMsg.detail.side = '1';  // 15
            //sideChar = 'B';
            break;
        case nsOrderMessageDefine::sSell:
            beMsg.detail.side = '2';  // 15
            //sideChar = 'S';
            break;
        default:
            beMsg.detail.side = '0';  // 15
    }  //switch(Msg->GetSide())

    switch(Msg->GetOrderType())
    {
        case nsOrderMessageDefine::otMarket:               beMsg.detail.ordtype = '1'; break;  // 16
        case nsOrderMessageDefine::otLimit:                beMsg.detail.ordtype = '2'; break;
        case nsOrderMessageDefine::otMarketWithProtection: beMsg.detail.ordtype = '3'; break;
        default:                                           beMsg.detail.ordtype = ' '; 
    }

    switch(Msg->GetTimeInForce())
    {
        case nsOrderMessageDefine::tifROD:
            beMsg.detail.ordcnd = '0';  // 17
            break;
        case nsOrderMessageDefine::tifIOC:
            if ((execType == '4') && (Msg->GetCumQty() > 0) && (Msg->GetLeavesQty() == 0) && (Msg->GetOrderQty() == 0))
            {
                return;
            }
            beMsg.detail.ordcnd = '3';  // 17
            break;
        case nsOrderMessageDefine::tifFOK:
            beMsg.detail.ordcnd = '4';  // 17
            break;
        case nsOrderMessageDefine::tifTFXQ:
            beMsg.detail.ordcnd = '8';  // 17
            break;
        default:
            beMsg.detail.ordcnd = ' ';
    }

    switch(Msg->GetPositionEffect())
    {
        case nsOrderMessageDefine::peOpen:           beMsg.detail.opnoff = 'O'; break;  // 18
        case nsOrderMessageDefine::peClose:          beMsg.detail.opnoff = 'C'; break;
        case nsOrderMessageDefine::peDayTrade:       beMsg.detail.opnoff = 'D'; break;
        case nsOrderMessageDefine::peTMPMarketMaker: beMsg.detail.opnoff = '9'; break;
        default:                                     beMsg.detail.opnoff = 'O';
    }

    UFC::AnsiString lastPriceStr;
    if (intLastPrice >= 0)
        lastPriceStr.Printf("%09d", intLastPrice);
    else
        lastPriceStr.Printf("%+09d", intLastPrice);
    memcpy(beMsg.detail.lastpx, lastPriceStr.c_str(), 9);  // 19
    
    UFC::AnsiString lastQtyStr;
    lastQtyStr.Printf("%05d", msgLastQty);
    memcpy(beMsg.detail.lastqty, lastQtyStr.c_str(), 5);  // 20

    Int64 msgPriceSubTotal = static_cast<Int64>(Msg->GetTMPPxSubTotal());
    UFC::AnsiString priceSubTotalStr;
    if (msgPriceSubTotal >= 0)
        priceSubTotalStr.Printf("%017lld", msgPriceSubTotal);
    else
        priceSubTotalStr.Printf("%+017lld", msgPriceSubTotal);
    memcpy(beMsg.detail.pxsubtotal, priceSubTotalStr.c_str(), 17);  //21

    UFC::AnsiString cumQtyStr;
    cumQtyStr.Printf("%05d", Msg->GetCumQty());
    memcpy(beMsg.detail.cumqty, cumQtyStr.c_str(), 5);  // 22

    Int32 leavesQty = Msg->GetLeavesQty();
    UFC::AnsiString leavesQtyStr;
    leavesQtyStr.Printf("%05d", leavesQty);
    memcpy(beMsg.detail.leavesqty, leavesQtyStr.c_str(), 5);  //23
    
    UFC::AnsiString beforeQtyStr;
    if (execType != '0') 
        beforeQtyStr.Printf("%05d", Msg->GetBeforeQty());
    else
        beforeQtyStr.Printf("%05d", leavesQty);        
    memcpy(beMsg.detail.beforeqty, beforeQtyStr.c_str(), 5);  //24

    switch(Msg->GetLegSide1())
    {
        case nsOrderMessageDefine::sNone: beMsg.detail.legside1 = '0'; break;  //25
        case nsOrderMessageDefine::sBuy:  beMsg.detail.legside1 = '1'; break;
        case nsOrderMessageDefine::sSell: beMsg.detail.legside1 = '2'; break;
        default:                          beMsg.detail.legside1 = '0';
    }

    switch(Msg->GetLegSide2())
    {
        case nsOrderMessageDefine::sNone: beMsg.detail.legside2 = '0'; break;  //26
        case nsOrderMessageDefine::sBuy:  beMsg.detail.legside2 = '1'; break;
        case nsOrderMessageDefine::sSell: beMsg.detail.legside2 = '2'; break;
        default:                          beMsg.detail.legside2 = '0';
    }

    Int32 intLeg1Price = UFC::DoubleToInt(Msg->GetLegPrice1(), msgPriceDigit);
    UFC::AnsiString leg1PriceStr;
    leg1PriceStr.Printf("%09d", intLeg1Price);
    memcpy(beMsg.detail.legpx1, leg1PriceStr.c_str(), 9);  // 27

    Int32 intLeg2Price = UFC::DoubleToInt(Msg->GetLegPrice2(), msgPriceDigit);
    UFC::AnsiString leg2PriceStr;
    leg2PriceStr.Printf("%09d", intLeg2Price);
    memcpy(beMsg.detail.legpx2, leg2PriceStr.c_str(), 9);  // 28
    
    UFC::AnsiString leg1QtyStr;
    leg1QtyStr.Printf("%05d", Msg->GetLegQty1());
    memcpy(beMsg.detail.legqty1, leg1QtyStr.c_str(), 5);  // 29

    UFC::AnsiString leg2QtyStr;
    leg2QtyStr.Printf("%05d", Msg->GetLegQty2());
    memcpy(beMsg.detail.legqty2, leg2QtyStr.c_str(), 5);  // 30

    int orgTransTime = UFC::AnsiString(Msg->GetOrgTransactTime()).ToInt();
    int orgTransHour = orgTransTime / 10000000;
    int orgTransMinute = (orgTransTime % 10000000) / 100000;
    int orgTransSecond = (orgTransTime % 100000) / 1000;
    int orgTransMilliSecond = orgTransTime % 1000;
    UFC::AnsiString orgTransTimeStr;
    orgTransTimeStr.Printf("%02d%02d%02d%03d", orgTransHour, orgTransMinute, orgTransSecond, orgTransMilliSecond);
    memcpy(beMsg.detail.orgtime, orgTransTimeStr.c_str(), 9);  // 31
    
    
    int transTime = UFC::AnsiString(Msg->GetTransactTime()).ToInt();
    int transHour = transTime / 10000000;
    int transMinute = (transTime % 10000000) / 100000;
    int transSecond = (transTime % 100000) / 1000;
    int transMilliSecond = transTime % 1000;
    UFC::AnsiString transTimeStr;
    transTimeStr.Printf("%02d%02d%02d%03d", transHour, transMinute, transSecond, transMilliSecond);
    memcpy(beMsg.detail.trntime, transTimeStr.c_str(), 9);  // 32

    beMsg.detail.targetid = '4';  // 33
    
    UFC::AnsiString tmpUniqueIDStr;
    tmpUniqueIDStr.Printf("%010d", Msg->GetTMPUniqueID());
    memcpy(beMsg.detail.uniqid, tmpUniqueIDStr.c_str(), 10);  // 34

    UFC::AnsiString reportSequenceStr;
    reportSequenceStr.Printf("%010d", Msg->GetReportSequence());
    memcpy(beMsg.detail.rptseq, reportSequenceStr.c_str(), 10);  // 35

    beMsg.detail.protocol = '1';  // 36

    UFC::AnsiString pvcStr;
    pvcStr.Printf("%05d", atoi(Msg->GetPVC()));
    memcpy(beMsg.detail.ssnid, pvcStr.c_str(), 5);  // 37

    UFC::AnsiString msgAE = Msg->GetAE();
    UFC::AnsiString traderStr = TrimBeginZero(msgAE);
    traderStr.PadThis(5, ' ');
    memcpy(beMsg.detail.trader, traderStr.c_str(), 5);  // 38

    nsOrderMessageDefine::MarketEnum market = Msg->GetMarket();
    nsOrderMessageDefine::TradingSessionIDEnum tradingSessionID = Msg->GetTradingSessionID();
    if( market == nsOrderMessageDefine::mTWFutures )
    {
        if( tradingSessionID == nsOrderMessageDefine::tsOffHour)
            memcpy(beMsg.detail.systype, "21", 2);  // 39
        else
            memcpy(beMsg.detail.systype, "20", 2);  // 39
    }
    else if( market == nsOrderMessageDefine::mTWOptions )
    {
        if( tradingSessionID == nsOrderMessageDefine::tsOffHour)
            memcpy(beMsg.detail.systype, "11", 2);  // 39
        else
            memcpy(beMsg.detail.systype, "10", 2);  // 39
    }  //if( market == nsOrderMessageDefine::mTWFutures )
    
    beMsg.detail.endChar = 0;
    
    UFC::AnsiString beDataStr;
    beDataStr.Printf("%s", beMsg.buffer);

    MTree Data;
    Data.append("Data", beDataStr);
    if (market == nsOrderMessageDefine::mTWFutures)
    {
        if (execType != 'F')
            UFC::BufferedLog::Printf(" <FUT CONFIRM> <%s>", beMsg.buffer);
        else
            UFC::BufferedLog::Printf(" <FUT Fill> <%s>", beMsg.buffer);
    }
    else if (market == nsOrderMessageDefine::mTWOptions)
    {
        if (execType != 'F')
            UFC::BufferedLog::Printf(" <OPT CONFIRM> <%s>", beMsg.buffer);
        else
            UFC::BufferedLog::Printf(" <OPT Fill> <%s>", beMsg.buffer);
    }
    else
        UFC::BufferedLog::Printf(" %s Wrong Market <%d><%s>", logHead.c_str(), market, beMsg.buffer );
        
    FExecReportLogLock.Acquire();
    if (FExecReportLogPtr != 0)
    {
        FExecReportLogPtr->Write(beDataStr.c_str(), beDataStr.Length());
        FExecReportLogPtr->Flush();        
    }
    FExecReportLogLock.Release();
        
    if (!FIsDumpOnly)
    {
        if( FMsgObjPtr->Send(gSendSubject, gSendKey, Data) != TRUE )
            UFC::BufferedLog::Printf( " %s Send <%s> to [%s][%s] failed.", logHead.c_str(), beMsg.buffer, gSendSubject.c_str(), gSendKey.c_str() );
            
        if (beMsg.detail.ordcnd == '0')  //ROD
            if( FMsgObjPtr->Send(gSendSubject, gSendFrontKey, Data ) != TRUE )
                UFC::BufferedLog::Printf( " %s Send <%s> to [%s][%s] failed.", logHead.c_str(), beMsg.buffer, gSendSubject.c_str(), gSendFrontKey.c_str() );
    }
}
//------------------------------------------------------------------------------
void UniExecConnection::FillToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString Src( Msg->GetSrc() );
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString GIGO( Msg->GetData() );
    UFC::AnsiString TFX, Backend, TimeNow, MatchTime, SSNID;
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;
    char QueueData[ 513 ];     

    if( Src[0] == '1' )
        MatchTime = Src.SubString( 61, 8 );
    else if( Src[0] == '2' )
        MatchTime = Src.SubString( 109 , 8 );
    else
        MatchTime = Src.SubString( 57, 8 );

    UFC::BufferedLog::DebugPrintf( " MatchTime:[%s]", MatchTime.c_str() );
    UFC::BufferedLog::DebugPrintf( " BrokerID:[%s]", Msg->GetBrokerID() );
    ///< TAIFEX part.
    //UFC::GetHHMMSS( TimeNow );
    if( Market == nsOrderMessageDefine::mTWFutures )
        TFX.Printf( "        500002%s00%s01%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetBrokerID(), Msg->GetSrc() );
    else
        TFX.Printf( "        950002%s00%s01%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetBrokerID(), Msg->GetSrc() );
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
    ///< Compose to a full As400 report.
    memset( QueueData, ' ', 512 );
    QueueData[512] = 0;
    memcpy( QueueData, TFX.c_str(), TFX.Length());
    memcpy( QueueData + 424, SSNID.c_str(), SSNID.Length() );
    memcpy( QueueData + 462, Backend.c_str(), Backend.Length());
    if( Src[0] == '3' || Src[0] == '4') ///< Canceled.
        QueueData[460] = 'S';
    Data.append("Data", QueueData );
    
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::Printf( " <FUT Fill> <%s>", QueueData );                                                
            if( !FIsDumpOnly )
            {
                FMsgObjPtr->Send( "Fill", "To400", Data );
                if( TIF == 'R')
                    FMsgObjPtr->Send( "Fill", "To400Front", Data );
            }
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " <OPT Fill> <%s>", QueueData );
            if( !FIsDumpOnly )
            {
                FMsgObjPtr->Send( "Fill", "To400", Data );
                if( TIF == 'R')
                    FMsgObjPtr->Send( "Fill", "To400Front", Data );
            }
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void UniExecConnection::TSEConfirmToBackend( TExecutionReportMessage* Msg )
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

    memcpy( (void*)(QueueData.c_str() + 244), AE.c_str(), 10 );
    memcpy( (void*)(QueueData.c_str() + 254), WebID.c_str(), 2 );

    Data.append("Data", QueueData );

    switch( Market )
    {
        case nsOrderMessageDefine::mTSE:
            UFC::BufferedLog::Printf( " <TSE CONFIRM> <%s>", QueueData.c_str() );
            if( !FIsDumpOnly )
                FMsgObjPtr->Send( "TSEConfirm", "To400", Data );
            break;
        case nsOrderMessageDefine::mOTC:
            UFC::BufferedLog::Printf( " <OTC CONFIRM> <%s>", QueueData.c_str() );
            if( !FIsDumpOnly )
                FMsgObjPtr->Send( "TSEConfirm", "To400", Data );
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void UniExecConnection::TSEFillToBackend( TExecutionReportMessage* Msg )
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

    memcpy( (void*)(QueueData.c_str() + 244), AE.c_str(), 10 );
    memcpy( (void*)(QueueData.c_str() + 254), WebID.c_str(), 2 );

    Data.append("Data", QueueData );
    switch( Market )
    {
        case nsOrderMessageDefine::mTSE:
            UFC::BufferedLog::Printf( " <TSE Fill> <%s>", QueueData.c_str() );
            if( !FIsDumpOnly )
                FMsgObjPtr->Send( "TSEFill", "To400", Data );
            break;
        case nsOrderMessageDefine::mOTC:
            UFC::BufferedLog::Printf( " <OTC Fill> <%s>", QueueData.c_str() );
            if( !FIsDumpOnly )
                FMsgObjPtr->Send( "TSEFill", "To400", Data );
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void UniExecConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    if( ( Market == nsOrderMessageDefine::mTWFutures ) || ( Market == nsOrderMessageDefine::mTWOptions ) )
    {
        switch( Msg->GetOrderStatus() )
        {
            case nsOrderMessageDefine::osNew:
            case nsOrderMessageDefine::osQuoteAccept:
                if (!gIsFillReportOnly) ConfirmToBackend( Msg ); 
                break;
            case nsOrderMessageDefine::osReplaced:
                if (!gIsFillReportOnly) 
                {
                    if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                        Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
                        ConfirmToBackend( Msg );
                    else
                        ConfirmToBackend( Msg );
                }
                break;
//            case nsOrderMessageDefine::osCanceled:
//                if ( ( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC ) && 
//                     ( Msg->GetCumQty() > 0 ) && ( Msg->GetLeavesQty() == 0 ) && ( Msg->GetOrderQty() == 0 ) )
//                    return;
//                else
//                    ConfirmToBackend( Msg );
//                break;
            case nsOrderMessageDefine::osPartiallyFilled:
            case nsOrderMessageDefine::osFilled:
                 ConfirmToBackend( Msg );
                 break;
            case nsOrderMessageDefine::osCanceled:
            case nsOrderMessageDefine::osPendingNew:
            case nsOrderMessageDefine::osPendingReplace:
            case nsOrderMessageDefine::osPendingCancel:
            case nsOrderMessageDefine::osRejected:
            default:
                break;
        }        
    }  //if( ( Market == nsOrderMessageDefine::mTWFutures ) || ( Market == nsOrderMessageDefine::mTWOptions ) )

}
//------------------------------------------------------------------------------
void UniExecConnection::Execute( void )
{      
    while( !IsTerminated() )
    {
        if( FIsLogon == FALSE )
            FConnection->Connect( FIP.c_str(), FPort, 10 );    
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
void UniExecConnection::PATSExecutionReport( TExecutionReportMessage* Msg, BOOL IsConfirm )
{
    MTree Data;
    UFC::AnsiString Today,Value,OrderID;
    UFC::AnsiString RptTime( Msg->GetTransactTime());
    UFC::AnsiString AE( Msg->GetAE() );
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
    UniExecConnection::FSequence += 1;
    if( FSequence.ToInt32() > 99999999 )
        FSequence = 1;
    Value.Printf( "%08d", FSequence.ToInt32() );
    memcpy( Rpt.SeqNo, Value.c_str(), 8 ); ///?
    memcpy( Rpt.FCM, "UOB", 3 );
    memcpy( Rpt.OrderID, PATSOrderID.c_str(), 20 );
    memcpy( Rpt.OOrderID, OrderID.c_str(), 20 ); //X(20) Speedy Order ID x(6)
    memcpy( Rpt.SecondaryOrderID, PATSOrderID.c_str(), 20 );         //X(20) ----- Not Use ----- ??    
    memcpy( Rpt.ExecID, ExecID.c_str(), 70 );              //X(70) PATS FIll execid
    memcpy( Rpt.OrderKind, "QUEST", 5 );           //X(20) QUEST,QUEST+1,EDS,REG... ??
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osNew:             Rpt.OrderStatus[0] = '0'; break;
        case nsOrderMessageDefine::osPartiallyFilled: Rpt.OrderStatus[0] = '1'; break;
        case nsOrderMessageDefine::osFilled:          Rpt.OrderStatus[0] = '2'; break;
        case nsOrderMessageDefine::osCanceled:        Rpt.OrderStatus[0] = '4'; break;
        case nsOrderMessageDefine::osRejected:        Rpt.OrderStatus[0] = '8'; break;
        default:                                      Rpt.OrderStatus[0] = ' ';
    }   //X(1)  New:0 Partially Filled:1 Filled:2 Canceled:4 Reject:8
    memcpy( Rpt.Exchange, Exchange.c_str(), 20);                   //X(20) Exchange
    memcpy( Rpt.Symbol, Symbol.c_str(), 20 );                      //X(20) Symbol
    memcpy( Rpt.SecurityType, "FUT",3 );                           //X(5)  "FUT"
    memcpy( Rpt.MaturityMonthYear, MaturityMonthYear.c_str(), 10 );//X(10) Maturity Month Year
    memcpy( Rpt.StrikePrice,"0000000000000",13 );         //9(7)V9(6) Strike Price
    Rpt.PutOrCall[0] = ' ';       //X(1)  Put:'0' Call:'1'
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
        default:                                Rpt.OrderType[0] = ' ';
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
    if( !FIsDumpOnly )
    {
        if( IsConfirm == TRUE )
            FMsgObjPtr->Send( "PATSConfirm", "To400", Data );    
        else
            FMsgObjPtr->Send( "PATSFill", "To400", Data );    
    }        
}
//------------------------------------------------------------------------------
