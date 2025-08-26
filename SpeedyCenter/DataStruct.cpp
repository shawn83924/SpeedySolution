//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "DataStruct.h"
//---------------------------------------------------------------------------
ControlHeader::ControlHeader( char* SubSys,char* Func, char* MsgType )
{
    TDateTime now = TDateTime::CurrentTime();
    AnsiString TimeStr = now.FormatString("hhmmss");

    memcpy(SystemName,SubSys,2);
    memcpy(FunctionCode,Func,2);
    memcpy(MessageType,MsgType,2);
    memcpy(MessageTime,TimeStr.c_str(),6);
    memset(StatusCode,'0',2);
}
//---------------------------------------------------------------------------
ControlHeader::ControlHeader( char* Data )
{
    memcpy(SystemName,Data,2);
    memcpy(FunctionCode,Data+2,2);
    memcpy(MessageType,Data+4,2);
    memcpy(MessageTime,Data+6,6);
    memcpy(StatusCode,Data+12,2);
}
//---------------------------------------------------------------------------
AnsiString ControlHeader::AsString()
{
    return  AnsiString(SystemName,2)+
            AnsiString(FunctionCode,2)+
            AnsiString(MessageType,2)+
            AnsiString(MessageTime,6)+
            AnsiString(StatusCode,2);
}
//---------------------------------------------------------------------------
int ControlHeader::GetSubSysem()
{
    AnsiString Str(SystemName,2);
    return StrToInt(Str);
}
//---------------------------------------------------------------------------
int ControlHeader::GetFunctionCode()
{
    AnsiString Str(FunctionCode,2);
    return StrToInt(Str);
}
//---------------------------------------------------------------------------
int ControlHeader::GetMessageType()
{
    AnsiString Str(MessageType,2);
    return StrToInt(Str);
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
R010::R010( char *FunctionCode )
:ControlHeader("30",FunctionCode,"00")
{
    memset( FcmNo,' ',7);
    memset( OrderNo,' ',5);
    memset( InvestorAcno,' ',7);
    memset( CommodityId,' ',20);
    memset( OrderPrice,' ',9);
    memset( OrderQty,' ',4);
    memset( ClearMemberId,' ',4);
    InvestorFlag = ' ';
    BuySellCode = ' ';
    OrderType = ' ';
    OrderCondition = ' ';
    OpenOffsetFlag = ' ';
}
//---------------------------------------------------------------------------
R010::R010( AnsiString& Data )
:ControlHeader( Data.c_str())
{
    memcpy( this, Data.c_str(), sizeof( R010 ));
}
//---------------------------------------------------------------------------
AnsiString R010::AsString()
{
    AnsiString Result = ControlHeader::AsString()
    + AnsiString(FcmNo,7)
    + AnsiString(OrderNo,5)
    + AnsiString(InvestorAcno,7)
    + AnsiString(InvestorFlag)
    + AnsiString(CommodityId,20)
    + AnsiString(BuySellCode)
    + AnsiString(OrderType)
    + AnsiString(OrderPrice,9)
    + AnsiString(OrderQty,4)
    + AnsiString(OrderCondition)
    + AnsiString(OpenOffsetFlag)
    + AnsiString(ClearMemberId,4);
    return Result;
}
//---------------------------------------------------------------------------
AnsiString R010::Print( void )
{
    AnsiString Result;

    Result =  "期貨商代號:" + AnsiString( FcmNo, 7 )+"\n";
    Result += "委託書編號:" + AnsiString( OrderNo, 5)+"\n";
    Result += "商品代號:"   + AnsiString( CommodityId,20)+"\n";
    Result += "委託價格/價差:" + AnsiString(OrderPrice,9)+"\n";
    Result += "委託合約數:" + AnsiString(OrderQty,4)+"\n";
    Result += "買賣別"+ AnsiString(BuySellCode)+"\n";
    return Result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void R010::SetFCM( AnsiString FCM )
{
    memcpy( FcmNo,FCM.c_str(),FCM.Length());
}
//---------------------------------------------------------------------------
void R010::SetOrderNo( AnsiString OrdeNumber)
{
    memcpy( OrderNo,OrdeNumber.c_str(),OrdeNumber.Length());
}
//---------------------------------------------------------------------------
void R010::SetInvestorAccount( AnsiString Account )
{
    memcpy( InvestorAcno,Account.c_str(),Account.Length());
}
//---------------------------------------------------------------------------
void R010::SetInvestorFlag( char Flag )
{
    InvestorFlag = Flag;
}
//---------------------------------------------------------------------------
void R010::SetCommodityID( AnsiString ProductID )
{
    memcpy( CommodityId, ProductID.c_str(),ProductID.Length());
}
//---------------------------------------------------------------------------
void R010::SetBuySellCode( char BSCode )
{
    BuySellCode = BSCode;
}
//---------------------------------------------------------------------------
void R010::SetOrderType( char Type )
{
    OrderType = Type;
}
//---------------------------------------------------------------------------
void R010::SetOrderPrice( int Price )
{
    AnsiString PriceStr;

    PriceStr.sprintf("%09d",Price);
    memcpy( OrderPrice,PriceStr.c_str(),9);
}
//---------------------------------------------------------------------------
void R010::SetOrderQuantity( int qty )
{
    AnsiString QtyStr;

    QtyStr.sprintf("%04d",qty);
    memcpy( OrderQty,QtyStr.c_str(),4);
}
//---------------------------------------------------------------------------
void R010::SetOrderCondition( char OC )
{
    OrderCondition = OC;
}
//---------------------------------------------------------------------------
void R010::SetOpenOffsetFlag( char OF )
{
    OpenOffsetFlag = OF;
}
//---------------------------------------------------------------------------
void R010::SetClearMemberID( AnsiString CM )
{
     memcpy( ClearMemberId,CM.c_str(),CM.Length());
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
R010F::R010F( char *FunctionCode )
:ControlHeader("30",FunctionCode,"00")
{
    memset( FcmNo,' ',7);
    memset( OrderNo,' ',5);
    memset( InvestorAcno,' ',7);
    memset( CommodityId,' ',20);
    memset( OrderPrice,' ',9);
    memset( OrderQty,' ',4);
    memset( ClearMemberId,' ',4);
    InvestorFlag = ' ';
    BuySellCode = ' ';
    OrderType = ' ';
    OpenOffsetFlag = ' ';
}
//---------------------------------------------------------------------------
R010F::R010F( AnsiString& Data )
:ControlHeader( Data.c_str())
{
    memcpy( this, Data.c_str(), sizeof( R010 ));
}
//---------------------------------------------------------------------------
AnsiString R010F::AsString()
{
    AnsiString Result = ControlHeader::AsString()
    + AnsiString(FcmNo,7)
    + AnsiString(OrderNo,5)
    + AnsiString(InvestorAcno,7)
    + AnsiString(InvestorFlag)
    + AnsiString(CommodityId,7)
    + AnsiString(SettlementMonth,6)
    + AnsiString(StrikePrice,8)
    + AnsiString(BuySellCode)
    + AnsiString(OrderType)
    + AnsiString(OrderPrice,8)
    + AnsiString(OrderQty,4)
    + AnsiString(OpenOffsetFlag)
    + AnsiString(ClearMemberId,4);
    return Result;
}
//---------------------------------------------------------------------------
AnsiString R010F::Print( void )
{
    AnsiString Result;

    Result =  "期貨商代號:" + AnsiString( FcmNo, 7 )+"\n";
    Result += "委託書編號:" + AnsiString( OrderNo, 5)+"\n";
    Result += "商品代號:"   + AnsiString( CommodityId,7)+"\n";
    Result += "委託價格/價差:" + AnsiString(OrderPrice,8)+"\n";
    Result += "委託合約數:" + AnsiString(OrderQty,4)+"\n";
    Result += "買賣別"+ AnsiString(BuySellCode)+"\n";
    return Result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void R010F::SetFCM( AnsiString FCM )
{
    memcpy( FcmNo,FCM.c_str(),7);
}
//---------------------------------------------------------------------------
void R010F::SetOrderNo( AnsiString OrdeNumber)
{
    memcpy( OrderNo,OrdeNumber.c_str(),5);
}
//---------------------------------------------------------------------------
void R010F::SetInvestorAccount( AnsiString Account )
{
    memcpy( InvestorAcno,Account.c_str(),7);
}
//---------------------------------------------------------------------------
void R010F::SetInvestorFlag( char Flag )
{
    InvestorFlag = Flag;
}
//---------------------------------------------------------------------------
void R010F::SetCommodityID( AnsiString ProductID )
{
    memcpy( CommodityId, ProductID.c_str(),ProductID.Length());
}
//---------------------------------------------------------------------------
void R010F::SetBuySellCode( char BSCode )
{
    BuySellCode = BSCode;
}
//---------------------------------------------------------------------------
void R010F::SetOrderType( char Type )
{
    OrderType = Type;
}
//---------------------------------------------------------------------------
void R010F::SetOrderPrice( int Price )
{
    AnsiString PriceStr;

    PriceStr.sprintf("%08d",Price);
    memcpy( OrderPrice,PriceStr.c_str(),8);
}
//---------------------------------------------------------------------------
void R010F::SetSettlementMonth( AnsiString Month )
{
    memcpy( SettlementMonth, Month.c_str(),Month.Length());
}
//---------------------------------------------------------------------------
void R010F::SetStrikePrice( int Price )
{
    AnsiString PriceStr;

    PriceStr.sprintf("%08d",Price);
    memcpy( StrikePrice,PriceStr.c_str(),8);
}
//---------------------------------------------------------------------------
void R010F::SetOrderQuantity( int qty )
{
    AnsiString QtyStr;

    QtyStr.sprintf("%04d",qty);
    memcpy( OrderQty,QtyStr.c_str(),4);
}
//---------------------------------------------------------------------------
void R010F::SetOpenOffsetFlag( char OF )
{
    OpenOffsetFlag = OF;
}
//---------------------------------------------------------------------------
void R010F::SetClearMemberID( AnsiString CM )
{
     memcpy( ClearMemberId,CM.c_str(),4);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
R020::R020( char *Data )
:ControlHeader( "30", "00", "00" )
{
    memcpy( this, Data, sizeof( R020 ));
}
//---------------------------------------------------------------------------
AnsiString R020::Print( void )
{
    AnsiString Result;

    Result =  "期貨商代號:" + AnsiString( FcmNo, 7 )+"\n";
    Result += "委託書編號:" + AnsiString( OrderNo, 5)+"\n";
    Result += "商品代號:"   + AnsiString( CommodityId,20)+"\n";
    Result += "委託價格/價差:" + AnsiString(OrderPrice,9)+"\n";
    Result += "委託合約數:" + AnsiString(OrderQty,4)+"\n";
    Result += "原有效數量:" + AnsiString(BeforeQty,4)+"\n";
    Result += "剩餘數量:"   + AnsiString(AfterQty,4)+"\n";
    Result += "委託接收時間:" + AnsiString(ReceiveTime,8)+"\n";
    Result += "買賣別"+ AnsiString(BuySellCode)+"\n";
    return Result;
}
//---------------------------------------------------------------------------
R070::R070( AnsiString FCM, AnsiString CommodityId )
:ControlHeader("30","01","06")
{
    memset( FFcmNo,' ',7);
    memset( FCommodityId,' ',20);
    memcpy( FFcmNo,FCM.c_str(),FCM.Length());
    memcpy( FCommodityId,CommodityId.c_str(),CommodityId.Length());
}
//---------------------------------------------------------------------------
AnsiString R070::AsString()
{
     AnsiString Result = ControlHeader::AsString()
    + AnsiString(FFcmNo,7)
    + AnsiString(FCommodityId,20);
    return Result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
R090::R090( char *FunctionCode )
:ControlHeader("30",FunctionCode,"08")
{
    memset( FcmNo,' ',7);
    memset( OrderNo,' ',5);
    memset( InvestorAcno,' ',7);
    memset( CommodityId,' ',20);
    memset( BuyPrice,' ',9);
    memset( SellPrice,' ',9);
    memset( BuyQty,' ',4);
    memset( SellQty,' ',4);
    memset( ClearMemberId,' ',4);
    InvestorFlag = ' ';
    OrderCondition = ' ';
    OpenOffsetFlag = ' ';
}
//---------------------------------------------------------------------------
AnsiString R090::AsString()
{
    AnsiString Result = ControlHeader::AsString()
    + AnsiString(FcmNo,7)
    + AnsiString(OrderNo,5)
    + AnsiString(InvestorAcno,7)
    + AnsiString(InvestorFlag)
    + AnsiString(CommodityId,20)
    + AnsiString(BuyPrice,9)
    + AnsiString(SellPrice,9)
    + AnsiString(BuyQty,4)
    + AnsiString(SellQty,4)
    + AnsiString(OrderCondition)
    + AnsiString(OpenOffsetFlag)
    + AnsiString(ClearMemberId,4);
    return Result;
}
//---------------------------------------------------------------------------
void R090::SetFCM( AnsiString FCM )
{
    memcpy( FcmNo,FCM.c_str(),FCM.Length());
}
//---------------------------------------------------------------------------
void R090::SetOrderNo( AnsiString OrdeNumber)
{
    memcpy( OrderNo,OrdeNumber.c_str(),OrdeNumber.Length());
}
//---------------------------------------------------------------------------
void R090::SetInvestorAccount( AnsiString Account )
{
    memcpy( InvestorAcno,Account.c_str(),Account.Length());
}
//---------------------------------------------------------------------------
void R090::SetInvestorFlag( char Flag )
{
    InvestorFlag = Flag;
}
//---------------------------------------------------------------------------
void R090::SetCommodityID( AnsiString ProductID )
{
    memcpy( CommodityId, ProductID.c_str(),ProductID.Length());
}
//---------------------------------------------------------------------------
void R090::SetBuyPrice( int Price )
{
    AnsiString PriceStr;

    PriceStr.sprintf("%09d",Price);
    memcpy( BuyPrice,PriceStr.c_str(),9);
}
//---------------------------------------------------------------------------
void R090::SetBuyQuantity( int qty )
{
    AnsiString QtyStr;

    QtyStr.sprintf("%04d",qty);
    memcpy( BuyQty,QtyStr.c_str(),4);
}
//---------------------------------------------------------------------------
void R090::SetSellPrice( int Price )
{
    AnsiString PriceStr;

    PriceStr.sprintf("%09d",Price);
    memcpy( SellPrice,PriceStr.c_str(),9);
}
//---------------------------------------------------------------------------
void R090::SetSellQuantity( int qty )
{
    AnsiString QtyStr;

    QtyStr.sprintf("%04d",qty);
    memcpy( SellQty,QtyStr.c_str(),4);
}
//---------------------------------------------------------------------------
void R090::SetOrderCondition( char OC )
{
    OrderCondition = OC;
}
//---------------------------------------------------------------------------
void R090::SetOpenOffsetFlag( char OF )
{
    OpenOffsetFlag = OF;
}
//---------------------------------------------------------------------------
void R090::SetClearMemberID( AnsiString CM )
{
     memcpy( ClearMemberId,CM.c_str(),CM.Length());
}
//---------------------------------------------------------------------------

