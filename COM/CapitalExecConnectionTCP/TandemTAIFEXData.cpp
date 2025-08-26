
#include "TandemObject.h"
#include "TMPMessage.h"      //TAIFEXLib
#include "MultilegSymbol.h"  //TaifexLib
//------------------------------------------------------------------------------
extern UFC::AnsiString  G_Today;
extern UFC::AnsiString  G_NextTradeDay;
//------------------------------------------------------------------------------
namespace capital
{
//--------------------------------- CUserData ----------------------------------
UFC::AnsiString CUserData::FFieldDelimiter = ",";
UFC::AnsiString CUserData::FNameValueDelimiter = "=";

//------------------------------------------------------------------------------
CUserData::CUserData()
:FSubAccountNo(" "),                    //62.ODK-SUB-MOTHER-ACNO X(7)   355
 FAEID(""),                             //58.ODK-AE-ID           9(4)   346
 FInputSeqNo(""),                       //65.ODK-INPUT-SEQNO     S9(13) 375
 FAp2InKind('s'),                       //67.ODK-AP2-IN-KIND     X(01)  403
 FSourceIP(UFC::PSocket::GetLocalIP())  //69.ODK-SRC-IP          X(20)  420
{
    FSourceIP.PadThis(20, ' ');
}  //CUserData::CUserData()

//------------------------------------------------------------------------------
CUserData::CUserData(const UFC::AnsiString& csvStr)
:FSubAccountNo(""),  //62.ODK-SUB-MOTHER-ACNO X(7)  355
 FAEID(""),          //58.ODK-AE-ID           9(4)  346
 FInputSeqNo(""),    //65.ODK-INPUT-SEQNO     S9(13) 375
 FAp2InKind('s'),    //67.ODK-AP2-IN-KIND     X(01) 403
 FSourceIP("")       //69.ODK-SRC-IP          X(20) 420
{    
    ParseFieldValue(csvStr);
    if (FSourceIP.Length() <= 0) 
    {
        FSourceIP = UFC::PSocket::GetLocalIP();
        FSourceIP.PadThis(20, ' ');
    }
}  //CUserData::CUserData()

//------------------------------------------------------------------------------
void CUserData::ParseFieldValue(const UFC::AnsiString& csvStr)
{
    UFC::PStringList NameValueList;
    NameValueList.SetStrings(csvStr, FFieldDelimiter);
    for (register int i = 0; i < NameValueList.ItemCount(); i++ )
    {
        UFC::AnsiString NameValueStr = NameValueList.GetItem(i);
        Int32 equlalPos = NameValueStr.AnsiPos(FNameValueDelimiter);
        if ((equlalPos > 0) && (NameValueStr.LastChar() != FNameValueDelimiter[0]))
        {
            UFC::AnsiString NameStr = NameValueStr.SubString(0, equlalPos);
            UFC::AnsiString ValueStr = NameValueStr.SubString(equlalPos + 1, NameValueStr.Length() - NameStr.Length() - 1);
            if ((NameStr.AnsiCompare("subacno") == 0) || (NameStr.AnsiCompare("SUBACNO") == 0))
                FSubAccountNo.Printf("%07d", ValueStr.ToInt());
            else if ((NameStr.AnsiCompare("aeid") == 0) || (NameStr.AnsiCompare("AEID") == 0))
                FAEID.Printf("%04d", ValueStr.ToInt());
            else if ((NameStr.AnsiCompare("apkind") == 0) || (NameStr.AnsiCompare("APKIND") == 0))
                FAp2InKind = ValueStr[0];
            else if ((NameStr.AnsiCompare("ip") == 0) || (NameStr.AnsiCompare("IP") == 0))
                FSourceIP = ValueStr;
            else if ((NameStr.AnsiCompare("key_no") == 0) || (NameStr.AnsiCompare("KEY_NO") == 0))
            {
                if ((ValueStr.Length() > 0) && (ValueStr.AnsiCompareIC("null") != 0))
                {
                    if (ValueStr.Length() > 13)
                    {
                        int fit = ValueStr.Length() - 13;
                        ValueStr = ValueStr.SubString(fit, 13);
                    }                    
                    FInputSeqNo.Printf("%+014d", ValueStr.ToInt());
                }
            }
        }  //if ((equlalPos > 0) && (NameValueStr.LastChar() != FNameValueDelimiter[0]))
    }  //for (register int i = 0; i < NameValueList.ItemCount(); i++ )    
}  //CUserData::ParseFieldValue()    
//------------------------------------------------------------------------------
TandemTAIFEXData::TandemTAIFEXData( TExecutionReportMessage *ExecRptPtr )
{
    FExecRptMarket      = nsOrderMessageDefine::mCNFutures;
    FExecRptOrderStatus = nsOrderMessageDefine::osNone; 
    FExecID             = "";
    
    FTradeDate           = "        ";              // 1.ODK-DATE                9(8)          0
    FSequenceNo          = 0;                       // 2.ODK-SEQNUM              9(10)         8
    FReceiveDate         = "        ";              // 3.ODK-MSG-DATE            9(8)         18
    FReceiveTime         = "      ";                // 4.ODK-MSG-TIME            9(6)         26
    FReceiveMilliSecond  = "    ";                  // 5.ODK-MSG-MIL             9(4)         32
    FMessageType         = "   ";                   // 6.ODK-MSG-TYPE            X(3)         36
    FLinkBrokerNo        = "     ";                 // 7.ODK-LNK-BROKER-NO       X(5)         39
    FSessionID           = "     ";                 // 8.ODK-SESSION-ID          9(5)         44
    FStatusCode          = "   ";                   // 9.ODK-STATUS-CODE         X(3)         49
    FExecType            = ' ';                     //10.ODK-EXEC-TYPE           X(1)         52
    FClearMemberIDNo     = "     ";                 //11.ODK-CLEAR-MEMBER-ID-NO  X(5)         53
    FBrokerIDNo          = "     ";                 //12.ODK-BROKER-ID-NO        X(5)         58
    FOrderNo             = "     ";                 //13.ODK-ORDER-NO            X(5)         63
    FOrderID             = "       ";               //14.ODK-ORD-ID              9(7)         68
    FUserDefine          = "        ";              //15.ODK-USER-DEFINE         X(8)         75
    FSymbolType          = ' ';                     //16.ODK-SYMBOL-TYPE         X(1)         83
    FSymbol              = "                    ";  //17.ODK-TR-ID               X(20)        84
    FOrderPrice          = 0;                       //18.ODK-ORDER-PRRICE        S9(8)       104
    FOrderQty            = 0;                       //19.ODK-ORDER-QTY           9(4)        113
    FInvestorAccountNo   = "       ";               //20.ODK-INVESTOR-ACNO       X(7)        117
    FInvestorFlag        = ' ';                     //21.ODK-INVESTOR-FLAG       X(1)        124
    FSide                = ' ';                     //22.ODK-BUY-SELL-KIND       X(1)        125
    FOrderType           = ' ';                     //23.ODK-ORDER-TYPE          X(1)        126
    FTimeInForce         = ' ';                     //24.ODK-ORDER-COND          X(1)        127
    FPositionEffect      = ' ';                     //25.ODK-OPEN-OFFSET-KIND    X(1)        128
    FLastPrice           = 0;                       //26.ODK-LAST-MATCH-PRICE    S9(9)       129
    FLastQty             = 0;                       //27.ODK-LAST-MATCH-QTY      9(4)        139
    FPriceSubTotal       = 0;                       //28.ODK-TOT-MATCH-PRICE     S9(16)      143
    FCumQty              = 0;                       //29.ODK-TOT-MATCH-QTY       9(4)        160
    FLeavesQty           = 0;                       //30.ODK-LEAVES-QTY          9(4)        164
    FBeforeQty           = 0;                       //31.ODK-BEFORE-QTY          9(4)        168
    FLegSide1            = '0';                     //32.ODK-BUY-SELL-KIND1      X(1)        172
    FLegSide2            = '0';                     //33.ODK-BUY-SELL-KIND2      X(1)        173
    FLegPrice1           = 0;                       //34.ODK-MATCH-PRICE1        9(9)        174
    FLegPrice2           = 0;                       //35.ODK-MATCH-PRICE2        9(9)        183
    FLegQty1             = 0;                       //36.ODK-MATCH-QTY1          9(4)        192
    FLegQty2             = 0;                       //37.ODK-MATCH-QTY2          9(4)        196
    FOrderDate           = "        ";              //38.ODK-ORDER-DATE          9(8)        200
    FOrderTime           = "      ";                //39.ODK-ORDER-TIME          9(6)        208
    FOrderMilliSecond    = "    ";                  //40.ODK-ORDER-MIL           9(4)        214
    FTransactDate        = "        ";              //41.ODK-LAST-DATE           9(8)        218
    FTransactTime        = "      ";                //42.ODK-LAST-TIME           9(6)        226
    FTransactMilliSecond = "    ";                  //43.ODK-LAST-MIL            9(4)        232
    FTargetID            = ' ';                     //44.ODK-TARGET-ID           X(1)        236
    FUniqueID            = 0;                       //45.ODK-UNIQUE-ID           9(10)       237
    FReportSequenceNo    = 0;;                      //46.ODK-SEQNO               9(10)       247
    FProtocolType        = ' ';                     //47.ODK-PROTOCOL-TYPE       X(1)        257
    FBrokerID            = "       ";               //48.ODK-BROKER-ID           X(7)        258
    FCommodityID1        = "       ";               //49.ODK-COMMODITY-ID1-1     X(7)        265
    FSettlementMonth1    = "      ";                //50.ODK-SETTLEMENT-MONTH1-1 9(6)        272
    FStrikePrice1        = "         ";             //51.ODK-STRIKE-PRICE1-1     9(6)V9(3)   278
    FPrice1              = 0;                       //52.ODK-PRICE1-1            9(6)V9(6)   287
    FCommodityID2        = "       ";               //53.ODK-COMMODITY-ID2-2     X(7)        299
    FSettlementMonth2    = "      ";                //54.ODK-SETTLEMENT-MONTH2-2 9(6)        306
    FStrikePrice2        = "         ";             //55.ODK-STRIKE-PRICE2-2     9(6)V9(3)   312
    FPrice2              = 0;                       //56.ODK-PRICE2-2            9(6)V9(6)   321
    FPriceDifference     = 0;                       //57.ODK-PRICE2-D            S9(6)V9(6)  333
    FAEID                = "    ";                  //58.ODK-AE-ID               9(4)        346
    FDayTradeID          = ' ';                     //59.ODK-DAY-TRADE-ID        X(1)        350
    FIBNO                = "   ";                   //60.ODK-IB-NO               X(03)       351
    FDosInKind           = ' ';                     //61.ODK-DOS-IN-KIND         X(01)       354
    FSubAccountNo        = "       ";               //62.ODK-SUB-MOTHER-ACNO     X(7)        355
    FOdrPrice            = 0;                       //63.ODK-ODR-PRRICE          S9(05)V9(4) 362
    FClearMemberID       = "F02";                   //64.ODK-CLEAR-MEMBER-ID     X(03)       372
    FInputSeqNo          = "              ";        //65.ODK-INPUT-SEQNO         S9(13)      375
    FInputKey            = "              ";        //66.ODK-INPUT-KEY           S9(13)      389
    FAp2InKind           = ' ';                     //67.ODK-AP2-IN-KIND         X(01)       403
    FAp2InKindN          = ' ';                     //68.ODK-AP2-IN-KIND-N       X(01)       404
    FPriceFlag           = ' ';                     //69.ODK-PRICE-FLAG          X(01)       405
    FMarketMark          = ' ';                     //70.ODK-MARKET-MARK         X(01)       406
    FSessionMark         = 'N';                     //71.ODK-MARKET-MARK         X(01)       406
    FFLEXFlag            = ' ';                     //72.ODK-FLEXFLAG            X(01)       407
    FPartID              = "000";                   //73.ODK-PART-ID             9(03)       408
    FStatusCodeA         = "00";                    //74.ODK-STATUS-CODE-A       9(02)       411    
    FFiller              = "       ";               //75.FILLER                  X(07)       413
    FSourceIP            = "                    ";  //76.ODK-SRC-IP              X(20)       420
    SetFieldsValue( ExecRptPtr );
} 
//------------------------------------------------------------------------------
void TandemTAIFEXData::SetFieldsValue(TExecutionReportMessage *ExecRptPtr)
{
    FExecRptMarket      = ExecRptPtr->GetMarket();
    FExecRptOrderStatus = ExecRptPtr->GetOrderStatus(); 
    FExecID             = ExecRptPtr->GetExecID();           
    nsOrderMessageDefine::ExecTypeEnum rptExecType = ExecRptPtr->GetExecType();
    nsOrderMessageDefine::TMPExecTypeEnum rptTMPExecType = ExecRptPtr->GetTMPExecType();
    nsOrderMessageDefine::CxlRejResponseToEnum rptRejResponseTo = ExecRptPtr->GetCxlRejResponseTo();
    UFC::UDateTime Now;
    
    if( ExecRptPtr->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour ) // 1.ODK-DATE 9(8) 0
        FTradeDate = G_NextTradeDay;
    else
        FTradeDate = G_Today;    
    FSequenceNo = CTandemMessage::GetNextSequenceNo(true); // 2.ODK-SEQNUM 9(10) 8
    FReceiveDate.Printf("%04d%02d%02d", Now.getYear(), Now.getMonth(), Now.getDate()); // 3.ODK-MSG-DATE 9(8) 18
    FReceiveTime.Printf("%02d%02d%02d", Now.getHour(), Now.getMinute(), Now.getSecond());  // 4.ODK-MSG-TIME 9(6) 26
    FReceiveMilliSecond.Printf("%04d", Now.getMillisecond()); // 5.ODK-MSG-MIL 9(4) 32

    // 6.ODK-MSG-TYPE X(3) 36
    if (FExecRptOrderStatus == nsOrderMessageDefine::osRejected)
        FMessageType.Printf("%03d", 103);
    else if ((FExecRptOrderStatus = nsOrderMessageDefine::osNew) ||
             (FExecRptOrderStatus = nsOrderMessageDefine::osReplaced) ||
             (FExecRptOrderStatus = nsOrderMessageDefine::osCanceled) ||
             (FExecRptOrderStatus = nsOrderMessageDefine::osPartiallyFilled) ||
             (FExecRptOrderStatus = nsOrderMessageDefine::osFilled) ||
             (FExecRptOrderStatus = nsOrderMessageDefine::osQuoteAccept))
        FMessageType.Printf("%03d", 102);
    else
        FMessageType.Printf("   ");
        
    FLinkBrokerNo.Printf("%05d", ExecRptPtr->GetTMPBrokerID());  // 7.ODK-LNK-BROKER-NO X(5) 39
    
    // 8.ODK-SESSION-ID 9(5) 44
    UFC::AnsiString tempValue = ExecRptPtr->GetPVC();
    if (tempValue.Length() > 0)
        FSessionID.Printf("%05d", tempValue.ToInt());
    else
        FSessionID = "     ";

    FStatusCode.Printf("%03d", ExecRptPtr->GetTMPStstusCode());  // 9.ODK-STATUS-CODE X(3) 49
    FExecType = TranslateExecType(rptExecType, rptTMPExecType, rptRejResponseTo);  //10.ODK-EXEC-TYPE X(1) 52
    
    int clearMemberIndex = TMP::TMPMessage::GetCMID(ExecRptPtr->GetCMID(),ExecRptPtr->GetBrokerID());
    FClearMemberIDNo.Printf("%05d", clearMemberIndex);  //11.ODK-CLEAR-MEMBER-ID-NO X(5) 53
    
    int BrokerIndex = TMP::TMPMessage::GetBrokerID(ExecRptPtr->GetBrokerID());
    FBrokerIDNo.Printf("%05d", BrokerIndex );  //12.ODK-BROKER-ID-NO X(5) 58
    
    FOrderNo.Printf("%s", ExecRptPtr->GetOrderID());  //13.ODK-ORDER-NO X(5) 63
    FOrderNo.PadThis(5,' ');
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " OrderNo[%s] ExecID[%s] OrderStatus[%d] ExecType[%d] TMPExecType[%c] rptRejResponseTo[%d] => [%c]", 
                                  FOrderNo.c_str(), FExecID.c_str(), FExecRptOrderStatus, rptExecType, rptTMPExecType, rptRejResponseTo, FExecType);
    
