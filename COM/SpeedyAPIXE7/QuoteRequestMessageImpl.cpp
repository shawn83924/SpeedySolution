// ---------------------------------------------------------------------------
// QUOTEREQUESTMESSAGEIMPL : Implementation of TQuoteRequestMessageImpl (CoClass: QuoteRequestMessage, Interface: IQuoteRequestMessage)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "QuoteRequestMessageImpl.h"
// ---------------------------------------------------------------------------
// TQuoteRequestMessageImpl
// ---------------------------------------------------------------------------
__fastcall TQuoteRequestMessageImpl::TQuoteRequestMessageImpl()
{
}
// ---------------------------------------------------------------------------
// TQuoteRequestMessageImpl
// ---------------------------------------------------------------------------
__fastcall TQuoteRequestMessageImpl::TQuoteRequestMessageImpl(const System::_di_IInterface Controller)
							  : inherited(Controller)
{
}
// ---------------------------------------------------------------------------
// TQuoteRequestMessageImpl
// ---------------------------------------------------------------------------
__fastcall TQuoteRequestMessageImpl::TQuoteRequestMessageImpl(Comobj::TComObjectFactory* Factory,
									  const System::_di_IInterface Controller)
							  : inherited(Factory, Controller)
{
}
// ---------------------------------------------------------------------------
// TQuoteRequestMessageImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TQuoteRequestMessageImpl>(Comserv::GetComServer(),
						   __classid(TQuoteRequestMessageImpl),
						   CLSID_QuoteRequestMessage,
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
STDMETHODIMP TQuoteRequestMessageImpl::get_Instance(VARIANT* Value)
{
	VARIANT MessageInstance;
	MessageInstance.vt = VT_BYREF;
	MessageInstance.byref = &FMessage;
	*Value = MessageInstance;

	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_Account(BSTR* Value)
{
	WideString Account( FMessage.GetAccount() );
	*Value = Account.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_AE(BSTR* Value)
{
	WideString AE( FMessage.GetAE() );
	*Value = AE.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_ClOrdID(BSTR* Value)
{
	WideString ClOrdID( FMessage.GetClOrdID() );
	*Value = ClOrdID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_Market(MarketEnum* Value)
{
	*Value = (MarketEnum)( FMessage.GetMarket() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_MessageType(MessageTypeEnum* Value)
{
	*Value = (MessageTypeEnum)( FMessage.GetMessageType() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_NID(__int64* Value)
{
	*Value = FMessage.GetNID();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_Symbol(BSTR* Value)
{
	WideString Symbol( FMessage.GetSymbol() );
	*Value = Symbol.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_Account(BSTR Value)
{
	AnsiString Account( Value );
	FMessage.SetAccount( Account.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_AE(BSTR Value)
{
	AnsiString AE( Value );
	FMessage.SetAE( AE.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_ClOrdID(BSTR Value)
{
	AnsiString ClOrdID( Value );
	FMessage.SetClOrdID( ClOrdID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_Market(MarketEnum Value)
{
	FMessage.SetMarket( (nsOrderMessageDefine::MarketEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_MessageType(MessageTypeEnum Value)
{
	FMessage.SetMessageType( (nsOrderMessageDefine::MessageTypeEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_NID(__int64 Value)
{
	FMessage.SetNID( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_Symbol(BSTR Value)
{
	AnsiString Symbol( Value );
	FMessage.SetSymbol( Symbol.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_Value(BSTR Name, BSTR* Value)
{
	AnsiString ValName( Name );
	UFC::AnsiString ValValue( "" );
	FMessage.GetValue( ValName.c_str(), ValValue );
	WideString WideValue( ValValue.c_str() );

	*Value = WideValue.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_Value(BSTR Name, BSTR Value)
{
	AnsiString ValName( Name );
	AnsiString ValValue( Value );

	FMessage.SetValue( ValName.c_str(), ValValue.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_AccountFlag(BSTR* Value)
{
	WideString AccountFlag( FMessage.GetAccountFlag() );
	*Value = AccountFlag.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_AccountFlag(BSTR Value)
{
	AnsiString AccountFlag( Value );
	FMessage.SetAccountFlag( AccountFlag.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_GatewayData(BSTR* Value)
{
	WideString GDD( FMessage.GetGatewayData() );
	*Value = GDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_GatewayData(BSTR Value)
{
	AnsiString GDD( Value );

	FMessage.SetGatewayData( GDD.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_Data(BSTR* Value)
{
	WideString UDD( FMessage.GetUserData() );
	*Value = UDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_Data(BSTR Value)
{
	AnsiString UDD( Value );

	FMessage.SetUserData( UDD.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_TradingSessionID(TradingSessionIDEnum* Value)












































{
	*Value = (TradingSessionIDEnum)( FMessage.GetTradingSessionID() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_TradingSessionID(TradingSessionIDEnum Value)












































{
	FMessage.SetTradingSessionID( (nsOrderMessageDefine::TradingSessionIDEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::get_OrderID(BSTR* Value)
{
	WideString OID( FMessage.GetOrderID() );
	*Value = OID.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TQuoteRequestMessageImpl::set_OrderID(BSTR Value)
{
	AnsiString OID( Value );

	FMessage.SetOrderID( OID.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------



