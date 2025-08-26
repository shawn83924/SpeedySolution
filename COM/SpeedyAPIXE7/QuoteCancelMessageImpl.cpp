// ---------------------------------------------------------------------------
// QUOTECANCELMESSAGEIMPL : Implementation of TQuoteCancelMessageImpl (CoClass: QuoteCancelMessage, Interface: IQuoteCancelMessage)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "QuoteCancelMessageImpl.h"
// ---------------------------------------------------------------------------
// TQuoteCancelMessageImpl
// ---------------------------------------------------------------------------
__fastcall TQuoteCancelMessageImpl::TQuoteCancelMessageImpl()
{
}
// ---------------------------------------------------------------------------
// TQuoteCancelMessageImpl
// ---------------------------------------------------------------------------
__fastcall TQuoteCancelMessageImpl::TQuoteCancelMessageImpl(const System::_di_IInterface Controller)
							  : inherited(Controller)
{
}
// ---------------------------------------------------------------------------
// TQuoteCancelMessageImpl
// ---------------------------------------------------------------------------
__fastcall TQuoteCancelMessageImpl::TQuoteCancelMessageImpl(Comobj::TComObjectFactory* Factory,
									  const System::_di_IInterface Controller)
							  : inherited(Factory, Controller)
{
}
// ---------------------------------------------------------------------------
// TQuoteCancelMessageImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TQuoteCancelMessageImpl>(Comserv::GetComServer(),
						   __classid(TQuoteCancelMessageImpl),
						   CLSID_QuoteCancelMessage,
						   Comobj::ciMultiInstance,
#if defined(USING_MTA)
						   Comobj::tmFree );
#else
						   Comobj::tmApartment );
#endif
}
#pragma startup createFactory 32
/////////////////////////////////////////////////////////////////////////////
// IBaseMessage
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_Instance(VARIANT* Value)
{
	VARIANT MessageInstance;
	MessageInstance.vt = VT_BYREF;
	MessageInstance.byref = &FMessage;
	*Value = MessageInstance;

	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_Account(BSTR* Value)
{
	WideString Account( FMessage.GetAccount() );
	*Value = Account.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_AE(BSTR* Value)
{
	WideString AE( FMessage.GetAE() );
	*Value = AE.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_ClOrdID(BSTR* Value)
{
	WideString ClOrdID( FMessage.GetClOrdID() );
	*Value = ClOrdID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_Market(MarketEnum* Value)
{
	*Value = (MarketEnum)( FMessage.GetMarket() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_MessageType(MessageTypeEnum* Value)
{
	*Value = (MessageTypeEnum)( FMessage.GetMessageType() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_NID(__int64* Value)
{
	*Value = FMessage.GetNID();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_Symbol(BSTR* Value)
{
	WideString Symbol( FMessage.GetSymbol() );
	*Value = Symbol.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_Account(BSTR Value)
{
	AnsiString Account( Value );
	FMessage.SetAccount( Account.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_AE(BSTR Value)
{
	AnsiString AE( Value );
	FMessage.SetAE( AE.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_ClOrdID(BSTR Value)
{
	AnsiString ClOrdID( Value );
	FMessage.SetClOrdID( ClOrdID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_Market(MarketEnum Value)
{
	FMessage.SetMarket( (nsOrderMessageDefine::MarketEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_MessageType(MessageTypeEnum Value)
{
	FMessage.SetMessageType( (nsOrderMessageDefine::MessageTypeEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_NID(__int64 Value)
{
	FMessage.SetNID( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_Symbol(BSTR Value)
{
	AnsiString Symbol( Value );
	FMessage.SetSymbol( Symbol.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_Value(BSTR Name, BSTR* Value)
{
	AnsiString ValName( Name );
	UFC::AnsiString ValValue( "" );
	FMessage.GetValue( ValName.c_str(), ValValue );
	WideString WideValue( ValValue.c_str() );

	*Value = WideValue.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_Value(BSTR Name, BSTR Value)
{
	AnsiString ValName( Name );
	AnsiString ValValue( Value );

	FMessage.SetValue( ValName.c_str(), ValValue.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_AccountFlag(BSTR* Value)
{
	WideString AccountFlag( FMessage.GetAccountFlag() );
	*Value = AccountFlag.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_AccountFlag(BSTR Value)
{
	AnsiString AccountFlag( Value );
	FMessage.SetAccountFlag( AccountFlag.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_GatewayData(BSTR* Value)
{
	WideString GDD( FMessage.GetGatewayData() );
	*Value = GDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_GatewayData(BSTR Value)
{
	AnsiString GDD( Value );

	FMessage.SetGatewayData( GDD.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_Data(BSTR* Value)
{
	WideString UDD( FMessage.GetUserData() );
	*Value = UDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_Data(BSTR Value)
{
	AnsiString UDD( Value );

	FMessage.SetUserData( UDD.c_str() );
	return S_OK;
};
/////////////////////////////////////////////////////////////////////////////
// TQuoteCancelMessageImpl
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_QuoteID(BSTR* Value)
{
	WideString QuoteID( FMessage.GetQuoteID() );
	*Value = QuoteID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_QuoteID(BSTR Value)
{
	AnsiString QuoteID( Value );
	FMessage.SetQuoteID( QuoteID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_BidPrice(double* Value)
{
	*Value = FMessage.GetBidPrice();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_BidPrice(double Value)
{
	FMessage.SetBidPrice( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_AskPrice(double* Value)
{
	*Value = FMessage.GetAskPrice();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_AskPrice(double Value)
{
	FMessage.SetAskPrice( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_MarketDataSource(BSTR* Value)
{
	WideString MDSrc( FMessage.GetMarketDataSource() );
	*Value = MDSrc.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_OrderSource(OrderSourceEnum* Value)
{
	*Value = (OrderSourceEnum)( FMessage.GetOrderSource() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_MarketDataSource(BSTR Value)
{
	AnsiString MDSrc( Value );
	FMessage.SetMarketDataSource( MDSrc.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_OrderSource(OrderSourceEnum Value)
{
	FMessage.SetOrderSource( (nsOrderMessageDefine::OrderSourceEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::get_TradingSessionID(TradingSessionIDEnum* Value)



















































































{
	*Value = (TradingSessionIDEnum)( FMessage.GetTradingSessionID() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteCancelMessageImpl::set_TradingSessionID(TradingSessionIDEnum Value)



















































































{
	FMessage.SetTradingSessionID( (nsOrderMessageDefine::TradingSessionIDEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------



