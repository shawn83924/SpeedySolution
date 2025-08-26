#include "PMFExecConnection.h"
#include "MessageStore.h"
#include "../../UFC/NetUtility.h"
#include "../../TAIFEXLib/TMPMessage.h"
#include <math.h>
#include <stdio.h>
//------------------------------------------------------------------------------
extern BOOL            DumpOnly;
extern BOOL            MatchOnly;
extern TMessageStore*  OptMessageStore;
extern TMessageStore*  FutMessageStore;
extern MessageObject*  MessageObj;
extern UFC::UiniFile*  pConfig;

UFC::UiniFile   *ConvertFilePtr = NULL;
UFC::AnsiString OdkNtChannelString;

//------------------------------------------------------------------------------
PMFExecConnection::PMFExecConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName( "PMFExecConnection" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "Admin" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F021999" )
,FWEBID( "000" )
,FRecoverBeginTime( RecoverBeginTime )
,FConnection( NULL )
,FIsLogon( FALSE )
{
    UFC::AnsiString LogName = "../log/PMFExec" + UFC::GetDateString() + ".log";
    LoadSetting(  );
    if( DumpOnly == TRUE )
        UFC::BufferedLog::Printf( " Dump to screen only.");
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   PMF backend interface module           " );
    UFC::BufferedLog::Printf( "   startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE )
        UFC::BufferedLog::Printf( "   Debug Mode:On (Dump to stdout only)");
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");
    UFC::BufferedLog::Printf( "__________________________________________" );
    ///< Create the connection object.
//    FConnection = new TTaifexConnection( NULL, &FResource, FAppName, this, TRUE );
    FConnection = new TTaifexConnection(FAppName, this, atSpeedy, false);
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
//    FConnection->SetX25StyleQtyPx( false ); /// why you are so lazy!?

    ///< Initialize TMPMessage
    TMP::TMPMessage::InitTMPMessageLibrary(TMP::tlmBoth);

    ///< Start running thread.
    Start();
}

//------------------------------------------------------------------------------
void PMFExecConnection::LoadSetting( void )
{
    try
    {
        if (pConfig->GetValue( "Setting", "IP", FIP ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Speedy Server IP not set.");
            exit(1);
        }

        if (pConfig->GetValue( "Setting", "ID", FID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon ID not set.");
            exit(1);
        }

        if (pConfig->GetValue( "Setting", "Password", FPassword ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon Password not set.");
            exit(1);
        }

        if (pConfig->GetValue( "Setting", "Account", FAccount ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon Account not set.");
            exit(1);
        }

        if (pConfig->GetValue( "Setting", "BrokerID", FBrokerID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" BrokerID not set.");
            exit(1);
        }

        if (pConfig->GetValue( "Setting", "WebID", FWEBID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" WebID not set.");
            exit(1);
        }
        FWEBID.PadThis( 3, ' ' );

        if (pConfig->GetValue( "Setting", "NTChannel", OdkNtChannelString) == FALSE )
        {
            UFC::BufferedLog::Printf(" ODK_NT_CHANNEL not set.");
            exit(1);
        }
        
        UFC::AnsiString Branch, Value;
        UFC::Section* pSection = pConfig->GetSection( "Branch" );
        
        if( pSection != NULL )
        {
            UFC::BufferedLog::Printf( " Load branch setting:" );
            for( register int i = 1; i <= pSection->ItemCount(); i++ )
            {
                Branch.Printf( "Branch%d", i );
                if( pSection->GetValue( Branch, Value ) )
                {
                    UFC::BufferedLog::Printf( " %s:[%s]", Branch.c_str(), Value.c_str() );
                    FBranchSet.Add( Value );
                }
            }
        }  //if( pSection != NULL )

        UFC::AnsiString ConvertFileName;
        if (pConfig->GetValue( "Setting", "ConvertFileName", ConvertFileName) == FALSE )
        {
            UFC::BufferedLog::Printf(" ConvertFileName not set.");
            exit(1);
        }

        ConvertFilePtr = new UFC::UiniFile(ConvertFileName);
        UFC::Section *ConvertSectionPtr = ConvertFilePtr->GetSection("Convert");
        if (ConvertSectionPtr != NULL)
        {
            UFC::AnsiString NewCommodityId;
            UFC::AnsiString OldCommodityId;
            for (int i = 0; i < ConvertSectionPtr->ItemCount(); i++)
            {
                ConvertSectionPtr->GetNameValue(i, NewCommodityId, OldCommodityId);
                UFC::BufferedLog::Printf(" [%s=%s]", NewCommodityId.c_str(), OldCommodityId.c_str());
//              PMF::InsertNewCommodityIdIndex(NewCommodityId, OldCommodityId);
            }  //for (int i = 0; i <ConvertSectionPtr->ItemCount(); i++)
        }  //if (ConvertSectionPtr != NULL)
    }
    catch (UFC::FileException& e)
    {
        UFC::BufferedLog::Printf(" %s", e.what());
        exit(1);
    }  //try
    catch(...)
    {
        UFC::BufferedLog::Printf(" PMFExecConnection.cfg not found.");
        exit(1);
    }
}

//------------------------------------------------------------------------------
PMFExecConnection::~PMFExecConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;
}

//------------------------------------------------------------------------------
void PMFExecConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );
    FIsLogon = TRUE;
}

//------------------------------------------------------------------------------
void PMFExecConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}

//------------------------------------------------------------------------------
void PMFExecConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk )
    {
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );

        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime );

        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdFill, rmAll )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll  );
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}

