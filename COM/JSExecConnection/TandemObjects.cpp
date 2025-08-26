/* 
 * File:   TandemObjects.cpp
 * Author: zhenfan
 *
 * Created on 2012/6/22 16:42
 */
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "TandemObjects.h"
#include "iniFile.h"
#include "TMPMessage.h"      //TAIFEXLib
#include "MultilegSymbol.h"  //TaifexLib

namespace jihsun
{
//------------------------------------------------------------------------------
const char *DescribeExecRptMarket(nsOrderMessageDefine::MarketEnum execRptMarket)
{
    switch (execRptMarket)
    {
        case nsOrderMessageDefine::mTWFutures:   return "TAIFEX Futures";  //0, ///< TAIFEX Taiwan Futures
        case nsOrderMessageDefine::mTWOptions:   return "TAIFEX Options";  //1, ///< TAIFEX Taiwan Options
        case nsOrderMessageDefine::mTSE:         return "TSEC";            //2, ///< TSEC   Taiwan equity
        case nsOrderMessageDefine::mOTC:         return "OTC";             //3, ///< OTC    Taiwan
        case nsOrderMessageDefine::mForeignFutures: return "PATS Futures";    //4, ///< PATS          Futures
        case nsOrderMessageDefine::mForeignOptions: return "PATS Options";    //5, ///< PATS          Options
        case nsOrderMessageDefine::mCNFutures:   return "SGCN Futures";    //6  ///< SGCN   China  Futures ( Via SUNGARD system)
        default:                                 return "Unknown";
    }  //switch (execRptOrderStatus)
}  //DescribeExecRptMarket()
    
//------------------------------------------------------------------------------
const char *DescribeExecRptOrderStatus(nsOrderMessageDefine::OrderStatusEnum execRptOrderStatus)
{
    switch (execRptOrderStatus)
    {
        case nsOrderMessageDefine::osNone:               return "None";                // 0
        case nsOrderMessageDefine::osPendingNew:         return "PendingNew";          // 1
        case nsOrderMessageDefine::osNew:                return "New";                 // 2
        case nsOrderMessageDefine::osPendingReplace:     return "PendingReplace";      // 3
        case nsOrderMessageDefine::osReplaced:           return "Replaced";            // 4
        case nsOrderMessageDefine::osPendingCancel:      return "PendingCancel";       // 5
        case nsOrderMessageDefine::osCanceled:           return "Canceled";            // 6
        case nsOrderMessageDefine::osPartiallyFilled:    return "PartiallyFilled";     // 7
        case nsOrderMessageDefine::osFilled:             return "Filled";              // 8
        case nsOrderMessageDefine::osQuoteAccept:        return "QuoteAccept";         // 9
        case nsOrderMessageDefine::osExpired:            return "Expired";             //10
        case nsOrderMessageDefine::osRejected:           return "Rejected";            //11
        case nsOrderMessageDefine::osQuoteRequestAccept: return "QuoteRequestAccept";  //12
        default:                                         return "Unknown";
    }  //switch (execRptOrderStatus)
}  //DescribeExecRptOrderStatus()
    
//--------------------------------- CUserData ----------------------------------
UFC::AnsiString CUserData::FFieldDelimiter = "^";
UFC::AnsiString CUserData::FNameValueDelimiter = "=";
UFC::AnsiString CUserData::FLocalIP = UFC::PSocket::GetLocalIP();
//------------------------------------------------------------------------------
CUserData::CUserData()
:FInternetMark("0"),
 FSubAccountNo(""),   //62.ODK-SUB-MOTHER-ACNO X(7)   355
 FAEID(""),           //58.ODK-AE-ID           9(4)   346
 FAp2InKind("SP"),    //67.ODK-AP2-IN-KIND     X(01)  403
 FSourceIP(FLocalIP), //66.ODK-SRC-IP          X(20)  420
 FOpenQty(0),         //68.ODK-OPEN-QTY            9(4)        418
 FPremium(0),         //69.ODK-TD-PP-PREMIUM       S9(6)V9(2)  422
 FFee(0)              //70.ODK-TD-PP-FEE           S9(6)V9(2)  431
{
    FSourceIP.PadThis(20, ' ');
} 
//------------------------------------------------------------------------------
CUserData::CUserData(const UFC::AnsiString& csvStr)
:FInternetMark("0"),
 FSubAccountNo(""),  //62.ODK-SUB-MOTHER-ACNO X(7)  355
 FAEID(""),          //58.ODK-AE-ID           9(4)  346
 FAp2InKind("SO"),   //67.ODK-AP2-IN-KIND     X(01) 403
 FSourceIP(""),       //69.ODK-SRC-IP          X(20) 420
 FOpenQty(0),         //68.ODK-OPEN-QTY            9(4)        418
 FPremium(0),         //69.ODK-TD-PP-PREMIUM       S9(6)V9(2)  422
 FFee(0)              //70.ODK-TD-PP-FEE           S9(6)V9(2)  431
{    
    ParseFieldValue( csvStr );
    if (FSourceIP.Length() <= 0) 
    {
        FSourceIP = FLocalIP;
        FSourceIP.PadThis(20, ' ');
    }
}  
//------------------------------------------------------------------------------
void CUserData::ParseFieldValue(const UFC::AnsiString& csvStr)
{
    UFC::NameValueMessage Msg( FFieldDelimiter, FNameValueDelimiter );
    
    Msg.FromString( csvStr );
    
    if( Msg.Get( "SUBACNO", FSubAccountNo ) == FALSE )
        Msg.Get( "subacno", FSubAccountNo );
    
    if( Msg.Get( "AEID", FAEID ) == FALSE )
        Msg.Get( "aeid", FAEID );
    
    if( Msg.Get( "IP", FSourceIP ) == FALSE )
        Msg.Get( "ip", FSourceIP );    
    
    if( Msg.Get( "APKIND", FAp2InKind ) == FALSE )
        Msg.Get( "apkind", FAp2InKind );        
    
    if( Msg.Get( "IM", FInternetMark ) == FALSE )
        Msg.Get( "im", FInternetMark );            
    
    UFC::AnsiString Value;
     
    if( Msg.Get( "OQ", Value ) == TRUE )    
        FOpenQty = Value.ToInt();
    else if( Msg.Get( "OPENQTY", Value ) == TRUE )    
        FOpenQty = Value.ToInt();
    
    if( Msg.Get( "PP", Value ) == TRUE )    
        FPremium = Value.ToDouble();
    else if( Msg.Get( "PREMIUM", Value ) == TRUE )    
        FPremium = Value.ToDouble();
    
    if( Msg.Get( "PF", Value ) == TRUE )    
        FFee = Value.ToDouble();
    else if( Msg.Get( "FEE", Value ) == TRUE )    
        FFee = Value.ToDouble();
    
    FAp2InKind.PadThis( 2, ' ');
}
//------------------------------- CTandemMessage -------------------------------
UFC::PCriticalSection CTandemMessage::FSequenceNoLocker;
Int32                 CTandemMessage::FCurrentSequenceNo = 0;

UFC::PCriticalSection CTandemMessage::FSymbolRootLocker;
CTandemMessage::FStringStringMap CTandemMessage::FNewSymbolRootIndex;
//------------------------------------------------------------------------------
Int32 CTandemMessage::GetCurrentSequenceNo(bool DoLock)
{
    if (DoLock) FSequenceNoLocker.Acquire();
    Int32 CurrentSequenceNo = FCurrentSequenceNo;
    if (DoLock) FSequenceNoLocker.Release();
    return CurrentSequenceNo;
}  //CTandemMessage::GetCurrentSequenceNo)

//------------------------------------------------------------------------------
void CTandemMessage::SetCurrentSequenceNo(Int32 SequenceNo, bool DoLock)
{
    if (DoLock) FSequenceNoLocker.Acquire();
    FCurrentSequenceNo = SequenceNo;
    if (DoLock) FSequenceNoLocker.Release();
}  //CTandemMessage::SetCurrentSequenceNo()

//------------------------------------------------------------------------------
Int32 CTandemMessage::GetNextSequenceNo(bool DoLock)
{
    if (DoLock) FSequenceNoLocker.Acquire();
    if (FCurrentSequenceNo >= 999999999) FCurrentSequenceNo = 0;
    Int32 nextSequenceNo = ++FCurrentSequenceNo;
    if (DoLock) FSequenceNoLocker.Release();
    return nextSequenceNo;
}  //CTandemMessage::GetNextSequenceNo()

//------------------------------------------------------------------------------
Int32 CTandemMessage::GetNewSymbolRootCount(bool DoLock)
{
    if (DoLock) FSymbolRootLocker.Acquire();
    Int32 NewSymbolRootCount = FNewSymbolRootIndex.size();
    if (DoLock) FSymbolRootLocker.Release();
    return NewSymbolRootCount;
}  //CTandemMessage::GetNewSymbolRootCount()

//------------------------------------------------------------------------------
UFC::AnsiString CTandemMessage::FindOldSymbolRootByNew(const UFC::AnsiString& NewSymbolRoot, bool DoLock)
{
    UFC::AnsiString OldSymbolRoot = "";
    if (NewSymbolRoot.Length() <= 0) return OldSymbolRoot;
    if (DoLock) FSymbolRootLocker.Acquire();
    FStringStringMap::iterator it = FNewSymbolRootIndex.find(NewSymbolRoot);
    if (it != FNewSymbolRootIndex.end()) OldSymbolRoot = it->second;
    if (DoLock) FSymbolRootLocker.Release();
    return OldSymbolRoot;
}  //CTandemMessage::FindOldSymbolRootByNew()

//------------------------------------------------------------------------------
bool CTandemMessage::InsertNewSymbolRootIndex(const UFC::AnsiString& NewSymbolRoot, const UFC::AnsiString& OldSymbolRoot, bool DoCheck, bool DoLock)
{
    bool IsSuccess = false;
    if ((NewSymbolRoot.Length() <= 0) || (OldSymbolRoot.Length() <= 0)) return IsSuccess;

    if (DoLock) FSymbolRootLocker.Acquire();
    UFC::AnsiString ExistOldSymbolRoot = "";
    if (DoCheck) ExistOldSymbolRoot = FindOldSymbolRootByNew(NewSymbolRoot, false);
    if (ExistOldSymbolRoot.Length() == 0)
    {
        std::pair<FStringStringMap::iterator, bool> InsertPair;
        InsertPair = FNewSymbolRootIndex.insert(FStringStringMap::value_type(NewSymbolRoot, OldSymbolRoot));
        IsSuccess = InsertPair.second;
    }
    if (DoLock) FSymbolRootLocker.Release();
    return IsSuccess;
}  //CTandemMessage::InsertNewSymbolRootIndex()

//-----------------------------------------------------------------------------
void CTandemMessage::DumpNewSymbolRootIndex(Int32 DebugLevel, bool DoLock)
{
    FStringStringMap::iterator it;
    if (DoLock) FSymbolRootLocker.Acquire();
    UFC::BufferedLog::DebugPrintf(DebugLevel, " ===== %u Symbol Root ======", FNewSymbolRootIndex.size());
    for (it = FNewSymbolRootIndex.begin(); it != FNewSymbolRootIndex.end(); it++)
    {
        UFC::AnsiString NewSymbolRoot = it->first;
        UFC::AnsiString OldSymbolRoot = it->second;
        UFC::BufferedLog::DebugPrintf(DebugLevel, " %s=%s", NewSymbolRoot.c_str(), OldSymbolRoot.c_str());
    }
    if (DoLock) FSymbolRootLocker.Release();
}  //CTandemMessage::DumpNewSymbolRootIndex()

//-----------------------------------------------------------------------------
Int32 CTandemMessage::LoadNewSymbolRootIndex(const UFC::AnsiString& ConfigFileName, bool DoLock)
{
    Int32 IndexCount = 0;
    if (ConfigFileName.Length() <= 0) return IndexCount;
    UFC::UiniFile *ConfigFilePtr = 0;
    try 
    {
        ConfigFilePtr = new UFC::UiniFile(ConfigFileName);
    }
    catch (UFC::FileException fe)
    {
        UFC::BufferedLog::Printf(" CTandemMessage::LoadNewSymbolRootIndex() [%s] Error: %s", ConfigFileName.c_str(), fe.what());
        ConfigFilePtr = NULL;
    }

    if (ConfigFilePtr != NULL)
    {
        UFC::Section *CurSectionPtr = ConfigFilePtr->GetSection("Convert");
        if (CurSectionPtr != NULL)
        {
            if (DoLock) FSymbolRootLocker.Acquire();
            for (Int32 i = 0; i < CurSectionPtr->ItemCount(); i++)
            {
                UFC::AnsiString NewSymbolRoot = "";
                UFC::AnsiString OldSymbolRoot = "";
                CurSectionPtr->GetNameValue(i, NewSymbolRoot, OldSymbolRoot);
                if (InsertNewSymbolRootIndex(NewSymbolRoot, OldSymbolRoot, true, false))
                    IndexCount++;
                else
                    UFC::BufferedLog::Printf(" CTandemMessage::LoadNewSymbolRootIndex() Insert New Symbol Root[%s] Index Error: Old Symbol Root[%s]", NewSymbolRoot.c_str(), OldSymbolRoot.c_str());
            }  //for (Int32 i = 0; i < CurSectionPtr->ItemCount(); i++)
            if (DoLock) FSymbolRootLocker.Release();
        }  //if (CurSectionPtr !=NULL)
        delete ConfigFilePtr;
    }  //if (ConfigFilePtr != NULL)
    return IndexCount;
}  //CTandemMessage::LoadNewSymbolRootIndex()

//-----------------------------------------------------------------------------
void CTandemMessage::InitField()
{
    FExecRptMarket      = nsOrderMessageDefine::mCNFutures;
    FExecRptOrderStatus = nsOrderMessageDefine::osNone; 
    FExecID             = "";
    
    FTradeDate           = "        ";                        // 1.ODK-DATE                9(8)          0
    FSequenceNo          = 0;                                 // 2.ODK-SEQNUM              9(10)         8
    FReceiveDate         = "        ";                        // 3.ODK-MSG-DATE            9(8)         18
    FReceiveTime         = "      ";                          // 4.ODK-MSG-TIME            9(6)         26
    FReceiveMilliSecond  = "    ";                            // 5.ODK-MSG-MIL             9(4)         32
    FMessageType         = "   ";                             // 6.ODK-MSG-TYPE            X(3)         36
    FLinkBrokerNo        = "     ";                           // 7.ODK-LNK-BROKER-NO       X(5)         39
    FSessionID           = "     ";                           // 8.ODK-SESSION-ID          9(5)         44
    FStatusCode          = "   ";                             // 9.ODK-STATUS-CODE         X(3)         49
    FExecType            = ' ';                               //10.ODK-EXEC-TYPE           X(1)         52
    FClearMemberIDNo     = "     ";                           //11.ODK-CLEAR-MEMBER-ID-NO  X(5)         53
    FBrokerIDNo          = "     ";                           //12.ODK-BROKER-ID-NO        X(5)         58
    FOrderNo             = "     ";                           //13.ODK-ORDER-NO            X(5)         63
    FOrderID             = "       ";                         //14.ODK-ORD-ID              9(7)         68
    FUserDefine          = "        ";                        //15.ODK-USER-DEFINE         X(8)         75
    FSymbolType          = ' ';                               //16.ODK-SYMBOL-TYPE         X(1)         83
    FSymbol              = "                    ";            //17.ODK-TR-ID               X(20)        84
    FOrderPrice          = 0;                                 //18.ODK-ORDER-PRRICE        S9(8)       104
    FOrderQty            = 0;                                 //19.ODK-ORDER-QTY           9(4)        113
    FInvestorAccountNo   = "       ";                         //20.ODK-INVESTOR-ACNO       X(7)        117
    FInvestorFlag        = ' ';                               //21.ODK-INVESTOR-FLAG       X(1)        124
    FSide                = ' ';                               //22.ODK-BUY-SELL-KIND       X(1)        125
    FOrderType           = ' ';                               //23.ODK-ORDER-TYPE          X(1)        126
    FTimeInForce         = ' ';                               //24.ODK-ORDER-COND          X(1)        127
    FPositionEffect      = ' ';                               //25.ODK-OPEN-OFFSET-KIND    X(1)        128
    FLastPrice           = 0;                                 //26.ODK-LAST-MATCH-PRICE    S9(9)       129
    FLastQty             = 0;                                 //27.ODK-LAST-MATCH-QTY      9(4)        139
    FPriceSubTotal       = 0;                                 //28.ODK-TOT-MATCH-PRICE     S9(16)      143
    FCumQty              = 0;                                 //29.ODK-TOT-MATCH-QTY       9(4)        160
    FLeavesQty           = 0;                                 //30.ODK-LEAVES-QTY          9(4)        164
    FBeforeQty           = 0;                                 //31.ODK-BEFORE-QTY          9(4)        168
    FLegSide1            = '0';                               //32.ODK-BUY-SELL-KIND1      X(1)        172
    FLegSide2            = '0';                               //33.ODK-BUY-SELL-KIND2      X(1)        173
    FLegPrice1           = 0;                                 //34.ODK-MATCH-PRICE1        9(9)        174
    FLegPrice2           = 0;                                 //35.ODK-MATCH-PRICE2        9(9)        183
    FLegQty1             = 0;                                 //36.ODK-MATCH-QTY1          9(4)        192
    FLegQty2             = 0;                                 //37.ODK-MATCH-QTY2          9(4)        196
    FOrderDate           = "        ";                        //38.ODK-ORDER-DATE          9(8)        200
    FOrderTime           = "      ";                          //39.ODK-ORDER-TIME          9(6)        208
    FOrderMilliSecond    = "    ";                            //40.ODK-ORDER-MIL           9(4)        214
    FTransactDate        = "        ";                        //41.ODK-LAST-DATE           9(8)        218
    FTransactTime        = "      ";                          //42.ODK-LAST-TIME           9(6)        226
    FTransactMilliSecond = "    ";                            //43.ODK-LAST-MIL            9(4)        232
    FTargetID            = ' ';                               //44.ODK-TARGET-ID           X(1)        236
    FUniqueID            = 0;                                 //45.ODK-UNIQUE-ID           9(10)       237
    FReportSequenceNo    = 0;;                                //46.ODK-SEQNO               9(10)       247
    FProtocolType        = ' ';                               //47.ODK-PROTOCOL-TYPE       X(1)        257
    FBrokerID            = "       ";                         //48.ODK-BROKER-ID           X(7)        258
    FCommodityID1        = "       ";                         //49.ODK-COMMODITY-ID1-1     X(7)        265
    FSettlementMonth1    = "      ";                          //50.ODK-SETTLEMENT-MONTH1-1 9(6)        272
    FStrikePrice1        = "         ";                       //51.ODK-STRIKE-PRICE1-1     9(6)V9(3)   278
    FPrice1              = 0;                                 //52.ODK-PRICE1-1            9(6)V9(6)   287
    FCommodityID2        = "       ";                         //53.ODK-COMMODITY-ID2-2     X(7)        299
    FSettlementMonth2    = "      ";                          //54.ODK-SETTLEMENT-MONTH2-2 9(6)        306
    FStrikePrice2        = "         ";                       //55.ODK-STRIKE-PRICE2-2     9(6)V9(3)   312
    FPrice2              = 0;                                 //56.ODK-PRICE2-2            9(6)V9(6)   321
    FPriceDifference     = 0;                                 //57.ODK-PRICE2-D            S9(6)V9(6)  333
    FAEID                = "    ";                            //58.ODK-AE-ID               9(4)        346
    FDayTradeID          = ' ';                               //59.ODK-DAY-TRADE-ID        X(1)        350
    FIBNO                = "   ";                             //60.ODK-IB-NO               X(03)       351
    FInternetMark        = '0';                               //61.ODK-INTERNET-MARK       X(1)        354
    FDCBCode             = "        ";                        //62.ODK-DCBCODE             X(8)        355
    FDealer              = "       ";                         //63.ODK-DEALER              X(7)        363
    FOdrPrice            = 0;                                 //64.ODK-ODR-PRRICE          S9(05)V9(3) 370
    FClearMemberID       = "    ";                            //65.ODK-CLEAR-MEMBER-ID     X(04)       379
    FIPNo                = "                              ";  //66.ODK-IP-NO               X(30)       383
    FAPKind              = "SP";                              //67.ODK-AP-KIND             X(2)        416
    FOpenQty             = 0;                                 //68.ODK-OPEN-QTY            9(4)        418
    FPremium             = 0;                                 //69.ODK-TD-PP-PREMIUM       S9(6)V9(2)  422
    FFee                 = 0;                                 //70.ODK-TD-PP-FEE           S9(6)V9(2)  431
    FOrderKind           = ' ';                               //71.ODK-ORDER-KIND          X(1)        440     
    FFiller              = "                                       ";//72.FILLER           X(39)       441
}  //CTandemMessage::InitField()

//-----------------------------------------------------------------------------
CTandemMessage::CTandemMessage()
{
    InitField();
}  //CTandemMessage::CTandemMessage()

//------------------------------------------------------------------------------
CTandemMessage::CTandemMessage(TExecutionReportMessage *ExecRptPtr)
{
    InitField();
    SetFieldsValue(ExecRptPtr);
    GenerateTandemData();
}  //CTandemMessage::CTandemMessage()
//------------------------------------------------------------------------------
char CTandemMessage::TranslateExecType(TExecutionReportMessage *ExecRptPtr,
                                       nsOrderMessageDefine::ExecTypeEnum rptExecType, 
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
            if( ExecRptPtr->GetTMPStstusCode() == 48 )
            {
                tandemExecType = 'F';  //for Hua Nan
            }
            else if( ExecRptPtr->GetTMPStstusCode() == 47 )
            {
                tandemExecType = '6';  //for Hua Nan 
            }            
            else if( (ExecRptPtr->GetTimeInForce() == nsOrderMessageDefine::tifFOK ||
                      ExecRptPtr->GetTimeInForce() == nsOrderMessageDefine::tifIOC )
                      && ExecRptPtr->GetCumQty() == 0 )
            {
                if (rptTMPExecType == nsOrderMessageDefine::tetCanceled)  //4
                    tandemExecType = '4';  ///< IOC canceled order before opend
                else if (rptTMPExecType == nsOrderMessageDefine::tetFilled )
                {
                    ExecRptPtr->SetOrderQty( 0 );
                    tandemExecType = 'F';  ///< IOC canceled order after opend
                }
                else
                    tandemExecType = '6';  //new and fill                
            }
            else
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
char CTandemMessage::TranslateSide(nsOrderMessageDefine::SideEnum rptSide, char& BuySellCode)
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
char CTandemMessage::TranslateOrderType(nsOrderMessageDefine::OrderTypeEnum rptOrderType)
{
    char tandemOrderType = ' ';
    if (rptOrderType == nsOrderMessageDefine::otMarket)
        tandemOrderType = '1';
    else if (rptOrderType == nsOrderMessageDefine::otLimit)
        tandemOrderType = '2';
    else if (rptOrderType == nsOrderMessageDefine::otMarketWithProtection)
        tandemOrderType = '3';
    return tandemOrderType;
}  //CTandemMessage::TranslateOrderType()

//------------------------------------------------------------------------------
char CTandemMessage::TranslateTimeInForce(nsOrderMessageDefine::TimeInForceEnum rptTimeInForce)
{
    char tandemTimeInForce;
    switch(rptTimeInForce)
    {
        case nsOrderMessageDefine::tifROD:  tandemTimeInForce = '0'; break;
        case nsOrderMessageDefine::tifIOC:  tandemTimeInForce = '3'; break;
        case nsOrderMessageDefine::tifFOK:  tandemTimeInForce = '4'; break;
        case nsOrderMessageDefine::tifTFXQ: tandemTimeInForce = '8'; break;
        default:                            tandemTimeInForce = '0'; break;
    }
    return tandemTimeInForce;
}  //CTandemMessage::TranslateTimeInForce()

//------------------------------------------------------------------------------
char CTandemMessage::TranslatePositionEffect(nsOrderMessageDefine::PositionEffectEnum rptPositionEffect)
{
    char tandemPositionEffect;
    switch(rptPositionEffect)
    {
        case nsOrderMessageDefine::peOpen:           tandemPositionEffect = 'O'; break;
        case nsOrderMessageDefine::peClose:          tandemPositionEffect = 'C'; break;
        case nsOrderMessageDefine::peRolled:         tandemPositionEffect = 'D'; break;
        case nsOrderMessageDefine::peTMPMarketMaker: tandemPositionEffect = '9'; break;
        case nsOrderMessageDefine::peTAIFEXAuto:     tandemPositionEffect = 'A'; break;
        default:                                     tandemPositionEffect = 'O'; break;
    }
    return tandemPositionEffect;
}  //CTandemMessage::TranslateTimeInForce()

//------------------------------------------------------------------------------
UFC::AnsiString CTandemMessage::ConvertToCommodityID(const UFC::AnsiString& newSymbol, nsOrderMessageDefine::MarketEnum rptMarket, char callPutCode)
{
    UFC::AnsiString commodityID = "";
    UFC::AnsiString newSymbolRoot = newSymbol.SubString(0, 3);
    UFC::AnsiString oldSymbolRoot = FindOldSymbolRootByNew(newSymbolRoot, true);
    if (oldSymbolRoot.Length() <= 0)
    {   //Find no Old SymbolRoot
        if (rptMarket == nsOrderMessageDefine::mTWFutures)
            commodityID.Printf("FI%s", newSymbolRoot.c_str());
        else
            commodityID = newSymbolRoot;
    }
    else
        commodityID = oldSymbolRoot;
    commodityID.PadThis(7, ' ');
    if (rptMarket == nsOrderMessageDefine::mTWOptions) commodityID[6] = callPutCode;
    return commodityID;
}  //CTandemMessage::commodityID()

//------------------------------------------------------------------------------
Int64 CTandemMessage::DoubleToInt64(double DoubleVal, int Digi)
{
    double precision = std::pow(0.1, (double)(Digi + 1));
    double multiple = std::pow(10.0, (double)Digi);

    if (DoubleVal >= 0.0)
        return (Int64)((DoubleVal + precision) * multiple);
    else
        return (Int64)((DoubleVal - precision) * multiple);
}  //CTandemMessage::DoubleToInt64()

//------------------------------------------------------------------------------
void CTandemMessage::SetFieldsValue(TExecutionReportMessage *ExecRptPtr)
{
    FExecRptMarket      = ExecRptPtr->GetMarket();
    FExecRptOrderStatus = ExecRptPtr->GetOrderStatus(); 
    FExecID             = ExecRptPtr->GetExecID();
    
    nsOrderMessageDefine::ExecTypeEnum rptExecType = ExecRptPtr->GetExecType();
    nsOrderMessageDefine::TMPExecTypeEnum rptTMPExecType = ExecRptPtr->GetTMPExecType();
    nsOrderMessageDefine::CxlRejResponseToEnum rptRejResponseTo = ExecRptPtr->GetCxlRejResponseTo();
    UFC::UDateTime Now;
    FTradeDate.Printf("%04d%02d%02d", Now.getYear(), Now.getMonth(), Now.getDate());       // 1.ODK-DATE     9(8)   0
    FSequenceNo = GetNextSequenceNo(true);                                                 // 2.ODK-SEQNUM   9(10)  8
    FReceiveDate.Printf("%04d%02d%02d", Now.getYear(), Now.getMonth(), Now.getDate());     // 3.ODK-MSG-DATE 9(8)  18
    FReceiveTime.Printf("%02d%02d%02d", Now.getHour(), Now.getMinute(), Now.getSecond());  // 4.ODK-MSG-TIME 9(6)  26
    FReceiveMilliSecond.Printf("%04d", Now.getMillisecond());                              // 5.ODK-MSG-MIL  9(4)  32

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

    long PriceDigit = ExecRptPtr->GetPxDigit();
    FStatusCode.Printf("%03d", ExecRptPtr->GetTMPStstusCode());  // 9.ODK-STATUS-CODE X(3) 49
    FExecType = TranslateExecType(ExecRptPtr, rptExecType, rptTMPExecType, rptRejResponseTo);  //10.ODK-EXEC-TYPE X(1) 52
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " OrderStatus[%d] rptExecType[%d] TMPExecType[%c] rptRejResponseTo[%d] => ExecType[%c] PriceDigit[%ld]", FExecRptOrderStatus, rptExecType, rptTMPExecType, rptRejResponseTo, FExecType, PriceDigit);
    
    int clearMemberIndex = TMP::TMPMessage::GetCMID(ExecRptPtr->GetCMID(),ExecRptPtr->GetBrokerID());
    FClearMemberIDNo.Printf("%05d", clearMemberIndex);  //11.ODK-CLEAR-MEMBER-ID-NO X(5) 53
    
    int BrokerIndex = TMP::TMPMessage::GetBrokerID(ExecRptPtr->GetBrokerID());
    FBrokerIDNo.Printf("%05d", BrokerIndex );  //12.ODK-BROKER-ID-NO X(5) 58
    
    FOrderNo.Printf("%s", ExecRptPtr->GetOrderID());  //13.ODK-ORDER-NO X(5) 63
    FOrderNo.PadThis(5,' ');
    
    FOrderID = "0000000";  //14.ODK-ORD-ID 9(7) 68
    
    UFC::AnsiString rptData = ExecRptPtr->GetData();  //UserData 8-15 X(8)
    rptData.PadThis(15, ' ');
    FUserDefine = "        ";           //15.ODK-USER-DEFINE X(8) 75    
    FSymbolType = '2';                  //16.ODK-SYMBOL-TYPE X(1) 83   
    FSymbol = ExecRptPtr->GetSymbol();  //17.ODK-TR-ID X(20) 84
    FSymbol.PadThis(20, ' ');
    FOrderPrice = UFC::DoubleToInt(ExecRptPtr->GetLastPx(), PriceDigit);  //18.ODK-ORDER-PRRICE  S9(8) 104
    FOrderQty = ExecRptPtr->GetTMPQty();                                  //19.ODK-ORDER-QTY     9(4)  113
    FInvestorAccountNo = ExecRptPtr->GetAccount();                        //20.ODK-INVESTOR-ACNO X(7)  117
    FInvestorAccountNo.PadThis(7, ' ');
    
    //21.ODK-INVESTOR-FLAG X(1) 124
    if (rptExecType == nsOrderMessageDefine::etQuoteAccept )
        FInvestorFlag = '8';
    else
        FInvestorFlag = *(ExecRptPtr->GetAccountFlag());

    char buySellCode;
    FSide = TranslateSide(ExecRptPtr->GetSide(), buySellCode);                   //22.ODK-BUY-SELL-KIND    X(1) 125
    FOrderType = TranslateOrderType(ExecRptPtr->GetOrderType());                 //23.ODK-ORDER-TYPE       X(1) 126
    FTimeInForce = TranslateTimeInForce(ExecRptPtr->GetTimeInForce());           //24.ODK-ORDER-COND       X(1) 127
    FPositionEffect = TranslatePositionEffect(ExecRptPtr->GetPositionEffect());  //25.ODK-OPEN-OFFSET-KIND X(1) 128
    
    if ((FExecType == '4') || (FExecType == '5') || (FExecType == 'M') || (FExecType == 'm'))
    {  //Canceled             Reduce Qty            Replace Price         Replace Price
        FLastPrice = 0;  //26.ODK-LAST-MATCH-PRICE S9(9) 129
        FLastQty   = 0;  //27.ODK-LAST-MATCH-QTY   9(4)  139
    }
    else
    {
        FLastPrice = UFC::DoubleToInt(ExecRptPtr->GetPrice(), PriceDigit);  //26.ODK-LAST-MATCH-PRICE S9(9) 129
        FLastQty   = ExecRptPtr->GetOrderQty();                             //27.ODK-LAST-MATCH-QTY   9(4)  139
    }
    if( (FExecType == '6') && (ExecRptPtr->GetCumQty() == 0) &&
        (ExecRptPtr->GetTimeInForce() == nsOrderMessageDefine::tifFOK ||
         ExecRptPtr->GetTimeInForce() == nsOrderMessageDefine::tifIOC ) )
    {  //< FOK /IOC canceled
        FLastPrice = 0;  //26.ODK-LAST-MATCH-PRICE S9(9) 129
        FLastQty   = 0;  //27.ODK-LAST-MATCH-QTY 9(4) 139
    }                
    if ((FExecRptOrderStatus == nsOrderMessageDefine::osReplaced) ||
        (FExecRptOrderStatus == nsOrderMessageDefine::osCanceled) ||
        (FExecType == '0'))  //New
    {
        FLastPrice = 0;
        FLastQty = 0;
    }
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " Px[%16.6lf] Leg1Px[%16.6lf] Leg2Px[%16.6lf] LastPx[%16.6lf]", ExecRptPtr->GetPrice(), ExecRptPtr->GetLegPrice1(), ExecRptPtr->GetLegPrice2(), ExecRptPtr->GetLastPx());
    FPriceSubTotal = (Int64)ExecRptPtr->GetTMPPxSubTotal();                 //28.ODK-TOT-MATCH-PRICE S9(16) 143
    FCumQty = ExecRptPtr->GetCumQty();                                      //29.ODK-TOT-MATCH-QTY   9(4)   160
    FLeavesQty = ExecRptPtr->GetLeavesQty();                                //30.ODK-LEAVES-QTY      9(4)   164
    FBeforeQty = ExecRptPtr->GetBeforeQty();                                //31.ODK-BEFORE-QTY      9(4)   168
    FLegPrice1 = UFC::DoubleToInt(ExecRptPtr->GetLegPrice1(), PriceDigit);  //34.ODK-MATCH-PRICE1    9(9)   174
    FLegPrice2 = UFC::DoubleToInt(ExecRptPtr->GetLegPrice2(), PriceDigit);  //35.ODK-MATCH-PRICE2    9(9)   183
    FLegQty1 = ExecRptPtr->GetLegQty1();                                    //36.ODK-MATCH-QTY1      9(4)   192
    FLegQty2 = ExecRptPtr->GetLegQty2();                                    //37.ODK-MATCH-QTY2      9(4)   196
    FOrderDate = FTradeDate;                                                //38.ODK-ORDER-DATE      9(8)   200
    
