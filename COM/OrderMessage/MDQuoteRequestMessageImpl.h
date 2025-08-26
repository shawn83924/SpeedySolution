// Unit1.h : Declaration of the TMDQuoteRequestMessageImpl

#ifndef MDQuoteRequestMessageImplH
#define MDQuoteRequestMessageImplH

#define _ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.h"
#include "TMDQuoteRequestMessage.h"

/////////////////////////////////////////////////////////////////////////////
// TMDQuoteRequestMessageImpl     Implements IMDQuoteRequestMessage, default interface of MDQuoteRequestMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.MDQuoteRequestMessage
// Description    :
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TMDQuoteRequestMessageImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TMDQuoteRequestMessageImpl, &CLSID_MDQuoteRequestMessage>,
  public IDispatchImpl<IMDQuoteRequestMessage, &IID_IMDQuoteRequestMessage, &LIBID_OrderMessage>
{
private:
	TMDQuoteRequestMessage	FMessage;
public:
  TMDQuoteRequestMessageImpl()
  {
  }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.MDQuoteRequestMessage");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TMDQuoteRequestMessageImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TMDQuoteRequestMessageImpl)
  COM_INTERFACE_ENTRY(IMDQuoteRequestMessage)
  COM_INTERFACE_ENTRY2(IDispatch, IMDQuoteRequestMessage)
END_COM_MAP()

// IMDBaseMessage
public:
 
  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_Market(BSTR* Value));

// IMDQuoteRequestMessage
public:
	STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_SecurityID(BSTR* Value));
};

#endif //Unit1H
