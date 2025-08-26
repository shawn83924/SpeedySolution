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