    //39.ODK-ORDER-TIME 9(6) 208
    UFC::AnsiString OrgTransactTimeStr(ExecRptPtr->GetOrgTransactTime());
    OrgTransactTimeStr.PadThis(9, '0');
    int OrgTransactTime = OrgTransactTimeStr.ToInt();
    FOrderTime.Printf("%02d%02d%02d", OrgTransactTime / 10000000,
                      (OrgTransactTime % 10000000) / 100000, (OrgTransactTime % 100000) / 1000);
    FOrderMilliSecond.Printf("%04d", OrgTransactTime % 1000);  //40.ODK-ORDER-MIL 9(4) 214
    FTransactDate = FTradeDate;                                //41.ODK-LAST-DATE 9(8) 218
    
    //42.ODK-LAST-TIME 9(6) 226
    UFC::AnsiString TransactTimeStr(ExecRptPtr->GetTransactTime());
    TransactTimeStr.PadThis(9, '0');
    int TransactTime = TransactTimeStr.ToInt();
    FTransactTime.Printf("%02d%02d%02d", TransactTime / 10000000,
                         (TransactTime % 10000000) / 100000, (TransactTime % 100000) / 1000);
    FTransactMilliSecond.Printf("%04d", TransactTime % 1000);  //43.ODK-LAST-MIL      9(4)  214
    FTargetID = '4';                                           //44.ODK-TARGET-ID     X(1)  236
    FUniqueID = ExecRptPtr->GetTMPUniqueID();                  //45.ODK-UNIQUE-ID     9(10) 237
    FReportSequenceNo = ExecRptPtr->GetReportSequence();       //46.ODK-SEQNO         9(10) 247
    FProtocolType = '1';                                       //47.ODK-PROTOCOL-TYPE X(1)  257
    FBrokerID = ExecRptPtr->GetBrokerID();                     //48.ODK-BROKER-ID     X(7)  258
    FBrokerID.PadThis(7, ' ');
    
