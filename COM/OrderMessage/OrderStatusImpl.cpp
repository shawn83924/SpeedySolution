// UNIT1 : Implementation of TOrderStatusRequestMessageImpl (CoClass: OrderStatusRequestMessage, Interface: IOrderStatusRequestMessage)

#include <vcl.h>
#pragma hdrstop
#include "OrderStatusImpl.h"
/////////////////////////////////////////////////////////////////////////////
// IBaseMessage
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_Instance(VARIANT* Value)
{
	VARIANT MessageInstance;
	MessageInstance.vt = VT_BYREF;
	MessageInstance.byref = &FMessage;
	*Value = MessageInstance;

	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_Account(BSTR* Value)
{
    WideString Account( FMessage.GetAccount() );
    *Value = Account.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_AE(BSTR* Value)
{
	WideString AE( FMessage.GetAE() );
    *Value = AE.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_ClOrdID(BSTR* Value)
{
    WideString ClOrdID( FMessage.GetClOrdID() );
    *Value = ClOrdID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_Market(MarketEnum* Value)
{
    *Value = (MarketEnum)( FMessage.GetMarket() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_MessageType(MessageTypeEnum* Value)
{
    *Value = (MessageTypeEnum)( FMessage.GetMessageType() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_NID(long* Value)
{
    *Value = FMessage.GetNID();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_Symbol(BSTR* Value)
{
	WideString Symbol( FMessage.GetSymbol() );
	*Value = Symbol.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_Account(BSTR Value)
{
    AnsiString Account( Value );
    FMessage.SetAccount( Account.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_AE(BSTR Value)
{
    AnsiString AE( Value );
    FMessage.SetAE( AE.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_ClOrdID(BSTR Value)
{
    AnsiString ClOrdID( Value );
    FMessage.SetClOrdID( ClOrdID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_Market(MarketEnum Value)
{
    FMessage.SetMarket( (nsOrderMessageDefine::MarketEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_MessageType(MessageTypeEnum Value)
{
    FMessage.SetMessageType( (nsOrderMessageDefine::MessageTypeEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_NID(long Value)
{
    FMessage.SetNID( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_Symbol(BSTR Value)
{
	AnsiString Symbol( Value );
	FMessage.SetSymbol( Symbol.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::GetValue(BSTR Name, BSTR* Value)
{
	AnsiString CName( Name );
	WideString WValue( FMessage.GetStringValue( CName.c_str()));
	*Value = WValue.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::SetValue(BSTR Name, BSTR Value)
{
	AnsiString CName( Name );
	AnsiString CValue( Value );
	FMessage.SetStringValue(CName.c_str(), CValue.c_str());
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
// TOrderStatusRequestImpl
STDMETHODIMP TOrderStatusRequestMessageImpl::get_Data(BSTR* Value)
{
	WideString Data( FMessage.GetData() );
	*Value = Data.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_OrderID(BSTR* Value)
{
	WideString OrderID( FMessage.GetOrderID() );
	*Value = OrderID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_Price(double* Value)
{
	*Value = FMessage.GetPrice();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_Side(SideEnum* Value)
{
	*Value = (SideEnum)FMessage.GetSide();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_TradingSessionID(
  TradingSessionIDEnum* Value)
{
	*Value = (TradingSessionIDEnum)FMessage.GetTradingSessionID();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_TSEExchangeCode(BSTR* Value)
{
	WideString ExchangeCode( FMessage.GetTSEExchangeCode() );
	*Value = ExchangeCode.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_TSEKeep(BSTR* Value)
{
	WideString Keep( FMessage.GetKeep() );
	*Value = Keep.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_TSEObjectBroker(BSTR* Value)
{
	WideString TSEObjectBroker( FMessage.GetTSEObjectBroker() );
	*Value = TSEObjectBroker.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_TSEOrderType(BSTR* Value)
{
	WideString TSEOrderType( FMessage.GetTSEOrderType() );
	*Value = TSEOrderType.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_TSEStockSeqNo(long* Value)
{
	*Value = FMessage.GetStockSeqNo();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::get_TSETradeKind(BSTR* Value)
{
	WideString TSETradeKind( FMessage.GetTSETradeKind() );
	*Value = TSETradeKind.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_Data(BSTR Value)
{
	AnsiString Data( Value );
	FMessage.SetData( Data.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_OrderID(BSTR Value)
{
	AnsiString OrderID( Value );
	FMessage.SetOrderID( OrderID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_Price(double Value)
{
	FMessage.SetPrice( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_Side(SideEnum Value)
{
	FMessage.SetSide( (nsOrderMessageDefine::SideEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_TradingSessionID(
  TradingSessionIDEnum Value)
{
	FMessage.SetTradingSessionID( (nsOrderMessageDefine::TradingSessionIDEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_TSEExchangeCode(BSTR Value)
{
	AnsiString ExchangeCode( Value );
	FMessage.SetTSEExchangeCode( ExchangeCode.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_TSEKeep(BSTR Value)
{
	AnsiString Keep( Value );
	FMessage.SetKeep( Keep.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_TSEObjectBroker(BSTR Value)
{
	AnsiString TSEObjectBroker( Value );
	FMessage.SetTSEObjectBroker( TSEObjectBroker.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_TSEOrderType(BSTR Value)
{
	AnsiString TSEOrderType( Value );
	FMessage.SetTSEOrderType( TSEOrderType.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_TSEStockSeqNo(long Value)
{
	FMessage.SetStockSeqNo( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderStatusRequestMessageImpl::set_TSETradeKind(BSTR Value)
{
	AnsiString TSETradeKind( Value );
	FMessage.SetTSETradeKind( TSETradeKind.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
