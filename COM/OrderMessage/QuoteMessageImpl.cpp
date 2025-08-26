// QUOTEMESSAGEIMPL : Implementation of TQuoteMessageImpl (CoClass: QuoteMessage, Interface: IQuoteMessage)

#include <vcl.h>
#pragma hdrstop

#include "QUOTEMESSAGEIMPL.H"

/////////////////////////////////////////////////////////////////////////////
// IBaseMessage
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_Instance(VARIANT* Value)
{
	VARIANT MessageInstance;
	MessageInstance.vt = VT_BYREF;
	MessageInstance.byref = &FMessage;
	*Value = MessageInstance;

	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_Account(BSTR* Value)
{
    WideString Account( FMessage.GetAccount() );
    *Value = Account.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_AE(BSTR* Value)
{
    WideString AE( FMessage.GetAE() );
	*Value = AE.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_ClOrdID(BSTR* Value)
{
	WideString ClOrdID( FMessage.GetClOrdID() );
	*Value = ClOrdID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_Market(MarketEnum* Value)
{
    *Value = (MarketEnum)( FMessage.GetMarket() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_MessageType(MessageTypeEnum* Value)
{
    *Value = (MessageTypeEnum)( FMessage.GetMessageType() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_NID(__int64* Value)
{
    *Value = FMessage.GetNID();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_Symbol(BSTR* Value)
{
    WideString Symbol( FMessage.GetSymbol() );
    *Value = Symbol.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_Account(BSTR Value)
{
    AnsiString Account( Value );
    FMessage.SetAccount( Account.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_AE(BSTR Value)
{
    AnsiString AE( Value );
    FMessage.SetAE( AE.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_ClOrdID(BSTR Value)
{
    AnsiString ClOrdID( Value );
    FMessage.SetClOrdID( ClOrdID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_Market(MarketEnum Value)
{
    FMessage.SetMarket( (nsOrderMessageDefine::MarketEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_MessageType(MessageTypeEnum Value)
{
    FMessage.SetMessageType( (nsOrderMessageDefine::MessageTypeEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_NID(__int64 Value)
{
    FMessage.SetNID( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_Symbol(BSTR Value)
{
    AnsiString Symbol( Value );
    FMessage.SetSymbol( Symbol.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_Value(BSTR Name, BSTR* Value)
{
	AnsiString ValName( Name );
	UFC::AnsiString ValValue( "" );
	FMessage.GetValue( ValName.c_str(), ValValue );
	WideString WideValue( ValValue.c_str() );

	*Value = WideValue.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_Value(BSTR Name, BSTR Value)
{
	AnsiString ValName( Name );
	AnsiString ValValue( Value );

	FMessage.SetValue( ValName.c_str(), ValValue.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_AccountFlag(BSTR* Value)
{
	WideString AccountFlag( FMessage.GetAccountFlag() );
	*Value = AccountFlag.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_AccountFlag(BSTR Value)
{
	AnsiString AccountFlag( Value );
	FMessage.SetAccountFlag( AccountFlag.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_GatewayData(BSTR* Value)
{
	WideString GDD( FMessage.GetGatewayData() );
	*Value = GDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_GatewayData(BSTR Value)
{
	AnsiString GDD( Value );

	FMessage.SetGatewayData( GDD.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_Data(BSTR* Value)
{
	WideString UDD( FMessage.GetUserData() );
	*Value = UDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_Data(BSTR Value)
{
	AnsiString UDD( Value );

	FMessage.SetUserData( UDD.c_str() );
	return S_OK;
};
/////////////////////////////////////////////////////////////////////////////
// TQuoteMessageImpl
//------------------------------------------------------------------------------
TQuoteMessageImpl::TQuoteMessageImpl()
{
}
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_AskPrice(double* Value)
{
    *Value = FMessage.GetAskPrice();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_AskQty(long* Value)
{
    *Value = FMessage.GetAskQty();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_BidPrice(double* Value)
{
    *Value = FMessage.GetBidPrice();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_BidQty(long* Value)
{
    *Value = FMessage.GetBidQty();
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_TimeInForce(TimeInForceEnum* Value)
{
    *Value = (TimeInForceEnum)( FMessage.GetTimeInForce() );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_AskPrice(double Value)
{
    FMessage.SetAskPrice( Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_AskQty(long Value)
{
    FMessage.SetAskQty( Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_BidPrice(double Value)
{
    FMessage.SetBidPrice( Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_BidQty(long Value)
{
    FMessage.SetBidQty( Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_TimeInForce(TimeInForceEnum Value)
{
    FMessage.SetTimeInForce( (nsOrderMessageDefine::TimeInForceEnum)Value );
    return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_QuoteID(BSTR* Value)
{
	WideString QuoteID( FMessage.GetQuoteID() );
	*Value = QuoteID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_QuoteID(BSTR Value)
{
	AnsiString QuoteID( Value );
	FMessage.SetQuoteID( QuoteID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_MarketDataSource(BSTR* Value)
{
	WideString MDSrc( FMessage.GetMarketDataSource() );
	*Value = MDSrc.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::get_OrderSource(OrderSourceEnum* Value)
{
	*Value = (OrderSourceEnum)( FMessage.GetOrderSource() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_MarketDataSource(BSTR Value)
{
	AnsiString MDSrc( Value );
	FMessage.SetMarketDataSource( MDSrc.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteMessageImpl::set_OrderSource(OrderSourceEnum Value)
{
	FMessage.SetOrderSource( (nsOrderMessageDefine::OrderSourceEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------