    BO_SymbolDeCompose symbolDeCompose(ExecRptPtr->GetSymbol(), buySellCode);
    if (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stUnKnown) //De compose failed
    {
        FLegSide1 = '0';               //32.ODK-BUY-SELL-KIND1      X(1)       172
        FLegSide2 = '0';               //33.ODK-BUY-SELL-KIND2      X(1)       173
        FLegPrice1 = 0;                //34.ODK-MATCH-PRICE1        9(9)       174
        FLegPrice2 = 0;                //35.ODK-MATCH-PRICE2        9(9)       183
        FLegQty1 = 0;                  //36.ODK-MATCH-QTY1          9(4)       192
        FLegQty2 = 0;                  //37.ODK-MATCH-QTY2          9(4)       196
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
        FCommodityID1 = ConvertToCommodityID(OrigSymbol1, FExecRptMarket, symbolDeCompose.FCP_Code1);  //49.ODK-COMMODITY-ID1-1 X(7) 265
        FSettlementMonth1 = symbolDeCompose.FSettleMonth1;  //50.ODK-SETTLEMENT-MONTH1-1 9(6) 272
        FSettlementMonth1.PadThis(6, '0');
        FStrikePrice1 = symbolDeCompose.FExcercisePrice1;   //51.ODK-STRIKE-PRICE1-1 9(6)V9(3) 278
        FStrikePrice1.PadThis(9, '0');
        if ((symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle) || 
            (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesSingle))
        {   // It's a single order.
            FLegSide1 = '0';               //32.ODK-BUY-SELL-KIND1 X(1)            172
            FLegSide2 = '0';               //33.ODK-BUY-SELL-KIND2 X(1)            173
            FLegPrice1 = 0;                //34.ODK-MATCH-PRICE1        9(9)       174
            FLegPrice2 = 0;                //35.ODK-MATCH-PRICE2        9(9)       183
            FLegQty1 = 0;                  //36.ODK-MATCH-QTY1          9(4)       192
            FLegQty2 = 0;                  //37.ODK-MATCH-QTY2          9(4)       196
            FCommodityID2 = "       ";     //53.ODK-COMMODITY-ID2-2     X(7)       299
            FSettlementMonth2 = "000000";  //54.ODK-SETTLEMENT-MONTH2-2 9(6)       306
            FStrikePrice2 = "000000000";   //55.ODK-STRIKE-PRICE2-2     9(6)V9(3)  312
            
            if ((FExecType == 'F') || (FExecType == '6'))  //F: Fill; 6:New & Fill
            {
                if( ExecRptPtr->GetCumQty() == 0 &&
                    (ExecRptPtr->GetTimeInForce() == nsOrderMessageDefine::tifFOK ||
                     ExecRptPtr->GetTimeInForce() == nsOrderMessageDefine::tifIOC ) )
                    FPrice1 = 0;
                else     
                    FPrice1 = DoubleToInt64(ExecRptPtr->GetPrice(), 6);  //52.ODK-PRICE1-1 9(6)V9(6) 287
            }
            else
                FPrice1 = 0;
            FPrice2 = 0;           //56.ODK-PRICE2-2 9(6)V9(6)  321
            FPriceDifference = 0;  //57.ODK-PRICE2-D S9(6)V9(6) 333
        }
        else
        {
            UFC::AnsiString OrigSymbol2 = symbolDeCompose.FComm2;
            FCommodityID2 = ConvertToCommodityID(OrigSymbol2, FExecRptMarket, symbolDeCompose.FCP_Code2);  //53.ODK-COMMODITY-ID2-2 X(7) 299
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
                if( ExecRptPtr->GetCumQty() == 0 &&
                    (ExecRptPtr->GetTimeInForce() == nsOrderMessageDefine::tifFOK ||
                     ExecRptPtr->GetTimeInForce() == nsOrderMessageDefine::tifIOC ) )
                {
                    FPrice1 = 0;
                    FPrice2 = 0;
                    FPriceDifference = 0;
                }
                else
                {
                    FPrice1 = DoubleToInt64(ExecRptPtr->GetLegPrice1(), 6);       //52.ODK-PRICE1-1 9(6)V9(6)  287
                    FPrice2 = DoubleToInt64(ExecRptPtr->GetLegPrice2(), 6);       //56.ODK-PRICE2-2 9(6)V9(6)  321
                    FPriceDifference = DoubleToInt64(ExecRptPtr->GetPrice(), 6);  //57.ODK-PRICE2-D S9(6)V9(6) 333
                }
            }
            else
            {
                FPrice1 = 0;           //52.ODK-PRICE1-1 9(6)V9(6)  287
                FPrice2 = 0;           //56.ODK-PRICE2-2 9(6)V9(6)  321
                FPriceDifference = 0;  //57.ODK-PRICE2-D S9(6)V9(6) 333
            }
        }  //if ((symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle) ||
    }  //if (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stUnKnown)
    
    UFC::AnsiString userData = ExecRptPtr->GetData();
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " UserData[%s]", userData.c_str());
    CUserData *userDataPtr = new CUserData(userData);    

    UFC::AnsiString userAEID = userDataPtr->GetAEID();
    if (userAEID.Length() < 4) userAEID.PadThis(4, '0', false);  //Pad Left
    FAEID = userAEID;
    
    if (FPositionEffect == 'D') FDayTradeID = 'D';  //59.ODK-DAY-TRADE-ID X(1) 350    
    FIBNO = FBrokerID.SubString(4, 3);              //60.ODK-IB-NO         X(03) 351    
    FInternetMark = userDataPtr->GetInternetMark(); //61.ODK-INTERNET-MARK X(1)  354
    UFC::AnsiString subAccountNo = userDataPtr->GetSubAccountNo();
    
    if (subAccountNo.Length() > 0)
    {
        FDCBCode = "FV      ";    //   ODK-DCBCODE X(8) 355
        FDealer  = subAccountNo;  //62.ODK-DEALER  X(7) 363
    }
    else
    {
        FDCBCode = "        ";
        FDealer  = ExecRptPtr->GetAE();
    }
    FDealer.PadThis(7, '0', FALSE);  //pad Left

    //63.ODK-ODR-PRRICE S9(05)V9(3) 362
    //                  S9(05)V9(6)
    if(FExecRptOrderStatus == nsOrderMessageDefine::osQuoteAccept)
        FOdrPrice = DoubleToInt64( ExecRptPtr->GetPrice(), 6);
    else
        FOdrPrice = DoubleToInt64( ExecRptPtr->GetLastPx(), 6);

    FClearMemberID = ExecRptPtr->GetCMID();  //64.ODK-CLEAR-MEMBER-ID X(04) 371
    FClearMemberID.PadThis(4, '0');
    
    FIPNo = userDataPtr->GetSourceIP();    //69.ODK-SRC-IP X(30) 383    
    FIPNo.PadThis(30, ' ');    
    FAPKind = userDataPtr->GetAp2InKind();
    
    FOpenQty   = userDataPtr->GetOpenQty(); //68.ODK-OPEN-QTY            9(4)        418
    FPremium   = UFC::DoubleToInt( userDataPtr->GetPremium(), 2);//69.ODK-TD-PP-PREMIUM       S9(6)V9(2)  422
    FFee       = UFC::DoubleToInt( userDataPtr->GetFee(), 2);    //70.ODK-TD-PP-FEE           S9(6)V9(2)  431
    if( ExecRptPtr->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
        FOrderKind = '1'; //71.ODK-ORDER-KIND          X(1)        440         
    else
        FOrderKind = ' ';
    FFiller    = "                                       ";  //68.FILLER X(39) 404
    delete userDataPtr;
    if( ExecRptPtr->GetTMPStstusCode() == 48 )
    {
        FLastPrice = 0;
        FPrice1 = 0;
    }
    if( ExecRptPtr->GetTMPStstusCode() == 47 &&  
        FExecType == '6' && 
        ExecRptPtr->GetCumQty() == 0 )
    {  //< FOK /IOC canceled
        FLastPrice = 0;  //26.ODK-LAST-MATCH-PRICE S9(9) 129
        FLastQty   = 0;
        FPrice1    = 0;        
    }
    UFC::BufferedLog::FlushToFile();
}  //CTandemMessage::SetFieldsValue()