    FOrderID = "0000000";  //14.ODK-ORD-ID 9(7) 68
    
    UFC::AnsiString rptData = ExecRptPtr->GetData();  //UserData 8-15 X(8)
    rptData.PadThis(15, ' ');
//    FUserDefine = rptData.SubString(7, 8);  //15.ODK-USER-DEFINE X(8) 75
    FUserDefine = "        ";  //15.ODK-USER-DEFINE X(8) 75
    
    FSymbolType = '2';  //16.ODK-SYMBOL-TYPE X(1) 83   
    FSymbol = ExecRptPtr->GetSymbol();  //17.ODK-TR-ID X(20) 84
    FSymbol.PadThis(20, ' ');    
    long PriceDigit = ExecRptPtr->GetPxDigit();
    FOrderPrice = UFC::DoubleToInt(ExecRptPtr->GetLastPx(), PriceDigit);  //18.ODK-ORDER-PRRICE S9(8) 104
    FOrderQty = ExecRptPtr->GetTMPQty();  //19.ODK-ORDER-QTY 9(4) 113
    FInvestorAccountNo = ExecRptPtr->GetAccount();  //20.ODK-INVESTOR-ACNO X(7) 117
    FInvestorAccountNo.PadThis(7, ' ');
    
    //21.ODK-INVESTOR-FLAG X(1) 124
    if (rptExecType == nsOrderMessageDefine::etQuoteAccept )
        FInvestorFlag = '8';
    else
        FInvestorFlag = *(ExecRptPtr->GetAccountFlag());
    char buySellCode;
    FSide = TranslateSide(ExecRptPtr->GetSide(), buySellCode);  //22.ODK-BUY-SELL-KIND X(1) 125
    FOrderType = TranslateOrderType(ExecRptPtr->GetOrderType());  //23.ODK-ORDER-TYPE X(1) 126
    FTimeInForce = TranslateTimeInForce(ExecRptPtr->GetTimeInForce());  //24.ODK-ORDER-COND X(1) 127
    FPositionEffect = TranslatePositionEffect(ExecRptPtr->GetPositionEffect());  //25.ODK-OPEN-OFFSET-KIND X(1) 128
    if ((FExecType == '4') || (FExecType == '5') || (FExecType == 'M') || (FExecType == 'm'))
    {  //Canceled             Reduce Qty            Replace Price         Replace Price
        FLastPrice = 0;  //26.ODK-LAST-MATCH-PRICE S9(9) 129
        FLastQty   = 0;  //27.ODK-LAST-MATCH-QTY 9(4) 139
    }
    else
    {
        FLastPrice = UFC::DoubleToInt(ExecRptPtr->GetPrice(), PriceDigit);  //26.ODK-LAST-MATCH-PRICE S9(9) 129
        FLastQty   = ExecRptPtr->GetOrderQty();  //27.ODK-LAST-MATCH-QTY 9(4) 139
    }
    if ((FExecRptOrderStatus == nsOrderMessageDefine::osReplaced) ||
        (FExecRptOrderStatus == nsOrderMessageDefine::osCanceled) ||
        (FExecType == '0'))  //New
    {
        FLastPrice = 0;
        FLastQty = 0;
    }
//UFC::BufferedLog::Printf(" ExecID[%s] Price[%16.6lf] LegPrice1[%16.6lf] LegPrice2[%16.6lf] LastPx[%16.6lf]", FExecID.c_str(), ExecRptPtr->GetPrice(), ExecRptPtr->GetLegPrice1(), ExecRptPtr->GetLegPrice2(), ExecRptPtr->GetLastPx());
//UFC::BufferedLog::FlushToFile();
    FPriceSubTotal = (Int64)ExecRptPtr->GetTMPPxSubTotal();  //28.ODK-TOT-MATCH-PRICE S9(16) 143
    FCumQty = ExecRptPtr->GetCumQty();  //29.ODK-TOT-MATCH-QTY 9(4) 160
    FLeavesQty = ExecRptPtr->GetLeavesQty();  //30.ODK-LEAVES-QTY 9(4) 164
    FBeforeQty = ExecRptPtr->GetBeforeQty();  //31.ODK-BEFORE-QTY 9(4) 168
    FLegPrice1 = UFC::DoubleToInt(ExecRptPtr->GetLegPrice1(), PriceDigit);  //34.ODK-MATCH-PRICE1 9(9) 174
    FLegPrice2 = UFC::DoubleToInt(ExecRptPtr->GetLegPrice2(), PriceDigit);  //35.ODK-MATCH-PRICE2 9(9) 183
    FLegQty1 = ExecRptPtr->GetLegQty1();  //36.ODK-MATCH-QTY1 9(4) 192
    FLegQty2 = ExecRptPtr->GetLegQty2();  //37.ODK-MATCH-QTY2 9(4) 196
    FOrderDate = FTradeDate;  //38.ODK-ORDER-DATE 9(8) 200
    
