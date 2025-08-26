/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <math.h>
#include "MLExecConnection.h"
#include "OverseasData.h"
#include "TUserData.h"
//------------------------------------------------------------------------------
extern UFC::PInt32 FOverseasSeqNo;
extern int         NextDayTime;
//------------------------------------------------------------------------------
bool NeedAdd24( int h,int m,int s )
{
    int now = h*10000 + m*100 + s;
    if( now < NextDayTime )
        return true;
    return false;
}
//------------------------------------------------------------------------------
void MakeTimeStr( UFC::AnsiString& TimeStr )
{
    UFC::UDateTime msgRecvTime;
    msgRecvTime.setCurrent();
    
    if( NeedAdd24( msgRecvTime.getHour(), msgRecvTime.getMinute(), msgRecvTime.getSecond() ) == true  )
        TimeStr.Printf("%02d%02d%02d00", msgRecvTime.getHour()+24, msgRecvTime.getMinute(), msgRecvTime.getSecond());
    else
        TimeStr.Printf("%02d%02d%02d00", msgRecvTime.getHour(), msgRecvTime.getMinute(), msgRecvTime.getSecond());
}
//------------------------------------------------------------------------------
bool ParseLegMaturityMonthYear(const UFC::AnsiString& LegMaturityMonthYear, char& Side, UFC::AnsiString& MaturityMonthYear, UFC::AnsiString& StrikePriceStr, char& CallPut)
{
    bool isSuccess = false;
    int legMaturityMonthYearLength = LegMaturityMonthYear.Length();
    if (legMaturityMonthYearLength >= 1) 
    {
        Side = LegMaturityMonthYear[0];
        if (Side == 'b')
            Side = 'B';
        else if (Side == 's')
            Side = 'S';
    
        if ((Side == 'B') || (Side == 'S'))
        {
            if (legMaturityMonthYearLength >= 7) 
            {
                MaturityMonthYear = LegMaturityMonthYear.SubString(1, 6);
                StrikePriceStr = "";
                CallPut = 'N';
            
                if (legMaturityMonthYearLength >= 8)  //Options
                {
                    CallPut = LegMaturityMonthYear[7];
                    if (CallPut == 'c') 
                        CallPut = 'C';
                    else if (CallPut == 'p')
                        CallPut = 'P';
                
                    if ((CallPut == 'C') || (CallPut == 'P'))
                    {
                        if (legMaturityMonthYearLength >= 9)
                        {   //S201710C25000
                            int strikePxStartIndex = 8;
                            int strikePxLength = legMaturityMonthYearLength - strikePxStartIndex;
                            if (strikePxLength > 0)
                            {
                                StrikePriceStr = LegMaturityMonthYear.SubString(strikePxStartIndex, strikePxLength);
                                isSuccess = true;
                            }
                        }
                    }  //if ((CallPut == 'C') || (CallPut == 'P'))
                }
                else  // Futures
                    isSuccess = true;
            }  //if (legMaturityMonthYearLength >= 7)
        }  //if ((Side == 'B') || (Side == 'S'))
    }  //if (legMaturityMonthYearLength >= 1)
    
    return isSuccess;
}  //ParseLegMaturityMonthYear()
//------------------------------------------------------------------------------
UFC::AnsiString MLExecConnection::CheckOverseasTimeStrFormat(const UFC::AnsiString& OverseasTimeStr)
{
    UFC::AnsiString resultStr = "";
    UFC::AnsiString tempStr = OverseasTimeStr;
    
    tempStr.TrimLeft();
    tempStr.TrimRight();
    int tempStrLength = tempStr.Length();
    if( tempStrLength < 4 ) 
    {
        MakeTimeStr( resultStr );
        return resultStr;
    }
    int colonPos1 = -1;
    int colonPos2 = -1;
    int dotPos = -1;
    for( int i = 0; i < tempStrLength; i++ )
    {
        char curChar = tempStr[i];
        if ((curChar >= '0') && (curChar <= '9')) 
            continue;
        else if (curChar == '.')
            dotPos = i;
        else if (curChar == ':')
        {
            if (colonPos1 < 0)
                colonPos1 = i;
            else if ((colonPos1 >= 0) && (colonPos2 < 0))
                colonPos2 = i;
            else
            {
                MakeTimeStr( resultStr );
                return resultStr;
            }
        }
        else
        {
            MakeTimeStr( resultStr );        
            return resultStr;
        }
    }  //for (int i = 0; i < tempStrLength; i++)
   
    UFC::AnsiString hourStr = "";
    UFC::AnsiString minuteStr = "";
    UFC::AnsiString secondStr = "";
    
    if( colonPos1 < 0 )  //no colon exist
    {
        if (tempStrLength >= 2) hourStr = tempStr.SubString(0, 2);
        if (tempStrLength >= 4) minuteStr = tempStr.SubString(2, 2);
        if (tempStrLength >= 6) secondStr = tempStr.SubString(4, 2);
    }
    else
    {
        if (colonPos1 > 0) hourStr = tempStr.SubString(0, colonPos1);
        
        if (colonPos2 < 0)  //just one colon
            minuteStr = tempStr.SubString(colonPos1 + 1, tempStrLength - colonPos1 - 1);
        else if (colonPos2 > colonPos1) 
            minuteStr = tempStr.SubString(colonPos1 + 1, colonPos2 - colonPos1 - 1);
        
        if (colonPos2 > 0)
        {
            if (dotPos < 0)
                secondStr = tempStr.SubString(colonPos2 + 1, tempStrLength - dotPos - 1);
            else if (dotPos > colonPos2)
                secondStr = tempStr.SubString(colonPos2 + 1, dotPos - colonPos2 - 1);
        }
        hourStr.PadThis(2, '0', false);    //pad left
        minuteStr.PadThis(2, '0', false);  //pad left
        secondStr.PadThis(2, '0', false);  //pad left
    }  //if (colonPos1 < 0)
    int H = hourStr.ToInt();
    int M = minuteStr.ToInt();
    int S = secondStr.ToInt();
    
    if( NeedAdd24( H,M,S ) == true )
        resultStr.Printf("%02d%02d%02d00",  H +24 ,M,S );
    else
        resultStr.Printf("%02d%02d%02d00",  H ,M,S );
    
    return resultStr;
}  //MLExecConnection::CheckOverseasTimeStrFormat()
//------------------------------------------------------------------------------
UFC::AnsiString MLExecConnection::ConvertOverseasMaturityMonthYear(const UFC::AnsiString& MonthYearStr)
{
    UFC::AnsiString upperMonthYearStr = MonthYearStr.UpperCase();
    upperMonthYearStr.TrimLeft();
    upperMonthYearStr.TrimRight();
    bool needConvert = false;
    for (int i = 0; i < upperMonthYearStr.Length(); i++)
    {
        char curChar = upperMonthYearStr[i];
        if ((curChar >= 'A') && (curChar <= 'Z')) 
        {
            needConvert = true;
            break;
        }
    }  //for (int i = 0; i < upperMonthYearStr.Length(); i++)
    
    UFC::AnsiString newStr = MonthYearStr;
    if (needConvert)
    {
        UFC::AnsiString monthStr = upperMonthYearStr.SubString(0, 3);
        int monthValue = 0;
        if (monthStr == "JAN")
            monthValue = 1;
        else if (monthStr == "FEB")
            monthValue = 2;
        else if (monthStr == "MAR")
            monthValue = 3;
        else if (monthStr == "APR")
            monthValue = 4;
        else if (monthStr == "MAY")
            monthValue = 5;
        else if (monthStr == "JUN")
            monthValue = 6;
        else if (monthStr == "JUL")
            monthValue = 7;
        else if (monthStr == "AUG")
            monthValue = 8;
        else if (monthStr == "SEP")
            monthValue = 9;
        else if (monthStr == "OCT")
            monthValue = 10;
        else if (monthStr == "NOV")
            monthValue = 11;
        else if (monthStr == "DEC")
            monthValue = 12;
        
        UFC::AnsiString yearStr = upperMonthYearStr.SubString(3, 2);
        newStr.Printf("20%s%02d", yearStr.c_str(), monthValue);
    }
    return newStr;
}  //MLExecConnection::ConvertOverseasMaturityMonthYear()
//------------------------------------------------------------------------------
void MLExecConnection::OverseasConfirmToBackend( TExecutionReportMessage* Msg )
{
    nsOrderMessageDefine::OrderStatusEnum msgOrderStatus = Msg->GetOrderStatus();
    UFC::AnsiString msgUserData = Msg->GetData();
    UFC::BufferedLog::DebugPrintf(" OverseasConfirmToBackend() UserData[%s]", msgUserData.c_str());
    UFC::AnsiString userData = "";
    Int32 proxyNID = 0;
    UFC::AnsiString proxyAE = "";
    int Status = UFC::AnsiString( Msg->GetStatusCode() ).ToInt();
    
    if( Status == 97 ) ///< Skip Tandem Risk Manager reject
    {
        UFC::BufferedLog::DebugPrintf( " OrderID[%s] Status[%s] Skip execution rejected by Tandem.", Msg->GetOrderID(), Msg->GetStatusCode() );
        return;           
    }        

    
    if (!IsProxyUserData(msgUserData, proxyAE, proxyNID, userData)) 
        userData = msgUserData;
    
    TUserData UDD( userData.c_str() );
    UFC::UDateTime msgRecvTime;
    msgRecvTime.setCurrent();
    UFC::AnsiString msgRecvDateStr;
    UFC::GetTradeYYYYMMDD( msgRecvDateStr, false );
    //msgRecvDateStr.Printf("%04d%02d%02d", msgRecvTime.getYear(), msgRecvTime.getMonth(),msgRecvTime.getDate());

    OS_CONFIRM_STRUCT osConfirm;

    //Int8 FUNCTION_CODE[3];      // 1.X(3)        0 201:Confirm  202:Reject
    if( msgOrderStatus == nsOrderMessageDefine::osRejected &&
        (Msg->GetExecType() == nsOrderMessageDefine::etReplaced ||
         Msg->GetExecType() == nsOrderMessageDefine::etCanceled ))
        memcpy(osConfirm.FUNCTION_CODE, "202", 3 ); ///< Canceled/Replaced failed.
    else
        memcpy(osConfirm.FUNCTION_CODE, "201", 3 );
        
    //Int8 SPEEDY_TIME[8];        // 2.9(8)        3
    UFC::AnsiString msgOrigTransTimeStr = Msg->GetTransactTime();
    UFC::AnsiString origTransTimeStr = CheckOverseasTimeStrFormat(msgOrigTransTimeStr);    
    memcpy(osConfirm.SPEEDY_TIME, origTransTimeStr.c_str(), 8);
        
    //Int8 PATS_TIME[8];          // 3.9(8)       11 
    UFC::AnsiString msgTransTimeStr = Msg->GetTransactTime();
    UFC::AnsiString transTimeStr = CheckOverseasTimeStrFormat(msgTransTimeStr);    
    memcpy(osConfirm.PATS_TIME, transTimeStr.c_str(), 8);

    //Int8 SPEEDY_SEQNO[8];       // 4.9(8)       19
    UFC::AnsiString sequenceNoStr;
    
    //if( Msg->GetReportSequence() == 0 )
    {
        FOverseasSeqNo = FOverseasSeqNo + 1;
        sequenceNoStr.Printf("%08d", FOverseasSeqNo.ToInt32());
        UFC::BufferedLog::Printf(" Overseas Sequence Number[%d].", FOverseasSeqNo.ToInt32() );
    }
    //else
    //    sequenceNoStr.Printf("%08d", Msg->GetReportSequence() );
    memcpy(osConfirm.SPEEDY_SEQNO, sequenceNoStr.c_str(), 8);
    
    //Int8 FCM_ID[10];            // 5.X(10)      27  SIM:test   PTH
    memcpy(osConfirm.FCM_ID, "PTH       ", 10);
        
    //Int8 FCM_ACCOUNT[20];       // 6.X(20)      37
    UFC::AnsiString accountStr = Msg->GetSellSideAccount();
    if (accountStr.Length() < 20) accountStr.PadThis(20, ' ', true);  //Pad Right
    memcpy(osConfirm.FCM_ACCOUNT, accountStr.c_str(), 20);
        
    //Int8 ORDER_SEQNO[20];       // 7.X(20)      57  YYYYMMDD + SPEEDY_SEQNO
    //Int8 BEF_ORDER_SEQNO[20];   // 8.X(20)      77
    UFC::AnsiString orderSeqNoStr;
    
    if( UDD.GetClOrdID().Length() > 1 )
    {
        orderSeqNoStr = UDD.GetClOrdID();
        orderSeqNoStr.PadThis(20, ' ', true);  //Pad Right
        memcpy(osConfirm.ORDER_SEQNO, orderSeqNoStr.c_str(), 20);
        
        orderSeqNoStr = UDD.GetOrigClOrdID();
        orderSeqNoStr.PadThis(20, ' ', true);  //Pad Right    
        memcpy(osConfirm.BEF_ORDER_SEQNO, orderSeqNoStr.c_str(), 20);
    }
    else
    {
        orderSeqNoStr = msgRecvDateStr + sequenceNoStr;    
        orderSeqNoStr.PadThis(20, ' ', true);  //Pad Right
        memcpy(osConfirm.ORDER_SEQNO, orderSeqNoStr.c_str(), 20);
        memcpy(osConfirm.BEF_ORDER_SEQNO, orderSeqNoStr.c_str(), 20);
    }

    //TT's Client_Order_ID field length greater then 20, Combine ORDER_ID and SEC_ORDER_ID. 2025/02/06 12:00
    //Int8 ORDER_ID[20];          // 9.X(20)      97
    //Int8 SEC_ORDER_ID[20];      //10.X(20)     117 
    UFC::AnsiString clientOrderID = Msg->GetClOrdID();
    if (clientOrderID.Length() < 40) clientOrderID.PadThis(40, ' ', true);  //Pad Right
    memcpy(osConfirm.ORDER_ID, clientOrderID.c_str(), 40);
    
    //Int8 EXEC_TYPE;             //11.X(1)      137
    nsOrderMessageDefine::ExecTypeEnum apiExecType = Msg->GetExecType();
    switch(apiExecType)
    {
        case nsOrderMessageDefine::etPendingNew:         osConfirm.EXEC_TYPE = 'A'; break;  //  1
        case nsOrderMessageDefine::etNew:                osConfirm.EXEC_TYPE = '0'; break;  //  2
        case nsOrderMessageDefine::etPendingReplace:     osConfirm.EXEC_TYPE = 'E'; break;  //  3
        case nsOrderMessageDefine::etReplaced:           osConfirm.EXEC_TYPE = '5'; break;  //  4
        case nsOrderMessageDefine::etPendingCancel:      osConfirm.EXEC_TYPE = '6'; break;  //  5
        case nsOrderMessageDefine::etCanceled:           osConfirm.EXEC_TYPE = '4'; break;  //  6
        case nsOrderMessageDefine::etPartiallyFilled:    osConfirm.EXEC_TYPE = '1'; break;  //  7
        case nsOrderMessageDefine::etFilled:             osConfirm.EXEC_TYPE = '2'; break;  //  8
        case nsOrderMessageDefine::etExpired:            osConfirm.EXEC_TYPE = '4'; break;  // 10
        case nsOrderMessageDefine::etRejected:           osConfirm.EXEC_TYPE = '8'; break;  // 11
        case nsOrderMessageDefine::etOrderStatus:        osConfirm.EXEC_TYPE = 'I'; break;  // 12
        case nsOrderMessageDefine::etRestated:           osConfirm.EXEC_TYPE = 'D'; break;  // 14
        case nsOrderMessageDefine::etNone:                                                  //  0
        case nsOrderMessageDefine::etQuoteAccept:                                           //  9
        case nsOrderMessageDefine::etQuoteRequestAccept:                                    // 13
        default:                                         osConfirm.EXEC_TYPE = ' '; 
    }  //switch(Msg->GetExecType())
    
    //Int8 ORDER_STATUS;          //12.X(1)      138
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osPendingNew:          //  1
            osConfirm.ORDER_STATUS = 'A';
            break;
        case nsOrderMessageDefine::osNew:                 //  2
            if (apiExecType == nsOrderMessageDefine::etReplaced)
                osConfirm.ORDER_STATUS = '5';
            else
                osConfirm.ORDER_STATUS = '0';
            break;
        case nsOrderMessageDefine::osPendingReplace:      //  3
            osConfirm.ORDER_STATUS = 'E';
            break;
        case nsOrderMessageDefine::osReplaced:            //  4
            osConfirm.ORDER_STATUS = '5';
            break;
        case nsOrderMessageDefine::osPendingCancel:       //  5
            osConfirm.ORDER_STATUS = '6';
            break;
        case nsOrderMessageDefine::osCanceled:            //  6
            osConfirm.ORDER_STATUS = '4';
            break;
        case nsOrderMessageDefine::osPartiallyFilled:     //  7
            osConfirm.ORDER_STATUS = '1';
            break;
        case nsOrderMessageDefine::osFilled:              //  8
            osConfirm.ORDER_STATUS = '2';
            break;
        case nsOrderMessageDefine::osExpired:             // 10
            osConfirm.ORDER_STATUS = '4';
            break;
        case nsOrderMessageDefine::osRejected:            // 11
            osConfirm.ORDER_STATUS = '8';
            break;
        case nsOrderMessageDefine::osNone:                //  0
        case nsOrderMessageDefine::osQuoteAccept:         //  9
        case nsOrderMessageDefine::osQuoteRequestAccept:  // 12
        default:
            osConfirm.ORDER_STATUS = ' '; 
    }  //switch(Msg->GetExecType())

    //Int8 EXCHANGE[20];          //13.X(20)     139
    UFC::AnsiString exchangeCode = Msg->GetExchangeCode();
    if (exchangeCode.Length() < 20) exchangeCode.PadThis(20, ' ', true);  //Pad Right
    memcpy(osConfirm.EXCHANGE, exchangeCode.c_str(), 20);
     
    //Int8 COMMODITY[20];         //14.X(20)     159
    UFC::AnsiString symbol = Msg->GetSymbol();
    if (symbol.Length() < 20) symbol.PadThis(20, ' ', true);  //Pad Right
    memcpy(osConfirm.COMMODITY, symbol.c_str(), 20);
    
    UFC::AnsiString msgMaturityMonthYearStr = Msg->GetMaturityMonthYear();
    UFC::AnsiString leg1MaturityMonthYear = "", leg2MaturityMonthYear = "", leg1StrikePriceStr = "", leg2StrikePriceStr = "";
    char leg1Side = 0, leg2Side = 0, leg1CallPut = 0, leg2CallPut = 0;
    double leg1StrikePrice = 0.0, leg2StrikePrice = 0.0;
    UFC::PStringList *fieldsListPtr = new UFC::PStringList();
    fieldsListPtr->SetStrings(msgMaturityMonthYearStr, "|");
    int legCount = fieldsListPtr->ItemCount();
    if (legCount <= 1)
        leg1MaturityMonthYear = ConvertOverseasMaturityMonthYear(msgMaturityMonthYearStr);
    else
    {
        UFC::AnsiString leg1Str = fieldsListPtr->GetItem(0);
        ParseLegMaturityMonthYear(leg1Str, leg1Side, leg1MaturityMonthYear, leg1StrikePriceStr, leg1CallPut);
        if ((leg1CallPut == 'C') || (leg1CallPut == 'P'))  leg1StrikePrice = leg1StrikePriceStr.ToDouble();
        UFC::AnsiString leg2Str = fieldsListPtr->GetItem(1);
        ParseLegMaturityMonthYear(leg2Str, leg2Side, leg2MaturityMonthYear, leg2StrikePriceStr, leg2CallPut);
        if ((leg2CallPut == 'C') || (leg2CallPut == 'P'))  leg2StrikePrice = leg2StrikePriceStr.ToDouble();
    }
    delete fieldsListPtr;
    
    nsOrderMessageDefine::EventTypeEnum msgCallPut = Msg->GetEventType();
    //Int8 SECU_TYPE[5];          //15.X(5)      179
    if (legCount <= 1)
    {
        if ((msgCallPut == nsOrderMessageDefine::evtCall) || (msgCallPut == nsOrderMessageDefine::evtPut))
            memcpy(osConfirm.SECU_TYPE, "OPT  ", 5);
        else
            memcpy(osConfirm.SECU_TYPE, "FUT  ", 5);
    }
    else
    {
        if ((leg1CallPut == 'C') || (leg1CallPut == 'P'))
            memcpy(osConfirm.SECU_TYPE, "OPT  ", 5);
        else
            memcpy(osConfirm.SECU_TYPE, "FUT  ", 5);
    }
    
    //Int8 SETTLEMENT_MONTH[6];   //16.9(6)      184
    if (leg1MaturityMonthYear.Length() < 6) leg1MaturityMonthYear.PadThis(6, '0', true);  //Pad Right
    memcpy(osConfirm.SETTLEMENT_MONTH, leg1MaturityMonthYear.c_str(), 6);
    
    //Int8 STRIKE_PRICE[13];      //17.9(7)V9(6) 190
    UFC::AnsiString tempDoubleStr = "", strikePriceStr = "";
    if (legCount <= 1)
        tempDoubleStr.Printf("%014.6lf", Msg->GetStrikePrice());
    else
        tempDoubleStr.Printf("%014.6lf", leg1StrikePrice);
    tempDoubleStr.PadThis(14, '0', true);  //Pad Right
    strikePriceStr.Printf("%7s%6s", tempDoubleStr.SubString(0, 7).c_str(), tempDoubleStr.SubString(8, 6).c_str());
    memcpy(osConfirm.STRIKE_PRICE, strikePriceStr.c_str(), 13);

    if (legCount <= 1)
    {
        //Int8 CALL_PUT_FLAG;         //18.X(1)      203  0:PUT  1:CALL
        if (msgCallPut == nsOrderMessageDefine::evtPut)
            osConfirm.CALL_PUT_FLAG = '0';
        else if (msgCallPut == nsOrderMessageDefine::evtCall)
            osConfirm.CALL_PUT_FLAG = '1';
        else
            osConfirm.CALL_PUT_FLAG = ' ';
        
        //Int8 BUY_SELL_KIND;         //19.X(1)      204  1:Buy  2:SELL
        if (Msg->GetSide() == nsOrderMessageDefine::sBuy)
            osConfirm.BUY_SELL_KIND = '1';
        else if (Msg->GetSide() == nsOrderMessageDefine::sSell)
            osConfirm.BUY_SELL_KIND = '2';
        else
            osConfirm.BUY_SELL_KIND = ' ';
    }
    else
    {
        //Int8 CALL_PUT_FLAG;         //18.X(1)      203  0:PUT  1:CALL
        if (leg1CallPut == 'P')
            osConfirm.CALL_PUT_FLAG = '0';
        else if (leg1CallPut == 'C')
            osConfirm.CALL_PUT_FLAG = '1';
        else
            osConfirm.CALL_PUT_FLAG = ' ';
        
        //Int8 BUY_SELL_KIND;         //19.X(1)      204  1:Buy  2:SELL
        if (leg1Side == 'B')
            osConfirm.BUY_SELL_KIND = '1';
        else if (leg1Side == 'S')
            osConfirm.BUY_SELL_KIND = '2';
        else
            osConfirm.BUY_SELL_KIND = ' ';
    }
        
    //Int8 ORDER_QTY[10];         //20.9(10)     205
    UFC::AnsiString orderQtyStr;
    orderQtyStr.Printf( "%010d", Msg->GetOrderQty());
    memcpy(osConfirm.ORDER_QTY, orderQtyStr.c_str(), 10);    

    //Int8 ORDER_PRICE[13];       //21.9(7)V9(6) 215
    tempDoubleStr.Printf("%014.6lf", fabs(Msg->GetPrice()));
    tempDoubleStr.PadThis(14, '0');
    UFC::AnsiString orderPriceStr;
    orderPriceStr.Printf("%7s%6s", tempDoubleStr.SubString(0, 7).c_str(), tempDoubleStr.SubString(8, 6).c_str());
    memcpy(osConfirm.ORDER_PRICE, orderPriceStr.c_str(), 13);
    
    //Int8 STOP_PRICE[13];        //22.9(7)V9(6) 228
    tempDoubleStr.Printf("%014.6lf", fabs(Msg->GetStopPx()));
    tempDoubleStr.PadThis(14, '0');
    UFC::AnsiString stopPriceStr;
    stopPriceStr.Printf("%7s%6s", tempDoubleStr.SubString(0, 7).c_str(), tempDoubleStr.SubString(8, 6).c_str());
    memcpy(osConfirm.STOP_PRICE, stopPriceStr.c_str(), 13);
    
    //Int8 ORDER_TYPE;            //23.X(1)      241
    switch(Msg->GetOrderType())
    {
        case nsOrderMessageDefine::otMarket:    osConfirm.ORDER_TYPE = '1'; break;  //  1
        case nsOrderMessageDefine::otLimit:     osConfirm.ORDER_TYPE = '2'; break;  //  2
        case nsOrderMessageDefine::otStop:      osConfirm.ORDER_TYPE = '3'; break;  //  3
        case nsOrderMessageDefine::otStopLimit: osConfirm.ORDER_TYPE = '4'; break;  //  4
        case nsOrderMessageDefine::otNone:                                          //  0
        default:                                osConfirm.ORDER_TYPE = ' ';         
    }  //switch(Msg->GetOrderType())
    
    //Int8 ORDER_COND;            //24.X(1)      242
    switch(Msg->GetTimeInForce())
    {
        case nsOrderMessageDefine::tifROD:  osConfirm.ORDER_COND = '0'; break;  //  1
        case nsOrderMessageDefine::tifIOC:  osConfirm.ORDER_COND = '3'; break;  //  2
        case nsOrderMessageDefine::tifFOK:  osConfirm.ORDER_COND = '4'; break;  //  3
        case nsOrderMessageDefine::tifNone:                                     //  0
        case nsOrderMessageDefine::tifTFXQ:                                     //  8
        default:                            osConfirm.ORDER_COND = '0';         
    }    
    
    //Int8 EXPIRE_DATE[8];        //25.9(8)      243
    memcpy(osConfirm.EXPIRE_DATE, "00000000", 8);
    
    //Int8 OPEN_OFFSET_KIND;      //26.X(1)      251
    switch(Msg->GetPositionEffect())
    {
        case nsOrderMessageDefine::peOpen:           osConfirm.OPEN_OFFSET_KIND = 'O'; break;  //  0
        case nsOrderMessageDefine::peClose:          osConfirm.OPEN_OFFSET_KIND = 'C'; break;  //  1
        case nsOrderMessageDefine::peDayTrade:       osConfirm.OPEN_OFFSET_KIND = 'D'; break;  //  2
        case nsOrderMessageDefine::peTMPMarketMaker: osConfirm.OPEN_OFFSET_KIND = '9'; break;  //  9
        case nsOrderMessageDefine::peDayTradeOpen:                                             //  3
        case nsOrderMessageDefine::peAuto:                                                     //  4
        case nsOrderMessageDefine::peAutoToday:                                                //  5
        default:                                     osConfirm.OPEN_OFFSET_KIND = ' ';         
    }  //switch(Msg->GetPositionEffect())
    
    //Int8 REJECT_TO;             //27.X(1)      252
    //Int8 REJECT_REASON;         //28.X(1)      253
    osConfirm.REJECT_TO = ' ';
    osConfirm.REJECT_REASON = ' ';

    if (msgOrderStatus == nsOrderMessageDefine::osRejected)
    {
        switch( Msg->GetCxlRejResponseTo() )
        {
            case nsOrderMessageDefine::crrCancel:  osConfirm.REJECT_TO = '1';
                                                   break;
            case nsOrderMessageDefine::crrReplace: osConfirm.REJECT_TO = '2';
                                                   break;
            default:                               switch( TNetworkID::GetMessageType( Msg->GetNID() ) )
                                                   {
                                                      case nsOrderMessageDefine::mtCancel:    osConfirm.REJECT_TO = '1';break;
                                                      case nsOrderMessageDefine::mtReplace: 
                                                      case nsOrderMessageDefine::mtReplacePx: osConfirm.REJECT_TO = '2';break;
                                                      default:                                osConfirm.REJECT_TO = ' ';break;     
                                                   }        
                                                   break;
        }        
        
        switch(Msg->GetOrdRejReason())
        {
            case nsOrderMessageDefine::orrBrokerExchangeOption:               osConfirm.REJECT_REASON = '2'; break; //  1
            case nsOrderMessageDefine::orrTooLateToEnter:                     osConfirm.REJECT_REASON = '0'; break; //  5
            case nsOrderMessageDefine::orrUnknownOrder:                       osConfirm.REJECT_REASON = '1'; break; //  6
            case nsOrderMessageDefine::orrNone:                                                                     //  0
            case nsOrderMessageDefine::orrUnknownSymbol:                                                            //  2
            case nsOrderMessageDefine::orrExchangeClosed:                                                           //  3
            case nsOrderMessageDefine::orrOrderExceedsLimit:                                                        //  4
            case nsOrderMessageDefine::orrDuplicateOrder:                                                           //  7
            case nsOrderMessageDefine::orrDuplicateVerballyCommunicatedOrder:                                       //  8
            case nsOrderMessageDefine::orrStaleOrder:                                                               //  9
            case nsOrderMessageDefine::orrTradeAlongRequired:                                                       // 10
            case nsOrderMessageDefine::orrInvalidInvestorID:                                                        // 11
            case nsOrderMessageDefine::orrUnsupportedOrderCharacteristic:                                           // 12
            case nsOrderMessageDefine::orrSurveillenceOption:                                                       // 13
            case nsOrderMessageDefine::orrIncorrectQuantity:                                                        // 14
            case nsOrderMessageDefine::orrIncorrectAllocatedQuantity:                                               // 15
            case nsOrderMessageDefine::orrUnknownAccount:                                                           // 16
            case nsOrderMessageDefine::orrOther:                                                                    // 17
            default:                                                          osConfirm.REJECT_REASON = '5';
        }  //switch(Msg->GetOrdRejReason())
    }  //if (msgOrderStatus == nsOrderMessageDefine::osRejected)
    
    //Int8 TEXT[100];             //29.X(100)    254
    UFC::AnsiString msgTextStr = Msg->GetText();
    if (msgTextStr.Length() < 100) msgTextStr.PadThis(100, ' ', true);  //Pad Right
    memcpy(osConfirm.TEXT, msgTextStr.c_str(), 100);
     
    //Int8 ACNO[20];              //30.X(20)     354
    UFC::AnsiString BrokerAccount( Msg->GetAccount());
    
    BrokerAccount.PadThis(20, ' ', true);  //Pad Right
    memcpy(osConfirm.ACNO, BrokerAccount.c_str(), 20);    
        
    //Int8 TOT_MATCH_QTY[10];     //31.9(10)     374
    UFC::AnsiString totMatchQtyStr;
    totMatchQtyStr.Printf("%010d", Msg->GetCumQty());
    memcpy(osConfirm.TOT_MATCH_QTY, totMatchQtyStr.c_str(), 10);
    
    // Int8 PARTYID[7];            //32.X(7)      384 
    memcpy(osConfirm.PARTYID, "       ", 7 );
    // Int8 EUREXACCTID[7];        //33.X(7)      391             
    memcpy(osConfirm.EUREXACCTID, "       ", 7 );
    
    if (legCount <= 1)
    {
        // Int8 COMMODITY_ID2[20];     //34.X(20)     398
        memcpy(osConfirm.COMMODITY_ID2, "                    ", 20);
        
        // Int8 SETTLEMENT_MONTH2[10]; //35.9(10)     418
        memcpy(osConfirm.SETTLEMENT_MONTH2, "0000000000000", 10);
        
        // Int8 SECU_TYPE2[5];         //36.X(5)      428
        memcpy( osConfirm.SECU_TYPE2, osConfirm.SECU_TYPE, 5 );
    
        // Int8 STRIKE_PRICE2[13];     //37.9(7)V9(6) 433
        memcpy(osConfirm.STRIKE_PRICE2, "0000000000000000", 13);
        
        // Int8 CALL_PUT_FLAG2;        //38.X(1)      446  0:PUT  1:CALL
        osConfirm.CALL_PUT_FLAG2 = ' ';

        // Int8 BUY_SELL_KIND2;        //39.X(1)      447  1:Buy  2:SELL
        osConfirm.BUY_SELL_KIND2 = ' ';
    }
    else
    {
        // Int8 COMMODITY_ID2[20];     //34.X(20)     398
         memcpy(osConfirm.COMMODITY_ID2, symbol.c_str(), 20);
        
        // Int8 SETTLEMENT_MONTH2[10]; //35.9(10)     418
        if (leg2MaturityMonthYear.Length() < 6) leg2MaturityMonthYear.PadThis(6, '0', true);  //Pad Right
        memcpy(osConfirm.SETTLEMENT_MONTH2, leg2MaturityMonthYear.c_str(), 6);
        
        // Int8 SECU_TYPE2[5];         //36.X(5)      428
        if ((leg2CallPut == 'C') || (leg2CallPut == 'P'))
            memcpy(osConfirm.SECU_TYPE2, "OPT  ", 5);
        else
            memcpy(osConfirm.SECU_TYPE2, "FUT  ", 5);
        
        // Int8 STRIKE_PRICE2[13];     //37.9(7)V9(6) 433
        tempDoubleStr.Printf("%014.6lf", leg2StrikePrice);
        tempDoubleStr.PadThis(14, '0', true);  //Pad Right
        strikePriceStr.Printf("%7s%6s", tempDoubleStr.SubString(0, 7).c_str(), tempDoubleStr.SubString(8, 6).c_str());
        memcpy(osConfirm.STRIKE_PRICE2, strikePriceStr.c_str(), 13);
    
        // Int8 CALL_PUT_FLAG2;        //38.X(1)      446  0:PUT  1:CALL
        if (leg2CallPut == 'P')
            osConfirm.CALL_PUT_FLAG2 = '0';
        else if (leg2CallPut == 'C')
            osConfirm.CALL_PUT_FLAG2 = '1';
        else
            osConfirm.CALL_PUT_FLAG2 = ' ';
        
        // Int8 BUY_SELL_KIND2;        //39.X(1)      447  1:Buy  2:SELL
        if (leg2Side == 'B')
            osConfirm.BUY_SELL_KIND2 = '1';
        else if (leg2Side == 'S')
            osConfirm.BUY_SELL_KIND2 = '2';
        else
            osConfirm.BUY_SELL_KIND2 = ' ';
    }
    
    // Int8 PRICE_FLAG;            //40.X(1)      448  + ' ' /-
    if( Msg->GetPrice() >= 0.0 )
        osConfirm.PRICE_FLAG = '+';
    else
        osConfirm.PRICE_FLAG = '-';
    // Int8 SUB_ACNO[7];           //41.X(7)      449     
    memcpy(osConfirm.SUB_ACNO, UDD.GetSubAccount().c_str(), 7);
    //Int8 DAY_TRADE_ID;          //42.X(1)      456    
    if (osConfirm.OPEN_OFFSET_KIND == 'D' )    
        osConfirm.DAY_TRADE_ID = 'Y';
    else
        osConfirm.DAY_TRADE_ID = ' ';    
    // Int8 ORDER_NO[5];           //43.X(5)      457
    UFC::AnsiString orderIDStr( Msg->GetOrderID() );
    if (orderIDStr.Length() < 5) orderIDStr.PadThis(5, ' ', true);  //Pad Right
    memcpy(osConfirm.ORDER_NO, orderIDStr.c_str(), 5);    
    //  Int8 FILLER[19];            //44.x(19)     462 
    memset( osConfirm.FILLER, ' ', 19  );  
    // Int8 END;                   //45.X(1)      481
    osConfirm.END = 0;    
    
    Send((char*)&osConfirm, etOverseaConfirm );
}  //MLExecConnection::OverseasConfirmToBackend()
//------------------------------------------------------------------------------
void MLExecConnection::OverseasFillToBackend( TExecutionReportMessage* Msg )
{
    UFC::UDateTime msgRecvTime;
    msgRecvTime.setCurrent();
    UFC::AnsiString msgRecvDateStr;
    
    UFC::GetTradeYYYYMMDD( msgRecvDateStr, false );
    //msgRecvDateStr.Printf("%04d%02d%02d", msgRecvTime.getYear(), msgRecvTime.getMonth(),msgRecvTime.getDate());
    UFC::AnsiString msgUserData = Msg->GetData();
    UFC::BufferedLog::DebugPrintf(" OverseasFillToBackend() UserData[%s]", msgUserData.c_str());
    UFC::AnsiString userData = "";
    Int32 proxyNID = 0;
    UFC::AnsiString proxyAE = "";
    if (!IsProxyUserData(msgUserData, proxyAE, proxyNID, userData)) userData = msgUserData;
    
    TUserData UDD( userData.c_str() );
    OS_MATCH_STRUCT osFill;
    //Int8 FUNCTION_CODE[3];      // 1.X(3)        0  203
    memcpy(osFill.FUNCTION_CODE, "203", 3);

    //Int8 MATCH_TIME[8];         // 2.9(8)        3
    UFC::AnsiString msgTransTimeStr = Msg->GetTransactTime();
    UFC::AnsiString transTimeStr = CheckOverseasTimeStrFormat(msgTransTimeStr);
    
    memcpy(osFill.MATCH_TIME, transTimeStr.c_str(), 8);
    
    //Int8 SPEEDY_SEQNO[8];       // 3.9(8)       11
    UFC::AnsiString sequenceNoStr;
    
    //if( Msg->GetReportSequence() == 0 )
    {
        FOverseasSeqNo = FOverseasSeqNo + 1;
        sequenceNoStr.Printf("%08d", FOverseasSeqNo.ToInt32());
        UFC::BufferedLog::Printf(" Overseas Sequence Number[%d].", FOverseasSeqNo.ToInt32() );
    }
    //else
    //    sequenceNoStr.Printf("%08d", Msg->GetReportSequence() );    
    memcpy(osFill.SPEEDY_SEQNO, sequenceNoStr.c_str(), 8);
    
    //Int8 FCM_ID[10];            // 4.X(10)      19  SIM:test   PTH
    memcpy(osFill.FCM_ID, "PTH       ", 10);
    
    //Int8 FCM_ACCOUNT[20];       // 5.X(20)      29
    UFC::AnsiString accountStr = Msg->GetSellSideAccount();
    if (accountStr.Length() < 20) accountStr.PadThis(20, ' ', true);  //Pad Right
    memcpy(osFill.FCM_ACCOUNT, accountStr.c_str(), 20);
    
    //Int8 MATCH_SEQNO[20];       // 6.X(20)      49  YYYYMMDD + SPEEDY_SEQNO
    //Int8 BEF_MATCH_SEQNO[20];   // 7.X(20)      69
    UFC::AnsiString matchSeqNoStr;
    
    if( UDD.GetClOrdID().Length() > 1 )
    {
        matchSeqNoStr = UDD.GetClOrdID();
        matchSeqNoStr.PadThis(20, ' ', true);  //Pad Right
        memcpy(osFill.MATCH_SEQNO, matchSeqNoStr.c_str(), 20);
        
        matchSeqNoStr = UDD.GetOrigClOrdID();
        matchSeqNoStr.PadThis(20, ' ', true);  //Pad Right    
        memcpy(osFill.BEF_MATCH_SEQNO, matchSeqNoStr.c_str(), 20);
    }
    else
    {
        matchSeqNoStr = msgRecvDateStr + sequenceNoStr;    
        matchSeqNoStr.PadThis(20, ' ', true);  //Pad Right
        memcpy(osFill.MATCH_SEQNO, matchSeqNoStr.c_str(), 20);
        memcpy(osFill.BEF_MATCH_SEQNO, matchSeqNoStr.c_str(), 20);
    }    
    
    //Int8 ORDER_ID[20];          // 8.X(20)      89
    UFC::AnsiString clientOrderID = Msg->GetClOrdID();
    if (clientOrderID.Length() < 20) clientOrderID.PadThis(20, ' ', true);  //Pad Right
    memcpy(osFill.ORDER_ID, clientOrderID.c_str(), 20);
    
    //Int8 PATS_SEQNO[70];        // 9.X(70)     109
    UFC::AnsiString execIDStr = Msg->GetExecID();
    if (execIDStr.Length() < 70) execIDStr.PadThis(70, ' ', true);  //Pad Right
    memcpy(osFill.PATS_SEQNO, execIDStr.c_str(), 70);
        
    //Int8 TRANS_TYPE;            //10.X(1)      179
    osFill.TRANS_TYPE = ' ';
    
    //Int8 REFID[20];             //11.X(20)     180
    memcpy(osFill.REFID, "                    ", 20);

    //Int8 EXEC_TYPE;             //12.X(1)      200
    switch(Msg->GetExecType())
    {
        case nsOrderMessageDefine::etPendingNew:         osFill.EXEC_TYPE = 'A'; break;  // 1
        case nsOrderMessageDefine::etNew:                osFill.EXEC_TYPE = '0'; break;  // 2
        case nsOrderMessageDefine::etPendingReplace:     osFill.EXEC_TYPE = 'E'; break;  // 3
        case nsOrderMessageDefine::etReplaced:           osFill.EXEC_TYPE = '5'; break;  // 4
        case nsOrderMessageDefine::etPendingCancel:      osFill.EXEC_TYPE = '6'; break;  // 5
        case nsOrderMessageDefine::etCanceled:           osFill.EXEC_TYPE = '4'; break;  // 6
        case nsOrderMessageDefine::etPartiallyFilled:    osFill.EXEC_TYPE = '1'; break;  // 7
        case nsOrderMessageDefine::etFilled:             osFill.EXEC_TYPE = '2'; break;  // 8
        case nsOrderMessageDefine::etExpired:            osFill.EXEC_TYPE = 'C'; break;  //10
        case nsOrderMessageDefine::etRejected:           osFill.EXEC_TYPE = '8'; break;  //11
        case nsOrderMessageDefine::etOrderStatus:        osFill.EXEC_TYPE = 'I'; break;  //12
        case nsOrderMessageDefine::etNone:                                               // 0
        case nsOrderMessageDefine::etQuoteAccept:                                        // 9
        case nsOrderMessageDefine::etQuoteRequestAccept:                                 //13
        default:                                         osFill.EXEC_TYPE = ' '; 
    }  //switch(Msg->GetExecType())
    
    //Int8 ORDER_STATUS;          //13.X(1)      201
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osPendingNew:         osFill.ORDER_STATUS = 'A'; break;  //  1
        case nsOrderMessageDefine::osNew:                osFill.ORDER_STATUS = '0'; break;  //  2
        case nsOrderMessageDefine::osPendingReplace:     osFill.ORDER_STATUS = 'E'; break;  //  3
        case nsOrderMessageDefine::osReplaced:           osFill.ORDER_STATUS = '5'; break;  //  4
        case nsOrderMessageDefine::osPendingCancel:      osFill.ORDER_STATUS = '6'; break;  //  5
        case nsOrderMessageDefine::osCanceled:           osFill.ORDER_STATUS = '4'; break;  //  6
        case nsOrderMessageDefine::osPartiallyFilled:    osFill.ORDER_STATUS = '1'; break;  //  7
        case nsOrderMessageDefine::osFilled:             osFill.ORDER_STATUS = '2'; break;  //  8
        case nsOrderMessageDefine::osExpired:            osFill.ORDER_STATUS = 'C'; break;  // 10
        case nsOrderMessageDefine::osRejected:           osFill.ORDER_STATUS = '8'; break;  // 11
        case nsOrderMessageDefine::osNone:                                                  //  0
        case nsOrderMessageDefine::osQuoteAccept:                                           //  9
        case nsOrderMessageDefine::osQuoteRequestAccept:                                    // 12
        default:                                         osFill.ORDER_STATUS = ' '; 
    }  //switch(Msg->GetExecType())
    
    //Int8 REST_REASON[8];        //14.9(8)      202
    memcpy(osFill.REST_REASON, "00000000", 8);

    //Int8 EXCHANGE[20];          //15.X(20)     210
    UFC::AnsiString exchangeCode = Msg->GetExchangeCode();
    if (exchangeCode.Length() < 20) exchangeCode.PadThis(20, ' ', true);  //Pad Right
    memcpy(osFill.EXCHANGE, exchangeCode.c_str(), 20);
     
    //Int8 COMMODITY[20];         //16.X(20)     230
    UFC::AnsiString symbol = Msg->GetSymbol();
    if (symbol.Length() < 20) symbol.PadThis(20, ' ', true);  //Pad Right
    memcpy(osFill.COMMODITY, symbol.c_str(), 20);

    //Int8 PRODUCT_TYPE[5];       //17.X(5)      250
    nsOrderMessageDefine::EventTypeEnum msgCallPut = Msg->GetEventType();
    if ((msgCallPut == nsOrderMessageDefine::evtCall) || (msgCallPut == nsOrderMessageDefine::evtPut))
        memcpy(osFill.PRODUCT_TYPE, "OPT  ", 5);
    else
        memcpy(osFill.PRODUCT_TYPE, "FUT  ", 5);
    
    //Int8 SETTLEMENT_MONTH[6];   //18.9(6)      255
    UFC::AnsiString msgMaturityMonthYearStr = Msg->GetMaturityMonthYear();
    UFC::AnsiString maturityMonthYearStr = ConvertOverseasMaturityMonthYear(msgMaturityMonthYearStr);
    if (maturityMonthYearStr.Length() < 6) maturityMonthYearStr.PadThis(6, '0', true);  //Pad Right
    memcpy(osFill.SETTLEMENT_MONTH, maturityMonthYearStr.c_str(), 6);
    
    //Int8 STRIKE_PRICE[13];      //19.9(7)V9(6) 261
    UFC::AnsiString tempDoubleStr, strikePriceStr; 
    tempDoubleStr.Printf("%014.6lf", Msg->GetStrikePrice());
    tempDoubleStr.PadThis(14, '0');
    strikePriceStr.Printf("%7s%6s", tempDoubleStr.SubString(0, 7).c_str(), tempDoubleStr.SubString(8, 6).c_str());
    memcpy(osFill.STRIKE_PRICE, strikePriceStr.c_str(), 13);

    //Int8 CALL_PUT_FLAG;         //20.X(1)      274  0:PUT  1:CALL
    if (msgCallPut == nsOrderMessageDefine::evtPut)
        osFill.CALL_PUT_FLAG = '0';
    else if (msgCallPut == nsOrderMessageDefine::evtCall)
        osFill.CALL_PUT_FLAG = '1';
    else
        osFill.CALL_PUT_FLAG = ' ';
        
    //Int8 BUY_SELL_KIND;         //21.X(1)      275  1:Buy  2:SELL
    if (Msg->GetSide() == nsOrderMessageDefine::sBuy)
        osFill.BUY_SELL_KIND = '1';
    else if (Msg->GetSide() == nsOrderMessageDefine::sSell)
        osFill.BUY_SELL_KIND = '2';
    else
        osFill.BUY_SELL_KIND = ' ';
    
    //Int8 ORDER_QTY[10];         //22.9(10)     276
    UFC::AnsiString orderQtyStr;
    orderQtyStr.Printf( "%010d", Msg->GetOrderQty());
    memcpy(osFill.ORDER_QTY, orderQtyStr.c_str(), 10);    

    //Int8 ORDER_PRICE[13];       //23.9(7)V9(6) 286
    tempDoubleStr.Printf("%014.6lf", fabs(Msg->GetPrice()));
    tempDoubleStr.PadThis(14, '0');
    UFC::AnsiString orderPriceStr;
    orderPriceStr.Printf("%7s%6s", tempDoubleStr.SubString(0, 7).c_str(), tempDoubleStr.SubString(8, 6).c_str());
    memcpy(osFill.ORDER_PRICE, orderPriceStr.c_str(), 13);

    //Int8 STOP_PRICE[13];        //24.9(7)V9(6) 299
    tempDoubleStr.Printf("%014.6lf", fabs(Msg->GetStopPx()));
    tempDoubleStr.PadThis(14, '0');
    UFC::AnsiString stopPriceStr;
    stopPriceStr.Printf("%7s%6s", tempDoubleStr.SubString(0, 7).c_str(), tempDoubleStr.SubString(8, 6).c_str());
    memcpy(osFill.STOP_PRICE, stopPriceStr.c_str(), 13);
    
    //Int8 ORDER_TYPE;            //25.X(1)      312
    switch(Msg->GetOrderType())
    {
        case nsOrderMessageDefine::otMarket:    osFill.ORDER_TYPE = '1'; break;  //  1
        case nsOrderMessageDefine::otLimit:     osFill.ORDER_TYPE = '2'; break;  //  2
        case nsOrderMessageDefine::otStop:      osFill.ORDER_TYPE = '3'; break;  //  3
        case nsOrderMessageDefine::otStopLimit: osFill.ORDER_TYPE = '4'; break;  //  4
        case nsOrderMessageDefine::otNone:                                       //  0
        default:                                osFill.ORDER_TYPE = ' ';         
    }  //switch(Msg->GetOrderType())
    
    //Int8 ORDER_COND;            //26.X(1)      313
    switch(Msg->GetTimeInForce())
    {
        case nsOrderMessageDefine::tifROD:  osFill.ORDER_COND = '0'; break;  //  1
        case nsOrderMessageDefine::tifIOC:  osFill.ORDER_COND = '3'; break;  //  2
        case nsOrderMessageDefine::tifFOK:  osFill.ORDER_COND = '4'; break;  //  3
        case nsOrderMessageDefine::tifNone:                                  //  0
        case nsOrderMessageDefine::tifTFXQ:                                  //  8
        default:                            osFill.ORDER_COND = '0';         
    }
    
    //Int8 EXPIRE_DATE[8];        //27.9(8)      314
    memcpy(osFill.EXPIRE_DATE, "00000000", 8);
    
    //Int8 OPEN_OFFSET_KIND;      //28.X(1)      322
    switch(Msg->GetPositionEffect())
    {
        case nsOrderMessageDefine::peOpen:           osFill.OPEN_OFFSET_KIND = 'O'; break;  //  0
        case nsOrderMessageDefine::peClose:          osFill.OPEN_OFFSET_KIND = 'C'; break;  //  1
        case nsOrderMessageDefine::peDayTrade:       osFill.OPEN_OFFSET_KIND = 'D'; break;  //  2
        case nsOrderMessageDefine::peTMPMarketMaker: osFill.OPEN_OFFSET_KIND = '9'; break;  //  9
        case nsOrderMessageDefine::peDayTradeOpen:                                          //  3
        case nsOrderMessageDefine::peAuto:                                                  //  4
        case nsOrderMessageDefine::peAutoToday:                                             //  5
        default:                                     osFill.OPEN_OFFSET_KIND = ' ';         
    }  //switch(Msg->GetPositionEffect())
    
    //Int8 MATCH_QTY[10];         //29.9(10)     323
    UFC::AnsiString matchQtyStr;
    matchQtyStr.Printf("%010d", Msg->GetLastQty());    
    memcpy(osFill.MATCH_QTY, matchQtyStr.c_str(), 10);
    
    //Int8 MATCH_PRICE[13];       //30.9(7)V9(6) 333
    tempDoubleStr.Printf("%014.6lf", Msg->GetLastPx());
    tempDoubleStr.PadThis(14, '0');
    UFC::AnsiString lastPriceStr;
    lastPriceStr.Printf("%7s%6s", tempDoubleStr.SubString(0, 7).c_str(), tempDoubleStr.SubString(8, 6).c_str());
    memcpy(osFill.MATCH_PRICE, lastPriceStr.c_str(), 13);
    
    //Int8 UNMATCH_QTY[10];       //31.9(10)     346
    UFC::AnsiString leavesQtyStr;
    leavesQtyStr.Printf("%010d", Msg->GetLeavesQty());
    memcpy(osFill.UNMATCH_QTY, leavesQtyStr.c_str(), 10);
    
    //Int8 TOT_MATCH_QTY[10];     //32.9(10)     356
    UFC::AnsiString totMatchQtyStr;
    totMatchQtyStr.Printf("%010d", Msg->GetCumQty());    
    memcpy(osFill.TOT_MATCH_QTY, totMatchQtyStr.c_str(), 10);
    
    //Int8 AVG_MATCH_PRICE[13];   //33.9(7)V9(6) 366
    tempDoubleStr.Printf("%014.6lf", Msg->GetAvgPx());
    tempDoubleStr.PadThis(14, '0');
    UFC::AnsiString averagePriceStr;
    averagePriceStr.Printf("%7s%6s", tempDoubleStr.SubString(0, 7).c_str(), tempDoubleStr.SubString(8, 6).c_str());
    memcpy(osFill.AVG_MATCH_PRICE, averagePriceStr.c_str(), 13);
    
    //Int8 ACNO[20];              //34.X(20)     379
    UFC::AnsiString BrokerAccount( Msg->GetAccount());
    
    BrokerAccount.PadThis(20, ' ', true);  //Pad Right
    memcpy(osFill.ACNO, BrokerAccount.c_str(), 20);    
    //Int8 PARTYID[7];            //35.X(7)      399
    memcpy(osFill.PARTYID, "       ", 7 );    
    // Int8 EUREXACCTID[7];        //36.X(7)      406         
    memcpy(osFill.EUREXACCTID, "       ", 7 );
    // Int8 COMMODITY_ID2[20];     //37.X(20)     413
    memcpy(osFill.COMMODITY_ID2, "                    ", 20);            
    // Int8 SETTLEMENT_MONTH2[10]; //38.9(10)     433 
    memcpy(osFill.SETTLEMENT_MONTH2, "0000000000000000", 10);            
    // Int8 SECU_TYPE2[5];         //39.X(5)      443            
    memcpy(osFill.SECU_TYPE2, osFill.PRODUCT_TYPE, 5 );
    // Int8 STRIKE_PRICE2[13];     //40.9(7)V9(6) 448    
    memcpy(osFill.STRIKE_PRICE2, "0000000000000000", 13);        
    // Int8 CALL_PUT_FLAG2;        //41.X(1)      461
    osFill.CALL_PUT_FLAG2 = ' ';
    // Int8 BUY_SELL_KIND2;        //42.X(1)      462    
    osFill.BUY_SELL_KIND2 = ' ';
    // Int8 PRICE_FLAG;            //43.X(1)      463  + ' ' /-
    if( Msg->GetPrice() >= 0.0 )
        osFill.PRICE_FLAG = '+';        
    else
        osFill.PRICE_FLAG = '-';        
    // Int8 SUB_ACNO[7];           //44.X(7)      464 
    memcpy(osFill.SUB_ACNO, UDD.GetSubAccount().c_str(), 7);
    // Int8 DAY_TRADE_ID;          //45.X(1)      471        
     if (osFill.OPEN_OFFSET_KIND == 'D')    
        osFill.DAY_TRADE_ID = 'Y';
    else
        osFill.DAY_TRADE_ID = ' ';
    // Int8 ORDER_NO[5];           //46.X(5)      472
    UFC::AnsiString orderIDStr( Msg->GetOrderID() );
    if (orderIDStr.Length() < 5) orderIDStr.PadThis(5, ' ', true);  //Pad Right
    memcpy(osFill.ORDER_NO, orderIDStr.c_str(), 5);
    // Int8 FILLER[14];            //47.x(14)     477 
    memset( osFill.FILLER, ' ', 14  );  
    // Int8 END;                   //48.X(1)      491    
    osFill.END = 0;    
    ///< End TEMPF-BODY
    
    
    Send((char*)&osFill, etOverseaFilled);    
}  //MLExecConnection::OverseasFillToBackend()
//------------------------------------------------------------------------------