//------------------------------------------------------------------------------
void CTandemMessage::GenerateTandemData()
{
    UFC::AnsiString tempStr;
    Int8 *CurPosition = FTandemDataBuffer;
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
    *CurPosition = FInternetMark;  //61.ODK-INTERNET-MARK X(1) 354
    
    CurPosition += 1;
    std::memcpy(CurPosition, FDCBCode.c_str(), 8);  //   ODK-DCBCODE X(8) 355
    
    CurPosition += 8;
    std::memcpy(CurPosition, FDealer.c_str(), 7);  //62.ODK-DEALER X(7) 363
    
    CurPosition += 7;
    tempStr.Printf("%+012lld", FOdrPrice);  
    std::memcpy(CurPosition, tempStr.c_str(), 12);  //63.ODK-ODR-PRRICE S9(05)V9(3) 362
                                                   //                  S9(05)V9(6)             
    CurPosition += 12;
    std::memcpy(CurPosition, FClearMemberID.c_str(), 4);  //64.ODK-CLEAR-MEMBER-ID X(04) 371
    
    CurPosition += 4;
    std::memcpy(CurPosition, FIPNo.c_str(), 30);  //   ODK-IP-NO X(30) 383    
    CurPosition += 30;
    
    std::memcpy(CurPosition, FAPKind.c_str(), 2);  // AP_KIND X(2)    
    CurPosition += 2;
    
    tempStr.Printf("%04d", FOpenQty );  
    std::memcpy(CurPosition, tempStr.c_str(), 4 );//68.ODK-OPEN-QTY            9(4)        418
    CurPosition += 4;    
    
    tempStr.Printf("%+09d", FPremium );  
    std::memcpy(CurPosition, tempStr.c_str(), 9 );//69.ODK-TD-PP-PREMIUM       S9(6)V9(2)  422
    CurPosition += 9;
    
    tempStr.Printf("%+09d", FFee );  
    std::memcpy(CurPosition, tempStr.c_str(), 9 );//70.ODK-TD-PP-FEE           S9(6)V9(2)  431
    CurPosition += 9;

    *CurPosition = FOrderKind; //71.ODK-ORDER-KIND          X(1)        440             
    CurPosition += 1;
    
    std::memcpy(CurPosition, FFiller.c_str(), 39 );  //72.FILLER X(39) 441    
    CurPosition += 39;
    *CurPosition = 0;
}  //CTandemMessage::GenerateTandemData()