    //39.ODK-ORDER-TIME 9(6) 208
    UFC::AnsiString OrgTransactTimeStr(ExecRptPtr->GetOrgTransactTime());
    OrgTransactTimeStr.PadThis(9, '0');
    int OrgTransactTime = OrgTransactTimeStr.ToInt();
    FOrderTime.Printf("%02d%02d%02d", OrgTransactTime / 10000000,
                      (OrgTransactTime % 10000000) / 100000, (OrgTransactTime % 100000) / 1000);
    FOrderMilliSecond.Printf("%04d", OrgTransactTime % 1000);  //40.ODK-ORDER-MIL 9(4) 214
    FTransactDate = FTradeDate;  //41.ODK-LAST-DATE 9(8) 218
    
    //42.ODK-LAST-TIME 9(6) 226
    UFC::AnsiString TransactTimeStr(ExecRptPtr->GetTransactTime());
    TransactTimeStr.PadThis(9, '0');
    int TransactTime = TransactTimeStr.ToInt();
    FTransactTime.Printf("%02d%02d%02d", TransactTime / 10000000,
                         (TransactTime % 10000000) / 100000, (TransactTime % 100000) / 1000);
    FTransactMilliSecond.Printf("%04d", TransactTime % 1000);  //43.ODK-LAST-MIL 9(4) 214
    FTargetID = '4';  //44.ODK-TARGET-ID X(1) 236
    FUniqueID = ExecRptPtr->GetTMPUniqueID();  //45.ODK-UNIQUE-ID 9(10) 237
    FReportSequenceNo = ExecRptPtr->GetReportSequence();  //46.ODK-SEQNO 9(10) 247
    FProtocolType = '1';  //47.ODK-PROTOCOL-TYPE X(1) 257
    FBrokerID = ExecRptPtr->GetBrokerID();  //48.ODK-BROKER-ID X(7) 258
    FBrokerID.PadThis(7, ' ');
    
