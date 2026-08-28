#include "TNewOrderMessage.h"
//---------------------------------------------------------------------------
TNewOrderMessage::TNewOrderMessage( void )
:TBaseMessage( )
,FMarketDataSource( "999" )
,FExchangeCode( "TAIFEX" )
,FTSEOrderType( "0" )
,FTSEExchangeCode( "0" )
,FTSETradeKind( "2" )
,FTSEObjectBroker( "0000" )
,FIBNO( "" )
,FPrice( 0.0 )
,FStopPx( 0.0 )
,FOrderQty( 0 )
,FStockSeqNo( 0 )
,FDayTrade( 0 )
,FOrderType( nsOrderMessageDefine::otLimit )
,FSide( nsOrderMessageDefine::sNone )
,FTimeInForce( nsOrderMessageDefine::tifNone )
,FPositionEffect( nsOrderMessageDefine::peOpen )
,FCombHedgeFlag( nsOrderMessageDefine::hfSpeculation )
,FOrderSource( nsOrderMessageDefine::osDedicatedLine )
,FTradingSessionID( nsOrderMessageDefine::tsAuto )
,FExpireDate( "" )
,FToMarketOrderIfOpen( FALSE )
{
    SetMessageType( nsOrderMessageDefine::mtNew );
}
//---------------------------------------------------------------------------
TNewOrderMessage::~TNewOrderMessage( void )
{
    UFC::Int32 item_count = FOrderAttributes.ItemCount();
    for (UFC::Int32 i = 0; i < item_count; ++i)
        delete FOrderAttributes.GetItem(i);
}
//---------------------------------------------------------------------------
void TNewOrderMessage::SetPositionEffect( nsOrderMessageDefine::PositionEffectEnum Value )
{
    if( Value == nsOrderMessageDefine::peDayTrade )
        SetDayTrade( true );
    else
        SetDayTrade( false );
    FPositionEffect = Value;
}
//---------------------------------------------------------------------------
void TNewOrderMessage::SetMarketDataSource( const char* Value )
{
    FMarketDataSource = Value;
    FMarketDataSource.PadThis( 3, '9' );
}
//---------------------------------------------------------------------------
void TNewOrderMessage::SetAttribute(const char* Key, const char* Value)
{
    UFC::AnsiString attr_name = Key;    

    if (FOrderAttributes.IsExists(attr_name))
    {
        delete FOrderAttributes.GetObjectByKey(attr_name);
        FOrderAttributes.DeleteByKey(attr_name);
    }
    FOrderAttributes.Add(attr_name, new UFC::AnsiString(Value));
}
//---------------------------------------------------------------------------
BOOL TNewOrderMessage::GetAttribute( UFCType::Int32 index, UFC::AnsiString& key, UFC::AnsiString& value )
{
    UFC::AnsiString* p_value = NULL;
    if (!FOrderAttributes.GetItem(index, key, p_value))
        return FALSE;
    value = *p_value;
    return TRUE;
}
//---------------------------------------------------------------------------
static BOOL IsValidDecimalFormat(const char* decimal_value)
{
    // Note: assume decimal_value is not NULL pointer and decimal_value[0] is not 0
    size_t len = strlen(decimal_value);
    int n_dot = 0;
    for(size_t i=0; i<len; ++i)
    {
        char ith_digit = decimal_value[i];
        if ( ith_digit == '.')
        {
            if (0 == i || len-1 == i || n_dot != 0 )
                return FALSE;
            ++n_dot;
            continue;
        }
        else if ( ith_digit < '0' || ith_digit > '9')
            return FALSE;
    }
    
    return TRUE;
}
//---------------------------------------------------------------------------
BOOL TNewOrderMessage::SetStopLoss(BOOL isTrailingStop, UFCType::Int32 ticks, const char* StopPrice, const char* OrderPrice)
{
    FStopOrderError = "";
    
    if (StopPrice && StopPrice[0] && !IsValidDecimalFormat(StopPrice))
    {
        FStopOrderError = "invalid StopPrice value format.";
        return FALSE;
    }
    
    if (OrderPrice && OrderPrice[0] && !IsValidDecimalFormat(OrderPrice))
    {
        FStopOrderError = "invalid OrderPrice value format.";
        return FALSE;
    }
    
    if (isTrailingStop)
    {
        if (!ticks)
        {
            FStopOrderError = "Ticks shouldn't be 0 for trailing stop!";
            return FALSE;
        }

        FStopLossSetting.AppendPrintf("ts=1&ticks=%d", ticks);
    }
    else
    {
        if (!ticks) // ticks == 0
        {
            if (!StopPrice || !StopPrice[0])
            {
                FStopOrderError = "Either ticks or stop price is required!";
                return FALSE;
            }

            FStopLossSetting.AppendPrintf("spx=%s", StopPrice);
        }
        else
        {
            FStopLossSetting.AppendPrintf("ticks=%d", ticks);
        }
    }

    if (OrderPrice && OrderPrice[0])
        FStopLossSetting.AppendPrintf("&px=%s", OrderPrice);

    /*
    UFC::AnsiString stop_price;
    if (!StopPrice || !StopPrice[0])
        stop_price = "spx=0";
    else
        stop_price.AppendPrintf("spx=%s", StopPrice);
    
    UFC::AnsiString order_price;
    if (!OrderPrice || !OrderPrice[0])
        order_price = "px=0";
    else
        order_price.AppendPrintf("px=%s", OrderPrice);
    
    if (isTrailingStop)
        FStopLossSetting = "ts=1";
    else
        FStopLossSetting = "";
    
    
    FStopLossSetting.AppendPrintf("&ticks=%d&%s&%s", ticks, stop_price.c_str(), order_price.c_str());
    */

    return TRUE;    
}
//---------------------------------------------------------------------------
BOOL TNewOrderMessage::SetTakeProfit(BOOL isTrailingStop, UFCType::Int32 ticks, const char* StopPrice, const char* OrderPrice)
{
    FStopOrderError = "";
    
    if (StopPrice && StopPrice[0] && !IsValidDecimalFormat(StopPrice))
    {
        FStopOrderError = "invalid StopPrice value format.";
        return FALSE;
    }
    
    if (OrderPrice && OrderPrice[0] && !IsValidDecimalFormat(OrderPrice))
    {
        FStopOrderError = "invalid OrderPrice value format.";
        return FALSE;
    }
    
    if (isTrailingStop)
    {
        if (!ticks)
        {
            FStopOrderError = "Ticks shouldn't be 0 for trailing stop!";
            return FALSE;
        }

        FTakeProfitSetting.AppendPrintf("ts=1&ticks=%d", ticks);
    }
    else
    {
        if (!ticks) // ticks == 0
        {
            if (!StopPrice || !StopPrice[0])
            {
                FStopOrderError = "Either ticks or stop price is required!";
                return FALSE;
            }

            FTakeProfitSetting.AppendPrintf("spx=%s", StopPrice);
        }
        else
        {
            FTakeProfitSetting.AppendPrintf("ticks=%d", ticks);
        }        
    }
    
    if (OrderPrice && OrderPrice[0])
        FTakeProfitSetting.AppendPrintf("&px=%s", OrderPrice);
                  
    return TRUE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const char* TNewOrderMessage::GetStopOrderSetting()
{
    if (FStopLossSetting.IsEmpty() && FTakeProfitSetting.IsEmpty() && FNewOrderIfMatchSetting.IsEmpty())
        return "";
    
    if (!FStopLossSetting.IsEmpty())
        FStopOrderSetting = FStopLossSetting;
    FStopOrderSetting += ";";
    
    if (!FTakeProfitSetting.IsEmpty())
        FStopOrderSetting += FTakeProfitSetting;
    FStopOrderSetting += ";";
    
    if (!FNewOrderIfMatchSetting.IsEmpty())
        FStopOrderSetting += FNewOrderIfMatchSetting;
    FStopOrderSetting += ";";
    
    // limit order -> market order if market open
    if (FToMarketOrderIfOpen)
        FStopOrderSetting += "mio=1;";
    else
        FStopOrderSetting += "mio=0;";

    return FStopOrderSetting.c_str();
}
//---------------------------------------------------------------------------
const char* TNewOrderMessage::GetStopOrderError()
{
    if (FStopOrderError.IsEmpty())
        return "";
    return FStopOrderError.c_str();
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::TradingSessionIDEnum TNewOrderMessage::GetTradingSessionID( void )
{
    if( GetMarket( ) == nsOrderMessageDefine::mTWFutures || GetMarket( ) == nsOrderMessageDefine::mTWOptions )
    {
        switch( FTradingSessionID )
        {
            case nsOrderMessageDefine::tsOffHour: return nsOrderMessageDefine::tsOffHour;
            case nsOrderMessageDefine::tsNormal:  return nsOrderMessageDefine::tsNormal;
            case nsOrderMessageDefine::tsAuto:
            default:                              return nsOrderMessageDefine::tsAuto;
        }
    }
    else
    {
        if( FTradingSessionID == nsOrderMessageDefine::tsAuto )
            return nsOrderMessageDefine::tsNormal;
        return FTradingSessionID;
    }
}
//---------------------------------------------------------------------------
const char* TNewOrderMessage::GetExchangeCode( void )
{
    switch( FMarket )
    {
        case nsOrderMessageDefine::mTWFutures:
        case nsOrderMessageDefine::mTWOptions: if( FExchangeCode != "TAIFEX" )
                                                   FExchangeCode = "TAIFEX";
                                               break;
        case nsOrderMessageDefine::mTSE:       if( FExchangeCode != "TWSE" )
                                                   FExchangeCode = "TWSE";
                                               break;
        case nsOrderMessageDefine::mOTC:       if( FExchangeCode != "OTC" )
                                                   FExchangeCode = "OTC";
											   break;
		case nsOrderMessageDefine::mES:        if( FExchangeCode != "ES" )
												   FExchangeCode = "ES";
											   break;
        default:                               break;
    }
    return FExchangeCode.c_str();
}
//---------------------------------------------------------------------------
