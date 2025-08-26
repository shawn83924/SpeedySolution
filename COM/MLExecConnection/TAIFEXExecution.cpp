/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "MLExecConnection.h"
#include "../../UFC/NetUtility.h"
#include "../../TAIFEXLib/TMPMessage.h"
#include "TUserData.h"
#include <math.h>
//------------------------------------------------------------------------------
extern UFC::UiniFile* pConfig;
extern Int64 DoubleToInt64( double DoubleVal, int Digi);
//------------------------------------------------------------------------------
void MLExecConnection::ToBackend( TExecutionReportMessage* Msg ,Int8 type)
{
    //  type == 2 //is Fill
    //  type == 1 //is New
    //  type == 0 // is Cancel Replace
    //  type == -1 //is Reject
    UFC::BufferedLog::DebugPrintf( " rpt seq:[%d]", Msg->GetReportSequence() );
    if(  Msg->GetReportSequence() == 0 )
        return;
    UFC::BufferedLog::DebugPrintf( " TMP ExecType:[%c]   ExecType:[%d]", Msg->GetTMPExecType(), Msg->GetExecType() );
    if( Msg->GetExecType() == nsOrderMessageDefine::etCanceled && Msg->GetTMPExecType() == nsOrderMessageDefine::tetFilled && Msg->GetCumQty() != 0 ) ///< drop IOC auto delete
    {
    	UFC::BufferedLog::DebugPrintf( " Drop IOC auto delete." );
        return;
    }

    UFC::AnsiString OrderID = Msg->GetOrderID();
    UFC::AnsiString Symbol = Msg->GetSymbol();

    if( Symbol == "     " )
        Symbol = *(FSymbolTable.GetObjectByKey( OrderID ));
    else
        AddToSymbolTable( OrderID, Symbol );
    UFC::BufferedLog::DebugPrintf( " Symbol:[%s]", Symbol.c_str() );

    OKD okd;
    memset(&okd,' ',400);
    okd.END = 0;
    UFC::UDateTime time;
    ///< Fill ODK-DATE
    time.setCurrent();
    UFC::AnsiString dateStr,timeStr;
    dateStr.Printf("%04d%02d%02d", time.getYear(),time.getMonth(),time.getDate());
    timeStr.Printf("%02d%02d%02d%04d",time.getHour(), time.getMinute(), time.getSecond(),time.getMillisecond());
    ///< Fill ODK-MSG-SEQNUM
    sn++;
    UFC::AnsiString data;
    data.Printf("%010d",sn);
    memcpy( okd.DATE,dateStr.c_str(),8);
    memcpy( okd.MSG_SEQNUM,data.c_str(), 10 );
    memcpy( okd.MSG_DATE,dateStr.c_str(), 8 );
    memcpy( okd.MSG_TIME,timeStr.c_str(), 10 );
    ///< FILL ODK-MSG-TYPE  102:Execution 103:Reject 108:Quote request.(Not support yet)
    if( type == -1 )
        memcpy(okd.MSG_TYPE,"103",3);
    else
        memcpy(okd.MSG_TYPE,"102",3);
    ///< Fill LNK_BROKER_NO
    data.Printf("%05d", Msg->GetTMPBrokerID() );
    data.PadThis(5,'0');
    memcpy(okd.LNK_BROKER_NO,data.c_str(),5);
    ///< Fill SESSION_ID
    data.Printf("%05d",UFC::AnsiString(Msg->GetPVC()).ToInt());
    data.PadThis(5,' ');
    memcpy(okd.SESSION_ID,data.c_str(),5);
    ///< Fill STATUS_CODE
    data.Printf("%03d",Msg->GetTMPStstusCode());
    data.PadThis(3,'0');
    memcpy( okd.STATUS_CODE,data.c_str(), 3 );

    switch( Msg->GetExecType())
    {
        case nsOrderMessageDefine::etNew:
            okd.EXEC_TYPE[0] = '0';
            break;
        case nsOrderMessageDefine::etReplaced:
            if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2  )
                okd.EXEC_TYPE[0] = 'M';
            else
                okd.EXEC_TYPE[0] = '5';
            break;
        case nsOrderMessageDefine::etCanceled:
            if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetNewAndFilled )
                okd.EXEC_TYPE[0] = '6';
            else
                okd.EXEC_TYPE[0] = '4';
            break;
        case nsOrderMessageDefine::etPartiallyFilled:
        case nsOrderMessageDefine::etFilled:
            if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetNewAndFilled )
                okd.EXEC_TYPE[0] = '6';
            else
                okd.EXEC_TYPE[0] = 'F';
            break;
        case nsOrderMessageDefine::etOrderStatus:
            okd.EXEC_TYPE[0] = 'I';
            break;
        case nsOrderMessageDefine::etRejected:
            switch( Msg->GetTMPExecType() )
            {
                case nsOrderMessageDefine::tetNew:      okd.EXEC_TYPE[0] = '0';break;
                case nsOrderMessageDefine::tetCanceled: okd.EXEC_TYPE[0] = '4';break;
                case nsOrderMessageDefine::tetReplaced: 
                    if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                        Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2  )
                        okd.EXEC_TYPE[0] = 'M';
                    else
                        okd.EXEC_TYPE[0] = '5';
                    break;
                default : 
                    UFC::BufferedLog::Printf( " CxlRejResponseTo:[%d]", Msg->GetCxlRejResponseTo() );
                    switch( Msg->GetCxlRejResponseTo() )
                    {
                        case nsOrderMessageDefine::crrNew:     okd.EXEC_TYPE[0] = '0';break;
                        case nsOrderMessageDefine::crrReplace: okd.EXEC_TYPE[0] = '5';break;
                        case nsOrderMessageDefine::crrCancel:  okd.EXEC_TYPE[0] = '4';break;
                        case nsOrderMessageDefine::crrOrderStatus: okd.EXEC_TYPE[0] = 'I';break;
                        default: okd.EXEC_TYPE[0] = '0'; break;
                    }
                    break;
            }
            break;
        case nsOrderMessageDefine::etQuoteAccept:
            UFC::BufferedLog::DebugPrintf( " Quote Accept: ExecType:[0]" );
            okd.EXEC_TYPE[0] = '0';
            break;
        default:
            break;
    }
    ///< Fill CLEAR_MEMBER_ID_NO
    int CMIndex = TMP::TMPMessage::GetCMID( Msg->GetCMID(), Msg->GetBrokerID() );
    data.Printf("%05d", CMIndex );
    data.PadThis( 5,'0');
    memcpy( okd.CLEAR_MEMBER_ID_NO, data.c_str(),5);
    ///< Fill BROKER_ID_NO.
    int BrokerIndex = TMP::TMPMessage::GetBrokerID( Msg->GetBrokerID());
    data.Printf("%05d", BrokerIndex );
    data.PadThis( 5,'0');
    memcpy( okd.BROKER_ID_NO, data.c_str(),5);
    ///< Fill ORDER_NO
    data.Printf("%s",Msg->GetOrderID());
    data.PadThis(5,' ');
    memcpy(okd.ORDER_NO,data.c_str(),5);
    ///< Fill ORRD_ID #### Not use ####
    memcpy(okd.ORD_ID,"0000000",7);         ///< OrderID (Internal use)
    
    UFC::AnsiString userData,AE;
    Int32 NID = 0;
    
    ///< Fill USER_DEFINE UserData 8-15 X(8)
    if( IsProxyUserData( Msg->GetData(), AE, NID, userData ) == TRUE )    
        UFC::BufferedLog::DebugPrintf( " Proxy UserData:[%s]", userData.c_str());
    else
        UFC::BufferedLog::DebugPrintf( " Gateway UserData:[%s]", userData.c_str());
    userData.PadThis( 15, ' ' );
    UFC::AnsiString OKD_USER_DEFINE( userData.SubString( 7, 8 ) );
    UFC::BufferedLog::DebugPrintf( " OKD_USER_DEFINE:[%s]", OKD_USER_DEFINE.c_str() );
    memcpy( okd.USER_DEFINE, OKD_USER_DEFINE.c_str(), 8 ); ///< UserDefine (Internal use)
    ///< Fill SYBMOL_TYPE support '2' now.( Text mode )
    okd.SYBMOL_TYPE[0] = '2';
    data.Printf("%s",Symbol.c_str() );
    data.PadThis( 20,' ');
    memcpy(okd.TR_ID,data.c_str(),20);

    ///< Fill ORDER_PRICE
    int price = UFC::DoubleToInt( Msg->GetLastPx(), Msg->GetPxDigit() );
    data.Printf("%+09d", price);
    UFC::BufferedLog::DebugPrintf( " OrderPx:[%.*f] Digit:[%d]", Msg->GetPxDigit() , Msg->GetLastPx(), Msg->GetPxDigit() );
    UFC::BufferedLog::DebugPrintf( " OKD-ORDER-PRICE:[%d]", price );
    memcpy(okd.ORDER_PRICE, data.c_str(), 9);

    ///< Fill ORDER_QTY
    data.Printf( "%04d", Msg->GetTMPQty() );
    UFC::BufferedLog::DebugPrintf( " OKD-ORDER-QTY:[%s]", data.c_str() );
    
    memcpy(okd.ORDER_QTY,data.c_str(),4);
    ///< Fill INVESTOR_ACNO
    data.Printf("%s",Msg->GetAccount());
    data.PadThis(7,' ');
    memcpy(okd.INVESTOR_ACNO,data.c_str(),7);
    ///< Fill INVESTOR_FLAG
    if( Msg->GetExecType() == nsOrderMessageDefine::etQuoteAccept )
        data.Printf( "%s", "8" );
    else
        data.Printf("%s",Msg->GetAccountFlag());
    data.PadThis(1,' ');
    memcpy(okd.INVESTOR_FLAG,data.c_str(),1);
    UFC::BufferedLog::DebugPrintf( " INVESTOR-FLAG:[%s]", data.c_str() );
    ///< Fill BUY_SELL_KIND
    char BS_Code = ' ';
    switch(Msg->GetSide())
    {
        case nsOrderMessageDefine::sBuy:
            okd.BUY_SELL_KIND[0] = '1';
            BS_Code = 'B';
            break;
        case nsOrderMessageDefine::sSell:
            okd.BUY_SELL_KIND[0] = '2';
            BS_Code = 'S';
            break;
        default:
            break;
    }
    ///< Fill ORDER_TYPE
    switch( Msg->GetOrderType() )
    {
        case nsOrderMessageDefine::otMarket:
            okd.ORDER_TYPE[0] = '1';
            break;
        case nsOrderMessageDefine::otLimit:
            okd.ORDER_TYPE[0] = '2';
            break;
        case nsOrderMessageDefine::otMarketWithProtection:
            okd.ORDER_TYPE[0] = '3';
            break;
        default:
            break;
    }
    ///< Fill ORDER_COND

    switch( Msg->GetTimeInForce() )
    {
        case nsOrderMessageDefine::tifROD:            
            okd.ORDER_COND[0] = '0';
            break;
        case nsOrderMessageDefine::tifIOC:            
            okd.ORDER_COND[0] = '3';
            break;
        case nsOrderMessageDefine::tifFOK:            
            okd.ORDER_COND[0] = '4';
            break;
        case nsOrderMessageDefine::tifTFXQ:
            okd.ORDER_COND[0] = '8';
        default:
            break;
    }
    UFC::BufferedLog::DebugPrintf( " Time in Force:[%d]  ORDER_COND:[%c]", Msg->GetTimeInForce(), okd.ORDER_COND[0] );
    ///< Fill OPEN_OFFSET_KIND
    UFC::BufferedLog::DebugPrintf( " PositionEffect:[%d]", Msg->GetPositionEffect() );
    switch(Msg->GetPositionEffect())
    {
        case nsOrderMessageDefine::peOpen:
            okd.OPEN_OFFSET_KIND[0] = 'O';
            break;
        case nsOrderMessageDefine::peClose:
            okd.OPEN_OFFSET_KIND[0] = 'C';
            break;
        case nsOrderMessageDefine::peDayTrade:
            okd.OPEN_OFFSET_KIND[0] = 'D';
            break;
        case nsOrderMessageDefine::peTMPMarketMaker:
            okd.OPEN_OFFSET_KIND[0] = '9';
            break;
        case nsOrderMessageDefine::peTAIFEXAuto:
            okd.OPEN_OFFSET_KIND[0] = 'A';
            break;
        default:
            break;
    }
    
    ///< Fill LAST_MATCH_PRICE
    price = UFC::DoubleToInt( Msg->GetPrice(), Msg->GetPxDigit() );
    data.Printf("%+09d", price);
    if( type == 0 || okd.EXEC_TYPE[0] == '0' )
        data.Printf("%+010d",0);
    else
        data.Printf("%+010d",price);
    UFC::BufferedLog::DebugPrintf( " Last Price:[%+010d]", price );
    memcpy(okd.LAST_MATCH_PRICE,data.c_str(),10);
    ///< Fill LAST_MATCH_QTY
    if( type == 0 || okd.EXEC_TYPE[0] == '0' )
        data.Printf("%04d",0);
    else
        data.Printf("%04d",Msg->GetOrderQty());
    memcpy(okd.LAST_MATCH_QTY,data.c_str(),4);
    ///< Fill TOT_MATCH_PRICE
    data.Printf("%+017lld",(Int64)Msg->GetTMPPxSubTotal() );
    memcpy(okd.TOT_MATCH_PRICE,data.c_str(),17);
    ///< Fill TOT_MATCH_QTY
    data.Printf("%04d", Msg->GetCumQty() );
    memcpy(okd.TOT_MATCH_QTY,data.c_str(),4);
    ///< Fill LEAVES_QTY
    data.Printf("%04d",Msg->GetLeavesQty());
    memcpy(okd.LEAVES_QTY,data.c_str(),4);
    ///< Fill BEFORE_QTY
    data.Printf("%04d",Msg->GetBeforeQty());
    memcpy(okd.BEFORE_QTY,data.c_str(),4);
    ///< Fill BUY_SELL_KIND1,BUY_SELL_KIND2
    //okd.BUY_SELL_KIND1[0] = '0'+Msg->GetLegSide1();
    //okd.BUY_SELL_KIND2[0] = '0'+Msg->GetLegSide2();    
    ///< Fill MATCH_PRICE1
    price = UFC::DoubleToInt( Msg->GetLegPrice1(),Msg->GetPxDigit());    
    data.Printf("%09d",price);
    memcpy(okd.MATCH_PRICE1,data.c_str(),9);
    ///< Fill MATCH_PRICE2
    price = UFC::DoubleToInt( Msg->GetLegPrice2(),Msg->GetPxDigit());
    data.Printf("%09d",price);
    memcpy(okd.MATCH_PRICE2,data.c_str(),9);
    ///< Fill MATCH_QTY1
    data.Printf("%04d",Msg->GetLegQty1());
    memcpy(okd.MATCH_QTY1,data.c_str(),4);
    ///< Fill MATCH_QTY2
    data.Printf("%04d",Msg->GetLegQty2());
    memcpy(okd.MATCH_QTY2,data.c_str(),4);
    ///< Fill ORDER_DATE,LAST_DATE
    memcpy(okd.ORDER_DATE,dateStr.c_str(),8);
    memcpy(okd.LAST_DATE,dateStr.c_str(),8);
    ///< Fill ORDER_TIME,ORDER_MIL,LAST_TIME,LAST_MIL
    UFC::AnsiString TransactTime( Msg->GetTransactTime() );
    UFC::AnsiString OrgTransactTime( Msg->GetOrgTransactTime() );
    TransactTime.PadThis( 9, '0' );
    OrgTransactTime.PadThis( 9, '0' );
    int TransTime    = TransactTime.ToInt();
    int OrgTransTime = OrgTransactTime.ToInt();
    
    timeStr.Printf("%02d%02d%02d%04d",OrgTransTime/10000000,(OrgTransTime%10000000)/100000,(OrgTransTime%100000)/1000,OrgTransTime%1000);
    timeStr.PadThis(10,'0');
    memcpy(okd.ORDER_TIME,timeStr.c_str(),10);
    timeStr.Printf("%02d%02d%02d%04d",TransTime/10000000,(TransTime%10000000)/100000,(TransTime%100000)/1000,TransTime%1000);
    timeStr.PadThis(10,'0');
    memcpy(okd.LAST_TIME,timeStr.c_str(),10);
    ///< Fill TARGET_ID
    okd.TARGET_ID[0] = '4';
    ///< Fill UNIQUE_ID
    data.Printf("%010d",Msg->GetTMPUniqueID());
    memcpy(okd.UNIQUE_ID,data.c_str(),10);
    UFC::BufferedLog::DebugPrintf( " TMP UniqueID:[%010d]", Msg->GetTMPUniqueID() );
    ///< Fill SEQNO
    data.Printf("%010d",Msg->GetReportSequence());
    memcpy(okd.SEQNO,data.c_str(),10);
    UFC::BufferedLog::DebugPrintf( " ReportSequence:[%010d]", Msg->GetReportSequence() );
    ///< Fill PROTOCOL_TYPE always 1: TMP
    okd.PROTOCOL_TYPE[0] = '1';
    ///< Fill BROKER_ID
    data.Printf("%s",Msg->GetBrokerID());
    data.PadThis(7,' ');
    memcpy(okd.BROKER_ID,data.c_str(),7);
    ///< Fill Multi-leg fields.
    double PriceDouble;

    BO_SymbolDeCompose symbolDeCompose( Msg->GetSymbol(), BS_Code );
    if( symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stUnKnown ) ///< De compose failed
    {
        memset(okd.SETTLEMENT_MONTH1_1,'0',15);
        memset(okd.PRICE1_1,'0',12);
        memset(okd.SETTLEMENT_MONTH2_2,'0',15);
        memset(okd.PRICE2_2,'0',12);        
        memcpy(okd.PRICE_D,"+000000000000",13);
    }
    else
    {
        ///< Fill COMMODITY_ID1_1
        data.Printf("%s",symbolDeCompose.FComm1);
        UFC::AnsiString convert;
        if (pConfig->GetValue("Convert", data.SubString(0,3), convert) == FALSE)
        {
            data = data.SubString(0,3); ///< Not found in old symbol table. it's a new symbol, use first 3 chars.
            if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures )
            {
                UFC::AnsiString Value;
                Value.Printf("FI%s", data.c_str() );
                data = Value;
            }
        }
        else
            data = convert; ///< Example : convert TXF to FITX
        data.PadThis( 7, ' ');
        if( Msg->GetMarket() == nsOrderMessageDefine::mTWOptions )
        {
            data[6] = symbolDeCompose.FCP_Code1;
            UFC::BufferedLog::DebugPrintf( " Commod 1:[%s]", data.c_str() );
        }
        memcpy(okd.COMMODITY_ID1_1,data.c_str(),7);        
        ///< Fill SETTLEMENT_MONTH1_1
        data.Printf( "%s", symbolDeCompose.FSettleMonth1 );
        data.PadThis( 6,'0');
        memcpy(okd.SETTLEMENT_MONTH1_1,data.c_str(),6);
        ///< Fill STRIKE_PRICE1_1
        data.Printf("%s",symbolDeCompose.FExcercisePrice1);
        data.PadThis(9,'0');
        UFC::BufferedLog::DebugPrintf( " Strike Price:[%s]", data.c_str() );
        memcpy(okd.STRIKE_PRICE1_1,data.c_str(),9);
        ///< Fill second leg info.
        if( symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle || symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesSingle )
        {
            ///< It's a single order.
            memset( okd.COMMODITY_ID2_2,' ', 7 );
            memset( okd.SETTLEMENT_MONTH2_2,'0', 6 );
            memset( okd.STRIKE_PRICE2_2,'0', 9 );
            okd.BUY_SELL_KIND1[0] = '0';
            okd.BUY_SELL_KIND2[0] = '0';
        }
        else
        {
            ///< Fill COMMODITY_ID1_2
            data.Printf("%s",symbolDeCompose.FComm2);
            if (pConfig->GetValue("Convert", data.SubString(0,3), convert) == FALSE)
            {
                data = data.SubString(0,3); ///< Not found in old symbol table. it's a new symbol, use first 3 chars.
                if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures )
                {
                    UFC::AnsiString Value;
                    Value.Printf("FI%s", data.c_str() );
                    data = Value;
                }
            }
            else
                data = convert;
            data.PadThis( 7, ' ' );
            if( Msg->GetMarket() == nsOrderMessageDefine::mTWOptions )
            {
                data[6] = symbolDeCompose.FCP_Code2;
                UFC::BufferedLog::DebugPrintf( " Commod 2:[%s]", data.c_str() );
            }
            memcpy(okd.COMMODITY_ID2_2,data.c_str(),7);
            if( symbolDeCompose.FBS_Code1 == 'B')
                okd.BUY_SELL_KIND1[0] = '1';
            else
                okd.BUY_SELL_KIND1[0] = '2';
            if( symbolDeCompose.FBS_Code2 == 'B')
                okd.BUY_SELL_KIND2[0] = '1';
            else
                okd.BUY_SELL_KIND2[0] = '2';
            ///< Fill SETTLEMENT_MONTH1_2
            data.Printf("%s",symbolDeCompose.FSettleMonth2);
            data.PadThis(6,'0');
            memcpy(okd.SETTLEMENT_MONTH2_2,data.c_str(),6);
            ///< Fill STRIKE_PRICE1_2
            data.Printf("%s",symbolDeCompose.FExcercisePrice2);
            data.PadThis(9,'0');
            memcpy(okd.STRIKE_PRICE2_2,data.c_str(),9);
        }
        UFC::BufferedLog::DebugPrintf( " Side:[%c]", okd.BUY_SELL_KIND[0] );
        UFC::BufferedLog::DebugPrintf( " Leg1 Side:[%c]", okd.BUY_SELL_KIND1[0] );
        UFC::BufferedLog::DebugPrintf( " Leg2 Side:[%c]", okd.BUY_SELL_KIND2[0] );
        ///< Fill PRICE1_1
        if( ( okd.EXEC_TYPE[0] == 'F' || okd.EXEC_TYPE[0] == '6' ) &&
                (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle || symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesSingle) )
            PriceDouble = Msg->GetPrice();
        else
            PriceDouble = Msg->GetLegPrice1();
        //FractionalPart = modf( PriceDouble, &IntPart );
        //data.Printf("%06d%06d",(Int32)IntPart, (Int32)(1000000*FractionalPart));
        data.Printf("%012lld", DoubleToInt64( PriceDouble, 6 ) );
        UFC::BufferedLog::DebugPrintf( " OKD-PRICE1-1:[%s]", data.c_str() );
        memcpy(okd.PRICE1_1,data.c_str(),12);
        ///< Fill PRICE2_1
        if( ( okd.EXEC_TYPE[0] == 'F' || okd.EXEC_TYPE[0] == '6' )
                && (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle || symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesSingle) )
            PriceDouble = 0;
        else
            PriceDouble = Msg->GetLegPrice2();
        //FractionalPart = modf( PriceDouble, &IntPart );
        //data.Printf("%06d%06d",(Int32)IntPart, (Int32)(1000000*FractionalPart));
        data.Printf("%012lld", DoubleToInt64( PriceDouble, 6 ) );
        UFC::BufferedLog::DebugPrintf( " OKD-PRICE2-2:[%s]", data.c_str() );
        memcpy(okd.PRICE2_2,data.c_str(),12);
        ///< Fill PRICE_D
        if( okd.EXEC_TYPE[0] == 'F' && (symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle || symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesSingle) )
            PriceDouble = 0;
        else if( okd.EXEC_TYPE[0] == '0' )
            PriceDouble = 0;
        else
            PriceDouble = Msg->GetPrice();
        //FractionalPart = modf( PriceDouble, &IntPart );
        data.Printf("%+013lld", DoubleToInt64( PriceDouble, 6 ) );
        UFC::BufferedLog::DebugPrintf( " PRICE-D:[%s]", data.c_str() );
        memcpy( okd.PRICE_D, data.c_str(), 13);
    }
    ///< Fill AE_ID
    data.Printf("%s",Msg->GetAE());
    data.PadThis(4,' ');
    memcpy(okd.AE_ID,data.c_str(),4);
    ///< Fill DAY_TRADE_ID
    if( okd.OPEN_OFFSET_KIND[0] == 'D' )    
        okd.DAY_TRADE_ID[0] = 'D';
    ///< Fill IB_NO
    data.Printf("%s",Msg->GetLINBRN());
    if( data.Length() > 3 )
        data = data.SubString(data.Length() - 3,3);
    data.PadThis(3,' ');
    memcpy(okd.IB_NO,data.c_str(),3);
    ///< Skip DOS_IN_KIND
    ///< Skip NET_NO
    ///< SUB_INVERSTOR_ACNO
    if( userData.Length() >= 7 )
    {
        UFC::AnsiString SubAccount( userData.SubString( 0, 7 ) );
        UFC::BufferedLog::DebugPrintf( " SubAccount:[%s]", SubAccount.c_str() );
        memcpy( okd.SUB_INVESTOR_ACNO, SubAccount.c_str(), 7 );
    }
    ///< FILL ODR_PRICE
    if( type == 3 )
        data.Printf( "%+09d", UFC::DoubleToInt( Msg->GetPrice(), 3 ) );
    else
        data.Printf( "%+09d", UFC::DoubleToInt( Msg->GetLastPx(), 3 ) );
    UFC::BufferedLog::DebugPrintf( " ODR-PRICE:[%s]", data.c_str() );
    memcpy(okd.ODR_PRICE,data.c_str(),9);
    ///< FILL CLEAR_MEMBER_ID
    data.Printf("%s", Msg->GetCMID());
    data.PadThis( 4,'0');
    memcpy(okd.CLEAR_MEMBER_ID,data.c_str(),4);
    ///< Skip FILLER
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour  )
    {
        FOffHourLog->WriteString( (char*)&okd );
        FOffHourLog->Flush();
    }
    else
    {
        FNormalLog->WriteString( (char*)&okd);
        FNormalLog->Flush();
    }        
    Send((char*)&okd, etTAIFEXConfirm);
}
//------------------------------------------------------------------------------
void MLExecConnection::RejectToBackend( TExecutionReportMessage* Msg )
{
    
    UFC::BufferedLog::DebugPrintf( " Reject To Backend:" );
    UFC::BufferedLog::DebugPrintf( " rpt seq:[%d]", Msg->GetReportSequence() );
    if(  Msg->GetReportSequence() == 0 )
    {
        int Status = UFC::AnsiString(Msg->GetStatusCode()).ToInt();
        if( Status > 82 && Status < 98 ) ///< Skip Speedy Error
        {
            UFC::BufferedLog::DebugPrintf( " Status[%d] Skip execution rejected by Speedy.", Status );
            return;           
        }    
    }
    
    UFC::AnsiString OrderID = Msg->GetOrderID();
    UFC::AnsiString Symbol = Msg->GetSymbol();

    if( Symbol == "     " )
        Symbol = *(FSymbolTable.GetObjectByKey( OrderID ));
    else
        AddToSymbolTable( OrderID, Symbol );
    UFC::BufferedLog::DebugPrintf( " Symbol:[%s]", Symbol.c_str() );

    OKD okd;
    InitialReport( &okd );

    //memset( &okd, ' ',  400 );
    okd.END = 0;
    UFC::UDateTime time;
    ///< Fill ODK-DATE
    time.setCurrent();
    UFC::AnsiString dateStr,timeStr;
    dateStr.Printf("%04d%02d%02d", time.getYear(),time.getMonth(),time.getDate());
    timeStr.Printf("%02d%02d%02d%04d",time.getHour(), time.getMinute(), time.getSecond(),time.getMillisecond());
    ///< Fill ODK-MSG-SEQNUM
    sn++;
    UFC::AnsiString data;
    data.Printf("%010d",sn);
    memcpy( okd.DATE,dateStr.c_str(),8);
    memcpy( okd.MSG_SEQNUM,data.c_str(), 10 );
    memcpy( okd.MSG_DATE,dateStr.c_str(), 8 );
    memcpy( okd.MSG_TIME,timeStr.c_str(), 10 );
    ///< FILL ODK-MSG-TYPE  102:Execution 103:Reject 108:Quote request.(Not support yet)
    memcpy(okd.MSG_TYPE,"103",3);
    switch( Msg->GetTMPExecType() )
    {
        case nsOrderMessageDefine::tetNew:      okd.EXEC_TYPE[0] = '0';break;
        case nsOrderMessageDefine::tetCanceled: okd.EXEC_TYPE[0] = '4'; break;
        case nsOrderMessageDefine::tetReplaced: 
                                                if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                                                    Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2  )
                                                    okd.EXEC_TYPE[0] = 'M';
                                                else
                                                    okd.EXEC_TYPE[0] = '5';
                                                break;
        default : 
            UFC::BufferedLog::Printf( " CxlRejResponseTo:[%d]", Msg->GetCxlRejResponseTo() );
            switch( Msg->GetCxlRejResponseTo() )
            {
                case nsOrderMessageDefine::crrNew:     okd.EXEC_TYPE[0] = '0';break;
                case nsOrderMessageDefine::crrCancel:  okd.EXEC_TYPE[0] = '4';break;
                case nsOrderMessageDefine::crrReplace: if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                                                           Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2  )
                                                           okd.EXEC_TYPE[0] = 'M';
                                                       else
                                                           okd.EXEC_TYPE[0] = '5';
                                                       break;                         
                case nsOrderMessageDefine::crrOrderStatus: okd.EXEC_TYPE[0] = 'I';break;
                default: okd.EXEC_TYPE[0] = '0'; break;
            }
            break;
    }
    ///< Fill SESSION_ID
    data.Printf("%05d",UFC::AnsiString(Msg->GetPVC()).ToInt());
    data.PadThis(5,' ');
    memcpy(okd.SESSION_ID,data.c_str(),5);
    ///< Fill STATUS_CODE
    data.Printf("%03d",Msg->GetTMPStstusCode());
    data.PadThis(3,'0');
    memcpy( okd.STATUS_CODE,data.c_str(), 3 );
    ///< Fill CLEAR_MEMBER_ID_NO
    int CMIndex = TMP::TMPMessage::GetCMID( Msg->GetCMID(), Msg->GetBrokerID());
    data.Printf("%05d", CMIndex );
    data.PadThis( 5,'0');
    memcpy( okd.CLEAR_MEMBER_ID_NO, data.c_str(),5);
    ///< Fill BROKER_ID_NO.
    int BrokerIndex = TMP::TMPMessage::GetBrokerID( Msg->GetBrokerID());
    data.Printf("%05d", BrokerIndex );
    data.PadThis( 5,'0');
    memcpy( okd.BROKER_ID_NO, data.c_str(),5);
    ///< Fill ORDER_NO
    data.Printf("%s",Msg->GetOrderID());
    data.PadThis(5,' ');
    memcpy(okd.ORDER_NO,data.c_str(),5);
    ///< Fill ORRD_ID #### Not use ####
    memcpy(okd.ORD_ID,"0000000",7);         ///< OrderID (Internal use)
    ///< Fill USER_DEFINE UserData 8-15 X(8)
    UFC::AnsiString userData,AE;
    Int32 NID = 0;
    
    ///< Fill USER_DEFINE UserData 8-15 X(8)
    if( IsProxyUserData( Msg->GetData(), AE, NID, userData ) == TRUE )    
        UFC::BufferedLog::DebugPrintf( " Proxy UserData:[%s]", userData.c_str());
    else
        UFC::BufferedLog::DebugPrintf( " Gateway UserData:[%s]", userData.c_str());
    userData.PadThis( 15, ' ' );
    UFC::AnsiString OKD_USER_DEFINE( userData.SubString( 7, 8 ) );
    UFC::BufferedLog::DebugPrintf( " OKD_USER_DEFINE:[%s]", OKD_USER_DEFINE.c_str() );
    memcpy( okd.USER_DEFINE, OKD_USER_DEFINE.c_str(), 8 ); ///< UserDefine (Internal use)
    ///< Fill SYBMOL_TYPE support '2' now.( Text mode )
    okd.SYBMOL_TYPE[0] = '2';
    data.Printf("%s",Symbol.c_str() );
    data.PadThis( 20,' ');
    memcpy(okd.TR_ID,data.c_str(),20);    
    ///< Fill INVESTOR_ACNO
    data.Printf("%s",Msg->GetAccount());
    data.PadThis(7,' ');
    memcpy(okd.INVESTOR_ACNO,data.c_str(),7);
    ///< Fill INVESTOR_FLAG
    if( Msg->GetExecType() == nsOrderMessageDefine::etQuoteAccept )
        data.Printf( "%s", "8" );
    else
        data.Printf("%s",Msg->GetAccountFlag());
    data.PadThis(1,' ');
    memcpy(okd.INVESTOR_FLAG,data.c_str(), 1);
    UFC::BufferedLog::DebugPrintf( " INVESTOR-FLAG:[%s]", data.c_str() );
    ///< Fill BUY_SELL_KIND
    char BS_Code = ' ';
    switch(Msg->GetSide())
    {
        case nsOrderMessageDefine::sBuy:
            okd.BUY_SELL_KIND[0] = '1';
            BS_Code = 'B';
            break;
        case nsOrderMessageDefine::sSell:
            okd.BUY_SELL_KIND[0] = '2';
            BS_Code = 'S';
            break;
        default:
            break;
    }    
    ///< Fill ORDER_DATE,LAST_DATE
    memcpy(okd.ORDER_DATE,dateStr.c_str(),8);
    memcpy(okd.LAST_DATE,dateStr.c_str(),8);
    
    ///< Fill ORDER_TIME,ORDER_MIL,LAST_TIME,LAST_MIL
    memcpy(okd.ORDER_TIME,timeStr.c_str(),10);
    memcpy(okd.LAST_TIME,timeStr.c_str(),10);
    ///< Fill TARGET_ID
    okd.TARGET_ID[0] = '4';
    ///< Fill UNIQUE_ID
    data.Printf("%010d",Msg->GetTMPUniqueID());
    memcpy(okd.UNIQUE_ID,data.c_str(),10);
    UFC::BufferedLog::DebugPrintf( " TMP UniqueID:[%010d]", Msg->GetTMPUniqueID() );
    ///< Fill SEQNO
    data.Printf("%010d",Msg->GetReportSequence());
    memcpy(okd.SEQNO,data.c_str(),10);
    UFC::BufferedLog::DebugPrintf( " ReportSequence:[%010d]", Msg->GetReportSequence() );
    ///< Fill PROTOCOL_TYPE always 1: TMP
    okd.PROTOCOL_TYPE[0] = '1';
    ///< Fill BROKER_ID
    data.Printf("%s",Msg->GetBrokerID());
    data.PadThis(7,' ');
    memcpy(okd.BROKER_ID,data.c_str(),7);
    ///< Fill Multi-leg fields.
    
    //double PriceDouble;

    BO_SymbolDeCompose symbolDeCompose( Msg->GetSymbol(), BS_Code );
    if( symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stUnKnown ) ///< De compose failed
    {
        memset(okd.SETTLEMENT_MONTH1_1,'0',15);
        memset(okd.PRICE1_1,'0',12);
        memset(okd.SETTLEMENT_MONTH2_2,'0',15);
        memset(okd.PRICE2_2,'0',12);        
        memcpy(okd.PRICE_D,"+000000000000",13);
    }
    else
    {
        ///< Fill COMMODITY_ID1_1
        data.Printf("%s",symbolDeCompose.FComm1);
        UFC::AnsiString convert;
        if (pConfig->GetValue("Convert", data.SubString(0,3), convert) == FALSE)
        {
            data = data.SubString(0,3); ///< Not found in old symbol table. it's a new symbol, use first 3 chars.
            if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures )
            {
                UFC::AnsiString Value;
                Value.Printf("FI%s", data.c_str() );
                data = Value;
            }
        }
        else
            data = convert; ///< Example : convert TXF to FITX
        data.PadThis( 7, ' ');
        if( Msg->GetMarket() == nsOrderMessageDefine::mTWOptions )
        {
            data[6] = symbolDeCompose.FCP_Code1;
            UFC::BufferedLog::DebugPrintf( " Commod 1:[%s]", data.c_str() );
        }
        memcpy(okd.COMMODITY_ID1_1,data.c_str(),7);
        ///< Fill SETTLEMENT_MONTH1_1
        data.Printf( "%s", symbolDeCompose.FSettleMonth1 );
        data.PadThis( 6,'0');
        memcpy(okd.SETTLEMENT_MONTH1_1,data.c_str(),6);
        ///< Fill STRIKE_PRICE1_1
        data.Printf("%s",symbolDeCompose.FExcercisePrice1);
        data.PadThis(9,'0');
        UFC::BufferedLog::DebugPrintf( " Strike Price:[%s]", data.c_str() );
        memcpy(okd.STRIKE_PRICE1_1,data.c_str(),9);
        ///< Fill second leg info.
        if( symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stSingle || symbolDeCompose.FStrategicType == BO_SymbolDeCompose::stFuturesSingle )
        {
            ///< It's a single order.
            memset( okd.COMMODITY_ID2_2,' ', 7 );
            memset( okd.SETTLEMENT_MONTH2_2,'0', 6 );
            memset( okd.STRIKE_PRICE2_2,'0', 9 );
            okd.BUY_SELL_KIND1[0] = '0';
            okd.BUY_SELL_KIND2[0] = '0';
        }
        else
        {
            ///< Fill COMMODITY_ID1_2
            data.Printf("%s",symbolDeCompose.FComm2);
            if (pConfig->GetValue("Convert", data.SubString(0,3), convert) == FALSE)
            {
                data = data.SubString(0,3); ///< Not found in old symbol table. it's a new symbol, use first 3 chars.
                if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures )
                {
                    UFC::AnsiString Value;
                    Value.Printf("FI%s", data.c_str() );
                    data = Value;
                }
            }
            else
                data = convert;
            data.PadThis( 7, ' ' );
            if( Msg->GetMarket() == nsOrderMessageDefine::mTWOptions )
            {
                data[6] = symbolDeCompose.FCP_Code2;
                UFC::BufferedLog::DebugPrintf( " Commod 2:[%s]", data.c_str() );
            }
            memcpy(okd.COMMODITY_ID2_2,data.c_str(),7);
            if( symbolDeCompose.FBS_Code1 == 'B')
                okd.BUY_SELL_KIND1[0] = '1';
            else
                okd.BUY_SELL_KIND1[0] = '2';
            if( symbolDeCompose.FBS_Code2 == 'B')
                okd.BUY_SELL_KIND2[0] = '1';
            else
                okd.BUY_SELL_KIND2[0] = '2';
            ///< Fill SETTLEMENT_MONTH1_2
            data.Printf("%s",symbolDeCompose.FSettleMonth2);
            data.PadThis(6,'0');
            memcpy(okd.SETTLEMENT_MONTH2_2,data.c_str(),6);
            ///< Fill STRIKE_PRICE1_2
            data.Printf("%s",symbolDeCompose.FExcercisePrice2);
            data.PadThis(9,'0');
            memcpy(okd.STRIKE_PRICE2_2,data.c_str(),9);
        }        
    }
    ///< Fill AE_ID
    data.Printf("%s",Msg->GetAE());
    data.PadThis(4,' ');
    memcpy(okd.AE_ID,data.c_str(),4);
    ///< Fill DAY_TRADE_ID
    if( okd.OPEN_OFFSET_KIND[0] == 'D' )    
        okd.DAY_TRADE_ID[0] = 'D';
    ///< Fill IB_NO
    data.Printf("%s",Msg->GetLINBRN());
    if( data.Length() > 3 )
        data = data.SubString(data.Length() - 3,3);
    data.PadThis(3,' ');
    memcpy(okd.IB_NO,data.c_str(),3);
    ///< Skip DOS_IN_KIND
    ///< Skip NET_NO
    ///< SUB_INVERSTOR_ACNO
    if( userData.Length() >= 7 )
    {
        UFC::AnsiString SubAccount( userData.SubString( 0, 7 ) );
        UFC::BufferedLog::DebugPrintf( " SubAccount:[%s]", SubAccount.c_str() );
        memcpy( okd.SUB_INVESTOR_ACNO, SubAccount.c_str(), 7 );
    }
    ///< FILL ODR_PRICE
    data.Printf( "%+09d", UFC::DoubleToInt( Msg->GetLastPx(), 3 ) );
    UFC::BufferedLog::DebugPrintf( " ODR-PRICE:[%s]", data.c_str() );
    memcpy(okd.ODR_PRICE,data.c_str(),9);
    ///< FILL CLEAR_MEMBER_ID
    data.Printf("%s", Msg->GetCMID());
    data.PadThis( 4,'0');
    UFC::BufferedLog::DebugPrintf( " CMID:[%s]", Msg->GetCMID() );
    memcpy(okd.CLEAR_MEMBER_ID, data.c_str(),4);
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour  )
    {
        FOffHourLog->WriteString((char*)&okd );
        FOffHourLog->Flush();
    }
    else
    {
        FNormalLog->WriteString( (char*)&okd );            
        FNormalLog->Flush();
    }
    ///< Skip FILLER
    Send((char*)&okd, etTAIFEXConfirm);
}
//------------------------------------------------------------------------------
void MLExecConnection::InitialReport( okdata* data )
{
    memset( data,  ' ', 400 );
    memset( data->DATE, '0', 8 );
    memset( data->MSG_SEQNUM, '0', 10 );
    memset( data->MSG_DATE, '0', 8 );
    memset( data->MSG_TIME, '0', 8 );
    memset( data->MSG_MIL, '0', 4 );
    memset( data->MSG_TYPE, ' ', 3 );
    memset( data->LNK_BROKER_NO, ' ', 3 );
    memset( data->SESSION_ID, '0', 5 );
    memset( data->STATUS_CODE, ' ', 3 );
    memset( data->EXEC_TYPE, ' ', 1 );
    memset( data->CLEAR_MEMBER_ID_NO, ' ', 5 );
    memset( data->BROKER_ID_NO, ' ', 5 );
    memset( data->ORDER_NO, ' ', 5 );
    memset( data->ORD_ID, '0', 7 );
    memset( data->USER_DEFINE, ' ', 8 );
    memset( data->SYBMOL_TYPE, ' ', 8 );
    memset( data->TR_ID, ' ', 20 );
    memset( data->ORDER_PRICE, '+', 1 );
    memset( data->ORDER_PRICE + 1, '0', 8 );
    memset( data->ORDER_QTY, '0', 4 );
    memset( data->INVESTOR_ACNO, ' ', 7 );
    memset( data->INVESTOR_FLAG, ' ', 1 );
    memset( data->BUY_SELL_KIND, ' ', 1 );
    memset( data->ORDER_TYPE, ' ', 1 );
    memset( data->ORDER_COND, ' ', 1 );
    memset( data->OPEN_OFFSET_KIND, ' ', 1 );
    memset( data->LAST_MATCH_PRICE, '+', 1 );
    memset( data->LAST_MATCH_PRICE + 1, '0', 9 );
    memset( data->LAST_MATCH_QTY, '0', 4 );
    memset( data->TOT_MATCH_PRICE, '+', 1 );
    memset( data->TOT_MATCH_PRICE + 1, '0', 16 );
    memset( data->TOT_MATCH_QTY, '0', 4 );
    memset( data->LEAVES_QTY, '0', 4 );
    memset( data->BEFORE_QTY, '0', 4 );
    memset( data->BUY_SELL_KIND1, ' ', 1 );
    memset( data->BUY_SELL_KIND2, ' ', 1 );
    memset( data->MATCH_PRICE1, '0', 9 );
    memset( data->MATCH_PRICE2, '0', 9 );
    memset( data->MATCH_QTY1, '0', 4 );
    memset( data->MATCH_QTY2, '0', 4 );
    memset( data->ORDER_DATE, '0', 8 );
    memset( data->ORDER_TIME, '0', 6 );
    memset( data->ORDER_MIL, '0', 4 );
    memset( data->LAST_DATE, '0', 8 );
    memset( data->LAST_TIME, '0', 6 );
    memset( data->LAST_MIL, '0', 4 );
    memset( data->TARGET_ID,' ', 1 );
    memset( data->UNIQUE_ID, '0', 10 );
    memset( data->SEQNO, '0', 10 );
    memset( data->PROTOCOL_TYPE, ' ', 1 );
    memset( data->BROKER_ID, ' ', 7 );
    memset( data->COMMODITY_ID1_1, ' ', 7 );
    memset( data->SETTLEMENT_MONTH1_1, '0', 9 );
    memset( data->STRIKE_PRICE1_1, '0', 6 );
    memset( data->PRICE1_1, '0', 12 );
    memset( data->COMMODITY_ID2_2, ' ', 7 );
    memset( data->SETTLEMENT_MONTH2_2, '0', 6 );
    memset( data->STRIKE_PRICE2_2, '0', 9 );
    memset( data->PRICE2_2, '0', 12 );
    memset( data->PRICE_D, '+', 1 );
    memset( data->PRICE_D + 1, '0', 12 );
    memset( data->AE_ID, '0', 4 );
    memset( data->DAY_TRADE_ID, ' ', 1 );
    memset( data->IB_NO, ' ', 3 );
    memset( data->DOS_IN_KIND, ' ', 1 );
    memset( data->NET_NO, ' ', 8 );
    memset( data->SUB_INVESTOR_ACNO, ' ', 7 );
    memset( data->ODR_PRICE, '0', 9 );
    memset( data->CLEAR_MEMBER_ID, ' ', 4 );
    memset( data->ORDER_KIND, ' ', 1 );
    memset( data->FILLER, ' ', 16 );
    data->END = 0;
    UFC::BufferedLog::DebugPrintf( " okd:[%s]", data );
}
//------------------------------------------------------------------------------