//------------------------------ CTandemTelegraph ------------------------------
Int32 CTandemTelegraph::FMaxTandemReportCount = 3;

//------------------------------------------------------------------------------
CTandemTelegraph::CTandemTelegraph(Int16 TelegraphLength, const UFC::AnsiString& TelegraphBody)
:FCommand(""),
 FNumber(0),
 FData(""),
 FTelegraphLength(TelegraphLength),
 FTelegraphBufferSize(0),
 FTelegraphBuffer(0)
{
    ParseTelegraph(TelegraphLength, TelegraphBody);
}  //CTandemTelegraph::CTandemTelegraph()

//------------------------------------------------------------------------------
CTandemTelegraph::CTandemTelegraph(const UFC::AnsiString& Command, Int32 Number, const UFC::AnsiString& Data)
:FCommand(Command),
 FNumber(Number),
 FData(Data),
 FTelegraphLength(0),
 FTelegraphBufferSize(0),
 FTelegraphBuffer(0)        
{
    BuildTelegraph(Command, Number, Data);
}  //CTandemTelegraph::CTandemTelegraph()

//------------------------------------------------------------------------------
CTandemTelegraph::~CTandemTelegraph()
{
    if (FTelegraphBuffer != 0) delete [] FTelegraphBuffer;
}  //CTandemTelegraph::~CTandemTelegraph()