    BO_SymbolDeCompose symbolDeCompose(ExecRptPtr->GetSymbol(), buySellCode);
    if (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stUnKnown) //De compose failed
    {
        FLegSide1 = '0';               //32.ODK-BUY-SELL-KIND1      X(1)       172
        FLegSide2 = '0';               //33.ODK-BUY-SELL-KIND2      X(1)       173
        FCommodityID1 = "       ";     //49.ODK-COMMODITY-ID1-1     X(7)       265
        FSettlementMonth1 = "000000";  //50.ODK-SETTLEMENT-MONTH1-1 9(6)       272
        FStrikePrice1 = "000000000";   //51.ODK-STRIKE-PRICE1-1     9(6)V9(3)  278
        FPrice1 = 0;                   //52.ODK-PRICE1-1            9(6)V9(6)  287
        FCommodityID2 = "       ";     //53.ODK-COMMODITY-ID2-2     X(7)       299
        FSettlementMonth2 = "000000";  //54.ODK-SETTLEMENT-MONTH2-2 9(6)       306
        FStrikePrice2 = "000000000";   //55.ODK-STRIKE-PRICE2-2     9(6)V9(3)  312
        FPrice2 = 0;                   //56.ODK-PRICE2-2            9(6)V9(6)  321
        FPriceDifference = 0;          //57.ODK-PRICE2-D            S9(6)V9(6) 333
    }
    else
    {
        UFC::AnsiString OrigSymbol1 = symbolDeCompose.FComm1;
        FCommodityID1 = ConvertToCommodityID(OrigSymbol1, FExecRptMarket, symbolDeCompose.FIsFLEX, symbolDeCompose.FCP_Code1);  //49.ODK-COMMODITY-ID1-1 X(7) 265
        FSettlementMonth1 = symbolDeCompose.FSettleMonth1;  //50.ODK-SETTLEMENT-MONTH1-1 9(6) 272
        FSettlementMonth1.PadThis(6, '0');
        FStrikePrice1 = symbolDeCompose.FExcercisePrice1;   //51.ODK-STRIKE-PRICE1-1 9(6)V9(3) 278
        FStrikePrice1.PadThis(9, '0');
        if ((symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle) || 
            (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesSingle)||
            (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFLEXSingle) || 
            (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesFLEXSingle))
        {   // It's a single order.
            FCommodityID2 = "       ";     //53.ODK-COMMODITY-ID2-2 X(7) 299
            FSettlementMonth2 = "000000";  //54.ODK-SETTLEMENT-MONTH2-2 9(6) 306
            FStrikePrice2 = "000000000";   //55.ODK-STRIKE-PRICE2-2 9(6)V9(3) 312
            FLegSide1 = '0';  //32.ODK-BUY-SELL-KIND1 X(1) 172
            FLegSide2 = '0';  //33.ODK-BUY-SELL-KIND2 X(1) 173
            
            if ((FExecType == 'F') || (FExecType == '6'))  //F: Fill; 6:New & Fill
                FPrice1 = DoubleToInt64(ExecRptPtr->GetPrice(), 6);  //52.ODK-PRICE1-1 9(6)V9(6) 287
            else
                FPrice1 = 0;
            FPrice2 = 0;           //56.ODK-PRICE2-2 9(6)V9(6)  321
            FPriceDifference = 0;  //57.ODK-PRICE2-D S9(6)V9(6) 333
        }
        else
        {
            UFC::AnsiString OrigSymbol2 = symbolDeCompose.FComm2;
            FCommodityID2 = ConvertToCommodityID(OrigSymbol2, FExecRptMarket, symbolDeCompose.FIsFLEX, symbolDeCompose.FCP_Code2);  //53.ODK-COMMODITY-ID2-2 X(7) 299
            FSettlementMonth2 = symbolDeCompose.FSettleMonth2;  //54.ODK-SETTLEMENT-MONTH2-2 9(6) 306
            FSettlementMonth2.PadThis(6, '0');
            FStrikePrice2 = symbolDeCompose.FExcercisePrice2;   //55.ODK-STRIKE-PRICE2-2 9(6)V9(3) 312
            FStrikePrice2.PadThis(9, '0');
            if (symbolDeCompose.FBS_Code1 == 'B')
                FLegSide1 = '1';  //32.ODK-BUY-SELL-KIND1 X(1) 172;
            else
                FLegSide1 = '2';  //32.ODK-BUY-SELL-KIND1 X(1) 172;
                        
            if (symbolDeCompose.FBS_Code2 == 'B')
                FLegSide2 = '1';  //33.ODK-BUY-SELL-KIND2 X(1) 173
            else
                FLegSide2 = '2';  //33.ODK-BUY-SELL-KIND2 X(1) 173

            if ((FExecType == 'F') || (FExecType == '6'))  //F: Fill; 6:New & Fill
            {
                FPrice1 = DoubleToInt64(ExecRptPtr->GetLegPrice1(), 6);       //52.ODK-PRICE1-1 9(6)V9(6) 287
                FPrice2 = DoubleToInt64(ExecRptPtr->GetLegPrice2(), 6);       //56.ODK-PRICE2-2 9(6)V9(6) 321
                FPriceDifference = DoubleToInt64(ExecRptPtr->GetPrice(), 6);  //57.ODK-PRICE2-D S9(6)V9(6) 333
            }
            else
            {
                FPrice1 = 0;           //52.ODK-PRICE1-1 9(6)V9(6) 287
                FPrice2 = 0;           //56.ODK-PRICE2-2 9(6)V9(6) 321
                FPriceDifference = 0;  //57.ODK-PRICE2-D S9(6)V9(6) 333
            }
        }  //if ((symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle) ||
    }  //if (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stUnKnown)
    
