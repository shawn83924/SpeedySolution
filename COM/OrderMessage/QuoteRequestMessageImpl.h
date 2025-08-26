// QUOTEREQUESTMESSAGEIMPL.H : Declaration of the TQuoteRequestMessageImpl

#ifndef QuoteRequestMessageImplH
#define QuoteRequestMessageImplH

#define ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.H"
#include <TQuoteRequestMessage.h>

/////////////////////////////////////////////////////////////////////////////
// TQuoteRequestMessageImpl     Implements IQuoteRequestMessage, default interface of QuoteRequestMessage
// ThreadingModel : Apartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.QuoteRequestMessage
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TQuoteRequestMessageImpl : 
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TQuoteRequestMessageImpl, &CLSID_QuoteRequestMessage>,
  public IDispatchImpl<IQuoteRequestMessage, &IID_IQuoteRequestMessage, &LIBID_OrderMessage>
{
private:
    TQuoteRequestMessage FMessage;
    
public:
  TQuoteRequestMessageImpl();

  // Data used when registering Object 
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.QuoteRequestMessage");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
	TTypedComServerRegistrarT<TQuoteRequestMessageImpl>
	regObj(GetObjectCLSID(), GetProgID(), GetDescription());
	return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TQuoteRequestMessageImpl)
  COM_INTERFACE_ENTRY(IQuoteRequestMessage)
  COM_INTERFACE_ENTRY2(IDispatch, IQuoteRequestMessage)
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
// IQuoteRequestMessage
public:

};

#endif //QuoteRequestMessageImplH