//------------------------------------------------------------------------------
void CTandemTelegraph::ParseTelegraph(Int16 TelegraphLength, const UFC::AnsiString& TelegraphBody)
{
    if ((TelegraphLength < 8) || (TelegraphLength != TelegraphBody.Length())) return;
    if ((TelegraphBody.AnsiPos("ACK") == 0) ||
        (TelegraphBody.AnsiPos("CNT") == 0) ||   //Hand Shake
        (TelegraphBody.AnsiPos("CFM") == 0) ||   //Heartbeat
        (TelegraphBody.AnsiPos("END") == 0))
    {
        FCommand = TelegraphBody.SubString(0, 3);
        FNumber  = TelegraphBody.SubString(3, 10).ToInt();
    }
    else if (TelegraphBody.AnsiPos("RS") == 0)
    {
        FCommand = TelegraphBody.SubString(0, 2);
        FNumber  = TelegraphBody.SubString(2, 6).ToInt();
    }
    else if ((TelegraphBody.AnsiPos("ADD") == 0) || (TelegraphBody.AnsiPos("UPD") == 0))
    {
        FCommand = TelegraphBody.SubString(0, 3);
        FNumber  = TelegraphBody.SubString(3, 10).ToInt();
        if (TelegraphLength > 13)
            FData = TelegraphBody.SubString(13, TelegraphLength - 13);
    }
}  //CTandemTelegraph::ParseTelegraph()