    UFC::AnsiString userData = ExecRptPtr->GetData();
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " OrderNo[%s] ExecID[%s] UserData[%s]", FOrderNo.c_str(), FExecID.c_str(), userData.c_str());
    UFC::BufferedLog::FlushToFile();
    CUserData *userDataPtr = new CUserData(userData);    
    FAEID = userDataPtr->GetAEID();  //58.ODK-AE-ID 9(4) 346
    FAEID.PadThis(4, '0');
    if (FPositionEffect == 'D') FDayTradeID = 'D';  //59.ODK-DAY-TRADE-ID X(1) 350

    //60.ODK-IB-NO X(03) 351
    FIBNO = FBrokerID.SubString(4, 3);
    
    FDosInKind = 'N';  //61.ODK-DOS-IN-KIND X(01) 354

    ///< SUB_INVERSTOR_ACNO
    FSubAccountNo = userDataPtr->GetSubAccountNo();  //62.ODK-SUB-MOTHER-ACNO X(7) 355
    if (FSubAccountNo.Length() < 7) FSubAccountNo.PadThis(7, ' ');
    
    //63.ODK-ODR-PRRICE S9(05)V9(4) 362
    if(FExecRptOrderStatus == nsOrderMessageDefine::osQuoteAccept)
        FOdrPrice = UFC::DoubleToInt(ExecRptPtr->GetPrice(), 4 );
    else
        FOdrPrice = UFC::DoubleToInt(ExecRptPtr->GetLastPx(), 4 );

    FClearMemberID = ExecRptPtr->GetCMID();  //64.ODK-CLEAR-MEMBER-ID X(03) 371
    FClearMemberID.PadThis(3, '0');
    FInputSeqNo = userDataPtr->GetInputSeqNo();  //65.ODK-INPUT-SEQNO S9(13) 375
    if (FInputSeqNo.Length() <= 0)
    {
        Int64 tempKeyNo = 9990000000000LL + ExecRptPtr->GetNID();
        FInputSeqNo.Printf("%+014lld", tempKeyNo);
    }
    
    FInputKey = "+0000000000000";              //66.ODK-INPUT-KEY   S9(13) 389
    FAp2InKind = userDataPtr->GetAp2InKind();  //67.ODK-AP2-IN-KIND X(01)  403
    
    if( ExecRptPtr->GetTradingSessionID() == nsOrderMessageDefine::tsNormal ) //71. ODK-ETS-FLAG         X(01)       406
        FSessionMark = 'N';    	            
    else if( ExecRptPtr->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
        FSessionMark = 'Y';    	                
    
    if( ExecRptPtr->GetTMPPartID() != 0 ) //72.ODK-ETS-FLAG            X(01)       407
        FFLEXFlag = 'Y';
    else
        FFLEXFlag = 'N';    
    
    FPartID.Printf("%03d", ExecRptPtr->GetTMPPartID()%1000 );           //73.ODK-PART-ID             9(03)       408
    FStatusCodeA.Printf("%02d", ExecRptPtr->GetTMPStstusCode()/1000);   //74.ODK-STATUS-CODE-A       9(02)       411    
    
    FFiller = "                ";              //75.FILLER          X(6)  404
    FSourceIP = userDataPtr->GetSourceIP();    //76.ODK-SRC-IP      X(20)  420
    if (FSourceIP.Length() < 20) FSourceIP.PadThis(20, ' ');
    delete userDataPtr;
}  //CTandemMessage::FillField()
//------------------------------------------------------------------------------
void TandemTAIFEXData::GenerateTandemData( Int8* TandemDataBuffer )
{
    UFC::AnsiString tempStr;
    Int8 *CurPosition = TandemDataBuffer;
    std::memcpy(CurPosition, FTradeDate.c_str(), 8);  // 1.ODK-DATE 9(8) 0

    CurPosition += 8;
    tempStr.Printf("%010d", FSequenceNo);
    std::memcpy(CurPosition, tempStr.c_str(), 10);  // 2.ODK-SEQNUM 9(10) 8
    
    CurPosition += 10;
    std::memcpy(CurPosition, FReceiveDate.c_str(), 8);  // 3.ODK-MSG-DATE 9(8) 18
    
    CurPosition += 8;
    std::memcpy(CurPosition, FReceiveTime.c_str(), 6);  // 4.ODK-MSG-TIME 9(6) 26
    
    CurPosition += 6;
    std::memcpy(CurPosition, FReceiveMilliSecond.c_str(), 4);  // 5.ODK-MSG-MIL 9(4) 326
    
    CurPosition += 4;
    std::memcpy(CurPosition, FMessageType.c_str(), 3);  // 6.ODK-MSG-TYPE X(3) 36
    
    CurPosition += 3;
    std::memcpy(CurPosition, FLinkBrokerNo.c_str(), 5);  // 7.ODK-LNK-BROKER-NO X(5) 39
    
    CurPosition += 5;
    std::memcpy(CurPosition, FSessionID.c_str(), 5);  // 8.ODK-SESSION-ID 9(5) 44
    
    CurPosition += 5;
    std::memcpy(CurPosition, FStatusCode.c_str(), 3);  // 9.ODK-STATUS-CODE X(3) 49

    CurPosition += 3;
    *CurPosition = FExecType;  //10.ODK-EXEC-TYPE X(1) 52
    
    CurPosition += 1;
    std::memcpy(CurPosition, FClearMemberIDNo.c_str(), 5);  //11.ODK-CLEAR-MEMBER-ID-NO X(5) 53
    
    CurPosition += 5;
    std::memcpy(CurPosition, FBrokerIDNo.c_str(), 5);  //12.ODK-BROKER-ID-NO X(5) 58
    
    CurPosition += 5;
    std::memcpy(CurPosition, FOrderNo.c_str(), 5);  //13.ODK-ORDER-NO X(5) 63
    
    CurPosition += 5;
    std::memcpy(CurPosition, FOrderID.c_str(), 7);  //14.ODK-ORD-ID 9(7) 68
    
    CurPosition += 7;
    std::memcpy(CurPosition, FUserDefine.c_str(), 8);  //15.ODK-USER-DEFINE X(8) 75
    
    CurPosition += 8;
    *CurPosition = FSymbolType;  //16.ODK-SYMBOL-TYPE X(1) 83
    
    CurPosition += 1;
    std::memcpy(CurPosition, FSymbol.c_str(), 20);  //17.ODK-TR-ID X(20) 84
    
    CurPosition += 20;
    tempStr.Printf("%+09d", FOrderPrice);  
    std::memcpy(CurPosition, tempStr.c_str(), 9);  //18.ODK-ORDER-PRRICE S9(8) 104
    
    CurPosition += 9;
    tempStr.Printf("%04d", FOrderQty);  
    std::memcpy(CurPosition, tempStr.c_str(), 4);  //19.ODK-ORDER-QTY 9(4) 113
    
    CurPosition += 4;
    std::memcpy(CurPosition,FInvestorAccountNo.c_str(), 7);  //20.ODK-INVESTOR-ACNO X(7) 117
    
    CurPosition += 7;
    *CurPosition = FInvestorFlag;  //21.ODK-INVESTOR-FLAG X(1) 124
    
    CurPosition += 1;
    *CurPosition = FSide;  //22.ODK-BUY-SELL-KIND X(1) 125
    
    CurPosition += 1;
    *CurPosition = FOrderType;  //23.ODK-ORDER-TYPE X(1) 126
    
    CurPosition += 1;
    *CurPosition = FTimeInForce;  //24.ODK-ORDER-COND X(1) 127
    
    CurPosition += 1;
    *CurPosition = FPositionEffect;  //25.ODK-OPEN-OFFSET-KIND X(1) 128
    
    CurPosition += 1;
    tempStr.Printf("%+010d", FLastPrice);  
    std::memcpy(CurPosition, tempStr.c_str(), 10);  //26.ODK-LAST-MATCH-PRICE S9(9) 129
    
    CurPosition += 10;
    tempStr.Printf("%04d", FLastQty);  
    std::memcpy(CurPosition, tempStr.c_str(), 4);  //27.ODK-LAST-MATCH-QTY 9(4) 139
    
    CurPosition += 4;
    tempStr.Printf("%+017lld", FPriceSubTotal);  
    std::memcpy(CurPosition, tempStr.c_str(), 17);  //28.ODK-TOT-MATCH-PRICE S9(16) 143
    
    CurPosition += 17;
    tempStr.Printf("%04d", FCumQty);  
    std::memcpy(CurPosition, tempStr.c_str(), 4);  //29.ODK-TOT-MATCH-QTY 9(4) 160
    
    CurPosition += 4;
    tempStr.Printf("%04d", FLeavesQty);  
    std::memcpy(CurPosition, tempStr.c_str(), 4);  //30.ODK-LEAVES-QTY 9(4) 164
    
    CurPosition += 4;
    tempStr.Printf("%04d", FBeforeQty);  
    std::memcpy(CurPosition, tempStr.c_str(), 4);  //31.ODK-BEFORE-QTY 9(4) 168
    
    CurPosition += 4;
    *CurPosition = FLegSide1; //32.ODK-BUY-SELL-KIND1 X(1) 172
    
    CurPosition += 1;
    *CurPosition = FLegSide2;  //33.ODK-BUY-SELL-KIND2 X(1) 173
    
    CurPosition += 1;
    tempStr.Printf("%09d", FLegPrice1);  
    std::memcpy(CurPosition, tempStr.c_str(), 9);  //34.ODK-MATCH-PRICE1 9(9) 174
    
    CurPosition += 9;
    tempStr.Printf("%09d", FLegPrice2);  
    std::memcpy(CurPosition, tempStr.c_str(), 9);  //35.ODK-MATCH-PRICE2 9(9) 183
    
    CurPosition += 9;
    tempStr.Printf("%04d", FLegQty1);  
    std::memcpy(CurPosition, tempStr.c_str(), 4);  //36.ODK-MATCH-QTY1 9(4) 192
    
    CurPosition += 4;
    tempStr.Printf("%04d", FLegQty2);  
    std::memcpy(CurPosition, tempStr.c_str(), 4);  //37.ODK-MATCH-QTY2 9(4) 196
    
    CurPosition += 4;
    std::memcpy(CurPosition, FOrderDate.c_str(), 8);  //38.ODK-ORDER-DATE 9(8) 200
    
    CurPosition += 8;
    std::memcpy(CurPosition, FOrderTime.c_str(), 6);  //39.ODK-ORDER-TIME 9(6) 208
    
    CurPosition += 6;
    std::memcpy(CurPosition, FOrderMilliSecond.c_str(), 4);  //40.ODK-ORDER-MIL 9(4) 214
    
    CurPosition += 4;
    std::memcpy(CurPosition, FTransactDate.c_str(), 8);  //41.ODK-LAST-DATE 9(8) 218
    
    CurPosition += 8;
    std::memcpy(CurPosition, FTransactTime.c_str(), 6);  //42.ODK-LAST-TIME 9(6) 226
    
    CurPosition += 6;
    std::memcpy(CurPosition, FTransactMilliSecond.c_str(), 4);  //43.ODK-LAST-MIL 9(4) 232
    
    CurPosition += 4;
    *CurPosition = FTargetID;  //44.ODK-TARGET-ID X(1) 236
    
    CurPosition += 1;
    tempStr.Printf("%010d", FUniqueID);  
    std::memcpy(CurPosition, tempStr.c_str(), 10);  //45.ODK-UNIQUE-ID 9(10) 237
    
    CurPosition += 10;
    tempStr.Printf("%010d", FReportSequenceNo);  
    std::memcpy(CurPosition, tempStr.c_str(), 10);  //46.ODK-SEQNO 9(10) 247
    
    CurPosition += 10;
    *CurPosition = FProtocolType;  //47.ODK-PROTOCOL-TYPE X(1) 257
    
    CurPosition += 1;
    std::memcpy(CurPosition, FBrokerID.c_str(), 7);  //48.ODK-BROKER-ID X(7) 258
    
    CurPosition += 7;
    std::memcpy(CurPosition, FCommodityID1.c_str(), 7);  //49.ODK-COMMODITY-ID1-1 X(7) 265
    
    CurPosition += 7;
    std::memcpy(CurPosition, FSettlementMonth1.c_str(), 6);  //50.ODK-SETTLEMENT-MONTH1-1 9(6) 272
    
    CurPosition += 6;
    std::memcpy(CurPosition, FStrikePrice1.c_str(), 9);  //51.ODK-STRIKE-PRICE1-1 9(6)V9(3) 278
    
    CurPosition += 9;
    tempStr.Printf("%012lld", FPrice1);  
    std::memcpy(CurPosition, tempStr.c_str(), 12);  //52.ODK-PRICE1-1 9(6)V9(6) 287
    
    CurPosition += 12;
    std::memcpy(CurPosition, FCommodityID2.c_str(), 7);  //53.ODK-COMMODITY-ID2-2 X(7) 299
    
    CurPosition += 7;
    std::memcpy(CurPosition, FSettlementMonth2.c_str(), 6);  //54.ODK-SETTLEMENT-MONTH2-2 9(6) 306
    
    CurPosition += 6;
    std::memcpy(CurPosition, FStrikePrice2.c_str(), 9);  //55.ODK-STRIKE-PRICE2-2 9(6)V9(3) 312
    
    CurPosition += 9;
    tempStr.Printf("%012lld", FPrice2);  
    std::memcpy(CurPosition, tempStr.c_str(), 12);  //56.ODK-PRICE2-2 9(6)V9(6) 321
    
    CurPosition += 12;
    tempStr.Printf("%+013lld", FPriceDifference);  
    std::memcpy(CurPosition, tempStr.c_str(), 13);  //57.ODK-PRICE2-D S9(6)V9(6) 333
    
    CurPosition += 13;
    std::memcpy(CurPosition, FAEID.c_str(), 4);  //58.ODK-AE-ID 9(4) 346
    
    CurPosition += 4;
    *CurPosition = FDayTradeID;  //59.ODK-DAY-TRADE-ID X(1) 350
    
    CurPosition += 1;
    std::memcpy(CurPosition, FIBNO.c_str(), 3);  //60.ODK-IB-NO X(03) 351
    
    CurPosition += 3;
    *CurPosition = FDosInKind;  //61.ODK-DOS-IN-KIND X(01) 354
    
    CurPosition += 1;
    std::memcpy(CurPosition, FSubAccountNo.c_str(), 7);  //62.ODK-SUB-MOTHER-ACNO X(7) 355
    
    CurPosition += 7;
    tempStr.Printf("%+010lld", FOdrPrice);  
    std::memcpy(CurPosition, tempStr.c_str(), 10);  //63.ODK-ODR-PRRICE S9(05)V9(4) 362
    
    CurPosition += 10;
    std::memcpy(CurPosition, FClearMemberID.c_str(), 3);  //64.ODK-CLEAR-MEMBER-ID X(04) 371
    
    CurPosition += 3;
    std::memcpy(CurPosition, FInputSeqNo.c_str(), 14);  //65.ODK-INPUT-SEQNO S9(13) 375
    
    CurPosition += 14;
    std::memcpy(CurPosition, FInputKey.c_str(), 14);  //66.ODK-INPUT-KEY S9(13) 389
    
    CurPosition += 14;
    *CurPosition = FAp2InKind;            //67.ODK-AP2-IN-KIND X(01) 403
    
    CurPosition += 1;
    *CurPosition = FAp2InKindN;           //68.ODK-AP2-IN-KIND-N       X(01)       404
    
    CurPosition += 1;
    *CurPosition = FPriceFlag;            //69.ODK-PRICE-FLAG          X(01)       405
    
    CurPosition += 1;
    *CurPosition = FMarketMark;           //70.ODK-MARKET-MARK         X(01)       406    
    
    CurPosition += 1;
    *CurPosition = FSessionMark;          //71.ODK-ETS-FLAG            X(01)       407
    
    CurPosition += 1;
    *CurPosition = FFLEXFlag;             //72.ODK-FLEXFLAG            X(01)       407
    
    CurPosition += 1;
    std::memcpy(CurPosition, FPartID.c_str(), 3 );//73.ODK-PART-ID             9(03)       408
    
    CurPosition += 3;
    std::memcpy(CurPosition, FStatusCodeA.c_str(), 2);//74.ODK-STATUS-CODE-A       9(02)       411    
    
    CurPosition += 2;        
    std::memcpy(CurPosition, FFiller.c_str(), 6);  //75.FILLER X(16) 404
    
    CurPosition += 6;
    std::memcpy(CurPosition, FSourceIP.c_str(), 20);  //76.ODK-SRC-IP X(20) 420    
    
    CurPosition += 20;
    *CurPosition = 0;                                ///<  EOL 440
}  //CTandemMessage::GenerateTandemData()
//------------------------------------------------------------------------------
char TandemTAIFEXData::TranslateExecType(nsOrderMessageDefine::ExecTypeEnum rptExecType, 
                                       nsOrderMessageDefine::TMPExecTypeEnum rptTMPExecType,
                                       nsOrderMessageDefine::CxlRejResponseToEnum rptRejResponseTo)
{
    char tandemExecType = ' ';
    switch(rptExecType)
    {
        case nsOrderMessageDefine::etNew:  //2
            tandemExecType = '0';
            break;
        case nsOrderMessageDefine::etReplaced:  //4
            if (rptTMPExecType == nsOrderMessageDefine::tetPxReplaced)  //M
                tandemExecType = 'M';
            else if (rptTMPExecType == nsOrderMessageDefine::tetPxReplaced2)  //m
                tandemExecType = 'm';
            else
                tandemExecType = '5';
            break;
        case nsOrderMessageDefine::etCanceled:  // 6
            tandemExecType = '4';
            break;
        case nsOrderMessageDefine::etPartiallyFilled:  //7
        case nsOrderMessageDefine::etFilled:           //8
            if (rptTMPExecType == nsOrderMessageDefine::tetNewAndFilled)  //6
                tandemExecType = '6';
            else
                tandemExecType = 'F';
            break;
        case nsOrderMessageDefine::etOrderStatus:  //12
            tandemExecType = 'I';
            break;
        case nsOrderMessageDefine::etRejected:  //11
            switch(rptTMPExecType)
            {
                case nsOrderMessageDefine::tetNew:      tandemExecType = '0'; break;
                case nsOrderMessageDefine::tetCanceled: tandemExecType = '4'; break;
                case nsOrderMessageDefine::tetReplaced: tandemExecType = '5'; break;
                default : 
                    switch(rptRejResponseTo)
                    {
                        case nsOrderMessageDefine::crrReplace:     tandemExecType = '5'; break;
                        case nsOrderMessageDefine::crrCancel:      tandemExecType = '4'; break;
                        case nsOrderMessageDefine::crrOrderStatus: tandemExecType = 'I'; break;
                        case nsOrderMessageDefine::crrNew:
                        default:                                   tandemExecType = '0';
                    }
            }
            break;
        case nsOrderMessageDefine::etQuoteAccept:  //9
            tandemExecType = '0';
            break;
        default:
            tandemExecType = ' ';
    }  //switch(rptExecType)
    return tandemExecType;
}  //CTandemMessage::TranslateExecType()

