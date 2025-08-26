// CANCELORDERMESSAGEIMPL : Implementation of TCancelOrderMessageImpl (CoClass: CancelOrderMessage, Interface: ICancelOrderMessage)

#include <vcl.h>
#pragma hdrstop

#include "CANCELORDERMESSAGEIMPL.H"

/////////////////////////////////////////////////////////////////////////////
// IBaseMessage
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_Instance(VARIANT* Value)
{
	VARIANT MessageInstance;
	MessageInstance.vt = VT_BYREF;
	MessageInstance.byref = &FMessage;
	*Value = MessageInstance;

	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_Account(BSTR* Value)
{
    WideString Account( FMessage.GetAccount() );
    *Value = Account.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_AE(BSTR* Value)
{
    WideString AE( FMessage.GetAE() );
    *Value = AE.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_ClOrdID(BSTR* Value)
{
    WideString ClOrdID( FMessage.GetClOrdID() );
    *Value = ClOrdID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_Market(MarketEnum* Value)
{
    *Value = (MarketEnum)( FMessage.GetMarket() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_MessageType(MessageTypeEnum* Value)
{
    *Value = (MessageTypeEnum)( FMessage.GetMessageType() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_NID(__int64* Value)
{
    *Value = FMessage.GetNID();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_Symbol(BSTR* Value)
{
    WideString Symbol( FMessage.GetSymbol() );
    *Value = Symbol.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_Account(BSTR Value)
{
    AnsiString Account( Value );
    FMessage.SetAccount( Account.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_AE(BSTR Value)
{
    AnsiString AE( Value );
    FMessage.SetAE( AE.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_ClOrdID(BSTR Value)
{
    AnsiString ClOrdID( Value );
    FMessage.SetClOrdID( ClOrdID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_Market(MarketEnum Value)
{
    FMessage.SetMarket( (nsOrderMessageDefine::MarketEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_MessageType(MessageTypeEnum Value)
{
    FMessage.SetMessageType( (nsOrderMessageDefine::MessageTypeEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_NID(__int64 Value)
{
    FMessage.SetNID( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_Symbol(BSTR Value)
{
    AnsiString Symbol( Value );
    FMessage.SetSymbol( Symbol.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_Value(BSTR Name, BSTR* Value)
{
	AnsiString ValName( Name );
	UFC::AnsiString ValValue( "" );
	FMessage.GetValue( ValName.c_str(), ValValue );
	WideString WideValue( ValValue.c_str() );

	*Value = WideValue.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_Value(BSTR Name, BSTR Value)
{
	AnsiString ValName( Name );
	AnsiString ValValue( Value );

	FMessage.SetValue( ValName.c_str(), ValValue.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_AccountFlag(BSTR* Value)
{
	WideString AccountFlag( FMessage.GetAccountFlag() );
	*Value = AccountFlag.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_AccountFlag(BSTR Value)
{
	AnsiString AccountFlag( Value );
	FMessage.SetAccountFlag( AccountFlag.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_GatewayData(BSTR* Value)
{
	WideString GDD( FMessage.GetGatewayData() );
	*Value = GDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_GatewayData(BSTR Value)
{
	AnsiString GDD( Value );

	FMessage.SetGatewayData( GDD.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_Data(BSTR* Value)
{
	WideString UDD( FMessage.GetUserData() );
	*Value = UDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_Data(BSTR Value)
{
	AnsiString UDD( Value );

	FMessage.SetUserData( UDD.c_str() );
	return S_OK;
};
/////////////////////////////////////////////////////////////////////////////
// TCancelOrderMessageImpl
//------------------------------------------------------------------------------
TCancelOrderMessageImpl::TCancelOrderMessageImpl()
{
}
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_EventType(EventTypeEnum* Value)
{
    *Value = (EventTypeEnum)( FMessage.GetEventType() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_MaturityMonthYear(BSTR* Value)
{
    WideString MaturityMonthYear( FMessage.GetMaturityMonthYear() );
    *Value = MaturityMonthYear.Copy();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_OrderID(BSTR* Value)
{
    WideString OrderID( FMessage.GetOrderID() );
    *Value = OrderID.Copy();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_OrderType(OrderTypeEnum* Value)
{
    *Value = (OrderTypeEnum)( FMessage.GetOrderType() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_OrigClOrdID(BSTR* Value)
{
    WideString OrigClOrdID( FMessage.GetOrigClOrdID() );
    *Value = OrigClOrdID.Copy();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_Side(SideEnum* Value)
{
    *Value = (SideEnum)( FMessage.GetSide() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_StrikePrice(double* Value)
{
    *Value = FMessage.GetStrikePrice();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_TimeInForce(TimeInForceEnum* Value)
{
    *Value = (TimeInForceEnum)( FMessage.GetTimeInForce() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_EventType(EventTypeEnum Value)
{
    FMessage.SetEventType( (nsOrderMessageDefine::EventTypeEnum)Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_MaturityMonthYear(BSTR Value)
{
    AnsiString MaturityMonthYear( Value );
    FMessage.SetMaturityMonthYear( MaturityMonthYear.c_str() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_OrderID(BSTR Value)
{
    AnsiString OrderID( Value );
    FMessage.SetOrderID( OrderID.c_str() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_OrderType(OrderTypeEnum Value)
{
    FMessage.SetOrderType( (nsOrderMessageDefine::OrderTypeEnum)Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_OrigClOrdID(BSTR Value)
{
    AnsiString OrigClOrdID( Value );
    FMessage.SetOrigClOrdID( OrigClOrdID.c_str() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_Side(SideEnum Value)
{
    FMessage.SetSide( (nsOrderMessageDefine::SideEnum)Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_StrikePrice(double Value)
{
    FMessage.SetStrikePrice( Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_TimeInForce(TimeInForceEnum Value)
{
    FMessage.SetTimeInForce( (nsOrderMessageDefine::TimeInForceEnum)Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_Price(double* Value)
{
	*Value = FMessage.GetPrice();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_Price(double Value)
{
	FMessage.SetPrice( Value );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_CancelNID(int* Value)
{
	*Value = FMessage.GetCNID();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_CancelNID(int Value)
{
	FMessage.SetCNID( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_TSEExchangeCode(BSTR* Value)
{
	WideString ExCode( FMessage.GetTSEExchangeCode() );
	*Value = ExCode.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_TSEObjectBroker(BSTR* Value)
{
	WideString ObjBroker( FMessage.GetTSEObjectBroker() );
	*Value = ObjBroker.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_TSEOrderType(BSTR* Value)
{
	WideString OrdType( FMessage.GetTSEOrderType() );
	*Value = OrdType.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_TSETradeKind(BSTR* Value)
{
	WideString TSETradeKind( FMessage.GetTSETradeKind() );
	*Value = TSETradeKind.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_TSEExchangeCode(BSTR Value)
{
	AnsiString ExchangeCode( Value );
	FMessage.SetTSEExchangeCode( ExchangeCode.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_TSEObjectBroker(BSTR Value)
{
	AnsiString TSEObjectBroker( Value );
	FMessage.SetTSEObjectBroker( TSEObjectBroker.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_TSEOrderType(BSTR Value)
{
	AnsiString TSEOrderType( Value );
	FMessage.SetTSEOrderType( TSEOrderType.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_TSETradeKind(BSTR Value)
{
	AnsiString TSETradeKind( Value );
	FMessage.SetTSETradeKind( TSETradeKind.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_TradingSessionID(TradingSessionIDEnum* Value)















{
	*Value = (TradingSessionIDEnum)( FMessage.GetTradingSessionID() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_TradingSessionID(TradingSessionIDEnum Value)















{
	FMessage.SetTradingSessionID( (nsOrderMessageDefine::TradingSessionIDEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_TSEStockSeqNo(long* Value)
{
	*Value = FMessage.GetStockSeqNo();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_TSEStockSeqNo(long Value)
{	FMessage.SetStockSeqNo( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_BrokerID(BSTR* Value)
{
	WideString BrokerID( FMessage.GetBrokerID() );
	*Value = BrokerID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_BrokerID(BSTR Value)
{
	AnsiString BrokerID( Value );
	FMessage.SetBrokerID( BrokerID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_ExchangeCode(BSTR* Value)
{
	WideString ExchangeCode( FMessage.GetExchangeCode() );
	*Value = ExchangeCode.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_ExchangeCode(BSTR Value)
{
	AnsiString ExchangeCode( Value );
	FMessage.SetExchangeCode( ExchangeCode.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_AENO(BSTR* Value)
{
	WideString AENO( FMessage.GetAENO() );
	*Value = AENO.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_IBNO(BSTR* Value)
{
	WideString IBNO( FMessage.GetIBNO() );
	*Value = IBNO.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_AENO(BSTR Value)
{
	AnsiString AENO( Value );
	FMessage.SetAENO( AENO.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_IBNO(BSTR Value)
{
	AnsiString IBNO( Value );
	FMessage.SetIBNO( IBNO.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_MarketDataSource(BSTR* Value)
{
	WideString MDSrc( FMessage.GetMarketDataSource() );
	*Value = MDSrc.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------

STDMETHODIMP TCancelOrderMessageImpl::get_OrderSource(OrderSourceEnum* Value)
{
	*Value = (OrderSourceEnum)( FMessage.GetOrderSource() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_MarketDataSource(BSTR Value)
{
	AnsiString MDSrc( Value );
	FMessage.SetMarketDataSource( MDSrc.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_OrderSource(OrderSourceEnum Value)
{
	FMessage.SetOrderSource( (nsOrderMessageDefine::OrderSourceEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_EUREX2TAIFEX(BSTR* Value)
{
	WideString EUCD( FMessage.GetEUCD() );
	*Value = EUCD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::get_Group(long* Value)
{
	*Value = FMessage.GetGroup();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_EUREX2TAIFEX(BSTR Value)
{
	AnsiString EUCD( Value );
	FMessage.SetEUCD( EUCD.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TCancelOrderMessageImpl::set_Group(long Value)
{
	FMessage.SetGroup( Value );
	return S_OK;
};
//------------------------------------------------------------------------------