//------------------------------------------------------------------------------
void CTandemTelegraph::BuildTelegraph( const UFC::AnsiString& Command, Int32 Number, const UFC::AnsiString& Data )
{
    UFC::AnsiString telegraphBody = "";
    if (Command.Length() == 3)
    {
        if (((Command == "ADD") || (Command == "UPD")) && (Data.Length() > 0))
            telegraphBody.Printf("%s%010d%s", Command.c_str(), Number, Data.c_str());
        else
            telegraphBody.Printf("%s%010d", Command.c_str(), Number);
    }
    else if (Command.Length() == 2)
        telegraphBody.Printf("%s%06d", Command.c_str(), Number);

    FTelegraphLength = telegraphBody.Length();
    if (FTelegraphLength <= 0) return;
    
    FTelegraphBufferSize = FTelegraphLength + sizeof(unsigned short) + 1;
    FTelegraphBuffer = new char[FTelegraphBufferSize];
    unsigned short localTempData = static_cast<unsigned short>(FTelegraphLength);
    unsigned short networkTempData;
    networkTempData = htons(localTempData);
    std::memcpy(FTelegraphBuffer, &networkTempData, sizeof(unsigned short));
    std::memcpy(FTelegraphBuffer + sizeof(unsigned short), telegraphBody.c_str(), FTelegraphLength);
    FTelegraphBuffer[FTelegraphBufferSize - 1] = 0;
}  //CTandemTelegraph::BuildTelegraph()

//----------------------------- CDataExchangePool ------------------------------
//------------------------------------------------------------------------------
CDataExchangePool::CDataExchangePool()
{
}  //CDataExchangePool::CDataExchangePool()

//------------------------------------------------------------------------------
CDataExchangePool::~CDataExchangePool()
{
    ClearTandemMsgQueue(true);
    ClearAlertMsgQueue(true);
    ClearNoticeMsgQueue(true);
}  //CDataExchangePool::~CDataExchangePool()

//------------------------------------------------------------------------------
Int32 CDataExchangePool::GetWaitingTandemMsgCount(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    Int32 MsgCount = FTandemMsgQueue.size();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return MsgCount;
}  //CDataExchangePool::GetWaitingTandemMsgCount()