//------------------------------------------------------------------------------
char TandemTAIFEXData::TranslateSide(nsOrderMessageDefine::SideEnum rptSide, char& BuySellCode)
{
    char tandemSide = ' ';
    if (rptSide == nsOrderMessageDefine::sBuy)
    {
        tandemSide = '1';
        BuySellCode = 'B';
    }
    else if (rptSide == nsOrderMessageDefine::sSell)
    {
        tandemSide = '2';
        BuySellCode = 'S';
    }
    else
        BuySellCode = ' ';
        
    return tandemSide;
}  //CTandemMessage::TranslateSide()

//------------------------------------------------------------------------------
char TandemTAIFEXData::TranslateOrderType(nsOrderMessageDefine::OrderTypeEnum rptOrderType)
{
    char tandemOrderType = ' ';
    if (rptOrderType == nsOrderMessageDefine::otMarket)
        tandemOrderType = '1';
    else if (rptOrderType == nsOrderMessageDefine::otLimit)
        tandemOrderType = '2';
    return tandemOrderType;
}  //CTandemMessage::TranslateOrderType()

//------------------------------------------------------------------------------
char TandemTAIFEXData::TranslateTimeInForce(nsOrderMessageDefine::TimeInForceEnum rptTimeInForce)
{
    char tandemTimeInForce;
    switch(rptTimeInForce)
    {
        case nsOrderMessageDefine::tifROD:  tandemTimeInForce = '0'; break;
        case nsOrderMessageDefine::tifIOC:  tandemTimeInForce = '3'; break;
        case nsOrderMessageDefine::tifFOK:  tandemTimeInForce = '4'; break;
        case nsOrderMessageDefine::tifTFXQ: tandemTimeInForce = '8'; break;
        default:                            tandemTimeInForce = ' '; 
    }
    return tandemTimeInForce;
}  //CTandemMessage::TranslateTimeInForce()

