// NEWORDERMESSAGEIMPL : Implementation of TNewOrderMessageImpl (CoClass: NewOrderMessage, Interface: INewOrderMessage)

#include <vcl.h>
#pragma hdrstop

#include "NEWORDERMESSAGEIMPL.H"

/////////////////////////////////////////////////////////////////////////////
// IBaseMessage
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_Instance(VARIANT* Value)
{
	VARIANT MessageInstance;
	MessageInstance.vt = VT_BYREF;
	MessageInstance.byref = &FMessage;
	*Value = MessageInstance;

	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_Account(BSTR* Value)
{
    WideString Account( FMessage.GetAccount() );
    *Value = Account.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_AE(BSTR* Value)
{
	WideString AE( FMessage.GetAE() );
    *Value = AE.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_ClOrdID(BSTR* Value)
{
    WideString ClOrdID( FMessage.GetClOrdID() );
    *Value = ClOrdID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_Market(MarketEnum* Value)
{
    *Value = (MarketEnum)( FMessage.GetMarket() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_MessageType(MessageTypeEnum* Value)
{
    *Value = (MessageTypeEnum)( FMessage.GetMessageType() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_NID(__int64* Value)
{
    *Value = FMessage.GetNID();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_Symbol(BSTR* Value)
{
    WideString Symbol( FMessage.GetSymbol() );
    *Value = Symbol.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_Account(BSTR Value)
{
	AnsiString Account( Value );
    FMessage.SetAccount( Account.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_AE(BSTR Value)
{
    AnsiString AE( Value );
    FMessage.SetAE( AE.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_ClOrdID(BSTR Value)
{
    AnsiString ClOrdID( Value );
    FMessage.SetClOrdID( ClOrdID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_Market(MarketEnum Value)
{
    FMessage.SetMarket( (nsOrderMessageDefine::MarketEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_MessageType(MessageTypeEnum Value)
{
    FMessage.SetMessageType( (nsOrderMessageDefine::MessageTypeEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_NID(__int64 Value)
{
    FMessage.SetNID( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_Symbol(BSTR Value)
{
    AnsiString Symbol( Value );
    FMessage.SetSymbol( Symbol.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_Value(BSTR Name, BSTR* Value)
{
	AnsiString ValName( Name );
	UFC::AnsiString ValValue( "" );
	FMessage.GetValue( ValName.c_str(), ValValue );
	WideString WideValue( ValValue.c_str() );

	*Value = WideValue.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_Value(BSTR Name, BSTR Value)
{
	AnsiString ValName( Name );
	AnsiString ValValue( Value );

	FMessage.SetValue( ValName.c_str(), ValValue.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_AccountFlag(BSTR* Value)
{
	WideString AccountFlag( FMessage.GetAccountFlag() );
	*Value = AccountFlag.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_AccountFlag(BSTR Value)
{
	AnsiString AccountFlag( Value );
	FMessage.SetAccountFlag( AccountFlag.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_GatewayData(BSTR* Value)
{
	WideString GDD( FMessage.GetGatewayData() );
	*Value = GDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_GatewayData(BSTR Value)
{
	AnsiString GDD( Value );

	FMessage.SetGatewayData( GDD.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_Data(BSTR* Value)
{
	WideString UDD( FMessage.GetUserData() );
	*Value = UDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_Data(BSTR Value)
{
	AnsiString UDD( Value );

	FMessage.SetUserData( UDD.c_str() );
	return S_OK;
};
/////////////////////////////////////////////////////////////////////////////
// TNewOrderMessageImpl
//------------------------------------------------------------------------------
TNewOrderMessageImpl::TNewOrderMessageImpl()
{
}
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_EventType(EventTypeEnum* Value)
{
    *Value = (EventTypeEnum)( FMessage.GetEventType() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_MaturityMonthYear(BSTR* Value)
{
    WideString MaturityMonthYear( FMessage.GetMaturityMonthYear() );
    *Value = MaturityMonthYear.Copy();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_OrderQty(long* Value)
{
    *Value = FMessage.GetOrderQty();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_OrderType(OrderTypeEnum* Value)
{
    *Value = (OrderTypeEnum)( FMessage.GetOrderType() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_Price(double* Value)
{
    *Value = FMessage.GetPrice();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_Side(SideEnum* Value)
{
    *Value = (SideEnum)( FMessage.GetSide() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_StopPx(double* Value)
{
    *Value = FMessage.GetStopPx();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_StrikePrice(double* Value)
{
    *Value = FMessage.GetStrikePrice();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_TimeInForce(TimeInForceEnum* Value)
{
    *Value = (TimeInForceEnum)( FMessage.GetTimeInForce() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_EventType(EventTypeEnum Value)
{
    FMessage.SetEventType( (nsOrderMessageDefine::EventTypeEnum)Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_MaturityMonthYear(BSTR Value)
{
	AnsiString MaturityMonthYear( Value );
	FMessage.SetMaturityMonthYear( MaturityMonthYear.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_OrderQty(long Value)
{
    FMessage.SetOrderQty( Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_OrderType(OrderTypeEnum Value)
{
    FMessage.SetOrderType( (nsOrderMessageDefine::OrderTypeEnum)Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_Price(double Value)
{
    FMessage.SetPrice( Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_Side(SideEnum Value)
{
    FMessage.SetSide( (nsOrderMessageDefine::SideEnum)Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_StopPx(double Value)
{
    FMessage.SetStopPx( Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_StrikePrice(double Value)
{
    FMessage.SetStrikePrice( Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_TimeInForce(TimeInForceEnum Value)
{
	FMessage.SetTimeInForce( (nsOrderMessageDefine::TimeInForceEnum)Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_PositionEffect(PositionEffectEnum* Value)
{
	*Value = (PositionEffectEnum)( FMessage.GetPositionEffect() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_PositionEffect(PositionEffectEnum Value)
{
	FMessage.SetPositionEffect( (nsOrderMessageDefine::PositionEffectEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_TradingSessionID(TradingSessionIDEnum* Value)















{
	*Value = (TradingSessionIDEnum)( FMessage.GetTradingSessionID() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_TradingSessionID(TradingSessionIDEnum Value)















{
	FMessage.SetTradingSessionID( (nsOrderMessageDefine::TradingSessionIDEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_TSEExchangeCode(BSTR* Value)
{
	WideString ExCode( FMessage.GetTSEExchangeCode() );
	*Value = ExCode.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_TSEObjectBroker(BSTR* Value)
{
	WideString ObjBroker( FMessage.GetTSEObjectBroker() );
	*Value = ObjBroker.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_TSEOrderType(BSTR* Value)
{
	WideString OrdType( FMessage.GetTSEOrderType() );
	*Value = OrdType.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_TSETradeKind(BSTR* Value)
{
	WideString TSETradeKind( FMessage.GetTSETradeKind() );
	*Value = TSETradeKind.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_TSEExchangeCode(BSTR Value)
{
	AnsiString ExchangeCode( Value );
	FMessage.SetTSEExchangeCode( ExchangeCode.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_TSEObjectBroker(BSTR Value)
{
	AnsiString TSEObjectBroker( Value );
	FMessage.SetTSEObjectBroker( TSEObjectBroker.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_TSEOrderType(BSTR Value)
{
	AnsiString TSEOrderType( Value );
	FMessage.SetTSEOrderType( TSEOrderType.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_TSETradeKind(BSTR Value)
{
	AnsiString TSETradeKind( Value );
	FMessage.SetTSETradeKind( TSETradeKind.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_TSEStockSeqNo(long* Value)
{
  *Value = FMessage.GetStockSeqNo();
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_TSEStockSeqNo(long Value)
{
  FMessage.SetStockSeqNo( Value );
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_BrokerID(BSTR* Value)
{
	WideString BrokerID( FMessage.GetBrokerID() );
	*Value = BrokerID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_BrokerID(BSTR Value)
{
	AnsiString BrokerID( Value );
	FMessage.SetBrokerID( BrokerID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_OrderID(BSTR* Value)
{
	WideString OrderID( FMessage.GetOrderID() );
	*Value = OrderID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_OrderID(BSTR Value)
{
	AnsiString OrderID( Value );
	FMessage.SetOrderID( OrderID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_PriceBase(PriceBaseEnum* Value)
{
	*Value = (PriceBaseEnum)( FMessage.GetPxBase() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_PriceBase(PriceBaseEnum Value)
{
	FMessage.SetPxBase( (nsOrderMessageDefine::PriceBaseEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_ExchangeCode(BSTR* Value)
{
	WideString ExchangeCode( FMessage.GetExchangeCode() );
	*Value = ExchangeCode.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_ExchangeCode(BSTR Value)
{
	AnsiString ExchangeCode( Value );
	FMessage.SetExchangeCode( ExchangeCode.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_AENO(BSTR* Value)
{
	WideString AENO( FMessage.GetAENO() );
	*Value = AENO.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_DayTrade(VARIANT_BOOL* Value)
{
  if( FMessage.GetDayTrade() == 0 )
	  *Value = VARIANT_FALSE;
  else
	  *Value = VARIANT_TRUE;
  return S_OK;
};
//------------------------------------------------------------------------------

STDMETHODIMP TNewOrderMessageImpl::get_IBNO(BSTR* Value)
{
	WideString IBNO( FMessage.GetIBNO() );
	*Value = IBNO.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_AENO(BSTR Value)
{
	AnsiString AENO( Value );
	FMessage.SetAENO( AENO.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_DayTrade(VARIANT_BOOL Value)
{
	if( Value == VARIANT_FALSE )
		FMessage.SetDayTrade( 0 );
	else
		FMessage.SetDayTrade( 1 );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_IBNO(BSTR Value)
{
	AnsiString IBNO( Value );
	FMessage.SetIBNO( IBNO.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_MarketDataSource(BSTR* Value)
{
	WideString MDSrc( FMessage.GetMarketDataSource() );
	*Value = MDSrc.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_OrderSource(OrderSourceEnum* Value)
{
	*Value = (OrderSourceEnum)( FMessage.GetOrderSource() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_MarketDataSource(BSTR Value)
{
	AnsiString MDSrc( Value );
	FMessage.SetMarketDataSource( MDSrc.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_OrderSource(OrderSourceEnum Value)
{
	FMessage.SetOrderSource( (nsOrderMessageDefine::OrderSourceEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_EUREX2TAIFEX(BSTR* Value)
{
	WideString EUCD( FMessage.GetEUCD() );
	*Value = EUCD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::get_Group(long* Value)
{
	*Value = FMessage.GetGroup();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_EUREX2TAIFEX(BSTR Value)
{
	AnsiString EUCD( Value );
	FMessage.SetEUCD( EUCD.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewOrderMessageImpl::set_Group(long Value)
{
	FMessage.SetGroup( Value );
	return S_OK;
};
//------------------------------------------------------------------------------


