// QUOTEMESSAGEIMPL.H : Declaration of the TQuoteMessageImpl

#ifndef QuoteMessageImplH
#define QuoteMessageImplH

#define ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.H"
#include <TQuoteMessage.h>

/////////////////////////////////////////////////////////////////////////////
// TQuoteMessageImpl     Implements IQuoteMessage, default interface of QuoteMessage
// ThreadingModel : Apartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.QuoteMessage
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TQuoteMessageImpl : 
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TQuoteMessageImpl, &CLSID_QuoteMessage>,
  public IDispatchImpl<IQuoteMessage, &IID_IQuoteMessage, &LIBID_OrderMessage>
{
private:
    TQuoteMessage FMessage;

public:
  TQuoteMessageImpl();

  // Data used when registering Object 
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.QuoteMessage");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TQuoteMessageImpl> 
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TQuoteMessageImpl)
  COM_INTERFACE_ENTRY(IQuoteMessage)
  COM_INTERFACE_ENTRY2(IDispatch, IQuoteMessage)
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
// IQuoteMessage
public:
 
  STDMETHOD(get_AskPrice(double* Value));
  STDMETHOD(get_AskQty(long* Value));
  STDMETHOD(get_BidPrice(double* Value));
  STDMETHOD(get_BidQty(long* Value));
  STDMETHOD(get_TimeInForce(TimeInForceEnum* Value));
  STDMETHOD(set_AskPrice(double Value));
  STDMETHOD(set_AskQty(long Value));
  STDMETHOD(set_BidPrice(double Value));
  STDMETHOD(set_BidQty(long Value));
  STDMETHOD(set_TimeInForce(TimeInForceEnum Value));
  STDMETHOD(get_QuoteID(BSTR* Value));
  STDMETHOD(set_QuoteID(BSTR Value));
  STDMETHOD(get_MarketDataSource(BSTR* Value));
  STDMETHOD(get_OrderSource(OrderSourceEnum* Value));
  STDMETHOD(set_MarketDataSource(BSTR Value));
  STDMETHOD(set_OrderSource(OrderSourceEnum Value));
};

#endif //QuoteMessageImplH