//------------------------------------------------------------------------------
CTandemMessage *CDataExchangePool::PopTandemMsg(bool DoLock)
{
    CTandemMessage *TandemMsgPtr = 0;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    if (FTandemMsgQueue.size() > 0)
    {
        TandemMsgPtr = FTandemMsgQueue.front();
        FTandemMsgQueue.pop();
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return TandemMsgPtr;    
}  //CDataExchangePool::PopTandemMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::PushTandemMsg(CTandemMessage *TandemMsgPtr, bool DoLock)
{
    if (TandemMsgPtr == 0) return;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    FTandemMsgQueue.push(TandemMsgPtr);
    if (DoLock) FTandemMsgQueueLocker.Signal();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::PushTandemMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::ClearTandemMsgQueue(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    while(FTandemMsgQueue.size() > 0)
    {
        CTandemMessage *TandemMsgPtr = FTandemMsgQueue.front();
        FTandemMsgQueue.pop();
        delete TandemMsgPtr;
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::ClearTandemMsgQueue()

//------------------------------------------------------------------------------
Int32 CDataExchangePool::GetWaitingAlertMsgCount(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    Int32 AlertCount = FAlertMsgQueue.size();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return AlertCount;
}  //CDataExchangePool::GetWaitingAlertMsgCount()

//------------------------------------------------------------------------------
UFC::AnsiString *CDataExchangePool::PopAlertMsg(bool DoLock)
{
    UFC::AnsiString *AlertMsgPtr = 0;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    if (FAlertMsgQueue.size() > 0)
    {
        AlertMsgPtr = FAlertMsgQueue.front();
        FAlertMsgQueue.pop();
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return AlertMsgPtr;    
}  //CDataExchangePool::PopAlertMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::PushAlertMsg(UFC::AnsiString *AlertMsgPtr, bool DoLock)
{
    if (AlertMsgPtr == 0) return;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    FAlertMsgQueue.push(AlertMsgPtr);
    if (DoLock) FTandemMsgQueueLocker.Signal();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::PushAlertMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::ClearAlertMsgQueue(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    while(FAlertMsgQueue.size() > 0)
    {
        UFC::AnsiString *AlertMsgPtr = FAlertMsgQueue.front();
        FAlertMsgQueue.pop();
        delete AlertMsgPtr;
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::ClearAlertMsgQueue()

//------------------------------------------------------------------------------
Int32 CDataExchangePool::GetWaitingNoticeMsgCount(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    Int32 NoticeCount = FNoticeMsgQueue.size();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return NoticeCount;
}  //CDataExchangePool::GetWaitingNoticeMsgCount()

//------------------------------------------------------------------------------
UFC::AnsiString *CDataExchangePool::PopNoticeMsg(bool DoLock)
{
    UFC::AnsiString *NoticeMsgPtr = 0;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    if (FNoticeMsgQueue.size() > 0)
    {
        NoticeMsgPtr = FNoticeMsgQueue.front();
        FNoticeMsgQueue.pop();
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return NoticeMsgPtr;    
}  //CDataExchangePool::PopNoticeMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::PushNoticeMsg(UFC::AnsiString *NoticeMsgPtr, bool DoLock)
{
    if (NoticeMsgPtr == 0) return;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    FNoticeMsgQueue.push(NoticeMsgPtr);
    if (DoLock) FTandemMsgQueueLocker.Signal();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::PushNoticeMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::ClearNoticeMsgQueue(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    while(FNoticeMsgQueue.size() > 0)
    {
        UFC::AnsiString *NoticeMsgPtr = FNoticeMsgQueue.front();
        FNoticeMsgQueue.pop();
        delete NoticeMsgPtr;
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::ClearAlertMsgQueue()

//------------------------------------------------------------------------------
Int32 CDataExchangePool::GetTandemMsgArrayCount(bool DoLock) 
{   
    if (DoLock) FTandemMsgArrayLocker.Acquire();
    Int32 tandemMsgCount = FTandemMsgArray.size();    
    if (DoLock) FTandemMsgArrayLocker.Release();
    return tandemMsgCount;
}  //CDataExchangePool::GetTandemMsgArrayCount()

//------------------------------------------------------------------------------
bool CDataExchangePool::InsertTandemMsgArray(CTandemMessage *TandemMsgPtr, bool DoLock)
{
    Int32 targetIndex = TandemMsgPtr->GetSequenceNo() - 1;
    if (targetIndex < 0) return false;
    if (DoLock) FTandemMsgArrayLocker.Acquire();
    Int32 maxIndex = FTandemMsgArray.size() - 1;
    while (maxIndex < targetIndex)
    {
        FTandemMsgArray.push_back(0);
        maxIndex++;
    }  //while (maxIndex < targetIndex)
    FTandemMsgArray[targetIndex] = TandemMsgPtr;
    if (DoLock) FTandemMsgArrayLocker.Release();
    return true;
}  //CDataExchangePool::InsertTandemMsg()

//------------------------------------------------------------------------------
CTandemMessage *CDataExchangePool::GetTandemMsgFromArray(Int32 MsgIndex, bool DoLock)
{
    CTandemMessage *tandemMsgPtr = 0;
    if (DoLock) FTandemMsgArrayLocker.Acquire();
    if ((MsgIndex >= 0) || (MsgIndex < static_cast<Int32>(FTandemMsgArray.size())))
    {
        tandemMsgPtr = FTandemMsgArray[MsgIndex];
    }
    if (DoLock) FTandemMsgArrayLocker.Release(); 
    return tandemMsgPtr;
}  //CDataExchangePool::GetTandemMsgFromArray()

//------------------------------------------------------------------------------
int CDataExchangePool::GetTandemMsgExecIDCount(bool DoLock)
{
    if (DoLock) FTandemMsgExecIDLocker.Acquire();
    int execCount = FTandemMsgExecIDIndex.size();
    if (DoLock) FTandemMsgExecIDLocker.Release();
    return execCount;
}  //CDataExchangePool::GetTandemMsgExecIDCount()

//------------------------------------------------------------------------------
CTandemMessage *CDataExchangePool::FindTandemMsgByExecID(const UFC::AnsiString& ExecID, bool DoLock)
{
    CTandemMessage *msgPtr = 0;
    if (DoLock) FTandemMsgExecIDLocker.Acquire();
    std::map<UFC::AnsiString, CTandemMessage*>::iterator it = FTandemMsgExecIDIndex.find(ExecID);
    if (it != FTandemMsgExecIDIndex.end()) msgPtr = it->second;
    if (DoLock) FTandemMsgExecIDLocker.Release();
    return msgPtr;
}  //CDataExchangePool::FindTandemMsgByExecID()

//------------------------------------------------------------------------------
bool CDataExchangePool::IsTandemMsgExecIDExist(const UFC::AnsiString& ExecID, bool DoLock)
{
    CTandemMessage *msgPtr = FindTandemMsgByExecID(ExecID, DoLock);
    if (msgPtr == 0)
        return false;
    else
        return true;        
}  //CDataExchangePool::IsTandemMsgExecIDExist()

//------------------------------------------------------------------------------
bool CDataExchangePool::InsertTandemMsgExecIDIndex(const UFC::AnsiString& ExecID, CTandemMessage *TandemMsgPtr, bool DoCheck, bool DoLock)
{
    bool isSuccess = false;
    if (TandemMsgPtr == 0) return isSuccess;
    if (DoLock) FTandemMsgExecIDLocker.Acquire();
    if (DoCheck && IsTandemMsgExecIDExist(ExecID, false)) 
    {
        if (DoLock) FTandemMsgExecIDLocker.Release();
        return isSuccess;
    }
    std::pair<std::map<UFC::AnsiString, CTandemMessage*>::iterator, bool> InsertPair = 
        FTandemMsgExecIDIndex.insert(std::map<UFC::AnsiString, CTandemMessage*>::value_type(ExecID, TandemMsgPtr));
    isSuccess = InsertPair.second;
    if (DoLock) FTandemMsgExecIDLocker.Release();
    return isSuccess;
}  //CDataExchangePool::InsertTandemMsgExecIDIndex()

//------------------------------------------------------------------------------
bool CDataExchangePool::InsertTandemMsgExecIDIndex(CTandemMessage *TandemMsgPtr, bool DoCheck, bool DoLock)
{
    if (TandemMsgPtr == 0) return false;
    UFC::AnsiString execID = TandemMsgPtr->GetExecID();
    return InsertTandemMsgExecIDIndex(execID, TandemMsgPtr, DoCheck, DoLock);
}  //CDataExchangePool::InsertTandemMsgExecIDIndex()

}  //namespace jihsun