//------------------------------------------------------------------------------
void PMFExecConnection::Recover( UFC::AnsiString BeginTime )
{
    if( FIsLogon == TRUE )
    {
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}

//------------------------------------------------------------------------------
void PMFExecConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " PMFExecConnection::OnRecoverFinished( Count = %d )", Count );

    UFC::SleepMS( 1000 );
}
//------------------------------------------------------------------------------
BOOL PMFExecConnection::IsBlocked( TExecutionReportMessage* Msg )
{
    return !FBranchSet.Exists( Msg->GetBrokerID() );
}
//------------------------------------------------------------------------------
void PMFExecConnection::BuildMessageString(TExecutionReportMessage* Msg, UFC::AnsiString& MessageString)
{
    if( IsBlocked( Msg ) == TRUE )
    {
        UFC::BufferedLog::DebugPrintf( " drop execution with broker id:[%s]", Msg->GetBrokerID() );
        return;
    }   
    ODKMessage odkMsg;
    memset(odkMsg.Data, ' ', 400);
    UFC::AnsiString TempData;

    odkMsg.Detail.END = 0;
//    MessageString.Printf("");

    nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus = Msg->GetOrderStatus();
    if ((MsgOrderStatus == nsOrderMessageDefine::osRejected) ||
        (MsgOrderStatus == nsOrderMessageDefine::osQuoteAccept))
        return;

    nsOrderMessageDefine::TMPExecTypeEnum TMPExecType = Msg->GetTMPExecType();
    if ((TMPExecType == nsOrderMessageDefine::tetNewAndFilled) &&
        (Msg->GetExecType() == nsOrderMessageDefine::etNew))
        return;

    UFC::UDateTime CurrentDateTime;
    CurrentDateTime.setCurrent();

    UFC::AnsiString CurrentDateString;
    CurrentDateString.Printf("%04d%02d%02d", CurrentDateTime.getYear(), CurrentDateTime.getMonth(), CurrentDateTime.getDate());

    UFC::AnsiString CurrentTimeString;
    CurrentTimeString.Printf("%02d%02d%02d%04d", CurrentDateTime.getHour(), CurrentDateTime.getMinute(), CurrentDateTime.getSecond(), CurrentDateTime.getMillisecond());

    UFC::AnsiString CurrentMilliSecondString;
    CurrentMilliSecondString.Printf("%04d", CurrentDateTime.getMillisecond());


    UFC::AnsiString SeqString;
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    if (Market == nsOrderMessageDefine::mTWFutures)
    {
        SeqString.Printf("%010d", FutMessageStore->GetItemCount() + 1);
    }
    else if (Market == nsOrderMessageDefine::mTWOptions)
    {
        SeqString.Printf("%010d", OptMessageStore->GetItemCount() + 1);
    }
    else
        return;

    memcpy(odkMsg.Detail.DATE, CurrentDateString.c_str(), 8);
    memcpy(odkMsg.Detail.ORDER_DATE, CurrentDateString.c_str(), 8);
    memcpy(odkMsg.Detail.LAST_DATE, CurrentDateString.c_str(), 8);
    memcpy(odkMsg.Detail.MSG_SEQNUM, SeqString.c_str(), 10);
    memcpy(odkMsg.Detail.MSG_DATE, CurrentDateString.c_str(), 8);
    memcpy(odkMsg.Detail.MSG_TIME, CurrentTimeString.c_str(), 6);
    memcpy(odkMsg.Detail.MSG_MIL, CurrentMilliSecondString.c_str(), 4);

    UFC::AnsiString MessageTypeString;
    int MsgType = 0;
    switch (MsgOrderStatus)
    {
        case nsOrderMessageDefine::osNew :
            MsgType = 1;
            MessageTypeString.Printf("%03d", 102);
            break;
        case nsOrderMessageDefine::osReplaced :
        case nsOrderMessageDefine::osCanceled :
            MsgType = 0;
            MessageTypeString.Printf("%03d", 102);
            break;
        case nsOrderMessageDefine::osPartiallyFilled :
        case nsOrderMessageDefine::osFilled :
            MsgType = 2;
            MessageTypeString.Printf("%03d", 102);
            break;
        case nsOrderMessageDefine::osRejected :
            MsgType = -1;
            MessageTypeString.Printf("%03d", 103);
            break;
        case nsOrderMessageDefine::osQuoteAccept :
            MsgType = 3;
            MessageTypeString.Printf("%03d", 102);
            break;
        case nsOrderMessageDefine::osPendingNew :
        case nsOrderMessageDefine::osPendingReplace :
        case nsOrderMessageDefine::osPendingCancel :
        default:
            return;
    }
    memcpy(odkMsg.Detail.MSG_TYPE, MessageTypeString.c_str(), 3);

    TempData.Printf("%05d", Msg->GetTMPBrokerID());
    memcpy(odkMsg.Detail.LNK_BROKER_NO, TempData.c_str(), 5);

    UFC::AnsiString SessionIDString;
    SessionIDString.Printf("%05d", UFC::AnsiString(Msg->GetPVC()).ToInt());
    SessionIDString.PadThis(5, ' ');
    memcpy(odkMsg.Detail.SESSION_ID, SessionIDString.c_str(), 5);

    TempData.Printf("%03d", Msg->GetTMPStstusCode());
    TempData.PadThis(3, '0');
    memcpy(odkMsg.Detail.STATUS_CODE, TempData.c_str(), 3);

    char ExecTypeChar;
    switch(Msg->GetExecType())
    {
        case nsOrderMessageDefine::etNew:
            ExecTypeChar = '0';
            break;
        case nsOrderMessageDefine::etReplaced:
            if (Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2)
                ExecTypeChar = 'M';
            else
                ExecTypeChar = '5';
            break;
        case nsOrderMessageDefine::etCanceled:
            ExecTypeChar = '4';
            break;
        case nsOrderMessageDefine::etPartiallyFilled:
        case nsOrderMessageDefine::etFilled:
            ExecTypeChar = 'F';
            break;
        case nsOrderMessageDefine::etOrderStatus:
            ExecTypeChar = 'I';
            break;
        case nsOrderMessageDefine::etRejected:
            switch( Msg->GetCxlRejResponseTo() )
            {
                case nsOrderMessageDefine::crrNew:         ExecTypeChar = '0'; break;
                case nsOrderMessageDefine::crrReplace:     if (Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                                                               Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
                                                               ExecTypeChar = 'M';
                                                           else
                                                               ExecTypeChar = '5';
                                                           break;
                case nsOrderMessageDefine::crrCancel:      ExecTypeChar = '4'; break;
                case nsOrderMessageDefine::crrOrderStatus: ExecTypeChar = 'I'; break;
                default:                                   ExecTypeChar = ' '; break;
            }
            break;
        default:
            ExecTypeChar = ' ';
            break;
    }
    if ((TMPExecType == nsOrderMessageDefine::tetNewAndFilled) &&
        ((ExecTypeChar == 'F') || (ExecTypeChar == '4')))
        odkMsg.Detail.EXEC_TYPE[0] = '6';
    else
        odkMsg.Detail.EXEC_TYPE[0] = ExecTypeChar;

    UFC::AnsiString ClearMemberIdNoString;
    int CMIndex = TMP::TMPMessage::GetCMID(Msg->GetCMID(), Msg->GetBrokerID());
    ClearMemberIdNoString.Printf("%05d", CMIndex );
    ClearMemberIdNoString.PadThis( 5, '0');
    memcpy(odkMsg.Detail.CLEAR_MEMBER_ID_NO, ClearMemberIdNoString.c_str(), 5);

    UFC::AnsiString BrokerIdNoString;
    int BrokerIndex = TMP::TMPMessage::GetBrokerID(Msg->GetBrokerID());
    BrokerIdNoString.Printf("%05d", BrokerIndex );
    BrokerIdNoString.PadThis(5, '0');
    memcpy(odkMsg.Detail.BROKER_ID_NO, BrokerIdNoString.c_str(), 5);

    UFC::AnsiString OrderNoString;
    OrderNoString.Printf("%s", Msg->GetOrderID());
    OrderNoString.PadThis(5, ' ');
    memcpy(odkMsg.Detail.ORDER_NO, OrderNoString.c_str(), 5);

    UFC::AnsiString OrdIdString;
    OrdIdString.Printf("0000000");
    memcpy(odkMsg.Detail.ORD_ID, OrdIdString.c_str(), 7);

    UFC::AnsiString UserDefineString;
    UserDefineString.Printf("        ");
    memcpy(odkMsg.Detail.USER_DEFINE, UserDefineString.c_str(), 8);

    char SymbolTypeChar = '2';
    odkMsg.Detail.SYBMOL_TYPE[0] = SymbolTypeChar;

    UFC::AnsiString TrIdString;
    TrIdString.Printf("%s", Msg->GetSymbol());
    TrIdString.PadThis(20, ' ');
    memcpy(odkMsg.Detail.TR_ID, TrIdString.c_str(), 20);

    Int32 OrderPrice = 0;
//    if (ExecTypeChar == '0')
//        OrderPrice = UFC::DoubleToInt(Msg->GetPrice(), Msg->GetPxDigit());
//    else
    OrderPrice = UFC::DoubleToInt(Msg->GetLastPx(), Msg->GetPxDigit());

    UFC::BufferedLog::DebugPrintf( " Price[%12.5lf]  LastPx[%12.5lf]   PxDigit[%ld]    OrderPrice[%d]", Msg->GetPrice(), Msg->GetLastPx(), Msg->GetPxDigit(), OrderPrice );
    TempData.Printf("%+09d", OrderPrice);
    memcpy(odkMsg.Detail.ORDER_PRICE, TempData.c_str(), 9);

    UFC::BufferedLog::DebugPrintf(" TMP qty:[%d]", Msg->GetTMPQty() );

    TempData.Printf("%04d", Msg->GetTMPQty());
    memcpy(odkMsg.Detail.ORDER_QTY, TempData.c_str(), 4);
  
    UFC::BufferedLog::DebugPrintf(" [%5s] ExecTypeChar[%c]  TMPQty[%ld]  CumQty [%ld]  LeavesQty[%ld]  OrderQty[%ld]  BeforeQty[%ld] ", OrderNoString.c_str(), ExecTypeChar, Msg->GetTMPQty(), Msg->GetCumQty(), Msg->GetLeavesQty(), Msg->GetOrderQty(), Msg->GetBeforeQty());

    UFC::AnsiString InvestorAcnoString;
    InvestorAcnoString.Printf("%s", Msg->GetAccount());
    InvestorAcnoString.PadThis(7, ' ');
    memcpy(odkMsg.Detail.INVESTOR_ACNO, InvestorAcnoString.c_str(), 7);

    UFC::AnsiString InvestorFlagString;
    InvestorFlagString.Printf("%s", Msg->GetAccountFlag());
    InvestorFlagString.PadThis(1, ' ');
    odkMsg.Detail.INVESTOR_FLAG[0] = InvestorFlagString[0];

    char BSCodeChar = ' ';
    char BSKindChar = ' ';
    switch(Msg->GetSide())
    {
        case nsOrderMessageDefine::sBuy:
            BSKindChar = '1';
            BSCodeChar = 'B';
            break;
        case nsOrderMessageDefine::sSell:
            BSKindChar = '2';
            BSCodeChar = 'S';
            break;
        default:
            break;
    }
    odkMsg.Detail.BUY_SELL_KIND[0] = BSKindChar;

    char OrderTypeChar = ' ';
    switch(Msg->GetOrderType())
    {
        case nsOrderMessageDefine::otMarket:
            OrderTypeChar = '1';
            break;
        case nsOrderMessageDefine::otLimit:
            OrderTypeChar = '2';
            break;
        default:
            break;
    }
    odkMsg.Detail.ORDER_TYPE[0] = OrderTypeChar;

    char OrderCondChar = ' ';
    switch(Msg->GetTimeInForce())
    {
        case nsOrderMessageDefine::tifROD:
            OrderCondChar = '0';
            break;
        case nsOrderMessageDefine::tifIOC:
            if ((ExecTypeChar == '4') && (Msg->GetCumQty() > 0) && (Msg->GetLeavesQty() == 0) && (Msg->GetOrderQty() == 0))
            {
                memset(odkMsg.Data, ' ', 400);
                return;
            }
            OrderCondChar = '3';
            break;
        case nsOrderMessageDefine::tifFOK:
            OrderCondChar = '4';
            break;
        default:
            break;
    }
    odkMsg.Detail.ORDER_COND[0] = OrderCondChar;

    char OffsetKindChar = ' ';
    switch(Msg->GetPositionEffect())
    {
        case nsOrderMessageDefine::peOpen:
            OffsetKindChar = 'O';
            break;
        case nsOrderMessageDefine::peClose:
            OffsetKindChar = 'C';
            break;
        case nsOrderMessageDefine::peDayTrade:
            OffsetKindChar = 'D';
            break;
        default:
            break;
    }
    odkMsg.Detail.OPEN_OFFSET_KIND[0] = OffsetKindChar;

    UFC::BufferedLog::DebugPrintf( " LastMatchPrice[%20.10lf]  MatchPrice1[%20.10lf]   MatchPrice2[%20.10lf]  PxDigit[%10ld]", Msg->GetPrice(), Msg->GetLegPrice1(), Msg->GetLegPrice2(), Msg->GetPxDigit() );
    Int32 LastMatchPrice = UFC::DoubleToInt(Msg->GetPrice(), Msg->GetPxDigit());
    Int32 LastMatchQty = Msg->GetOrderQty();
    if ((MsgType == 0) || (ExecTypeChar == '0')) {
        LastMatchPrice = 0;
        LastMatchQty = 0;
    }
    TempData.Printf("%+010d", LastMatchPrice);
    memcpy(odkMsg.Detail.LAST_MATCH_PRICE, TempData.c_str(), 10);
    TempData.Printf("%04d", LastMatchQty);
    memcpy(odkMsg.Detail.LAST_MATCH_QTY, TempData.c_str(), 4);

    Int64 TotMatchPrice = (Int64)Msg->GetTMPPxSubTotal();
    TempData.Printf("%+017lld", TotMatchPrice);
    memcpy(odkMsg.Detail.TOT_MATCH_PRICE, TempData.c_str(), 17);

    TempData.Printf("%04d", Msg->GetCumQty());
    memcpy(odkMsg.Detail.TOT_MATCH_QTY, TempData.c_str(), 4);

    TempData.Printf("%04d", Msg->GetLeavesQty());
    memcpy(odkMsg.Detail.LEAVES_QTY, TempData.c_str(), 4);

    if (ExecTypeChar != '0')
        TempData.Printf("%04d", Msg->GetBeforeQty());
    memcpy(odkMsg.Detail.BEFORE_QTY, TempData.c_str(), 4);

    char BSKind1 = ' ';
    switch(Msg->GetLegSide1())
    {
        case nsOrderMessageDefine::sNone:
            BSKind1 = '0';
            break;
        case nsOrderMessageDefine::sBuy:
            BSKind1 = '1';
            break;
        case nsOrderMessageDefine::sSell:
            BSKind1 = '2';
            break;
        default:
            BSKind1 = ' ';
            break;
    }
    odkMsg.Detail.BUY_SELL_KIND1[0] = BSKind1;

    char BSKind2 = ' ';
    switch(Msg->GetLegSide2())
    {
        case nsOrderMessageDefine::sNone:
            BSKind2 = '0';
            break;
        case nsOrderMessageDefine::sBuy:
            BSKind2 = '1';
            break;
        case nsOrderMessageDefine::sSell:
            BSKind2 = '2';
            break;
        default:
            BSKind2 = ' ';
            break;
    }
    odkMsg.Detail.BUY_SELL_KIND2[0] = BSKind2;

    Int32 MatchPrice1 = UFC::DoubleToInt(Msg->GetLegPrice1(), Msg->GetPxDigit());
    TempData.Printf("%09d", MatchPrice1);
    memcpy(odkMsg.Detail.MATCH_PRICE1, TempData.c_str(), 9);

    Int32 MatchPrice2 = UFC::DoubleToInt(Msg->GetLegPrice2(), Msg->GetPxDigit());
    TempData.Printf("%09d", MatchPrice2);
    memcpy(odkMsg.Detail.MATCH_PRICE2, TempData.c_str(), 9);

    TempData.Printf("%04d", Msg->GetLegQty1());
    memcpy(odkMsg.Detail.MATCH_QTY1, TempData.c_str(), 4);

    TempData.Printf("%04d", Msg->GetLegQty2());
    memcpy(odkMsg.Detail.MATCH_QTY2, TempData.c_str(), 4);

    int OrgTransTime = UFC::AnsiString(Msg->GetOrgTransactTime()).ToInt();
    UFC::AnsiString OrderTimeString;
    OrderTimeString.Printf("%02d%02d%02d%04d",
                           OrgTransTime / 10000000, (OrgTransTime % 10000000) / 100000,
                           (OrgTransTime % 100000) / 1000, OrgTransTime % 1000);
    UFC::AnsiString OrderMilliSecondString;

    OrderMilliSecondString.Printf("%04d", OrgTransTime % 1000);
    memcpy(odkMsg.Detail.ORDER_TIME, OrderTimeString.c_str(), 6);
    memcpy(odkMsg.Detail.ORDER_MIL, OrderMilliSecondString.c_str(), 4);

    int TransTime = UFC::AnsiString(Msg->GetTransactTime()).ToInt();
    UFC::AnsiString LastTimeString;
    LastTimeString.Printf("%02d%02d%02d%04d",
                           TransTime / 10000000, (TransTime % 10000000) / 100000,
                           (TransTime % 100000) / 1000, TransTime % 1000);

    UFC::AnsiString LastMilliSecondString;
    LastMilliSecondString.Printf("%04d", TransTime % 1000);
    memcpy(odkMsg.Detail.LAST_TIME, LastTimeString.c_str(), 6);
    memcpy(odkMsg.Detail.LAST_MIL,  LastMilliSecondString.c_str(), 4);

    char TargetIdChar = '4';
    odkMsg.Detail.TARGET_ID[0] = TargetIdChar;

    TempData.Printf("%010d", Msg->GetTMPUniqueID());
    memcpy(odkMsg.Detail.UNIQUE_ID, TempData.c_str(), 10);

    TempData.Printf("%010d", Msg->GetReportSequence());
    memcpy(odkMsg.Detail.SEQNO, TempData.c_str(), 10);
    
    char ProtocolTypeChar = '1';
    odkMsg.Detail.PROTOCOL_TYPE[0] = ProtocolTypeChar;

    UFC::AnsiString BrokerIdString;
    BrokerIdString.Printf("%s", Msg->GetBrokerID());
    BrokerIdString.PadThis(7, ' ');
    memcpy(odkMsg.Detail.BROKER_ID, BrokerIdString.c_str(), 7);

    BO_SymbolDeCompose symbolDeCompose(Msg->GetSymbol(), BSCodeChar);
    UFC::AnsiString CommodityId1String;
    UFC::AnsiString CommodityId2String;
    UFC::AnsiString SettlementMonth1String;
    UFC::AnsiString SettlementMonth2String;
    UFC::AnsiString StrikePrice1String;
    UFC::AnsiString StrikePrice2String;
    UFC::AnsiString MatchPrice1String;
    UFC::AnsiString MatchPrice2String;
    UFC::AnsiString MatchPriceDString;

    UFC::AnsiString convert;

    double PriceDouble;
    if (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stUnKnown) {
        CommodityId1String.Printf("       ");
        CommodityId2String.Printf("       ");
        SettlementMonth1String.Printf("000000");
        SettlementMonth2String.Printf("000000");
        StrikePrice1String.Printf("000000000");
        StrikePrice2String.Printf("000000000");
        MatchPrice1String.Printf("000000000000");
        MatchPrice2String.Printf("000000000000");
        MatchPriceDString.Printf("+000000000000");
    }
    else
    {
        CommodityId1String.Printf("       ");
        CommodityId2String.Printf("       ");
        SettlementMonth1String.Printf("000000");
        SettlementMonth2String.Printf("000000");
        StrikePrice1String.Printf("000000000");
        StrikePrice2String.Printf("000000000");
        MatchPrice1String.Printf("000000000000");
        MatchPrice2String.Printf("000000000000");
        MatchPriceDString.Printf("+000000000000");

        TempData.Printf("%s", symbolDeCompose.FComm1);
        if (ConvertFilePtr->GetValue("Convert", TempData.SubString(0, 3), convert) == FALSE)
        {
            CommodityId1String = TempData.SubString(0, 3); ///< Not found in old symbol table. it's a new symbol, use first 3 chars.
            UFC::BufferedLog::Printf(" Can not Found New Name[%s] in convert file .", CommodityId1String.c_str());
        }
        else
            CommodityId1String = convert; ///< Example : convert TXF to FITX

        CommodityId1String.PadThis(7, ' ');
        CommodityId1String[6] = symbolDeCompose.FCP_Code1;

        SettlementMonth1String.Printf("%s", symbolDeCompose.FSettleMonth1);
        SettlementMonth1String.PadThis(6, '0');

        StrikePrice1String.Printf("%s", symbolDeCompose.FExcercisePrice1);
        StrikePrice1String.PadThis(9, '0');

        if (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle ||
            symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesSingle)
        {
            ///< It's a single order.
            CommodityId2String.Printf("       ");
            SettlementMonth2String.Printf("000000");
            StrikePrice2String.Printf("000000000");
        }
        else
        {
            ///< Fill COMMODITY_ID1_2
            TempData.Printf("%s", symbolDeCompose.FComm2);
            if (ConvertFilePtr->GetValue("Convert", TempData.SubString(0, 3), convert) == FALSE)
                CommodityId2String = TempData.SubString(0, 3); ///< Not found in old symbol table. it's a new symbol, use first 3 chars.
            else
                CommodityId2String = convert; ///< Example : convert TXF to FITX
            CommodityId2String.PadThis(7, ' ');
            CommodityId2String[6] = symbolDeCompose.FCP_Code2;

            if (symbolDeCompose.FBS_Code1 == 'B')
                odkMsg.Detail.BUY_SELL_KIND1[0] = '1';
            else
                odkMsg.Detail.BUY_SELL_KIND1[0] = '2';

            if (symbolDeCompose.FBS_Code2 == 'B')
                odkMsg.Detail.BUY_SELL_KIND2[0] = '1';
            else
                odkMsg.Detail.BUY_SELL_KIND2[0] = '2';

            SettlementMonth2String.Printf("%s", symbolDeCompose.FSettleMonth2);
            SettlementMonth2String.PadThis(6, '0');

            StrikePrice2String.Printf("%s", symbolDeCompose.FExcercisePrice2);
            StrikePrice2String.PadThis(9, '0');
        }

        UFC::AnsiString tempPrice;
        if (ExecTypeChar == 'F' &&
            (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle ||
             symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesSingle))
            PriceDouble = Msg->GetPrice();
        else
            PriceDouble = Msg->GetLegPrice1();

        tempPrice.Printf("%013.6lf", PriceDouble);
        tempPrice.PadThis(13, '0');

        MatchPrice1String.Printf("%6s%6s", tempPrice.SubString(0, 6).c_str(), tempPrice.SubString(7, 6).c_str());

        if (ExecTypeChar == 'F' &&
            (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle ||
             symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesSingle))
            PriceDouble = 0;
        else
            PriceDouble = Msg->GetLegPrice2();

        tempPrice.Printf("%013.6lf", PriceDouble);
        tempPrice.PadThis(13, '0');

        MatchPrice2String.Printf("%6s%6s", tempPrice.SubString(0, 6).c_str(), tempPrice.SubString(7, 6).c_str());

        if ((ExecTypeChar != 'F') ||
            ((ExecTypeChar == 'F') &&
             ((symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle) ||
              (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesSingle))))
            PriceDouble = 0;
        else
            PriceDouble = Msg->GetPrice();

        tempPrice.Printf("%+014.6lf", PriceDouble);
        tempPrice.PadThis(14, '0');
        MatchPriceDString.Printf("%7s%6s", tempPrice.SubString(0, 7).c_str(), tempPrice.SubString(8, 6).c_str());
    }
    memcpy(odkMsg.Detail.COMMODITY_ID1_1, CommodityId1String.c_str(), 7);
    memcpy(odkMsg.Detail.COMMODITY_ID2_2, CommodityId2String.c_str(), 7);
    memcpy(odkMsg.Detail.SETTLEMENT_MONTH1_1, SettlementMonth1String.c_str(), 6);
    memcpy(odkMsg.Detail.SETTLEMENT_MONTH2_2, SettlementMonth2String.c_str(), 6);
    memcpy(odkMsg.Detail.STRIKE_PRICE1_1, StrikePrice1String.c_str(), 9);
    memcpy(odkMsg.Detail.STRIKE_PRICE2_2, StrikePrice2String.c_str(), 9);
    memcpy(odkMsg.Detail.MATCH_PRICE1_1, MatchPrice1String.c_str(), 12);
    memcpy(odkMsg.Detail.MATCH_PRICE2_2, MatchPrice2String.c_str(), 12);
    memcpy(odkMsg.Detail.MATCH_PRICE_D, MatchPriceDString.c_str(), 13);

//    Int32 OdrPrice = 0;
//    if (ExecTypeChar == '0')
//        OdrPrice = UFC::DoubleToInt(Msg->GetPrice(), 3);
//    else
//        OdrPrice = UFC::DoubleToInt(Msg->GetLastPx(), 3);

    Int32 OdrPrice = UFC::DoubleToInt(Msg->GetLastPx(), 3);
    TempData.Printf("%+09d", OdrPrice);
    memcpy(odkMsg.Detail.ODR_PRICE, TempData.c_str(), 9);

    UFC::AnsiString ClearMemberIdString;
    ClearMemberIdString.Printf("%s", Msg->GetCMID());
    ClearMemberIdString.PadThis(4, ' ');
    memcpy(odkMsg.Detail.CLEAR_MEMBER_ID, ClearMemberIdString.c_str(), 4);

    UFC::AnsiString AeIdString;

    AeIdString.Printf("0000");
    memcpy(odkMsg.Detail.AE_ID, AeIdString.c_str(), 4);

    char DayTradeIdChar = ' ';
    if (OffsetKindChar == 'D')
        DayTradeIdChar = 'Y';
    odkMsg.Detail.DAY_TRADE_ID[0] = DayTradeIdChar;

    UFC::AnsiString IbNoString;
    IbNoString.Printf("   ");

    memcpy(odkMsg.Detail.IB_NO, IbNoString.c_str(), 3);

    char DosInKindChar = ' ';
    UFC::AnsiString SubInvestorAcNoString;
    SubInvestorAcNoString.Printf("   ");
    if (Msg->GetData() != NULL)
    {
       UFC::BufferedLog::DebugPrintf( " Data:[%s]", Msg->GetData() );
       TempData.Printf("%s", Msg->GetData());
       if ((TempData.Length() >= 25) && (TempData.AnsiPos("<") == 0) && (TempData.AnsiPos("Proxy") == 1) && (TempData.AnsiPos(">") == 24))
       {
            SubInvestorAcNoString.Printf( "   " );
            if (TempData.Length() >= 44) DosInKindChar = TempData[43];
       }
       else if ((TempData.Length() >= 48) && (TempData.AnsiPos("46") == 0))
       {
            if (TempData.SubString(13, 3) == "000")
                SubInvestorAcNoString.Printf("   ");
            else
                SubInvestorAcNoString.Printf("%3s", TempData.SubString(13, 3).c_str());
            
            if (TempData[18] == 'N')
                DosInKindChar = 'N';
            else
                DosInKindChar = ' ';
       }
       else if (TempData.Length() >= 3)
       {
           SubInvestorAcNoString.Printf("%3s", TempData.SubString(0, 3).c_str());
           if (TempData.Length() >= 4)
               DosInKindChar = TempData[3];
       }
    }  
    
    UFC::BufferedLog::DebugPrintf( " SubAccount:[%s] DosInKind:[%c]", SubInvestorAcNoString.c_str(), DosInKindChar );
    odkMsg.Detail.DOS_IN_KIND[0] = DosInKindChar;
    memcpy(odkMsg.Detail.SUB_INVESTOR_ACNO, SubInvestorAcNoString.c_str(), 3);

    memcpy(odkMsg.Detail.NT_CHANNEL, OdkNtChannelString.c_str(), 2);

    UFC::AnsiString FillerString;
    FillerString.Printf("                           ");
    memcpy(odkMsg.Detail.FILLER, FillerString.c_str(), 27);

    MessageString.Printf("%s", odkMsg.Data);
} 

//------------------------------------------------------------------------------
void PMFExecConnection::OnExecutionReport(TExecutionReportMessage* Msg, ExecDup PossDup)
{
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();

    if (Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mOTC)
        return;

    UFC::BufferedLog::DebugPrintf( " OnExecutionReport:[%s]", Msg->GetSrc() );

    nsOrderMessageDefine::ExecTypeEnum ExecType = Msg->GetExecType();
//    UFC::BufferedLog::DebugPrintf( " ExecType:[%d]", (int)ExecType );
//    UFC::BufferedLog::DebugPrintf( " TimeInForce:[%d]", (int)Msg->GetTimeInForce() );
    
    UFC::AnsiString MessageString = "";
    switch (ExecType)
    {
    case nsOrderMessageDefine::osNew :
        if (MatchOnly == FALSE)
            BuildMessageString(Msg, MessageString);
        else
            return;

        break;
    case nsOrderMessageDefine::osReplaced :
    case nsOrderMessageDefine::osCanceled :
        if (MatchOnly == FALSE)
            BuildMessageString(Msg, MessageString);
        else
            return;

        break;
    case nsOrderMessageDefine::osPartiallyFilled :
    case nsOrderMessageDefine::osFilled :
        BuildMessageString(Msg, MessageString);

        break;
    case nsOrderMessageDefine::osRejected :
        if (MatchOnly == FALSE)
            BuildMessageString(Msg, MessageString);
        else
            return;

        break;
    case nsOrderMessageDefine::osQuoteAccept :
        if (MatchOnly == FALSE)
            BuildMessageString(Msg, MessageString);
        else
            return;

        break;
    case nsOrderMessageDefine::osPendingNew :
    case nsOrderMessageDefine::osPendingReplace :
    case nsOrderMessageDefine::osPendingCancel :
    default:
        return;
        break;
    } 

    if (MessageString.Length() > 0)
    {
        if (DumpOnly == FALSE) {
            if (Market == nsOrderMessageDefine::mTWFutures)
            {
//UFC::BufferedLog::Printf(" Futures: length[%d] [%s]", MessageString.Length(), MessageString.c_str());
                FutMessageStore->Add(MessageString.c_str());
            }
            else if (Market == nsOrderMessageDefine::mTWOptions)
            {
//UFC::BufferedLog::Printf(" Options: length[%d] [%s]", MessageString.Length(), MessageString.c_str());
                OptMessageStore->Add(MessageString.c_str());
            }
            else
                UFC::BufferedLog::Printf(" OnExecutionReport() Error UnKnown Market [%d]", Market);
        } 
        else
        {
            UFC::BufferedLog::Printf(" Send dump: length[%d]   [%s]", MessageString.Length(), MessageString.c_str());
        }
    }
    UFC::BufferedLog::Printf(" ");
} 

//------------------------------------------------------------------------------
void PMFExecConnection::Execute( void )
{
    while( !IsTerminated() )
    {
        if( FIsLogon == FALSE )
            FConnection->Connect( FIP.c_str() , FPort, 10 );
        sleep( 10 ); ///< wait 10 sec.
    }
}
//------------------------------------------------------------------------------
