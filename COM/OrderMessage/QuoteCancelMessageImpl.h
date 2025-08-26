// QUOTECANCELMESSAGEIMPL.H : Declaration of the TQuoteCancelMessageImpl

#ifndef QuoteCancelMessageImplH
#define QuoteCancelMessageImplH

#define ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.H"
#include <TQuoteCancelMessage.h>

/////////////////////////////////////////////////////////////////////////////
// TQuoteCancelMessageImpl     Implements IQuoteCancelMessage, default interface of QuoteCancelMessage
// ThreadingModel : Apartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.QuoteCancelMessage
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TQuoteCancelMessageImpl : 
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TQuoteCancelMessageImpl, &CLSID_QuoteCancelMessage>,
  public IDispatchImpl<IQuoteCancelMessage, &IID_IQuoteCancelMessage, &LIBID_OrderMessage>
{
private:
    TQuoteCancelMessage FMessage;
    
public:
  TQuoteCancelMessageImpl();

  // Data used when registering Object 
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.QuoteCancelMessage");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TQuoteCancelMessageImpl> 
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TQuoteCancelMessageImpl)
  COM_INTERFACE_ENTRY(IQuoteCancelMessage)
  COM_INTERFACE_ENTRY2(IDispatch, IQuoteCancelMessage)
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
// IQuoteCancelMessage
public:

  STDMETHOD(get_QuoteID(BSTR* Value));
  STDMETHOD(set_QuoteID(BSTR Value));
  STDMETHOD(get_BidPrice(double* Value));
  STDMETHOD(set_BidPrice(double Value));
  STDMETHOD(get_AskPrice(double* Value));
  STDMETHOD(set_AskPrice(double Value));
  STDMETHOD(get_MarketDataSource(BSTR* Value));
  STDMETHOD(get_OrderSource(OrderSourceEnum* Value));
  STDMETHOD(set_MarketDataSource(BSTR Value));
  STDMETHOD(set_OrderSource(OrderSourceEnum Value));
};

#endif //QuoteCancelMessageImplH
