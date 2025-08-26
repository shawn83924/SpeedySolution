// Unit1.h : Declaration of the TOrderStatusRequestMessageImpl

#ifndef OrderStatusRequestMessageImplH
#define OrderStatusRequestMessageImplH

#define _ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.h"
#include <TOrderStatusRequest.h>

/////////////////////////////////////////////////////////////////////////////
// TOrderStatusRequestMessageImpl     Implements IOrderStatusRequestMessage, default interface of OrderStatusRequestMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.OrderStatusRequestMessage
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TOrderStatusRequestMessageImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TOrderStatusRequestMessageImpl, &CLSID_OrderStatusRequestMessage>,
  public IDispatchImpl<IOrderStatusRequestMessage, &IID_IOrderStatusRequestMessage, &LIBID_OrderMessage>
{
private:
	TOrderStatusRequest FMessage;
public:
  TOrderStatusRequestMessageImpl()
  {
  }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.OrderStatusRequestMessage");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
	TTypedComServerRegistrarT<TOrderStatusRequestMessageImpl>
	regObj(GetObjectCLSID(), GetProgID(), GetDescription());
	return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TOrderStatusRequestMessageImpl)
  COM_INTERFACE_ENTRY(IOrderStatusRequestMessage)
  COM_INTERFACE_ENTRY2(IDispatch, IOrderStatusRequestMessage)
END_COM_MAP()
// IBaseMessage
public:
  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_Account(BSTR* Value));
  STDMETHOD(get_AE(BSTR* Value));
  STDMETHOD(get_ClOrdID(BSTR* Value));
  STDMETHOD(get_Market(MarketEnum* Value));
  STDMETHOD(get_MessageType(MessageTypeEnum* Value));
  STDMETHOD(get_NID(__int64* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(set_Account(BSTR Value));
  STDMETHOD(set_AE(BSTR Value));
  STDMETHOD(set_ClOrdID(BSTR Value));
  STDMETHOD(set_Market(MarketEnum Value));
  STDMETHOD(set_MessageType(MessageTypeEnum Value));
  STDMETHOD(set_NID(__int64 Value));
  STDMETHOD(set_Symbol(BSTR Value));
  STDMETHOD(get_AccountFlag(BSTR* Value));
  STDMETHOD(set_AccountFlag(BSTR Value));
  STDMETHOD(get_Value(BSTR Name, BSTR* Value));
  STDMETHOD(set_Value(BSTR Name, BSTR Value));
  STDMETHOD(get_GatewayData(BSTR* Value));
  STDMETHOD(set_GatewayData(BSTR Value));
  STDMETHOD(get_Data(BSTR* Value));
  STDMETHOD(set_Data(BSTR Value));
// IOrderStatusRequestMessage
public:

  STDMETHOD(get_OrderID(BSTR* Value));
  STDMETHOD(set_OrderID(BSTR Value));
  STDMETHOD(get_Price(double* Value));
  STDMETHOD(get_Side(SideEnum* Value));
  STDMETHOD(set_Price(double Value));
  STDMETHOD(set_Side(SideEnum Value));
  STDMETHOD(get_TradingSessionID(TradingSessionIDEnum* Value));
  STDMETHOD(set_TradingSessionID(TradingSessionIDEnum Value));
  STDMETHOD(get_TSEExchangeCode(BSTR* Value));
  STDMETHOD(set_TSEExchangeCode(BSTR Value));
  STDMETHOD(get_TSEKeep(BSTR* Value));
  STDMETHOD(set_TSEKeep(BSTR Value));
  STDMETHOD(get_TSEObjectBroker(BSTR* Value));
  STDMETHOD(set_TSEObjectBroker(BSTR Value));
  STDMETHOD(get_TSEOrderType(BSTR* Value));
  STDMETHOD(set_TSEOrderType(BSTR Value));
  STDMETHOD(get_TSEStockSeqNo(long* Value));
  STDMETHOD(set_TSEStockSeqNo(long Value));
  STDMETHOD(get_TSETradeKind(BSTR* Value));
  STDMETHOD(set_TSETradeKind(BSTR Value));
  STDMETHOD(get_BrokerID(BSTR* Value));
  STDMETHOD(set_BrokerID(BSTR Value));
  STDMETHOD(get_ExchangeCode(BSTR* Value));
  STDMETHOD(set_ExchangeCode(BSTR Value));
};

#endif //Unit1H