//------------------------------------------------------------------------------
char TandemTAIFEXData::TranslatePositionEffect(nsOrderMessageDefine::PositionEffectEnum rptPositionEffect)
{
    char tandemPositionEffect;
    switch(rptPositionEffect)
    {
        case nsOrderMessageDefine::peOpen:           tandemPositionEffect = 'O'; break;
        case nsOrderMessageDefine::peClose:          tandemPositionEffect = 'C'; break;
        case nsOrderMessageDefine::peDayTrade:       tandemPositionEffect = 'D'; break;
        case nsOrderMessageDefine::peTMPMarketMaker: tandemPositionEffect = '9'; break;
        case nsOrderMessageDefine::peTAIFEXAuto:     tandemPositionEffect = 'A'; break;
        default:                                     tandemPositionEffect = ' '; 
    }
    return tandemPositionEffect;
}  //CTandemMessage::TranslateTimeInForce()

//------------------------------------------------------------------------------
UFC::AnsiString TandemTAIFEXData::ConvertToCommodityID(const UFC::AnsiString& newSymbol, nsOrderMessageDefine::MarketEnum rptMarket, BOOL IsFLEX, char callPutCode)
{    
    UFC::AnsiString commodityID( "" );
    
    if( IsFLEX == FALSE )
    {
        UFC::AnsiString newSymbolRoot = newSymbol.SubString(0, 3);
        UFC::AnsiString oldSymbolRoot = CTandemMessage::FindOldSymbolRootByNew(newSymbolRoot, true);
        
        if (oldSymbolRoot.Length() <= 0)
        {   //Find no Old SymbolRoot
            if (rptMarket == nsOrderMessageDefine::mTWFutures)
                commodityID.Printf("FI%s", newSymbolRoot.c_str());
            else
                commodityID = newSymbolRoot;
        }
        else
            commodityID = oldSymbolRoot;
    }
    else
    {   ///< MXFFX or TXOOX       
        commodityID = newSymbol.SubString( 0, 5 );
    }
    commodityID.PadThis( 7, ' ');
    if( rptMarket == nsOrderMessageDefine::mTWOptions) 
        commodityID[6] = callPutCode;
    return commodityID;        
}
//------------------------------------------------------------------------------
};
//------------------------------------------------------------------------------