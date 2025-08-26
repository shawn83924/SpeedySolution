// NEWORDERMESSAGEIMPL.H : Declaration of the TNewOrderMessageImpl

#ifndef NewOrderMessageImplH
#define NewOrderMessageImplH

#define ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.H"
#include <TNewOrderMessage.h>

/////////////////////////////////////////////////////////////////////////////
// TNewOrderMessageImpl     Implements INewOrderMessage, default interface of NewOrderMessage
// ThreadingModel : Apartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.NewOrderMessage
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TNewOrderMessageImpl : 
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TNewOrderMessageImpl, &CLSID_NewOrderMessage>,
  public IDispatchImpl<INewOrderMessage, &IID_INewOrderMessage, &LIBID_OrderMessage>
{
private:
	TNewOrderMessage FMessage;

public:
  TNewOrderMessageImpl();

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.NewOrderMessage");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
	TTypedComServerRegistrarT<TNewOrderMessageImpl>
	regObj(GetObjectCLSID(), GetProgID(), GetDescription());
	return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TNewOrderMessageImpl)
  COM_INTERFACE_ENTRY(INewOrderMessage)
  COM_INTERFACE_ENTRY2(IDispatch, INewOrderMessage)
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
// INewOrderMessage
public:

  STDMETHOD(get_EventType(EventTypeEnum* Value));
  STDMETHOD(get_MaturityMonthYear(BSTR* Value));
  STDMETHOD(get_OrderQty(long* Value));
  STDMETHOD(get_OrderType(OrderTypeEnum* Value));
  STDMETHOD(get_Price(double* Value));
  STDMETHOD(get_Side(SideEnum* Value));
  STDMETHOD(get_StopPx(double* Value));
  STDMETHOD(get_StrikePrice(double* Value));
  STDMETHOD(get_TimeInForce(TimeInForceEnum* Value));
  STDMETHOD(set_EventType(EventTypeEnum Value));
  STDMETHOD(set_MaturityMonthYear(BSTR Value));
  STDMETHOD(set_OrderQty(long Value));
  STDMETHOD(set_OrderType(OrderTypeEnum Value));
  STDMETHOD(set_Price(double Value));
  STDMETHOD(set_Side(SideEnum Value));
  STDMETHOD(set_StopPx(double Value));
  STDMETHOD(set_StrikePrice(double Value));
  STDMETHOD(set_TimeInForce(TimeInForceEnum Value));
  STDMETHOD(get_PositionEffect(PositionEffectEnum* Value));
  STDMETHOD(set_PositionEffect(PositionEffectEnum Value));
  STDMETHOD(get_TradingSessionID(TradingSessionIDEnum* Value));
  STDMETHOD(set_TradingSessionID(TradingSessionIDEnum Value));
  STDMETHOD(get_TSEExchangeCode(BSTR* Value));
  STDMETHOD(get_TSEObjectBroker(BSTR* Value));
  STDMETHOD(get_TSEOrderType(BSTR* Value));
  STDMETHOD(get_TSETradeKind(BSTR* Value));
  STDMETHOD(set_TSEExchangeCode(BSTR Value));
  STDMETHOD(set_TSEObjectBroker(BSTR Value));
  STDMETHOD(set_TSEOrderType(BSTR Value));
  STDMETHOD(set_TSETradeKind(BSTR Value));
  STDMETHOD(get_TSEStockSeqNo(long* Value));
  STDMETHOD(set_TSEStockSeqNo(long Value));
  STDMETHOD(get_BrokerID(BSTR* Value));
  STDMETHOD(set_BrokerID(BSTR Value));
  STDMETHOD(get_OrderID(BSTR* Value));
  STDMETHOD(set_OrderID(BSTR Value));
  STDMETHOD(get_PriceBase(PriceBaseEnum* Value));
  STDMETHOD(set_PriceBase(PriceBaseEnum Value));
  STDMETHOD(get_ExchangeCode(BSTR* Value));
  STDMETHOD(set_ExchangeCode(BSTR Value));
  STDMETHOD(get_AENO(BSTR* Value));
  STDMETHOD(get_DayTrade(VARIANT_BOOL* Value));
  STDMETHOD(get_IBNO(BSTR* Value));
  STDMETHOD(set_AENO(BSTR Value));
  STDMETHOD(set_DayTrade(VARIANT_BOOL Value));
  STDMETHOD(set_IBNO(BSTR Value));
  STDMETHOD(get_MarketDataSource(BSTR* Value));
  STDMETHOD(get_OrderSource(OrderSourceEnum* Value));
  STDMETHOD(set_MarketDataSource(BSTR Value));
  STDMETHOD(set_OrderSource(OrderSourceEnum Value));
  STDMETHOD(get_EUREX2TAIFEX(BSTR* Value));
  STDMETHOD(get_Group(long* Value));
  STDMETHOD(set_EUREX2TAIFEX(BSTR Value));
  STDMETHOD(set_Group(long Value));
};

#endif //NewOrderMessageImplH
